/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSemReqCreate
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

#include <limits.h>
#include <rtems.h>
#include <string.h>
#include <rtems/rtems/semimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsSemReqCreate spec:/rtems/sem/req/create
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsSemReqCreate_Pre_Name_Valid,
  RtemsSemReqCreate_Pre_Name_Invalid,
  RtemsSemReqCreate_Pre_Name_NA
} RtemsSemReqCreate_Pre_Name;

typedef enum {
  RtemsSemReqCreate_Pre_Id_Valid,
  RtemsSemReqCreate_Pre_Id_Null,
  RtemsSemReqCreate_Pre_Id_NA
} RtemsSemReqCreate_Pre_Id;

typedef enum {
  RtemsSemReqCreate_Pre_Count_Zero,
  RtemsSemReqCreate_Pre_Count_One,
  RtemsSemReqCreate_Pre_Count_GtOne,
  RtemsSemReqCreate_Pre_Count_NA
} RtemsSemReqCreate_Pre_Count;

typedef enum {
  RtemsSemReqCreate_Pre_Binary_Yes,
  RtemsSemReqCreate_Pre_Binary_No,
  RtemsSemReqCreate_Pre_Binary_NA
} RtemsSemReqCreate_Pre_Binary;

typedef enum {
  RtemsSemReqCreate_Pre_Simple_Yes,
  RtemsSemReqCreate_Pre_Simple_No,
  RtemsSemReqCreate_Pre_Simple_NA
} RtemsSemReqCreate_Pre_Simple;

typedef enum {
  RtemsSemReqCreate_Pre_Inherit_Yes,
  RtemsSemReqCreate_Pre_Inherit_No,
  RtemsSemReqCreate_Pre_Inherit_NA
} RtemsSemReqCreate_Pre_Inherit;

typedef enum {
  RtemsSemReqCreate_Pre_Ceiling_Yes,
  RtemsSemReqCreate_Pre_Ceiling_No,
  RtemsSemReqCreate_Pre_Ceiling_NA
} RtemsSemReqCreate_Pre_Ceiling;

typedef enum {
  RtemsSemReqCreate_Pre_MrsP_Yes,
  RtemsSemReqCreate_Pre_MrsP_No,
  RtemsSemReqCreate_Pre_MrsP_NA
} RtemsSemReqCreate_Pre_MrsP;

typedef enum {
  RtemsSemReqCreate_Pre_Disc_FIFO,
  RtemsSemReqCreate_Pre_Disc_Prio,
  RtemsSemReqCreate_Pre_Disc_NA
} RtemsSemReqCreate_Pre_Disc;

typedef enum {
  RtemsSemReqCreate_Pre_Prio_LeCur,
  RtemsSemReqCreate_Pre_Prio_GtCur,
  RtemsSemReqCreate_Pre_Prio_Invalid,
  RtemsSemReqCreate_Pre_Prio_NA
} RtemsSemReqCreate_Pre_Prio;

typedef enum {
  RtemsSemReqCreate_Pre_Free_Yes,
  RtemsSemReqCreate_Pre_Free_No,
  RtemsSemReqCreate_Pre_Free_NA
} RtemsSemReqCreate_Pre_Free;

typedef enum {
  RtemsSemReqCreate_Post_Status_Ok,
  RtemsSemReqCreate_Post_Status_InvName,
  RtemsSemReqCreate_Post_Status_InvAddr,
  RtemsSemReqCreate_Post_Status_InvNum,
  RtemsSemReqCreate_Post_Status_InvPrio,
  RtemsSemReqCreate_Post_Status_NotDef,
  RtemsSemReqCreate_Post_Status_TooMany,
  RtemsSemReqCreate_Post_Status_NA
} RtemsSemReqCreate_Post_Status;

typedef enum {
  RtemsSemReqCreate_Post_Name_Valid,
  RtemsSemReqCreate_Post_Name_Invalid,
  RtemsSemReqCreate_Post_Name_NA
} RtemsSemReqCreate_Post_Name;

typedef enum {
  RtemsSemReqCreate_Post_IdVar_Set,
  RtemsSemReqCreate_Post_IdVar_Nop,
  RtemsSemReqCreate_Post_IdVar_NA
} RtemsSemReqCreate_Post_IdVar;

typedef enum {
  RtemsSemReqCreate_Post_Variant_Cnt,
  RtemsSemReqCreate_Post_Variant_Bin,
  RtemsSemReqCreate_Post_Variant_PI,
  RtemsSemReqCreate_Post_Variant_PC,
  RtemsSemReqCreate_Post_Variant_SB,
  RtemsSemReqCreate_Post_Variant_MrsP,
  RtemsSemReqCreate_Post_Variant_NA
} RtemsSemReqCreate_Post_Variant;

typedef enum {
  RtemsSemReqCreate_Post_Disc_FIFO,
  RtemsSemReqCreate_Post_Disc_Prio,
  RtemsSemReqCreate_Post_Disc_NA
} RtemsSemReqCreate_Post_Disc;

typedef enum {
  RtemsSemReqCreate_Post_Count_Initial,
  RtemsSemReqCreate_Post_Count_NA
} RtemsSemReqCreate_Post_Count;

typedef enum {
  RtemsSemReqCreate_Post_Owner_Caller,
  RtemsSemReqCreate_Post_Owner_No,
  RtemsSemReqCreate_Post_Owner_NA
} RtemsSemReqCreate_Post_Owner;

