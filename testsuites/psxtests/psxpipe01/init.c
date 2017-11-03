/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp.h>
#include <pmacros.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <rtems/libcsupport.h>
#include <rtems/malloc.h>

const char rtems_test_name[] = "PSXPIPE 1";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument ignored);

rtems_task Init(
  rtems_task_argument ignored
)
{
  int fd[2] = {0,0};
  int dummy_fd[2] = {0,0};
  int status = 0;
  void *opaque = NULL;

  TEST_BEGIN();

  puts( "Init - attempt to create pipe -- expect EFAULT" );
  status = pipe( NULL );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EFAULT );

  puts( "Init - create pipe -- OK" );
  status = pipe( fd );
  rtems_test_assert( status == 0 );

  status = close( fd[0] );
  status |= close( fd[1] );
  rtems_test_assert( status == 0 );

  puts( "Init - create pipe -- OK" );
  status = pipe( fd );
  rtems_test_assert( status == 0 );

  status = close( fd[0] );
  status |= close( fd[1] );
  rtems_test_assert( status == 0 );

  opaque = rtems_heap_greedy_allocate( NULL, 0 );

  /* case where mkfifo fails */
  puts( "Init - attempt to create pipe -- expect ENOMEM" );
  status = pipe( fd );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENOMEM );

  rtems_heap_greedy_free( opaque );
  
  dummy_fd[0] = open( "/file01", O_RDONLY | O_CREAT, S_IRWXU );
  rtems_test_assert( dummy_fd[0] != -1 );
  dummy_fd[1] = open( "/file02", O_RDONLY | O_CREAT, S_IRWXU );
  rtems_test_assert( dummy_fd[1] != -1 );

  /* case where fifo_open for read => open fails */
  puts( "Init - create pipe -- expect ENFILE" );
  status = pipe( fd );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENFILE );

  status = close( dummy_fd[1] );
  status |= unlink( "/file02" );
  rtems_test_assert( status == 0 );

  /* case where fifo_open for write => open fails */
  puts( "Init - create pipe -- expect ENFILE" );
  status = pipe( fd );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == ENFILE );

  status = close( dummy_fd[0] );
  status |= unlink( "/file01" );
  rtems_test_assert( status == 0 );

  TEST_END();
  rtems_test_exit( 0 );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 5

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_MAXIMUM_PIPES 2

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
