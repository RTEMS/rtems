#ifndef __LINUX_KERNEL_H__
#define __LINUX_KERNEL_H__

#include <rtems.h>
#include <stdlib.h>
#include <string.h>

#include "kernel-rtems.h"

#define jiffies ((unsigned long)rtems_clock_get_ticks_since_boot())

#define ERR_PTR(err) ((void*)(err))
#define PTR_ERR(err) ((unsigned long)(err))
#define IS_ERR(err) ((unsigned long)err > (unsigned long)-1000L)
static inline void *ERR_CAST(const void *ptr)
{
	return (void *) (uintptr_t) ptr;
}

#define CURRENT_TIME cyg_timestamp()

#define	KERN_EMERG              "<0>"
#define	KERN_ALERT              "<1>"
#define	KERN_CRIT               "<2>"
#define	KERN_ERR                "<3>"
#define	KERN_WARNING            "<4>"
#define	KERN_NOTICE             "<5>"
#define	KERN_INFO               "<6>"
#define	KERN_DEBUG              "<7>"
#define KERN_CONT		""

#ifndef pr_fmt
#define pr_fmt(fmt) fmt
#endif

#define pr_emerg(fmt, ...) \
	printk(KERN_EMERG pr_fmt(fmt), ##__VA_ARGS__)
#define pr_alert(fmt, ...) \
	printk(KERN_ALERT pr_fmt(fmt), ##__VA_ARGS__)
#define pr_crit(fmt, ...) \
	printk(KERN_CRIT pr_fmt(fmt), ##__VA_ARGS__)
#define pr_err(fmt, ...) \
	printk(KERN_ERR pr_fmt(fmt), ##__VA_ARGS__)
#define pr_warning(fmt, ...) \
	printk(KERN_WARNING pr_fmt(fmt), ##__VA_ARGS__)
#define pr_warn pr_warning
#define pr_notice(fmt, ...) \
	printk(KERN_NOTICE pr_fmt(fmt), ##__VA_ARGS__)
#define pr_info(fmt, ...) \
	printk(KERN_INFO pr_fmt(fmt), ##__VA_ARGS__)
#define pr_cont(fmt, ...) \
	printk(KERN_CONT fmt, ##__VA_ARGS__)

#ifdef DEBUG
#define pr_debug(fmt, ...) \
	printk(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)
#else
static inline int no_printk(const char *fmt, ...)
{
	(void) fmt;
	return 0;
}
#define pr_debug(fmt, ...) \
	no_printk(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)
#endif

#define min(x,y) (x<y?x:y)
#define max(x,y) (x<y?y:x)
#define min_t(t, x,y) ((t)x<(t)y?(t)x:(t)y)

#define capable(x) 0

#define kmemdup(x, y, z) do_kmemdup(x, y)

#define from_kuid(x, y) (y)
#define from_kgid(x, y) (y)
#define i_uid_read(x) ((x)->i_uid)
#define i_gid_read(x) ((x)->i_gid)
#define i_uid_write(x, y) do { (x)->i_uid = (y); } while (0)
#define i_gid_write(x, y) do { (x)->i_gid = (y); } while (0)
#define truncate_setsize(x, y) do { (x)->i_size = (y); } while (0)

#endif /* __LINUX_KERNEL_H__ */