typedef enum {
  RtemsSemReqCreate_Post_Prio_Ceiling,
  RtemsSemReqCreate_Post_Prio_Nop,
  RtemsSemReqCreate_Post_Prio_NA
} RtemsSemReqCreate_Post_Prio;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Name_NA : 1;
  uint32_t Pre_Id_NA : 1;
  uint32_t Pre_Count_NA : 1;
  uint32_t Pre_Binary_NA : 1;
  uint32_t Pre_Simple_NA : 1;
  uint32_t Pre_Inherit_NA : 1;
  uint32_t Pre_Ceiling_NA : 1;
  uint32_t Pre_MrsP_NA : 1;
  uint32_t Pre_Disc_NA : 1;
  uint32_t Pre_Prio_NA : 1;
  uint32_t Pre_Free_NA : 1;
  uint32_t Post_Status : 3;
  uint32_t Post_Name : 2;
  uint32_t Post_IdVar : 2;
  uint32_t Post_Variant : 3;
  uint32_t Post_Disc : 2;
  uint32_t Post_Count : 1;
  uint32_t Post_Owner : 2;
  uint32_t Post_Prio : 2;
} RtemsSemReqCreate_Entry;

/**
 * @brief Test context for spec:/rtems/sem/req/create test case.
 */
typedef struct {
  void *seized_objects;

  rtems_status_code status;

  Semaphore_Variant variant;

  Semaphore_Discipline discipline;

  uint32_t sem_count;

  Thread_Control *executing;

  Thread_Control *owner;

  rtems_name name;

  uint32_t count;

  rtems_attribute attribute_set;

  rtems_task_priority priority_ceiling;

  rtems_id *id;

  rtems_id id_value;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 11 ];

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
    RtemsSemReqCreate_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsSemReqCreate_Context;

static RtemsSemReqCreate_Context
  RtemsSemReqCreate_Instance;

