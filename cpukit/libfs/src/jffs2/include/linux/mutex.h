#ifndef __LINUX_MUTEX_H
#define __LINUX_MUTEX_H

#include <rtems/thread.h>

struct mutex { rtems_mutex r_m; };

#define DEFINE_MUTEX(m) struct mutex m

#define mutex_init(m) rtems_mutex_init(&(m)->r_m, "JFFS2 Mutex");

#define mutex_lock(m) rtems_mutex_lock(&(m)->r_m);

#define mutex_lock_interruptible(m) ({ mutex_lock(m); 0; })

#define mutex_unlock(m) rtems_mutex_unlock(&(m)->r_m);

#define mutex_is_locked(m) 1

#endif
