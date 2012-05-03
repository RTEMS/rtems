/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/ramdisk.h>
#include <rtems/blkdev.h>
#include <rtems/libio.h>

#include "ramdisk_support.h"
#include "fstest.h"
#include "pmacros.h"

/*
 * Ramdisk information
 */

dev_t dev = 0;

void
init_ramdisk (void)
{

  int rc = 0;
  rc = rtems_disk_io_initialize ();
  rtems_test_assert (rc == 0);
  rc = ramdisk_register (RAMDISK_BLOCK_SIZE, RAMDISK_BLOCK_COUNT,
                         false, RAMDISK_PATH, &dev);
  rtems_test_assert (rc == 0);

}

void
del_ramdisk (void)
{

  int rc = 0;
  rtems_device_major_number major = 0;
  rtems_device_minor_number minor = 0;

  rc = rtems_disk_delete (dev);
  rtems_test_assert (rc == 0);

  rtems_filesystem_split_dev_t (dev, major, minor);

  rtems_test_assert (major >= 0);
  rtems_test_assert (minor >= 0);

  rc = rtems_io_unregister_driver (major);
  rtems_test_assert (rc == 0);
  rc = rtems_disk_io_done ();
  rtems_test_assert (rc == 0);

}
