// SPDX-License-Identifier: GPL-2-Clause
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/ktime.h>
#include <linux/list.h>
#include <linux/slab.h>

MODULE_AUTHOR("Chernoivanenko Viktoriia");
MODULE_DESCRIPTION("Hello, world message");
MODULE_LICENSE("Dual BSD/GPL");

static uint howmany = 1;
module_param(howmany, uint, 0444);
MODULE_PARM_DESC(howmany, "how many times the greeting will be printed");

struct message_time {
	struct list_head tlist;
	ktime_t time;
};

static LIST_HEAD(hello_time_list);

static int __init hello_init(void)
{
	if (howmany == 0 || (howmany >= 5 && howmany <= 10)) {
		printk(KERN_WARNING "Warning: howmany is equal to %d\n", howmany);
	}

	BUG_ON(howmany > 10);


	int i = 0;

	for (i = 0; i < howmany; i++) {
		struct message_time *tst = kmalloc(sizeof(*tst), GFP_KERNEL);

		if (i == howmany - 1) tst = NULL;

		if (tst != NULL) {
			tst->time = ktime_get();
			list_add_tail(&tst->tlist, &hello_time_list);
			printk(KERN_INFO "Hello, world!\n");
		}
		else {
			struct message_time *cur, *tmp;
        		list_for_each_entry_safe(cur, tmp, &hello_time_list, tlist) {
	                list_del(&cur->tlist);
        		kfree(cur);

			BUG();
        		}
		}
	}
	return 0;
}

static void __exit hello_exit(void)
{
	int i = 0;
	struct message_time *cur, *tmp;

	list_for_each_entry_safe(cur, tmp, &hello_time_list, tlist) {
		long long ns_time = ktime_to_ns(cur->time);

		printk(KERN_INFO "[%d] kernel time = %lld\n", i++, ns_time);
		list_del(&cur->tlist);
		kfree(cur);
	}
	printk(KERN_INFO "Bye, helloworld is not here anymore.\n");
}

module_init(hello_init);
module_exit(hello_exit);
