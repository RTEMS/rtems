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

#define CONFIGURE_INIT
#include "system.h"
#include <errno.h>
#include "tmacros.h"

const char rtems_test_name[] = "PSXKEY 1";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void Key_destructor(void *key_data);

void Key_destructor(void *key_data)
{
}

rtems_task Init(rtems_task_argument argument)
{
  int    status;

  TEST_BEGIN();

  /* get id of this thread */

  Init_id = rtems_task_self();
  printf( "Init's ID is 0x%08" PRIxpthread_t "\n", Init_id );

  rtems_workspace_greedy_allocate( NULL, 0 );

  puts("Init: pthread_key_create - OK");
  status = pthread_key_create( &Key_id[0], Key_destructor );
  fatal_directive_check_status_only( status, 0, "OK" );

  TEST_END();
  rtems_test_exit( 0 );
}
