/*
 * Copyright (c) 2014, 2018 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/smplock.h>
#include <rtems/score/chainimpl.h>

#include <string.h>

#if defined(RTEMS_SMP) && defined(RTEMS_PROFILING)

typedef struct {
  SMP_lock_Control Lock;
  Chain_Control Stats_chain;
  Chain_Control Iterator_chain;
} SMP_lock_Stats_control;

static SMP_lock_Stats_control _SMP_lock_Stats_control = {
  .Lock = {
    .Ticket_lock = {
      .next_ticket = ATOMIC_INITIALIZER_UINT( 0U ),
      .now_serving = ATOMIC_INITIALIZER_UINT( 0U )
    },
    .Stats = {
      .Node = CHAIN_NODE_INITIALIZER_ONE_NODE_CHAIN(
        &_SMP_lock_Stats_control.Stats_chain
      ),
      .name = "SMP Lock Stats"
    }
  },
  .Stats_chain = CHAIN_INITIALIZER_ONE_NODE(
    &_SMP_lock_Stats_control.Lock.Stats.Node
  ),
  .Iterator_chain = CHAIN_INITIALIZER_EMPTY(
    _SMP_lock_Stats_control.Iterator_chain
  )
};

void _SMP_lock_Stats_destroy( SMP_lock_Stats *stats )
{
  if ( !_Chain_Is_node_off_chain( &stats->Node ) ) {
    SMP_lock_Stats_control *control = &_SMP_lock_Stats_control;
    SMP_lock_Context lock_context;
    SMP_lock_Stats_iteration_context *iteration_context;
    SMP_lock_Stats_iteration_context *iteration_context_tail;
    SMP_lock_Stats *next_stats;

    _SMP_lock_ISR_disable_and_acquire( &control->Lock, &lock_context );

    next_stats = (SMP_lock_Stats *) _Chain_Next( &stats->Node );
    _Chain_Extract_unprotected( &stats->Node );

    iteration_context = (SMP_lock_Stats_iteration_context *)
      _Chain_First( &control->Iterator_chain );
    iteration_context_tail = (SMP_lock_Stats_iteration_context *)
      _Chain_Tail( &control->Iterator_chain );

    while ( iteration_context != iteration_context_tail ) {
      if ( iteration_context->current == stats ) {
        iteration_context->current = next_stats;
      }

      iteration_context = (SMP_lock_Stats_iteration_context *)
        _Chain_Next( &iteration_context->Node );
    }

    _SMP_lock_Release_and_ISR_enable( &control->Lock, &lock_context );
  }
}

void _SMP_lock_Stats_register_or_max_section_time(
  SMP_lock_Stats    *stats,
  CPU_Counter_ticks  max_section_time
)
{
  stats->max_section_time = max_section_time;

  if ( _Chain_Is_node_off_chain( &stats->Node ) ) {
    SMP_lock_Stats_control *control;
    SMP_lock_Context        lock_context;

    control = &_SMP_lock_Stats_control;
    _SMP_lock_ISR_disable_and_acquire( &control->Lock, &lock_context );
    _Chain_Append_unprotected( &control->Stats_chain, &stats->Node );
    _SMP_lock_Release_and_ISR_enable( &control->Lock, &lock_context );
  }
}

void _SMP_lock_Stats_iteration_start(
  SMP_lock_Stats_iteration_context *iteration_context
)
{
  SMP_lock_Stats_control *control = &_SMP_lock_Stats_control;
  SMP_lock_Context lock_context;

  _SMP_lock_ISR_disable_and_acquire( &control->Lock, &lock_context );

  _Chain_Initialize_node( &iteration_context->Node );
  _Chain_Append_unprotected(
    &control->Iterator_chain,
    &iteration_context->Node
  );
  iteration_context->current =
    (SMP_lock_Stats *) _Chain_First( &control->Stats_chain );

  _SMP_lock_Release_and_ISR_enable( &control->Lock, &lock_context );
}

bool _SMP_lock_Stats_iteration_next(
  SMP_lock_Stats_iteration_context *iteration_context,
  SMP_lock_Stats                   *snapshot,
  char                             *name,
  size_t                            name_size
)
{
  SMP_lock_Stats_control *control = &_SMP_lock_Stats_control;
  SMP_lock_Context lock_context;
  SMP_lock_Stats *current;
  bool valid;

  _SMP_lock_ISR_disable_and_acquire( &control->Lock, &lock_context );

  current = iteration_context->current;
  if ( !_Chain_Is_tail( &control->Stats_chain, &current->Node ) ) {
    size_t name_len = current->name != NULL ? strlen(current->name) : 0;

    valid = true;

    iteration_context->current = (SMP_lock_Stats *)
      _Chain_Next( &current->Node );

    *snapshot = *current;
    snapshot->name = name;

    if ( name_len >= name_size ) {
      name_len = name_size - 1;
    }

    name[name_len] = '\0';
    memcpy(name, current->name, name_len);
  } else {
    valid = false;
  }

  _SMP_lock_Release_and_ISR_enable( &control->Lock, &lock_context );

  return valid;
}

void _SMP_lock_Stats_iteration_stop(
  SMP_lock_Stats_iteration_context *iteration_context
)
{
  SMP_lock_Stats_control *control = &_SMP_lock_Stats_control;
  SMP_lock_Context lock_context;

  _SMP_lock_ISR_disable_and_acquire( &control->Lock, &lock_context );
  _Chain_Extract_unprotected( &iteration_context->Node );
  _SMP_lock_Release_and_ISR_enable( &control->Lock, &lock_context );
}

#endif /* RTEMS_SMP && RTEMS_PROFILING */
