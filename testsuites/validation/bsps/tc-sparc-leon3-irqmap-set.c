/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup BspSparcLeon3ReqIrqmapSet
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <string.h>
#include <bsp/irq-generic.h>
#include <bsp/irq.h>
#include <bsp/irqimpl.h>
#include <grlib/io.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup BspSparcLeon3ReqIrqmapSet spec:/bsp/sparc/leon3/req/irqmap-set
 *
 * @ingroup TestsuitesBspsValidationBsp0
 *
 * @{
 */

typedef enum {
  BspSparcLeon3ReqIrqmapSet_Pre_InterruptMap_Yes,
  BspSparcLeon3ReqIrqmapSet_Pre_InterruptMap_No,
  BspSparcLeon3ReqIrqmapSet_Pre_InterruptMap_NA
} BspSparcLeon3ReqIrqmapSet_Pre_InterruptMap;

typedef enum {
  BspSparcLeon3ReqIrqmapSet_Pre_BusLine_Zero,
  BspSparcLeon3ReqIrqmapSet_Pre_BusLine_Valid,
  BspSparcLeon3ReqIrqmapSet_Pre_BusLine_Invalid,
  BspSparcLeon3ReqIrqmapSet_Pre_BusLine_NA
} BspSparcLeon3ReqIrqmapSet_Pre_BusLine;

typedef enum {
  BspSparcLeon3ReqIrqmapSet_Pre_Change_Same,
  BspSparcLeon3ReqIrqmapSet_Pre_Change_Other,
  BspSparcLeon3ReqIrqmapSet_Pre_Change_NA
} BspSparcLeon3ReqIrqmapSet_Pre_Change;

typedef enum {
  BspSparcLeon3ReqIrqmapSet_Pre_ControllerLine_Zero,
  BspSparcLeon3ReqIrqmapSet_Pre_ControllerLine_Last,
  BspSparcLeon3ReqIrqmapSet_Pre_ControllerLine_Invalid,
  BspSparcLeon3ReqIrqmapSet_Pre_ControllerLine_NA
} BspSparcLeon3ReqIrqmapSet_Pre_ControllerLine;

typedef enum {
  BspSparcLeon3ReqIrqmapSet_Pre_Handler_Yes,
  BspSparcLeon3ReqIrqmapSet_Pre_Handler_No,
  BspSparcLeon3ReqIrqmapSet_Pre_Handler_NA
} BspSparcLeon3ReqIrqmapSet_Pre_Handler;

typedef enum {
  BspSparcLeon3ReqIrqmapSet_Pre_Init_Yes,
  BspSparcLeon3ReqIrqmapSet_Pre_Init_No,
  BspSparcLeon3ReqIrqmapSet_Pre_Init_NA
} BspSparcLeon3ReqIrqmapSet_Pre_Init;

typedef enum {
  BspSparcLeon3ReqIrqmapSet_Pre_ISR_Yes,
  BspSparcLeon3ReqIrqmapSet_Pre_ISR_No,
  BspSparcLeon3ReqIrqmapSet_Pre_ISR_NA
} BspSparcLeon3ReqIrqmapSet_Pre_ISR;

typedef enum {
  BspSparcLeon3ReqIrqmapSet_Post_Status_Ok,
  BspSparcLeon3ReqIrqmapSet_Post_Status_InvNum,
  BspSparcLeon3ReqIrqmapSet_Post_Status_Unsat,
  BspSparcLeon3ReqIrqmapSet_Post_Status_IncStat,
  BspSparcLeon3ReqIrqmapSet_Post_Status_CalledFromIsr,
  BspSparcLeon3ReqIrqmapSet_Post_Status_InUse,
  BspSparcLeon3ReqIrqmapSet_Post_Status_NA
} BspSparcLeon3ReqIrqmapSet_Post_Status;

typedef enum {
  BspSparcLeon3ReqIrqmapSet_Post_MapEntry_Set,
  BspSparcLeon3ReqIrqmapSet_Post_MapEntry_Nop,
  BspSparcLeon3ReqIrqmapSet_Post_MapEntry_NA
} BspSparcLeon3ReqIrqmapSet_Post_MapEntry;

typedef enum {
  BspSparcLeon3ReqIrqmapSet_Post_OtherEntries_Nop,
  BspSparcLeon3ReqIrqmapSet_Post_OtherEntries_NA
} BspSparcLeon3ReqIrqmapSet_Post_OtherEntries;

typedef enum {
  BspSparcLeon3ReqIrqmapSet_Post_LineState_Nop,
  BspSparcLeon3ReqIrqmapSet_Post_LineState_NA
} BspSparcLeon3ReqIrqmapSet_Post_LineState;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_InterruptMap_NA : 1;
  uint16_t Pre_BusLine_NA : 1;
  uint16_t Pre_Change_NA : 1;
  uint16_t Pre_ControllerLine_NA : 1;
  uint16_t Pre_Handler_NA : 1;
  uint16_t Pre_Init_NA : 1;
  uint16_t Pre_ISR_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_MapEntry : 2;
  uint16_t Post_OtherEntries : 1;
  uint16_t Post_LineState : 1;
} BspSparcLeon3ReqIrqmapSet_Entry;

