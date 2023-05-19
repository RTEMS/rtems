/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsStatusReqText
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

#include <rtems.h>

#include <rtems/test.h>

/**
 * @defgroup RtemsStatusReqText spec:/rtems/status/req/text
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsStatusReqText_Pre_Code_AlreadySuspended,
  RtemsStatusReqText_Pre_Code_CalledFromIsr,
  RtemsStatusReqText_Pre_Code_IllegalOnRemoteObject,
  RtemsStatusReqText_Pre_Code_IllegalOnSelf,
  RtemsStatusReqText_Pre_Code_IncorrectState,
  RtemsStatusReqText_Pre_Code_InternalError,
  RtemsStatusReqText_Pre_Code_Interrupted,
  RtemsStatusReqText_Pre_Code_InvalidAddress,
  RtemsStatusReqText_Pre_Code_InvalidClock,
  RtemsStatusReqText_Pre_Code_InvalidId,
  RtemsStatusReqText_Pre_Code_InvalidName,
  RtemsStatusReqText_Pre_Code_InvalidNode,
  RtemsStatusReqText_Pre_Code_InvalidNumber,
  RtemsStatusReqText_Pre_Code_InvalidPriority,
  RtemsStatusReqText_Pre_Code_InvalidSize,
  RtemsStatusReqText_Pre_Code_IoError,
  RtemsStatusReqText_Pre_Code_MpNotConfigured,
  RtemsStatusReqText_Pre_Code_NoMemory,
  RtemsStatusReqText_Pre_Code_NotConfigured,
  RtemsStatusReqText_Pre_Code_NotDefined,
  RtemsStatusReqText_Pre_Code_NotImplemented,
  RtemsStatusReqText_Pre_Code_NotOwnerOfResource,
  RtemsStatusReqText_Pre_Code_ObjectWasDeleted,
  RtemsStatusReqText_Pre_Code_ProxyBlocking,
  RtemsStatusReqText_Pre_Code_ResourceInUse,
  RtemsStatusReqText_Pre_Code_Successful,
  RtemsStatusReqText_Pre_Code_TaskExitted,
  RtemsStatusReqText_Pre_Code_Timeout,
  RtemsStatusReqText_Pre_Code_TooMany,
  RtemsStatusReqText_Pre_Code_Unsatisfied,
  RtemsStatusReqText_Pre_Code_NotAStatusCode,
  RtemsStatusReqText_Pre_Code_NA
} RtemsStatusReqText_Pre_Code;

typedef enum {
  RtemsStatusReqText_Post_Result_AlreadySuspended,
  RtemsStatusReqText_Post_Result_CalledFromIsr,
  RtemsStatusReqText_Post_Result_IllegalOnRemoteObject,
  RtemsStatusReqText_Post_Result_IllegalOnSelf,
  RtemsStatusReqText_Post_Result_IncorrectState,
  RtemsStatusReqText_Post_Result_InternalError,
  RtemsStatusReqText_Post_Result_Interrupted,
  RtemsStatusReqText_Post_Result_InvalidAddress,
  RtemsStatusReqText_Post_Result_InvalidClock,
  RtemsStatusReqText_Post_Result_InvalidId,
  RtemsStatusReqText_Post_Result_InvalidName,
  RtemsStatusReqText_Post_Result_InvalidNode,
  RtemsStatusReqText_Post_Result_InvalidNumber,
  RtemsStatusReqText_Post_Result_InvalidPriority,
  RtemsStatusReqText_Post_Result_InvalidSize,
  RtemsStatusReqText_Post_Result_IoError,
  RtemsStatusReqText_Post_Result_MpNotConfigured,
  RtemsStatusReqText_Post_Result_NoMemory,
  RtemsStatusReqText_Post_Result_NotConfigured,
  RtemsStatusReqText_Post_Result_NotDefined,
  RtemsStatusReqText_Post_Result_NotImplemented,
  RtemsStatusReqText_Post_Result_NotOwnerOfResource,
  RtemsStatusReqText_Post_Result_ObjectWasDeleted,
  RtemsStatusReqText_Post_Result_ProxyBlocking,
  RtemsStatusReqText_Post_Result_ResourceInUse,
  RtemsStatusReqText_Post_Result_Successful,
  RtemsStatusReqText_Post_Result_TaskExitted,
  RtemsStatusReqText_Post_Result_Timeout,
  RtemsStatusReqText_Post_Result_TooMany,
  RtemsStatusReqText_Post_Result_Unsatisfied,
  RtemsStatusReqText_Post_Result_NotAStatusCode,
  RtemsStatusReqText_Post_Result_NA
} RtemsStatusReqText_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Code_NA : 1;
  uint8_t Post_Result : 5;
} RtemsStatusReqText_Entry;

/**
 * @brief Test context for spec:/rtems/status/req/text test case.
 */
