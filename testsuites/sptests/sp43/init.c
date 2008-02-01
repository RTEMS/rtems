/*
 *  Exercise Object Manager Services
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define CONFIGURE_INIT
#include "system.h"

#undef rtems_build_id
#undef rtems_build_name
#undef rtems_object_id_api_maximum
#undef rtems_object_id_api_minimum
#undef rtems_object_id_get_api
#undef rtems_object_id_get_class
#undef rtems_object_id_get_index
#undef rtems_object_id_get_node

rtems_id         main_task;
rtems_name       main_name;

void print_class_info(
  uint32_t                            api,
  uint32_t                            class,
  rtems_object_api_class_information *info
)
{
  printf(
    "%s API %s Information\n"
    "    minimum id  : 0x%08x maximum id: 0x%08x\n"
    "    maximum     :    %7d available : %d\n"
    "    auto_extend : %s\n",
    rtems_object_get_api_name(api),
    rtems_object_get_api_class_name(api, class),
    info->minimum_id,
    info->maximum_id,
    info->maximum,
    info->unallocated,
    ((info->auto_extend) ? "yes" : "no")
  );
}

void change_name(
  const char *newName,
  boolean     printable
)
{
  rtems_status_code    status;
  char                 name[ 5 ];
  char                *ptr;
  const char          *c;

  printf( "rtems_object_set_name - change name of init task to " );
  if ( printable )
    printf( "(%s)\n", newName );
  else {
    printf( "(" );
    for (c=newName ; *c ; ) {
       if (isprint(*ptr)) printf( "%c", *c );
       else               printf( "0x%02x", *c );
       c++;
       if ( *c )
         printf( "-" );
    }
    printf( ")\n" );
  }

  status = rtems_object_set_name( main_task, newName );
  directive_failed( status, "rtems_object_set_name" );

  status = rtems_object_get_classic_name( main_task, &main_name );
  directive_failed( status, "rtems_object_get_classic_name" );
  put_name( main_name, FALSE );
  puts( " - name returned by rtems_object_get_classic_name" );

  ptr = rtems_object_get_name( main_task, 5, name );
  rtems_test_assert(ptr != NULL);
  printf( "rtems_object_get_name returned (%s) for init task\n", ptr );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code                   status;
  rtems_id                            tmpId;
  rtems_name                          tmpName;
  char                                name[5];
  char                               *ptr;
  const char                          newName[5] = "New1";
  char                                tmpNameString[5];
  uint32_t                            part;
  rtems_object_api_class_information  info;

  puts( "\n\n*** TEST 43 ***" );

  main_task = rtems_task_self();

  puts( "rtems_object_get_classic_name - INVALID_ADDRESS" );
  status = rtems_object_get_classic_name( main_task, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_object_get_classic_name #1"
  );

  puts( "rtems_object_get_classic_name - INVALID_ID (bad index)" );
  status = rtems_object_get_classic_name( main_task + 5, &main_name );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_object_get_classic_name #2"
  );

  puts( "rtems_object_get_classic_name - INVALID_ID (unallocated index)" );
  status = rtems_object_get_classic_name( main_task + 1, &main_name );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_object_get_classic_name #4"
  );

  puts( "rtems_object_get_classic_name - INVALID_ID (bad API)" );
  tmpId = rtems_build_id( 0xff, OBJECTS_RTEMS_TASKS, 1, 1 ),
  status = rtems_object_get_classic_name( tmpId, &main_name );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_object_get_classic_name #5"
  );

  status = rtems_object_get_classic_name( main_task, &main_name );
  directive_failed( status, "rtems_object_get_classic_name" );
  put_name( main_name, FALSE );
  puts( " - name returned by rtems_object_get_classic_name" );

  tmpName = rtems_build_name( 'T', 'E', 'M', 'P' );
  put_name( tmpName, FALSE );
  puts( " - rtems_build_name for TEMP" );

  
  /*
   * rtems_object_get_name - cases
   */
  puts( "rtems_object_get_name - bad id for class with instances" );
  ptr = rtems_object_get_name( main_task + 5, 5, name );
  rtems_test_assert(ptr == NULL);

  puts( "rtems_object_get_name - bad id for class without instances" );
  ptr = rtems_object_get_name(
    rtems_build_id( OBJECTS_CLASSIC_API, OBJECTS_RTEMS_BARRIERS, 1, 1 ),
    5,
    name
  );
  rtems_test_assert(ptr == NULL);

  puts( "rtems_object_get_name - bad length" );
  ptr = rtems_object_get_name( main_task, 0, name );
  rtems_test_assert(ptr == NULL);

  puts( "rtems_object_get_name - bad pointer" );
  ptr = rtems_object_get_name( main_task, 5, NULL );
  rtems_test_assert(ptr == NULL);

  ptr = rtems_object_get_name( main_task, 5, name );
  rtems_test_assert(ptr != NULL);
  printf( "rtems_object_get_name returned (%s) for init task\n", ptr );

  /*
   * rtems_object_set_name - errors
   */

  puts( "rtems_object_set_name - INVALID_ID (bad API)" );
  tmpId = rtems_build_id( 0xff, OBJECTS_RTEMS_TASKS, 1, 1 ),
  status = rtems_object_set_name( tmpId, newName );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_object_set_name #1"
  );

  puts( "rtems_object_set_name - INVALID_ID (bad index)" );
  status = rtems_object_set_name( main_task + 10, newName );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_object_set_name #2"
  );

  /*
   * rtems_object_set_name - change name of init task in various ways.
   *
   * This is strange but pushes the SuperCore code to do different things.
   */

  change_name( "New1", TRUE );
  change_name( "Ne1", TRUE );
  change_name( "N1", TRUE );
  change_name( "N", TRUE );
  change_name( "", TRUE );
  tmpNameString[0] = 'N';
  tmpNameString[1] = 0x07;
  tmpNameString[2] = 0x09;
  tmpNameString[3] = '1';
  tmpNameString[4] = '\0';
  change_name( tmpNameString, FALSE );

  /*
   * Exercise id build and extraction routines
   */

  puts( "rtems_build_id - build an id to match init task" );
  tmpId = rtems_build_id( OBJECTS_CLASSIC_API, OBJECTS_RTEMS_TASKS, 1, 1 ),
  assert( tmpId == main_task );
  
  puts( "rtems_object_id_get_api - OK" );
  part = rtems_object_id_get_api( main_task );
  assert( part == OBJECTS_CLASSIC_API );

  puts( "rtems_object_id_get_class - OK" );
  part = rtems_object_id_get_class( main_task );
  assert( part == OBJECTS_RTEMS_TASKS );

  puts( "rtems_object_id_get_node - OK" );
  part = rtems_object_id_get_node( main_task );
  assert( part == 1 );

  puts( "rtems_object_id_get_index - OK" );
  part = rtems_object_id_get_index( main_task );
  assert( part == 1 );

  /*
   * Start another screen and do the API/Class min/max routines
   */
  rtems_test_pause();

  printf( "rtems_object_id_api_minimum returned %d\n",
          rtems_object_id_api_minimum() );
  printf( "rtems_object_id_api_maximum returned %d\n",
          rtems_object_id_api_maximum() );

  printf( "rtems_object_api_minimum_class(0) returned %d\n",
          rtems_object_api_minimum_class(0) );
  printf( "rtems_object_api_maximum_class(0) returned %d\n",
          rtems_object_api_maximum_class(0) );

  printf( "rtems_object_api_minimum_class(0) returned %d\n",
          rtems_object_api_minimum_class(0) );
  printf( "rtems_object_api_maximum_class(0) returned %d\n",
          rtems_object_api_maximum_class(0) );
  printf( "rtems_object_api_minimum_class(255) returned %d\n",
          rtems_object_api_minimum_class(255) );
  printf( "rtems_object_api_maximum_class(255) returned %d\n",
          rtems_object_api_maximum_class(255) );

  printf( "rtems_object_api_minimum_class(OBJECTS_INTERNAL_API) returned %d\n",
          rtems_object_api_minimum_class(OBJECTS_INTERNAL_API) );
  printf( "rtems_object_api_maximum_class(OBJECTS_INTERNAL_API) returned %d\n",
          rtems_object_api_maximum_class(OBJECTS_INTERNAL_API) );

  printf( "rtems_object_api_minimum_class(OBJECTS_CLASSIC_API) returned %d\n",
          rtems_object_api_minimum_class(OBJECTS_CLASSIC_API) );
  printf( "rtems_object_api_maximum_class(OBJECTS_CLASSIC_API) returned %d\n",
          rtems_object_api_maximum_class(OBJECTS_CLASSIC_API) );

  printf( "rtems_object_api_minimum_class(OBJECTS_ITRON_API) returned %d\n",
          rtems_object_api_minimum_class(OBJECTS_ITRON_API) );
  printf( "rtems_object_api_maximum_class(OBJECTS_ITRON_API) returned %d\n",
          rtems_object_api_maximum_class(OBJECTS_ITRON_API) );

  /*
   *  Another screen break for the API and class name tests
   */
  rtems_test_pause();
 
  printf( "rtems_object_get_api_name(0) = %s\n", rtems_object_get_api_name(0) );
  printf( "rtems_object_get_api_name(255) = %s\n",
    rtems_object_get_api_name(255));

  printf( "rtems_object_get_api_name(INTERNAL_API) = %s\n",
     rtems_object_get_api_name(OBJECTS_INTERNAL_API) );
  printf( "rtems_object_get_api_name(CLASSIC_API) = %s\n",
     rtems_object_get_api_name(OBJECTS_CLASSIC_API) );
  printf( "rtems_object_get_api_name(ITRON_API) = %s\n",
     rtems_object_get_api_name(OBJECTS_ITRON_API) );

  printf( "rtems_object_get_api_class_name(0, RTEMS_TASKS) = %s\n", 
    rtems_object_get_api_class_name( 0, OBJECTS_RTEMS_TASKS ) );
  printf( "rtems_object_get_api_class_name(CLASSIC_API, 0) = %s\n", 
    rtems_object_get_api_class_name( OBJECTS_CLASSIC_API, 0 ) );
  printf("rtems_object_get_api_class_name(INTERNAL_API, MUTEXES) = %s\n", 
    rtems_object_get_api_class_name(
       OBJECTS_INTERNAL_API, OBJECTS_INTERNAL_MUTEXES));
  printf("rtems_object_get_api_class_name(CLASSIC_API, RTEMS_BARRIERS) = %s\n", 
    rtems_object_get_api_class_name(
       OBJECTS_CLASSIC_API, OBJECTS_RTEMS_BARRIERS));

  /*
   *  Another screen break for the information
   */

  rtems_test_pause();
  
  puts( "rtems_object_get_class_information - INVALID_ADDRESS" );
  status = rtems_object_get_class_information(
             OBJECTS_INTERNAL_API, OBJECTS_INTERNAL_THREADS, NULL );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_object_get_class_information"
  );

  puts( "rtems_object_get_class_information - INVALID_NUMBER (bad API)" );
  status =
    rtems_object_get_class_information(0, OBJECTS_INTERNAL_THREADS, &info);
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_object_get_class_information (API)"
  );

  puts( "rtems_object_get_class_information - INVALID_NUMBER (bad class)" );
  status =
    rtems_object_get_class_information( OBJECTS_INTERNAL_API, 0, &info );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_object_get_class_information (API)"
  );

  puts( "rtems_object_get_class_information - Classic Tasks - OK" );
  status = rtems_object_get_class_information(
             OBJECTS_CLASSIC_API, OBJECTS_RTEMS_TASKS, &info );
  directive_failed( status, "rtems_object_get_class_information" );
  print_class_info( OBJECTS_CLASSIC_API, OBJECTS_RTEMS_TASKS, &info );

  puts( "rtems_object_get_class_information - Classic Timers - OK" );
  status = rtems_object_get_class_information(
             OBJECTS_CLASSIC_API, OBJECTS_RTEMS_TIMERS, &info );
  directive_failed( status, "rtems_object_get_class_information" );
  print_class_info( OBJECTS_CLASSIC_API, OBJECTS_RTEMS_TIMERS, &info );


  puts( "*** END OF TEST 43 ***" );
  rtems_test_exit( 0 );
}