#define _RTEMS_TMTEST27
#include <tm27.h>

/*
 * The IRQ(A)MP provides 32 controller lines, see the
 * GR740 User's Manual, section 21.3.19, and the
 * GR765 datasheet, section 14.3.20.
 */
#define CONTROLLER_LINE_COUNT 32

#if LEON3_IRQMAP_BUS_LINE_COUNT != 0
#define BUS_LINE_COUNT LEON3_IRQMAP_BUS_LINE_COUNT
#define BUS_LINE_VALID TM27_IRQMAP_BUS_LINE
#else
/* Without the interrupt map, a bus line is a controller line. */
#define BUS_LINE_COUNT CONTROLLER_LINE_COUNT
#define BUS_LINE_VALID 13
#endif

/*
 * The test arranges this controller line for the bus line.  No driver of
 * the test suite uses it.
 */
#define CURRENT_LINE 20

/**
 * @brief Test context for spec:/bsp/sparc/leon3/req/irqmap-set test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `bus_line` parameter value.
   */
  rtems_vector_number bus_line;

  /**
   * @brief This member specifies the `controller_line` parameter value.
   */
  rtems_vector_number controller_line;

  /**
   * @brief If this member is true, then the `controller_line` parameter shall
   *   differ from the current controller line of the bus line.
   */
  bool change;

  /**
   * @brief This member specifies the `controller_line` parameter value if it
   *   differs from the current controller line of the bus line.
   */
  rtems_vector_number other_line;

  /**
   * @brief This member contains the arranged controller line of the bus line.
   */
  rtems_vector_number current_line;

  /**
   * @brief If this member is true, then an interrupt handler shall be
   *   installed on the controller line of the bus line.
   */
  bool handler;

  /**
   * @brief If this member is true, then the interrupt support shall be
   *   initialized.
   */
  bool initialized;

  /**
   * @brief If this member is true, then the interrupt support was initialized
   *   during setup.
   */
  bool initialized_during_setup;

  /**
   * @brief If this member is true, then the directive shall be called from
   *   within interrupt context.
   */
  bool isr;

  /**
   * @brief This member provides the interrupt entry of the handler.
   */
  rtems_interrupt_entry entry;

  /**
   * @brief This member contains the interrupt map copy entry before the
   *   arrangement.
   */
  uint8_t entry_before;

  /**
   * @brief This member contains the interrupt map register of the bus line
   *   before the arrangement.
   */
  uint32_t register_before;

  /**
   * @brief This member contains the interrupt map copy before the call.
   */
  uint8_t mapping_before[ BUS_LINE_COUNT ];

  /**
   * @brief This member contains the interrupt map registers before the call.
   */
  uint32_t map_registers_before[ BUS_LINE_COUNT / 4 ];

  /**
   * @brief This member contains the IRQ(A)MP register block before the call.
   */
  irqamp regs_before;

  /**
   * @brief This member contains the return value of the directive call.
   */
  rtems_status_code status;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 7 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 7 ];

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
    BspSparcLeon3ReqIrqmapSet_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} BspSparcLeon3ReqIrqmapSet_Context;

static BspSparcLeon3ReqIrqmapSet_Context BspSparcLeon3ReqIrqmapSet_Instance;

static const char *const BspSparcLeon3ReqIrqmapSet_PreDesc_InterruptMap[] =
  { "Yes", "No", "NA" };

static const char *const BspSparcLeon3ReqIrqmapSet_PreDesc_BusLine[] =
  { "Zero", "Valid", "Invalid", "NA" };

static const char *const BspSparcLeon3ReqIrqmapSet_PreDesc_Change[] =
  { "Same", "Other", "NA" };

static const char *const BspSparcLeon3ReqIrqmapSet_PreDesc_ControllerLine[] =
  { "Zero", "Last", "Invalid", "NA" };

static const char *const BspSparcLeon3ReqIrqmapSet_PreDesc_Handler[] =
  { "Yes", "No", "NA" };

static const char *const BspSparcLeon3ReqIrqmapSet_PreDesc_Init[] =
  { "Yes", "No", "NA" };

static const char *const BspSparcLeon3ReqIrqmapSet_PreDesc_ISR[] =
  { "Yes", "No", "NA" };

static const char *const *const BspSparcLeon3ReqIrqmapSet_PreDesc[] = {
  BspSparcLeon3ReqIrqmapSet_PreDesc_InterruptMap,
  BspSparcLeon3ReqIrqmapSet_PreDesc_BusLine,
  BspSparcLeon3ReqIrqmapSet_PreDesc_Change,
  BspSparcLeon3ReqIrqmapSet_PreDesc_ControllerLine,
  BspSparcLeon3ReqIrqmapSet_PreDesc_Handler,
  BspSparcLeon3ReqIrqmapSet_PreDesc_Init,
  BspSparcLeon3ReqIrqmapSet_PreDesc_ISR,
  NULL
};

