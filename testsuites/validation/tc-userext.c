/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsUserextValUserext
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/atomic.h>

#include "tc-userext.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsUserextValUserext spec:/rtems/userext/val/userext
 *
 * @ingroup TestsuitesUserext
 *
 * @brief Tests the thread user extensions.
 *
 * This test case performs the following actions:
 *
 * - Create five dynamic extensions.  Switch to a started thread.  Delete three
 *   dynamic extension during the thread begin invocation.  Clean up the used
 *   resources.
 *
 *   - Check that the thread switch extensions were invoked in the right order
 *     before the thread begin extensions.
 *
 *   - Check that the thread begin extensions were invoked in the right order.
 *
 *   - Check that the other extensions were not invoked.
 *
 *   - Check that the thread begin extension of the extension set deleted
 *     before its turn in the invocation was not invoked.
 *
 * - Create five dynamic extensions.  Create a thread.  Delete three dynamic
 *   extension during the thread create invocation.  Clean up the used
 *   resources.
 *
 *   - Check that the thread create extensions were invoked in the right order.
 *
 *   - Check that the thread create extensions were invoked under protection of
 *     the allocator mutex.
 *
 *   - Check that the other extensions were not invoked.
 *
 *   - Check that the thread create extension of the extension set deleted
 *     before its turn in the invocation was not invoked.
 *
 * - Create five dynamic extensions.  Delete a thread.  Delete three dynamic
 *   extension during the thread delete invocation.  Clean up the used
 *   resources.
 *
 *   - Check that the thread delete extensions were invoked in the right order.
 *
 *   - Check that the thread delete extensions were invoked under protection of
 *     the allocator mutex.
 *
 *   - Check that the other extensions were not invoked.
 *
 *   - Check that the thread delete extension of the extension set deleted
 *     before its turn in the invocation was not invoked.
 *
 * - Create five dynamic extensions.  Return from a thread entry.  Delete three
 *   dynamic extension during the thread exitted invocation.  Clean up the used
 *   resources.
 *
 *   - Check that the thread exitted extensions were invoked in the right
 *     order.
 *
 *   - Check that the other extensions were not invoked.
 *
 *   - Check that the thread exitted extension of the extension set deleted
 *     before its turn in the invocation was not invoked.
 *
 * - Create five dynamic extensions.  Restart a thread.  Delete three dynamic
 *   extension during the thread restart invocation.  Clean up the used
 *   resources.
 *
 *   - Check that the thread restart extensions were invoked in the right
 *     order.
 *
 *   - Check that the other extensions were not invoked.
 *
 *   - Check that the thread restart extension of the extension set deleted
 *     before its turn in the invocation was not invoked.
 *
 * - Create five dynamic extensions.  Start a thread.  Delete three dynamic
 *   extension during the thread start invocation.  Clean up the used
 *   resources.
 *
 *   - Check that the thread start extensions were invoked in the right order.
 *
 *   - Check that the other extensions were not invoked.
 *
 *   - Check that the thread start extension of the extension set deleted
 *     before its turn in the invocation was not invoked.
 *
 * - Create five dynamic extensions.  Terminate a thread.  Delete three dynamic
 *   extension during the thread terminate invocation.  Clean up the used
 *   resources.
 *
 *   - Check that the thread terminate extensions were invoked in the right
 *     order.
 *
 *   - Check that the other extensions were not invoked.
 *
 *   - Check that the thread terminate extension of the extension set deleted
 *     before its turn in the invocation was not invoked.
 *
 * - Create five dynamic extensions.  Let an idle thread return from its entry.
 *   Delete three dynamic extension during the thread exitted invocation.
 *   Clean up the used resources.
 *
 *   - Check that the thread exitted extensions were invoked in the right
 *     order.
 *
 *   - Check that the other extensions were not invoked.
 *
 *   - Check that the thread exitted extension of the extension set deleted
 *     before its turn in the invocation was not invoked.
 *
 * @{
 */

typedef struct {
  unsigned int counter;
  rtems_tcb   *executing;
  rtems_tcb   *thread;
} ExtensionEvent;

typedef enum {
  THREAD_BEGIN,
  THREAD_CREATE,
  THREAD_DELETE,
  THREAD_EXITTED,
  THREAD_RESTART,
  THREAD_START,
  THREAD_SWITCH,
  THREAD_TERMINATE,
  EXTENSION_KIND_COUNT
} ExtensionKind;

static rtems_id extension_ids[ 7 ];

static Atomic_Uint extension_counter[ RTEMS_ARRAY_SIZE( extension_ids ) ]
  [ EXTENSION_KIND_COUNT ];

static ExtensionEvent extension_events[ RTEMS_ARRAY_SIZE( extension_ids ) ]
  [ EXTENSION_KIND_COUNT ][ 3 ];

static Atomic_Uint global_counter;

static ExtensionKind extension_under_test = EXTENSION_KIND_COUNT;

static uint32_t thread_create_allocator_owner_count;

static uint32_t thread_delete_allocator_owner_count;

static void StopTestCase( void )
{
  ExtensionKind     kind;
  rtems_status_code sc;

  kind = extension_under_test;
  extension_under_test = EXTENSION_KIND_COUNT;

  sc = rtems_extension_delete( extension_ids[ 2 ] );
  T_rsc_success( sc );

  if ( kind == THREAD_SWITCH ) {
    sc = rtems_extension_delete( extension_ids[ 3 ] );
    T_rsc_success( sc );

    sc = rtems_extension_delete( extension_ids[ 4 ] );
    T_rsc_success( sc );

    sc = rtems_extension_delete( extension_ids[ 5 ] );
    T_rsc_success( sc );
  }

  sc = rtems_extension_delete( extension_ids[ 6 ] );
  T_rsc_success( sc );
}

static void Extension(
  size_t        index,
  ExtensionKind kind,
  rtems_tcb    *executing,
  rtems_tcb    *thread
)
{
  unsigned int      gc;
  unsigned int      c;
  rtems_status_code sc;

  if ( extension_under_test == EXTENSION_KIND_COUNT ) {
    return;
  }

  if ( kind == THREAD_CREATE && _RTEMS_Allocator_is_owner() ) {
    ++thread_create_allocator_owner_count;
  }

  if ( kind == THREAD_DELETE && _RTEMS_Allocator_is_owner() ) {
    ++thread_delete_allocator_owner_count;
  }

  gc = _Atomic_Fetch_add_uint( &global_counter, 1, ATOMIC_ORDER_RELAXED ) + 1;
  c = _Atomic_Fetch_add_uint(
    &extension_counter[ index ][ kind ],
    1,
    ATOMIC_ORDER_RELAXED
  );

  if ( c < RTEMS_ARRAY_SIZE( extension_events[ index ][ kind ] ) ) {
    extension_events[ index ][ kind ][ c ].counter = gc;
    extension_events[ index ][ kind ][ c ].executing = executing;
    extension_events[ index ][ kind ][ c ].thread = thread;
  }

  if ( kind == THREAD_SWITCH ) {
    /* Extension set deletion is not allowed in thread switch extensions */
    return;
  }

  if ( kind != extension_under_test ) {
    return;
  }

  if ( kind == THREAD_DELETE || kind == THREAD_TERMINATE ) {
    if ( index == 6 ) {
      sc = rtems_extension_delete( extension_ids[ 5 ] );
      T_rsc_success( sc );
    } else if ( index == 3 ) {
      sc = rtems_extension_delete( extension_ids[ 3 ] );
      T_rsc_success( sc );
    } else if ( index == 2 ) {
      sc = rtems_extension_delete( extension_ids[ 4 ] );
      T_rsc_success( sc );
    }
  } else {
    if ( index == 2 ) {
      sc = rtems_extension_delete( extension_ids[ 3 ] );
      T_rsc_success( sc );
    } else if ( index == 5 ) {
      sc = rtems_extension_delete( extension_ids[ 5 ] );
      T_rsc_success( sc );
    } else if ( index == 6 ) {
      sc = rtems_extension_delete( extension_ids[ 4 ] );
      T_rsc_success( sc );
    }
  }

  if ( index == 6 && ( kind == THREAD_EXITTED || kind == THREAD_RESTART ) ) {
    StopTestCase();

    if ( GetExecuting()->is_idle ) {
      SetSelfPriority( RTEMS_MAXIMUM_PRIORITY );
      _CPU_Thread_Idle_body( 0 );
    } else {
      rtems_task_exit();
    }
  }

  if ( index == 0 && kind == THREAD_TERMINATE ) {
    StopTestCase();
  }
}

#define DEFINE_EXTENSIONS( index, linkage ) \
  linkage void ThreadBeginExtension##index( rtems_tcb *executing ) \
  { \
    Extension( index, THREAD_BEGIN, executing, NULL ); \
  } \
  linkage bool ThreadCreateExtension##index( \
    rtems_tcb *executing, \
    rtems_tcb *created \
  ) \
  { \
    Extension( index, THREAD_CREATE, executing, created ); \
    return true; \
  } \
  linkage void ThreadDeleteExtension##index( \
    rtems_tcb *executing, \
    rtems_tcb *deleted \
  ) \
  { \
    Extension( index, THREAD_DELETE, executing, deleted ); \
  } \
  linkage void ThreadExittedExtension##index( rtems_tcb *executing ) \
  { \
    Extension( index, THREAD_EXITTED, executing, NULL ); \
  } \
  linkage void ThreadRestartExtension##index( \
    rtems_tcb *executing, \
    rtems_tcb *restarted \
  ) \
  { \
    Extension( index, THREAD_RESTART, executing, restarted ); \
  } \
  linkage void ThreadStartExtension##index( \
    rtems_tcb *executing, \
    rtems_tcb *started \
  ) \
  { \
    Extension( index, THREAD_START, executing, started ); \
  } \
  linkage void ThreadSwitchExtension##index( \
    rtems_tcb *executing, \
    rtems_tcb *heir \
  ) \
  { \
    Extension( index, THREAD_SWITCH, executing, heir ); \
  } \
  linkage void ThreadTerminateExtension##index( rtems_tcb *executing ) \
  { \
    Extension( index, THREAD_TERMINATE, executing, NULL ); \
  }

