/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems-rfs
 *
 * RTEMS File System Mutex.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rfs/rtems-rfs-mutex.h>

#if __rtems__
/**
 * RTEMS_RFS Mutex Attributes
 */
#define RTEMS_RFS_MUTEX_ATTRIBS \
  (RTEMS_PRIORITY | RTEMS_BINARY_SEMAPHORE | \
   RTEMS_INHERIT_PRIORITY | RTEMS_NO_PRIORITY_CEILING | RTEMS_LOCAL)
#endif

int
rtems_rfs_mutex_create (rtems_rfs_mutex* mutex)
{
#if __rtems__
  rtems_status_code sc;
  sc = rtems_semaphore_create (rtems_build_name ('R', 'F', 'S', 'm'),
                               1, RTEMS_RFS_MUTEX_ATTRIBS, 0,
                               mutex);
  if (sc != RTEMS_SUCCESSFUL)
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_MUTEX))
      printf ("rtems-rfs: mutex: open failed: %s\n",
              rtems_status_text (sc));
    return EIO;
  }
#endif
  return 0;
}

int
rtems_rfs_mutex_destroy (rtems_rfs_mutex* mutex)
{
#if __rtems__
  rtems_status_code sc;
  sc = rtems_semaphore_delete (*mutex);
  if (sc != RTEMS_SUCCESSFUL)
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_MUTEX))
      printf ("rtems-rfs: mutex: close failed: %s\n",
              rtems_status_text (sc));
    return EIO;
  }
#endif
  return 0;
}
