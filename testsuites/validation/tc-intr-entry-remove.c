/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrReqEntryRemove
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

#include <bsp/irq-generic.h>
#include <rtems/irq-extension.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsIntrReqEntryRemove spec:/rtems/intr/req/entry-remove
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @{
 */

typedef enum {
  RtemsIntrReqEntryRemove_Pre_Vector_Valid,
  RtemsIntrReqEntryRemove_Pre_Vector_Invalid,
  RtemsIntrReqEntryRemove_Pre_Vector_NA
} RtemsIntrReqEntryRemove_Pre_Vector;

typedef enum {
  RtemsIntrReqEntryRemove_Pre_Entry_Obj,
  RtemsIntrReqEntryRemove_Pre_Entry_Null,
  RtemsIntrReqEntryRemove_Pre_Entry_NA
} RtemsIntrReqEntryRemove_Pre_Entry;

typedef enum {
  RtemsIntrReqEntryRemove_Pre_Routine_Valid,
  RtemsIntrReqEntryRemove_Pre_Routine_Null,
  RtemsIntrReqEntryRemove_Pre_Routine_NA
} RtemsIntrReqEntryRemove_Pre_Routine;

typedef enum {
  RtemsIntrReqEntryRemove_Pre_EntryObj_Installed,
  RtemsIntrReqEntryRemove_Pre_EntryObj_Match,
  RtemsIntrReqEntryRemove_Pre_EntryObj_NoMatch,
  RtemsIntrReqEntryRemove_Pre_EntryObj_NA
} RtemsIntrReqEntryRemove_Pre_EntryObj;

typedef enum {
  RtemsIntrReqEntryRemove_Pre_Init_Yes,
  RtemsIntrReqEntryRemove_Pre_Init_No,
  RtemsIntrReqEntryRemove_Pre_Init_NA
} RtemsIntrReqEntryRemove_Pre_Init;

typedef enum {
  RtemsIntrReqEntryRemove_Pre_ISR_Yes,
  RtemsIntrReqEntryRemove_Pre_ISR_No,
  RtemsIntrReqEntryRemove_Pre_ISR_NA
} RtemsIntrReqEntryRemove_Pre_ISR;

typedef enum {
  RtemsIntrReqEntryRemove_Pre_CanDisable_Yes,
  RtemsIntrReqEntryRemove_Pre_CanDisable_Maybe,
  RtemsIntrReqEntryRemove_Pre_CanDisable_No,
  RtemsIntrReqEntryRemove_Pre_CanDisable_NA
} RtemsIntrReqEntryRemove_Pre_CanDisable;

typedef enum {
  RtemsIntrReqEntryRemove_Pre_First_Yes,
  RtemsIntrReqEntryRemove_Pre_First_No,
  RtemsIntrReqEntryRemove_Pre_First_NA
} RtemsIntrReqEntryRemove_Pre_First;

typedef enum {
  RtemsIntrReqEntryRemove_Pre_Last_Yes,
  RtemsIntrReqEntryRemove_Pre_Last_No,
  RtemsIntrReqEntryRemove_Pre_Last_NA
} RtemsIntrReqEntryRemove_Pre_Last;

typedef enum {
  RtemsIntrReqEntryRemove_Post_Status_Ok,
  RtemsIntrReqEntryRemove_Post_Status_InvAddr,
  RtemsIntrReqEntryRemove_Post_Status_IncStat,
  RtemsIntrReqEntryRemove_Post_Status_InvId,
  RtemsIntrReqEntryRemove_Post_Status_CalledFromISR,
  RtemsIntrReqEntryRemove_Post_Status_Unsat,
  RtemsIntrReqEntryRemove_Post_Status_NA
} RtemsIntrReqEntryRemove_Post_Status;

typedef enum {
  RtemsIntrReqEntryRemove_Post_Disabled_Nop,
  RtemsIntrReqEntryRemove_Post_Disabled_Yes,
  RtemsIntrReqEntryRemove_Post_Disabled_Maybe,
  RtemsIntrReqEntryRemove_Post_Disabled_No,
  RtemsIntrReqEntryRemove_Post_Disabled_NA
} RtemsIntrReqEntryRemove_Post_Disabled;

typedef enum {
  RtemsIntrReqEntryRemove_Post_Installed_No,
  RtemsIntrReqEntryRemove_Post_Installed_Nop,
  RtemsIntrReqEntryRemove_Post_Installed_NA
} RtemsIntrReqEntryRemove_Post_Installed;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Vector_NA : 1;
  uint32_t Pre_Entry_NA : 1;
  uint32_t Pre_Routine_NA : 1;
  uint32_t Pre_EntryObj_NA : 1;
  uint32_t Pre_Init_NA : 1;
  uint32_t Pre_ISR_NA : 1;
  uint32_t Pre_CanDisable_NA : 1;
  uint32_t Pre_First_NA : 1;
  uint32_t Pre_Last_NA : 1;
  uint32_t Post_Status : 3;
  uint32_t Post_Disabled : 3;
  uint32_t Post_Installed : 2;
} RtemsIntrReqEntryRemove_Entry;

/**
 * @brief Test context for spec:/rtems/intr/req/entry-remove test case.
 */
