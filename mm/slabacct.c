/*
 * linux/mm/slabacct.c - provide a method to track callers to slab APIs
 *
 * This used to be called kmalloc_acct.c, and only tracked callers
 * to kmalloc.  There are lots of residual references to this previous
 * name (e.g. kma_caller)
 *
 * Written originally by Matt Mackall
 *
 * 15 August 2007 - Switch to track all of slab allocators - Tim Bird
 *     Change some names from k* to cache_*.  Also, fix some bugs
 *     with free(0).  Introduce highwater, dup_frees and zero_frees stats.
 */
#include	<linux/spinlock.h>
#include	<linux/seq_file.h>
#include	<linux/kallsyms.h>


struct kma_caller {
	const void *caller;
	int total, net, slack, allocs, frees, highwater;
};

struct kma_list {
	int callerhash;
	const void *address;
};

#define MAX_CALLER_TABLE 512
/* #define MAX_ALLOC_TRACK 4096 */
#define MAX_ALLOC_TRACK 8192

#define kma_hash(address, size) (((u32)address / (u32)size) % size)

static struct kma_list kma_alloc[MAX_ALLOC_TRACK];
static struct kma_caller kma_caller[MAX_CALLER_TABLE];

static int kma_callers;
static int kma_lost_callers, kma_lost_allocs, kma_unknown_frees;
static int kma_total, kma_net, kma_slack, kma_allocs, kma_frees;
static int kma_highwater, kma_dup_frees, kma_zero_frees;
DEFINE_SPIN_LOCK(kma_lock);

void __cache_alloc_account(const void *caller, const void *addr, int size, int req)
{
	int i, hasha, hashc;
	unsigned long flags;

	spin_lock_irqsave(&kma_lock, flags);

	if (req == -1)
		req = size;

	/* find callers slot */
	hashc = kma_hash(caller, MAX_CALLER_TABLE);
	/* don't allow use of the 0th hash slot */
	if (hashc == 0)
		hashc++;
	for (i = 0; i < MAX_CALLER_TABLE; i++) {
		if (!kma_caller[hashc].caller ||
		    kma_caller[hashc].caller == caller)
			break;
		hashc = (hashc + 1) % MAX_CALLER_TABLE;
		if (hashc == 0)
			hashc++;
	}

	if (!kma_caller[hashc].caller)
		kma_callers++;

	if (i < MAX_CALLER_TABLE) {
		/* update callers stats */
		kma_caller[hashc].caller = caller;
		kma_caller[hashc].total += size;
		kma_caller[hashc].net += size;
		if (kma_caller[hashc].net > kma_caller[hashc].highwater)
			kma_caller[hashc].highwater = kma_caller[hashc].net;

		kma_caller[hashc].slack += size - req;
		kma_caller[hashc].allocs++;

		/* add malloc to list */
		hasha = kma_hash(addr, MAX_ALLOC_TRACK);
		for (i = 0; i < MAX_ALLOC_TRACK; i++) {
			if (kma_alloc[hasha].callerhash == 0 ||
				kma_alloc[hasha].callerhash == -1)
				break;
			hasha = (hasha + 1) % MAX_ALLOC_TRACK;
		}

		if (i < MAX_ALLOC_TRACK) {
			kma_alloc[hasha].callerhash = hashc;
			kma_alloc[hasha].address = addr;
		} else
			kma_lost_allocs++;
	} else {
		kma_lost_callers++;
		kma_lost_allocs++;
	}

	kma_total += size;
	kma_net += size;
	if (kma_net > kma_highwater)
		kma_highwater = kma_net;

	kma_slack += size - req;
	kma_allocs++;

	spin_unlock_irqrestore(&kma_lock, flags);
}

