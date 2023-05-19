/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSemReqSetPriority
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
#include <string.h>

#include "ts-config.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsSemReqSetPriority spec:/rtems/sem/req/set-priority
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsSemReqSetPriority_Pre_Class_Counting,
  RtemsSemReqSetPriority_Pre_Class_Simple,
  RtemsSemReqSetPriority_Pre_Class_Binary,
  RtemsSemReqSetPriority_Pre_Class_PrioCeilingNoOwner,
  RtemsSemReqSetPriority_Pre_Class_PrioCeilingOwner,
  RtemsSemReqSetPriority_Pre_Class_PrioInherit,
  RtemsSemReqSetPriority_Pre_Class_MrsP,
  RtemsSemReqSetPriority_Pre_Class_NA
} RtemsSemReqSetPriority_Pre_Class;

typedef enum {
  RtemsSemReqSetPriority_Pre_SemId_Valid,
  RtemsSemReqSetPriority_Pre_SemId_Invalid,
  RtemsSemReqSetPriority_Pre_SemId_NA
} RtemsSemReqSetPriority_Pre_SemId;

typedef enum {
  RtemsSemReqSetPriority_Pre_SchedId_Invalid,
  RtemsSemReqSetPriority_Pre_SchedId_Create,
  RtemsSemReqSetPriority_Pre_SchedId_Other,
  RtemsSemReqSetPriority_Pre_SchedId_NA
} RtemsSemReqSetPriority_Pre_SchedId;

typedef enum {
  RtemsSemReqSetPriority_Pre_NewPrio_Current,
  RtemsSemReqSetPriority_Pre_NewPrio_Valid,
  RtemsSemReqSetPriority_Pre_NewPrio_Invalid,
  RtemsSemReqSetPriority_Pre_NewPrio_NA
} RtemsSemReqSetPriority_Pre_NewPrio;

typedef enum {
  RtemsSemReqSetPriority_Pre_OldPrio_Valid,
  RtemsSemReqSetPriority_Pre_OldPrio_Null,
  RtemsSemReqSetPriority_Pre_OldPrio_NA
} RtemsSemReqSetPriority_Pre_OldPrio;

typedef enum {
  RtemsSemReqSetPriority_Post_Status_Ok,
  RtemsSemReqSetPriority_Post_Status_InvAddr,
  RtemsSemReqSetPriority_Post_Status_InvId,
  RtemsSemReqSetPriority_Post_Status_InvPrio,
  RtemsSemReqSetPriority_Post_Status_NotDef,
  RtemsSemReqSetPriority_Post_Status_NA
} RtemsSemReqSetPriority_Post_Status;

typedef enum {
  RtemsSemReqSetPriority_Post_OwnerPrio_Nop,
  RtemsSemReqSetPriority_Post_OwnerPrio_New,
  RtemsSemReqSetPriority_Post_OwnerPrio_NA
} RtemsSemReqSetPriority_Post_OwnerPrio;

typedef enum {
  RtemsSemReqSetPriority_Post_SemPrio_Set,
  RtemsSemReqSetPriority_Post_SemPrio_Nop,
  RtemsSemReqSetPriority_Post_SemPrio_NA
} RtemsSemReqSetPriority_Post_SemPrio;

typedef enum {
  RtemsSemReqSetPriority_Post_OldPrioVar_Set,
  RtemsSemReqSetPriority_Post_OldPrioVar_Nop,
  RtemsSemReqSetPriority_Post_OldPrioVar_NA
} RtemsSemReqSetPriority_Post_OldPrioVar;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Class_NA : 1;
  uint16_t Pre_SemId_NA : 1;
  uint16_t Pre_SchedId_NA : 1;
  uint16_t Pre_NewPrio_NA : 1;
  uint16_t Pre_OldPrio_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_OwnerPrio : 2;
  uint16_t Post_SemPrio : 2;
  uint16_t Post_OldPrioVar : 2;
} RtemsSemReqSetPriority_Entry;

/**
 * @brief Test context for spec:/rtems/sem/req/set-priority test case.
 */
