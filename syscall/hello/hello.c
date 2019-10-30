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
	
    /*
struct hlist_head {
    struct hlist_node *first;
};

struct hlist_node {
    struct hlist_node *next, **pprev;
};

void __hlist_del(struct hlist_node* entry1)
{
	struct hlist_node *next = entry1->next;
	struct hlist_node **pprev = entry->pprev;
	**pprev = next;
	if (next)
		next->pprev = pprev;

}
*/

struct mystruct {
	int id;
	char entry[16];
	struct hlist_node node;
};

DEFINE_HASHTABLE(a, 3);
//hash_init(a);
struct mystruct first = {
	.id = key,
	.entry = data,
	.node = 0,
};

//hash_add(a, &first.next, first.key1);
hash_add(a, &first.node, first.id);

int key2 = 1;
struct mystruct* obj;
hash_for_each_possible(a, obj, node, /*next*/ key2){
	printk("data=%s is in bucket %d\n", obj->entry, key2);
}

    return 0;
}


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