typedef BspSparcLeon3ReqIrqmapSet_Context Context;

static void Handler( void *arg )
{
  (void) arg;
}

#if LEON3_IRQMAP_BUS_LINE_COUNT != 0
static uint32_t GetMapShift( rtems_vector_number bus_line )
{
  return 24 - 8 * ( bus_line % 4 );
}

static uint32_t GetMapField( rtems_vector_number bus_line )
{
  uint32_t value;

  value = grlib_load_32( &LEON3_IrqCtrl_Regs->irqmap[ bus_line / 4 ] );
  return ( value >> GetMapShift( bus_line ) ) & 0xff;
}
#endif

static bool IsArrangedBusLine( const Context *ctx )
{
  return ctx->bus_line == BUS_LINE_VALID;
}

static void Arrange( Context *ctx )
{
  rtems_status_code sc;

#if LEON3_IRQMAP_BUS_LINE_COUNT != 0
  ctx->current_line = CURRENT_LINE;
#else
  ctx->current_line = ctx->bus_line;
#endif

  if ( !IsArrangedBusLine( ctx ) ) {
    ctx->controller_line = CURRENT_LINE;
    return;
  }

#if LEON3_IRQMAP_BUS_LINE_COUNT != 0
  {
    rtems_interrupt_level level;
    uint32_t             *reg;
    uint32_t              shift;
    uint32_t              value;

    reg = &LEON3_IrqCtrl_Regs->irqmap[ ctx->bus_line / 4 ];
    shift = GetMapShift( ctx->bus_line );
    rtems_interrupt_local_disable( level );
    ctx->entry_before = LEON3_IrqCtrl_Mapping[ ctx->bus_line ];
    ctx->register_before = grlib_load_32( reg );
    value = ctx->register_before & ~( UINT32_C( 0xff ) << shift );
    value |= (uint32_t) ctx->current_line << shift;
    grlib_store_32( reg, value );
    LEON3_IrqCtrl_Mapping[ ctx->bus_line ] = (uint8_t) ctx->current_line;
    rtems_interrupt_local_enable( level );
  }
#endif

  if ( ctx->change ) {
    ctx->controller_line = ctx->other_line;
  } else {
    ctx->controller_line = ctx->current_line;
  }

  if ( ctx->handler ) {
    rtems_interrupt_entry_initialize( &ctx->entry, Handler, ctx, "Test" );
    sc = rtems_interrupt_entry_install(
      ctx->bus_line,
      RTEMS_INTERRUPT_SHARED,
      &ctx->entry
    );
    T_rsc_success( sc );
  }
}

static void Restore( Context *ctx )
{
  rtems_status_code sc;

  if ( !IsArrangedBusLine( ctx ) ) {
    return;
  }

  if ( ctx->handler ) {
    sc = rtems_interrupt_entry_remove( ctx->bus_line, &ctx->entry );
    T_rsc_success( sc );
  }

#if LEON3_IRQMAP_BUS_LINE_COUNT != 0
  {
    rtems_interrupt_level level;

    rtems_interrupt_local_disable( level );
    grlib_store_32(
      &LEON3_IrqCtrl_Regs->irqmap[ ctx->bus_line / 4 ],
      ctx->register_before
    );
    LEON3_IrqCtrl_Mapping[ ctx->bus_line ] = ctx->entry_before;
    rtems_interrupt_local_enable( level );
  }
#endif
}

static void SaveState( Context *ctx )
{
  size_t i;

  ctx->regs_before = *LEON3_IrqCtrl_Regs;

#if LEON3_IRQMAP_BUS_LINE_COUNT != 0
  memcpy(
    ctx->mapping_before,
    LEON3_IrqCtrl_Mapping,
    sizeof( ctx->mapping_before )
  );
#endif

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->map_registers_before ); ++i ) {
    ctx->map_registers_before[ i ] = grlib_load_32(
      &LEON3_IrqCtrl_Regs->irqmap[ i ]
    );
  }
}

static void Action( void *arg )
{
  Context *ctx;

  ctx = arg;
  ctx->status = leon3_irqmap_set( ctx->bus_line, ctx->controller_line );
}

static void CheckOtherEntries( const Context *ctx )
{
#if LEON3_IRQMAP_BUS_LINE_COUNT != 0
  size_t i;

  for ( i = 0; i < BUS_LINE_COUNT; ++i ) {
    if ( i != ctx->bus_line ) {
      T_eq_u32( LEON3_IrqCtrl_Mapping[ i ], ctx->mapping_before[ i ] );
    }
  }

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->map_registers_before ); ++i ) {
    uint32_t mask;

    if ( i == ctx->bus_line / 4 ) {
      mask = ~( UINT32_C( 0xff ) << GetMapShift( ctx->bus_line ) );
    } else {
      mask = 0xffffffff;
    }

    T_eq_u32(
      grlib_load_32( &LEON3_IrqCtrl_Regs->irqmap[ i ] ) & mask,
      ctx->map_registers_before[ i ] & mask
    );
  }
