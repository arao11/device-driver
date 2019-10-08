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
	
    

struct h_struct {
	int key1;
	char *entry;
	struct hlist_node node;
};

struct h_struct first = {
	.key1 = key;
	.entry = data;
	.node = 0;
};


DECLARE_HASHTABLE(a, 3);
hash_init(a);

hash_add(a, &first.next, first.data);

int bkt;
struct h_struct * current;
hash_for_each_entry(a, bkt, current, next){
	printk(KERN_INFO "data=%d is in bucket %d\n", current->entry, bkt);
}

/*
struct hlist_head {
    struct hlist_node *first;
};

struct hlist_node {
    struct hlist_node *next, **pprev;
};
*/

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
