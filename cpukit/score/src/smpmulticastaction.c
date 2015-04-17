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
  Chain_Node Node;
  SMP_Multicast_action_handler handler;
  void *arg;
  cpu_set_t *recipients;
  size_t setsize;
  Atomic_Ulong done;
} SMP_Multicast_action;

typedef struct {
  SMP_lock_Control Lock;
  Chain_Control List;
} SMP_Multicast_action_context;

static SMP_Multicast_action_context _SMP_Multicast_action_context = {
  .Lock = SMP_LOCK_INITIALIZER("SMP Multicast Action"),
  .List = CHAIN_INITIALIZER_EMPTY(_SMP_Multicast_action_context.List)
};

void
_SMP_Multicast_actions_process(void)
{
  SMP_lock_Context lock_context;
  SMP_Multicast_action *node;
  SMP_Multicast_action *next;
  uint32_t cpu_self_idx;

  _SMP_lock_ISR_disable_and_acquire( &_SMP_Multicast_action_context.Lock,
      &lock_context );
  cpu_self_idx = _SMP_Get_current_processor();

  node = (SMP_Multicast_action*)_Chain_First(
      &_SMP_Multicast_action_context.List );
  while ( !_Chain_Is_tail( &_SMP_Multicast_action_context.List, &node->Node ) ) {
    next = (SMP_Multicast_action*)_Chain_Next( &node->Node );
    if ( CPU_ISSET_S ( cpu_self_idx, node->setsize, node->recipients ) ) {
      CPU_CLR_S ( cpu_self_idx, node->setsize, node->recipients );

      node->handler( node->arg );

      if ( CPU_COUNT_S( node->setsize, node->recipients ) == 0 ) {
        _Chain_Extract_unprotected( &node->Node );
        _Atomic_Store_ulong( &node->done, 1, ATOMIC_ORDER_RELEASE );
      }
    }
    node = next;
  }

  _SMP_lock_Release_and_ISR_enable( &_SMP_Multicast_action_context.Lock,
      &lock_context );
}

static void
_SMP_Multicast_actions_try_process( void )
{
  unsigned long message;
  Per_CPU_Control *cpu_self;
  ISR_Level isr_level;

  _ISR_Disable_without_giant( isr_level );

  cpu_self = _Per_CPU_Get();

  message = _Atomic_Load_ulong( &cpu_self->message, ATOMIC_ORDER_RELAXED );

  if ( message & SMP_MESSAGE_MULTICAST_ACTION ) {
    if ( _Atomic_Compare_exchange_ulong( &cpu_self->message, &message,
        message & ~SMP_MESSAGE_MULTICAST_ACTION, ATOMIC_ORDER_RELAXED,
        ATOMIC_ORDER_RELAXED ) ) {
      _SMP_Multicast_actions_process();
    }
  }

  _ISR_Enable_without_giant( isr_level );
}

void _SMP_Multicast_action(
  const size_t setsize,
  const cpu_set_t *cpus,
  SMP_Multicast_action_handler handler,
  void *arg
)
{
  uint32_t i;
  SMP_Multicast_action node;
  size_t set_size = CPU_ALLOC_SIZE( _SMP_Get_processor_count() );
  char cpu_set_copy[set_size];
  SMP_lock_Context lock_context;

  if ( ! _System_state_Is_up( _System_state_Get() ) ) {
    handler( arg );
    return;
  }

  memset( cpu_set_copy, 0, set_size );
  if( cpus == NULL ) {
    for( i=0; i<_SMP_Get_processor_count(); ++i )
      CPU_SET_S( i, set_size, (cpu_set_t *)cpu_set_copy );
  } else {
    for( i=0; i<_SMP_Get_processor_count(); ++i )
      if( CPU_ISSET_S( i, set_size, cpus ) )
        CPU_SET_S( i, set_size, (cpu_set_t *)cpu_set_copy );
  }

  node.handler = handler;
  node.arg = arg;
  node.setsize = set_size;
  node.recipients = (cpu_set_t *)cpu_set_copy;
  _Atomic_Store_ulong( &node.done, 0, ATOMIC_ORDER_RELAXED );


  _SMP_lock_ISR_disable_and_acquire( &_SMP_Multicast_action_context.Lock,
      &lock_context );
  _Chain_Prepend_unprotected( &_SMP_Multicast_action_context.List, &node.Node );
  _SMP_lock_Release_and_ISR_enable( &_SMP_Multicast_action_context.Lock,
      &lock_context );

  _SMP_Send_message_multicast( set_size, node.recipients,
      SMP_MESSAGE_MULTICAST_ACTION );

  _SMP_Multicast_actions_try_process();

  while ( !_Atomic_Load_ulong( &node.done, ATOMIC_ORDER_ACQUIRE ) );
}
