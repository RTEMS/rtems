#ifndef __LINUX_SPINLOCK_H__
#define __LINUX_SPINLOCK_H__

#include <rtems/thread.h>

typedef struct { rtems_mutex r_m; } spinlock_t;

#define DEFINE_SPINLOCK(x) spinlock_t x

#define spin_lock_init(x) rtems_mutex_init(&(x)->r_m, "JFFS2 Spinlock");

#define spin_lock(x) rtems_mutex_lock(&(x)->r_m);

#define spin_unlock(x) rtems_mutex_unlock(&(x)->r_m);

#endif /* __LINUX_SPINLOCK_H__ */
