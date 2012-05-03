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
#include "pmacros.h"

#include "fs_config.h"

#include <sys/stat.h>
#include <sys/types.h>

#include <rtems/libio.h>
#include <rtems/dosfs.h>

#include "ramdisk_support.h"
#include "fstest.h"
#include "fstest_support.h"

#define BLOCK_SIZE 512

msdos_format_request_param_t rqdata = {
    OEMName:             "RTEMS",
    VolLabel:            "RTEMSDisk",
    sectors_per_cluster: 0,
    fat_num:             0,
    files_per_root_dir:  0,
    fattype:             MSDOS_FMT_FATANY,
    media:               0,
    quick_format:        FALSE,
    cluster_align:       0,
    info_level:          0
};

void test_initialize_filesystem(void)
{
  int rc=0;
  rc = mkdir (BASE_FOR_TEST,S_IRWXU|S_IRWXG|S_IRWXO);
  rtems_test_assert(rc==0);

  init_ramdisk();

  rc=msdos_format(RAMDISK_PATH,&rqdata);
  rtems_test_assert(rc==0);

  rc=mount(RAMDISK_PATH,
      BASE_FOR_TEST,
      "dosfs",
      RTEMS_FILESYSTEM_READ_WRITE,
      NULL);
  rtems_test_assert(rc==0);
}


void test_shutdown_filesystem(void)
{
  int rc=0;
  rc=unmount(BASE_FOR_TEST) ;
  rtems_test_assert(rc==0);
  del_ramdisk();
}

/* configuration information */

/* drivers */
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

/**
 * Configure base RTEMS resources.
 */
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MAXIMUM_TASKS                     10
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_MAXIMUM_DRIVERS                   10
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS    40
#define CONFIGURE_INIT_TASK_STACK_SIZE (16 * 1024)

#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_FILESYSTEM_DOSFS

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

