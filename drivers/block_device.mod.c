#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x28950ef1, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xb5a459dc, __VMLINUX_SYMBOL_STR(unregister_blkdev) },
	{ 0x61762346, __VMLINUX_SYMBOL_STR(blk_cleanup_queue) },
	{ 0xbc28fd2e, __VMLINUX_SYMBOL_STR(add_disk) },
	{ 0x177c57ca, __VMLINUX_SYMBOL_STR(blk_queue_logical_block_size) },
	{ 0x84daafd0, __VMLINUX_SYMBOL_STR(blk_init_queue) },
	{ 0x7959fc3f, __VMLINUX_SYMBOL_STR(alloc_disk) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x71a50dbc, __VMLINUX_SYMBOL_STR(register_blkdev) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "8CBF58CFEE695F97FD7D122");
MODULE_INFO(rhelversion, "7.4");
