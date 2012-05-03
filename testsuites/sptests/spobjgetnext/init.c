/*
 *  Exercise SuperCore Object Get Next
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__ 1
#include "system.h"

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
  Objects_Locations location;
  Objects_Id        id;

  memset( o, 1, sizeof(o) );

  id = start;
  for (i=0 ; i<MAX_SCAN ; i++ ) {
    o[i] = _Objects_Get_next(
      information,
      id,
      &location,
      &id
    );
    if ( !o[i] )
      break;
    if ( location == OBJECTS_ERROR )
      break;
    /* XXX check dispatch level with macros */

    _Thread_Enable_dispatch();

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
  Objects_Locations     location;
  Objects_Id            id;
  Objects_Information  *info;

  puts( "\n\n*** TEST OBJECT GET NEXT ***" );

  info      = &_RTEMS_tasks_Information;
  main_task = rtems_task_self();

  puts( "Init - _Objects_Get_next - NULL object information" );
  o = _Objects_Get_next( NULL, main_task, &location, &id );
  rtems_test_assert( o == NULL );

  puts( "Init - _Objects_Get_next - NULL location" );
  o = _Objects_Get_next( info, main_task, NULL, &id );
  rtems_test_assert( o == NULL );

  puts( "Init - _Objects_Get_next - NULL id" );
  o = _Objects_Get_next( info, main_task, &location, NULL );
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

  puts( "*** END OF TEST OBJECT GET NEXT ***" );
  rtems_test_exit( 0 );
}
