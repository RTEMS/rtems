/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrReqEntryInstall
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
 * @defgroup RtemsIntrReqEntryInstall spec:/rtems/intr/req/entry-install
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @{
 */

typedef enum {
  RtemsIntrReqEntryInstall_Pre_Vector_Valid,
  RtemsIntrReqEntryInstall_Pre_Vector_Invalid,
  RtemsIntrReqEntryInstall_Pre_Vector_NA
} RtemsIntrReqEntryInstall_Pre_Vector;

typedef enum {
  RtemsIntrReqEntryInstall_Pre_Options_Unique,
  RtemsIntrReqEntryInstall_Pre_Options_Shared,
  RtemsIntrReqEntryInstall_Pre_Options_Replace,
  RtemsIntrReqEntryInstall_Pre_Options_NA
} RtemsIntrReqEntryInstall_Pre_Options;

typedef enum {
  RtemsIntrReqEntryInstall_Pre_Entry_Obj,
  RtemsIntrReqEntryInstall_Pre_Entry_Null,
  RtemsIntrReqEntryInstall_Pre_Entry_NA
} RtemsIntrReqEntryInstall_Pre_Entry;

typedef enum {
  RtemsIntrReqEntryInstall_Pre_Routine_Valid,
  RtemsIntrReqEntryInstall_Pre_Routine_Null,
  RtemsIntrReqEntryInstall_Pre_Routine_NA
} RtemsIntrReqEntryInstall_Pre_Routine;

typedef enum {
  RtemsIntrReqEntryInstall_Pre_Init_Yes,
  RtemsIntrReqEntryInstall_Pre_Init_No,
  RtemsIntrReqEntryInstall_Pre_Init_NA
} RtemsIntrReqEntryInstall_Pre_Init;

typedef enum {
  RtemsIntrReqEntryInstall_Pre_ISR_Yes,
  RtemsIntrReqEntryInstall_Pre_ISR_No,
  RtemsIntrReqEntryInstall_Pre_ISR_NA
} RtemsIntrReqEntryInstall_Pre_ISR;

typedef enum {
  RtemsIntrReqEntryInstall_Pre_CanEnable_Yes,
  RtemsIntrReqEntryInstall_Pre_CanEnable_Maybe,
  RtemsIntrReqEntryInstall_Pre_CanEnable_No,
  RtemsIntrReqEntryInstall_Pre_CanEnable_NA
} RtemsIntrReqEntryInstall_Pre_CanEnable;

typedef enum {
  RtemsIntrReqEntryInstall_Pre_Installed_None,
  RtemsIntrReqEntryInstall_Pre_Installed_Unique,
  RtemsIntrReqEntryInstall_Pre_Installed_Other,
  RtemsIntrReqEntryInstall_Pre_Installed_EqRoutine,
  RtemsIntrReqEntryInstall_Pre_Installed_EqArg,
  RtemsIntrReqEntryInstall_Pre_Installed_Match,
  RtemsIntrReqEntryInstall_Pre_Installed_NA
} RtemsIntrReqEntryInstall_Pre_Installed;

typedef enum {
  RtemsIntrReqEntryInstall_Post_Status_Ok,
  RtemsIntrReqEntryInstall_Post_Status_InvAddr,
  RtemsIntrReqEntryInstall_Post_Status_IncStat,
  RtemsIntrReqEntryInstall_Post_Status_InvId,
  RtemsIntrReqEntryInstall_Post_Status_InvNum,
  RtemsIntrReqEntryInstall_Post_Status_CalledFromISR,
  RtemsIntrReqEntryInstall_Post_Status_InUse,
  RtemsIntrReqEntryInstall_Post_Status_TooMany,
  RtemsIntrReqEntryInstall_Post_Status_NA
} RtemsIntrReqEntryInstall_Post_Status;

typedef enum {
  RtemsIntrReqEntryInstall_Post_Enable_Nop,
  RtemsIntrReqEntryInstall_Post_Enable_Yes,
  RtemsIntrReqEntryInstall_Post_Enable_Maybe,
  RtemsIntrReqEntryInstall_Post_Enable_No,
  RtemsIntrReqEntryInstall_Post_Enable_NA
} RtemsIntrReqEntryInstall_Post_Enable;

typedef enum {
  RtemsIntrReqEntryInstall_Post_Installed_No,
  RtemsIntrReqEntryInstall_Post_Installed_Last,
  RtemsIntrReqEntryInstall_Post_Installed_NA
} RtemsIntrReqEntryInstall_Post_Installed;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Vector_NA : 1;
  uint32_t Pre_Options_NA : 1;
  uint32_t Pre_Entry_NA : 1;
  uint32_t Pre_Routine_NA : 1;
  uint32_t Pre_Init_NA : 1;
  uint32_t Pre_ISR_NA : 1;
  uint32_t Pre_CanEnable_NA : 1;
  uint32_t Pre_Installed_NA : 1;
  uint32_t Post_Status : 4;
  uint32_t Post_Enable : 3;
  uint32_t Post_Installed : 2;
} RtemsIntrReqEntryInstall_Entry;

