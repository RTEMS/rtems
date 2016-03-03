/**
 * @file
 *
 * @brief Allocate and Initialize Per CPU Structures
 * @ingroup PerCPU
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/percpu.h>
#include <rtems/score/assert.h>
#include <rtems/score/isrlock.h>
#include <rtems/score/smpimpl.h>
#include <rtems/config.h>

RTEMS_STATIC_ASSERT(
  sizeof( CPU_Uint32ptr ) >= sizeof( uintptr_t ),
  CPU_Uint32ptr_greater_equal_uintptr_t
);

RTEMS_STATIC_ASSERT(
  sizeof( CPU_Uint32ptr ) >= sizeof( uint32_t ),
  CPU_Uint32ptr_greater_equal_uint32_t
);

#if defined(RTEMS_SMP)

typedef struct {
  SMP_Action_handler handler;
  void *arg;
} SMP_Before_multicast_action;

ISR_LOCK_DEFINE( static, _Per_CPU_State_lock, "Per-CPU State" )

static void _Per_CPU_State_acquire( ISR_lock_Context *lock_context )
{
  _ISR_lock_ISR_disable_and_acquire( &_Per_CPU_State_lock, lock_context );
}

static void _Per_CPU_State_release( ISR_lock_Context *lock_context )
{
  _ISR_lock_Release_and_ISR_enable( &_Per_CPU_State_lock, lock_context );
}

static void _Per_CPU_State_before_multitasking_action( Per_CPU_Control *cpu )
{
  uintptr_t action_value;

  action_value = _Atomic_Load_uintptr(
    &cpu->before_multitasking_action,
    ATOMIC_ORDER_ACQUIRE
  );

  if ( action_value != 0 ) {
    SMP_Before_multicast_action *action =
      (SMP_Before_multicast_action *) action_value;

    ( *action->handler )( action->arg );

    _Atomic_Store_uintptr(
      &cpu->before_multitasking_action,
      0,
      ATOMIC_ORDER_RELEASE
    );
  }
}

static void _Per_CPU_State_busy_wait(
  Per_CPU_Control *cpu,
  Per_CPU_State new_state
)
{
  Per_CPU_State state = cpu->state;

  switch ( new_state ) {
    case PER_CPU_STATE_REQUEST_START_MULTITASKING:
      while (
        state != PER_CPU_STATE_READY_TO_START_MULTITASKING
          && state != PER_CPU_STATE_SHUTDOWN
      ) {
        _CPU_SMP_Processor_event_receive();
        state = cpu->state;
      }
      break;
    case PER_CPU_STATE_UP:
      while (
        state != PER_CPU_STATE_REQUEST_START_MULTITASKING
          && state != PER_CPU_STATE_SHUTDOWN
      ) {
        _Per_CPU_State_before_multitasking_action( cpu );
        _CPU_SMP_Processor_event_receive();
        state = cpu->state;
      }
      break;
    default:
      /* No need to wait */
      break;
  }
}

static Per_CPU_State _Per_CPU_State_get_next(
  Per_CPU_State current_state,
  Per_CPU_State new_state
)
{
  switch ( current_state ) {
    case PER_CPU_STATE_INITIAL:
      switch ( new_state ) {
        case PER_CPU_STATE_READY_TO_START_MULTITASKING:
        case PER_CPU_STATE_SHUTDOWN:
          /* Change is acceptable */
          break;
        default:
          new_state = PER_CPU_STATE_SHUTDOWN;
          break;
      }
      break;
    case PER_CPU_STATE_READY_TO_START_MULTITASKING:
      switch ( new_state ) {
        case PER_CPU_STATE_REQUEST_START_MULTITASKING:
        case PER_CPU_STATE_SHUTDOWN:
          /* Change is acceptable */
          break;
        default:
          new_state = PER_CPU_STATE_SHUTDOWN;
          break;
      }
      break;
    case PER_CPU_STATE_REQUEST_START_MULTITASKING:
      switch ( new_state ) {
        case PER_CPU_STATE_UP:
        case PER_CPU_STATE_SHUTDOWN:
          /* Change is acceptable */
          break;
        default:
          new_state = PER_CPU_STATE_SHUTDOWN;
          break;
      }
      break;
    default:
      new_state = PER_CPU_STATE_SHUTDOWN;
      break;
  }

  return new_state;
}

void _Per_CPU_State_change(
  Per_CPU_Control *cpu,
  Per_CPU_State new_state
)
{
  ISR_lock_Context lock_context;
  Per_CPU_State next_state;

  _Per_CPU_State_busy_wait( cpu, new_state );

  _Per_CPU_State_acquire( &lock_context );

  next_state = _Per_CPU_State_get_next( cpu->state, new_state );
  cpu->state = next_state;

  if ( next_state == PER_CPU_STATE_SHUTDOWN ) {
    uint32_t cpu_max = rtems_configuration_get_maximum_processors();
    uint32_t cpu_index;

    for ( cpu_index = 0 ; cpu_index < cpu_max ; ++cpu_index ) {
      Per_CPU_Control *cpu_other = _Per_CPU_Get_by_index( cpu_index );

      if ( cpu_other != cpu ) {
        switch ( cpu_other->state ) {
          case PER_CPU_STATE_UP:
            _SMP_Send_message( cpu_index, SMP_MESSAGE_SHUTDOWN );
            break;
          default:
            /* Nothing to do */
            break;
        }

        cpu_other->state = PER_CPU_STATE_SHUTDOWN;
      }
    }
  }

  _CPU_SMP_Processor_event_broadcast();

  _Per_CPU_State_release( &lock_context );

  if (
    next_state == PER_CPU_STATE_SHUTDOWN
      && new_state != PER_CPU_STATE_SHUTDOWN
  ) {
    _SMP_Fatal( SMP_FATAL_SHUTDOWN );
  }
}

bool _SMP_Before_multitasking_action(
  Per_CPU_Control    *cpu,
  SMP_Action_handler  handler,
  void               *arg
)
{
  bool done;

  _Assert( _Per_CPU_Is_boot_processor( _Per_CPU_Get() ) );

  if ( _Per_CPU_Is_processor_online( cpu ) ) {
    SMP_Before_multicast_action action = {
      .handler = handler,
      .arg = arg
    };
    Per_CPU_State expected_state = PER_CPU_STATE_READY_TO_START_MULTITASKING;

    _Atomic_Store_uintptr(
      &cpu->before_multitasking_action,
      (uintptr_t) &action,
      ATOMIC_ORDER_RELEASE
    );

    _CPU_SMP_Processor_event_broadcast();

    _Per_CPU_State_busy_wait( cpu, expected_state );

    do {
      done = _Atomic_Load_uintptr(
        &cpu->before_multitasking_action,
        ATOMIC_ORDER_ACQUIRE
      ) == 0;
    } while ( !done && cpu->state == expected_state );
  } else {
    done = false;
  }

  return done;
}

#else
  /*
   * On single core systems, we can efficiently directly access a single
   * statically allocated per cpu structure.  And the fields are initialized
   * as individual elements just like it has always been done.
   */
  Per_CPU_Control_envelope _Per_CPU_Information[1];
#endif