typedef struct {
  /**
   * @brief This member contains the scheduler identifier of the runner task.
   */
  rtems_id runner_scheduler_id;

  /**
   * @brief This member contains the scheduler identifier of a scheduler not
   *   used by the runner task.
   */
  rtems_id other_scheduler_id;

  /**
   * @brief This member specifies the initial count of the semaphore.
   */
  uint32_t count;

  /**
   * @brief This member specifies the attribute set of the semaphore.
   */
  rtems_attribute attribute_set;

  /**
   * @brief This member contains the semaphore identifier.
   */
  rtems_id the_semaphore_id;

  /**
   * @brief If this member is true, then the ``semaphore_id`` parameter value
   *   shall be associated with the semaphore, otherwise it shall be not
   *   associated with a semaphore.
   */
  bool valid_id;

  /**
   * @brief This member may contain the task priority returned by
   *   rtems_semaphore_set_priority().
   */
  rtems_task_priority old_priority_value;

  /**
   * @brief This member specifies the ``semaphore_id`` parameter for the
   *   rtems_semaphore_set_priority() call.
   */
  rtems_id semaphore_id;

  /**
   * @brief This member specifies the ``scheduler_id`` parameter for the
   *   rtems_semaphore_set_priority() call.
   */
  rtems_id scheduler_id;

  /**
   * @brief This member specifies the ``new_priority`` parameter for the
   *   rtems_semaphore_set_priority() call.
   */
  rtems_task_priority new_priority;

  /**
   * @brief This member specifies the ``old_priority`` parameter for the
   *   rtems_semaphore_set_priority() call.
   */
  rtems_task_priority *old_priority;

  /**
   * @brief This member contains the status of the
   *   rtems_semaphore_set_priority() call.
   */
  rtems_status_code status;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 5 ];

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
    RtemsSemReqSetPriority_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsSemReqSetPriority_Context;

static RtemsSemReqSetPriority_Context
  RtemsSemReqSetPriority_Instance;

static const char * const RtemsSemReqSetPriority_PreDesc_Class[] = {
  "Counting",
  "Simple",
  "Binary",
  "PrioCeilingNoOwner",
  "PrioCeilingOwner",
  "PrioInherit",
  "MrsP",
  "NA"
};

