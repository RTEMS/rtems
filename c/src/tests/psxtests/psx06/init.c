/* 
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#define CONFIGURE_INIT
#include "system.h"
#include <errno.h>

void Key_destructor(
   void *key_data
)
{
  Destructor_invoked++;

  /*
   *  This checks out that we only run the destructor multiple times
   *  when the key data is non null.
   */

  if ( Destructor_invoked == 5 )
     (void) pthread_setspecific( Key_id, NULL );
}

void *POSIX_Init(
  void *argument
)
{
  int               status;
  unsigned int      remaining;
  rtems_unsigned32 *key_data;

  puts( "\n\n*** POSIX TEST 6 ***" );

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08x\n", Init_id );
  
  /* create a couple of threads */

  status = pthread_create( &Task_id, NULL, Task_1, NULL );
  assert( !status );

  status = pthread_create( &Task2_id, NULL, Task_2, NULL );
  assert( !status );

  /* create a key */

  empty_line();

  Destructor_invoked = 0;
  puts( "Init: pthread_key_create - SUCCESSFUL" );
  status = pthread_key_create( &Key_id, Key_destructor );
  if ( status )
    printf( "status = %d\n", status );
  assert( !status );

  printf( "Destructor invoked %d times\n", Destructor_invoked );

  puts( "Init: pthread_key_create - EAGAIN (too many keys)" );
  status = pthread_key_create( &Key_id, Key_destructor );
  assert( status == EAGAIN );

  puts( "Init: pthread_setspecific - EINVAL (invalid key)" );
  status = pthread_setspecific( -1, &Data_array[ 0 ] );
  assert( status == EINVAL );

  puts( "Init: pthread_getspecific - EINVAL (invalid key)" );
  key_data = pthread_getspecific( -1 );
  assert( !key_data );

  puts( "Init: pthread_key_delete - EINVAL (invalid key)" );
  status = pthread_key_delete( -1 );
  assert( status == EINVAL );

  printf( "Init: Setting the key to %d\n", 0 );
  status = pthread_setspecific( Key_id, &Data_array[ 0 ] );
  if ( status )
    printf( "status = %d\n", status );
  assert( !status );

     /* switch to task 1 */

  key_data = pthread_getspecific( Key_id );
  printf( "Init: Got the key value of %ld\n",
          (unsigned long) ((rtems_unsigned32 *)key_data - Data_array) );

  remaining = sleep( 3 );
  if ( remaining )
     printf( "seconds remaining = %d\n", remaining );
  assert( !remaining );

     /* switch to task 1 */

  /* delete the key */

  puts( "Init: pthread_key_delete - SUCCESSFUL" );
  status = pthread_key_delete( Key_id );
  if ( status )
    printf( "status = %d\n", status );
  assert( !status );

  printf( "Destructor invoked %d times\n", Destructor_invoked );

  puts( "*** END OF POSIX TEST 6 ***" );
  exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