typedef struct {
  /**
   * @brief If this member is true, then the service was initialized during
   *   setup.
   */
  bool initialized_during_setup;

  /**
   * @brief If this member is true, then an interrupt occurred.
   */
  bool interrupt_occurred;

  /**
   * @brief This member provides a counter incremented by EntryRoutine().
   */
  uint32_t entry_counter;

  /**
   * @brief This member provides another rtems_interrupt_entry object.
   */
  rtems_interrupt_entry other_entry;

  /**
   * @brief If this member is true, then another entry was installed.
   */
  bool other_installed;

  /**
   * @brief This member provides a third rtems_interrupt_entry object.
   */
  rtems_interrupt_entry third_entry;

  /**
   * @brief If this member is true, then a third entry was installed.
   */
  bool third_installed;

  /**
   * @brief If this member is true, then the entry is installed.
   */
  bool installed;

  /**
   * @brief If this member is true, then the entry is installed as the first
   *   entry.
   */
  bool first;

  /**
   * @brief If this member is true, then the entry is installed as the last
   *   entry.
   */
  bool last;

  /**
   * @brief If this member is true, then the entry matches with another
   *   installed entry.
   */
  bool match;

  /**
   * @brief This member provides the vector number of a testable interrupt
   *   vector.
   */
  rtems_vector_number test_vector;

  /**
   * @brief If this member is true, then the testable interrupt vector was
   *   enabled at the test case begin.
   */
  bool test_vector_was_enabled;

  /**
   * @brief This member provides the attributes of the testable interrupt
   *   vector.
   */
  rtems_interrupt_attributes attributes;

  /**
   * @brief If this member is true, then the service shall be initialized.
   */
  bool initialized;

  /**
   * @brief If this member is true, then rtems_interrupt_entry_remove() shall
   *   be called from within interrupt context.
   */
  bool isr;

  /**
   * @brief This member contains the enabled status before the
   *   rtems_interrupt_entry_remove() call.
   */
  bool enabled_before;

  /**
   * @brief This member contains the enabled status after the
   *   rtems_interrupt_entry_remove() call.
   */
  bool enabled_after;

  /**
   * @brief This member provides the count of visited entries.
   */
  uint32_t visited_entries;

  /**
   * @brief This member provides the rtems_interrupt_entry object.
   */
  rtems_interrupt_entry entry_obj;

  /**
   * @brief This member specifies if the ``vector`` parameter value.
   */
  rtems_vector_number vector;

  /**
   * @brief This member specifies if the ``entry`` parameter value.
   */
  rtems_interrupt_entry *entry;

  /**
   * @brief This member contains the return value of the
   *   rtems_interrupt_entry_remove() call.
   */
  rtems_status_code status;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 9 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 9 ];

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
    RtemsIntrReqEntryRemove_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsIntrReqEntryRemove_Context;

static RtemsIntrReqEntryRemove_Context
  RtemsIntrReqEntryRemove_Instance;

