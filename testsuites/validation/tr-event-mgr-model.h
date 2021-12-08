/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
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
 * Do not manually edit this file.  It is part of the RTEMS quality process
 * and was automatically generated.
 *
 * If you find something that needs to be fixed or worded better please
 * post a report to an RTEMS mailing list or raise a bug report:
 *
 * https://docs.rtems.org/branches/master/user/support/bugs.html
 *
 * For information on updating and regenerating please refer to:
 *
 * https://docs.rtems.org/branches/master/eng/req/howto.html
 */

#ifndef _TR_MODEL_EVENTS_MGR_H
#define _TR_MODEL_EVENTS_MGR_H

#include <rtems.h>
#include <rtems/score/thread.h>

#include <rtems/test.h>
#include "ts-config.h"

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Run Setup/Cleanup structs/functions
 */
typedef struct {
  rtems_status_code ( *send )( rtems_id, rtems_event_set ); // copy of the
                       // corresponding RtemsModelEventsMgr_Run() parameter
  rtems_status_code ( *receive )
                    ( rtems_event_set, rtems_option
                    , rtems_interval, rtems_event_set * ); // copy of the
                 // corresponding RtemsModelEventsMgr_Run() parameter
  rtems_event_set ( *get_pending_events )( Thread_Control * ); // copy of the
                    // corresponding RtemsModelEventsMgr_Run() parameter
  unsigned int wait_class; // copy of the corresponding
                           // RtemsModelEventsMgr_Run() parameter
  int waiting_for_event; // copy of the corresponding
                         // RtemsModelEventsMgr_Run() parameter
  int this_test_number; // test number used to identify a test runner instance
  rtems_id receiver_id; // receiver ID used for the event send action.
  rtems_event_set events_to_send; // events to send for the event send action
  rtems_status_code send_status; // status of the event send action.
  rtems_option receive_option_set; // option set used for the event receive action
  rtems_interval receive_timeout; // timeout used for the event receive action
  rtems_event_set received_events; // events received by the event receive action
  rtems_status_code receive_status; // status of the event receive action
  rtems_event_set unsatisfied_pending; // pending events after an event send action
                       // which did not satsify the event condition of the receiver
  Thread_Control *runner_thread; // TCB of the runner task
  rtems_id runner_id; // ID of the runner task
  rtems_id worker_id; // task ID of the worker task
  rtems_id worker_wakeup; // ID of the semaphore used to wake up the worker task
  rtems_id runner_wakeup; // ID of the semaphore used to wake up the runner task
  rtems_id runner_sched; // scheduler ID of scheduler used by the runner task
  rtems_id other_sched; // scheduler ID of another scheduler
                        // which is not used by the runner task
  T_thread_switch_log_4 thread_switch_log; // thread switch log
} RtemsModelEventsMgr_Context;

typedef enum {
  PRIO_HIGH = 1,
  PRIO_NORMAL,
  PRIO_LOW,
  PRIO_OTHER
} Priorities;

#define POWER_OF_10 100

#define WORKER_ATTRIBUTES RTEMS_DEFAULT_ATTRIBUTES

#define MAX_TLS_SIZE RTEMS_ALIGN_UP( 64, RTEMS_TASK_STORAGE_ALIGNMENT )

typedef RtemsModelEventsMgr_Context Context;

rtems_id CreateWakeupSema( void );

void DeleteWakeupSema( rtems_id id );

void Wait( rtems_id id );

void Wakeup( rtems_id id ) ;

rtems_event_set GetPending( Context *ctx );

rtems_option mergeopts( bool wait, bool wantall );

rtems_id mapid( Context *ctx, int pid ) ;

void checkTaskIs( rtems_id expected_id ) ;

void initialise_pending( rtems_event_set pending[], int max );

void ShowWorkerSemaId( Context *ctx ) ;

void ShowRunnerSemaId( Context *ctx ) ;

void initialise_semaphore( Context *ctx, rtems_id semaphore[] );

void RtemsModelEventsMgr_Setup_Wrap( void *arg ) ;


void RtemsModelEventsMgr_Teardown_Wrap( void *arg ) ;

