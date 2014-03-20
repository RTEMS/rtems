/**
 * @file
 *
 * @brief RTEMS File System Mutex
 *
 * @ingroup rtems_rfs
 *
 * RTEMS File System Mutex.
 *
 * It may be suprising we abstract this for the RTEMS file system but this code
 * is designed to be run on host operating systems.
 */

/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if !defined (_RTEMS_RFS_MUTEX_H_)
#define _RTEMS_RFS_MUTEX_H_

#include <errno.h>

#include <rtems/rfs/rtems-rfs-trace.h>

#if __rtems__
#include <rtems.h>
#include <rtems/error.h>
#endif

/**
 * RFS Mutex type.
 */
#if __rtems__
typedef rtems_id rtems_rfs_mutex;
#else
typedef uint32_t rtems_rfs_mutex; /* place holder */
#endif

/**
 * @brief Create the mutex.
 *
 * @param [in] mutex is pointer to the mutex handle returned to the caller.
 *
 * @retval 0 Successful operation.
 * @retval EIO An error occurred.
 *
 */
int rtems_rfs_mutex_create (rtems_rfs_mutex* mutex);

/**
 * @brief Destroy the mutex.
 *
 * @param[in] mutex Reference to the mutex handle returned to the caller.
 *
 * @retval 0 Successful operation.
 * @retval EIO An error occurred.
 */
int rtems_rfs_mutex_destroy (rtems_rfs_mutex* mutex);

/**
 * @brief Lock the mutex.
 *
 * @param[in] mutex is a pointer to the mutex to lock.
 *
 * @retval 0 Successful operation.
 * @retval EIO An error occurred.
 */
static inline int
rtems_rfs_mutex_lock (rtems_rfs_mutex* mutex)
{
#if __rtems__
  rtems_status_code sc = rtems_semaphore_obtain (*mutex, RTEMS_WAIT, 0);
  if (sc != RTEMS_SUCCESSFUL)
  {
#if RTEMS_RFS_TRACE
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_MUTEX))
      printf ("rtems-rfs: mutex: obtain failed: %s\n",
              rtems_status_text (sc));
#endif
    return EIO;
  }
#endif
  return 0;
}

/**
 * @brief Unlock the mutex.
 *
 * @param[in] mutex is a pointer to the mutex to unlock.
 *
 * @retval 0 Successful operation.
 * @retval EIO An error occurred.
 */
static inline int
rtems_rfs_mutex_unlock (rtems_rfs_mutex* mutex)
{
#if __rtems__
  rtems_status_code sc = rtems_semaphore_release (*mutex);
  if (sc != RTEMS_SUCCESSFUL)
  {
#if RTEMS_RFS_TRACE
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_MUTEX))
      printf ("rtems-rfs: mutex: release failed: %s\n",
              rtems_status_text (sc));
#endif
    return EIO;
  }
#endif
  return 0;
}

#endif
