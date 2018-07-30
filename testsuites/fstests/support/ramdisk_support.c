/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>

#include <rtems/ramdisk.h>

#include "ramdisk_support.h"
#include "fstest.h"
#include "tmacros.h"

/*
 * Ramdisk information
 */

void
init_ramdisk (void)
{

  int rc = 0;

  rc = ramdisk_register (RAMDISK_BLOCK_SIZE, RAMDISK_BLOCK_COUNT,
                         false, RAMDISK_PATH);
  rtems_test_assert (rc == 0);
}

void
del_ramdisk (void)
{

  int rc = 0;

  rc = unlink (RAMDISK_PATH);
  rtems_test_assert (rc == 0);
}