typedef struct {
  /**
   * @brief This member specifies the ``status_code`` parameter value.
   */
  rtems_status_code code;

  /**
   * @brief This member contains the return value of the rtems_status_text()
   *   call.
   */
  const char *result;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 1 ];

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
    RtemsStatusReqText_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsStatusReqText_Context;

static RtemsStatusReqText_Context
  RtemsStatusReqText_Instance;

static const char * const RtemsStatusReqText_PreDesc_Code[] = {
  "AlreadySuspended",
  "CalledFromIsr",
  "IllegalOnRemoteObject",
  "IllegalOnSelf",
  "IncorrectState",
  "InternalError",
  "Interrupted",
  "InvalidAddress",
  "InvalidClock",
  "InvalidId",
  "InvalidName",
  "InvalidNode",
  "InvalidNumber",
  "InvalidPriority",
  "InvalidSize",
  "IoError",
  "MpNotConfigured",
  "NoMemory",
  "NotConfigured",
  "NotDefined",
  "NotImplemented",
  "NotOwnerOfResource",
  "ObjectWasDeleted",
  "ProxyBlocking",
  "ResourceInUse",
  "Successful",
  "TaskExitted",
  "Timeout",
  "TooMany",
  "Unsatisfied",
  "NotAStatusCode",
  "NA"
};

static const char * const * const RtemsStatusReqText_PreDesc[] = {
  RtemsStatusReqText_PreDesc_Code,
  NULL
};

