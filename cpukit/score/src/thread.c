/**
 * @file
 *
 * @ingroup RTEMSScoreThread
 *
 * @brief This source file contains static assertions related to the Thread
 *   Handler and the implementation of _Thread_Initialize_information() and
 *   _Thread_Handler_initialization().
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>
#include <rtems/score/freechainimpl.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/scheduler.h>

#define THREAD_OFFSET_ASSERT( field ) \
  RTEMS_STATIC_ASSERT( \
    offsetof( Thread_Control, field ) == offsetof( Thread_Proxy_control, field ), \
    field \
  )

THREAD_OFFSET_ASSERT( Object );
THREAD_OFFSET_ASSERT( Join_queue );
THREAD_OFFSET_ASSERT( current_state );
THREAD_OFFSET_ASSERT( Real_priority );
#if defined(RTEMS_SCORE_THREAD_ENABLE_RESOURCE_COUNT)
THREAD_OFFSET_ASSERT( resource_count );
#endif
THREAD_OFFSET_ASSERT( Scheduler );
THREAD_OFFSET_ASSERT( Wait );
THREAD_OFFSET_ASSERT( Timer );
#if defined(RTEMS_MULTIPROCESSING)
THREAD_OFFSET_ASSERT( receive_packet );
#endif

void _Thread_Initialize_information( Thread_Information *information )
{
  _Objects_Initialize_information( &information->Objects );

  _Freechain_Initialize(
    &information->Thread_queue_heads.Free,
    information->Thread_queue_heads.initial,
    _Objects_Get_maximum_index( &information->Objects ),
    _Thread_queue_Heads_size
  );
}

void _Thread_Handler_initialization(void)
{
#if defined(RTEMS_MULTIPROCESSING)
  _Thread_MP_Handler_initialization( _MPCI_Configuration.maximum_proxies );
#endif

  /*
   *  Initialize the internal class of threads.  We need an IDLE thread
   *  per CPU in an SMP system.  In addition, if this is a loosely
   *  coupled multiprocessing system, account for the MPCI Server Thread.
   */
  _Thread_Initialize_information( &_Thread_Information );
}
