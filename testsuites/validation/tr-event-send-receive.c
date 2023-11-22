/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsEventReqSendReceive
 */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
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

#include <rtems/score/threadimpl.h>

#include "tr-event-send-receive.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsEventReqSendReceive spec:/rtems/event/req/send-receive
 *
 * @ingroup TestsuitesValidation0
 *
 * @{
 */

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Pre_Send_NA : 1;
  uint16_t Pre_ReceiverState_NA : 1;
  uint16_t Pre_Satisfy_NA : 1;
  uint16_t Post_SendStatus : 2;
  uint16_t Post_ReceiveStatus : 3;
  uint16_t Post_SenderPreemption : 2;
} RtemsEventReqSendReceive_Entry;

#define PRIO_OTHER UINT32_MAX

typedef enum {
  SENDER_NONE,
  SENDER_SELF,
  SENDER_SELF_2,
  SENDER_WORKER,
  SENDER_INTERRUPT
} SenderTypes;

typedef enum {
  RECEIVE_SKIP,
  RECEIVE_NORMAL,
  RECEIVE_INTERRUPT
} ReceiveTypes;

typedef enum {
  RECEIVE_COND_UNKNOWN,
  RECEIVE_COND_SATSIFIED,
  RECEIVE_COND_UNSATISFIED
} ReceiveConditionStates;

/**
 * @brief Test context for spec:/rtems/event/req/send-receive test case.
 */
typedef struct {
  /**
   * @brief This member defines the sender type to perform the event send
   *   action.
   */
  SenderTypes sender_type;

  /**
   * @brief This member defines the sender task priority.
   */
  rtems_task_priority sender_prio;

  /**
   * @brief This member defines the receiver ID used for the event send action.
   */
  rtems_id receiver_id;

  /**
   * @brief This member defines the events to send for the event send action.
   */
  rtems_event_set events_to_send;

  /**
   * @brief This member contains the status of the event send action.
   */
  rtems_status_code send_status;

  /**
   * @brief This member contains the scheduler ID of the runner task.
   */
  ReceiveTypes receive_type;

  /**
   * @brief This member defines the option set used for the event receive
   *   action.
   */
  rtems_option receive_option_set;

  /**
   * @brief This member defines the timeout used for the event receive action.
   */
  rtems_interval receive_timeout;

  /**
   * @brief This member contains the events received by the event receive
   *   action.
   */
  rtems_event_set received_events;

  /**
   * @brief This member references the event set received by the event receive
   *   action or is NULL.
   */
  rtems_event_set *received_events_parameter;

  /**
   * @brief This member contains the status of the event receive action.
   */
  rtems_status_code receive_status;

  /**
   * @brief This member contains the event condition state of the receiver task
   *   after the event send action.
   */
  ReceiveConditionStates receive_condition_state;

  /**
   * @brief This member contains the pending events after an event send action
   *   which did not satisify the event condition of the receiver.
   */
  rtems_event_set unsatisfied_pending;

  /**
   * @brief This member contains the TCB of the runner task.
   */
  Thread_Control *runner_thread;

  /**
   * @brief This member contains the ID of the runner task.
   */
  rtems_id runner_id;

  /**
   * @brief This member contains the task ID of the worker task.
   */
  rtems_id worker_id;

  /**
   * @brief This member contains the ID of the semaphore used to wake up the
   *   worker task.
   */
  rtems_id worker_wakeup;

  /**
   * @brief This member contains the ID of the semaphore used to wake up the
   *   runner task.
   */
  rtems_id runner_wakeup;

  /**
   * @brief This member contains the scheduler ID of scheduler used by the
   *   runner task.
   */
  rtems_id runner_sched;

  /**
   * @brief This member contains the scheduler ID of another scheduler which is
   *   not used by the runner task.
   */
  rtems_id other_sched;

  /**
   * @brief This member contains the thread switch log.
   */
  T_thread_switch_log_4 thread_switch_log;

  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsEventReqSendReceive_Run() parameter.
   */
  rtems_status_code ( *send )( rtems_id, rtems_event_set );

  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsEventReqSendReceive_Run() parameter.
   */
  rtems_status_code ( *receive )( rtems_event_set, rtems_option, rtems_interval, rtems_event_set * );

  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsEventReqSendReceive_Run() parameter.
   */
  rtems_event_set ( *get_pending_events )( Thread_Control * );

  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsEventReqSendReceive_Run() parameter.
   */
  unsigned int wait_class;

  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsEventReqSendReceive_Run() parameter.
   */
  int waiting_for_event;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 4 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 4 ];

    /**
     * @brief If this member is true, then the test action loop is executed.
     */
    bool in_action_loop;

    /**
     * @brief This member contains the next transition map index.
     */
    size_t index;

    /**
     * @brief This member contains the current transition map entry.
     */
    RtemsEventReqSendReceive_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsEventReqSendReceive_Context;