static const char * const RtemsSemReqSetPriority_PreDesc_SemId[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsSemReqSetPriority_PreDesc_SchedId[] = {
  "Invalid",
  "Create",
  "Other",
  "NA"
};

static const char * const RtemsSemReqSetPriority_PreDesc_NewPrio[] = {
  "Current",
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsSemReqSetPriority_PreDesc_OldPrio[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const RtemsSemReqSetPriority_PreDesc[] = {
  RtemsSemReqSetPriority_PreDesc_Class,
  RtemsSemReqSetPriority_PreDesc_SemId,
  RtemsSemReqSetPriority_PreDesc_SchedId,
  RtemsSemReqSetPriority_PreDesc_NewPrio,
  RtemsSemReqSetPriority_PreDesc_OldPrio,
  NULL
};

#define NAME rtems_build_name( 'T', 'E', 'S', 'T' )

typedef RtemsSemReqSetPriority_Context Context;

static void ReleaseSemaphore( const Context *ctx )
{
  rtems_status_code sc;

  sc = rtems_semaphore_release( ctx->the_semaphore_id );
  T_rsc_success( sc );
}

static void ChangeScheduler( rtems_id scheduler_id )
{
#if defined(RTEMS_SMP)
  rtems_status_code sc;

  sc = rtems_task_set_scheduler( RTEMS_SELF, scheduler_id, PRIO_NORMAL );
  T_rsc_success( sc );
#else
  (void) scheduler_id;
#endif
}

static void CheckPriority(
  const Context      *ctx,
  rtems_id            scheduler_id,
  rtems_task_priority priority
)
{
  rtems_status_code sc;

  ChangeScheduler( scheduler_id );

  sc = rtems_semaphore_obtain(
    ctx->the_semaphore_id,
    RTEMS_WAIT,
    RTEMS_NO_TIMEOUT
  );
  T_rsc_success( sc );

  T_eq_u32( GetSelfPriority(), priority );

  ReleaseSemaphore( ctx );
  ChangeScheduler( ctx->runner_scheduler_id );
}

static void CheckNotDefined(
  const Context      *ctx,
  rtems_id            scheduler_id
)
{
#if defined(RTEMS_SMP)
  rtems_status_code sc;

  ChangeScheduler( scheduler_id );

  sc = rtems_semaphore_obtain(
    ctx->the_semaphore_id,
    RTEMS_WAIT,
    RTEMS_NO_TIMEOUT
  );
  T_rsc( sc, RTEMS_NOT_DEFINED );

  ChangeScheduler( ctx->runner_scheduler_id );
#else
  (void) ctx;
  (void) scheduler_id;
#endif
}

static void RtemsSemReqSetPriority_Pre_Class_Prepare(
  RtemsSemReqSetPriority_Context  *ctx,
  RtemsSemReqSetPriority_Pre_Class state
)
{
  switch ( state ) {
    case RtemsSemReqSetPriority_Pre_Class_Counting: {
      /*
       * While the semaphore object is a counting semaphore.
       */
      ctx->attribute_set |= RTEMS_COUNTING_SEMAPHORE;
      break;
    }

    case RtemsSemReqSetPriority_Pre_Class_Simple: {
      /*
       * While the semaphore object is a simple binary semaphore.
       */
      ctx->attribute_set |= RTEMS_SIMPLE_BINARY_SEMAPHORE;
      break;
    }

    case RtemsSemReqSetPriority_Pre_Class_Binary: {
      /*
       * While the semaphore object is a binary semaphore.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE;
      break;
    }

    case RtemsSemReqSetPriority_Pre_Class_PrioCeilingNoOwner: {
      /*
       * While the semaphore object is a priority ceiling semaphore, while the
       * semaphore has no owner.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY_CEILING;
      break;
    }

    case RtemsSemReqSetPriority_Pre_Class_PrioCeilingOwner: {
      /*
       * While the semaphore object is a priority ceiling semaphore, while the
       * semaphore has an owner.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY_CEILING;
      ctx->count = 0;
      break;
    }

    case RtemsSemReqSetPriority_Pre_Class_PrioInherit: {
      /*
       * While the semaphore object is a priority inheritance semaphore.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY;
      break;
    }

    case RtemsSemReqSetPriority_Pre_Class_MrsP: {
      /*
       * While the semaphore object is a MrsP semaphore.
       */
      ctx->attribute_set |= RTEMS_BINARY_SEMAPHORE |
        RTEMS_MULTIPROCESSOR_RESOURCE_SHARING;
      break;
    }

    case RtemsSemReqSetPriority_Pre_Class_NA:
      break;
  }
}

static void RtemsSemReqSetPriority_Pre_SemId_Prepare(
  RtemsSemReqSetPriority_Context  *ctx,
  RtemsSemReqSetPriority_Pre_SemId state
)
{
  switch ( state ) {
    case RtemsSemReqSetPriority_Pre_SemId_Valid: {
      /*
       * While the ``semaphore_id`` parameter is associated with the semaphore.
       */
      ctx->valid_id = true;
      break;
    }

    case RtemsSemReqSetPriority_Pre_SemId_Invalid: {
      /*
       * While the ``semaphore_id`` parameter is not associated with a
       * semaphore.
       */
      ctx->valid_id = false;
      break;
    }

    case RtemsSemReqSetPriority_Pre_SemId_NA:
      break;
  }
}

static void RtemsSemReqSetPriority_Pre_SchedId_Prepare(
  RtemsSemReqSetPriority_Context    *ctx,
  RtemsSemReqSetPriority_Pre_SchedId state
)
{
  switch ( state ) {
    case RtemsSemReqSetPriority_Pre_SchedId_Invalid: {
      /*
       * While the ``scheduler_id`` parameter is not associated with a
       * scheduler.
       */
      ctx->scheduler_id = INVALID_ID;
      break;
    }

    case RtemsSemReqSetPriority_Pre_SchedId_Create: {
      /*
       * While the ``scheduler_id`` parameter is associated with the scheduler
       * used to create the semaphore.
       */
      ctx->scheduler_id = ctx->runner_scheduler_id;
      break;
    }

    case RtemsSemReqSetPriority_Pre_SchedId_Other: {
      /*
       * While the ``scheduler_id`` parameter is associated with a scheduler
       * other than the one used to create the semaphore.
       */
      ctx->scheduler_id = ctx->other_scheduler_id;
      break;
    }

    case RtemsSemReqSetPriority_Pre_SchedId_NA:
      break;
  }
}

static void RtemsSemReqSetPriority_Pre_NewPrio_Prepare(
  RtemsSemReqSetPriority_Context    *ctx,
  RtemsSemReqSetPriority_Pre_NewPrio state
)
{
  switch ( state ) {
    case RtemsSemReqSetPriority_Pre_NewPrio_Current: {
      /*
       * While the ``new_priority`` parameter is equal to
       * RTEMS_CURRENT_PRIORITY.
       */
      ctx->new_priority = RTEMS_CURRENT_PRIORITY;
      break;
    }

    case RtemsSemReqSetPriority_Pre_NewPrio_Valid: {
      /*
       * While the ``new_priority`` parameter is not equal to
       * RTEMS_CURRENT_PRIORITY and valid with respect to the scheduler
       * specified by the ``scheduler_id`` parameter.
       */
      ctx->new_priority = PRIO_VERY_HIGH;
      break;
    }

    case RtemsSemReqSetPriority_Pre_NewPrio_Invalid: {
      /*
       * While the ``new_priority`` parameter is invalid with respect to the
       * scheduler specified by the ``scheduler_id`` parameter.
       */
      ctx->new_priority = PRIO_INVALID;
      break;
    }

    case RtemsSemReqSetPriority_Pre_NewPrio_NA:
      break;
  }
}

static void RtemsSemReqSetPriority_Pre_OldPrio_Prepare(
  RtemsSemReqSetPriority_Context    *ctx,
  RtemsSemReqSetPriority_Pre_OldPrio state
)
{
  switch ( state ) {
    case RtemsSemReqSetPriority_Pre_OldPrio_Valid: {
      /*
       * While the ``old_priority`` parameter references an object of type
       * rtems_task_priority.
       */
      ctx->old_priority = &ctx->old_priority_value;
      break;
    }

    case RtemsSemReqSetPriority_Pre_OldPrio_Null: {
      /*
       * While the ``old_priority`` parameter is NULL.
       */
      ctx->old_priority = NULL;
      break;
    }

    case RtemsSemReqSetPriority_Pre_OldPrio_NA:
      break;
  }
}

static void RtemsSemReqSetPriority_Post_Status_Check(
  RtemsSemReqSetPriority_Context    *ctx,
  RtemsSemReqSetPriority_Post_Status state
)
{
  switch ( state ) {
    case RtemsSemReqSetPriority_Post_Status_Ok: {
      /*
       * The return status of rtems_semaphore_set_priority() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsSemReqSetPriority_Post_Status_InvAddr: {
      /*
       * The return status of rtems_semaphore_set_priority() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsSemReqSetPriority_Post_Status_InvId: {
      /*
       * The return status of rtems_semaphore_set_priority() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsSemReqSetPriority_Post_Status_InvPrio: {
      /*
       * The return status of rtems_semaphore_set_priority() shall be
       * RTEMS_INVALID_PRIORITY.
       */
      T_rsc( ctx->status, RTEMS_INVALID_PRIORITY );
      break;
    }

    case RtemsSemReqSetPriority_Post_Status_NotDef: {
      /*
       * The return status of rtems_semaphore_set_priority() shall be
       * RTEMS_NOT_DEFINED.
       */
      T_rsc( ctx->status, RTEMS_NOT_DEFINED );
      break;
    }

    case RtemsSemReqSetPriority_Post_Status_NA:
      break;
  }
}

static void RtemsSemReqSetPriority_Post_OwnerPrio_Check(
  RtemsSemReqSetPriority_Context       *ctx,
  RtemsSemReqSetPriority_Post_OwnerPrio state
)
{
  switch ( state ) {
    case RtemsSemReqSetPriority_Post_OwnerPrio_Nop: {
      /*
       * The current priority of the owner task of the semaphore for the
       * scheduler specified by the ``scheduler_id`` parameter shall not be
       * modified by the rtems_semaphore_set_priority() call.
       */
      T_eq_u32( GetSelfPriority(), PRIO_HIGH );
      ReleaseSemaphore( ctx );
      break;
    }

    case RtemsSemReqSetPriority_Post_OwnerPrio_New: {
      /*
       * The current priority of the owner task of the semaphore for the
       * scheduler specified by the ``scheduler_id`` parameter shall be less
       * than or equal to the value of the ``new_priority`` parameter.
       */
      T_eq_u32( GetSelfPriority(), PRIO_VERY_HIGH );
      ReleaseSemaphore( ctx );
      break;
    }

    case RtemsSemReqSetPriority_Post_OwnerPrio_NA:
      break;
  }
}

static void RtemsSemReqSetPriority_Post_SemPrio_Check(
  RtemsSemReqSetPriority_Context     *ctx,
  RtemsSemReqSetPriority_Post_SemPrio state
)
{
  switch ( state ) {
    case RtemsSemReqSetPriority_Post_SemPrio_Set: {
      /*
       * The priority used for the scheduler specified by the ``scheduler_id``
       * parameter of the semaphore associated with the identifier specified by
       * the ``semaphore_id`` parameter shall be set to the prioriy specified
       * by the ``new_priority`` parameter during the
       * rtems_semaphore_set_priority() call.
       */
      if ( ( ctx->attribute_set & RTEMS_MULTIPROCESSOR_RESOURCE_SHARING ) != 0 ) {
        if ( ctx->scheduler_id == ctx->other_scheduler_id ) {
          CheckPriority( ctx, ctx->runner_scheduler_id, PRIO_HIGH );
          CheckPriority( ctx, ctx->other_scheduler_id, PRIO_VERY_HIGH );
        } else {
          CheckPriority( ctx, ctx->runner_scheduler_id, PRIO_VERY_HIGH );
      #if defined(RTEMS_SMP)
          CheckPriority( ctx, ctx->other_scheduler_id, 0 );
      #endif
        }
      } else if ( ( ctx->attribute_set & RTEMS_PRIORITY_CEILING ) != 0 ) {
        CheckPriority( ctx, ctx->runner_scheduler_id, PRIO_VERY_HIGH );
        CheckNotDefined( ctx, ctx->other_scheduler_id );
      }
      break;
    }

    case RtemsSemReqSetPriority_Post_SemPrio_Nop: {
      /*
       * Priorities used by semaphores shall not be modified by the
       * rtems_semaphore_set_priority() call.
       */
      if ( ( ctx->attribute_set & RTEMS_MULTIPROCESSOR_RESOURCE_SHARING ) != 0 ) {
        CheckPriority( ctx, ctx->runner_scheduler_id, PRIO_HIGH );
      #if defined(RTEMS_SMP)
        CheckPriority( ctx, ctx->other_scheduler_id, 0 );
      #endif
      } else if ( ( ctx->attribute_set & RTEMS_PRIORITY_CEILING ) != 0 ) {
        CheckPriority( ctx, ctx->runner_scheduler_id, PRIO_HIGH );
        CheckNotDefined( ctx, ctx->other_scheduler_id );
      }
      break;
    }

    case RtemsSemReqSetPriority_Post_SemPrio_NA:
      break;
  }
}

static void RtemsSemReqSetPriority_Post_OldPrioVar_Check(
  RtemsSemReqSetPriority_Context        *ctx,
  RtemsSemReqSetPriority_Post_OldPrioVar state
)
{
  switch ( state ) {
    case RtemsSemReqSetPriority_Post_OldPrioVar_Set: {
      /*
       * The value of the object referenced by the ``old_priority`` parameter
       * shall be set to the priority used for the scheduler specified by the
       * ``scheduler_id`` parameter of the semaphore associated with the
       * identifier specified by the ``semaphore_id`` parameter right before
       * the priority is set by the rtems_semaphore_set_priority() call.
       */
      T_eq_ptr( ctx->old_priority, &ctx->old_priority_value );

      if ( ctx->scheduler_id == ctx->other_scheduler_id ) {
        T_eq_u32( ctx->old_priority_value, 0 );
      } else {
        T_eq_u32( ctx->old_priority_value, PRIO_HIGH );
      }
      break;
    }

    case RtemsSemReqSetPriority_Post_OldPrioVar_Nop: {
      /*
       * Objects referenced by the ``old_priority`` parameter in past calls to
       * rtems_semaphore_set_priority() shall not be accessed by the
       * rtems_semaphore_set_priority() call.
       */
      T_eq_u32( ctx->old_priority_value, PRIO_INVALID );
      break;
    }

    case RtemsSemReqSetPriority_Post_OldPrioVar_NA:
      break;
  }
}

static void RtemsSemReqSetPriority_Setup( RtemsSemReqSetPriority_Context *ctx )
{
  rtems_status_code sc;

  memset( ctx, 0, sizeof( *ctx ) );
  SetSelfPriority( PRIO_NORMAL );

  sc = rtems_task_get_scheduler( RTEMS_SELF, &ctx->runner_scheduler_id );
  T_rsc_success( sc );

  #if defined(RTEMS_SMP)
  sc = rtems_scheduler_ident(
    TEST_SCHEDULER_B_NAME,
    &ctx->other_scheduler_id
  );
  T_rsc_success( sc );
  #else
  ctx->other_scheduler_id = INVALID_ID;
  #endif
}

static void RtemsSemReqSetPriority_Setup_Wrap( void *arg )
{
  RtemsSemReqSetPriority_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSemReqSetPriority_Setup( ctx );
}

static void RtemsSemReqSetPriority_Teardown(
  RtemsSemReqSetPriority_Context *ctx
)
{
  RestoreRunnerPriority();
}

static void RtemsSemReqSetPriority_Teardown_Wrap( void *arg )
{
  RtemsSemReqSetPriority_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSemReqSetPriority_Teardown( ctx );
}

static void RtemsSemReqSetPriority_Prepare(
  RtemsSemReqSetPriority_Context *ctx
)
{
  ctx->old_priority_value = PRIO_INVALID;
  ctx->count = 1;
  ctx->attribute_set = RTEMS_PRIORITY;
  ctx->valid_id = true;
}

static void RtemsSemReqSetPriority_Action(
  RtemsSemReqSetPriority_Context *ctx
)
{
  rtems_status_code sc;

  sc = rtems_semaphore_create(
    NAME,
    ctx->count,
    ctx->attribute_set,
    PRIO_HIGH,
    &ctx->the_semaphore_id
  );
  T_rsc_success( sc );

  if ( ctx->valid_id ) {
    ctx->semaphore_id = ctx->the_semaphore_id;
  } else {
    ctx->semaphore_id = INVALID_ID;
  }

  ctx->status = rtems_semaphore_set_priority(
    ctx->semaphore_id,
    ctx->scheduler_id,
    ctx->new_priority,
    ctx->old_priority
  );
}

static void RtemsSemReqSetPriority_Cleanup(
  RtemsSemReqSetPriority_Context *ctx
)
{
  rtems_status_code sc;

  T_eq_u32( GetSelfPriority(), PRIO_NORMAL );

  sc = rtems_semaphore_delete( ctx->the_semaphore_id );
  T_rsc_success( sc );
}

static const RtemsSemReqSetPriority_Entry
RtemsSemReqSetPriority_Entries[] = {
  { 0, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_InvAddr,
    RtemsSemReqSetPriority_Post_OwnerPrio_NA,
    RtemsSemReqSetPriority_Post_SemPrio_Nop,
    RtemsSemReqSetPriority_Post_OldPrioVar_Nop },
  { 0, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_InvId,
    RtemsSemReqSetPriority_Post_OwnerPrio_NA,
    RtemsSemReqSetPriority_Post_SemPrio_Nop,
    RtemsSemReqSetPriority_Post_OldPrioVar_Nop },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_InvAddr,
    RtemsSemReqSetPriority_Post_OwnerPrio_NA,
    RtemsSemReqSetPriority_Post_SemPrio_Nop,
    RtemsSemReqSetPriority_Post_OldPrioVar_Nop },
#else
  { 1, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_NA,
    RtemsSemReqSetPriority_Post_OwnerPrio_NA,
    RtemsSemReqSetPriority_Post_SemPrio_NA,
    RtemsSemReqSetPriority_Post_OldPrioVar_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_InvId,
    RtemsSemReqSetPriority_Post_OwnerPrio_NA,
    RtemsSemReqSetPriority_Post_SemPrio_Nop,
    RtemsSemReqSetPriority_Post_OldPrioVar_Nop },
#else
  { 1, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_NA,
    RtemsSemReqSetPriority_Post_OwnerPrio_NA,
    RtemsSemReqSetPriority_Post_SemPrio_NA,
    RtemsSemReqSetPriority_Post_OldPrioVar_NA },
#endif
  { 0, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_InvAddr,
    RtemsSemReqSetPriority_Post_OwnerPrio_Nop,
    RtemsSemReqSetPriority_Post_SemPrio_Nop,
    RtemsSemReqSetPriority_Post_OldPrioVar_Nop },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_NotDef,
    RtemsSemReqSetPriority_Post_OwnerPrio_NA,
    RtemsSemReqSetPriority_Post_SemPrio_Nop,
    RtemsSemReqSetPriority_Post_OldPrioVar_Nop },
#else
  { 1, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_NA,
    RtemsSemReqSetPriority_Post_OwnerPrio_NA,
    RtemsSemReqSetPriority_Post_SemPrio_NA,
    RtemsSemReqSetPriority_Post_OldPrioVar_NA },
#endif
  { 0, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_InvId,
    RtemsSemReqSetPriority_Post_OwnerPrio_Nop,
    RtemsSemReqSetPriority_Post_SemPrio_Nop,
    RtemsSemReqSetPriority_Post_OldPrioVar_Nop },
  { 0, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_NotDef,
    RtemsSemReqSetPriority_Post_OwnerPrio_NA,
    RtemsSemReqSetPriority_Post_SemPrio_Nop,
    RtemsSemReqSetPriority_Post_OldPrioVar_Nop },
  { 0, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_InvPrio,
    RtemsSemReqSetPriority_Post_OwnerPrio_NA,
    RtemsSemReqSetPriority_Post_SemPrio_Nop,
    RtemsSemReqSetPriority_Post_OldPrioVar_Nop },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_InvPrio,
    RtemsSemReqSetPriority_Post_OwnerPrio_NA,
    RtemsSemReqSetPriority_Post_SemPrio_Nop,
    RtemsSemReqSetPriority_Post_OldPrioVar_Nop },
#else
  { 1, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_NA,
    RtemsSemReqSetPriority_Post_OwnerPrio_NA,
    RtemsSemReqSetPriority_Post_SemPrio_NA,
    RtemsSemReqSetPriority_Post_OldPrioVar_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_InvAddr,
    RtemsSemReqSetPriority_Post_OwnerPrio_Nop,
    RtemsSemReqSetPriority_Post_SemPrio_Nop,
    RtemsSemReqSetPriority_Post_OldPrioVar_Nop },
#else
  { 1, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_NA,
    RtemsSemReqSetPriority_Post_OwnerPrio_NA,
    RtemsSemReqSetPriority_Post_SemPrio_NA,
    RtemsSemReqSetPriority_Post_OldPrioVar_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_InvId,
    RtemsSemReqSetPriority_Post_OwnerPrio_Nop,
    RtemsSemReqSetPriority_Post_SemPrio_Nop,
    RtemsSemReqSetPriority_Post_OldPrioVar_Nop },
#else
  { 1, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_NA,
    RtemsSemReqSetPriority_Post_OwnerPrio_NA,
    RtemsSemReqSetPriority_Post_SemPrio_NA,
    RtemsSemReqSetPriority_Post_OldPrioVar_NA },
#endif
  { 0, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_Ok,
    RtemsSemReqSetPriority_Post_OwnerPrio_NA,
    RtemsSemReqSetPriority_Post_SemPrio_Nop,
    RtemsSemReqSetPriority_Post_OldPrioVar_Set },
  { 0, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_Ok,
    RtemsSemReqSetPriority_Post_OwnerPrio_NA,
    RtemsSemReqSetPriority_Post_SemPrio_Set,
    RtemsSemReqSetPriority_Post_OldPrioVar_Set },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_NotDef,
    RtemsSemReqSetPriority_Post_OwnerPrio_Nop,
    RtemsSemReqSetPriority_Post_SemPrio_Nop,
    RtemsSemReqSetPriority_Post_OldPrioVar_Nop },
