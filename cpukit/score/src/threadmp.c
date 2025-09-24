/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreThreadMP
 *
 * @brief This source file contains the implementation of
 *   _Thread_MP_Handler_initialization(), _Thread_MP_Allocate_proxy(),
 *   _Thread_MP_Find_proxy(), _Thread_MP_Free_proxy().
 */

/*
 *  COPYRIGHT (c) 1989-2006.
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

#include <rtems/score/isrlock.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threadimpl.h>

#include <string.h>

static RBTREE_DEFINE_EMPTY( _Thread_MP_Active_proxies );

static CHAIN_DEFINE_EMPTY( _Thread_MP_Inactive_proxies );

#if ISR_LOCK_NEEDS_OBJECT
static ISR_lock_Control _Thread_MP_Proxies_lock = ISR_LOCK_INITIALIZER(
  "Thread MP Proxies"
);
#endif

static void _Thread_MP_Proxies_acquire( ISR_lock_Context *lock_context )
{
  _ISR_lock_ISR_disable_and_acquire( &_Thread_MP_Proxies_lock, lock_context );
}

static void _Thread_MP_Proxies_release( ISR_lock_Context *lock_context )
{
  _ISR_lock_Release_and_ISR_enable( &_Thread_MP_Proxies_lock, lock_context );
}

void _Thread_MP_Handler_initialization( uint32_t maximum_proxies )
{
  size_t   proxy_size;
  char    *proxies;
  uint32_t i;

  if ( maximum_proxies == 0 ) {
    return;
  }

  proxy_size = sizeof( Thread_Proxy_control ) + _Thread_queue_Heads_size;
  proxies = (char *) _Thread_MP_Proxies;

  _Chain_Initialize(
    &_Thread_MP_Inactive_proxies,
    proxies,
    maximum_proxies,
    proxy_size
  );

  for ( i = 0; i < maximum_proxies; ++i ) {
    Thread_Proxy_control *proxy;

    proxy = (Thread_Proxy_control *) ( proxies + i * proxy_size );

    _Thread_Timer_initialize( &proxy->Timer, _Per_CPU_Get_by_index( 0 ) );
    _RBTree_Initialize_node( &proxy->Active );

    proxy->Scheduler.nodes = &proxy->Scheduler_node;
    _Scheduler_Node_do_initialize(
      &_Scheduler_Table[ 0 ],
      &proxy->Scheduler_node,
      (Thread_Control *) proxy,
      0
    );

    proxy->Wait.spare_heads = &proxy->Thread_queue_heads[ 0 ];
    _Thread_queue_Heads_initialize( proxy->Wait.spare_heads );
  }
}

#define THREAD_MP_PROXY_OF_ACTIVE_NODE( the_node ) \
  RTEMS_CONTAINER_OF( the_node, Thread_Proxy_control, Active )

static bool _Thread_MP_Proxy_equal( const void *left, const RBTree_Node *right )
{
  const Objects_Id           *the_left;
  const Thread_Proxy_control *the_right;

  the_left = left;
  the_right = THREAD_MP_PROXY_OF_ACTIVE_NODE( right );

  return *the_left == the_right->Object.id;
}

static bool _Thread_MP_Proxy_less( const void *left, const RBTree_Node *right )
{
  const Objects_Id           *the_left;
  const Thread_Proxy_control *the_right;

  the_left = left;
  the_right = THREAD_MP_PROXY_OF_ACTIVE_NODE( right );

  return *the_left < the_right->Object.id;
}

static void *_Thread_MP_Proxy_map( RBTree_Node *node )
{
  return THREAD_MP_PROXY_OF_ACTIVE_NODE( node );
}

Thread_Control *_Thread_MP_Allocate_proxy( States_Control the_state )
{
  Thread_Proxy_control *the_proxy;
  ISR_lock_Context      lock_context;

  _Thread_MP_Proxies_acquire( &lock_context );

  the_proxy = (Thread_Proxy_control *) _Chain_Get_unprotected(
    &_Thread_MP_Inactive_proxies
  );
  if ( the_proxy != NULL ) {
    Thread_Control   *executing;
    MP_packet_Prefix *receive_packet;
    Objects_Id        source_tid;

    executing = _Thread_Executing;
    receive_packet = _MPCI_Receive_server_tcb->receive_packet;
    source_tid = receive_packet->source_tid;

    executing->Wait.return_code = STATUS_PROXY_BLOCKING;

    the_proxy->receive_packet = receive_packet;
    the_proxy->Object.id = source_tid;
    the_proxy->Real_priority.priority = receive_packet->source_priority;
    the_proxy->current_state = _States_Set( STATES_DORMANT, the_state );

    the_proxy->Wait.count = executing->Wait.count;
    the_proxy->Wait.return_argument = executing->Wait.return_argument;
    the_proxy->Wait.return_argument_second = executing->Wait
                                               .return_argument_second;
    the_proxy->Wait.option = executing->Wait.option;
    the_proxy->Wait.return_code = executing->Wait.return_code;

    the_proxy->thread_queue_callout = _Thread_queue_MP_callout_do_nothing;

    _RBTree_Insert_inline(
      &_Thread_MP_Active_proxies,
      &the_proxy->Active,
      &source_tid,
      _Thread_MP_Proxy_less
    );

    _Thread_MP_Proxies_release( &lock_context );

    return (Thread_Control *) the_proxy;
  }

  _Thread_MP_Proxies_release( &lock_context );

  _Internal_error( INTERNAL_ERROR_OUT_OF_PROXIES );

  /*
   *  NOTE: The following return ensures that the compiler will
   *        think that all paths return a value.
   */

  return NULL;
}

Thread_Control *_Thread_MP_Find_proxy( Objects_Id the_id )
{
  Thread_Proxy_control *the_proxy;
  ISR_lock_Context      lock_context;

  _Thread_MP_Proxies_acquire( &lock_context );

  the_proxy = _RBTree_Find_inline(
    &_Thread_MP_Active_proxies,
    &the_id,
    _Thread_MP_Proxy_equal,
    _Thread_MP_Proxy_less,
    _Thread_MP_Proxy_map
  );

  _Thread_MP_Proxies_release( &lock_context );

  return (Thread_Control *) the_proxy;
}

void _Thread_MP_Free_proxy( Thread_Control *the_thread )
{
  Thread_Proxy_control *the_proxy;
  ISR_lock_Context      lock_context;

  the_proxy = (Thread_Proxy_control *) the_thread;

  _Thread_MP_Proxies_acquire( &lock_context );

  _RBTree_Extract( &_Thread_MP_Active_proxies, &the_proxy->Active );
  _Chain_Append_unprotected(
    &_Thread_MP_Inactive_proxies,
    &the_proxy->Object.Node
  );

  _Thread_MP_Proxies_release( &lock_context );
}