static RtemsEventReqSendReceive_Context
  RtemsEventReqSendReceive_Instance;

static const char * const RtemsEventReqSendReceive_PreDesc_Id[] = {
  "InvId",
  "Task",
  "NA"
};

static const char * const RtemsEventReqSendReceive_PreDesc_Send[] = {
  "Zero",
  "Unrelated",
  "Any",
  "All",
  "MixedAny",
  "MixedAll",
  "NA"
};

static const char * const RtemsEventReqSendReceive_PreDesc_ReceiverState[] = {
  "InvAddr",
  "NotWaiting",
  "Poll",
  "Timeout",
  "Lower",
  "Equal",
  "Higher",
  "Other",
  "Intend",
  "NA"
};

static const char * const RtemsEventReqSendReceive_PreDesc_Satisfy[] = {
  "All",
  "Any",
  "NA"
};

static const char * const * const RtemsEventReqSendReceive_PreDesc[] = {
  RtemsEventReqSendReceive_PreDesc_Id,
  RtemsEventReqSendReceive_PreDesc_Send,
  RtemsEventReqSendReceive_PreDesc_ReceiverState,
  RtemsEventReqSendReceive_PreDesc_Satisfy,
  NULL
};

#define INPUT_EVENTS ( RTEMS_EVENT_5 | RTEMS_EVENT_23 )

typedef RtemsEventReqSendReceive_Context Context;

static rtems_id CreateWakeupSema( void )
{
  rtems_status_code sc;
  rtems_id          id;

  sc = rtems_semaphore_create(
    rtems_build_name( 'W', 'K', 'U', 'P' ),
    0,
    RTEMS_SIMPLE_BINARY_SEMAPHORE,
    0,
    &id
  );
  T_assert_rsc_success( sc );

  return id;
}

static void DeleteWakeupSema( rtems_id id )
{
  if ( id != 0 ) {
    rtems_status_code sc;

    sc = rtems_semaphore_delete( id );
    T_rsc_success( sc );
  }
}

static void Wait( rtems_id id )
{
  rtems_status_code sc;

  sc = rtems_semaphore_obtain( id, RTEMS_WAIT, RTEMS_NO_TIMEOUT );
  T_quiet_rsc_success( sc );
}

static void Wakeup( rtems_id id )
{
  rtems_status_code sc;

  sc = rtems_semaphore_release( id );
  T_quiet_rsc_success( sc );
}

static bool BlockedForEvent( Context *ctx, Thread_Wait_flags flags )
{
  return flags == ( ctx->wait_class | THREAD_WAIT_STATE_BLOCKED );
}

static bool IntendsToBlockForEvent( Context *ctx, Thread_Wait_flags flags )
{
  return flags == ( ctx->wait_class | THREAD_WAIT_STATE_INTEND_TO_BLOCK );
}

static bool IsReady( Context *ctx, Thread_Wait_flags flags )
{
  return flags == THREAD_WAIT_STATE_READY;
}

static bool IsSatisfiedFlags( Context *ctx )
{
  return IsReady(
    ctx,
    _Thread_Wait_flags_get( ctx->runner_thread )
  );
}

static bool IsSatisfiedState( Context *ctx )
{
  return ctx->runner_thread->current_state != ctx->waiting_for_event;
}

static void SendAction( Context *ctx )
{
  T_thread_switch_log *log;

  log = T_thread_switch_record_4( &ctx->thread_switch_log );
  T_quiet_null( log );
  ctx->send_status = ( *ctx->send )( ctx->receiver_id, ctx->events_to_send );
  log = T_thread_switch_record( NULL );
  T_quiet_eq_ptr( &log->header, &ctx->thread_switch_log.header );
}

