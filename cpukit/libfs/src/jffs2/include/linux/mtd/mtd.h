#ifndef __LINUX_MTD_MTD_H__
#define __LINUX_MTD_MTD_H__

#include <linux/slab.h>
#include <errno.h>

#define MTD_FAIL_ADDR_UNKNOWN -1LL

static inline int do_mtd_point(size_t *retlen, void **ebuf)
{
	*retlen = 0;
	*ebuf = NULL;

	return -EOPNOTSUPP;
}

#define mtd_point(a, b, c, d, e, f) do_mtd_point(d, e)

#define mtd_unpoint(a, b, c) do { } while (0)

#define mtd_kmalloc_up_to(a, b) kmalloc(*(b), GFP_KERNEL)

#endif /* __LINUX_MTD_MTD_H__ */
