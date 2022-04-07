/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"
#include <errno.h>

const char rtems_test_name[] = "PSX 8";

void *POSIX_Init(
  void *argument
)
{
  int    status;
  void  *return_pointer;

  TEST_BEGIN();

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08" PRIxpthread_t "\n", Init_id );

  puts( "Init: pthread_detach - ESRCH (invalid id)" );
  status = pthread_detach( (pthread_t) -1 );
  rtems_test_assert( status == ESRCH );

  /* detach this thread */

  puts( "Init: pthread_detach self" );
  status = pthread_detach( pthread_self() );
  rtems_test_assert( !status );

  /* create thread */

  status = pthread_create( &Task1_id, NULL, Task_1, NULL );
  rtems_test_assert( !status );

  puts( "Init: pthread_join - ESRCH (invalid id)" );
  status = pthread_join( (pthread_t) -1, &return_pointer );
  rtems_test_assert( status == ESRCH );

  puts( "Init: pthread_join - SUCCESSFUL" );
  status = pthread_join( Task1_id, &return_pointer );

  puts( "Init: returned from pthread_join through return" );
  if ( status )
    printf( "status = %d\n", status );
  rtems_test_assert( !status );

  if ( return_pointer == &Task1_id )
    puts( "Init: pthread_join returned correct pointer" );
  else
    printf(
      "Init: pthread_join returned incorrect pointer (%p != %p)\n",
      return_pointer,
      &Task1_id
    );

  puts( "Init: creating two pthreads" );
  status = pthread_create( &Task2_id, NULL, Task_2, NULL );
  rtems_test_assert( !status );

  status = pthread_create( &Task3_id, NULL, Task_3, NULL );
  rtems_test_assert( !status );

  puts( "Init: pthread_join - SUCCESSFUL" );
  status = pthread_join( Task2_id, &return_pointer );
  /* assert is below comment */

  puts( "Init: returned from pthread_join through pthread_exit" );
  if ( status )
    printf( "status = %d\n", status );
  rtems_test_assert( !status );

  if ( return_pointer == &Task2_id )
    puts( "Init: pthread_join returned correct pointer" );
  else
    printf(
      "Init: pthread_join returned incorrect pointer (%p != %p)\n",
      return_pointer,
      &Task2_id
    );

  puts( "Init: exitting" );
  return NULL;
}