static const char * const RtemsSemReqCreate_PreDesc_Name[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsSemReqCreate_PreDesc_Id[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsSemReqCreate_PreDesc_Count[] = {
  "Zero",
  "One",
  "GtOne",
  "NA"
};

static const char * const RtemsSemReqCreate_PreDesc_Binary[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsSemReqCreate_PreDesc_Simple[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsSemReqCreate_PreDesc_Inherit[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsSemReqCreate_PreDesc_Ceiling[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsSemReqCreate_PreDesc_MrsP[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsSemReqCreate_PreDesc_Disc[] = {
  "FIFO",
  "Prio",
  "NA"
};

static const char * const RtemsSemReqCreate_PreDesc_Prio[] = {
  "LeCur",
  "GtCur",
  "Invalid",
  "NA"
};

static const char * const RtemsSemReqCreate_PreDesc_Free[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsSemReqCreate_PreDesc[] = {
  RtemsSemReqCreate_PreDesc_Name,
  RtemsSemReqCreate_PreDesc_Id,
  RtemsSemReqCreate_PreDesc_Count,
  RtemsSemReqCreate_PreDesc_Binary,
  RtemsSemReqCreate_PreDesc_Simple,
  RtemsSemReqCreate_PreDesc_Inherit,
  RtemsSemReqCreate_PreDesc_Ceiling,
  RtemsSemReqCreate_PreDesc_MrsP,
  RtemsSemReqCreate_PreDesc_Disc,
  RtemsSemReqCreate_PreDesc_Prio,
  RtemsSemReqCreate_PreDesc_Free,
  NULL
};

#define NAME rtems_build_name( 'T', 'E', 'S', 'T' )

typedef RtemsSemReqCreate_Context Context;

static rtems_status_code Create( void *arg, uint32_t *id )
{
  (void) arg;

  return rtems_semaphore_create(
    rtems_build_name( 'S', 'I', 'Z', 'E' ),
    1,
    RTEMS_DEFAULT_ATTRIBUTES,
    0,
    id
  );
}

static void GetSemAttributes( Context *ctx )
{
  if ( ctx->id_value != INVALID_ID ) {
    Semaphore_Control   *semaphore;
    Thread_queue_Context queue_context;
    uintptr_t            flags;

    semaphore = _Semaphore_Get( ctx->id_value, &queue_context );
    T_assert_not_null( semaphore );
    ctx->sem_count = semaphore->Core_control.Semaphore.count;
    ctx->owner = semaphore->Core_control.Wait_queue.Queue.owner;
    flags = _Semaphore_Get_flags( semaphore );
    _ISR_lock_ISR_enable( &queue_context.Lock_context.Lock_context );
    ctx->variant = _Semaphore_Get_variant( flags );
    ctx->discipline = _Semaphore_Get_discipline( flags );
  } else {
    ctx->sem_count = 123;
    ctx->owner = (void *)(uintptr_t) 1;
    ctx->variant = INT_MAX;
    ctx->discipline = INT_MAX;
  }
}

static void RtemsSemReqCreate_Pre_Name_Prepare(
  RtemsSemReqCreate_Context *ctx,
  RtemsSemReqCreate_Pre_Name state
)
{
  switch ( state ) {
    case RtemsSemReqCreate_Pre_Name_Valid: {
      /*
       * While the ``name`` parameter is valid.
       */
      ctx->name = NAME;
      break;
    }

    case RtemsSemReqCreate_Pre_Name_Invalid: {
      /*
       * While the ``name`` parameter is invalid.
       */
      ctx->name = 0;
      break;
    }

    case RtemsSemReqCreate_Pre_Name_NA:
      break;
  }
}

static void RtemsSemReqCreate_Pre_Id_Prepare(
  RtemsSemReqCreate_Context *ctx,
  RtemsSemReqCreate_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsSemReqCreate_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter references an object of type rtems_id.
       */
      ctx->id = &ctx->id_value;
      break;
    }

    case RtemsSemReqCreate_Pre_Id_Null: {
      /*
       * While the ``id`` parameter is NULL.
       */
      ctx->id = NULL;
      break;
    }

    case RtemsSemReqCreate_Pre_Id_NA:
      break;
  }
}

static void RtemsSemReqCreate_Pre_Count_Prepare(
  RtemsSemReqCreate_Context  *ctx,
  RtemsSemReqCreate_Pre_Count state
)
{
  switch ( state ) {
    case RtemsSemReqCreate_Pre_Count_Zero: {
      /*
       * While the ``count`` parameter is zero.
       */
      ctx->count = 0;
      break;
    }

    case RtemsSemReqCreate_Pre_Count_One: {
      /*
       * While the ``count`` parameter is one.
       */
      ctx->count = 1;
      break;
    }

    case RtemsSemReqCreate_Pre_Count_GtOne: {
      /*
       * While the ``count`` parameter is greater than one.
       */
      ctx->count = UINT32_MAX;
      break;
    }

    case RtemsSemReqCreate_Pre_Count_NA:
      break;
  }
}

static void RtemsSemReqCreate_Pre_Binary_Prepare(
  RtemsSemReqCreate_Context   *ctx,
  RtemsSemReqCreate_Pre_Binary state
)
{
  switch ( state ) {
    case RtemsSemReqCreate_Pre_Binary_Yes: {
      /*
       * While the ``attribute_set`` parameter specifies the binary semaphore
       * class.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE;
      break;
    }

    case RtemsSemReqCreate_Pre_Binary_No: {
      /*
       * While the ``attribute_set`` parameter does not specify the binary
       * semaphore class.
       */
      /* Use default */
      break;
    }

    case RtemsSemReqCreate_Pre_Binary_NA:
      break;
  }
}

static void RtemsSemReqCreate_Pre_Simple_Prepare(
  RtemsSemReqCreate_Context   *ctx,
  RtemsSemReqCreate_Pre_Simple state
)
{
  switch ( state ) {
    case RtemsSemReqCreate_Pre_Simple_Yes: {
      /*
       * While the ``attribute_set`` parameter specifies the simple binary
       * semaphore class.
       */
      ctx->attribute_set |= RTEMS_SIMPLE_BINARY_SEMAPHORE;
      break;
    }

    case RtemsSemReqCreate_Pre_Simple_No: {
      /*
       * While the ``attribute_set`` parameter does not specify the simple
       * binary semaphore class.
       */
      /* Use default */
      break;
    }

    case RtemsSemReqCreate_Pre_Simple_NA:
      break;
  }
}

static void RtemsSemReqCreate_Pre_Inherit_Prepare(
  RtemsSemReqCreate_Context    *ctx,
  RtemsSemReqCreate_Pre_Inherit state
)
{
  switch ( state ) {
    case RtemsSemReqCreate_Pre_Inherit_Yes: {
      /*
       * While the ``attribute_set`` parameter specifies the priority
       * inheritance locking protocol.
       */
      ctx->attribute_set |= RTEMS_INHERIT_PRIORITY;
      break;
    }

    case RtemsSemReqCreate_Pre_Inherit_No: {
      /*
       * While the ``attribute_set`` parameter does not specify the priority
       * inheritance locking protocol.
       */
      ctx->attribute_set |= RTEMS_NO_INHERIT_PRIORITY;
      break;
    }

    case RtemsSemReqCreate_Pre_Inherit_NA:
      break;
  }
}

static void RtemsSemReqCreate_Pre_Ceiling_Prepare(
  RtemsSemReqCreate_Context    *ctx,
  RtemsSemReqCreate_Pre_Ceiling state
)
{
  switch ( state ) {
    case RtemsSemReqCreate_Pre_Ceiling_Yes: {
      /*
       * While the ``attribute_set`` parameter specifies the priority ceiling
       * locking protocol.
       */
      ctx->attribute_set |= RTEMS_PRIORITY_CEILING;
      break;
    }

    case RtemsSemReqCreate_Pre_Ceiling_No: {
      /*
       * While the ``attribute_set`` parameter does not specify the priority
       * ceiling locking protocol.
       */
      ctx->attribute_set |= RTEMS_NO_PRIORITY_CEILING;
      break;
    }

    case RtemsSemReqCreate_Pre_Ceiling_NA:
      break;
  }
}

static void RtemsSemReqCreate_Pre_MrsP_Prepare(
  RtemsSemReqCreate_Context *ctx,
  RtemsSemReqCreate_Pre_MrsP state
)
{
  switch ( state ) {
    case RtemsSemReqCreate_Pre_MrsP_Yes: {
      /*
       * While the ``attribute_set`` parameter specifies the MrsP locking
       * protocol.
       */
      ctx->attribute_set |= RTEMS_MULTIPROCESSOR_RESOURCE_SHARING;
      break;
    }

    case RtemsSemReqCreate_Pre_MrsP_No: {
      /*
       * While the ``attribute_set`` parameter does not specify the MrsP
       * locking protocol.
       */
      ctx->attribute_set |= RTEMS_NO_MULTIPROCESSOR_RESOURCE_SHARING;
      break;
    }

    case RtemsSemReqCreate_Pre_MrsP_NA:
      break;
  }
}

static void RtemsSemReqCreate_Pre_Disc_Prepare(
  RtemsSemReqCreate_Context *ctx,
  RtemsSemReqCreate_Pre_Disc state
)
{
  switch ( state ) {
    case RtemsSemReqCreate_Pre_Disc_FIFO: {
      /*
       * While the ``attribute_set`` parameter specifies the FIFO task wait
       * queue discipline or the default task wait queue discipline.
       */
      RTEMS_STATIC_ASSERT( RTEMS_DEFAULT_ATTRIBUTES == RTEMS_FIFO, RTEMS_FIFO );
      ctx->attribute_set |= RTEMS_FIFO;
      break;
    }

    case RtemsSemReqCreate_Pre_Disc_Prio: {
      /*
       * While the ``attribute_set`` parameter specifies the priority task wait
       * queue discipline.
       */
      ctx->attribute_set |= RTEMS_PRIORITY;
      break;
    }

    case RtemsSemReqCreate_Pre_Disc_NA:
      break;
  }
}

static void RtemsSemReqCreate_Pre_Prio_Prepare(
  RtemsSemReqCreate_Context *ctx,
  RtemsSemReqCreate_Pre_Prio state
)
{
  switch ( state ) {
    case RtemsSemReqCreate_Pre_Prio_LeCur: {
      /*
       * While the ``priority_ceiling`` parameter is a valid task priority less
       * than or equal to the current priority of the calling task with respect
       * to the scheduler of the calling task at some point during the
       * directive call.
       */
      ctx->priority_ceiling = 0;
      break;
    }

    case RtemsSemReqCreate_Pre_Prio_GtCur: {
      /*
       * While the ``priority_ceiling`` parameter is a valid task priority
       * greater than the current priority of the calling task with respect to
       * the scheduler of the calling task at some point during the directive
       * call.
       */
      ctx->priority_ceiling = 2;
      break;
    }

    case RtemsSemReqCreate_Pre_Prio_Invalid: {
      /*
       * The ``priority_ceiling`` parameter shall not be a valid task priority
       * with respect to the scheduler of the calling task at some point during
       * the directive call.
       */
      ctx->priority_ceiling = UINT32_MAX;
      break;
    }

    case RtemsSemReqCreate_Pre_Prio_NA:
      break;
  }
}

static void RtemsSemReqCreate_Pre_Free_Prepare(
  RtemsSemReqCreate_Context *ctx,
  RtemsSemReqCreate_Pre_Free state
)
{
  size_t i;

  switch ( state ) {
    case RtemsSemReqCreate_Pre_Free_Yes: {
      /*
       * While the system has at least one inactive semaphore object available.
       */
      /* Nothing to do */
      break;
    }

    case RtemsSemReqCreate_Pre_Free_No: {
      /*
       * While the system has no inactive semaphore object available.
       */
      i = 0;
      ctx->seized_objects = T_seize_objects( Create, &i );
      break;
    }

    case RtemsSemReqCreate_Pre_Free_NA:
      break;
  }
}

static void RtemsSemReqCreate_Post_Status_Check(
  RtemsSemReqCreate_Context    *ctx,
  RtemsSemReqCreate_Post_Status state
)
{
  switch ( state ) {
    case RtemsSemReqCreate_Post_Status_Ok: {
      /*
       * The return status of rtems_semaphore_create() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsSemReqCreate_Post_Status_InvName: {
      /*
       * The return status of rtems_semaphore_create() shall be
       * RTEMS_INVALID_NAME.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsSemReqCreate_Post_Status_InvAddr: {
      /*
       * The return status of rtems_semaphore_create() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsSemReqCreate_Post_Status_InvNum: {
      /*
       * The return status of rtems_semaphore_create() shall be
       * RTEMS_INVALID_NUMBER.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NUMBER );
      break;
    }

    case RtemsSemReqCreate_Post_Status_InvPrio: {
      /*
       * The return status of rtems_semaphore_create() shall be
       * RTEMS_INVALID_PRIORITY.
       */
      T_rsc( ctx->status, RTEMS_INVALID_PRIORITY );
      break;
    }

    case RtemsSemReqCreate_Post_Status_NotDef: {
      /*
       * The return status of rtems_semaphore_create() shall be
       * RTEMS_NOT_DEFINED.
       */
      T_rsc( ctx->status, RTEMS_NOT_DEFINED );
      break;
    }

    case RtemsSemReqCreate_Post_Status_TooMany: {
      /*
       * The return status of rtems_semaphore_create() shall be RTEMS_TOO_MANY.
       */
      T_rsc( ctx->status, RTEMS_TOO_MANY );
      break;
    }

    case RtemsSemReqCreate_Post_Status_NA:
      break;
  }
}

static void RtemsSemReqCreate_Post_Name_Check(
  RtemsSemReqCreate_Context  *ctx,
  RtemsSemReqCreate_Post_Name state
)
{
  rtems_status_code sc;
  rtems_id          id;

  switch ( state ) {
    case RtemsSemReqCreate_Post_Name_Valid: {
      /*
       * The unique object name shall identify the semaphore created by the
       * rtems_semaphore_create() call.
       */
      id = 0;
      sc = rtems_semaphore_ident( NAME, RTEMS_SEARCH_LOCAL_NODE, &id );
      T_rsc_success( sc );
      T_eq_u32( id, ctx->id_value );
      break;
    }

    case RtemsSemReqCreate_Post_Name_Invalid: {
      /*
       * The unique object name shall not identify a semaphore.
       */
      sc = rtems_semaphore_ident( NAME, RTEMS_SEARCH_LOCAL_NODE, &id );
      T_rsc( sc, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsSemReqCreate_Post_Name_NA:
      break;
  }
}

static void RtemsSemReqCreate_Post_IdVar_Check(
  RtemsSemReqCreate_Context   *ctx,
  RtemsSemReqCreate_Post_IdVar state
)
{
  switch ( state ) {
    case RtemsSemReqCreate_Post_IdVar_Set: {
      /*
       * The value of the object referenced by the ``id`` parameter shall be
       * set to the object identifier of the created semaphore after the return
       * of the rtems_semaphore_create() call.
       */
      T_eq_ptr( ctx->id, &ctx->id_value );
      T_ne_u32( ctx->id_value, INVALID_ID );
      break;
    }

    case RtemsSemReqCreate_Post_IdVar_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in past calls to
       * rtems_semaphore_create() shall not be accessed by the
       * rtems_semaphore_create() call.
       */
      T_eq_u32( ctx->id_value, INVALID_ID );
      break;
    }

    case RtemsSemReqCreate_Post_IdVar_NA:
      break;
  }
}

static void RtemsSemReqCreate_Post_Variant_Check(
  RtemsSemReqCreate_Context     *ctx,
  RtemsSemReqCreate_Post_Variant state
)
{
  switch ( state ) {
    case RtemsSemReqCreate_Post_Variant_Cnt: {
      /*
       * The semaphore created by the rtems_semaphore_create() call shall be a
       * counting semaphore.
       */
      T_eq_int( ctx->variant, SEMAPHORE_VARIANT_COUNTING );
      break;
    }

    case RtemsSemReqCreate_Post_Variant_Bin: {
      /*
       * The semaphore created by the rtems_semaphore_create() call shall be a
       * binary semaphore not using a locking protocol.
       */
      T_eq_int( ctx->variant, SEMAPHORE_VARIANT_MUTEX_NO_PROTOCOL );
      break;
    }

    case RtemsSemReqCreate_Post_Variant_PI: {
      /*
       * The semaphore created by the rtems_semaphore_create() call shall be a
       * binary semaphore using the priority inheritance locking protocol.
       */
      T_eq_int( ctx->variant, SEMAPHORE_VARIANT_MUTEX_INHERIT_PRIORITY );
      break;
    }

    case RtemsSemReqCreate_Post_Variant_PC: {
      /*
       * The semaphore created by the rtems_semaphore_create() call shall be a
       * binary semaphore using the priority ceiling locking protocol.
       */
      T_eq_int( ctx->variant, SEMAPHORE_VARIANT_MUTEX_PRIORITY_CEILING );
      break;
    }

    case RtemsSemReqCreate_Post_Variant_SB: {
      /*
       * The semaphore created by the rtems_semaphore_create() call shall be a
       * simple binary semaphore.
       */
      T_eq_int( ctx->variant, SEMAPHORE_VARIANT_SIMPLE_BINARY );
      break;
    }

    case RtemsSemReqCreate_Post_Variant_MrsP: {
      /*
       * The semaphore created by the rtems_semaphore_create() call shall be a
       * binary semaphore using the MrsP locking protocol.
       */
      #if defined(RTEMS_SMP)
      T_eq_int( ctx->variant, SEMAPHORE_VARIANT_MRSP );
      #else
      T_true( false );
      #endif
      break;
    }

    case RtemsSemReqCreate_Post_Variant_NA:
      break;
  }
}

static void RtemsSemReqCreate_Post_Disc_Check(
  RtemsSemReqCreate_Context  *ctx,
  RtemsSemReqCreate_Post_Disc state
)
{
  switch ( state ) {
    case RtemsSemReqCreate_Post_Disc_FIFO: {
      /*
       * The semaphore created by the rtems_semaphore_create() call shall use
       * the FIFO task wait queue discipline.
       */
      T_eq_int( ctx->discipline, SEMAPHORE_DISCIPLINE_FIFO );
      break;
    }

    case RtemsSemReqCreate_Post_Disc_Prio: {
      /*
       * The semaphore created by the rtems_semaphore_create() call shall use
       * the priority task wait queue discipline.
       */
      T_eq_int( ctx->discipline, SEMAPHORE_DISCIPLINE_PRIORITY );
      break;
    }

    case RtemsSemReqCreate_Post_Disc_NA:
      break;
  }
}

static void RtemsSemReqCreate_Post_Count_Check(
  RtemsSemReqCreate_Context   *ctx,
  RtemsSemReqCreate_Post_Count state
)
{
  switch ( state ) {
    case RtemsSemReqCreate_Post_Count_Initial: {
      /*
       * The semaphore created by the rtems_semaphore_create() call shall have
       * an initial count equal to the value of the ``count`` parameter.
       */
      T_eq_u32( ctx->sem_count, ctx->count );
      break;
    }

    case RtemsSemReqCreate_Post_Count_NA:
      break;
  }
}

static void RtemsSemReqCreate_Post_Owner_Check(
  RtemsSemReqCreate_Context   *ctx,
  RtemsSemReqCreate_Post_Owner state
)
{
  switch ( state ) {
    case RtemsSemReqCreate_Post_Owner_Caller: {
      /*
       * The semaphore created by the rtems_semaphore_create() call shall be
       * initially owned by the calling task.
       */
      T_eq_ptr( ctx->owner, ctx->executing );
      break;
    }

    case RtemsSemReqCreate_Post_Owner_No: {
      /*
       * The semaphore created by the rtems_semaphore_create() call shall not
       * initially have an owner.
       */
      T_null( ctx->owner );
      break;
    }

    case RtemsSemReqCreate_Post_Owner_NA:
      break;
  }
}

static void RtemsSemReqCreate_Post_Prio_Check(
  RtemsSemReqCreate_Context  *ctx,
  RtemsSemReqCreate_Post_Prio state
)
{
  switch ( state ) {
    case RtemsSemReqCreate_Post_Prio_Ceiling: {
      /*
       * The current priority of the task which called rtems_semaphore_create()
       * shall be equal to the value of the ``priority_ceiling`` parameter.
       */
      T_eq_u32( GetSelfPriority(), ctx->priority_ceiling );
      break;
    }

    case RtemsSemReqCreate_Post_Prio_Nop: {
      /*
       * The current priority of the task which called rtems_semaphore_create()
       * shall not be modified by the rtems_semaphore_create() call.
       */
      T_eq_u32( GetSelfPriority(), 1 );
      break;
    }

    case RtemsSemReqCreate_Post_Prio_NA:
      break;
  }
}

static void RtemsSemReqCreate_Setup( RtemsSemReqCreate_Context *ctx )
{
  memset( ctx, 0, sizeof( *ctx ) );
  ctx->executing = _Thread_Get_executing();
}

static void RtemsSemReqCreate_Setup_Wrap( void *arg )
{
  RtemsSemReqCreate_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSemReqCreate_Setup( ctx );
}

static void RtemsSemReqCreate_Prepare( RtemsSemReqCreate_Context *ctx )
{
  rtems_status_code sc;
  rtems_id          id;

  ctx->id_value = INVALID_ID;
  ctx->attribute_set = RTEMS_DEFAULT_ATTRIBUTES;

  id = INVALID_ID;
  sc = rtems_semaphore_ident( NAME, RTEMS_SEARCH_LOCAL_NODE, &id );
  T_rsc( sc, RTEMS_INVALID_NAME );
  T_eq_u32( id, INVALID_ID );
}

static void RtemsSemReqCreate_Action( RtemsSemReqCreate_Context *ctx )
{
  ctx->status = rtems_semaphore_create(
    ctx->name,
    ctx->count,
    ctx->attribute_set,
    ctx->priority_ceiling,
    ctx->id
  );

  GetSemAttributes( ctx );
}

static void RtemsSemReqCreate_Cleanup( RtemsSemReqCreate_Context *ctx )
{
  rtems_status_code sc;

  if ( ctx->id_value != INVALID_ID ) {
    if ( ctx->count == 0 ) {
      sc = rtems_semaphore_release( ctx->id_value );
      T_rsc_success( sc );
    }

    sc = rtems_semaphore_delete( ctx->id_value );
    T_rsc_success( sc );
  }

  T_surrender_objects( &ctx->seized_objects, rtems_semaphore_delete );
}

static const RtemsSemReqCreate_Entry
RtemsSemReqCreate_Entries[] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsSemReqCreate_Post_Status_InvName,
    RtemsSemReqCreate_Post_Name_Invalid, RtemsSemReqCreate_Post_IdVar_Nop,
    RtemsSemReqCreate_Post_Variant_NA, RtemsSemReqCreate_Post_Disc_NA,
    RtemsSemReqCreate_Post_Count_NA, RtemsSemReqCreate_Post_Owner_NA,
    RtemsSemReqCreate_Post_Prio_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsSemReqCreate_Post_Status_InvAddr,
    RtemsSemReqCreate_Post_Name_Invalid, RtemsSemReqCreate_Post_IdVar_Nop,
    RtemsSemReqCreate_Post_Variant_NA, RtemsSemReqCreate_Post_Disc_NA,
    RtemsSemReqCreate_Post_Count_NA, RtemsSemReqCreate_Post_Owner_NA,
    RtemsSemReqCreate_Post_Prio_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsSemReqCreate_Post_Status_NotDef,
    RtemsSemReqCreate_Post_Name_Invalid, RtemsSemReqCreate_Post_IdVar_Nop,
    RtemsSemReqCreate_Post_Variant_NA, RtemsSemReqCreate_Post_Disc_NA,
    RtemsSemReqCreate_Post_Count_NA, RtemsSemReqCreate_Post_Owner_NA,
    RtemsSemReqCreate_Post_Prio_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsSemReqCreate_Post_Status_TooMany,
    RtemsSemReqCreate_Post_Name_Invalid, RtemsSemReqCreate_Post_IdVar_Nop,
    RtemsSemReqCreate_Post_Variant_NA, RtemsSemReqCreate_Post_Disc_NA,
    RtemsSemReqCreate_Post_Count_NA, RtemsSemReqCreate_Post_Owner_NA,
    RtemsSemReqCreate_Post_Prio_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsSemReqCreate_Post_Status_InvNum,
    RtemsSemReqCreate_Post_Name_Invalid, RtemsSemReqCreate_Post_IdVar_Nop,
    RtemsSemReqCreate_Post_Variant_NA, RtemsSemReqCreate_Post_Disc_NA,
    RtemsSemReqCreate_Post_Count_NA, RtemsSemReqCreate_Post_Owner_NA,
    RtemsSemReqCreate_Post_Prio_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsSemReqCreate_Post_Status_Ok,
    RtemsSemReqCreate_Post_Name_Valid, RtemsSemReqCreate_Post_IdVar_Set,
    RtemsSemReqCreate_Post_Variant_Cnt, RtemsSemReqCreate_Post_Disc_FIFO,
    RtemsSemReqCreate_Post_Count_Initial, RtemsSemReqCreate_Post_Owner_No,
    RtemsSemReqCreate_Post_Prio_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsSemReqCreate_Post_Status_Ok,
    RtemsSemReqCreate_Post_Name_Valid, RtemsSemReqCreate_Post_IdVar_Set,
    RtemsSemReqCreate_Post_Variant_Cnt, RtemsSemReqCreate_Post_Disc_Prio,
    RtemsSemReqCreate_Post_Count_Initial, RtemsSemReqCreate_Post_Owner_No,
    RtemsSemReqCreate_Post_Prio_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsSemReqCreate_Post_Status_InvPrio,
    RtemsSemReqCreate_Post_Name_Invalid, RtemsSemReqCreate_Post_IdVar_Nop,
    RtemsSemReqCreate_Post_Variant_NA, RtemsSemReqCreate_Post_Disc_NA,
    RtemsSemReqCreate_Post_Count_NA, RtemsSemReqCreate_Post_Owner_NA,
    RtemsSemReqCreate_Post_Prio_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsSemReqCreate_Post_Status_Ok,
    RtemsSemReqCreate_Post_Name_Valid, RtemsSemReqCreate_Post_IdVar_Set,
    RtemsSemReqCreate_Post_Variant_SB, RtemsSemReqCreate_Post_Disc_FIFO,
    RtemsSemReqCreate_Post_Count_Initial, RtemsSemReqCreate_Post_Owner_No,
    RtemsSemReqCreate_Post_Prio_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsSemReqCreate_Post_Status_Ok,
    RtemsSemReqCreate_Post_Name_Valid, RtemsSemReqCreate_Post_IdVar_Set,
    RtemsSemReqCreate_Post_Variant_SB, RtemsSemReqCreate_Post_Disc_Prio,
    RtemsSemReqCreate_Post_Count_Initial, RtemsSemReqCreate_Post_Owner_No,
    RtemsSemReqCreate_Post_Prio_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsSemReqCreate_Post_Status_Ok,
    RtemsSemReqCreate_Post_Name_Valid, RtemsSemReqCreate_Post_IdVar_Set,
    RtemsSemReqCreate_Post_Variant_PI, RtemsSemReqCreate_Post_Disc_Prio,
    RtemsSemReqCreate_Post_Count_NA, RtemsSemReqCreate_Post_Owner_Caller,
    RtemsSemReqCreate_Post_Prio_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsSemReqCreate_Post_Status_Ok,
    RtemsSemReqCreate_Post_Name_Valid, RtemsSemReqCreate_Post_IdVar_Set,
    RtemsSemReqCreate_Post_Variant_Bin, RtemsSemReqCreate_Post_Disc_FIFO,
    RtemsSemReqCreate_Post_Count_NA, RtemsSemReqCreate_Post_Owner_Caller,
    RtemsSemReqCreate_Post_Prio_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsSemReqCreate_Post_Status_Ok,
    RtemsSemReqCreate_Post_Name_Valid, RtemsSemReqCreate_Post_IdVar_Set,
    RtemsSemReqCreate_Post_Variant_Bin, RtemsSemReqCreate_Post_Disc_Prio,
    RtemsSemReqCreate_Post_Count_NA, RtemsSemReqCreate_Post_Owner_Caller,
    RtemsSemReqCreate_Post_Prio_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsSemReqCreate_Post_Status_Ok,
    RtemsSemReqCreate_Post_Name_Valid, RtemsSemReqCreate_Post_IdVar_Set,
    RtemsSemReqCreate_Post_Variant_PI, RtemsSemReqCreate_Post_Disc_Prio,
    RtemsSemReqCreate_Post_Count_NA, RtemsSemReqCreate_Post_Owner_No,
    RtemsSemReqCreate_Post_Prio_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsSemReqCreate_Post_Status_Ok,
    RtemsSemReqCreate_Post_Name_Valid, RtemsSemReqCreate_Post_IdVar_Set,
    RtemsSemReqCreate_Post_Variant_Bin, RtemsSemReqCreate_Post_Disc_FIFO,
    RtemsSemReqCreate_Post_Count_NA, RtemsSemReqCreate_Post_Owner_No,
    RtemsSemReqCreate_Post_Prio_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsSemReqCreate_Post_Status_Ok,
    RtemsSemReqCreate_Post_Name_Valid, RtemsSemReqCreate_Post_IdVar_Set,
    RtemsSemReqCreate_Post_Variant_Bin, RtemsSemReqCreate_Post_Disc_Prio,
    RtemsSemReqCreate_Post_Count_NA, RtemsSemReqCreate_Post_Owner_No,
    RtemsSemReqCreate_Post_Prio_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsSemReqCreate_Post_Status_Ok,
    RtemsSemReqCreate_Post_Name_Valid, RtemsSemReqCreate_Post_IdVar_Set,
    RtemsSemReqCreate_Post_Variant_PC, RtemsSemReqCreate_Post_Disc_Prio,
    RtemsSemReqCreate_Post_Count_NA, RtemsSemReqCreate_Post_Owner_No,
    RtemsSemReqCreate_Post_Prio_Nop },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsSemReqCreate_Post_Status_Ok,
    RtemsSemReqCreate_Post_Name_Valid, RtemsSemReqCreate_Post_IdVar_Set,
    RtemsSemReqCreate_Post_Variant_MrsP, RtemsSemReqCreate_Post_Disc_Prio,
    RtemsSemReqCreate_Post_Count_NA, RtemsSemReqCreate_Post_Owner_No,
    RtemsSemReqCreate_Post_Prio_Nop },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsSemReqCreate_Post_Status_Ok,
    RtemsSemReqCreate_Post_Name_Valid, RtemsSemReqCreate_Post_IdVar_Set,
    RtemsSemReqCreate_Post_Variant_PC, RtemsSemReqCreate_Post_Disc_Prio,
    RtemsSemReqCreate_Post_Count_NA, RtemsSemReqCreate_Post_Owner_No,
    RtemsSemReqCreate_Post_Prio_Nop },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsSemReqCreate_Post_Status_Ok,
    RtemsSemReqCreate_Post_Name_Valid, RtemsSemReqCreate_Post_IdVar_Set,
    RtemsSemReqCreate_Post_Variant_PC, RtemsSemReqCreate_Post_Disc_Prio,
    RtemsSemReqCreate_Post_Count_NA, RtemsSemReqCreate_Post_Owner_Caller,
    RtemsSemReqCreate_Post_Prio_Ceiling },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsSemReqCreate_Post_Status_Ok,
    RtemsSemReqCreate_Post_Name_Valid, RtemsSemReqCreate_Post_IdVar_Set,
    RtemsSemReqCreate_Post_Variant_MrsP, RtemsSemReqCreate_Post_Disc_Prio,
    RtemsSemReqCreate_Post_Count_NA, RtemsSemReqCreate_Post_Owner_Caller,
    RtemsSemReqCreate_Post_Prio_Ceiling }
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsSemReqCreate_Post_Status_Ok,
    RtemsSemReqCreate_Post_Name_Valid, RtemsSemReqCreate_Post_IdVar_Set,
    RtemsSemReqCreate_Post_Variant_PC, RtemsSemReqCreate_Post_Disc_Prio,
    RtemsSemReqCreate_Post_Count_NA, RtemsSemReqCreate_Post_Owner_Caller,
    RtemsSemReqCreate_Post_Prio_Ceiling }
#endif
};