/**
 * @brief Test context for spec:/rtems/intr/req/entry-install test case.
 */
typedef struct {
  /**
   * @brief If this member is true, then the service was initialized during
   *   setup.
   */
  bool initialized_during_setup;

  /**
   * @brief This member provides a counter for handler invocations.
   */
  uint32_t handler_counter;

  /**
   * @brief This member provides a counter snapshot for each entry.
   */
  uint32_t counter_by_entry[ 3 ];

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
   * @brief If this member is true, then rtems_interrupt_entry_install() shall
   *   be called from within interrupt context.
   */
  bool isr;

  /**
   * @brief This member contains the enabled status before the
   *   rtems_interrupt_entry_install() call.
   */
  bool enabled_before;

  /**
   * @brief This member contains the enabled status after the
   *   rtems_interrupt_entry_install() call.
   */
  bool enabled_after;

  /**
   * @brief This member provides the count of visited entries.
   */
  uint32_t visited_entries;

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
   * @brief If this member is true, then the third entry was installed.
   */
  bool third_installed;

  /**
   * @brief This member provides the options used to install the other
   *   rtems_interrupt_entry object.
   */
  rtems_option other_options;

  /**
   * @brief This member provides the rtems_interrupt_entry object.
   */
  rtems_interrupt_entry entry_obj;

  /**
   * @brief This member specifies if the ``vector`` parameter value.
   */
  rtems_vector_number vector;

  /**
   * @brief This member specifies if the ``options`` parameter value.
   */
  rtems_option options;

  /**
   * @brief This member specifies if the ``entry`` parameter value.
   */
  rtems_interrupt_entry *entry;

  /**
   * @brief This member contains the return value of the
   *   rtems_interrupt_entry_install() call.
   */
  rtems_status_code status;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 8 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 8 ];

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
    RtemsIntrReqEntryInstall_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsIntrReqEntryInstall_Context;

static RtemsIntrReqEntryInstall_Context
  RtemsIntrReqEntryInstall_Instance;