DEFINE_EXTENSIONS( 0, )
DEFINE_EXTENSIONS( 1, )

#define DEFINE_EXTENSIONS_AND_TABLE( index ) \
  DEFINE_EXTENSIONS( index, static ) \
  static const rtems_extensions_table table_##index = { \
    .thread_begin = ThreadBeginExtension##index, \
    .thread_create = ThreadCreateExtension##index, \
    .thread_delete = ThreadDeleteExtension##index, \
    .thread_exitted = ThreadExittedExtension##index, \
    .thread_restart = ThreadRestartExtension##index, \
    .thread_start = ThreadStartExtension##index, \
    .thread_switch = ThreadSwitchExtension##index, \
    .thread_terminate = ThreadTerminateExtension##index \
  }

DEFINE_EXTENSIONS_AND_TABLE( 2 );
DEFINE_EXTENSIONS_AND_TABLE( 3 );
DEFINE_EXTENSIONS_AND_TABLE( 4 );
DEFINE_EXTENSIONS_AND_TABLE( 5 );
DEFINE_EXTENSIONS_AND_TABLE( 6 );

static const rtems_extensions_table * const tables[] = {
  NULL,
  NULL,
  &table_2,
  &table_3,
  &table_4,
  &table_5,
  &table_6
};

static rtems_tcb *StartTestCase( ExtensionKind kind )
{
  size_t i;

  thread_create_allocator_owner_count = 0;
  thread_delete_allocator_owner_count = 0;
  _Atomic_Store_uint( &global_counter, 0, ATOMIC_ORDER_RELAXED );
  memset( extension_counter, 0, sizeof( extension_counter ) );
  memset( extension_events, 0, sizeof( extension_events ) );

  extension_under_test = kind;

  for ( i = 2; i < RTEMS_ARRAY_SIZE( extension_ids ); ++i ) {
    rtems_status_code sc;

    sc = rtems_extension_create(
      rtems_build_name( ' ', ' ', ' ', '2' + i ),
      tables[ i ],
      &extension_ids[ i ]
    );
    T_rsc_success( sc );
  }

  return GetExecuting();
}

