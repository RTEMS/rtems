/*
 *  COPYRIGHT (c) 1989-2008.
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
#include "system.h"
#include <rtems.h>
#include "tmacros.h"

void *POSIX_Init(
  void *argument
)
{
  char               name[128];
  char              *ptr;
  rtems_status_code  status;

  puts( "\n\n*** POSIX TEST 14 ***" );

  ptr = rtems_object_get_name( pthread_self(), 128, name );
  printf( "rtems_object_get_name returned (%s) for init thread\n", ptr );

  /* Set my name to Justin */
  puts( "Setting current thread name to Justin" );
  status = rtems_object_set_name( pthread_self(), "Justin" );
  directive_failed( status, "rtems_object_set_name" );

  ptr = rtems_object_get_name( pthread_self(), 128, name );
  printf( "rtems_object_get_name returned (%s) for init thread\n", ptr );

  /* Set my name to Jordan */
  puts( "Setting current thread name to Jordan" );
  status = rtems_object_set_name( pthread_self(), "Jordan" );
  directive_failed( status, "rtems_object_set_name" );

  ptr = rtems_object_get_name( pthread_self(), 128, name );
  printf( "rtems_object_get_name returned (%s) for init thread\n", ptr );

  /* exercise the POSIX path through some routines */
  printf( "rtems_object_api_minimum_class(OBJECTS_POSIX_API) returned %d\n",
          rtems_object_api_minimum_class(OBJECTS_POSIX_API) );
  printf( "rtems_object_api_maximum_class(OBJECTS_POSIX_API) returned %d\n",
          rtems_object_api_maximum_class(OBJECTS_POSIX_API) );

  printf( "rtems_object_get_api_name(POSIX_API) = %s\n",
     rtems_object_get_api_name(OBJECTS_POSIX_API) );

  printf("rtems_object_get_api_class_name(POSIX_API, POSIX_KEYS) = %s\n",
    rtems_object_get_api_class_name( OBJECTS_POSIX_API, OBJECTS_POSIX_KEYS)
  );


  puts( "*** END OF POSIX TEST 14 ***" );
  rtems_test_exit( 0 );

  return NULL;

}