static void Send(
  Context *ctx,
  bool  ( *is_satsified )( Context * )
)
{
  SendAction( ctx );

  if ( ( *is_satsified )( ctx ) ) {
    ctx->receive_condition_state = RECEIVE_COND_SATSIFIED;
  } else {
    rtems_status_code sc;
    rtems_event_set pending;
    rtems_event_set missing;

    ctx->receive_condition_state = RECEIVE_COND_UNSATISFIED;
    pending = ( *ctx->get_pending_events )( ctx->runner_thread );
    ctx->unsatisfied_pending = pending;

    missing = INPUT_EVENTS & ~ctx->events_to_send;
    T_ne_u32( missing, 0 );
    sc = ( *ctx->send )( ctx->runner_id, missing );
    T_rsc_success( sc );

    pending = ( *ctx->get_pending_events )( ctx->runner_thread );
    T_eq_u32( pending, ctx->events_to_send & ~INPUT_EVENTS );
  }
}

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    rtems_status_code    sc;
    rtems_task_priority  prio;
    T_thread_switch_log *log;

    Wait( ctx->worker_wakeup );

    switch ( ctx->sender_prio ) {
      case PRIO_NORMAL:
      case PRIO_HIGH:
        prio = SetSelfPriority( ctx->sender_prio );
        T_eq_u32( prio, PRIO_LOW );
        break;
      case PRIO_OTHER:
        log = T_thread_switch_record_4( &ctx->thread_switch_log );
        T_null( log );
        sc = rtems_task_set_scheduler(
          RTEMS_SELF,
          ctx->other_sched,
          PRIO_LOW
        );
        T_rsc_success( sc );

        /*
         * Make sure the context switch to the IDLE thread on the previous
         * CPU is recorded, otherwise the preemption check may sporadically
         * fail on some targets.
         */
        while (ctx->thread_switch_log.header.recorded < 2) {
          RTEMS_COMPILER_MEMORY_BARRIER();
        }

        log = T_thread_switch_record( NULL );
        T_eq_ptr( &log->header, &ctx->thread_switch_log.header );
        break;
      case PRIO_LOW:
        break;
    }

    Send( ctx, IsSatisfiedState );

    sc = rtems_task_set_scheduler(
      RTEMS_SELF,
      ctx->runner_sched,
      PRIO_HIGH
    );
    T_rsc_success( sc );

    Wakeup( ctx->runner_wakeup );
  }
}

static rtems_event_set GetPendingEvents( Context *ctx )
{
  rtems_event_set pending;
  rtems_status_code sc;

  sc = ( *ctx->receive )(
    RTEMS_PENDING_EVENTS,
    RTEMS_DEFAULT_OPTIONS,
    0,
    &pending
  );
  T_quiet_rsc_success( sc );

  return pending;
}

static void RtemsEventReqSendReceive_Cleanup( Context *ctx );

static void InterruptPrepare( void *arg )
{
  RtemsEventReqSendReceive_Cleanup( arg );
}

static void InterruptAction( void *arg )
{
  Context *ctx;

  ctx = arg;
  ctx->receive_status = ( *ctx->receive )(
    INPUT_EVENTS,
    ctx->receive_option_set,
    ctx->receive_timeout,
    &ctx->received_events
  );
  T_quiet_rsc_success( ctx->receive_status );
}

static void InterruptContinue( Context *ctx )
{
  rtems_status_code sc;

  sc = ( *ctx->send )( ctx->receiver_id, INPUT_EVENTS );
  T_quiet_rsc_success( sc );
}

static T_interrupt_test_state Interrupt( void *arg )
{
  Context                *ctx;
  Thread_Wait_flags       flags;
  T_interrupt_test_state  next_state;
  T_interrupt_test_state  previous_state;

  ctx = arg;
  flags = _Thread_Wait_flags_get( ctx->runner_thread );

  if ( IntendsToBlockForEvent( ctx, flags ) ) {
    next_state = T_INTERRUPT_TEST_DONE;
  } else if ( BlockedForEvent( ctx, flags ) ) {
    next_state = T_INTERRUPT_TEST_LATE;
  } else {
    next_state = T_INTERRUPT_TEST_EARLY;
  }

  previous_state = T_interrupt_test_change_state(
    T_INTERRUPT_TEST_ACTION,
    next_state
  );

  if ( previous_state == T_INTERRUPT_TEST_ACTION ) {
    if ( next_state == T_INTERRUPT_TEST_DONE ) {
      Send( ctx, IsSatisfiedFlags );
    } else {
      InterruptContinue( ctx );
    }
  }

  return next_state;
}

static const T_interrupt_test_config InterruptConfig = {
  .prepare = InterruptPrepare,
  .action = InterruptAction,
  .interrupt = Interrupt,
  .max_iteration_count = 10000
};

static void RtemsEventReqSendReceive_Pre_Id_Prepare(
  RtemsEventReqSendReceive_Context *ctx,
  RtemsEventReqSendReceive_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsEventReqSendReceive_Pre_Id_InvId: {
      /*
       * While the id parameter of the send directive is not associated with a
       * task.
       */
      ctx->receiver_id = 0xffffffff;
      ctx->sender_type = SENDER_SELF;
      break;
    }

    case RtemsEventReqSendReceive_Pre_Id_Task: {
      /*
       * While the id parameter of the send directive is is associated with a
       * task.
       */
      ctx->receiver_id = ctx->runner_id;
      break;
    }

    case RtemsEventReqSendReceive_Pre_Id_NA:
      break;
  }
}