static void CheckForward(
  ExtensionKind kind,
  unsigned int  counter,
  unsigned int  increment,
  rtems_tcb    *executing,
  rtems_tcb    *thread
)
{
  size_t i;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( extension_ids ); ++i ) {
    if ( i == 3 && kind != THREAD_SWITCH ) {
      continue;
    }

    if ( counter == 0 ) {
      T_eq_uint( extension_counter[ i ][ kind ], 0 );
    } else {
      T_eq_uint( extension_counter[ i ][ kind ], 1 );
      T_eq_uint( extension_events[ i ][ kind ][ 0 ].counter, counter );
      T_eq_ptr( extension_events[ i ][ kind ][ 0 ].executing, executing );
      T_eq_ptr( extension_events[ i ][ kind ][ 0 ].thread, thread );

      counter += increment;
    }
  }
}

static void CheckReverse(
  ExtensionKind kind,
  unsigned int  counter,
  unsigned int  increment,
  rtems_tcb    *executing,
  rtems_tcb    *thread
)
{
  size_t i;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( extension_ids ); ++i ) {
    if ( i == 5 && kind != THREAD_SWITCH ) {
      continue;
    }

    if ( counter == 0 ) {
      T_eq_uint( extension_counter[ i ][ kind ], 0 );
    } else {
      T_eq_uint( extension_counter[ i ][ kind ], 1 );
      T_eq_uint(
        extension_events[ i ][ kind ][ 0 ].counter,
        7 - counter
      );
      T_eq_ptr( extension_events[ i ][ kind ][ 0 ].executing, executing );
      T_eq_ptr( extension_events[ i ][ kind ][ 0 ].thread, thread );

      counter += increment;
    }
  }
}