static const char * const RtemsIntrReqEntryRemove_PreDesc_Vector[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsIntrReqEntryRemove_PreDesc_Entry[] = {
  "Obj",
  "Null",
  "NA"
};

static const char * const RtemsIntrReqEntryRemove_PreDesc_Routine[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsIntrReqEntryRemove_PreDesc_EntryObj[] = {
  "Installed",
  "Match",
  "NoMatch",
  "NA"
};

static const char * const RtemsIntrReqEntryRemove_PreDesc_Init[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsIntrReqEntryRemove_PreDesc_ISR[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsIntrReqEntryRemove_PreDesc_CanDisable[] = {
  "Yes",
  "Maybe",
  "No",
  "NA"
};

static const char * const RtemsIntrReqEntryRemove_PreDesc_First[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsIntrReqEntryRemove_PreDesc_Last[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsIntrReqEntryRemove_PreDesc[] = {
  RtemsIntrReqEntryRemove_PreDesc_Vector,
  RtemsIntrReqEntryRemove_PreDesc_Entry,
  RtemsIntrReqEntryRemove_PreDesc_Routine,
  RtemsIntrReqEntryRemove_PreDesc_EntryObj,
  RtemsIntrReqEntryRemove_PreDesc_Init,
  RtemsIntrReqEntryRemove_PreDesc_ISR,
  RtemsIntrReqEntryRemove_PreDesc_CanDisable,
  RtemsIntrReqEntryRemove_PreDesc_First,
  RtemsIntrReqEntryRemove_PreDesc_Last,
  NULL
};

typedef RtemsIntrReqEntryRemove_Context Context;

static char entry_arg;

static char other_arg;

static char third_arg;

static const char info[] = "Info";

static void Install(
  Context                *ctx,
  rtems_interrupt_entry  *entry,
  rtems_interrupt_handler routine,
  void                   *arg
)
{
  rtems_status_code sc;

  rtems_interrupt_entry_initialize(
    entry,
    routine,
    arg,
    info
  );

  sc = rtems_interrupt_entry_install(
    ctx->test_vector,
    RTEMS_INTERRUPT_SHARED,
    entry
  );
  T_rsc_success( sc );
}

static void OtherRoutine( void *arg )
{
  Context          *ctx;
  rtems_status_code sc;

  (void) arg;
  ctx = T_fixture_context();
  sc = rtems_interrupt_vector_disable( ctx->test_vector );
  T_rsc_success( sc );

  ctx->interrupt_occurred = true;
}

static void ThirdRoutine( void *arg )
{
  T_eq_ptr( arg, &third_arg );
  OtherRoutine( NULL );
}

static void EntryRoutine( void *arg )
{
  Context *ctx;

  ctx = T_fixture_context();
  ++ctx->entry_counter;

  T_eq_ptr( arg, &entry_arg );
  OtherRoutine( NULL );
}

static void Action( void *arg )
{
  Context          *ctx;
  rtems_status_code sc;

  ctx = arg;

  sc = rtems_interrupt_vector_is_enabled(
    ctx->test_vector,
    &ctx->enabled_before
  );
  T_rsc_success( sc );

  bsp_interrupt_set_handler_unique(
    BSP_INTERRUPT_DISPATCH_TABLE_SIZE,
    ctx->initialized
  );

  ctx->status = rtems_interrupt_entry_remove( ctx->vector, ctx->entry );

  bsp_interrupt_set_handler_unique(
    BSP_INTERRUPT_DISPATCH_TABLE_SIZE,
    ctx->initialized_during_setup
  );

  sc = rtems_interrupt_vector_is_enabled(
    ctx->test_vector,
    &ctx->enabled_after
  );
  T_rsc_success( sc );
}

static void VisitInstalledNo(
  void                   *arg,
  const char             *entry_info,
  rtems_option            options,
  rtems_interrupt_handler handler_routine,
  void                   *handler_arg
)
{
  Context *ctx;
  uint32_t visited_entries;

  ctx = arg;
  visited_entries = ctx->visited_entries;

  T_eq_u32( options, RTEMS_INTERRUPT_SHARED );
  T_eq_ptr( entry_info, info );

  if ( visited_entries == 0 ) {
    T_eq_ptr( handler_routine, ctx->other_entry.handler );
    T_eq_ptr( handler_arg, ctx->other_entry.arg );
  } else if ( visited_entries == 1 ) {
    T_eq_ptr( handler_routine, ctx->third_entry.handler );
    T_eq_ptr( handler_arg, ctx->third_entry.arg );
  }

  ctx->visited_entries = visited_entries + 1;
}

static void VisitInstalledNop(
  void                   *arg,
  const char             *entry_info,
  rtems_option            options,
  rtems_interrupt_handler handler_routine,
  void                   *handler_arg
)
{
  Context *ctx;
  uint32_t visited_entries;

  ctx = arg;
  visited_entries = ctx->visited_entries;

  T_eq_u32( options, RTEMS_INTERRUPT_SHARED );
  T_eq_ptr( entry_info, info );

  if ( visited_entries == 0 ) {
    if ( ctx->installed && ctx->first ) {
      T_eq_ptr( handler_routine, ctx->entry_obj.handler );
      T_eq_ptr( handler_arg, ctx->entry_obj.arg );
    } else {
      T_eq_ptr( info, ctx->other_entry.info );
      T_eq_ptr( handler_routine, ctx->other_entry.handler );
      T_eq_ptr( handler_arg, ctx->other_entry.arg );
    }
  } else if ( visited_entries == 1 ) {
    if (
      ctx->installed && ( ( !ctx->first && ctx->last ) ||
        ( !ctx->first && !ctx->last ) )
    ) {
      T_eq_ptr( handler_routine, ctx->entry_obj.handler );
      T_eq_ptr( handler_arg, ctx->entry_obj.arg );
    } else {
      T_eq_ptr( handler_routine, ctx->other_entry.handler );
      T_eq_ptr( handler_arg, ctx->other_entry.arg );
    }
  } else if ( visited_entries == 2 ) {
    if ( ctx->installed && !ctx->first && ctx->last ) {
      T_eq_ptr( handler_routine, ctx->entry_obj.handler );
      T_eq_ptr( handler_arg, ctx->entry_obj.arg );
    } else {
      T_eq_ptr( handler_routine, ctx->third_entry.handler );
      T_eq_ptr( handler_arg, ctx->third_entry.arg );
    }
  }

  ctx->visited_entries = visited_entries + 1;
}

static void RtemsIntrReqEntryRemove_Pre_Vector_Prepare(
  RtemsIntrReqEntryRemove_Context   *ctx,
  RtemsIntrReqEntryRemove_Pre_Vector state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryRemove_Pre_Vector_Valid: {
      /*
       * While the ``vector`` parameter is associated with an interrupt vector.
       */
      ctx->vector = ctx->test_vector;
      break;
    }

    case RtemsIntrReqEntryRemove_Pre_Vector_Invalid: {
      /*
       * While the ``vector`` parameter is not associated with an interrupt
       * vector.
       */
      ctx->vector = BSP_INTERRUPT_VECTOR_COUNT;
      break;
    }

    case RtemsIntrReqEntryRemove_Pre_Vector_NA:
      break;
  }
}

static void RtemsIntrReqEntryRemove_Pre_Entry_Prepare(
  RtemsIntrReqEntryRemove_Context  *ctx,
  RtemsIntrReqEntryRemove_Pre_Entry state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryRemove_Pre_Entry_Obj: {
      /*
       * While the ``entry`` parameter references an object of type
       * rtems_interrupt_entry.
       */
      ctx->entry = &ctx->entry_obj;
      break;
    }

    case RtemsIntrReqEntryRemove_Pre_Entry_Null: {
      /*
       * While the ``entry`` parameter is equal to NULL.
       */
      ctx->entry = NULL;
      break;
    }

    case RtemsIntrReqEntryRemove_Pre_Entry_NA:
      break;
  }
}

static void RtemsIntrReqEntryRemove_Pre_Routine_Prepare(
  RtemsIntrReqEntryRemove_Context    *ctx,
  RtemsIntrReqEntryRemove_Pre_Routine state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryRemove_Pre_Routine_Valid: {
      /*
       * While the handler routine of the entry referenced by the ``options``
       * parameter is valid.
       */
      rtems_interrupt_entry_initialize(
        &ctx->entry_obj,
        EntryRoutine,
        &entry_arg,
        info
      );
      break;
    }

    case RtemsIntrReqEntryRemove_Pre_Routine_Null: {
      /*
       * While the handler routine of the entry referenced by the ``options``
       * parameter is equal to NULL.
       */
      rtems_interrupt_entry_initialize(
        &ctx->entry_obj,
        NULL,
        &entry_arg,
        info
      );
      break;
    }

    case RtemsIntrReqEntryRemove_Pre_Routine_NA:
      break;
  }
}

static void RtemsIntrReqEntryRemove_Pre_EntryObj_Prepare(
  RtemsIntrReqEntryRemove_Context     *ctx,
  RtemsIntrReqEntryRemove_Pre_EntryObj state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryRemove_Pre_EntryObj_Installed: {
      /*
       * While the entry referenced by the ``options`` parameter is installed
       * at the interrupt vector specified by the ``vector`` parameter.
       */
      ctx->installed = true;
      break;
    }

    case RtemsIntrReqEntryRemove_Pre_EntryObj_Match: {
      /*
       * While the entry referenced by the ``options`` parameter not installed
       * at the interrupt vector specified by the ``vector`` parameter, while
       * the entry referenced by the ``options`` has a handler routine and
       * argument pair which is equal to the handler routine and argument pair
       * of an entry installed at the interrupt vector specified by the
       * ``vector`` parameter.
       */
      ctx->installed = false;
      ctx->other_installed = true;
      ctx->match = true;
      break;
    }

    case RtemsIntrReqEntryRemove_Pre_EntryObj_NoMatch: {
      /*
       * While the entry referenced by the ``options`` parameter not installed
       * at the interrupt vector specified by the ``vector`` parameter, while
       * the entry referenced by the ``options`` has a handler routine and
       * argument pair which is not equal to the handler routine and argument
       * pairs of all entries installed at the interrupt vector specified by
       * the ``vector`` parameter.
       */
      ctx->installed = false;
      ctx->other_installed = true;
      ctx->match = false;
      break;
    }

    case RtemsIntrReqEntryRemove_Pre_EntryObj_NA:
      break;
  }
}

static void RtemsIntrReqEntryRemove_Pre_Init_Prepare(
  RtemsIntrReqEntryRemove_Context *ctx,
  RtemsIntrReqEntryRemove_Pre_Init state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryRemove_Pre_Init_Yes: {
      /*
       * While the service is initialized.
       */
      ctx->initialized = true;
      break;
    }

    case RtemsIntrReqEntryRemove_Pre_Init_No: {
      /*
       * While the service is not initialized.
       */
      ctx->initialized = false;
      break;
    }

    case RtemsIntrReqEntryRemove_Pre_Init_NA:
      break;
  }
}

static void RtemsIntrReqEntryRemove_Pre_ISR_Prepare(
  RtemsIntrReqEntryRemove_Context *ctx,
  RtemsIntrReqEntryRemove_Pre_ISR  state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryRemove_Pre_ISR_Yes: {
      /*
       * While rtems_interrupt_entry_remove() is called from within interrupt
       * context.
       */
      ctx->isr = true;
      break;
    }

    case RtemsIntrReqEntryRemove_Pre_ISR_No: {
      /*
       * While rtems_interrupt_entry_remove() is not called from within
       * interrupt context.
       */
      ctx->isr = false;
      break;
    }

    case RtemsIntrReqEntryRemove_Pre_ISR_NA:
      break;
  }
}