size_t RtemsModelEventsMgr_Scope( void *arg, char *buf, size_t n ) ;

void RtemsModelEventsMgr_Cleanup( RtemsModelEventsMgr_Context *ctx );

/**
 * @brief Runs the parameterized test case.
 *
 * @param send is the event send handler.
 *
 * @param receive is the event receive handler.
 *
 * @param get_pending_events is the get pending events handler.
 *
 * @param wait_class is the thread wait class.
 *
 * @param waiting_for_event is the thread waiting for event state.
 */

void RtemsModelEventsMgr_Run0(
  rtems_status_code ( *send )( rtems_id, rtems_event_set ),
  rtems_status_code ( *receive )( rtems_event_set, rtems_option, rtems_interval, rtems_event_set * ),
  rtems_event_set (   *get_pending_events )( Thread_Control * ),
  unsigned int         wait_class,
  int                  waiting_for_event
);

void RtemsModelEventsMgr_Run1(
  rtems_status_code ( *send )( rtems_id, rtems_event_set ),
  rtems_status_code ( *receive )( rtems_event_set, rtems_option, rtems_interval, rtems_event_set * ),
  rtems_event_set (   *get_pending_events )( Thread_Control * ),
  unsigned int         wait_class,
  int                  waiting_for_event
);

void RtemsModelEventsMgr_Run2(
  rtems_status_code ( *send )( rtems_id, rtems_event_set ),
  rtems_status_code ( *receive )( rtems_event_set, rtems_option, rtems_interval, rtems_event_set * ),
  rtems_event_set (   *get_pending_events )( Thread_Control * ),
  unsigned int         wait_class,
  int                  waiting_for_event
);

void RtemsModelEventsMgr_Run3(
  rtems_status_code ( *send )( rtems_id, rtems_event_set ),
  rtems_status_code ( *receive )( rtems_event_set, rtems_option, rtems_interval, rtems_event_set * ),
  rtems_event_set (   *get_pending_events )( Thread_Control * ),
  unsigned int         wait_class,
  int                  waiting_for_event
);

void RtemsModelEventsMgr_Run4(
  rtems_status_code ( *send )( rtems_id, rtems_event_set ),
  rtems_status_code ( *receive )( rtems_event_set, rtems_option, rtems_interval, rtems_event_set * ),
  rtems_event_set (   *get_pending_events )( Thread_Control * ),
  unsigned int         wait_class,
  int                  waiting_for_event
);

void RtemsModelEventsMgr_Run5(
  rtems_status_code ( *send )( rtems_id, rtems_event_set ),
  rtems_status_code ( *receive )( rtems_event_set, rtems_option, rtems_interval, rtems_event_set * ),
  rtems_event_set (   *get_pending_events )( Thread_Control * ),
  unsigned int         wait_class,
  int                  waiting_for_event
);

void RtemsModelEventsMgr_Run6(
  rtems_status_code ( *send )( rtems_id, rtems_event_set ),
  rtems_status_code ( *receive )( rtems_event_set, rtems_option, rtems_interval, rtems_event_set * ),
  rtems_event_set (   *get_pending_events )( Thread_Control * ),
  unsigned int         wait_class,
  int                  waiting_for_event
);

void RtemsModelEventsMgr_Run7(
  rtems_status_code ( *send )( rtems_id, rtems_event_set ),
  rtems_status_code ( *receive )( rtems_event_set, rtems_option, rtems_interval, rtems_event_set * ),
  rtems_event_set (   *get_pending_events )( Thread_Control * ),
  unsigned int         wait_class,
  int                  waiting_for_event
);

void RtemsModelEventsMgr_Run8(
  rtems_status_code ( *send )( rtems_id, rtems_event_set ),
  rtems_status_code ( *receive )( rtems_event_set, rtems_option, rtems_interval, rtems_event_set * ),
  rtems_event_set (   *get_pending_events )( Thread_Control * ),
  unsigned int         wait_class,
  int                  waiting_for_event
);

#ifdef __cplusplus
}
#endif

#endif /* _TR_EVENT_SEND_RECEIVE_H */
