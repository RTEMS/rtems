#ifndef __LINUX_MUTEX_H
#define __LINUX_MUTEX_H

struct mutex { };

#define DEFINE_MUTEX(m) struct mutex m

static inline void mutex_init(struct mutex *m)
{
	(void) m;
}

static inline void mutex_lock(struct mutex *m)
{
	(void) m;
}

static inline int mutex_lock_interruptible(struct mutex *m)
{
	(void) m;
	return 0;
}

static inline void mutex_unlock(struct mutex *m)
{
	(void) m;
}

#endif