static void RtemsIntrReqEntryRemove_Pre_CanDisable_Prepare(
  RtemsIntrReqEntryRemove_Context       *ctx,
  RtemsIntrReqEntryRemove_Pre_CanDisable state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryRemove_Pre_CanDisable_Yes: {
      /*
       * While the interrupt vector associated with the ``vector`` parameter
       * can be disabled.
       */
      /*
       * This pre-condition depends on the attributes of an interrupt vector.
       * For the validation test of rtems_interrupt_entry_remove() a testable
       * interrupt vector is determined by GetTestableInterruptVector().  The
       * implementation of rtems_interrupt_entry_remove() uses
       * rtems_interrupt_vector_disable() which is validated separately in detail.
       */
      break;
    }

    case RtemsIntrReqEntryRemove_Pre_CanDisable_Maybe: {
      /*
       * While the interrupt vector associated with the ``vector`` parameter
       * may be disabled.
       */
      /* See comment for ``Yes`` state */
      break;
    }

    case RtemsIntrReqEntryRemove_Pre_CanDisable_No: {
      /*
       * While the interrupt vector associated with the ``vector`` parameter
       * cannot be disabled.
       */
      /* See comment for ``Yes`` state */
      break;
    }

    case RtemsIntrReqEntryRemove_Pre_CanDisable_NA:
      break;
  }
}

