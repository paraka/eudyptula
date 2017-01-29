#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/sysfs.h>
#include <linux/jiffies.h>
#include <linux/mutex.h>
#include <linux/string.h>

#define MY_EUDYPTULA_ID "a8a3ead89815"
#define MY_EUDYPTULA_ID_LEN 13	/* ID length + final NULL */

static DEFINE_MUTEX(foo_mutex);
static char foo_data[PAGE_SIZE];
static int foo_len;

static struct kobject *ekobject;

static ssize_t id_show(struct kobject *kobj, struct kobj_attribute *attr,
		       char *buf)
{
	strncpy(buf, (char *)MY_EUDYPTULA_ID, MY_EUDYPTULA_ID_LEN);
	return MY_EUDYPTULA_ID_LEN;
}

static ssize_t id_store(struct kobject *kobj, struct kobj_attribute *attr,
			const char *buf, size_t count)
{
	if ((count != MY_EUDYPTULA_ID_LEN)
	    || (strncmp((char *)MY_EUDYPTULA_ID, buf, MY_EUDYPTULA_ID_LEN - 1)))
		return -EINVAL;
	else
		return MY_EUDYPTULA_ID_LEN;
}

static ssize_t jiffies_show(struct kobject *kobj, struct kobj_attribute *attr,
			    char *buf)
{
	return sprintf(buf, "%lu\n", jiffies);
}

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	mutex_lock(&foo_mutex);
	strncpy(buf, foo_data, foo_len);
	mutex_unlock(&foo_mutex);

	return foo_len;
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	if (count > PAGE_SIZE)
		return -EINVAL;

	mutex_lock(&foo_mutex);
	strncpy(foo_data, buf, count);
	foo_len = count;
	mutex_unlock(&foo_mutex);

	return count;
}

static struct kobj_attribute id_attribute = __ATTR_RW(id);
static struct kobj_attribute jiffies_attribute = __ATTR_RO(jiffies);
static struct kobj_attribute foo_attribute = __ATTR_RW(foo);

static struct attribute *attrs[] = {
	&foo_attribute.attr,
	&id_attribute.attr,
	&jiffies_attribute.attr,
	NULL,
};

static struct attribute_group eudyptula_group = {
	.attrs = attrs,
};

static void __exit task9_exit(void)
{
	kobject_put(ekobject);
	pr_debug("Eudyptula Task 9 exit\n");
}

static int __init task9_init(void)
{
	int ret;

	ekobject = kobject_create_and_add("eudyptula", kernel_kobj);
	if (!ekobject)
		return -ENOMEM;

	ret = sysfs_create_group(ekobject, &eudyptula_group);
	if (ret)
		kobject_put(ekobject);

	pr_debug("Eudyptula Task 9 init\n");

	return ret;
}

module_init(task9_init);
module_exit(task9_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Eudyptula Challenge: Task9");
MODULE_AUTHOR("Sergio Paracuellos");
