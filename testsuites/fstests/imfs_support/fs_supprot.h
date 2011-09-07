/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include "pmacros.h"
#include "fs_config.h"

void test_initialize_filesystem(void)
{
  int rc=0;
  rc=mkdir(BASE_FOR_TEST,0777);
  rtems_test_assert(rc==0);
}

void test_shutdown_filesystem(void)
{
}
/* configuration information */
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 40

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