static void RtemsStatusReqText_Pre_Code_Prepare(
  RtemsStatusReqText_Context *ctx,
  RtemsStatusReqText_Pre_Code state
)
{
  switch ( state ) {
    case RtemsStatusReqText_Pre_Code_AlreadySuspended: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_ALREADY_SUSPENDED.
       */
      ctx->code = RTEMS_ALREADY_SUSPENDED;
      break;
    }

    case RtemsStatusReqText_Pre_Code_CalledFromIsr: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_CALLED_FROM_ISR.
       */
      ctx->code = RTEMS_CALLED_FROM_ISR;
      break;
    }

    case RtemsStatusReqText_Pre_Code_IllegalOnRemoteObject: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_ILLEGAL_ON_REMOTE_OBJECT.
       */

      break;
    }

    case RtemsStatusReqText_Pre_Code_IllegalOnSelf: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_ILLEGAL_ON_SELF.
       */
      ctx->code = RTEMS_ILLEGAL_ON_SELF;
      break;
    }

    case RtemsStatusReqText_Pre_Code_IncorrectState: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_INCORRECT_STATE.
       */
      ctx->code = RTEMS_INCORRECT_STATE;
      break;
    }

    case RtemsStatusReqText_Pre_Code_InternalError: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_INTERNAL_ERROR.
       */
      ctx->code = RTEMS_INTERNAL_ERROR;
      break;
    }

    case RtemsStatusReqText_Pre_Code_Interrupted: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_INTERRUPTED.
       */
      ctx->code = RTEMS_INTERRUPTED;
      break;
    }

    case RtemsStatusReqText_Pre_Code_InvalidAddress: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_INVALID_ADDRESS.
       */
      ctx->code = RTEMS_INVALID_ADDRESS;
      break;
    }

    case RtemsStatusReqText_Pre_Code_InvalidClock: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_INVALID_CLOCK.
       */
      ctx->code = RTEMS_INVALID_CLOCK;
      break;
    }

    case RtemsStatusReqText_Pre_Code_InvalidId: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_INVALID_ID.
       */
      ctx->code = RTEMS_INVALID_ID;
      break;
    }

    case RtemsStatusReqText_Pre_Code_InvalidName: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_INVALID_NAME.
       */
      ctx->code = RTEMS_INVALID_NAME;
      break;
    }

    case RtemsStatusReqText_Pre_Code_InvalidNode: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_INVALID_NODE.
       */
      ctx->code = RTEMS_INVALID_NODE;
      break;
    }

    case RtemsStatusReqText_Pre_Code_InvalidNumber: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_INVALID_NUMBER.
       */
      ctx->code = RTEMS_INVALID_NUMBER;
      break;
    }

    case RtemsStatusReqText_Pre_Code_InvalidPriority: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_INVALID_PRIORITY.
       */
      ctx->code = RTEMS_INVALID_PRIORITY;
      break;
    }

    case RtemsStatusReqText_Pre_Code_InvalidSize: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_INVALID_SIZE.
       */
      ctx->code = RTEMS_INVALID_SIZE;
      break;
    }

    case RtemsStatusReqText_Pre_Code_IoError: {
      /*
       * While the ``status_code`` parameter value is equal to RTEMS_IO_ERROR.
       */
      ctx->code = RTEMS_IO_ERROR;
      break;
    }

    case RtemsStatusReqText_Pre_Code_MpNotConfigured: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_MP_NOT_CONFIGURED.
       */
      ctx->code = RTEMS_MP_NOT_CONFIGURED;
      break;
    }

    case RtemsStatusReqText_Pre_Code_NoMemory: {
      /*
       * While the ``status_code`` parameter value is equal to RTEMS_NO_MEMORY.
       */
      ctx->code = RTEMS_NO_MEMORY;
      break;
    }

    case RtemsStatusReqText_Pre_Code_NotConfigured: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_NOT_CONFIGURED.
       */
      ctx->code = RTEMS_NOT_CONFIGURED;
      break;
    }

    case RtemsStatusReqText_Pre_Code_NotDefined: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_NOT_DEFINED.
       */
      ctx->code = RTEMS_NOT_DEFINED;
      break;
    }

    case RtemsStatusReqText_Pre_Code_NotImplemented: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_NOT_IMPLEMENTED.
       */
      ctx->code = RTEMS_NOT_IMPLEMENTED;
      break;
    }

    case RtemsStatusReqText_Pre_Code_NotOwnerOfResource: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_NOT_OWNER_OF_RESOURCE.
       */
      ctx->code = RTEMS_NOT_OWNER_OF_RESOURCE;
      break;
    }

    case RtemsStatusReqText_Pre_Code_ObjectWasDeleted: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_OBJECT_WAS_DELETED.
       */
      ctx->code = RTEMS_OBJECT_WAS_DELETED;
      break;
    }

    case RtemsStatusReqText_Pre_Code_ProxyBlocking: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_PROXY_BLOCKING.
       */
      ctx->code = RTEMS_PROXY_BLOCKING;
      break;
    }

    case RtemsStatusReqText_Pre_Code_ResourceInUse: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_RESOURCE_IN_USE.
       */
      ctx->code = RTEMS_RESOURCE_IN_USE;
      break;
    }

    case RtemsStatusReqText_Pre_Code_Successful: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_SUCCESSFUL.
       */
      ctx->code = RTEMS_SUCCESSFUL;
      break;
    }

    case RtemsStatusReqText_Pre_Code_TaskExitted: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_TASK_EXITTED.
       */
      ctx->code = RTEMS_TASK_EXITTED;
      break;
    }

    case RtemsStatusReqText_Pre_Code_Timeout: {
      /*
       * While the ``status_code`` parameter value is equal to RTEMS_TIMEOUT.
       */
      ctx->code = RTEMS_TIMEOUT;
      break;
    }

    case RtemsStatusReqText_Pre_Code_TooMany: {
      /*
       * While the ``status_code`` parameter value is equal to RTEMS_TOO_MANY.
       */
      ctx->code = RTEMS_TOO_MANY;
      break;
    }

    case RtemsStatusReqText_Pre_Code_Unsatisfied: {
      /*
       * While the ``status_code`` parameter value is equal to
       * RTEMS_UNSATISFIED.
       */
      ctx->code = RTEMS_UNSATISFIED;
      break;
    }

    case RtemsStatusReqText_Pre_Code_NotAStatusCode: {
      /*
       * While the ``status_code`` parameter is not equal to an enumerator of
       * rtems_status_code.
       */
      ctx->code = (rtems_status_code) 123;
      break;
    }

    case RtemsStatusReqText_Pre_Code_NA:
      break;
  }
}