static void RtemsEventReqSendReceive_Pre_Send_Prepare(
  RtemsEventReqSendReceive_Context *ctx,
  RtemsEventReqSendReceive_Pre_Send state
)
{
  switch ( state ) {
    case RtemsEventReqSendReceive_Pre_Send_Zero: {
      /*
       * While the event set sent is the empty.
       */
      ctx->events_to_send = 0;
      break;
    }

    case RtemsEventReqSendReceive_Pre_Send_Unrelated: {
      /*
       * While the event set sent is unrelated to the event receive condition.
       */
      ctx->events_to_send = RTEMS_EVENT_7;
      break;
    }

    case RtemsEventReqSendReceive_Pre_Send_Any: {
      /*
       * While the event set sent is contain at least one but not all events of
       * the event receive condition.
       */
      ctx->events_to_send = RTEMS_EVENT_5;
      break;
    }

    case RtemsEventReqSendReceive_Pre_Send_All: {
      /*
       * While the event set sent is contain all events of the event receive
       * condition.
       */
      ctx->events_to_send = RTEMS_EVENT_5 | RTEMS_EVENT_23;
      break;
    }

    case RtemsEventReqSendReceive_Pre_Send_MixedAny: {
      /*
       * While the event set sent is contain at least one but not all events of
       * the event receive condition and at least one unrelated event.
       */
      ctx->events_to_send = RTEMS_EVENT_5 | RTEMS_EVENT_7;
      break;
    }

    case RtemsEventReqSendReceive_Pre_Send_MixedAll: {
      /*
       * While the event set sent is contain all events of the event receive
       * condition and at least one unrelated event.
       */
      ctx->events_to_send = RTEMS_EVENT_5 | RTEMS_EVENT_7 | RTEMS_EVENT_23;
      break;
    }

    case RtemsEventReqSendReceive_Pre_Send_NA:
      break;
  }
}

static void RtemsEventReqSendReceive_Pre_ReceiverState_Prepare(
  RtemsEventReqSendReceive_Context          *ctx,
  RtemsEventReqSendReceive_Pre_ReceiverState state
)
{
  switch ( state ) {
    case RtemsEventReqSendReceive_Pre_ReceiverState_InvAddr: {
      /*
       * While the receiver task calls the receive directive with the event set
       * to receive parameter set to NULL.
       */
      ctx->sender_type = SENDER_SELF;
      ctx->receive_type = RECEIVE_NORMAL;
      ctx->received_events_parameter = NULL;
      break;
    }

    case RtemsEventReqSendReceive_Pre_ReceiverState_NotWaiting: {
      /*
       * While the receiver task is not waiting for events.
       */
      ctx->sender_type = SENDER_SELF;
      ctx->receive_type = RECEIVE_SKIP;
      break;
    }

    case RtemsEventReqSendReceive_Pre_ReceiverState_Poll: {
      /*
       * While the receiver task polls for events.
       */
      ctx->sender_type = SENDER_SELF;
      ctx->receive_type = RECEIVE_NORMAL;
      ctx->receive_option_set |= RTEMS_NO_WAIT;
      break;
    }

    case RtemsEventReqSendReceive_Pre_ReceiverState_Timeout: {
      /*
       * While the receiver task waited for events with a timeout which
       * occurred.
       */
      ctx->sender_type = SENDER_SELF_2;
      ctx->receive_type = RECEIVE_NORMAL;
      ctx->receive_timeout = 1;
      break;
    }

    case RtemsEventReqSendReceive_Pre_ReceiverState_Lower: {
      /*
       * While the receiver task is blocked waiting for events and the receiver
       * task shall have a lower priority than the sender task.
       */
      ctx->sender_type = SENDER_WORKER;
      ctx->sender_prio = PRIO_HIGH;
      ctx->receive_type = RECEIVE_NORMAL;
      break;
    }

    case RtemsEventReqSendReceive_Pre_ReceiverState_Equal: {
      /*
       * While the receiver task is blocked waiting for events and the receiver
       * task shall have a priority equal to the sender task.
       */
      ctx->sender_type = SENDER_WORKER;
      ctx->sender_prio = PRIO_NORMAL;
      ctx->receive_type = RECEIVE_NORMAL;
      break;
    }

    case RtemsEventReqSendReceive_Pre_ReceiverState_Higher: {
      /*
       * While the receiver task is blocked waiting for events and the receiver
       * task shall have a higher priority than the sender task.
       */
      ctx->sender_type = SENDER_WORKER;
      ctx->sender_prio = PRIO_LOW;
      ctx->receive_type = RECEIVE_NORMAL;
      break;
    }

    case RtemsEventReqSendReceive_Pre_ReceiverState_Other: {
      /*
       * While the receiver task is blocked waiting for events and the receiver
       * task shall be on another scheduler instance than the sender task.
       */
      ctx->sender_type = SENDER_WORKER;
      ctx->sender_prio = PRIO_OTHER;
      ctx->receive_type = RECEIVE_NORMAL;
      break;
    }

    case RtemsEventReqSendReceive_Pre_ReceiverState_Intend: {
      /*
       * While the receiver task intends to block for waiting for events.
       */
      ctx->sender_type = SENDER_INTERRUPT;
      ctx->receive_type = RECEIVE_INTERRUPT;
      break;
    }

    case RtemsEventReqSendReceive_Pre_ReceiverState_NA:
      break;
  }
}

