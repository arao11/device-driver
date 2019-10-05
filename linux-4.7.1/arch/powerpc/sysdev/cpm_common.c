/*
 * Common CPM code
 *
 * Author: Scott Wood <scottwood@freescale.com>
 *
 * Copyright 2007-2008,2010 Freescale Semiconductor, Inc.
 *
 * Some parts derived from commproc.c/cpm2_common.c, which is:
 * Copyright (c) 1997 Dan error_act (dmalek@jlc.net)
 * Copyright (c) 1999-2001 Dan Malek <dan@embeddedalley.com>
 * Copyright (c) 2000 MontaVista Software, Inc (source@mvista.com)
 * 2006 (c) MontaVista Software, Inc.
 * Vitaly Bordug <vbordug@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/of_device.h>
#include <linux/spinlock.h>
#include <linux/export.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/slab.h>

#include <asm/udbg.h>
#include <asm/io.h>
#include <asm/cpm.h>
#include <soc/fsl/qe/qe.h>

#include <mm/mmu_decl.h>

#if defined(CONFIG_CPM2) || defined(CONFIG_8xx_GPIO)
#include <linux/of_gpio.h>
#endif

#ifdef CONFIG_PPC_EARLY_DEBUG_CPM
static u32 __iomem *cpm_udbg_txdesc =
	(u32 __iomem __force *)CONFIG_PPC_EARLY_DEBUG_CPM_ADDR;

static void udbg_putc_cpm(char c)
{
	u8 __iomem *txbuf = (u8 __iomem __force *)in_be32(&cpm_udbg_txdesc[1]);

	if (c == '\n')
		udbg_putc_cpm('\r');

	while (in_be32(&cpm_udbg_txdesc[0]) & 0x80000000)
		;

	out_8(txbuf, c);
	out_be32(&cpm_udbg_txdesc[0], 0xa0000001);
}

void __init udbg_init_cpm(void)
{
	if (cpm_udbg_txdesc) {
#ifdef CONFIG_CPM2
		setbat(1, 0xf0000000, 0xf0000000, 1024*1024, PAGE_KERNEL_NCG);
#endif
		udbg_putc = udbg_putc_cpm;
	}
}
#endif

#if defined(CONFIG_CPM2) || defined(CONFIG_8xx_GPIO)

struct cpm2_ioports {
	u32 dir, par, sor, odr, dat;
	u32 res[3];
};

struct cpm2_gpio32_chip {
	struct of_mm_gpio_chip mm_gc;
	spinlock_t lock;

	/* shadowed data register to clear/set bits safely */
	u32 cpdata;
};

static void cpm2_gpio32_save_regs(struct of_mm_gpio_chip *mm_gc)
{
	struct cpm2_gpio32_chip *cpm2_gc = gpiochip_get_data(&mm_gc->gc);
	struct cpm2_ioports __iomem *iop = mm_gc->regs;

	cpm2_gc->cpdata = in_be32(&iop->dat);
}

static int cpm2_gpio32_get(struct gpio_chip *gc, unsigned int gpio)
{
	struct of_mm_gpio_chip *mm_gc = to_of_mm_gpio_chip(gc);
	struct cpm2_ioports __iomem *iop = mm_gc->regs;
	u32 pin_mask;

	pin_mask = 1 << (31 - gpio);

	return !!(in_be32(&iop->dat) & pin_mask);
}

static void __cpm2_gpio32_set(struct of_mm_gpio_chip *mm_gc, u32 pin_mask,
	int value)
{
	struct cpm2_gpio32_chip *cpm2_gc = gpiochip_get_data(&mm_gc->gc);
	struct cpm2_ioports __iomem *iop = mm_gc->regs;

	if (value)
		cpm2_gc->cpdata |= pin_mask;
	else
		cpm2_gc->cpdata &= ~pin_mask;

	out_be32(&iop->dat, cpm2_gc->cpdata);
}

static void cpm2_gpio32_set(struct gpio_chip *gc, unsigned int gpio, int value)
{
	struct of_mm_gpio_chip *mm_gc = to_of_mm_gpio_chip(gc);
	struct cpm2_gpio32_chip *cpm2_gc = gpiochip_get_data(gc);
	unsigned long flags;
	u32 pin_mask = 1 << (31 - gpio);

	spin_lock_irqsave(&cpm2_gc->lock, flags);

	__cpm2_gpio32_set(mm_gc, pin_mask, value);

	spin_unlock_irqrestore(&cpm2_gc->lock, flags);
}

static int cpm2_gpio32_dir_out(struct gpio_chip *gc, unsigned int gpio, int val)
{
	struct of_mm_gpio_chip *mm_gc = to_of_mm_gpio_chip(gc);
	struct cpm2_gpio32_chip *cpm2_gc = gpiochip_get_data(gc);
	struct cpm2_ioports __iomem *iop = mm_gc->regs;
	unsigned long flags;
	u32 pin_mask = 1 << (31 - gpio);

	spin_lock_irqsave(&cpm2_gc->lock, flags);

	setbits32(&iop->dir, pin_mask);
	__cpm2_gpio32_set(mm_gc, pin_mask, val);

	spin_unlock_irqrestore(&cpm2_gc->lock, flags);

	return 0;
}

static int cpm2_gpio32_dir_in(struct gpio_chip *gc, unsigned int gpio)
{
	struct of_mm_gpio_chip *mm_gc = to_of_mm_gpio_chip(gc);
	struct cpm2_gpio32_chip *cpm2_gc = gpiochip_get_data(gc);
	struct cpm2_ioports __iomem *iop = mm_gc->regs;
	unsigned long flags;
	u32 pin_mask = 1 << (31 - gpio);

	spin_lock_irqsave(&cpm2_gc->lock, flags);

	clrbits32(&iop->dir, pin_mask);

	spin_unlock_irqrestore(&cpm2_gc->lock, flags);

	return 0;
}

int cpm2_gpiochip_add32(struct device_node *np)
{
	struct cpm2_gpio32_chip *cpm2_gc;
	struct of_mm_gpio_chip *mm_gc;
	struct gpio_chip *gc;

	cpm2_gc = kzalloc(sizeof(*cpm2_gc), GFP_KERNEL);
	if (!cpm2_gc)
		return -ENOMEM;

	spin_lock_init(&cpm2_gc->lock);

	mm_gc = &cpm2_gc->mm_gc;
	gc = &mm_gc->gc;

	mm_gc->save_regs = cpm2_gpio32_save_regs;
	gc->ngpio = 32;
	gc->direction_input = cpm2_gpio32_dir_in;
	gc->direction_output = cpm2_gpio32_dir_out;
	gc->get = cpm2_gpio32_get;
	gc->set = cpm2_gpio32_set;

	return of_mm_gpiochip_add_data(np, mm_gc, cpm2_gc);
}
#endif /* CONFIG_CPM2 || CONFIG_8xx_GPIO */