void __cache_free_account(const void *addr, int size)
{
	int i, hasha, hashc;
	unsigned long flags;

	spin_lock_irqsave(&kma_lock, flags);

	kma_frees++;

	if (unlikely(addr == 0)) {
		kma_zero_frees++;
		goto kfree_unlock_exit;
	}

	kma_net -= size;

	/* find allocation record */
	hasha = kma_hash(addr, MAX_ALLOC_TRACK);
	for (i = 0; i < MAX_ALLOC_TRACK ; i++) {
		if (kma_alloc[hasha].address == addr)
			break;
		hasha = (hasha + 1) % MAX_ALLOC_TRACK;
	}

	if (i < MAX_ALLOC_TRACK) {
		hashc = kma_alloc[hasha].callerhash;
		if (hashc != -1) {
			kma_alloc[hasha].callerhash = -1;
			kma_caller[hashc].net -= size;
			kma_caller[hashc].frees++;
		} else {
			/* found address in hash table, but it was
			 * listed as already freed.
			 */
			kma_dup_frees++;
		}
	} else {
		/* couldn't find address in hash table */
		kma_unknown_frees++;
	}

kfree_unlock_exit:
	spin_unlock_irqrestore(&kma_lock, flags);
}


static void *as_start(struct seq_file *m, loff_t *pos)
{
	int i;
	loff_t n = *pos;

	if (!n) {
		seq_printf(m, "total bytes allocated: %8d\n", kma_total);
		seq_printf(m, "slack bytes allocated: %8d\n", kma_slack);
		seq_printf(m, "net bytes allocated:   %8d\n", kma_net);
		seq_printf(m, "net highwater:         %8d\n", kma_highwater);
		seq_printf(m, "number of allocs:      %8d\n", kma_allocs);
		seq_printf(m, "number of frees:       %8d\n", kma_frees);
		seq_printf(m, "number of callers:     %8d\n", kma_callers);
		seq_printf(m, "lost callers:          %8d\n",
			   kma_lost_callers);
		seq_printf(m, "lost allocs:           %8d\n",
			   kma_lost_allocs);
		seq_printf(m, "unknown frees:         %8d\n",
			   kma_unknown_frees);
		seq_printf(m, "duplicate frees:       %8d\n", kma_dup_frees);
		seq_printf(m, "zero frees:            %8d\n", kma_zero_frees);
		seq_puts(m, "\n   total    slack      net alloc/free highwater caller\n");
	}

	for (i = 0; i < MAX_CALLER_TABLE; i++) {
		if (kma_caller[i].caller)
			n--;
		if (n < 0)
			return (void *)(i+1);
	}

	return 0;
}

static void *as_next(struct seq_file *m, void *p, loff_t *pos)
{
	int n = (int)p-1, i;
	++*pos;

	for (i = n + 1; i < MAX_CALLER_TABLE; i++)
		if (kma_caller[i].caller)
			return (void *)(i+1);

	return 0;
}

static void as_stop(struct seq_file *m, void *p)
{
}

static int as_show(struct seq_file *m, void *p)
{
	int n = (int)p-1;
	struct kma_caller *c;
#ifdef CONFIG_KALLSYMS
	char *modname;
	const char *name;
	unsigned long offset = 0, size;
	char namebuf[128];

	c = &kma_caller[n];
	name = kallsyms_lookup((int)c->caller, &size, &offset, &modname,
			       namebuf);
	seq_printf(m, "%8d %8d %8d %5d/%-5d %8d %s+0x%lx\n",
		   c->total, c->slack, c->net, c->allocs, c->frees,
		   c->highwater,
		   name, offset);
#else
	c = &kma_caller[n];
	seq_printf(m, "%8d %8d %8d %5d/%-5d %8d %p\n",
		   c->total, c->slack, c->net, c->allocs, c->frees,
		   c->highwater,
		   c->caller);
#endif

	return 0;
}

const struct seq_operations slab_account_op = {
	.start	= as_start,
	.next	= as_next,
	.stop	= as_stop,
	.show	= as_show,
};