static void RtemsStatusReqText_Post_Result_Check(
  RtemsStatusReqText_Context    *ctx,
  RtemsStatusReqText_Post_Result state
)
{
  switch ( state ) {
    case RtemsStatusReqText_Post_Result_AlreadySuspended: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_ALREADY_SUSPENDED".
       */
      T_eq_str( ctx->result, "RTEMS_ALREADY_SUSPENDED" );
      break;
    }

    case RtemsStatusReqText_Post_Result_CalledFromIsr: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_CALLED_FROM_ISR".
       */
      T_eq_str( ctx->result, "RTEMS_CALLED_FROM_ISR" );
      break;
    }

    case RtemsStatusReqText_Post_Result_IllegalOnRemoteObject: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_ILLEGAL_ON_REMOTE_OBJECT".
       */

      break;
    }

    case RtemsStatusReqText_Post_Result_IllegalOnSelf: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_ILLEGAL_ON_SELF".
       */
      T_eq_str( ctx->result, "RTEMS_ILLEGAL_ON_SELF" );
      break;
    }

    case RtemsStatusReqText_Post_Result_IncorrectState: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_INCORRECT_STATE".
       */
      T_eq_str( ctx->result, "RTEMS_INCORRECT_STATE" );
      break;
    }

    case RtemsStatusReqText_Post_Result_InternalError: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_INTERNAL_ERROR".
       */
      T_eq_str( ctx->result, "RTEMS_INTERNAL_ERROR" );
      break;
    }

    case RtemsStatusReqText_Post_Result_Interrupted: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_INTERRUPTED".
       */
      T_eq_str( ctx->result, "RTEMS_INTERRUPTED" );
      break;
    }

    case RtemsStatusReqText_Post_Result_InvalidAddress: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_INVALID_ADDRESS".
       */
      T_eq_str( ctx->result, "RTEMS_INVALID_ADDRESS" );
      break;
    }

    case RtemsStatusReqText_Post_Result_InvalidClock: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_INVALID_CLOCK".
       */
      T_eq_str( ctx->result, "RTEMS_INVALID_CLOCK" );
      break;
    }

    case RtemsStatusReqText_Post_Result_InvalidId: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_INVALID_ID".
       */
      T_eq_str( ctx->result, "RTEMS_INVALID_ID" );
      break;
    }

    case RtemsStatusReqText_Post_Result_InvalidName: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_INVALID_NAME".
       */
      T_eq_str( ctx->result, "RTEMS_INVALID_NAME" );
      break;
    }

    case RtemsStatusReqText_Post_Result_InvalidNode: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_INVALID_NODE".
       */
      T_eq_str( ctx->result, "RTEMS_INVALID_NODE" );
      break;
    }

    case RtemsStatusReqText_Post_Result_InvalidNumber: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_INVALID_NUMBER".
       */
      T_eq_str( ctx->result, "RTEMS_INVALID_NUMBER" );
      break;
    }

    case RtemsStatusReqText_Post_Result_InvalidPriority: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_INVALID_PRIORITY".
       */
      T_eq_str( ctx->result, "RTEMS_INVALID_PRIORITY" );
      break;
    }

    case RtemsStatusReqText_Post_Result_InvalidSize: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_INVALID_SIZE".
       */
      T_eq_str( ctx->result, "RTEMS_INVALID_SIZE" );
      break;
    }

    case RtemsStatusReqText_Post_Result_IoError: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_IO_ERROR".
       */
      T_eq_str( ctx->result, "RTEMS_IO_ERROR" );
      break;
    }

    case RtemsStatusReqText_Post_Result_MpNotConfigured: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_MP_NOT_CONFIGURED".
       */
      T_eq_str( ctx->result, "RTEMS_MP_NOT_CONFIGURED" );
      break;
    }

    case RtemsStatusReqText_Post_Result_NoMemory: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_NO_MEMORY".
       */
      T_eq_str( ctx->result, "RTEMS_NO_MEMORY" );
      break;
    }

    case RtemsStatusReqText_Post_Result_NotConfigured: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_NOT_CONFIGURED".
       */
      T_eq_str( ctx->result, "RTEMS_NOT_CONFIGURED" );
      break;
    }

    case RtemsStatusReqText_Post_Result_NotDefined: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_NOT_DEFINED".
       */
      T_eq_str( ctx->result, "RTEMS_NOT_DEFINED" );
      break;
    }

    case RtemsStatusReqText_Post_Result_NotImplemented: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_NOT_IMPLEMENTED".
       */
      T_eq_str( ctx->result, "RTEMS_NOT_IMPLEMENTED" );
      break;
    }

    case RtemsStatusReqText_Post_Result_NotOwnerOfResource: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_NOT_OWNER_OF_RESOURCE".
       */
      T_eq_str( ctx->result, "RTEMS_NOT_OWNER_OF_RESOURCE" );
      break;
    }

    case RtemsStatusReqText_Post_Result_ObjectWasDeleted: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_OBJECT_WAS_DELETED".
       */
      T_eq_str( ctx->result, "RTEMS_OBJECT_WAS_DELETED" );
      break;
    }

    case RtemsStatusReqText_Post_Result_ProxyBlocking: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_PROXY_BLOCKING".
       */
      T_eq_str( ctx->result, "RTEMS_PROXY_BLOCKING" );
      break;
    }

    case RtemsStatusReqText_Post_Result_ResourceInUse: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_RESOURCE_IN_USE".
       */
      T_eq_str( ctx->result, "RTEMS_RESOURCE_IN_USE" );
      break;
    }

    case RtemsStatusReqText_Post_Result_Successful: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_SUCCESSFUL".
       */
      T_eq_str( ctx->result, "RTEMS_SUCCESSFUL" );
      break;
    }

    case RtemsStatusReqText_Post_Result_TaskExitted: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_TASK_EXITTED".
       */
      T_eq_str( ctx->result, "RTEMS_TASK_EXITTED" );
      break;
    }

    case RtemsStatusReqText_Post_Result_Timeout: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_TIMEOUT".
       */
      T_eq_str( ctx->result, "RTEMS_TIMEOUT" );
      break;
    }

    case RtemsStatusReqText_Post_Result_TooMany: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_TOO_MANY".
       */
      T_eq_str( ctx->result, "RTEMS_TOO_MANY" );
      break;
    }

    case RtemsStatusReqText_Post_Result_Unsatisfied: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "RTEMS_UNSATISFIED".
       */
      T_eq_str( ctx->result, "RTEMS_UNSATISFIED" );
      break;
    }

    case RtemsStatusReqText_Post_Result_NotAStatusCode: {
      /*
       * The return value of rtems_status_text() shall reference a string which
       * is equal to "?".
       */
      T_eq_str( ctx->result, "?" );
      break;
    }

    case RtemsStatusReqText_Post_Result_NA:
      break;
  }
}

