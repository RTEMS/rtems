/*
 *  Exercise Object Manager Services
 *
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

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#define CONFIGURE_INIT
#include "system.h"

/* These functions have both macro and function incarnations */
#undef rtems_build_id
extern rtems_id rtems_build_id(int api,int class,int node,int index);
#undef rtems_build_name
extern rtems_name rtems_build_name(char C1,char C2,char C3,char C4);
#undef rtems_object_id_api_maximum
extern int rtems_object_id_api_maximum(void);
#undef rtems_object_id_api_minimum
extern int rtems_object_id_api_minimum(void);
#undef rtems_object_id_get_api
extern int rtems_object_id_get_api(rtems_id id);
#undef rtems_object_id_get_class
extern int rtems_object_id_get_class(rtems_id id);
#undef rtems_object_id_get_index
extern int rtems_object_id_get_index(rtems_id id);
#undef rtems_object_id_get_node
extern int rtems_object_id_get_node(rtems_id id);

void print_class_info(
  int                                 api,
  int                                 class,
  rtems_object_api_class_information *info
);

void change_name(
  rtems_id    id,
  const char *newName,
  bool        printable
);

rtems_id         main_task;
rtems_name       main_name;

void print_class_info(
  int                                 api,
  int                                 class,
  rtems_object_api_class_information *info
)
{
  printf(
    "%s API %s Information\n"
    "    minimum id  : 0x%08" PRIxrtems_id
      " maximum id: 0x%08" PRIxrtems_id "\n"
    "    maximum     :    %7" PRIu32 " available : %" PRIu32 "\n"
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
  rtems_id    id,
  const char *newName,
  bool        printable
)
{
  rtems_status_code    sc;
  char                 name[ 5 ];
  char                *ptr;
  const char          *c;

  printf( "rtems_object_set_name - change name of init task to " );
  if ( printable )
    printf( "(%s)\n", newName );
  else {
    printf( "(" );
    for (c=newName ; *c ; ) {
       if (isprint((unsigned char)*c)) printf( "%c", *c );
       else                            printf( "0x%02x", *c );
       c++;
       if ( *c )
         printf( "-" );
    }
    printf( ")\n" );
  }

  sc = rtems_object_set_name( id, newName );
  directive_failed( sc, "rtems_object_set_name" );

  sc = rtems_object_get_classic_name( id, &main_name );
  directive_failed( sc, "rtems_object_get_classic_name" );
  put_name( main_name, FALSE );
  puts( " - name returned by rtems_object_get_classic_name" );

  ptr = rtems_object_get_name( id, 5, name );
  rtems_test_assert(ptr != NULL);
  printf( "rtems_object_get_name returned (%s) for init task\n", ptr );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code                   sc;
  rtems_id                            tmpId;
  rtems_name                          tmpName;
  char                                name[5];
  char                               *ptr;
  const char                          newName[5] = "New1";
  char                                tmpNameString[5];
  int                                 part;
  rtems_object_api_class_information  info;

  puts( "\n\n*** TEST 43 ***" );

  printf( "RTEMS Version: %s\n", rtems_get_version_string() );

  main_task = rtems_task_self();

  puts( "rtems_object_get_classic_name - INVALID_ADDRESS" );
  sc = rtems_object_get_classic_name( main_task, NULL );
  fatal_directive_status(
    sc,
    RTEMS_INVALID_ADDRESS,
    "rtems_object_get_classic_name #1"
  );

  puts( "rtems_object_get_classic_name - INVALID_ID (bad index)" );
  sc = rtems_object_get_classic_name( main_task + 5, &main_name );
  fatal_directive_status(
    sc,
    RTEMS_INVALID_ID,
    "rtems_object_get_classic_name #2"
  );

  puts( "rtems_object_get_classic_name - INVALID_ID (unallocated index)" );
  sc = rtems_object_get_classic_name( main_task + 1, &main_name );
  fatal_directive_status(
    sc,
    RTEMS_INVALID_ID,
    "rtems_object_get_classic_name #4"
  );

  puts( "rtems_object_get_classic_name - INVALID_ID (bad API)" );
  tmpId = rtems_build_id( 0xff, OBJECTS_RTEMS_TASKS, 1, 1 ),
  sc = rtems_object_get_classic_name( tmpId, &main_name );
  fatal_directive_status(
    sc,
    RTEMS_INVALID_ID,
    "rtems_object_get_classic_name #5"
  );

  sc = rtems_object_get_classic_name( main_task, &main_name );
  directive_failed( sc, "rtems_object_get_classic_name" );
  put_name( main_name, FALSE );
  puts( " - name returned by rtems_object_get_classic_name for Init task id" );

  sc = rtems_object_get_classic_name( RTEMS_SELF, &main_name );
  directive_failed( sc, "rtems_object_get_classic_name" );
  put_name( main_name, FALSE );
  puts( " - name returned by rtems_object_get_classic_name for RTEMS_SELF" );

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
  printf( "rtems_object_get_name returned (%s) for init task id\n", ptr );

  ptr = rtems_object_get_name( RTEMS_SELF, 5, name );
  rtems_test_assert(ptr != NULL);
  printf( "rtems_object_get_name returned (%s) for RTEMS_SELF\n", ptr );

  /*
   * rtems_object_set_name - errors
   */

  puts( "rtems_object_set_name - INVALID_ADDRESS" );
  sc = rtems_object_set_name( tmpId, NULL );
  fatal_directive_status(
    sc,
    RTEMS_INVALID_ADDRESS,
    "rtems_object_set_name INVALID_ADDRESS"
  );

  puts( "rtems_object_set_name - INVALID_ID (bad API)" );
  tmpId = rtems_build_id( 0xff, OBJECTS_RTEMS_TASKS, 1, 1 ),
  sc = rtems_object_set_name( tmpId, newName );
  fatal_directive_status(
    sc,
    RTEMS_INVALID_ID,
    "rtems_object_set_name #1"
  );

  puts( "rtems_object_set_name - INVALID_ID (bad index)" );
  sc = rtems_object_set_name( main_task + 10, newName );
  fatal_directive_status(
    sc,
    RTEMS_INVALID_ID,
    "rtems_object_set_name #2"
  );

  /*
   * rtems_object_set_name - change name of init task in various ways.
   *
   * This is strange but pushes the SuperCore code to do different things.
   */

  change_name( main_task,  "New1", TRUE );
  change_name( main_task, "Ne1", TRUE );
  change_name( main_task, "N1", TRUE );
  change_name( main_task, "N", TRUE );
  change_name( main_task, "", TRUE );
  tmpNameString[0] = 'N';
  tmpNameString[1] = 0x07;
  tmpNameString[2] = 0x09;
  tmpNameString[3] = '1';
  tmpNameString[4] = '\0';
  change_name( main_task, tmpNameString, FALSE );

  /*
   * Change object name using SELF ID
   */

  change_name( RTEMS_SELF,  "SELF", TRUE );

  ptr = rtems_object_get_name( main_task, 5, name );
  rtems_test_assert(ptr != NULL);
  printf( "rtems_object_get_name returned (%s) for init task id\n", ptr );


  /*
   * Exercise id build and extraction routines
   */

  puts( "rtems_build_id - build an id to match init task" );
  tmpId = rtems_build_id( OBJECTS_CLASSIC_API, OBJECTS_RTEMS_TASKS, 1, 1 );
  rtems_test_assert( tmpId == main_task );

  puts( "rtems_object_id_get_api - OK" );
  part = rtems_object_id_get_api( main_task );
  rtems_test_assert( part == OBJECTS_CLASSIC_API );

  puts( "rtems_object_id_get_class - OK" );
  part = rtems_object_id_get_class( main_task );
  rtems_test_assert( part == OBJECTS_RTEMS_TASKS );

  puts( "rtems_object_id_get_node - OK" );
  part = rtems_object_id_get_node( main_task );
  rtems_test_assert( part == 1 );

  puts( "rtems_object_id_get_index - OK" );
  part = rtems_object_id_get_index( main_task );
  rtems_test_assert( part == 1 );

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
  sc = rtems_object_get_class_information(
             OBJECTS_INTERNAL_API, OBJECTS_INTERNAL_THREADS, NULL );
  fatal_directive_status(
    sc,
    RTEMS_INVALID_ADDRESS,
    "rtems_object_get_class_information"
  );

  puts( "rtems_object_get_class_information - INVALID_NUMBER (bad API)" );
  sc =
    rtems_object_get_class_information(0, OBJECTS_INTERNAL_THREADS, &info);
  fatal_directive_status(
    sc,
    RTEMS_INVALID_NUMBER,
    "rtems_object_get_class_information (API)"
  );

  puts( "rtems_object_get_class_information - INVALID_NUMBER (api=0xff)" );
  sc = rtems_object_get_class_information( 0xff, 1, &info );
  fatal_directive_status(
    sc,
    RTEMS_INVALID_NUMBER,
    "rtems_object_get_class_information (api=0xff)"
  );

  puts( "rtems_object_get_class_information - INVALID_NUMBER (class=0)" );
  sc = rtems_object_get_class_information(
    OBJECTS_INTERNAL_API, 0, &info );
  fatal_directive_status(
    sc,
    RTEMS_INVALID_NUMBER,
    "rtems_object_get_class_information (class=0)"
  );

  puts(
    "rtems_object_get_class_information - INVALID_NUMBER (class too high)" );
  sc = rtems_object_get_class_information(
    OBJECTS_INTERNAL_API, 0xff, &info);
  fatal_directive_status(
    sc,
    RTEMS_INVALID_NUMBER,
    "rtems_object_get_class_information (class #2)"
  );

  puts( "rtems_object_get_class_information - Classic Tasks - OK" );
  sc = rtems_object_get_class_information(
             OBJECTS_CLASSIC_API, OBJECTS_RTEMS_TASKS, &info );
  directive_failed( sc, "rtems_object_get_class_information" );
  print_class_info( OBJECTS_CLASSIC_API, OBJECTS_RTEMS_TASKS, &info );

  puts( "rtems_object_get_class_information - Classic Timers - OK" );
  sc = rtems_object_get_class_information(
             OBJECTS_CLASSIC_API, OBJECTS_RTEMS_TIMERS, &info );
  directive_failed( sc, "rtems_object_get_class_information" );
  print_class_info( OBJECTS_CLASSIC_API, OBJECTS_RTEMS_TIMERS, &info );

  /*
   *  Ugly hack to force a weird error.
   *
   *  The weird case is that we need to look up a thread Id where the
   *  thread classes' object information table pointer is NULL.  Probably
   *  impossible to really hit until registration is completely dynamically
   *  configurable.
   */
  {
    rtems_task_priority              old_priority;
    void                            *tmp;
    int                              class;
    int                              api;

    class = OBJECTS_INTERNAL_API;
    api   = OBJECTS_INTERNAL_THREADS;

    puts( "rtems_task_set_priority - use valid Idle thread id" );
    sc = rtems_task_set_priority(
      rtems_build_id( class, api, 1, 1 ),
      RTEMS_CURRENT_PRIORITY,
      &old_priority
    );
    directive_failed( sc, "rtems_task_set_priority" );

    /* destroy internal API pointer */
    puts( "rtems_task_set_priority - clobber internal API info" );
    tmp = _Objects_Information_table[ api ];
    _Objects_Information_table[ api ] = NULL;

    puts( "rtems_task_set_priority - use valid Idle thread id again" );
    sc = rtems_task_set_priority(
      rtems_build_id( class, api, 1, 1 ),
      RTEMS_CURRENT_PRIORITY,
      &old_priority
    );
    fatal_directive_status( sc, RTEMS_INVALID_ID, "rtems_task_set_priority" );

    /* restore pointer */
    puts( "rtems_task_set_priority - restore internal api info" );
    _Objects_Information_table[ api ] = tmp;

    /* destroy internal API thread class pointer */
    puts( "rtems_task_set_priority - clobber internal thread class info" );
    tmp = _Objects_Information_table[ api ][ class ];
    _Objects_Information_table[ api ][ class ] = NULL;

    puts( "rtems_task_set_priority - use valid Idle thread id again" );
    sc = rtems_task_set_priority(
      rtems_build_id( class, api, 1, 1 ),
      RTEMS_CURRENT_PRIORITY,
      &old_priority
    );
    fatal_directive_status( sc, RTEMS_INVALID_ID, "rtems_task_set_priority" );

    puts( "rtems_task_set_priority - use valid Idle thread id again" );
    sc = rtems_task_set_priority(
      rtems_build_id( class, api, 1, 1 ),
      RTEMS_CURRENT_PRIORITY,
      &old_priority
    );
    fatal_directive_status( sc, RTEMS_INVALID_ID, "rtems_task_set_priority" );

    /* restore pointer */
    puts( "rtems_task_set_priority - restore internal thread class info" );
    _Objects_Information_table[ api ][ class ] = tmp;
  }

  /*
   *  Bad Id on an object which disables interrupts as part of translating
   *  the Id into an object pointer.  Semaphore is the only object that
   *  needs this. This is a "good" Id in that is it in range, but bad in
   *  that it has not been allocated so the local_table pointer is NULL.
   */
  puts( "rtems_semaphore_obtain - good but uncreated ID - INVALID_ID - OK" );
  sc = rtems_semaphore_obtain(
    rtems_build_id(
      OBJECTS_CLASSIC_API,
      OBJECTS_RTEMS_SEMAPHORES,
      1,
      rtems_configuration_get_maximum_semaphores()
    ),
    RTEMS_DEFAULT_OPTIONS,
    0
  );
  fatal_directive_status( sc, RTEMS_INVALID_ID, "rtems_semaphore_obtain" );

  /*
   * Invalid POSIX API pointer on get name
   */
  {
    void *tmp;
    tmp = _Objects_Information_table[OBJECTS_POSIX_API];
    _Objects_Information_table[OBJECTS_POSIX_API] = NULL;

    puts( "rtems_object_get_classic_name - bad API pointer - INVALID_ID" );
    sc = rtems_object_get_classic_name(
      rtems_build_id( OBJECTS_POSIX_API, OBJECTS_POSIX_THREADS, 1, 1 ),
      &tmpName
    );
    fatal_directive_status( sc, RTEMS_INVALID_ID, "object_get_classic_name" );

    _Objects_Information_table[OBJECTS_POSIX_API] = tmp;
  }

  puts( "*** END OF TEST 43 ***" );
  rtems_test_exit( 0 );
}
