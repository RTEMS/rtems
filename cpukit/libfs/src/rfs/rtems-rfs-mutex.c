/**
 * @file
 *
 * @ingroup rtems_rfs
 *
 * @brief RTEMS File System Mutex
 */
/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rfs/rtems-rfs-mutex.h>

int
rtems_rfs_mutex_create (rtems_rfs_mutex* mutex)
{
#if __rtems__
  rtems_recursive_mutex_init(mutex, "RFS");
#endif
  return 0;
}

int
rtems_rfs_mutex_destroy (rtems_rfs_mutex* mutex)
{
#if __rtems__
  rtems_recursive_mutex_destroy(mutex);
#endif
  return 0;
}