static void RtemsIntrReqEntryRemove_Pre_First_Prepare(
  RtemsIntrReqEntryRemove_Context  *ctx,
  RtemsIntrReqEntryRemove_Pre_First state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryRemove_Pre_First_Yes: {
      /*
       * While the entry referenced by the ``entry`` parameter is installed as
       * the first entry at the interrupt vector specified by the ``vector``
       * parameter.
       */
      ctx->first = true;
      break;
    }

    case RtemsIntrReqEntryRemove_Pre_First_No: {
      /*
       * While the entry referenced by the ``entry`` parameter is installed as
       * not the first entry at the interrupt vector specified by the
       * ``vector`` parameter.
       */
      ctx->first = false;
      break;
    }

    case RtemsIntrReqEntryRemove_Pre_First_NA:
      break;
  }
}

static void RtemsIntrReqEntryRemove_Pre_Last_Prepare(
  RtemsIntrReqEntryRemove_Context *ctx,
  RtemsIntrReqEntryRemove_Pre_Last state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryRemove_Pre_Last_Yes: {
      /*
       * While the entry referenced by the ``entry`` parameter is installed as
       * the last entry at the interrupt vector specified by the ``vector``
       * parameter.
       */
      ctx->last = true;
      break;
    }

    case RtemsIntrReqEntryRemove_Pre_Last_No: {
      /*
       * While the entry referenced by the ``entry`` parameter is installed as
       * not the last entry at the interrupt vector specified by the ``vector``
       * parameter.
       */
      ctx->last = false;
      break;
    }

    case RtemsIntrReqEntryRemove_Pre_Last_NA:
      break;
  }
}

static void RtemsIntrReqEntryRemove_Post_Status_Check(
  RtemsIntrReqEntryRemove_Context    *ctx,
  RtemsIntrReqEntryRemove_Post_Status state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryRemove_Post_Status_Ok: {
      /*
       * The return status of rtems_interrupt_entry_remove() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsIntrReqEntryRemove_Post_Status_InvAddr: {
      /*
       * The return status of rtems_interrupt_entry_remove() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsIntrReqEntryRemove_Post_Status_IncStat: {
      /*
       * The return status of rtems_interrupt_entry_remove() shall be
       * RTEMS_INCORRECT_STATE.
       */
      T_rsc( ctx->status, RTEMS_INCORRECT_STATE );
      break;
    }

    case RtemsIntrReqEntryRemove_Post_Status_InvId: {
      /*
       * The return status of rtems_interrupt_entry_remove() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsIntrReqEntryRemove_Post_Status_CalledFromISR: {
      /*
       * The return status of rtems_interrupt_entry_remove() shall be
       * RTEMS_CALLED_FROM_ISR.
       */
      T_rsc( ctx->status, RTEMS_CALLED_FROM_ISR );
      break;
    }

    case RtemsIntrReqEntryRemove_Post_Status_Unsat: {
      /*
       * The return status of rtems_interrupt_entry_remove() shall be
       * RTEMS_UNSATISFIED.
       */
      T_rsc( ctx->status, RTEMS_UNSATISFIED );
      break;
    }

    case RtemsIntrReqEntryRemove_Post_Status_NA:
      break;
  }
}

static void RtemsIntrReqEntryRemove_Post_Disabled_Check(
  RtemsIntrReqEntryRemove_Context      *ctx,
  RtemsIntrReqEntryRemove_Post_Disabled state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryRemove_Post_Disabled_Nop: {
      /*
       * The enabled status of the interrupt vector specified by ``vector``
       * shall not be modified by the rtems_interrupt_entry_remove() call.
       */
      if ( !ctx->interrupt_occurred ) {
        T_eq( ctx->enabled_before, ctx->enabled_after );
      }
      break;
    }

    case RtemsIntrReqEntryRemove_Post_Disabled_Yes: {
      /*
       * The interrupt vector specified by ``vector`` shall be disabled.
       */
      if ( ctx->attributes.can_disable ) {
        T_false( ctx->enabled_after );
      }
      break;
    }

    case RtemsIntrReqEntryRemove_Post_Disabled_Maybe: {
      /*
       * The interrupt vector specified by ``vector`` may be disabled.
       */
      /* The comment of pre-condition ``CanDisable`` for the ``Yes`` state. */
      if ( ctx->attributes.can_disable ) {
        T_false( ctx->enabled_after );
      }
      break;
    }

    case RtemsIntrReqEntryRemove_Post_Disabled_No: {
      /*
       * The interrupt vector specified by ``vector`` shall not be disabled.
       */
      /* The comment of pre-condition ``CanDisable`` for the ``Yes`` state. */
      if ( ctx->attributes.can_disable ) {
        T_false( ctx->enabled_after );
      }
      break;
    }

    case RtemsIntrReqEntryRemove_Post_Disabled_NA:
      break;
  }
}