static void RtemsEventReqSendReceive_Pre_Satisfy_Prepare(
  RtemsEventReqSendReceive_Context    *ctx,
  RtemsEventReqSendReceive_Pre_Satisfy state
)
{
  switch ( state ) {
    case RtemsEventReqSendReceive_Pre_Satisfy_All: {
      /*
       * While the receiver task is interested in all input events.
       */
      ctx->receive_option_set |= RTEMS_EVENT_ALL;
      break;
    }

    case RtemsEventReqSendReceive_Pre_Satisfy_Any: {
      /*
       * While the receiver task is interested in any input event.
       */
      ctx->receive_option_set |= RTEMS_EVENT_ANY;
      break;
    }

    case RtemsEventReqSendReceive_Pre_Satisfy_NA:
      break;
  }
}

static void RtemsEventReqSendReceive_Post_SendStatus_Check(
  RtemsEventReqSendReceive_Context        *ctx,
  RtemsEventReqSendReceive_Post_SendStatus state
)
{
  switch ( state ) {
    case RtemsEventReqSendReceive_Post_SendStatus_Ok: {
      /*
       * The send event status shall be RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->send_status );
      break;
    }

    case RtemsEventReqSendReceive_Post_SendStatus_InvId: {
      /*
       * The send event status shall be RTEMS_INVALID_ID.
       */
      T_rsc( ctx->send_status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsEventReqSendReceive_Post_SendStatus_NA:
      break;
  }
}

static void RtemsEventReqSendReceive_Post_ReceiveStatus_Check(
  RtemsEventReqSendReceive_Context           *ctx,
  RtemsEventReqSendReceive_Post_ReceiveStatus state
)
{
  switch ( state ) {
    case RtemsEventReqSendReceive_Post_ReceiveStatus_None: {
      /*
       * The receiver task shall not have pending events.
       */
      T_eq_int( ctx->receive_condition_state, RECEIVE_COND_UNKNOWN );
      T_eq_u32( GetPendingEvents( ctx ), 0 );
      break;
    }

    case RtemsEventReqSendReceive_Post_ReceiveStatus_Pending: {
      /*
       * The receiver task shall have all events sent pending.
       */
      T_eq_int( ctx->receive_condition_state, RECEIVE_COND_UNKNOWN );
      T_eq_u32( GetPendingEvents( ctx ), ctx->events_to_send );
      break;
    }

    case RtemsEventReqSendReceive_Post_ReceiveStatus_Timeout: {
      /*
       * The receive event status shall be RTEMS_TIMEOUT.  The receiver task
       * shall have all events sent after the timeout pending.
       */
      T_rsc( ctx->receive_status, RTEMS_TIMEOUT );
      T_eq_int( ctx->receive_condition_state, RECEIVE_COND_UNKNOWN );
      T_eq_u32( GetPendingEvents( ctx ), ctx->events_to_send );
      break;
    }

    case RtemsEventReqSendReceive_Post_ReceiveStatus_Satisfied: {
      /*
       * The receive event status shall be RTEMS_SUCCESSFUL. The receiver task
       * shall receive all events sent which are an element of the input
       * events.  The receiver task shall have all events sent which are not an
       * element of the input events pending.
       */
      T_rsc( ctx->receive_status, RTEMS_SUCCESSFUL );

      if ( ctx->receive_type != RECEIVE_NORMAL ) {
        T_eq_int( ctx->receive_condition_state, RECEIVE_COND_SATSIFIED );
      }

      T_eq_u32( ctx->received_events, ctx->events_to_send & INPUT_EVENTS );
      T_eq_u32( GetPendingEvents( ctx ), ctx->events_to_send & ~INPUT_EVENTS );
      break;
    }

    case RtemsEventReqSendReceive_Post_ReceiveStatus_Unsatisfied: {
      /*
       * The receive event status shall be RTEMS_UNSATISFIED. The receiver task
       * shall have all events sent pending.
       */
      T_rsc( ctx->receive_status, RTEMS_UNSATISFIED );
      T_eq_int( ctx->receive_condition_state, RECEIVE_COND_UNKNOWN );
      T_eq_u32( GetPendingEvents( ctx ), ctx->events_to_send );
      break;
    }

    case RtemsEventReqSendReceive_Post_ReceiveStatus_Blocked: {
      /*
       * The receiver task shall remain blocked waiting for events after the
       * directive call.  The receiver task shall have all events sent pending.
       */
      T_eq_int( ctx->receive_condition_state, RECEIVE_COND_UNSATISFIED );
      T_eq_u32( ctx->unsatisfied_pending, ctx->events_to_send );
      break;
    }

    case RtemsEventReqSendReceive_Post_ReceiveStatus_InvAddr: {
      /*
       * The receive event status shall be RTEMS_INVALID_ADDRESS.  The receiver
       * task shall have all events sent pending.
       */
      T_rsc( ctx->receive_status, RTEMS_INVALID_ADDRESS );
      T_eq_int( ctx->receive_condition_state, RECEIVE_COND_UNKNOWN );
      T_eq_u32( GetPendingEvents( ctx ), ctx->events_to_send );
      break;
    }

    case RtemsEventReqSendReceive_Post_ReceiveStatus_NA:
      break;
  }
}

static void RtemsEventReqSendReceive_Post_SenderPreemption_Check(
  RtemsEventReqSendReceive_Context              *ctx,
  RtemsEventReqSendReceive_Post_SenderPreemption state
)
{
  const T_thread_switch_log_4 *log;
  size_t                       i;

  log = &ctx->thread_switch_log;

  switch ( state ) {
    case RtemsEventReqSendReceive_Post_SenderPreemption_No: {
      /*
       * When the sender task calls the directive to send the events, the
       * sender task shall not be preempted as a result of the call.
       */
      /*
       * There may be a thread switch to the runner thread if the sender thread
       * was on another scheduler instance.
       */

      T_le_sz( log->header.recorded, 1 );

      for ( i = 0; i < log->header.recorded; ++i ) {
        T_ne_u32( log->events[ i ].executing, ctx->worker_id );
        T_eq_u32( log->events[ i ].heir, ctx->runner_id );
      }
      break;
    }

    case RtemsEventReqSendReceive_Post_SenderPreemption_Yes: {
      /*
       * When the sender task calls the directive to send the events, the
       * sender task shall be preempted as a result of the call.
       */
      T_eq_sz( log->header.recorded, 2 );
      T_eq_u32( log->events[ 0 ].heir, ctx->runner_id );
      T_eq_u32( log->events[ 1 ].heir, ctx->worker_id );
      break;
    }

    case RtemsEventReqSendReceive_Post_SenderPreemption_NA:
      break;
  }
}

static void RtemsEventReqSendReceive_Setup(
  RtemsEventReqSendReceive_Context *ctx
)
{
  rtems_status_code sc;

  ctx->runner_thread = _Thread_Get_executing();
  ctx->runner_id = ctx->runner_thread->Object.id;
  ctx->worker_wakeup = CreateWakeupSema();
  ctx->runner_wakeup = CreateWakeupSema();

  sc = rtems_task_get_scheduler( RTEMS_SELF, &ctx->runner_sched );
  T_rsc_success( sc );

  #if defined(RTEMS_SMP)
  sc = rtems_scheduler_ident_by_processor( 1, &ctx->other_sched );
  T_rsc_success( sc );
  T_ne_u32( ctx->runner_sched, ctx->other_sched );
  #endif

  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_LOW );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void RtemsEventReqSendReceive_Setup_Wrap( void *arg )
{
  RtemsEventReqSendReceive_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsEventReqSendReceive_Setup( ctx );
}

static void RtemsEventReqSendReceive_Teardown(
  RtemsEventReqSendReceive_Context *ctx
)
{
  DeleteTask( ctx->worker_id );
  DeleteWakeupSema( ctx->worker_wakeup );
  DeleteWakeupSema( ctx->runner_wakeup );
  RestoreRunnerPriority();
}

static void RtemsEventReqSendReceive_Teardown_Wrap( void *arg )
{
  RtemsEventReqSendReceive_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsEventReqSendReceive_Teardown( ctx );
}

static void RtemsEventReqSendReceive_Prepare(
  RtemsEventReqSendReceive_Context *ctx
)
{
  ctx->events_to_send = 0;
  ctx->send_status = RTEMS_INCORRECT_STATE;
  ctx->received_events = 0xffffffff;
  ctx->received_events_parameter = &ctx->received_events;
  ctx->receive_option_set = 0;
  ctx->receive_timeout = RTEMS_NO_TIMEOUT;
  ctx->sender_type = SENDER_NONE;
  ctx->sender_prio = PRIO_NORMAL;
  ctx->receive_type = RECEIVE_SKIP;
  ctx->receive_condition_state = RECEIVE_COND_UNKNOWN;
  ctx->unsatisfied_pending = 0xffffffff;
  memset( &ctx->thread_switch_log, 0, sizeof( ctx->thread_switch_log ) );
  T_eq_u32( GetPendingEvents( ctx ), 0 );
}

static void RtemsEventReqSendReceive_Action(
  RtemsEventReqSendReceive_Context *ctx
)
{
  if ( ctx->sender_type == SENDER_SELF ) {
    SendAction( ctx );
  } else if ( ctx->sender_type == SENDER_WORKER ) {
    Wakeup( ctx->worker_wakeup );
  }

  if ( ctx->receive_type == RECEIVE_NORMAL ) {
    ctx->receive_status = ( *ctx->receive )(
      INPUT_EVENTS,
      ctx->receive_option_set,
      ctx->receive_timeout,
      ctx->received_events_parameter
    );
  } else if ( ctx->receive_type == RECEIVE_INTERRUPT ) {
    T_interrupt_test_state state;

    state = T_interrupt_test( &InterruptConfig, ctx );
    T_eq_int( state, T_INTERRUPT_TEST_DONE );
  }

  if ( ctx->sender_type == SENDER_SELF_2 ) {
    SendAction( ctx );
  } else if ( ctx->sender_type == SENDER_WORKER ) {
    rtems_task_priority prio;

    Wait( ctx->runner_wakeup );
    prio = SetPriority( ctx->worker_id, PRIO_LOW );
    T_eq_u32( prio, PRIO_HIGH );
  }
}

static void RtemsEventReqSendReceive_Cleanup(
  RtemsEventReqSendReceive_Context *ctx
)
{
  rtems_status_code sc;
  rtems_event_set   events;

  events = 0;
  sc = ( *ctx->receive )(
    RTEMS_ALL_EVENTS,
    RTEMS_NO_WAIT | RTEMS_EVENT_ANY,
    0,
    &events
  );
  if ( sc == RTEMS_SUCCESSFUL ) {
    T_quiet_ne_u32( events, 0 );
  } else {
    T_quiet_rsc( sc, RTEMS_UNSATISFIED );
    T_quiet_eq_u32( events, 0 );
  }
}

static const RtemsEventReqSendReceive_Entry
RtemsEventReqSendReceive_Entries[] = {
  { 0, 0, 1, 1, 1, RtemsEventReqSendReceive_Post_SendStatus_InvId,
    RtemsEventReqSendReceive_Post_ReceiveStatus_None,
    RtemsEventReqSendReceive_Post_SenderPreemption_No },
  { 0, 0, 0, 0, 1, RtemsEventReqSendReceive_Post_SendStatus_Ok,
    RtemsEventReqSendReceive_Post_ReceiveStatus_Pending,
    RtemsEventReqSendReceive_Post_SenderPreemption_No },
  { 0, 0, 0, 0, 0, RtemsEventReqSendReceive_Post_SendStatus_Ok,
    RtemsEventReqSendReceive_Post_ReceiveStatus_Blocked,
    RtemsEventReqSendReceive_Post_SenderPreemption_No },
  { 0, 0, 0, 0, 0, RtemsEventReqSendReceive_Post_SendStatus_Ok,
    RtemsEventReqSendReceive_Post_ReceiveStatus_Satisfied,
    RtemsEventReqSendReceive_Post_SenderPreemption_No },
  { 0, 0, 0, 0, 0, RtemsEventReqSendReceive_Post_SendStatus_Ok,
    RtemsEventReqSendReceive_Post_ReceiveStatus_Timeout,
    RtemsEventReqSendReceive_Post_SenderPreemption_No },
  { 0, 0, 0, 0, 0, RtemsEventReqSendReceive_Post_SendStatus_Ok,
    RtemsEventReqSendReceive_Post_ReceiveStatus_Unsatisfied,
    RtemsEventReqSendReceive_Post_SenderPreemption_No },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, RtemsEventReqSendReceive_Post_SendStatus_Ok,
    RtemsEventReqSendReceive_Post_ReceiveStatus_Blocked,
    RtemsEventReqSendReceive_Post_SenderPreemption_No },
#else
  { 1, 0, 0, 0, 0, RtemsEventReqSendReceive_Post_SendStatus_NA,
    RtemsEventReqSendReceive_Post_ReceiveStatus_NA,
    RtemsEventReqSendReceive_Post_SenderPreemption_NA },
#endif
  { 0, 0, 0, 0, 0, RtemsEventReqSendReceive_Post_SendStatus_Ok,
    RtemsEventReqSendReceive_Post_ReceiveStatus_Satisfied,
    RtemsEventReqSendReceive_Post_SenderPreemption_Yes },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, RtemsEventReqSendReceive_Post_SendStatus_Ok,
    RtemsEventReqSendReceive_Post_ReceiveStatus_Satisfied,
    RtemsEventReqSendReceive_Post_SenderPreemption_No }
#else
  { 1, 0, 0, 0, 0, RtemsEventReqSendReceive_Post_SendStatus_NA,
    RtemsEventReqSendReceive_Post_ReceiveStatus_NA,
    RtemsEventReqSendReceive_Post_SenderPreemption_NA }
#endif
};