static void RtemsStatusReqText_Action( RtemsStatusReqText_Context *ctx )
{
  ctx->result = rtems_status_text( ctx->code );
}

static const RtemsStatusReqText_Entry
RtemsStatusReqText_Entries[] = {
  { 0, 0, RtemsStatusReqText_Post_Result_AlreadySuspended },
  { 0, 0, RtemsStatusReqText_Post_Result_CalledFromIsr },
  { 0, 0, RtemsStatusReqText_Post_Result_IllegalOnRemoteObject },
  { 0, 0, RtemsStatusReqText_Post_Result_IllegalOnSelf },
  { 0, 0, RtemsStatusReqText_Post_Result_IncorrectState },
  { 0, 0, RtemsStatusReqText_Post_Result_InternalError },
  { 0, 0, RtemsStatusReqText_Post_Result_Interrupted },
  { 0, 0, RtemsStatusReqText_Post_Result_InvalidAddress },
  { 0, 0, RtemsStatusReqText_Post_Result_InvalidClock },
  { 0, 0, RtemsStatusReqText_Post_Result_InvalidId },
  { 0, 0, RtemsStatusReqText_Post_Result_InvalidName },
  { 0, 0, RtemsStatusReqText_Post_Result_InvalidNode },
  { 0, 0, RtemsStatusReqText_Post_Result_InvalidNumber },
  { 0, 0, RtemsStatusReqText_Post_Result_InvalidPriority },
  { 0, 0, RtemsStatusReqText_Post_Result_InvalidSize },
  { 0, 0, RtemsStatusReqText_Post_Result_IoError },
  { 0, 0, RtemsStatusReqText_Post_Result_MpNotConfigured },
  { 0, 0, RtemsStatusReqText_Post_Result_NoMemory },
  { 0, 0, RtemsStatusReqText_Post_Result_NotConfigured },
  { 0, 0, RtemsStatusReqText_Post_Result_NotDefined },
  { 0, 0, RtemsStatusReqText_Post_Result_NotImplemented },
  { 0, 0, RtemsStatusReqText_Post_Result_NotOwnerOfResource },
  { 0, 0, RtemsStatusReqText_Post_Result_ObjectWasDeleted },
  { 0, 0, RtemsStatusReqText_Post_Result_ProxyBlocking },
  { 0, 0, RtemsStatusReqText_Post_Result_ResourceInUse },
  { 0, 0, RtemsStatusReqText_Post_Result_Successful },
  { 0, 0, RtemsStatusReqText_Post_Result_TaskExitted },
  { 0, 0, RtemsStatusReqText_Post_Result_Timeout },
  { 0, 0, RtemsStatusReqText_Post_Result_TooMany },
  { 0, 0, RtemsStatusReqText_Post_Result_Unsatisfied },
  { 0, 0, RtemsStatusReqText_Post_Result_NotAStatusCode }
};