static void RtemsIntrReqEntryRemove_Post_Installed_Check(
  RtemsIntrReqEntryRemove_Context       *ctx,
  RtemsIntrReqEntryRemove_Post_Installed state
)
{
  rtems_status_code sc;
  uint32_t          expected_entries;

  switch ( state ) {
    case RtemsIntrReqEntryRemove_Post_Installed_No: {
      /*
       * The entry referenced by ``entry`` shall not be installed at the
       * interrupt vector specified by ``vector``.
       */
      ctx->visited_entries = 0;
      sc = rtems_interrupt_handler_iterate(
        ctx->test_vector,
        VisitInstalledNo,
        ctx
      );
      T_rsc_success( sc );

      if ( ctx->other_installed && ctx->third_installed ) {
        expected_entries = 2;
      } else if ( ctx->other_installed ) {
        expected_entries = 1;
      } else {
        expected_entries = 0;
      }

      T_eq_u32( ctx->visited_entries, expected_entries );

      if ( expected_entries > 0 ) {
        ctx->entry_counter = 0;
        bsp_interrupt_handler_dispatch_unchecked( ctx->test_vector );
        T_eq_u32( ctx->entry_counter, 0 );
      } else {
        rtems_interrupt_entry *first;

        first = bsp_interrupt_dispatch_table[
          bsp_interrupt_dispatch_index( ctx->test_vector )
        ];
        T_null( first );
      }
      break;
    }

    case RtemsIntrReqEntryRemove_Post_Installed_Nop: {
      /*
       * The entries installed at the interrupt vector specified by ``vector``
       * shall not be modified by the rtems_interrupt_entry_remove() call.
       */
      ctx->visited_entries = 0;
      sc = rtems_interrupt_handler_iterate(
        ctx->test_vector,
        VisitInstalledNop,
        ctx
      );
      T_rsc_success( sc );

      if ( ctx->installed && ctx->other_installed && ctx->third_installed ) {
        expected_entries = 3;
      } else if ( ctx->installed && ctx->other_installed ) {
        expected_entries = 2;
      } else if ( ctx->installed || ctx->other_installed ) {
        expected_entries = 1;
      } else {
        expected_entries = 0;
      }

      T_eq_u32( ctx->visited_entries, expected_entries );

      if ( ctx->installed ) {
        ctx->entry_counter = 0;
        bsp_interrupt_handler_dispatch_unchecked( ctx->test_vector );
        T_eq_u32( ctx->entry_counter, 1 );
      }
      break;
    }

    case RtemsIntrReqEntryRemove_Post_Installed_NA:
      break;
  }
}

static void RtemsIntrReqEntryRemove_Setup(
  RtemsIntrReqEntryRemove_Context *ctx
)
{
  rtems_status_code sc;

  ctx->initialized_during_setup = bsp_interrupt_is_initialized();
  ctx->test_vector = GetTestableInterruptVector( NULL );
  ctx->test_vector_was_enabled = false;
  (void) rtems_interrupt_vector_is_enabled(
    ctx->test_vector,
    &ctx->test_vector_was_enabled
  );
  sc = rtems_interrupt_get_attributes( ctx->test_vector, &ctx->attributes );
  T_rsc_success( sc );
}

static void RtemsIntrReqEntryRemove_Setup_Wrap( void *arg )
{
  RtemsIntrReqEntryRemove_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsIntrReqEntryRemove_Setup( ctx );
}

static void RtemsIntrReqEntryRemove_Teardown(
  RtemsIntrReqEntryRemove_Context *ctx
)
{
  if ( ctx->test_vector_was_enabled ) {
    (void) rtems_interrupt_vector_enable( ctx->test_vector );
  }
}

static void RtemsIntrReqEntryRemove_Teardown_Wrap( void *arg )
{
  RtemsIntrReqEntryRemove_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsIntrReqEntryRemove_Teardown( ctx );
}

static void RtemsIntrReqEntryRemove_Prepare(
  RtemsIntrReqEntryRemove_Context *ctx
)
{
  ctx->interrupt_occurred = false;
  ctx->installed = false;
  ctx->other_installed = false;
  ctx->third_installed = false;
}

static void RtemsIntrReqEntryRemove_Action(
  RtemsIntrReqEntryRemove_Context *ctx
)
{
  if ( ctx->installed ) {
    ctx->entry = &ctx->entry_obj;

    if ( ctx->first && ctx->last ) {
      Install( ctx, &ctx->entry_obj, EntryRoutine, &entry_arg );
    } else if ( ctx->first ) {
      ctx->other_installed = true;
      Install( ctx, &ctx->entry_obj, EntryRoutine, &entry_arg );
      Install( ctx, &ctx->other_entry, OtherRoutine, &other_arg );
    } else if ( ctx->last ) {
      ctx->other_installed = true;
      Install( ctx, &ctx->other_entry, OtherRoutine, &other_arg );
      Install( ctx, &ctx->entry_obj, EntryRoutine, &entry_arg );
    } else {
      ctx->other_installed = true;
      ctx->third_installed = true;
      Install( ctx, &ctx->other_entry, OtherRoutine, &other_arg );
      Install( ctx, &ctx->entry_obj, EntryRoutine, &entry_arg );
      Install( ctx, &ctx->third_entry, ThirdRoutine, &third_arg );
    }
  } else if ( ctx->other_installed ) {
    ctx->entry = &ctx->entry_obj;

    if ( ctx->match ) {
      Install( ctx, &ctx->other_entry, EntryRoutine, &entry_arg );
    } else {
      Install( ctx, &ctx->other_entry, OtherRoutine, &other_arg );
    }
  }

  if ( ctx->isr ) {
    CallWithinISR( Action, ctx );
  } else {
    Action( ctx );
  }
}

