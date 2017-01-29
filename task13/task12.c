#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/err.h>
#include <linux/string.h>
#include <linux/slab.h>

#define NAME_LEN 20

struct identity {
	char  name[NAME_LEN];
	int   id;
	bool  busy;
	struct list_head list; /* kernel's list structure */
};

static LIST_HEAD(identity_list);

static struct kmem_cache *identity_cache;

/* Prototypes */
static int identity_create(char *name, int id);
static struct identity *identity_find(int id);
static void identity_destroy(int id);

static int identity_create(char *name, int id)
{
	struct identity *new;

	if (identity_find(id))
		return -EINVAL;

	new = kmem_cache_alloc(identity_cache, GFP_KERNEL);
	if (!new)
		return -ENOMEM;

	strncpy(new->name, name, NAME_LEN);
	new->name[NAME_LEN - 1] = '\0';
	new->id = id;
	new->busy = false;
	list_add(&(new->list), &identity_list);

	pr_debug("identity %d: %s was created succesfully\n", id, name);

	return 0;
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

static void identity_destroy(int id)
{
	struct identity *tmp;

	tmp = identity_find(id);
	if (!tmp)
		return;

	pr_debug("destroying identity %d: %s\n", tmp->id, tmp->name);

	list_del(&(tmp->list));
	kmem_cache_free(identity_cache, tmp);
}

static int __init task12_init(void)
{
	struct identity *tmp;

	identity_cache = kmem_cache_create("identity",
					   sizeof(struct identity),
					   0, 0, NULL);

	if (!identity_cache)
		return -ENOMEM;

	if (identity_create("Alice", 1))
		pr_debug("error creating Alice\n");
	if (identity_create("Bob", 2))
		pr_debug("error creating Bob\n");
	if (identity_create("Dave", 3))
		pr_debug("error creating Dave\n");
	if (identity_create("Gena", 10))
		pr_debug("error creating Gena\n");

	tmp = identity_find(3);
	pr_debug("id 3 = %s\n", tmp->name);

	tmp = identity_find(42);
	if (tmp == NULL)
		pr_debug("id 42 not found\n");

	identity_destroy(2);
	identity_destroy(1);
	identity_destroy(10);
	identity_destroy(42);
	identity_destroy(3);

	return 0;
}

static void __exit task12_exit(void)
{
	if (identity_cache)
		kmem_cache_destroy(identity_cache);

	pr_debug("Eudyptula task12: exiting...\n");
}

module_init(task12_init);
module_exit(task12_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sergio Paracuellos");
MODULE_DESCRIPTION("Eudyptula task 12 : linked lists");