#else
  { 1, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_NA,
    RtemsSemReqSetPriority_Post_OwnerPrio_NA,
    RtemsSemReqSetPriority_Post_SemPrio_NA,
    RtemsSemReqSetPriority_Post_OldPrioVar_NA },
#endif
  { 0, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_Ok,
    RtemsSemReqSetPriority_Post_OwnerPrio_Nop,
    RtemsSemReqSetPriority_Post_SemPrio_Nop,
    RtemsSemReqSetPriority_Post_OldPrioVar_Set },
  { 0, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_Ok,
    RtemsSemReqSetPriority_Post_OwnerPrio_New,
    RtemsSemReqSetPriority_Post_SemPrio_Set,
    RtemsSemReqSetPriority_Post_OldPrioVar_Set },
  { 0, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_InvPrio,
    RtemsSemReqSetPriority_Post_OwnerPrio_Nop,
    RtemsSemReqSetPriority_Post_SemPrio_Nop,
    RtemsSemReqSetPriority_Post_OldPrioVar_Nop },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_InvPrio,
    RtemsSemReqSetPriority_Post_OwnerPrio_Nop,
    RtemsSemReqSetPriority_Post_SemPrio_Nop,
    RtemsSemReqSetPriority_Post_OldPrioVar_Nop },
#else
  { 1, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_NA,
    RtemsSemReqSetPriority_Post_OwnerPrio_NA,
    RtemsSemReqSetPriority_Post_SemPrio_NA,
    RtemsSemReqSetPriority_Post_OldPrioVar_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_Ok,
    RtemsSemReqSetPriority_Post_OwnerPrio_NA,
    RtemsSemReqSetPriority_Post_SemPrio_Nop,
    RtemsSemReqSetPriority_Post_OldPrioVar_Set },