static const char * const RtemsIntrReqEntryInstall_PreDesc_Vector[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsIntrReqEntryInstall_PreDesc_Options[] = {
  "Unique",
  "Shared",
  "Replace",
  "NA"
};

static const char * const RtemsIntrReqEntryInstall_PreDesc_Entry[] = {
  "Obj",
  "Null",
  "NA"
};

static const char * const RtemsIntrReqEntryInstall_PreDesc_Routine[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsIntrReqEntryInstall_PreDesc_Init[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsIntrReqEntryInstall_PreDesc_ISR[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsIntrReqEntryInstall_PreDesc_CanEnable[] = {
  "Yes",
  "Maybe",
  "No",
  "NA"
};

static const char * const RtemsIntrReqEntryInstall_PreDesc_Installed[] = {
  "None",
  "Unique",
  "Other",
  "EqRoutine",
  "EqArg",
  "Match",
  "NA"
};

static const char * const * const RtemsIntrReqEntryInstall_PreDesc[] = {
  RtemsIntrReqEntryInstall_PreDesc_Vector,
  RtemsIntrReqEntryInstall_PreDesc_Options,
  RtemsIntrReqEntryInstall_PreDesc_Entry,
  RtemsIntrReqEntryInstall_PreDesc_Routine,
  RtemsIntrReqEntryInstall_PreDesc_Init,
  RtemsIntrReqEntryInstall_PreDesc_ISR,
  RtemsIntrReqEntryInstall_PreDesc_CanEnable,
  RtemsIntrReqEntryInstall_PreDesc_Installed,
  NULL
};

typedef RtemsIntrReqEntryInstall_Context Context;

static char entry_arg;

static char other_arg;

static char third_arg;

static const char entry_info[] = "Entry";

static const char other_info[] = "Other";

static const char third_info[] = "Third";

static void Install(
  Context                *ctx,
  rtems_option            options,
  rtems_interrupt_handler routine,
  void                   *arg
)
{
  rtems_status_code sc;

  ctx->other_options = options;
  rtems_interrupt_entry_initialize(
    &ctx->other_entry,
    routine,
    arg,
    other_info
  );

  sc = rtems_interrupt_entry_install(
    ctx->test_vector,
    options,
    &ctx->other_entry
  );
  T_rsc_success( sc );

  ctx->other_installed = true;
}

static void Routine( Context *ctx, uint32_t counter )
{
  rtems_status_code sc;

  ctx->handler_counter = counter;

  if ( !ctx->attributes.cleared_by_acknowledge ) {
    sc = ClearSoftwareInterrupt( ctx->test_vector );
    T_rsc_success( sc );
  }

  if ( counter > 3 ) {
    sc = rtems_interrupt_vector_disable( ctx->test_vector );
    T_rsc_success( sc );
  }
}

static void EntryRoutine( void *arg )
{
  Context *ctx;
  uint32_t counter;

  ctx = T_fixture_context();
  counter = ctx->handler_counter + 1;

  if ( arg == &other_arg ) {
    ctx->counter_by_entry[ 1 ] = counter;
  } else {
    ctx->counter_by_entry[ 0 ] = counter;
    T_eq_ptr( arg, &entry_arg );
  }

  Routine( ctx, counter );
}

static void OtherRoutine( void *arg )
{
  Context *ctx;
  uint32_t counter;

  (void) arg;
  ctx = T_fixture_context();
  counter = ctx->handler_counter + 1;
  ctx->counter_by_entry[ 1 ] = counter;
  Routine( ctx, counter );
}

static void ThirdRoutine( void *arg )
{
  Context *ctx;
  uint32_t counter;

  ctx = T_fixture_context();
  counter = ctx->handler_counter + 1;
  ctx->counter_by_entry[ 2 ] = counter;
  T_eq_ptr( arg, &third_arg );
  Routine( ctx, counter );
}

static void InstallThird( Context *ctx )
{
  rtems_status_code sc;

  rtems_interrupt_entry_initialize(
    &ctx->third_entry,
    ThirdRoutine,
    &third_arg,
    third_info
  );

  sc = rtems_interrupt_entry_install(
    ctx->test_vector,
    RTEMS_INTERRUPT_SHARED,
    &ctx->third_entry
  );
  T_rsc_success( sc );

  ctx->third_installed = true;
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

  ctx->status = rtems_interrupt_entry_install(
    ctx->vector,
    ctx->options,
    ctx->entry
  );

  bsp_interrupt_set_handler_unique(
    BSP_INTERRUPT_DISPATCH_TABLE_SIZE,
    ctx->initialized_during_setup
  );

  sc = rtems_interrupt_vector_is_enabled(
    ctx->test_vector,
    &ctx->enabled_after
  );
  T_rsc_success( sc );

  if ( ctx->status == RTEMS_SUCCESSFUL ) {
    sc = RaiseSoftwareInterrupt( ctx->test_vector );
    T_rsc_success( sc );
  }
}

static void VisitInstalled(
  void                   *arg,
  const char             *info,
  rtems_option            options,
  rtems_interrupt_handler handler_routine,
  void                   *handler_arg
)
{
  Context *ctx;
  uint32_t visited_entries;

  ctx = arg;
  visited_entries = ctx->visited_entries;

  if ( visited_entries == 0 && ctx->other_installed ) {
    T_eq_ptr( info, other_info );
    T_eq_u32( options, ctx->other_options );
    T_eq_ptr( handler_routine, ctx->other_entry.handler );
    T_eq_ptr( handler_arg, ctx->other_entry.arg );
  } else if ( visited_entries == 1 && ctx->third_installed ) {
    T_eq_ptr( info, third_info );
    T_eq_u32( options, RTEMS_INTERRUPT_SHARED );
    T_eq_ptr( handler_routine, ThirdRoutine );
    T_eq_ptr( handler_arg, &third_arg );
  } else {
    T_eq_ptr( info, entry_info );
    T_eq_u32( options, ctx->options );
    T_eq_ptr( handler_routine, ctx->entry_obj.handler );
    T_eq_ptr( handler_arg, ctx->entry_obj.arg );
  }

  ctx->visited_entries = visited_entries + 1;
}

static void RtemsIntrReqEntryInstall_Pre_Vector_Prepare(
  RtemsIntrReqEntryInstall_Context   *ctx,
  RtemsIntrReqEntryInstall_Pre_Vector state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Pre_Vector_Valid: {
      /*
       * While the ``vector`` parameter is associated with an interrupt vector.
       */
      ctx->vector = ctx->test_vector;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Vector_Invalid: {
      /*
       * While the ``vector`` parameter is not associated with an interrupt
       * vector.
       */
      ctx->vector = BSP_INTERRUPT_VECTOR_COUNT;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Vector_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Pre_Options_Prepare(
  RtemsIntrReqEntryInstall_Context    *ctx,
  RtemsIntrReqEntryInstall_Pre_Options state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Pre_Options_Unique: {
      /*
       * While the ``options`` indicates that an unique entry shall be
       * installed.
       */
      ctx->options = RTEMS_INTERRUPT_UNIQUE;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Options_Shared: {
      /*
       * While the ``options`` indicates that a shared entry shall be
       * installed.
       */
      ctx->options = RTEMS_INTERRUPT_SHARED;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Options_Replace: {
      /*
       * While the ``options`` indicates that the entry handler routine shall
       * be replaced.
       */
      ctx->options = RTEMS_INTERRUPT_REPLACE;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Options_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Pre_Entry_Prepare(
  RtemsIntrReqEntryInstall_Context  *ctx,
  RtemsIntrReqEntryInstall_Pre_Entry state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Pre_Entry_Obj: {
      /*
       * While the ``entry`` parameter references an object of type
       * rtems_interrupt_entry.
       */
      ctx->entry = &ctx->entry_obj;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Entry_Null: {
      /*
       * While the ``entry`` parameter is equal to NULL.
       */
      ctx->entry = NULL;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Entry_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Pre_Routine_Prepare(
  RtemsIntrReqEntryInstall_Context    *ctx,
  RtemsIntrReqEntryInstall_Pre_Routine state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Pre_Routine_Valid: {
      /*
       * While the handler routine of the object referenced by the ``entry``
       * parameter is valid.
       */
      rtems_interrupt_entry_initialize(
        &ctx->entry_obj,
        EntryRoutine,
        &entry_arg,
        entry_info
      );
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Routine_Null: {
      /*
       * While the handler routine of the object referenced by the ``entry``
       * parameter is equal to NULL.
       */
      rtems_interrupt_entry_initialize(
        &ctx->entry_obj,
        NULL,
        &entry_arg,
        entry_info
      );
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Routine_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Pre_Init_Prepare(
  RtemsIntrReqEntryInstall_Context *ctx,
  RtemsIntrReqEntryInstall_Pre_Init state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Pre_Init_Yes: {
      /*
       * While the service is initialized.
       */
      ctx->initialized = true;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Init_No: {
      /*
       * While the service is not initialized.
       */
      ctx->initialized = false;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Init_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Pre_ISR_Prepare(
  RtemsIntrReqEntryInstall_Context *ctx,
  RtemsIntrReqEntryInstall_Pre_ISR  state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Pre_ISR_Yes: {
      /*
       * While rtems_interrupt_entry_install() is called from within interrupt
       * context.
       */
      ctx->isr = true;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_ISR_No: {
      /*
       * While rtems_interrupt_entry_install() is not called from within
       * interrupt context.
       */
      ctx->isr = false;
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_ISR_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Pre_CanEnable_Prepare(
  RtemsIntrReqEntryInstall_Context      *ctx,
  RtemsIntrReqEntryInstall_Pre_CanEnable state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Pre_CanEnable_Yes: {
      /*
       * While the interrupt vector associated with the ``vector`` parameter
       * can be enabled.
       */
      /*
       * This pre-condition depends on the attributes of an interrupt vector.
       * For the validation test of rtems_interrupt_entry_install() a testable
       * interrupt vector is determined by GetTestableInterruptVector().  The
       * implementation of rtems_interrupt_entry_install() uses
       * rtems_interrupt_vector_enable() which is validated separately in detail.
       */
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_CanEnable_Maybe: {
      /*
       * While the interrupt vector associated with the ``vector`` parameter
       * may be enabled.
       */
      /* See comment for ``Yes`` state */
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_CanEnable_No: {
      /*
       * While the interrupt vector associated with the ``vector`` parameter
       * cannot be enabled.
       */
      /* See comment for ``Yes`` state */
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_CanEnable_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Pre_Installed_Prepare(
  RtemsIntrReqEntryInstall_Context      *ctx,
  RtemsIntrReqEntryInstall_Pre_Installed state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Pre_Installed_None: {
      /*
       * While the no entry is installed at the interrupt vector specified by
       * the ``vector`` parameter.
       */
      /* Nothing to do */
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Installed_Unique: {
      /*
       * While a unique entry is installed at the interrupt vector specified by
       * the ``vector`` parameter.
       */
      Install( ctx, RTEMS_INTERRUPT_UNIQUE, EntryRoutine, &entry_arg );
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Installed_Other: {
      /*
       * While at least one non-unique entry is installed at the interrupt
       * vector specified by the ``vector`` parameter, while all entries
       * installed at the interrupt vector specified by the ``vector``
       * parameter have a handler routine which is not equal to the handler
       * routine of the object referenced by the ``entry`` parameter, while all
       * entries installed at the interrupt vector specified by the ``vector``
       * parameter have a handler argument which is not equal to the handler
       * argument of the object referenced by the ``entry`` parameter.
       */
      Install( ctx, RTEMS_INTERRUPT_SHARED, OtherRoutine, &other_arg );
      InstallThird( ctx );
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Installed_EqRoutine: {
      /*
       * While at least one non-unique entry is installed at the interrupt
       * vector specified by the ``vector`` parameter, while at least one entry
       * installed at the interrupt vector specified by the ``vector``
       * parameter has a handler routine which is equal to the handler routine
       * of the object referenced by the ``entry`` parameter, while all entries
       * installed at the interrupt vector specified by the ``vector``
       * parameter have a handler argument which is not equal to the handler
       * argument of the object referenced by the ``entry`` parameter.
       */
      Install( ctx, RTEMS_INTERRUPT_SHARED, EntryRoutine, &other_arg );
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Installed_EqArg: {
      /*
       * While at least one non-unique entry is installed at the interrupt
       * vector specified by the ``vector`` parameter, while all entries
       * installed at the interrupt vector specified by the ``vector``
       * parameter have a handler routine which is not equal to the handler
       * routine of the object referenced by the ``entry`` parameter, while at
       * least one entry installed at the interrupt vector specified by the
       * ``vector`` parameter has a handler argument which is equal to the
       * handler argument of the object referenced by the ``entry`` parameter.
       */
      Install( ctx, RTEMS_INTERRUPT_SHARED, OtherRoutine, &other_arg );
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Installed_Match: {
      /*
       * While at least one non-unique entry with a handler routine which is
       * equal to the handler routine of the object referenced by the ``entry``
       * parameter and with a handler argument which is equal to the handler
       * argument of the object referenced by the ``entry`` parameter is
       * installed at the interrupt vector specified by the ``vector``
       * parameter.
       */
      Install( ctx, RTEMS_INTERRUPT_SHARED, EntryRoutine, &entry_arg );
      break;
    }

    case RtemsIntrReqEntryInstall_Pre_Installed_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Post_Status_Check(
  RtemsIntrReqEntryInstall_Context    *ctx,
  RtemsIntrReqEntryInstall_Post_Status state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Post_Status_Ok: {
      /*
       * The return status of rtems_interrupt_entry_install() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Status_InvAddr: {
      /*
       * The return status of rtems_interrupt_entry_install() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Status_IncStat: {
      /*
       * The return status of rtems_interrupt_entry_install() shall be
       * RTEMS_INCORRECT_STATE.
       */
      T_rsc( ctx->status, RTEMS_INCORRECT_STATE );
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Status_InvId: {
      /*
       * The return status of rtems_interrupt_entry_install() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Status_InvNum: {
      /*
       * The return status of rtems_interrupt_entry_install() shall be
       * RTEMS_INVALID_NUMBER.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NUMBER );
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Status_CalledFromISR: {
      /*
       * The return status of rtems_interrupt_entry_install() shall be
       * RTEMS_CALLED_FROM_ISR.
       */
      T_rsc( ctx->status, RTEMS_CALLED_FROM_ISR );
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Status_InUse: {
      /*
       * The return status of rtems_interrupt_entry_install() shall be
       * RTEMS_RESOURCE_IN_USE.
       */
      T_rsc( ctx->status, RTEMS_RESOURCE_IN_USE  );
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Status_TooMany: {
      /*
       * The return status of rtems_interrupt_entry_install() shall be
       * RTEMS_TOO_MANY.
       */
      T_rsc( ctx->status, RTEMS_TOO_MANY );
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Status_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Post_Enable_Check(
  RtemsIntrReqEntryInstall_Context    *ctx,
  RtemsIntrReqEntryInstall_Post_Enable state
)
{
  switch ( state ) {
    case RtemsIntrReqEntryInstall_Post_Enable_Nop: {
      /*
       * The enabled status of the interrupt vector specified by ``vector``
       * shall not be modified by the rtems_interrupt_entry_install() call.
       */
      if ( ctx->handler_counter == 0 ) {
        T_eq( ctx->enabled_before, ctx->enabled_after );
      }
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Enable_Yes: {
      /*
       * The interrupt vector specified by ``vector`` shall be enabled.
       */
      T_true( ctx->enabled_after || ctx->handler_counter > 3 );
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Enable_Maybe: {
      /*
       * The interrupt vector specified by ``vector`` may be enabled.
       */
      /*
       * Interrupt vectors which cannot be enabled are not selected as a
       * testable interrupt vector by GetTestableInterruptVector(), so this
       * path is not validated by this test.  See also comment for
       * ``CanEnable`` pre-condition state ``Yes``.
       */
      T_true( ctx->enabled_after || ctx->handler_counter > 3 );
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Enable_No: {
      /*
       * The interrupt vector specified by ``vector`` shall not be enabled.
       */
      /*
       * Interrupt vectors which cannot be enabled are not selected as a
       * testable interrupt vector by GetTestableInterruptVector(), so this
       * path is not validated by this test.  See also comment for
       * ``CanEnable`` pre-condition state ``Yes``.
       */
      T_true( ctx->enabled_after || ctx->handler_counter > 3 );
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Enable_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Post_Installed_Check(
  RtemsIntrReqEntryInstall_Context       *ctx,
  RtemsIntrReqEntryInstall_Post_Installed state
)
{
  rtems_status_code sc;

  ctx->visited_entries = 0;
  sc = rtems_interrupt_handler_iterate(
    ctx->test_vector,
    VisitInstalled,
    ctx
  );
  T_rsc_success( sc );

  if ( ctx->status == RTEMS_SUCCESSFUL ) {
    uint32_t counter;

    counter = 1;

    if ( ctx->other_installed ) {
      T_eq_u32( ctx->counter_by_entry[ 1 ], counter );
      ++counter;
    }

    if ( ctx->third_installed ) {
      T_eq_u32( ctx->counter_by_entry[ 2 ], counter );
      ++counter;
    }

    T_eq_u32( ctx->counter_by_entry[ 0 ], counter );
  }

  switch ( state ) {
    case RtemsIntrReqEntryInstall_Post_Installed_No: {
      /*
       * The entry referenced by ``entry`` shall not be installed at the
       * interrupt vector specified by ``vector``.
       */
      if ( ctx->other_installed && ctx->third_installed ) {
        T_eq_u32( ctx->visited_entries, 2 );
      } else if ( ctx->other_installed ) {
        T_eq_u32( ctx->visited_entries, 1 );
      } else {
        T_eq_u32( ctx->visited_entries, 0 );
      }
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Installed_Last: {
      /*
       * The entry referenced by ``entry`` shall be installed as the last entry
       * at the interrupt vector specified by ``vector``.
       */
      if ( ctx->other_installed && ctx->third_installed ) {
        T_eq_u32( ctx->visited_entries, 3 );
      } else if ( ctx->other_installed ) {
        T_eq_u32( ctx->visited_entries, 2 );
      } else {
        T_eq_u32( ctx->visited_entries, 1 );
      }
      break;
    }

    case RtemsIntrReqEntryInstall_Post_Installed_NA:
      break;
  }
}

static void RtemsIntrReqEntryInstall_Setup(
  RtemsIntrReqEntryInstall_Context *ctx
)
{
  rtems_interrupt_attributes required = {
    .can_raise = true
  };
  rtems_status_code sc;

  ctx->initialized_during_setup = bsp_interrupt_is_initialized();
  ctx->test_vector = GetTestableInterruptVector( &required );
  ctx->test_vector_was_enabled = false;
  (void) rtems_interrupt_vector_is_enabled(
    ctx->test_vector,
    &ctx->test_vector_was_enabled
  );
  sc = rtems_interrupt_get_attributes( ctx->test_vector, &ctx->attributes );
  T_rsc_success( sc );
}

static void RtemsIntrReqEntryInstall_Setup_Wrap( void *arg )
{
  RtemsIntrReqEntryInstall_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsIntrReqEntryInstall_Setup( ctx );
}

static void RtemsIntrReqEntryInstall_Teardown(
  RtemsIntrReqEntryInstall_Context *ctx
)
{
  if ( ctx->test_vector_was_enabled ) {
    (void) rtems_interrupt_vector_enable( ctx->test_vector );
  }
}

static void RtemsIntrReqEntryInstall_Teardown_Wrap( void *arg )
{
  RtemsIntrReqEntryInstall_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsIntrReqEntryInstall_Teardown( ctx );
}

static void RtemsIntrReqEntryInstall_Prepare(
  RtemsIntrReqEntryInstall_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->counter_by_entry ); ++i ) {
    ctx->counter_by_entry[ i ] = 0;
  }

  ctx->handler_counter = 0;
  ctx->other_installed = false;
  ctx->third_installed = false;
}

static void RtemsIntrReqEntryInstall_Action(
  RtemsIntrReqEntryInstall_Context *ctx
)
{
  if ( ctx->isr ) {
    CallWithinISR( Action, ctx );
  } else {
    Action( ctx );
  }
}

static void RtemsIntrReqEntryInstall_Cleanup(
  RtemsIntrReqEntryInstall_Context *ctx
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

  if ( ctx->status == RTEMS_SUCCESSFUL ) {
    sc = rtems_interrupt_entry_remove( ctx->test_vector, ctx->entry );
    T_rsc_success( sc );
  }
}

static const RtemsIntrReqEntryInstall_Entry
RtemsIntrReqEntryInstall_Entries[] = {
  { 0, 0, 0, 0, 1, 0, 0, 0, 0, RtemsIntrReqEntryInstall_Post_Status_InvAddr,
    RtemsIntrReqEntryInstall_Post_Enable_Nop,
    RtemsIntrReqEntryInstall_Post_Installed_NA },
  { 0, 0, 0, 0, 1, 0, 0, 1, 1, RtemsIntrReqEntryInstall_Post_Status_InvAddr,
    RtemsIntrReqEntryInstall_Post_Enable_NA,
    RtemsIntrReqEntryInstall_Post_Installed_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsIntrReqEntryInstall_Post_Status_IncStat,
    RtemsIntrReqEntryInstall_Post_Enable_Nop,
    RtemsIntrReqEntryInstall_Post_Installed_No },
  { 0, 0, 0, 0, 0, 0, 0, 1, 1, RtemsIntrReqEntryInstall_Post_Status_IncStat,
    RtemsIntrReqEntryInstall_Post_Enable_NA,
    RtemsIntrReqEntryInstall_Post_Installed_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsIntrReqEntryInstall_Post_Status_InvAddr,
    RtemsIntrReqEntryInstall_Post_Enable_Nop,
    RtemsIntrReqEntryInstall_Post_Installed_No },
  { 0, 0, 0, 0, 0, 0, 0, 1, 1, RtemsIntrReqEntryInstall_Post_Status_InvId,
    RtemsIntrReqEntryInstall_Post_Enable_NA,
    RtemsIntrReqEntryInstall_Post_Installed_NA },
  { 0, 0, 0, 0, 0, 0, 0, 1, 1, RtemsIntrReqEntryInstall_Post_Status_InvAddr,
    RtemsIntrReqEntryInstall_Post_Enable_NA,
    RtemsIntrReqEntryInstall_Post_Installed_NA },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsIntrReqEntryInstall_Post_Status_CalledFromISR,
    RtemsIntrReqEntryInstall_Post_Enable_Nop,
    RtemsIntrReqEntryInstall_Post_Installed_No },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsIntrReqEntryInstall_Post_Status_InUse,
    RtemsIntrReqEntryInstall_Post_Enable_Nop,
    RtemsIntrReqEntryInstall_Post_Installed_No },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsIntrReqEntryInstall_Post_Status_InvNum,
    RtemsIntrReqEntryInstall_Post_Enable_Nop,
    RtemsIntrReqEntryInstall_Post_Installed_No },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsIntrReqEntryInstall_Post_Status_Ok,
    RtemsIntrReqEntryInstall_Post_Enable_Yes,
    RtemsIntrReqEntryInstall_Post_Installed_Last },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsIntrReqEntryInstall_Post_Status_Ok,
    RtemsIntrReqEntryInstall_Post_Enable_Maybe,
    RtemsIntrReqEntryInstall_Post_Installed_Last },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsIntrReqEntryInstall_Post_Status_Ok,
    RtemsIntrReqEntryInstall_Post_Enable_No,
    RtemsIntrReqEntryInstall_Post_Installed_Last },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsIntrReqEntryInstall_Post_Status_TooMany,
    RtemsIntrReqEntryInstall_Post_Enable_Nop,
    RtemsIntrReqEntryInstall_Post_Installed_No }
};

static const uint8_t
RtemsIntrReqEntryInstall_Map[] = {
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 10, 8, 8, 8, 8, 8, 11,
  8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 10, 8, 10,
  10, 10, 13, 11, 8, 11, 11, 11, 13, 12, 8, 12, 12, 12, 13, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

static size_t RtemsIntrReqEntryInstall_Scope( void *arg, char *buf, size_t n )
{
  RtemsIntrReqEntryInstall_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsIntrReqEntryInstall_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsIntrReqEntryInstall_Fixture = {
  .setup = RtemsIntrReqEntryInstall_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsIntrReqEntryInstall_Teardown_Wrap,
  .scope = RtemsIntrReqEntryInstall_Scope,
  .initial_context = &RtemsIntrReqEntryInstall_Instance
};

static inline RtemsIntrReqEntryInstall_Entry RtemsIntrReqEntryInstall_PopEntry(
  RtemsIntrReqEntryInstall_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsIntrReqEntryInstall_Entries[
    RtemsIntrReqEntryInstall_Map[ index ]
  ];
}

static void RtemsIntrReqEntryInstall_SetPreConditionStates(
  RtemsIntrReqEntryInstall_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];

  if ( ctx->Map.entry.Pre_Routine_NA ) {
    ctx->Map.pcs[ 3 ] = RtemsIntrReqEntryInstall_Pre_Routine_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];

  if ( ctx->Map.entry.Pre_CanEnable_NA ) {
    ctx->Map.pcs[ 6 ] = RtemsIntrReqEntryInstall_Pre_CanEnable_NA;
  } else {
    ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];
  }

  if ( ctx->Map.entry.Pre_Installed_NA ) {
    ctx->Map.pcs[ 7 ] = RtemsIntrReqEntryInstall_Pre_Installed_NA;
  } else {
    ctx->Map.pcs[ 7 ] = ctx->Map.pci[ 7 ];
  }
}

static void RtemsIntrReqEntryInstall_TestVariant(
  RtemsIntrReqEntryInstall_Context *ctx
)
{
  RtemsIntrReqEntryInstall_Pre_Vector_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsIntrReqEntryInstall_Pre_Options_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsIntrReqEntryInstall_Pre_Entry_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsIntrReqEntryInstall_Pre_Routine_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsIntrReqEntryInstall_Pre_Init_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsIntrReqEntryInstall_Pre_ISR_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  RtemsIntrReqEntryInstall_Pre_CanEnable_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  RtemsIntrReqEntryInstall_Pre_Installed_Prepare( ctx, ctx->Map.pcs[ 7 ] );
  RtemsIntrReqEntryInstall_Action( ctx );
  RtemsIntrReqEntryInstall_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsIntrReqEntryInstall_Post_Enable_Check(
    ctx,
    ctx->Map.entry.Post_Enable
  );
  RtemsIntrReqEntryInstall_Post_Installed_Check(
    ctx,
    ctx->Map.entry.Post_Installed
  );
}

/**
 * @fn void T_case_body_RtemsIntrReqEntryInstall( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsIntrReqEntryInstall,
  &RtemsIntrReqEntryInstall_Fixture
)
{
  RtemsIntrReqEntryInstall_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsIntrReqEntryInstall_Pre_Vector_Valid;
    ctx->Map.pci[ 0 ] < RtemsIntrReqEntryInstall_Pre_Vector_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsIntrReqEntryInstall_Pre_Options_Unique;
      ctx->Map.pci[ 1 ] < RtemsIntrReqEntryInstall_Pre_Options_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsIntrReqEntryInstall_Pre_Entry_Obj;
        ctx->Map.pci[ 2 ] < RtemsIntrReqEntryInstall_Pre_Entry_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = RtemsIntrReqEntryInstall_Pre_Routine_Valid;
          ctx->Map.pci[ 3 ] < RtemsIntrReqEntryInstall_Pre_Routine_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = RtemsIntrReqEntryInstall_Pre_Init_Yes;
            ctx->Map.pci[ 4 ] < RtemsIntrReqEntryInstall_Pre_Init_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = RtemsIntrReqEntryInstall_Pre_ISR_Yes;
              ctx->Map.pci[ 5 ] < RtemsIntrReqEntryInstall_Pre_ISR_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] = RtemsIntrReqEntryInstall_Pre_CanEnable_Yes;
                ctx->Map.pci[ 6 ] < RtemsIntrReqEntryInstall_Pre_CanEnable_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                for (
                  ctx->Map.pci[ 7 ] = RtemsIntrReqEntryInstall_Pre_Installed_None;
                  ctx->Map.pci[ 7 ] < RtemsIntrReqEntryInstall_Pre_Installed_NA;
                  ++ctx->Map.pci[ 7 ]
                ) {
                  ctx->Map.entry = RtemsIntrReqEntryInstall_PopEntry( ctx );
                  RtemsIntrReqEntryInstall_SetPreConditionStates( ctx );
                  RtemsIntrReqEntryInstall_Prepare( ctx );
                  RtemsIntrReqEntryInstall_TestVariant( ctx );
                  RtemsIntrReqEntryInstall_Cleanup( ctx );
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
