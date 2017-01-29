#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/errno.h>

#define MY_EUDYPTULA_ID "a8a3ead89815"
#define MY_EUDYPTULA_ID_LEN 13	/* ID length + final NULL */

static DECLARE_WAIT_QUEUE_HEAD(wee_wait);

static struct task_struct *eudyptula_thread;

static int thread_func(void *data)
{
	while (!kthread_should_stop()) {
		if (wait_event_interruptible(wee_wait, 1))
			return -ERESTARTSYS;
	}

	return 0;
}

static ssize_t eudyptula_write(struct file *file, const char __user *buf,
			       size_t count, loff_t *ppos)
{
	char *id_str = MY_EUDYPTULA_ID;
	char buffer_input[MY_EUDYPTULA_ID_LEN];
	ssize_t len;

	len =
	    simple_write_to_buffer(buffer_input, MY_EUDYPTULA_ID_LEN - 1, ppos,
				   buf, count);

	if (len < 0)
		return len;

	if (strncmp(id_str, buffer_input, MY_EUDYPTULA_ID_LEN - 1))
		return -EINVAL;
	else
		return count;
}

static const struct file_operations eudyptula_fops = {
	.owner = THIS_MODULE,
	.write = eudyptula_write,
};

struct miscdevice eudyptula_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "eudyptula",
	.fops = &eudyptula_fops,
	.mode = 0222
};

static int __init task17_init(void)
{
	const char *th_name = "eudyptula";

	eudyptula_thread = kthread_run(thread_func, NULL, th_name);
	if (eudyptula_thread == ERR_PTR(-ENOMEM))
		return -ENOMEM;

	return misc_register(&eudyptula_device);
}

static void __exit task17_exit(void)
{
	kthread_stop(eudyptula_thread);
	misc_deregister(&eudyptula_device);
	pr_debug("Eudyptula misc device deregistered\n");
}

module_init(task17_init);
module_exit(task17_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Eudyptula Challenge: Task17");
MODULE_AUTHOR("Sergio Paracuellos");