static const uint8_t
RtemsSemReqCreate_Map[] = {
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 10, 3, 10, 3, 10, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 18, 3, 7, 3, 7, 3, 2, 2, 2, 2, 2, 2, 19, 3, 7, 3, 7, 3,
  11, 3, 11, 3, 11, 3, 12, 3, 12, 3, 12, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8, 3, 8, 3, 8, 3,
  9, 3, 9, 3, 9, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 5, 3, 5, 3, 5, 3, 6, 3, 6, 3, 6, 3, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 13, 3, 13, 3, 13, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 16, 3, 16, 3, 7, 3, 2, 2, 2, 2, 2, 2, 17, 3, 17, 3, 7, 3, 14,
  3, 14, 3, 14, 3, 15, 3, 15, 3, 15, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 8, 3, 8, 3, 8, 3, 9,
  3, 9, 3, 9, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 5, 3, 5, 3, 5, 3, 6, 3, 6, 3, 6, 3, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 5, 3, 5, 3, 5, 3, 6, 3, 6, 3, 6, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static size_t RtemsSemReqCreate_Scope( void *arg, char *buf, size_t n )
{
  RtemsSemReqCreate_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsSemReqCreate_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsSemReqCreate_Fixture = {
  .setup = RtemsSemReqCreate_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsSemReqCreate_Scope,
  .initial_context = &RtemsSemReqCreate_Instance
};

static inline RtemsSemReqCreate_Entry RtemsSemReqCreate_PopEntry(
  RtemsSemReqCreate_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsSemReqCreate_Entries[
    RtemsSemReqCreate_Map[ index ]
  ];
}

static void RtemsSemReqCreate_TestVariant( RtemsSemReqCreate_Context *ctx )
{
  RtemsSemReqCreate_Pre_Name_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsSemReqCreate_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsSemReqCreate_Pre_Count_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsSemReqCreate_Pre_Binary_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsSemReqCreate_Pre_Simple_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsSemReqCreate_Pre_Inherit_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  RtemsSemReqCreate_Pre_Ceiling_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  RtemsSemReqCreate_Pre_MrsP_Prepare( ctx, ctx->Map.pcs[ 7 ] );
  RtemsSemReqCreate_Pre_Disc_Prepare( ctx, ctx->Map.pcs[ 8 ] );
  RtemsSemReqCreate_Pre_Prio_Prepare( ctx, ctx->Map.pcs[ 9 ] );
  RtemsSemReqCreate_Pre_Free_Prepare( ctx, ctx->Map.pcs[ 10 ] );
  RtemsSemReqCreate_Action( ctx );
  RtemsSemReqCreate_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsSemReqCreate_Post_Name_Check( ctx, ctx->Map.entry.Post_Name );
  RtemsSemReqCreate_Post_IdVar_Check( ctx, ctx->Map.entry.Post_IdVar );
  RtemsSemReqCreate_Post_Variant_Check( ctx, ctx->Map.entry.Post_Variant );
  RtemsSemReqCreate_Post_Disc_Check( ctx, ctx->Map.entry.Post_Disc );
  RtemsSemReqCreate_Post_Count_Check( ctx, ctx->Map.entry.Post_Count );
  RtemsSemReqCreate_Post_Owner_Check( ctx, ctx->Map.entry.Post_Owner );
  RtemsSemReqCreate_Post_Prio_Check( ctx, ctx->Map.entry.Post_Prio );
}

/**
 * @fn void T_case_body_RtemsSemReqCreate( void )
 */
T_TEST_CASE_FIXTURE( RtemsSemReqCreate, &RtemsSemReqCreate_Fixture )
{
  RtemsSemReqCreate_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsSemReqCreate_Pre_Name_Valid;
    ctx->Map.pcs[ 0 ] < RtemsSemReqCreate_Pre_Name_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsSemReqCreate_Pre_Id_Valid;
      ctx->Map.pcs[ 1 ] < RtemsSemReqCreate_Pre_Id_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsSemReqCreate_Pre_Count_Zero;
        ctx->Map.pcs[ 2 ] < RtemsSemReqCreate_Pre_Count_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsSemReqCreate_Pre_Binary_Yes;
          ctx->Map.pcs[ 3 ] < RtemsSemReqCreate_Pre_Binary_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = RtemsSemReqCreate_Pre_Simple_Yes;
            ctx->Map.pcs[ 4 ] < RtemsSemReqCreate_Pre_Simple_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            for (
              ctx->Map.pcs[ 5 ] = RtemsSemReqCreate_Pre_Inherit_Yes;
              ctx->Map.pcs[ 5 ] < RtemsSemReqCreate_Pre_Inherit_NA;
              ++ctx->Map.pcs[ 5 ]
            ) {
              for (
                ctx->Map.pcs[ 6 ] = RtemsSemReqCreate_Pre_Ceiling_Yes;
                ctx->Map.pcs[ 6 ] < RtemsSemReqCreate_Pre_Ceiling_NA;
                ++ctx->Map.pcs[ 6 ]
              ) {
                for (
                  ctx->Map.pcs[ 7 ] = RtemsSemReqCreate_Pre_MrsP_Yes;
                  ctx->Map.pcs[ 7 ] < RtemsSemReqCreate_Pre_MrsP_NA;
                  ++ctx->Map.pcs[ 7 ]
                ) {
                  for (
                    ctx->Map.pcs[ 8 ] = RtemsSemReqCreate_Pre_Disc_FIFO;
                    ctx->Map.pcs[ 8 ] < RtemsSemReqCreate_Pre_Disc_NA;
                    ++ctx->Map.pcs[ 8 ]
                  ) {
                    for (
                      ctx->Map.pcs[ 9 ] = RtemsSemReqCreate_Pre_Prio_LeCur;
                      ctx->Map.pcs[ 9 ] < RtemsSemReqCreate_Pre_Prio_NA;
                      ++ctx->Map.pcs[ 9 ]
                    ) {
                      for (
                        ctx->Map.pcs[ 10 ] = RtemsSemReqCreate_Pre_Free_Yes;
                        ctx->Map.pcs[ 10 ] < RtemsSemReqCreate_Pre_Free_NA;
                        ++ctx->Map.pcs[ 10 ]
                      ) {
                        ctx->Map.entry = RtemsSemReqCreate_PopEntry( ctx );
                        RtemsSemReqCreate_Prepare( ctx );
                        RtemsSemReqCreate_TestVariant( ctx );
                        RtemsSemReqCreate_Cleanup( ctx );
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

/** @} */
