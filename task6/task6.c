#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/errno.h>

#define MY_EUDYPTULA_ID "a8a3ead89815"
#define MY_EUDYPTULA_ID_LEN 13	/* ID length + final NULL */

static ssize_t eudyptula_read(struct file *file, char __user *buf,
			      size_t count, loff_t *ppos)
{
	return simple_read_from_buffer(buf, count, ppos,
				       (char *)MY_EUDYPTULA_ID,
				       MY_EUDYPTULA_ID_LEN);
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
	.read = eudyptula_read,
};

struct miscdevice eudyptula_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "eudyptula",
	.fops = &eudyptula_fops,
};

static void __exit task6_exit(void)
{
	misc_deregister(&eudyptula_device);
	pr_debug("Eudyptula misc device deregistered\n");
}

static int __init task6_init(void)
{
	int err;

	err = misc_register(&eudyptula_device);
	pr_debug("Eudyptula misc device registered\n");

	return err;
}

module_init(task6_init);
module_exit(task6_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Eudyptula Challenge: Task6");
MODULE_AUTHOR("Sergio Paracuellos");