static const uint8_t
RtemsStatusReqText_Map[] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
  22, 23, 24, 25, 26, 27, 28, 29, 30
};

static size_t RtemsStatusReqText_Scope( void *arg, char *buf, size_t n )
{
  RtemsStatusReqText_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsStatusReqText_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsStatusReqText_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsStatusReqText_Scope,
  .initial_context = &RtemsStatusReqText_Instance
};

static inline RtemsStatusReqText_Entry RtemsStatusReqText_PopEntry(
  RtemsStatusReqText_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsStatusReqText_Entries[
    RtemsStatusReqText_Map[ index ]
  ];
}

static void RtemsStatusReqText_TestVariant( RtemsStatusReqText_Context *ctx )
{
  RtemsStatusReqText_Pre_Code_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsStatusReqText_Action( ctx );
  RtemsStatusReqText_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_RtemsStatusReqText( void )
 */
T_TEST_CASE_FIXTURE( RtemsStatusReqText, &RtemsStatusReqText_Fixture )
{
  RtemsStatusReqText_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsStatusReqText_Pre_Code_AlreadySuspended;
    ctx->Map.pcs[ 0 ] < RtemsStatusReqText_Pre_Code_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = RtemsStatusReqText_PopEntry( ctx );
    RtemsStatusReqText_TestVariant( ctx );
  }
}

/** @} */
