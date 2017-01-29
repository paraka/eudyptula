#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/debugfs.h>

#define MY_EUDYPTULA_ID "a8a3ead89815"
#define MY_EUDYPTULA_ID_LEN 13	/* ID length + final NULL */

static struct dentry *edentry;

static ssize_t id_read(struct file *file, char __user *buf,
		       size_t count, loff_t *ppos)
{
	return simple_read_from_buffer(buf, count, ppos,
				       (char *)MY_EUDYPTULA_ID,
				       MY_EUDYPTULA_ID_LEN);
}

static ssize_t id_write(struct file *file, const char __user *buf,
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

static const struct file_operations id_fops = {
	.owner = THIS_MODULE,
	.write = id_write,
	.read = id_read,
};

static void __exit task8_exit(void)
{
	debugfs_remove_recursive(edentry);
	pr_debug("Eudyptula Task 8 exit\n");
}

static int __init task8_init(void)
{
	edentry = debugfs_create_dir("eudyptula", NULL);
	if (!edentry)
		goto fail;

	if (!debugfs_create_file("id", 0666, edentry, NULL, &id_fops))
		goto fail;

	pr_debug("Eudyptula Task8 init!\n");

	return 0;

 fail:
	pr_alert("Could not create devices");
	return -ENODEV;
}

module_init(task8_init);
module_exit(task8_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Eudyptula Challenge: Task8");
MODULE_AUTHOR("Sergio Paracuellos");