static const uint8_t
RtemsEventReqSendReceive_Map[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 1, 1, 1, 5, 5, 4, 4, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 1, 1, 1, 1,
  5, 5, 4, 4, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 1, 1, 1, 1, 5, 3, 4, 4, 2, 3, 2, 3,
  2, 7, 6, 8, 2, 3, 1, 1, 1, 1, 3, 3, 4, 4, 3, 3, 3, 3, 7, 7, 8, 8, 3, 3, 1, 1,
  1, 1, 5, 3, 4, 4, 2, 3, 2, 3, 2, 7, 6, 8, 2, 3, 1, 1, 1, 1, 3, 3, 4, 4, 3, 3,
  3, 3, 7, 7, 8, 8, 3, 3
};

static size_t RtemsEventReqSendReceive_Scope( void *arg, char *buf, size_t n )
{
  RtemsEventReqSendReceive_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsEventReqSendReceive_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsEventReqSendReceive_Fixture = {
  .setup = RtemsEventReqSendReceive_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsEventReqSendReceive_Teardown_Wrap,
  .scope = RtemsEventReqSendReceive_Scope,
  .initial_context = &RtemsEventReqSendReceive_Instance
};

static inline RtemsEventReqSendReceive_Entry RtemsEventReqSendReceive_PopEntry(
  RtemsEventReqSendReceive_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsEventReqSendReceive_Entries[
    RtemsEventReqSendReceive_Map[ index ]
  ];
}

static void RtemsEventReqSendReceive_SetPreConditionStates(
  RtemsEventReqSendReceive_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Send_NA ) {
    ctx->Map.pcs[ 1 ] = RtemsEventReqSendReceive_Pre_Send_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_ReceiverState_NA ) {
    ctx->Map.pcs[ 2 ] = RtemsEventReqSendReceive_Pre_ReceiverState_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_Satisfy_NA ) {
    ctx->Map.pcs[ 3 ] = RtemsEventReqSendReceive_Pre_Satisfy_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }
}

