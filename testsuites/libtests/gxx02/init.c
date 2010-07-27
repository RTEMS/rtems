/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <tmacros.h>
#include "test_support.h"
#include "gxx_wrappers.h"
#include <rtems/score/heap.h>

rtems_task Init(
  rtems_task_argument argument
)
{

  void                   *alloc_ptr = (void *)0;
  Heap_Information_block  Info;
  __gthread_key_t         key = 0;
  int                     status = 0;
  void                   *retptr;

  puts( "\n\n*** TEST GXX 02 ***" );

  puts( "Init - allocating most of workspace memory" );
  status = rtems_workspace_get_information( &Info );
  rtems_test_assert( status == true );
  status = rtems_workspace_allocate( Info.Free.largest - 4, &alloc_ptr );
  rtems_test_assert( status == true );

  puts( "rtems_gxx_getspecific() - panic and exit" );

  puts( "*** END OF TEST GXX 02 ***" );

  /* The below call will not return and hence the END OF above */
  retptr = rtems_gxx_getspecific( key );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS        1
#define CONFIGURE_MAXIMUM_SEMAPHORES   2
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