#else
  { 1, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_NA,
    RtemsSemReqSetPriority_Post_OwnerPrio_NA,
    RtemsSemReqSetPriority_Post_SemPrio_NA,
    RtemsSemReqSetPriority_Post_OldPrioVar_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_Ok,
    RtemsSemReqSetPriority_Post_OwnerPrio_NA,
    RtemsSemReqSetPriority_Post_SemPrio_Set,
    RtemsSemReqSetPriority_Post_OldPrioVar_Set }
#else
  { 1, 0, 0, 0, 0, 0, RtemsSemReqSetPriority_Post_Status_NA,
    RtemsSemReqSetPriority_Post_OwnerPrio_NA,
    RtemsSemReqSetPriority_Post_SemPrio_NA,
    RtemsSemReqSetPriority_Post_OldPrioVar_NA }
#endif
};

static const uint8_t
RtemsSemReqSetPriority_Map[] = {
  1, 0, 1, 0, 1, 0, 7, 0, 7, 0, 8, 0, 5, 2, 5, 2, 9, 2, 1, 0, 1, 0, 1, 0, 1, 0,
  1, 0, 1, 0, 3, 2, 3, 2, 3, 2, 1, 0, 1, 0, 1, 0, 7, 0, 7, 0, 8, 0, 5, 2, 5, 2,
  9, 2, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 3, 2, 3, 2, 3, 2, 1, 0, 1, 0, 1, 0,
  7, 0, 7, 0, 8, 0, 5, 2, 5, 2, 9, 2, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 3, 2,
  3, 2, 3, 2, 1, 0, 1, 0, 1, 0, 12, 0, 13, 0, 8, 0, 5, 2, 5, 2, 9, 2, 1, 0, 1,
  0, 1, 0, 1, 0, 1, 0, 1, 0, 3, 2, 3, 2, 3, 2, 6, 4, 6, 4, 6, 4, 15, 4, 16, 4,
  17, 4, 14, 10, 14, 10, 18, 10, 6, 4, 6, 4, 6, 4, 6, 4, 6, 4, 6, 4, 11, 10,
  11, 10, 11, 10, 1, 0, 1, 0, 1, 0, 7, 0, 7, 0, 8, 0, 5, 2, 5, 2, 9, 2, 1, 0,
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 3, 2, 3, 2, 3, 2, 1, 0, 1, 0, 1, 0, 12, 0, 13,
  0, 8, 0, 19, 2, 20, 2, 9, 2, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 3, 2, 3, 2,
  3, 2
};