static void RtemsIntrReqEntryRemove_Cleanup(
  RtemsIntrReqEntryRemove_Context *ctx
)
{
  rtems_status_code sc;

  if ( ctx->third_installed ) {
    sc = rtems_interrupt_entry_remove( ctx->test_vector, &ctx->third_entry );
    T_rsc_success( sc );
  }

  if ( ctx->other_installed ) {
    sc = rtems_interrupt_entry_remove( ctx->test_vector, &ctx->other_entry );
    T_rsc_success( sc );
  }

  if ( ctx->installed && ctx->status != RTEMS_SUCCESSFUL ) {
    sc = rtems_interrupt_entry_remove( ctx->test_vector, ctx->entry );
    T_rsc_success( sc );
  }
}

static const RtemsIntrReqEntryRemove_Entry
RtemsIntrReqEntryRemove_Entries[] = {
  { 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, RtemsIntrReqEntryRemove_Post_Status_InvAddr,
    RtemsIntrReqEntryRemove_Post_Disabled_Nop,
    RtemsIntrReqEntryRemove_Post_Installed_NA },
  { 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, RtemsIntrReqEntryRemove_Post_Status_InvAddr,
    RtemsIntrReqEntryRemove_Post_Disabled_NA,
    RtemsIntrReqEntryRemove_Post_Installed_NA },
  { 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, RtemsIntrReqEntryRemove_Post_Status_IncStat,
    RtemsIntrReqEntryRemove_Post_Disabled_Nop,
    RtemsIntrReqEntryRemove_Post_Installed_NA },
  { 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, RtemsIntrReqEntryRemove_Post_Status_IncStat,
    RtemsIntrReqEntryRemove_Post_Disabled_NA,
    RtemsIntrReqEntryRemove_Post_Installed_NA },
  { 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, RtemsIntrReqEntryRemove_Post_Status_InvAddr,
    RtemsIntrReqEntryRemove_Post_Disabled_Nop,
    RtemsIntrReqEntryRemove_Post_Installed_NA },
  { 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, RtemsIntrReqEntryRemove_Post_Status_InvId,
    RtemsIntrReqEntryRemove_Post_Disabled_NA,
    RtemsIntrReqEntryRemove_Post_Installed_NA },
  { 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, RtemsIntrReqEntryRemove_Post_Status_InvAddr,
    RtemsIntrReqEntryRemove_Post_Disabled_NA,
    RtemsIntrReqEntryRemove_Post_Installed_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    RtemsIntrReqEntryRemove_Post_Status_CalledFromISR,
    RtemsIntrReqEntryRemove_Post_Disabled_Nop,
    RtemsIntrReqEntryRemove_Post_Installed_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, RtemsIntrReqEntryRemove_Post_Status_Unsat,
    RtemsIntrReqEntryRemove_Post_Disabled_Nop,
    RtemsIntrReqEntryRemove_Post_Installed_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsIntrReqEntryRemove_Post_Status_CalledFromISR,
    RtemsIntrReqEntryRemove_Post_Disabled_Nop,
    RtemsIntrReqEntryRemove_Post_Installed_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsIntrReqEntryRemove_Post_Status_Ok,
    RtemsIntrReqEntryRemove_Post_Disabled_Nop,
    RtemsIntrReqEntryRemove_Post_Installed_No },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsIntrReqEntryRemove_Post_Status_Ok,
    RtemsIntrReqEntryRemove_Post_Disabled_Yes,
    RtemsIntrReqEntryRemove_Post_Installed_No },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsIntrReqEntryRemove_Post_Status_Ok,
    RtemsIntrReqEntryRemove_Post_Disabled_Maybe,
    RtemsIntrReqEntryRemove_Post_Installed_No },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsIntrReqEntryRemove_Post_Status_Ok,
    RtemsIntrReqEntryRemove_Post_Disabled_No,
    RtemsIntrReqEntryRemove_Post_Installed_No }
};

