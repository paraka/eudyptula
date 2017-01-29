#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/textsearch.h>

#define MY_EUDYPTULA_ID "a8a3ead89815"

static struct ts_config *conf;

static unsigned int hook_func(void *priv,
			struct sk_buff *skb,
			const struct nf_hook_state *state)
{
	int pos;
	struct ts_state st = {0};

	pos = textsearch_find_continuous(conf, &st, skb->data, skb->len);
	if (pos != UINT_MAX)
		pr_debug(MY_EUDYPTULA_ID " found at %d\n", pos);

	return NF_ACCEPT;
}

static struct nf_hook_ops eudyptula_hooks = {
	.hook = hook_func,
	.hooknum = NF_INET_LOCAL_IN,
	.pf = NFPROTO_IPV4,
	.priority = NF_IP_PRI_FIRST
};

static int __init task19_init(void)
{
	pr_debug("Registering hook\n");

	conf = textsearch_prepare("kmp", MY_EUDYPTULA_ID, 12,
				  GFP_KERNEL, TS_AUTOLOAD);
	if (IS_ERR(conf))
		return PTR_ERR(conf);

	return nf_register_hook(&eudyptula_hooks);
}

static void __exit task19_exit(void)
{
	pr_debug("Unregistering hook\n");
	textsearch_destroy(conf);
	nf_unregister_hook(&eudyptula_hooks);
}

module_init(task19_init);
module_exit(task19_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sergio Paracuellos");
MODULE_DESCRIPTION("netfilter module task 19");