static void CheckDeletedNotInvoked( ExtensionKind kind )
{
  size_t index;

  if ( kind == THREAD_DELETE || kind == THREAD_TERMINATE ) {
    index = 5;
  } else {
    index = 3;
  }

  T_eq_uint( extension_events[ index ][ kind ][ 0 ].counter, 0 );
  T_null( extension_events[ index ][ kind ][ 0 ].executing );
  T_null( extension_events[ index ][ kind ][ 0 ].thread );
}

static void BeginWorker( rtems_task_argument arg )
{
  T_eq_u32( arg, 0 );
  StopTestCase();
  rtems_task_exit();
}

static void ExittedWorker( rtems_task_argument arg )
{
  T_eq_u32( arg, 0 );
  (void) StartTestCase( THREAD_EXITTED );
}

static void RestartWorker( rtems_task_argument arg )
{
  T_eq_u32( arg, 0 );
  (void) StartTestCase( THREAD_RESTART );
  (void) rtems_task_restart( RTEMS_SELF, 1 );
}

static void StartWorker( rtems_task_argument arg )
{
  (void) arg;
  T_unreachable();
}

static void TerminateWorker( rtems_task_argument arg )
{
  T_eq_u32( arg, 0 );
  (void) StartTestCase( THREAD_TERMINATE );
  rtems_task_exit();
}

void *IdleBody( uintptr_t arg )
{
  rtems_event_set events;

  do {
    events = PollAnyEvents();
  } while ( events == 0 );

  (void) StartTestCase( THREAD_EXITTED );
  return (void *) arg;
}

static void RtemsUserextValUserext_Setup( void *ctx )
{
  SetSelfPriority( PRIO_NORMAL );
}

static void RtemsUserextValUserext_Teardown( void *ctx )
{
  RestoreRunnerPriority();
}

static T_fixture RtemsUserextValUserext_Fixture = {
  .setup = RtemsUserextValUserext_Setup,
  .stop = NULL,
  .teardown = RtemsUserextValUserext_Teardown,
  .scope = NULL,
  .initial_context = NULL
};

/**
 * @brief Create five dynamic extensions.  Switch to a started thread.  Delete
 *   three dynamic extension during the thread begin invocation.  Clean up the
 *   used resources.
 */
