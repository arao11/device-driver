#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/hashtable.h>

//asmlinkage long sys_hello(void) {
//    printk("Hello, world!\n");
//    return 0;
// }

SYSCALL_DEFINE3(hello, int, key, char *, data, int, action) {
//Key is key for hashtable. Data is char variable to store.
//Action is an int, we can do 0 for add and 1 for delete/vice versa
    printk("sys_hello : %d, %s, %d\n",key, data, action);
	
    DECLARE_HASHTABLE(data_hash, 32);
    hash_init(data_hash);

    struct h_struct {
	int key1;
	char *entry;
	struct hlist_node node;
};

struct hlist_head {
    struct hlist_node *first;
};

struct hlist_node {
    struct hlist_node *next, **pprev;
};

//hash_add(data_hash, "entry node", key);

/*
void del_node(int data)
{
    int key = hash_func(data);
    struct h_struct *entry;

    hash_for_each_possible(data_hash, entry, node, key) {
        if (entry->data == data) {
            hash_del(&entry->node);
            return;
        }
    }
}
*/
    return 0;
 }
