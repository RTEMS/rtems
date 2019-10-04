#ifndef __LINUX_RWSEM_H__
#define __LINUX_RWSEM_H__

#include <pthread.h>

struct rw_semaphore {
  pthread_rwlock_t lock;
};

#define init_rwsem(rwsem) pthread_rwlock_init(&(rwsem)->lock, NULL)

#define down_read(rwsem) pthread_rwlock_rdlock(&(rwsem)->lock)

#define down_write(rwsem) pthread_rwlock_wrlock(&(rwsem)->lock)

#define up_read(rwsem) pthread_rwlock_unlock(&(rwsem)->lock)

#define up_write(rwsem) pthread_rwlock_unlock(&(rwsem)->lock)

#endif /* __LINUX_RWSEM_H__ */