static void RtemsUserextValUserext_Action_0( void )
{
  rtems_tcb *executing;
  rtems_tcb *thread;
  rtems_id   id;

  id = CreateTask( "WORK", PRIO_LOW );
  thread = GetThread( id );
  StartTask( id, BeginWorker, NULL );
  executing = StartTestCase( THREAD_BEGIN );
  SetPriority( id, PRIO_HIGH );
  KillZombies();

  /*
   * Check that the thread switch extensions were invoked in the right order
   * before the thread begin extensions.
   */
  CheckForward( THREAD_SWITCH, 1, 1, executing, thread );

  /*
   * Check that the thread begin extensions were invoked in the right order.
   */
  CheckForward( THREAD_BEGIN, 8, 1, thread, NULL );

  /*
   * Check that the other extensions were not invoked.
   */
  CheckForward( THREAD_CREATE, 0, 0, NULL, NULL );
  CheckReverse( THREAD_DELETE, 0, 0, NULL, NULL );
  CheckForward( THREAD_EXITTED, 0, 0, NULL, NULL );
  CheckForward( THREAD_RESTART, 0, 0, NULL, NULL );
  CheckForward( THREAD_START, 0, 0, NULL, NULL );
  CheckReverse( THREAD_TERMINATE, 0, 0, NULL, NULL );

  /*
   * Check that the thread begin extension of the extension set deleted before
   * its turn in the invocation was not invoked.
   */
  CheckDeletedNotInvoked( THREAD_BEGIN );
}

/**
 * @brief Create five dynamic extensions.  Create a thread.  Delete three
 *   dynamic extension during the thread create invocation.  Clean up the used
 *   resources.
 */
static void RtemsUserextValUserext_Action_1( void )
{
  rtems_tcb *executing;
  rtems_tcb *thread;
  rtems_id   id;

  executing = StartTestCase( THREAD_CREATE );
  id = CreateTask( "WORK", PRIO_NORMAL );
  thread = GetThread( id );
  StopTestCase();
  DeleteTask( id );
  KillZombies();

  /*
   * Check that the thread create extensions were invoked in the right order.
   */
  CheckForward( THREAD_CREATE, 1, 1, executing, thread );

  /*
   * Check that the thread create extensions were invoked under protection of
   * the allocator mutex.
   */
  T_eq_u32( thread_create_allocator_owner_count, 6 );

  /*
   * Check that the other extensions were not invoked.
   */
  CheckForward( THREAD_BEGIN, 0, 0, NULL, NULL );
  CheckReverse( THREAD_DELETE, 0, 0, NULL, NULL );
  CheckForward( THREAD_EXITTED, 0, 0, NULL, NULL );
  CheckForward( THREAD_RESTART, 0, 0, NULL, NULL );
  CheckForward( THREAD_START, 0, 0, NULL, NULL );
  CheckForward( THREAD_SWITCH, 0, 0, NULL, NULL );
  CheckReverse( THREAD_TERMINATE, 0, 0, NULL, NULL );

  /*
   * Check that the thread create extension of the extension set deleted before
   * its turn in the invocation was not invoked.
   */
  CheckDeletedNotInvoked( THREAD_CREATE );
}

/**
 * @brief Create five dynamic extensions.  Delete a thread.  Delete three
 *   dynamic extension during the thread delete invocation.  Clean up the used
 *   resources.
 */
static void RtemsUserextValUserext_Action_2( void )
{
  rtems_tcb *executing;
  rtems_tcb *thread;
  rtems_id   id;

  id = CreateTask( "WORK", PRIO_NORMAL );
  thread = GetThread( id );
  DeleteTask( id );
  executing = StartTestCase( THREAD_DELETE );
  KillZombies();
  StopTestCase();

  /*
   * Check that the thread delete extensions were invoked in the right order.
   */
  CheckReverse( THREAD_DELETE, 1, 1, executing, thread );

  /*
   * Check that the thread delete extensions were invoked under protection of
   * the allocator mutex.
   */
  T_eq_u32( thread_delete_allocator_owner_count, 6 );

  /*
   * Check that the other extensions were not invoked.
   */
  CheckForward( THREAD_BEGIN, 0, 0, NULL, NULL );
  CheckForward( THREAD_CREATE, 0, 0, NULL, NULL );
  CheckForward( THREAD_EXITTED, 0, 0, NULL, NULL );
  CheckForward( THREAD_RESTART, 0, 0, NULL, NULL );
  CheckForward( THREAD_START, 0, 0, NULL, NULL );
  CheckForward( THREAD_SWITCH, 0, 0, NULL, NULL );
  CheckReverse( THREAD_TERMINATE, 0, 0, NULL, NULL );

  /*
   * Check that the thread delete extension of the extension set deleted before
   * its turn in the invocation was not invoked.
   */
  CheckDeletedNotInvoked( THREAD_DELETE );
}