static const uint8_t
RtemsIntrReqEntryRemove_Map[] = {
  9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 11, 10, 10, 10, 12, 10, 10, 10, 13, 10,
  10, 10, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
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
  0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
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
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

static size_t RtemsIntrReqEntryRemove_Scope( void *arg, char *buf, size_t n )
{
  RtemsIntrReqEntryRemove_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsIntrReqEntryRemove_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsIntrReqEntryRemove_Fixture = {
  .setup = RtemsIntrReqEntryRemove_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsIntrReqEntryRemove_Teardown_Wrap,
  .scope = RtemsIntrReqEntryRemove_Scope,
  .initial_context = &RtemsIntrReqEntryRemove_Instance
};

static inline RtemsIntrReqEntryRemove_Entry RtemsIntrReqEntryRemove_PopEntry(
  RtemsIntrReqEntryRemove_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsIntrReqEntryRemove_Entries[
    RtemsIntrReqEntryRemove_Map[ index ]
  ];
}

static void RtemsIntrReqEntryRemove_SetPreConditionStates(
  RtemsIntrReqEntryRemove_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_Routine_NA ) {
    ctx->Map.pcs[ 2 ] = RtemsIntrReqEntryRemove_Pre_Routine_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_EntryObj_NA ) {
    ctx->Map.pcs[ 3 ] = RtemsIntrReqEntryRemove_Pre_EntryObj_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];

  if ( ctx->Map.entry.Pre_CanDisable_NA ) {
    ctx->Map.pcs[ 6 ] = RtemsIntrReqEntryRemove_Pre_CanDisable_NA;
  } else {
    ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];
  }

  if ( ctx->Map.entry.Pre_First_NA ) {
    ctx->Map.pcs[ 7 ] = RtemsIntrReqEntryRemove_Pre_First_NA;
  } else {
    ctx->Map.pcs[ 7 ] = ctx->Map.pci[ 7 ];
  }

  if ( ctx->Map.entry.Pre_Last_NA ) {
    ctx->Map.pcs[ 8 ] = RtemsIntrReqEntryRemove_Pre_Last_NA;
  } else {
    ctx->Map.pcs[ 8 ] = ctx->Map.pci[ 8 ];
  }
}

static void RtemsIntrReqEntryRemove_TestVariant(
  RtemsIntrReqEntryRemove_Context *ctx
)
{
  RtemsIntrReqEntryRemove_Pre_Vector_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsIntrReqEntryRemove_Pre_Entry_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsIntrReqEntryRemove_Pre_Routine_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsIntrReqEntryRemove_Pre_EntryObj_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsIntrReqEntryRemove_Pre_Init_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsIntrReqEntryRemove_Pre_ISR_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  RtemsIntrReqEntryRemove_Pre_CanDisable_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  RtemsIntrReqEntryRemove_Pre_First_Prepare( ctx, ctx->Map.pcs[ 7 ] );
  RtemsIntrReqEntryRemove_Pre_Last_Prepare( ctx, ctx->Map.pcs[ 8 ] );
  RtemsIntrReqEntryRemove_Action( ctx );
  RtemsIntrReqEntryRemove_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsIntrReqEntryRemove_Post_Disabled_Check(
    ctx,
    ctx->Map.entry.Post_Disabled
  );
  RtemsIntrReqEntryRemove_Post_Installed_Check(
    ctx,
    ctx->Map.entry.Post_Installed
  );
}

/**
 * @fn void T_case_body_RtemsIntrReqEntryRemove( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsIntrReqEntryRemove,
  &RtemsIntrReqEntryRemove_Fixture
)
{
  RtemsIntrReqEntryRemove_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsIntrReqEntryRemove_Pre_Vector_Valid;
    ctx->Map.pci[ 0 ] < RtemsIntrReqEntryRemove_Pre_Vector_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsIntrReqEntryRemove_Pre_Entry_Obj;
      ctx->Map.pci[ 1 ] < RtemsIntrReqEntryRemove_Pre_Entry_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsIntrReqEntryRemove_Pre_Routine_Valid;
        ctx->Map.pci[ 2 ] < RtemsIntrReqEntryRemove_Pre_Routine_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = RtemsIntrReqEntryRemove_Pre_EntryObj_Installed;
          ctx->Map.pci[ 3 ] < RtemsIntrReqEntryRemove_Pre_EntryObj_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = RtemsIntrReqEntryRemove_Pre_Init_Yes;
            ctx->Map.pci[ 4 ] < RtemsIntrReqEntryRemove_Pre_Init_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = RtemsIntrReqEntryRemove_Pre_ISR_Yes;
              ctx->Map.pci[ 5 ] < RtemsIntrReqEntryRemove_Pre_ISR_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] = RtemsIntrReqEntryRemove_Pre_CanDisable_Yes;
                ctx->Map.pci[ 6 ] < RtemsIntrReqEntryRemove_Pre_CanDisable_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                for (
                  ctx->Map.pci[ 7 ] = RtemsIntrReqEntryRemove_Pre_First_Yes;
                  ctx->Map.pci[ 7 ] < RtemsIntrReqEntryRemove_Pre_First_NA;
                  ++ctx->Map.pci[ 7 ]
                ) {
                  for (
                    ctx->Map.pci[ 8 ] = RtemsIntrReqEntryRemove_Pre_Last_Yes;
                    ctx->Map.pci[ 8 ] < RtemsIntrReqEntryRemove_Pre_Last_NA;
                    ++ctx->Map.pci[ 8 ]
                  ) {
                    ctx->Map.entry = RtemsIntrReqEntryRemove_PopEntry( ctx );
                    RtemsIntrReqEntryRemove_SetPreConditionStates( ctx );
                    RtemsIntrReqEntryRemove_Prepare( ctx );
                    RtemsIntrReqEntryRemove_TestVariant( ctx );
                    RtemsIntrReqEntryRemove_Cleanup( ctx );
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
