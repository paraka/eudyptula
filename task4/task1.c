#include <linux/module.h>

static int __init task1_init(void)
{
	pr_debug("Hello World!\n");

	return 0;
}

static void __exit task1_exit(void)
{
	pr_debug("Bye Bye World!\n");
}

module_init(task1_init);
module_exit(task1_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Eudyptula Challenge: Task1");
MODULE_AUTHOR("Sergio Paracuellos");