/**
 * @brief Create five dynamic extensions.  Return from a thread entry.  Delete
 *   three dynamic extension during the thread exitted invocation.  Clean up
 *   the used resources.
 */
static void RtemsUserextValUserext_Action_3( void )
{
  rtems_tcb *thread;
  rtems_id   id;

  id = CreateTask( "WORK", PRIO_HIGH );
  thread = GetThread( id );
  StartTask( id, ExittedWorker, NULL );
  KillZombies();

  /*
   * Check that the thread exitted extensions were invoked in the right order.
   */
  CheckForward( THREAD_EXITTED, 1, 1, thread, NULL );

  /*
   * Check that the other extensions were not invoked.
   */
  CheckForward( THREAD_BEGIN, 0, 0, NULL, NULL );
  CheckForward( THREAD_CREATE, 0, 0, NULL, NULL );
  CheckReverse( THREAD_DELETE, 0, 0, NULL, NULL );
  CheckForward( THREAD_RESTART, 0, 0, NULL, NULL );
  CheckForward( THREAD_START, 0, 0, NULL, NULL );
  CheckForward( THREAD_SWITCH, 0, 0, NULL, NULL );
  CheckReverse( THREAD_TERMINATE, 0, 0, NULL, NULL );

  /*
   * Check that the thread exitted extension of the extension set deleted
   * before its turn in the invocation was not invoked.
   */
  CheckDeletedNotInvoked( THREAD_EXITTED );
}

/**
 * @brief Create five dynamic extensions.  Restart a thread.  Delete three
 *   dynamic extension during the thread restart invocation.  Clean up the used
 *   resources.
 */
static void RtemsUserextValUserext_Action_4( void )
{
  rtems_tcb *thread;
  rtems_id   id;

  id = CreateTask( "WORK", PRIO_HIGH );
  thread = GetThread( id );
  StartTask( id, RestartWorker, NULL );
  KillZombies();

  /*
   * Check that the thread restart extensions were invoked in the right order.
   */
  CheckForward( THREAD_RESTART, 1, 1, thread, thread );

  /*
   * Check that the other extensions were not invoked.
   */
  CheckForward( THREAD_BEGIN, 0, 0, NULL, NULL );
  CheckForward( THREAD_EXITTED, 0, 0, NULL, NULL );
  CheckForward( THREAD_CREATE, 0, 0, NULL, NULL );
  CheckReverse( THREAD_DELETE, 0, 0, NULL, NULL );
  CheckForward( THREAD_START, 0, 0, NULL, NULL );
  CheckForward( THREAD_SWITCH, 0, 0, NULL, NULL );
  CheckForward( THREAD_TERMINATE, 0, 0, NULL, NULL );

  /*
   * Check that the thread restart extension of the extension set deleted
   * before its turn in the invocation was not invoked.
   */
  CheckDeletedNotInvoked( THREAD_RESTART );
}

/**
 * @brief Create five dynamic extensions.  Start a thread.  Delete three
 *   dynamic extension during the thread start invocation.  Clean up the used
 *   resources.
 */
static void RtemsUserextValUserext_Action_5( void )
{
  rtems_tcb *executing;
  rtems_tcb *thread;
  rtems_id   id;

  id = CreateTask( "WORK", PRIO_LOW );
  thread = GetThread( id );
  executing = StartTestCase( THREAD_START );
  StartTask( id, StartWorker, NULL );
  StopTestCase();
  DeleteTask( id );
  KillZombies();

  /*
   * Check that the thread start extensions were invoked in the right order.
   */
  CheckForward( THREAD_START, 1, 1, executing, thread );

  /*
   * Check that the other extensions were not invoked.
   */
  CheckForward( THREAD_BEGIN, 0, 0, NULL, NULL );
  CheckForward( THREAD_EXITTED, 0, 0, NULL, NULL );
  CheckForward( THREAD_CREATE, 0, 0, NULL, NULL );
  CheckReverse( THREAD_DELETE, 0, 0, NULL, NULL );
  CheckForward( THREAD_RESTART, 0, 0, NULL, NULL );
  CheckForward( THREAD_SWITCH, 0, 0, NULL, NULL );
  CheckForward( THREAD_TERMINATE, 0, 0, NULL, NULL );

  /*
   * Check that the thread start extension of the extension set deleted before
   * its turn in the invocation was not invoked.
   */
  CheckDeletedNotInvoked( THREAD_START );
}

