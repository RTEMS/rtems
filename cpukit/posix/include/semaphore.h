/**
 * @file
 * 
 * @brief Private Support Information for POSIX Semaphores
 *
 * This file contains definitions that are internal to the RTEMS
 * implementation of POSIX Semaphores.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup POSIX_SEMAPHORE POSIX Semaphores Support
 *
 * @ingroup POSIXAPI
 *
 * @brief Private Support Information for POSIX Semaphores
 */

#include <unistd.h>

#if defined(_POSIX_SEMAPHORES)

#include <sys/time.h>

/*
 *  11.1 Semaphore Characteristics, P1003.1b-1993, p.219
 */
typedef int sem_t;

/*
 *  Bad semaphore Id
 */
#define SEM_FAILED (sem_t *) -1

/*
 *  11.2.1 Initialize an Unnamed Semaphore, P1003.1b-1993, p.219
 */
int sem_init(
  sem_t         *sem,
  int            pshared,
  unsigned int   value
);

/**
 * @brief Destroy an unnamed semaphore.
 *
 * 11.2.2 Destroy an Unnamed Semaphore, P1003.1b-1993, p.220
 */
int sem_destroy(
  sem_t *sem
);

/*
 *  11.2.3 Initialize/Open a Named Semaphore, P1003.1b-1993, p.221
 *
 *  NOTE: Follows open() calling conventions.
 */
sem_t *sem_open(
  const char *name,
  int         oflag,
  ...
);

/**
 * @brief Close a named semaphore.
 *
 * Routine to close a semaphore that has been opened or initialized.
 *
 * 11.2.4 Close a Named Semaphore, P1003.1b-1993, p.224
 */
int sem_close(
  sem_t *sem
);

/**
 * @brief Remove a named semaphore.
 *
 * Unlinks a named semaphore, sem_close must also be called to remove
 * the semaphore.
 *
 * 11.2.5 Remove a Named Semaphore, P1003.1b-1993, p.225
 */
int sem_unlink(
  const char *name
);

/**
 * @brief Lock a semaphore.
 *
 * 11.2.6 Lock a Semaphore, P1003.1b-1993, p.226
 *
 * NOTE: P1003.4b/D8 adds sem_timedwait(), p. 27
 */
int sem_wait(
  sem_t *sem
);

/**
 * @brief Lock a semaphore.
 *
 * @see sem_wait()
 */
int sem_trywait(
  sem_t *sem
);

#if defined(_POSIX_TIMEOUTS)
/**
 * @brief Lock a semaphore.
 */
int sem_timedwait(
  sem_t                 *__restrict sem,
  const struct timespec *__restrict timeout
);
#endif

/**
 * @brief Unlock a semaphore.
 *
 * 11.2.7 Unlock a Semaphore, P1003.1b-1993, p.227
 */
int sem_post(
  sem_t  *sem
);

/**
 * @brief Get the value of a semaphore.
 *
 * 11.2.8 Get the Value of a Semaphore, P1003.1b-1993, p.229
 */
int sem_getvalue(
  sem_t  *__restrict sem,
  int    *__restrict sval
);

#endif   /* _POSIX_SEMAPHORES */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