static size_t RtemsSemReqSetPriority_Scope( void *arg, char *buf, size_t n )
{
  RtemsSemReqSetPriority_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsSemReqSetPriority_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsSemReqSetPriority_Fixture = {
  .setup = RtemsSemReqSetPriority_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsSemReqSetPriority_Teardown_Wrap,
  .scope = RtemsSemReqSetPriority_Scope,
  .initial_context = &RtemsSemReqSetPriority_Instance
};

static inline RtemsSemReqSetPriority_Entry RtemsSemReqSetPriority_PopEntry(
  RtemsSemReqSetPriority_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsSemReqSetPriority_Entries[
    RtemsSemReqSetPriority_Map[ index ]
  ];
}

static void RtemsSemReqSetPriority_TestVariant(
  RtemsSemReqSetPriority_Context *ctx
)
{
  RtemsSemReqSetPriority_Pre_Class_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsSemReqSetPriority_Pre_SemId_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsSemReqSetPriority_Pre_SchedId_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsSemReqSetPriority_Pre_NewPrio_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsSemReqSetPriority_Pre_OldPrio_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsSemReqSetPriority_Action( ctx );
  RtemsSemReqSetPriority_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsSemReqSetPriority_Post_OwnerPrio_Check(
    ctx,
    ctx->Map.entry.Post_OwnerPrio
  );
  RtemsSemReqSetPriority_Post_SemPrio_Check(
    ctx,
    ctx->Map.entry.Post_SemPrio
  );
  RtemsSemReqSetPriority_Post_OldPrioVar_Check(
    ctx,
    ctx->Map.entry.Post_OldPrioVar
  );
}

/**
 * @fn void T_case_body_RtemsSemReqSetPriority( void )
 */
T_TEST_CASE_FIXTURE( RtemsSemReqSetPriority, &RtemsSemReqSetPriority_Fixture )
{
  RtemsSemReqSetPriority_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsSemReqSetPriority_Pre_Class_Counting;
    ctx->Map.pcs[ 0 ] < RtemsSemReqSetPriority_Pre_Class_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsSemReqSetPriority_Pre_SemId_Valid;
      ctx->Map.pcs[ 1 ] < RtemsSemReqSetPriority_Pre_SemId_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsSemReqSetPriority_Pre_SchedId_Invalid;
        ctx->Map.pcs[ 2 ] < RtemsSemReqSetPriority_Pre_SchedId_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsSemReqSetPriority_Pre_NewPrio_Current;
          ctx->Map.pcs[ 3 ] < RtemsSemReqSetPriority_Pre_NewPrio_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = RtemsSemReqSetPriority_Pre_OldPrio_Valid;
            ctx->Map.pcs[ 4 ] < RtemsSemReqSetPriority_Pre_OldPrio_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            ctx->Map.entry = RtemsSemReqSetPriority_PopEntry( ctx );

            if ( ctx->Map.entry.Skip ) {
              continue;
            }

            RtemsSemReqSetPriority_Prepare( ctx );
            RtemsSemReqSetPriority_TestVariant( ctx );
            RtemsSemReqSetPriority_Cleanup( ctx );
          }
        }
      }
    }
  }
}

/** @} */