/**
 * @brief Create five dynamic extensions.  Terminate a thread.  Delete three
 *   dynamic extension during the thread terminate invocation.  Clean up the
 *   used resources.
 */
static void RtemsUserextValUserext_Action_6( void )
{
  rtems_tcb *thread;
  rtems_id   id;

  id = CreateTask( "WORK", PRIO_HIGH );
  thread = GetThread( id );
  StartTask( id, TerminateWorker, NULL );
  KillZombies();

  /*
   * Check that the thread terminate extensions were invoked in the right
   * order.
   */
  CheckReverse( THREAD_TERMINATE, 1, 1, thread, NULL );

  /*
   * Check that the other extensions were not invoked.
   */
  CheckForward( THREAD_BEGIN, 0, 0, NULL, NULL );
  CheckForward( THREAD_EXITTED, 0, 0, NULL, NULL );
  CheckForward( THREAD_CREATE, 0, 0, NULL, NULL );
  CheckReverse( THREAD_DELETE, 0, 0, NULL, NULL );
  CheckForward( THREAD_RESTART, 0, 0, NULL, NULL );
  CheckForward( THREAD_START, 0, 0, NULL, NULL );
  CheckForward( THREAD_SWITCH, 0, 0, NULL, NULL );

  /*
   * Check that the thread terminate extension of the extension set deleted
   * before its turn in the invocation was not invoked.
   */
  CheckDeletedNotInvoked( THREAD_TERMINATE );
}

/**
 * @brief Create five dynamic extensions.  Let an idle thread return from its
 *   entry. Delete three dynamic extension during the thread exitted
 *   invocation.  Clean up the used resources.
 */
static void RtemsUserextValUserext_Action_7( void )
{
  rtems_tcb *thread;
  rtems_id   id;

  /* ID of idle thread of processor 0 */
  id = 0x09010001;
  thread = GetThread( id );
  SendEvents( id, RTEMS_EVENT_0 );
  SetPriority( id, PRIO_HIGH );

  /*
   * Check that the thread exitted extensions were invoked in the right order.
   */
  CheckForward( THREAD_EXITTED, 1, 1, thread, NULL );

  /*
   * Check that the other extensions were not invoked.
   */
  CheckForward( THREAD_BEGIN, 0, 0, NULL, NULL );
  CheckForward( THREAD_CREATE, 0, 0, NULL, NULL );
  CheckReverse( THREAD_DELETE, 0, 0, NULL, NULL );
  CheckForward( THREAD_RESTART, 0, 0, NULL, NULL );
  CheckForward( THREAD_START, 0, 0, NULL, NULL );
  CheckForward( THREAD_SWITCH, 0, 0, NULL, NULL );
  CheckReverse( THREAD_TERMINATE, 0, 0, NULL, NULL );

  /*
   * Check that the thread exitted extension of the extension set deleted
   * before its turn in the invocation was not invoked.
   */
  CheckDeletedNotInvoked( THREAD_EXITTED );
}

/**
 * @fn void T_case_body_RtemsUserextValUserext( void )
 */
T_TEST_CASE_FIXTURE( RtemsUserextValUserext, &RtemsUserextValUserext_Fixture )
{
  RtemsUserextValUserext_Action_0();
  RtemsUserextValUserext_Action_1();
  RtemsUserextValUserext_Action_2();
  RtemsUserextValUserext_Action_3();
  RtemsUserextValUserext_Action_4();
  RtemsUserextValUserext_Action_5();
  RtemsUserextValUserext_Action_6();
  RtemsUserextValUserext_Action_7();
}

/** @} */