static void RtemsEventReqSendReceive_TestVariant(
  RtemsEventReqSendReceive_Context *ctx
)
{
  RtemsEventReqSendReceive_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsEventReqSendReceive_Pre_Send_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsEventReqSendReceive_Pre_ReceiverState_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsEventReqSendReceive_Pre_Satisfy_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsEventReqSendReceive_Action( ctx );
  RtemsEventReqSendReceive_Post_SendStatus_Check(
    ctx,
    ctx->Map.entry.Post_SendStatus
  );
  RtemsEventReqSendReceive_Post_ReceiveStatus_Check(
    ctx,
    ctx->Map.entry.Post_ReceiveStatus
  );
  RtemsEventReqSendReceive_Post_SenderPreemption_Check(
    ctx,
    ctx->Map.entry.Post_SenderPreemption
  );
}

static T_fixture_node RtemsEventReqSendReceive_Node;

static T_remark RtemsEventReqSendReceive_Remark = {
  .next = NULL,
  .remark = "RtemsEventReqSendReceive"
};

void RtemsEventReqSendReceive_Run(
  rtems_status_code ( *send )( rtems_id, rtems_event_set ),
  rtems_status_code ( *receive )( rtems_event_set, rtems_option, rtems_interval, rtems_event_set * ),
  rtems_event_set (   *get_pending_events )( Thread_Control * ),
  unsigned int         wait_class,
  int                  waiting_for_event
)
{
  RtemsEventReqSendReceive_Context *ctx;

  ctx = &RtemsEventReqSendReceive_Instance;
  ctx->send = send;
  ctx->receive = receive;
  ctx->get_pending_events = get_pending_events;
  ctx->wait_class = wait_class;
  ctx->waiting_for_event = waiting_for_event;

  ctx = T_push_fixture(
    &RtemsEventReqSendReceive_Node,
    &RtemsEventReqSendReceive_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsEventReqSendReceive_Pre_Id_InvId;
    ctx->Map.pci[ 0 ] < RtemsEventReqSendReceive_Pre_Id_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsEventReqSendReceive_Pre_Send_Zero;
      ctx->Map.pci[ 1 ] < RtemsEventReqSendReceive_Pre_Send_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsEventReqSendReceive_Pre_ReceiverState_InvAddr;
        ctx->Map.pci[ 2 ] < RtemsEventReqSendReceive_Pre_ReceiverState_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = RtemsEventReqSendReceive_Pre_Satisfy_All;
          ctx->Map.pci[ 3 ] < RtemsEventReqSendReceive_Pre_Satisfy_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = RtemsEventReqSendReceive_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          RtemsEventReqSendReceive_SetPreConditionStates( ctx );
          RtemsEventReqSendReceive_Prepare( ctx );
          RtemsEventReqSendReceive_TestVariant( ctx );
          RtemsEventReqSendReceive_Cleanup( ctx );
        }
      }
    }
  }

  T_add_remark( &RtemsEventReqSendReceive_Remark );
  T_pop_fixture();
}

/** @} */
