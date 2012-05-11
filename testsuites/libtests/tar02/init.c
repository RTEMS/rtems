/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp.h> /* for device driver prototypes */
#include "tmacros.h"
#include <rtems/imfs.h>
#include <rtems/error.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "initial_filesystem_tar.h"

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void test_tarfs_load(void);

#define TARFILE_START initial_filesystem_tar
#define TARFILE_SIZE  initial_filesystem_tar_size

void test_cat(
  char *file,
  int   offset_arg,
  int   length
);

void test_tarfs_load(void)
{
  rtems_status_code sc;

  printf("Loading tarfs image ... ");
  sc = rtems_tarfs_load("/",(void *)TARFILE_START, TARFILE_SIZE);
  if (sc != RTEMS_SUCCESSFUL) {
    printf ("error: untar failed: %s\n", rtems_status_text (sc));
    exit(1);
  }
  printf ("successful\n");

  /******************/
  printf( "========= /home/test_file =========\n" );
  test_cat( "/home/test_file", 0, 0 );
  
  /******************/
  puts("*** Skipping symlink -- NOT CURRENTLY SUPPORTED ***" );
#if 0
  printf( "========= /symlink =========\n" );
  test_cat( "/symlink", 0, 0 );
#endif
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  printf( "\n\n*** TAR02 TEST ***\n" );

  test_tarfs_load();
  IMFS_dump();

  printf( "*** END OF TAR02 TEST ***\n" );
  exit( 0 );
}


/* NOTICE: the clock driver is explicitly disabled */
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS            1
#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 5

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
