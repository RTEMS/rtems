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

#ifndef _TR_EVENT_SEND_RECEIVE_H
#define _TR_EVENT_SEND_RECEIVE_H

#include <rtems.h>
#include <rtems/score/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RtemsEventReqSendReceive
 *
 * @{
 */

typedef enum {
  RtemsEventReqSendReceive_Pre_Id_InvId,
  RtemsEventReqSendReceive_Pre_Id_Task,
  RtemsEventReqSendReceive_Pre_Id_NA
} RtemsEventReqSendReceive_Pre_Id;

typedef enum {
  RtemsEventReqSendReceive_Pre_Send_Zero,
  RtemsEventReqSendReceive_Pre_Send_Unrelated,
  RtemsEventReqSendReceive_Pre_Send_Any,
  RtemsEventReqSendReceive_Pre_Send_All,
  RtemsEventReqSendReceive_Pre_Send_MixedAny,
  RtemsEventReqSendReceive_Pre_Send_MixedAll,
  RtemsEventReqSendReceive_Pre_Send_NA
} RtemsEventReqSendReceive_Pre_Send;

typedef enum {
  RtemsEventReqSendReceive_Pre_ReceiverState_InvAddr,
  RtemsEventReqSendReceive_Pre_ReceiverState_NotWaiting,
  RtemsEventReqSendReceive_Pre_ReceiverState_Poll,
  RtemsEventReqSendReceive_Pre_ReceiverState_Timeout,
  RtemsEventReqSendReceive_Pre_ReceiverState_Lower,
  RtemsEventReqSendReceive_Pre_ReceiverState_Equal,
  RtemsEventReqSendReceive_Pre_ReceiverState_Higher,
  RtemsEventReqSendReceive_Pre_ReceiverState_Other,
  RtemsEventReqSendReceive_Pre_ReceiverState_Intend,
  RtemsEventReqSendReceive_Pre_ReceiverState_NA
} RtemsEventReqSendReceive_Pre_ReceiverState;

typedef enum {
  RtemsEventReqSendReceive_Pre_Satisfy_All,
  RtemsEventReqSendReceive_Pre_Satisfy_Any,
  RtemsEventReqSendReceive_Pre_Satisfy_NA
} RtemsEventReqSendReceive_Pre_Satisfy;

typedef enum {
  RtemsEventReqSendReceive_Post_SendStatus_Ok,
  RtemsEventReqSendReceive_Post_SendStatus_InvId,
  RtemsEventReqSendReceive_Post_SendStatus_NA
} RtemsEventReqSendReceive_Post_SendStatus;

typedef enum {
  RtemsEventReqSendReceive_Post_ReceiveStatus_None,
  RtemsEventReqSendReceive_Post_ReceiveStatus_Pending,
  RtemsEventReqSendReceive_Post_ReceiveStatus_Timeout,
  RtemsEventReqSendReceive_Post_ReceiveStatus_Satisfied,
  RtemsEventReqSendReceive_Post_ReceiveStatus_Unsatisfied,
  RtemsEventReqSendReceive_Post_ReceiveStatus_Blocked,
  RtemsEventReqSendReceive_Post_ReceiveStatus_InvAddr,
  RtemsEventReqSendReceive_Post_ReceiveStatus_NA
} RtemsEventReqSendReceive_Post_ReceiveStatus;

typedef enum {
  RtemsEventReqSendReceive_Post_SenderPreemption_No,
  RtemsEventReqSendReceive_Post_SenderPreemption_Yes,
  RtemsEventReqSendReceive_Post_SenderPreemption_NA
} RtemsEventReqSendReceive_Post_SenderPreemption;

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
void RtemsEventReqSendReceive_Run(
  rtems_status_code ( *send )( rtems_id, rtems_event_set ),
  rtems_status_code ( *receive )( rtems_event_set, rtems_option, rtems_interval, rtems_event_set * ),
  rtems_event_set (   *get_pending_events )( Thread_Control * ),
  unsigned int         wait_class,
  int                  waiting_for_event
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_EVENT_SEND_RECEIVE_H */
