#ifndef __LINUX_SPINLOCK_H__
#define __LINUX_SPINLOCK_H__


typedef struct { } spinlock_t;
#define SPIN_LOCK_UNLOCKED (spinlock_t) { }
#define DEFINE_SPINLOCK(x) spinlock_t x = SPIN_LOCK_UNLOCKED

#define spin_lock_init(lock)             \
CYG_MACRO_START;                         \
CYG_UNUSED_PARAM(spinlock_t *, lock);    \
CYG_MACRO_END

#define spin_lock(lock)                  \
CYG_MACRO_START;                         \
CYG_UNUSED_PARAM(spinlock_t *, lock);    \
CYG_MACRO_END

#define spin_unlock(lock)                \
CYG_MACRO_START;                         \
CYG_UNUSED_PARAM(spinlock_t *, lock);    \
CYG_MACRO_END

#define spin_lock_bh(lock)               \
CYG_MACRO_START;                         \
CYG_UNUSED_PARAM(spinlock_t *, lock);    \
CYG_MACRO_END

#define spin_unlock_bh(lock)             \
CYG_MACRO_START;                         \
CYG_UNUSED_PARAM(spinlock_t *, lock);    \
CYG_MACRO_END

#endif /* __LINUX_SPINLOCK_H__ */