#else
  (void) ctx;
#endif
}

static void CheckLineState( const Context *ctx )
{
  const irqamp *regs;
  size_t        i;

  regs = LEON3_IrqCtrl_Regs;
  T_eq_u32( grlib_load_32( &regs->ilevel ), ctx->regs_before.ilevel );
  T_eq_u32( grlib_load_32( &regs->brdcst ), ctx->regs_before.brdcst );

  for ( i = 0; i < RTEMS_ARRAY_SIZE( regs->pimask ); ++i ) {
    T_eq_u32(
      grlib_load_32( &regs->pimask[ i ] ),
      ctx->regs_before.pimask[ i ]
    );
    T_eq_u32(
      grlib_load_32( &regs->piforce[ i ] ),
      ctx->regs_before.piforce[ i ]
    );
  }
}

static void BspSparcLeon3ReqIrqmapSet_Pre_InterruptMap_Prepare(
  BspSparcLeon3ReqIrqmapSet_Context         *ctx,
  BspSparcLeon3ReqIrqmapSet_Pre_InterruptMap state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqIrqmapSet_Pre_InterruptMap_Yes: {
      /*
       * Where the BSP uses the interrupt map.
       */
      #if LEON3_IRQMAP_BUS_LINE_COUNT == 0
      ctx->Map.skip = true;
      #endif
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Pre_InterruptMap_No: {
      /*
       * Where the BSP uses no interrupt map.
       */
      #if LEON3_IRQMAP_BUS_LINE_COUNT != 0
      ctx->Map.skip = true;
      #endif
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Pre_InterruptMap_NA:
      break;
  }
}

static void BspSparcLeon3ReqIrqmapSet_Pre_BusLine_Prepare(
  BspSparcLeon3ReqIrqmapSet_Context    *ctx,
  BspSparcLeon3ReqIrqmapSet_Pre_BusLine state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqIrqmapSet_Pre_BusLine_Zero: {
      /*
       * While the `bus_line` parameter is zero.
       */
      ctx->bus_line = 0;
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Pre_BusLine_Valid: {
      /*
       * While the `bus_line` parameter is greater than zero and less than the
       * count of bus lines.
       */
      ctx->bus_line = BUS_LINE_VALID;
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Pre_BusLine_Invalid: {
      /*
       * While the `bus_line` parameter is greater than or equal to the count
       * of bus lines.
       */
      ctx->bus_line = BUS_LINE_COUNT;
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Pre_BusLine_NA:
      break;
  }
}

static void BspSparcLeon3ReqIrqmapSet_Pre_Change_Prepare(
  BspSparcLeon3ReqIrqmapSet_Context   *ctx,
  BspSparcLeon3ReqIrqmapSet_Pre_Change state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqIrqmapSet_Pre_Change_Same: {
      /*
       * While the `controller_line` parameter is equal to the controller line
       * of the bus line.
       */
      ctx->change = false;
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Pre_Change_Other: {
      /*
       * While the `controller_line` parameter is not equal to the controller
       * line of the bus line.
       */
      ctx->change = true;
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Pre_Change_NA:
      break;
  }
}

static void BspSparcLeon3ReqIrqmapSet_Pre_ControllerLine_Prepare(
  BspSparcLeon3ReqIrqmapSet_Context           *ctx,
  BspSparcLeon3ReqIrqmapSet_Pre_ControllerLine state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqIrqmapSet_Pre_ControllerLine_Zero: {
      /*
       * While the `controller_line` parameter is zero.
       */
      ctx->other_line = 0;
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Pre_ControllerLine_Last: {
      /*
       * While the `controller_line` parameter is the last controller line.
       */
      ctx->other_line = CONTROLLER_LINE_COUNT - 1;
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Pre_ControllerLine_Invalid: {
      /*
       * While the `controller_line` parameter is greater than the last
       * controller line.
       */
      ctx->other_line = CONTROLLER_LINE_COUNT;
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Pre_ControllerLine_NA:
      break;
  }
}

static void BspSparcLeon3ReqIrqmapSet_Pre_Handler_Prepare(
  BspSparcLeon3ReqIrqmapSet_Context    *ctx,
  BspSparcLeon3ReqIrqmapSet_Pre_Handler state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqIrqmapSet_Pre_Handler_Yes: {
      /*
       * While an interrupt handler is installed on the controller line of the
       * bus line.
       */
      ctx->handler = true;
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Pre_Handler_No: {
      /*
       * While no interrupt handler is installed on the controller line of the
       * bus line.
       */
      ctx->handler = false;
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Pre_Handler_NA:
      break;
  }
}

static void BspSparcLeon3ReqIrqmapSet_Pre_Init_Prepare(
  BspSparcLeon3ReqIrqmapSet_Context *ctx,
  BspSparcLeon3ReqIrqmapSet_Pre_Init state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqIrqmapSet_Pre_Init_Yes: {
      /*
       * While the interrupt support is initialized.
       */
      ctx->initialized = true;
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Pre_Init_No: {
      /*
       * While the interrupt support is not initialized.
       */
      ctx->initialized = false;
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Pre_Init_NA:
      break;
  }
}

static void BspSparcLeon3ReqIrqmapSet_Pre_ISR_Prepare(
  BspSparcLeon3ReqIrqmapSet_Context *ctx,
  BspSparcLeon3ReqIrqmapSet_Pre_ISR  state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqIrqmapSet_Pre_ISR_Yes: {
      /*
       * While leon3_irqmap_set() is called from within interrupt context.
       */
      ctx->isr = true;
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Pre_ISR_No: {
      /*
       * While leon3_irqmap_set() is not called from within interrupt context.
       */
      ctx->isr = false;
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Pre_ISR_NA:
      break;
  }
}

static void BspSparcLeon3ReqIrqmapSet_Post_Status_Check(
  BspSparcLeon3ReqIrqmapSet_Context    *ctx,
  BspSparcLeon3ReqIrqmapSet_Post_Status state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqIrqmapSet_Post_Status_Ok: {
      /*
       * The return status of leon3_irqmap_set() shall be RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Post_Status_InvNum: {
      /*
       * The return status of leon3_irqmap_set() shall be RTEMS_INVALID_NUMBER.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NUMBER );
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Post_Status_Unsat: {
      /*
       * The return status of leon3_irqmap_set() shall be RTEMS_UNSATISFIED.
       */
      T_rsc( ctx->status, RTEMS_UNSATISFIED );
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Post_Status_IncStat: {
      /*
       * The return status of leon3_irqmap_set() shall be
       * RTEMS_INCORRECT_STATE.
       */
      T_rsc( ctx->status, RTEMS_INCORRECT_STATE );
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Post_Status_CalledFromIsr: {
      /*
       * The return status of leon3_irqmap_set() shall be
       * RTEMS_CALLED_FROM_ISR.
       */
      T_rsc( ctx->status, RTEMS_CALLED_FROM_ISR );
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Post_Status_InUse: {
      /*
       * The return status of leon3_irqmap_set() shall be
       * RTEMS_RESOURCE_IN_USE.
       */
      T_rsc( ctx->status, RTEMS_RESOURCE_IN_USE );
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Post_Status_NA:
      break;
  }
}

static void BspSparcLeon3ReqIrqmapSet_Post_MapEntry_Check(
  BspSparcLeon3ReqIrqmapSet_Context      *ctx,
  BspSparcLeon3ReqIrqmapSet_Post_MapEntry state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqIrqmapSet_Post_MapEntry_Set: {
      /*
       * The interrupt map entry of the bus line in the interrupt map and in
       * the interrupt map copy shall be set to the controller line specified
       * by the `controller_line` parameter.
       */
      #if LEON3_IRQMAP_BUS_LINE_COUNT != 0
      T_eq_u32( LEON3_IrqCtrl_Mapping[ ctx->bus_line ], ctx->controller_line );
      T_eq_u32( GetMapField( ctx->bus_line ), ctx->controller_line );
      #else
      (void) ctx;
      #endif
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Post_MapEntry_Nop: {
      /*
       * The interrupt map entry of the bus line in the interrupt map and in
       * the interrupt map copy shall not be modified by the function call.
       */
      #if LEON3_IRQMAP_BUS_LINE_COUNT != 0
      T_eq_u32( LEON3_IrqCtrl_Mapping[ ctx->bus_line ], ctx->current_line );
      T_eq_u32( GetMapField( ctx->bus_line ), ctx->current_line );
      #else
      (void) ctx;
      #endif
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Post_MapEntry_NA:
      break;
  }
}

static void BspSparcLeon3ReqIrqmapSet_Post_OtherEntries_Check(
  BspSparcLeon3ReqIrqmapSet_Context          *ctx,
  BspSparcLeon3ReqIrqmapSet_Post_OtherEntries state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqIrqmapSet_Post_OtherEntries_Nop: {
      /*
       * The interrupt map entries of the bus lines other than the bus line
       * specified by the `bus_line` parameter shall not be modified by the
       * function call.
       */
      CheckOtherEntries( ctx );
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Post_OtherEntries_NA:
      break;
  }
}

static void BspSparcLeon3ReqIrqmapSet_Post_LineState_Check(
  BspSparcLeon3ReqIrqmapSet_Context       *ctx,
  BspSparcLeon3ReqIrqmapSet_Post_LineState state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqIrqmapSet_Post_LineState_Nop: {
      /*
       * The ILEVEL, PIMASK, PIFORCE and BRDCST registers of the IRQ(A)MP shall
       * not be modified by the function call.
       */
      CheckLineState( ctx );
      break;
    }

    case BspSparcLeon3ReqIrqmapSet_Post_LineState_NA:
      break;
  }
}

static void BspSparcLeon3ReqIrqmapSet_Setup(
  BspSparcLeon3ReqIrqmapSet_Context *ctx
)
{
  ctx->initialized_during_setup = bsp_interrupt_is_initialized();
}

static void BspSparcLeon3ReqIrqmapSet_Setup_Wrap( void *arg )
{
  BspSparcLeon3ReqIrqmapSet_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  BspSparcLeon3ReqIrqmapSet_Setup( ctx );
}

static void BspSparcLeon3ReqIrqmapSet_Action(
  BspSparcLeon3ReqIrqmapSet_Context *ctx
)
{
  Arrange( ctx );
  SaveState( ctx );

  bsp_interrupt_set_handler_unique(
    BSP_INTERRUPT_DISPATCH_TABLE_SIZE,
    ctx->initialized
  );

  if ( ctx->isr ) {
    CallWithinISR( Action, ctx );
  } else {
    Action( ctx );
  }

  bsp_interrupt_set_handler_unique(
    BSP_INTERRUPT_DISPATCH_TABLE_SIZE,
    ctx->initialized_during_setup
  );
}

static void BspSparcLeon3ReqIrqmapSet_Cleanup(
  BspSparcLeon3ReqIrqmapSet_Context *ctx
)
{
  Restore( ctx );
}

/* clang-format off */

static const BspSparcLeon3ReqIrqmapSet_Entry
BspSparcLeon3ReqIrqmapSet_Entries[] = {
  { 0, 0, 0, 1, 1, 1, 0, 0, BspSparcLeon3ReqIrqmapSet_Post_Status_InvNum,
    BspSparcLeon3ReqIrqmapSet_Post_MapEntry_NA,
    BspSparcLeon3ReqIrqmapSet_Post_OtherEntries_Nop,
    BspSparcLeon3ReqIrqmapSet_Post_LineState_Nop },
  { 0, 0, 0, 1, 1, 1, 0, 0, BspSparcLeon3ReqIrqmapSet_Post_Status_InvNum,
    BspSparcLeon3ReqIrqmapSet_Post_MapEntry_NA,
    BspSparcLeon3ReqIrqmapSet_Post_OtherEntries_NA,
    BspSparcLeon3ReqIrqmapSet_Post_LineState_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, BspSparcLeon3ReqIrqmapSet_Post_Status_Unsat,
    BspSparcLeon3ReqIrqmapSet_Post_MapEntry_NA,
    BspSparcLeon3ReqIrqmapSet_Post_OtherEntries_NA,
    BspSparcLeon3ReqIrqmapSet_Post_LineState_Nop },
  { 0, 0, 0, 0, 1, 0, 0, 0, BspSparcLeon3ReqIrqmapSet_Post_Status_IncStat,
    BspSparcLeon3ReqIrqmapSet_Post_MapEntry_Nop,
    BspSparcLeon3ReqIrqmapSet_Post_OtherEntries_Nop,
    BspSparcLeon3ReqIrqmapSet_Post_LineState_Nop },
  { 0, 0, 0, 0, 1, 0, 0, 0, BspSparcLeon3ReqIrqmapSet_Post_Status_IncStat,
    BspSparcLeon3ReqIrqmapSet_Post_MapEntry_NA,
    BspSparcLeon3ReqIrqmapSet_Post_OtherEntries_NA,
    BspSparcLeon3ReqIrqmapSet_Post_LineState_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, BspSparcLeon3ReqIrqmapSet_Post_Status_IncStat,
    BspSparcLeon3ReqIrqmapSet_Post_MapEntry_Nop,
    BspSparcLeon3ReqIrqmapSet_Post_OtherEntries_Nop,
    BspSparcLeon3ReqIrqmapSet_Post_LineState_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, BspSparcLeon3ReqIrqmapSet_Post_Status_InvNum,
    BspSparcLeon3ReqIrqmapSet_Post_MapEntry_Nop,
    BspSparcLeon3ReqIrqmapSet_Post_OtherEntries_Nop,
    BspSparcLeon3ReqIrqmapSet_Post_LineState_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, BspSparcLeon3ReqIrqmapSet_Post_Status_InvNum,
    BspSparcLeon3ReqIrqmapSet_Post_MapEntry_NA,
    BspSparcLeon3ReqIrqmapSet_Post_OtherEntries_NA,
    BspSparcLeon3ReqIrqmapSet_Post_LineState_Nop },
  { 0, 0, 0, 0, 1, 0, 0, 0,
    BspSparcLeon3ReqIrqmapSet_Post_Status_CalledFromIsr,
    BspSparcLeon3ReqIrqmapSet_Post_MapEntry_Nop,
    BspSparcLeon3ReqIrqmapSet_Post_OtherEntries_Nop,
    BspSparcLeon3ReqIrqmapSet_Post_LineState_Nop },
  { 0, 0, 0, 0, 1, 0, 0, 0, BspSparcLeon3ReqIrqmapSet_Post_Status_Ok,
    BspSparcLeon3ReqIrqmapSet_Post_MapEntry_Set,
    BspSparcLeon3ReqIrqmapSet_Post_OtherEntries_Nop,
    BspSparcLeon3ReqIrqmapSet_Post_LineState_Nop },
  { 0, 0, 0, 0, 1, 0, 0, 0,
    BspSparcLeon3ReqIrqmapSet_Post_Status_CalledFromIsr,
    BspSparcLeon3ReqIrqmapSet_Post_MapEntry_NA,
    BspSparcLeon3ReqIrqmapSet_Post_OtherEntries_NA,
    BspSparcLeon3ReqIrqmapSet_Post_LineState_Nop },
  { 0, 0, 0, 0, 1, 0, 0, 0, BspSparcLeon3ReqIrqmapSet_Post_Status_Ok,
    BspSparcLeon3ReqIrqmapSet_Post_MapEntry_NA,
    BspSparcLeon3ReqIrqmapSet_Post_OtherEntries_NA,
    BspSparcLeon3ReqIrqmapSet_Post_LineState_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0,
    BspSparcLeon3ReqIrqmapSet_Post_Status_CalledFromIsr,
    BspSparcLeon3ReqIrqmapSet_Post_MapEntry_Nop,
    BspSparcLeon3ReqIrqmapSet_Post_OtherEntries_Nop,
    BspSparcLeon3ReqIrqmapSet_Post_LineState_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, BspSparcLeon3ReqIrqmapSet_Post_Status_InUse,
    BspSparcLeon3ReqIrqmapSet_Post_MapEntry_Nop,
    BspSparcLeon3ReqIrqmapSet_Post_OtherEntries_Nop,
    BspSparcLeon3ReqIrqmapSet_Post_LineState_Nop },
  { 0, 0, 0, 0, 0, 0, 0, 0, BspSparcLeon3ReqIrqmapSet_Post_Status_Ok,
    BspSparcLeon3ReqIrqmapSet_Post_MapEntry_Set,
    BspSparcLeon3ReqIrqmapSet_Post_OtherEntries_Nop,
    BspSparcLeon3ReqIrqmapSet_Post_LineState_Nop }
};

static const uint8_t
BspSparcLeon3ReqIrqmapSet_Map[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 9, 3, 3,
  8, 9, 3, 3, 8, 9, 3, 3, 8, 9, 3, 3, 8, 9, 3, 3, 8, 9, 3, 3, 12, 13, 5, 5, 12,
  14, 5, 5, 12, 13, 5, 5, 12, 14, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 10, 11, 4, 4, 10, 11, 4, 4, 10, 11, 4,
  4, 10, 11, 4, 4, 10, 11, 4, 4, 10, 11, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 7, 7, 7, 7, 7, 7, 7, 7, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1
};

/* clang-format on */

static size_t BspSparcLeon3ReqIrqmapSet_Scope( void *arg, char *buf, size_t n )
{
  BspSparcLeon3ReqIrqmapSet_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      BspSparcLeon3ReqIrqmapSet_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture BspSparcLeon3ReqIrqmapSet_Fixture = {
  .setup = BspSparcLeon3ReqIrqmapSet_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = BspSparcLeon3ReqIrqmapSet_Scope,
  .initial_context = &BspSparcLeon3ReqIrqmapSet_Instance
};

static const uint8_t BspSparcLeon3ReqIrqmapSet_Weights[] =
  { 144, 48, 24, 8, 4, 2, 1 };

static void BspSparcLeon3ReqIrqmapSet_Skip(
  BspSparcLeon3ReqIrqmapSet_Context *ctx,
  size_t                             index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pci[ 1 ] = BspSparcLeon3ReqIrqmapSet_Pre_BusLine_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pci[ 2 ] = BspSparcLeon3ReqIrqmapSet_Pre_Change_NA - 1;
      /* Fall through */
    case 3:
      ctx->Map.pci[ 3 ] = BspSparcLeon3ReqIrqmapSet_Pre_ControllerLine_NA - 1;
      /* Fall through */
    case 4:
      ctx->Map.pci[ 4 ] = BspSparcLeon3ReqIrqmapSet_Pre_Handler_NA - 1;
      /* Fall through */
    case 5:
      ctx->Map.pci[ 5 ] = BspSparcLeon3ReqIrqmapSet_Pre_Init_NA - 1;
      /* Fall through */
    case 6:
      ctx->Map.pci[ 6 ] = BspSparcLeon3ReqIrqmapSet_Pre_ISR_NA - 1;
      break;
  }
}

static inline BspSparcLeon3ReqIrqmapSet_Entry
BspSparcLeon3ReqIrqmapSet_PopEntry( BspSparcLeon3ReqIrqmapSet_Context *ctx )
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 7; ++i ) {
      index += BspSparcLeon3ReqIrqmapSet_Weights[ i ] * ctx->Map.pci[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return BspSparcLeon3ReqIrqmapSet_Entries
    [ BspSparcLeon3ReqIrqmapSet_Map[ index ] ];
}

static void BspSparcLeon3ReqIrqmapSet_SetPreConditionStates(
  BspSparcLeon3ReqIrqmapSet_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_Change_NA ) {
    ctx->Map.pcs[ 2 ] = BspSparcLeon3ReqIrqmapSet_Pre_Change_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_ControllerLine_NA ) {
    ctx->Map.pcs[ 3 ] = BspSparcLeon3ReqIrqmapSet_Pre_ControllerLine_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_Handler_NA ) {
    ctx->Map.pcs[ 4 ] = BspSparcLeon3ReqIrqmapSet_Pre_Handler_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  ctx->Map.pcs[ 6 ] = ctx->Map.pci[ 6 ];
}

static void BspSparcLeon3ReqIrqmapSet_TestVariant(
  BspSparcLeon3ReqIrqmapSet_Context *ctx
)
{
  BspSparcLeon3ReqIrqmapSet_Pre_InterruptMap_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip ) {
    BspSparcLeon3ReqIrqmapSet_Skip( ctx, 0 );
    return;
  }

  BspSparcLeon3ReqIrqmapSet_Pre_BusLine_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  BspSparcLeon3ReqIrqmapSet_Pre_Change_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  BspSparcLeon3ReqIrqmapSet_Pre_ControllerLine_Prepare(
    ctx,
    ctx->Map.pcs[ 3 ]
  );
  BspSparcLeon3ReqIrqmapSet_Pre_Handler_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  BspSparcLeon3ReqIrqmapSet_Pre_Init_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  BspSparcLeon3ReqIrqmapSet_Pre_ISR_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  BspSparcLeon3ReqIrqmapSet_Action( ctx );
  BspSparcLeon3ReqIrqmapSet_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  BspSparcLeon3ReqIrqmapSet_Post_MapEntry_Check(
    ctx,
    ctx->Map.entry.Post_MapEntry
  );
  BspSparcLeon3ReqIrqmapSet_Post_OtherEntries_Check(
    ctx,
    ctx->Map.entry.Post_OtherEntries
  );
  BspSparcLeon3ReqIrqmapSet_Post_LineState_Check(
    ctx,
    ctx->Map.entry.Post_LineState
  );
}

/**
 * @fn void T_case_body_BspSparcLeon3ReqIrqmapSet( void )
 */
T_TEST_CASE_FIXTURE(
  BspSparcLeon3ReqIrqmapSet,
  &BspSparcLeon3ReqIrqmapSet_Fixture
)
{
  BspSparcLeon3ReqIrqmapSet_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pci[ 0 ] = BspSparcLeon3ReqIrqmapSet_Pre_InterruptMap_Yes;
    ctx->Map.pci[ 0 ] < BspSparcLeon3ReqIrqmapSet_Pre_InterruptMap_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = BspSparcLeon3ReqIrqmapSet_Pre_BusLine_Zero;
      ctx->Map.pci[ 1 ] < BspSparcLeon3ReqIrqmapSet_Pre_BusLine_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = BspSparcLeon3ReqIrqmapSet_Pre_Change_Same;
        ctx->Map.pci[ 2 ] < BspSparcLeon3ReqIrqmapSet_Pre_Change_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] =
            BspSparcLeon3ReqIrqmapSet_Pre_ControllerLine_Zero;
          ctx->Map.pci[ 3 ] < BspSparcLeon3ReqIrqmapSet_Pre_ControllerLine_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = BspSparcLeon3ReqIrqmapSet_Pre_Handler_Yes;
            ctx->Map.pci[ 4 ] < BspSparcLeon3ReqIrqmapSet_Pre_Handler_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = BspSparcLeon3ReqIrqmapSet_Pre_Init_Yes;
              ctx->Map.pci[ 5 ] < BspSparcLeon3ReqIrqmapSet_Pre_Init_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              for (
                ctx->Map.pci[ 6 ] = BspSparcLeon3ReqIrqmapSet_Pre_ISR_Yes;
                ctx->Map.pci[ 6 ] < BspSparcLeon3ReqIrqmapSet_Pre_ISR_NA;
                ++ctx->Map.pci[ 6 ]
              ) {
                ctx->Map.entry = BspSparcLeon3ReqIrqmapSet_PopEntry( ctx );
                BspSparcLeon3ReqIrqmapSet_SetPreConditionStates( ctx );
                BspSparcLeon3ReqIrqmapSet_TestVariant( ctx );
                BspSparcLeon3ReqIrqmapSet_Cleanup( ctx );
              }
            }
          }
        }
      }
    }
  }
}

/** @} */
