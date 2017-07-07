/*
 * Copyright (c) 2014 Aeroflex Gaisler AB.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/smpimpl.h>
#include <rtems/score/smplock.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/sysstate.h>

typedef struct {
  Chain_Node          Node;
  SMP_Action_handler  handler;
  void               *arg;
  Processor_mask      targets;
  Atomic_Ulong        done;
} SMP_Multicast_action;

typedef struct {
  SMP_lock_Control Lock;
  Chain_Control    Actions;
} SMP_Multicast_context;

static SMP_Multicast_context _SMP_Multicast = {
  .Lock = SMP_LOCK_INITIALIZER( "SMP Multicast Action" ),
  .Actions = CHAIN_INITIALIZER_EMPTY( _SMP_Multicast.Actions )
};

void _SMP_Multicast_actions_process( void )
{
  SMP_lock_Context      lock_context;
  uint32_t              cpu_self_index;
  SMP_Multicast_action *node;
  SMP_Multicast_action *next;

  _SMP_lock_ISR_disable_and_acquire( &_SMP_Multicast.Lock, &lock_context );
  cpu_self_index = _SMP_Get_current_processor();
  node = (SMP_Multicast_action *) _Chain_First( &_SMP_Multicast.Actions );

  while ( !_Chain_Is_tail( &_SMP_Multicast.Actions, &node->Node ) ) {
    next = (SMP_Multicast_action *) _Chain_Next( &node->Node );

    if ( _Processor_mask_Is_set( &node->targets, cpu_self_index ) ) {
      _Processor_mask_Clear( &node->targets, cpu_self_index );

      ( *node->handler )( node->arg );

      if ( _Processor_mask_Is_zero( &node->targets ) ) {
        _Chain_Extract_unprotected( &node->Node );
        _Atomic_Store_ulong( &node->done, 1, ATOMIC_ORDER_RELEASE );
      }
    }

    node = next;
  }

  _SMP_lock_Release_and_ISR_enable( &_SMP_Multicast.Lock, &lock_context );
}

static void
_SMP_Multicasts_try_process( void )
{
  unsigned long message;
  Per_CPU_Control *cpu_self;
  ISR_Level isr_level;

  _ISR_Local_disable( isr_level );

  cpu_self = _Per_CPU_Get();

  message = _Atomic_Load_ulong( &cpu_self->message, ATOMIC_ORDER_RELAXED );

  if ( message & SMP_MESSAGE_MULTICAST_ACTION ) {
    if ( _Atomic_Compare_exchange_ulong( &cpu_self->message, &message,
        message & ~SMP_MESSAGE_MULTICAST_ACTION, ATOMIC_ORDER_RELAXED,
        ATOMIC_ORDER_RELAXED ) ) {
      _SMP_Multicast_actions_process();
    }
  }

  _ISR_Local_enable( isr_level );
}

void _SMP_Multicast_action(
  const size_t setsize,
  const cpu_set_t *cpus,
  SMP_Action_handler handler,
  void *arg
)
{
  SMP_Multicast_action node;
  Processor_mask       targets;
  SMP_lock_Context     lock_context;
  uint32_t             i;

  if ( ! _System_state_Is_up( _System_state_Get() ) ) {
    ( *handler )( arg );
    return;
  }

  if( cpus == NULL ) {
    _Processor_mask_Assign( &targets, _SMP_Get_online_processors() );
  } else {
    _Processor_mask_Zero( &targets );

    for ( i = 0; i < _SMP_Get_processor_count(); ++i ) {
      if ( CPU_ISSET_S( i, setsize, cpus ) ) {
        _Processor_mask_Set( &targets, i );
      }
    }
  }

  _Chain_Initialize_node( &node.Node );
  node.handler = handler;
  node.arg = arg;
  _Processor_mask_Assign( &node.targets, &targets );
  _Atomic_Store_ulong( &node.done, 0, ATOMIC_ORDER_RELAXED );

  _SMP_lock_ISR_disable_and_acquire( &_SMP_Multicast.Lock, &lock_context );
  _Chain_Prepend_unprotected( &_SMP_Multicast.Actions, &node.Node );
  _SMP_lock_Release_and_ISR_enable( &_SMP_Multicast.Lock, &lock_context );

  _SMP_Send_message_multicast( &targets, SMP_MESSAGE_MULTICAST_ACTION );
  _SMP_Multicasts_try_process();

  while ( _Atomic_Load_ulong( &node.done, ATOMIC_ORDER_ACQUIRE ) == 0 ) {
    /* Wait */
  };
}
