#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/mutex.h>

#define NAME_LEN 20

struct identity {
	char  name[NAME_LEN];
	int   id;
	bool  busy;
	struct list_head list; /* kernel's list structure */
};

static LIST_HEAD(identity_list);

static int counter;

/*
 * This mutex ensure exclusive access
 * to identity_list and counter variables
 */
static DEFINE_MUTEX(eudyptula_mutex);

static DECLARE_WAIT_QUEUE_HEAD(wee_wait);

static struct task_struct *eudyptula_thread;

/* Prototypes */
static int identity_create(char *name, int id);
static struct identity *identity_get(void);
static struct identity *identity_find(int id);
static void identity_destroy(void);

static int identity_create(char *name, int id)
{
	struct identity *new;

	if (identity_find(id))
		return -EINVAL;

	new = kmalloc(sizeof(*new), GFP_KERNEL);
	if (!new)
		return -ENOMEM;

	mutex_lock_interruptible(&eudyptula_mutex);
	strncpy(new->name, name, NAME_LEN);
	new->name[NAME_LEN - 1] = '\0';
	new->id = id;
	new->busy = false;
	list_add(&(new->list), &identity_list);
	mutex_unlock(&eudyptula_mutex);

	pr_debug("identity %d: %s was created succesfully\n", id, name);

	return 0;
}

static struct identity *identity_get(void)
{
	struct identity *temp;

	if (list_empty(&identity_list))
		return NULL;

	mutex_lock_interruptible(&eudyptula_mutex);
	temp = list_entry(identity_list.next, struct identity, list);
	list_del(&temp->list);
	mutex_unlock(&eudyptula_mutex);

	return temp;
}

static struct identity *identity_find(int id)
{
	struct identity *tmp;

	list_for_each_entry(tmp, &identity_list, list) {
		if (tmp->id == id)
			return tmp;
	}

	return NULL;
}

static void identity_destroy(void)
{
	struct identity *temp, *next;

	list_for_each_entry_safe(temp, next, &identity_list, list) {
		list_del(&temp->list);
		kfree(temp);
	}
}


static int thread_func(void *data)
{
	struct identity *temp;

	while (!kthread_should_stop()) {
		if (wait_event_interruptible(wee_wait, 1))
			return -ERESTARTSYS;

		temp = identity_get();

		if (temp) {
			msleep_interruptible(5000);
			pr_debug("Got identity: %s %i\n", temp->name, temp->id);
			kfree(temp);
		}
	}

	return 0;
}

static ssize_t eudyptula_write(struct file *file, const char __user *buf,
			       size_t count, loff_t *ppos)
{
	char input[NAME_LEN] = {0};
	ssize_t len = (count >= NAME_LEN) ? NAME_LEN - 1 : count;

	len =
	    simple_write_to_buffer(input, len, ppos,
				   buf, count);
	if (len < 0)
		return -EINVAL;

	if (identity_create(input, counter++))
		return -EINVAL;

	wake_up(&wee_wait);

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

static int __init task18_init(void)
{
	const char *th_name = "eudyptula";

	eudyptula_thread = kthread_run(thread_func, NULL, th_name);
	if (eudyptula_thread == ERR_PTR(-ENOMEM))
		return -ENOMEM;

	return misc_register(&eudyptula_device);
}

static void __exit task18_exit(void)
{
	kthread_stop(eudyptula_thread);
	identity_destroy();
	misc_deregister(&eudyptula_device);
	pr_debug("Eudyptula misc device deregistered\n");
}

module_init(task18_init);
module_exit(task18_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Eudyptula Challenge: Task18");
MODULE_AUTHOR("Sergio Paracuellos");
