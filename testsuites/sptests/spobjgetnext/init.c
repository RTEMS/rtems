/*
 *  Exercise SuperCore Object Get Next
 *
 *  COPYRIGHT (c) 1989-2009.
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

#include <rtems/rtems/tasksimpl.h>

const char rtems_test_name[] = "SPOBJGETNEXT";

/* prototypes */
int scan_objects(
  Objects_Information *information,
  Objects_Id           start
);

#define MAX_SCAN 10

int scan_objects(
  Objects_Information *information,
  Objects_Id           start
)
{
  Objects_Control  *o[MAX_SCAN];
  int               i;
  Objects_Id        id;

  memset( o, 1, sizeof(o) );

  id = start;
  for (i=0 ; i<MAX_SCAN ; i++ ) {
    o[i] = _Objects_Get_next(
      id,
      information,
      &id
    );
    if ( !o[i] )
      break;
    /* XXX check dispatch level with macros */

    /* XXX should be able to check that next Id is not one we have seen */
  }
  return i;
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_id              main_task;
  int                   count;
  Objects_Control      *o;
  Objects_Id            id;
  Objects_Information  *info;
  Objects_Maximum       active_count;

  TEST_BEGIN();

  info      = &_RTEMS_tasks_Information.Objects;
  main_task = rtems_task_self();

  puts( "Init - _Objects_Get_next - NULL object information" );
  o = _Objects_Get_next( main_task, NULL, &id );
  rtems_test_assert( o == NULL );
  rtems_test_assert( o == NULL );

  puts( "Init - _Objects_Get_next - NULL id" );
  o = _Objects_Get_next( main_task, info, NULL );
  rtems_test_assert( o == NULL );

  /* XXX push the three NULL error cases */

  /* simple case of only all tasks in the system, starting at initial */
  count = scan_objects( info, OBJECTS_ID_INITIAL_INDEX );
  printf( "%d RTEMS Task%s\n", count, ((count == 1) ? "" : "s") );
  rtems_test_assert( count == 1 );

  /* simple case of only 1 task in the system, starting at that task */
  count = scan_objects( info, main_task );
  printf( "%d RTEMS Task%s\n", count, ((count == 1) ? "" : "s") );
  rtems_test_assert( count == 1 );

  /* XXX create >= 1 task and make sure the counts are correct when */
  /* XXX you start the search at initial, first id, arbitrary id */

  /* XXX try with a manager with no objects created */

  puts( "Init - _Objects_Active_count" );
  _Objects_Allocator_lock();
  active_count = _Objects_Active_count( info );
  _Objects_Allocator_unlock();
  rtems_test_assert( active_count == 1 );

  TEST_END();
  rtems_test_exit( 0 );
}
