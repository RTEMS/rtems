/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Copyright (C) 2022 embedded brains GmbH & Co. KG
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

static void *async_join_thread( void *arg )
{
  pthread_t *th;
  int        eno;
  int        type;

  th = arg;

  type = PTHREAD_CANCEL_ASYNCHRONOUS;
  eno = pthread_setcanceltype( type, &type );
  rtems_test_assert( eno == 0 );
  rtems_test_assert( type == PTHREAD_CANCEL_DEFERRED );

  (void) pthread_join( *th, NULL );
  rtems_test_assert( 0 );
}

static void test_join_deadlock( void )
{
  pthread_t td;
  pthread_t self;
  int       eno;
  void     *value;

  self = pthread_self();

  eno = pthread_create( &td, NULL, async_join_thread, &self );
  rtems_test_assert( eno == 0 );

  sched_yield();

  eno = pthread_join( td, NULL );
  rtems_test_assert( eno == EDEADLK );

  eno = pthread_cancel( td );
  rtems_test_assert( eno == 0 );

  value = NULL;
  eno = pthread_join( td, &value );
  rtems_test_assert( eno == 0 );
  rtems_test_assert( value == PTHREAD_CANCELED );
}

typedef struct {
  pthread_t protected_join;
  pthread_t deleter;
  rtems_status_code delete_status;
} delete_deadlock_context;

static void *protected_join_thread( void *arg )
{
  delete_deadlock_context *ctx;
  int                      state;
  int                      eno;
  void                    *value;

  ctx = arg;

  state = PTHREAD_CANCEL_DISABLE;
  eno = pthread_setcancelstate( state, &state );
  rtems_test_assert( eno == 0 );
  rtems_test_assert( state == PTHREAD_CANCEL_ENABLE );

  value = NULL;
  eno = pthread_join( ctx->deleter, &value );
  rtems_test_assert( eno == 0 );
  rtems_test_assert( value == &ctx->deleter );

  state = PTHREAD_CANCEL_ENABLE;
  eno = pthread_setcancelstate( state, &state );
  rtems_test_assert( eno == 0 );
  rtems_test_assert( state == PTHREAD_CANCEL_DISABLE );

  pthread_testcancel();
  rtems_test_assert( 0 );
}

static void *deleter_thread( void *arg )
{
  delete_deadlock_context *ctx;

  ctx = arg;
  ctx->delete_status = rtems_task_delete( ctx->protected_join );
  return &ctx->deleter;
}

static void test_delete_deadlock( void )
{
  delete_deadlock_context ctx;
  int                     eno;
  void                   *value;

  ctx.delete_status = RTEMS_NOT_IMPLEMENTED;

  eno = pthread_create(
    &ctx.protected_join,
    NULL,
    protected_join_thread,
    &ctx
  );
  rtems_test_assert( eno == 0 );

  eno = pthread_create( &ctx.deleter, NULL, deleter_thread, &ctx );
  rtems_test_assert( eno == 0 );

  value = NULL;
  eno = pthread_join( ctx.protected_join, &value );
  rtems_test_assert( eno == 0 );
  rtems_test_assert( value == PTHREAD_CANCELED );

  rtems_test_assert( ctx.delete_status == RTEMS_INCORRECT_STATE );
}

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

  test_join_deadlock();
  test_delete_deadlock();

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
