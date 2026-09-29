/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup BspSparcLeon3ReqIrqmapGet
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
#include <bsp/irq.h>
#include <bsp/irqimpl.h>
#include <grlib/io.h>

#include <rtems/test.h>

/**
 * @defgroup BspSparcLeon3ReqIrqmapGet spec:/bsp/sparc/leon3/req/irqmap-get
 *
 * @ingroup TestsuitesBspsValidationBsp0
 *
 * @{
 */

typedef enum {
  BspSparcLeon3ReqIrqmapGet_Pre_InterruptMap_Yes,
  BspSparcLeon3ReqIrqmapGet_Pre_InterruptMap_No,
  BspSparcLeon3ReqIrqmapGet_Pre_InterruptMap_NA
} BspSparcLeon3ReqIrqmapGet_Pre_InterruptMap;

typedef enum {
  BspSparcLeon3ReqIrqmapGet_Pre_ControllerLine_Valid,
  BspSparcLeon3ReqIrqmapGet_Pre_ControllerLine_Null,
  BspSparcLeon3ReqIrqmapGet_Pre_ControllerLine_NA
} BspSparcLeon3ReqIrqmapGet_Pre_ControllerLine;

typedef enum {
  BspSparcLeon3ReqIrqmapGet_Pre_BusLine_First,
  BspSparcLeon3ReqIrqmapGet_Pre_BusLine_Between,
  BspSparcLeon3ReqIrqmapGet_Pre_BusLine_Last,
  BspSparcLeon3ReqIrqmapGet_Pre_BusLine_Invalid,
  BspSparcLeon3ReqIrqmapGet_Pre_BusLine_NA
} BspSparcLeon3ReqIrqmapGet_Pre_BusLine;

typedef enum {
  BspSparcLeon3ReqIrqmapGet_Pre_MapEntry_First,
  BspSparcLeon3ReqIrqmapGet_Pre_MapEntry_Between,
  BspSparcLeon3ReqIrqmapGet_Pre_MapEntry_Last,
  BspSparcLeon3ReqIrqmapGet_Pre_MapEntry_Other,
  BspSparcLeon3ReqIrqmapGet_Pre_MapEntry_NA
} BspSparcLeon3ReqIrqmapGet_Pre_MapEntry;

typedef enum {
  BspSparcLeon3ReqIrqmapGet_Post_Status_Ok,
  BspSparcLeon3ReqIrqmapGet_Post_Status_InvAddr,
  BspSparcLeon3ReqIrqmapGet_Post_Status_InvNum,
  BspSparcLeon3ReqIrqmapGet_Post_Status_NA
} BspSparcLeon3ReqIrqmapGet_Post_Status;

typedef enum {
  BspSparcLeon3ReqIrqmapGet_Post_ControllerLine_Entry,
  BspSparcLeon3ReqIrqmapGet_Post_ControllerLine_BusLine,
  BspSparcLeon3ReqIrqmapGet_Post_ControllerLine_Invalid,
  BspSparcLeon3ReqIrqmapGet_Post_ControllerLine_Nop,
  BspSparcLeon3ReqIrqmapGet_Post_ControllerLine_NA
} BspSparcLeon3ReqIrqmapGet_Post_ControllerLine;

typedef enum {
  BspSparcLeon3ReqIrqmapGet_Post_Map_Nop,
  BspSparcLeon3ReqIrqmapGet_Post_Map_NA
} BspSparcLeon3ReqIrqmapGet_Post_Map;

typedef enum {
  BspSparcLeon3ReqIrqmapGet_Post_MapRegisters_Nop,
  BspSparcLeon3ReqIrqmapGet_Post_MapRegisters_NA
} BspSparcLeon3ReqIrqmapGet_Post_MapRegisters;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_InterruptMap_NA : 1;
  uint16_t Pre_ControllerLine_NA : 1;
  uint16_t Pre_BusLine_NA : 1;
  uint16_t Pre_MapEntry_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_ControllerLine : 3;
  uint16_t Post_Map : 1;
  uint16_t Post_MapRegisters : 1;
} BspSparcLeon3ReqIrqmapGet_Entry;

#define _RTEMS_TMTEST27
#include <tm27.h>

/*
 * The IRQ(A)MP provides 32 controller lines, see the GR740 User's Manual,
 * section 21.3.19, and the GR765 datasheet, section 14.3.20.
 */
#define CONTROLLER_LINE_COUNT 32

#if LEON3_IRQMAP_BUS_LINE_COUNT != 0
#define BUS_LINE_COUNT   LEON3_IRQMAP_BUS_LINE_COUNT
#define BUS_LINE_BETWEEN TM27_IRQMAP_BUS_LINE
#else
/* Without the interrupt map, a bus line is a controller line. */
#define BUS_LINE_COUNT   CONTROLLER_LINE_COUNT
#define BUS_LINE_BETWEEN ( CONTROLLER_LINE_COUNT / 2 )
#endif

/**
 * @brief Test context for spec:/bsp/sparc/leon3/req/irqmap-get test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `bus_line` parameter value.
   */
  rtems_vector_number bus_line;

  /**
   * @brief This member specifies the `controller_line` parameter value.
   */
  rtems_vector_number *controller_line_pointer;

  /**
   * @brief This member specifies the map entry of the bus line.
   */
  rtems_vector_number entry;

  /**
   * @brief This member contains the map entry before the arrangement.
   */
  uint8_t entry_before;

  /**
   * @brief This member contains the interrupt map copy of the BSP before the
   *   call.
   */
  uint8_t mapping_before[ BUS_LINE_COUNT ];

  /**
   * @brief This member contains the interrupt map registers before the call.
   */
  uint32_t map_registers_before[ BUS_LINE_COUNT / 4 ];

  /**
   * @brief This member contains the object referenced by the `controller_line`
   *   parameter.
   */
  rtems_vector_number controller_line;

  /**
   * @brief This member contains the return value of the directive call.
   */
  rtems_status_code status;

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
    BspSparcLeon3ReqIrqmapGet_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} BspSparcLeon3ReqIrqmapGet_Context;

static BspSparcLeon3ReqIrqmapGet_Context BspSparcLeon3ReqIrqmapGet_Instance;

static const char *const BspSparcLeon3ReqIrqmapGet_PreDesc_InterruptMap[] =
  { "Yes", "No", "NA" };

static const char *const BspSparcLeon3ReqIrqmapGet_PreDesc_ControllerLine[] =
  { "Valid", "Null", "NA" };

static const char *const BspSparcLeon3ReqIrqmapGet_PreDesc_BusLine[] =
  { "First", "Between", "Last", "Invalid", "NA" };

static const char *const BspSparcLeon3ReqIrqmapGet_PreDesc_MapEntry[] =
  { "First", "Between", "Last", "Other", "NA" };

static const char *const *const BspSparcLeon3ReqIrqmapGet_PreDesc[] = {
  BspSparcLeon3ReqIrqmapGet_PreDesc_InterruptMap,
  BspSparcLeon3ReqIrqmapGet_PreDesc_ControllerLine,
  BspSparcLeon3ReqIrqmapGet_PreDesc_BusLine,
  BspSparcLeon3ReqIrqmapGet_PreDesc_MapEntry,
  NULL
};

typedef BspSparcLeon3ReqIrqmapGet_Context Context;

#define CONTROLLER_LINE_INITIAL 0x5a5a5a5a

#if LEON3_IRQMAP_BUS_LINE_COUNT != 0
static void SaveMapRegisters( Context *ctx )
{
  size_t i;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->map_registers_before ); ++i ) {
    ctx->map_registers_before[ i ] = grlib_load_32(
      &LEON3_IrqCtrl_Regs->irqmap[ i ]
    );
  }
}

static void CheckMapRegisters( const Context *ctx )
{
  size_t i;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->map_registers_before ); ++i ) {
    T_eq_u32(
      grlib_load_32( &LEON3_IrqCtrl_Regs->irqmap[ i ] ),
      ctx->map_registers_before[ i ]
    );
  }
}
#endif

static void BspSparcLeon3ReqIrqmapGet_Pre_InterruptMap_Prepare(
  BspSparcLeon3ReqIrqmapGet_Context         *ctx,
  BspSparcLeon3ReqIrqmapGet_Pre_InterruptMap state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqIrqmapGet_Pre_InterruptMap_Yes: {
      /*
       * Where the BSP uses the interrupt map.
       */
      #if LEON3_IRQMAP_BUS_LINE_COUNT == 0
      ctx->Map.skip = true;
      #endif
      break;
    }

    case BspSparcLeon3ReqIrqmapGet_Pre_InterruptMap_No: {
      /*
       * Where the BSP uses no interrupt map.
       */
      #if LEON3_IRQMAP_BUS_LINE_COUNT != 0
      ctx->Map.skip = true;
      #endif
      break;
    }

    case BspSparcLeon3ReqIrqmapGet_Pre_InterruptMap_NA:
      break;
  }
}

static void BspSparcLeon3ReqIrqmapGet_Pre_ControllerLine_Prepare(
  BspSparcLeon3ReqIrqmapGet_Context           *ctx,
  BspSparcLeon3ReqIrqmapGet_Pre_ControllerLine state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqIrqmapGet_Pre_ControllerLine_Valid: {
      /*
       * While the `controller_line` parameter references an object of type
       * rtems_vector_number.
       */
      ctx->controller_line_pointer = &ctx->controller_line;
      break;
    }

    case BspSparcLeon3ReqIrqmapGet_Pre_ControllerLine_Null: {
      /*
       * While the `controller_line` parameter is equal to NULL.
       */
      ctx->controller_line_pointer = NULL;
      break;
    }

    case BspSparcLeon3ReqIrqmapGet_Pre_ControllerLine_NA:
      break;
  }
}

static void BspSparcLeon3ReqIrqmapGet_Pre_BusLine_Prepare(
  BspSparcLeon3ReqIrqmapGet_Context    *ctx,
  BspSparcLeon3ReqIrqmapGet_Pre_BusLine state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqIrqmapGet_Pre_BusLine_First: {
      /*
       * While the `bus_line` parameter is the first bus line.
       */
      ctx->bus_line = 0;
      break;
    }

    case BspSparcLeon3ReqIrqmapGet_Pre_BusLine_Between: {
      /*
       * While the `bus_line` parameter is greater than the first and less than
       * the last bus line.
       */
      ctx->bus_line = BUS_LINE_BETWEEN;
      break;
    }

    case BspSparcLeon3ReqIrqmapGet_Pre_BusLine_Last: {
      /*
       * While the `bus_line` parameter is the last bus line.
       */
      ctx->bus_line = BUS_LINE_COUNT - 1;
      break;
    }

    case BspSparcLeon3ReqIrqmapGet_Pre_BusLine_Invalid: {
      /*
       * While the `bus_line` parameter is greater than the last bus line.
       */
      ctx->bus_line = BUS_LINE_COUNT;
      break;
    }

    case BspSparcLeon3ReqIrqmapGet_Pre_BusLine_NA:
      break;
  }
}

static void BspSparcLeon3ReqIrqmapGet_Pre_MapEntry_Prepare(
  BspSparcLeon3ReqIrqmapGet_Context     *ctx,
  BspSparcLeon3ReqIrqmapGet_Pre_MapEntry state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqIrqmapGet_Pre_MapEntry_First: {
      /*
       * While the interrupt map entry of the bus line is the first controller
       * line.
       */
      ctx->entry = 0;
      break;
    }

    case BspSparcLeon3ReqIrqmapGet_Pre_MapEntry_Between: {
      /*
       * While the interrupt map entry of the bus line is greater than the
       * first and less than the last controller line.
       */
      ctx->entry = CONTROLLER_LINE_COUNT / 2 + 1;
      break;
    }

    case BspSparcLeon3ReqIrqmapGet_Pre_MapEntry_Last: {
      /*
       * While the interrupt map entry of the bus line is the last controller
       * line.
       */
      ctx->entry = CONTROLLER_LINE_COUNT - 1;
      break;
    }

    case BspSparcLeon3ReqIrqmapGet_Pre_MapEntry_Other: {
      /*
       * While the interrupt map entry of the bus line is greater than the last
       * controller line.
       */
      ctx->entry = CONTROLLER_LINE_COUNT;
      break;
    }

    case BspSparcLeon3ReqIrqmapGet_Pre_MapEntry_NA:
      break;
  }
}

static void BspSparcLeon3ReqIrqmapGet_Post_Status_Check(
  BspSparcLeon3ReqIrqmapGet_Context    *ctx,
  BspSparcLeon3ReqIrqmapGet_Post_Status state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqIrqmapGet_Post_Status_Ok: {
      /*
       * The return status of leon3_irqmap_get() shall be RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case BspSparcLeon3ReqIrqmapGet_Post_Status_InvAddr: {
      /*
       * The return status of leon3_irqmap_get() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case BspSparcLeon3ReqIrqmapGet_Post_Status_InvNum: {
      /*
       * The return status of leon3_irqmap_get() shall be RTEMS_INVALID_NUMBER.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NUMBER );
      break;
    }

    case BspSparcLeon3ReqIrqmapGet_Post_Status_NA:
      break;
  }
}

static void BspSparcLeon3ReqIrqmapGet_Post_ControllerLine_Check(
  BspSparcLeon3ReqIrqmapGet_Context            *ctx,
  BspSparcLeon3ReqIrqmapGet_Post_ControllerLine state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqIrqmapGet_Post_ControllerLine_Entry: {
      /*
       * The object referenced by the `controller_line` parameter shall be set
       * to the interrupt map entry of the bus line specified by the `bus_line`
       * parameter.
       */
      T_eq_u32( ctx->controller_line, ctx->entry );
      break;
    }

    case BspSparcLeon3ReqIrqmapGet_Post_ControllerLine_BusLine: {
      /*
       * The object referenced by the `controller_line` parameter shall be set
       * to the number of the bus line specified by the `bus_line` parameter.
       */
      T_eq_u32( ctx->controller_line, ctx->bus_line );
      break;
    }

    case BspSparcLeon3ReqIrqmapGet_Post_ControllerLine_Invalid: {
      /*
       * The object referenced by the `controller_line` parameter shall be set
       * to UINT32_MAX.
       */
      T_eq_u32( ctx->controller_line, UINT32_MAX );
      break;
    }

    case BspSparcLeon3ReqIrqmapGet_Post_ControllerLine_Nop: {
      /*
       * The object referenced by the `controller_line` parameter shall not be
       * modified by the function call.
       */
      T_eq_u32( ctx->controller_line, CONTROLLER_LINE_INITIAL );
      break;
    }

    case BspSparcLeon3ReqIrqmapGet_Post_ControllerLine_NA:
      break;
  }
}

static void BspSparcLeon3ReqIrqmapGet_Post_Map_Check(
  BspSparcLeon3ReqIrqmapGet_Context *ctx,
  BspSparcLeon3ReqIrqmapGet_Post_Map state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqIrqmapGet_Post_Map_Nop: {
      /*
       * The interrupt map copy shall not be modified by the function call.
       */
      #if LEON3_IRQMAP_BUS_LINE_COUNT != 0
      T_eq_mem(
        LEON3_IrqCtrl_Mapping,
        ctx->mapping_before,
        sizeof( ctx->mapping_before )
      );
      #else
      (void) ctx;
      #endif
      break;
    }

    case BspSparcLeon3ReqIrqmapGet_Post_Map_NA:
      break;
  }
}

static void BspSparcLeon3ReqIrqmapGet_Post_MapRegisters_Check(
  BspSparcLeon3ReqIrqmapGet_Context          *ctx,
  BspSparcLeon3ReqIrqmapGet_Post_MapRegisters state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqIrqmapGet_Post_MapRegisters_Nop: {
      /*
       * The registers of the interrupt map shall not be modified by the
       * function call.
       */
      #if LEON3_IRQMAP_BUS_LINE_COUNT != 0
      CheckMapRegisters( ctx );
      #else
      (void) ctx;
      #endif
      break;
    }

    case BspSparcLeon3ReqIrqmapGet_Post_MapRegisters_NA:
      break;
  }
}

static void BspSparcLeon3ReqIrqmapGet_Action(
  BspSparcLeon3ReqIrqmapGet_Context *ctx
)
{
  #if LEON3_IRQMAP_BUS_LINE_COUNT != 0
  if ( ctx->bus_line < BUS_LINE_COUNT ) {
    ctx->entry_before = LEON3_IrqCtrl_Mapping[ ctx->bus_line ];
    LEON3_IrqCtrl_Mapping[ ctx->bus_line ] = (uint8_t) ctx->entry;
  }

  memcpy(
    ctx->mapping_before,
    LEON3_IrqCtrl_Mapping,
    sizeof( ctx->mapping_before )
  );
  SaveMapRegisters( ctx );
  #endif

  ctx->controller_line = CONTROLLER_LINE_INITIAL;
  ctx->status = leon3_irqmap_get(
    ctx->bus_line,
    ctx->controller_line_pointer
  );
}

static void BspSparcLeon3ReqIrqmapGet_Cleanup(
  BspSparcLeon3ReqIrqmapGet_Context *ctx
)
{
  #if LEON3_IRQMAP_BUS_LINE_COUNT != 0
  if ( ctx->bus_line < BUS_LINE_COUNT ) {
    LEON3_IrqCtrl_Mapping[ ctx->bus_line ] = ctx->entry_before;
  }
  #else
  (void) ctx;
  #endif
}

/* clang-format off */

static const BspSparcLeon3ReqIrqmapGet_Entry
BspSparcLeon3ReqIrqmapGet_Entries[] = {
  { 0, 0, 0, 0, 1, BspSparcLeon3ReqIrqmapGet_Post_Status_InvAddr,
    BspSparcLeon3ReqIrqmapGet_Post_ControllerLine_Nop,
    BspSparcLeon3ReqIrqmapGet_Post_Map_Nop,
    BspSparcLeon3ReqIrqmapGet_Post_MapRegisters_Nop },
  { 0, 0, 0, 0, 1, BspSparcLeon3ReqIrqmapGet_Post_Status_InvAddr,
    BspSparcLeon3ReqIrqmapGet_Post_ControllerLine_Nop,
    BspSparcLeon3ReqIrqmapGet_Post_Map_NA,
    BspSparcLeon3ReqIrqmapGet_Post_MapRegisters_NA },
  { 0, 0, 0, 0, 0, BspSparcLeon3ReqIrqmapGet_Post_Status_Ok,
    BspSparcLeon3ReqIrqmapGet_Post_ControllerLine_Entry,
    BspSparcLeon3ReqIrqmapGet_Post_Map_Nop,
    BspSparcLeon3ReqIrqmapGet_Post_MapRegisters_Nop },
  { 0, 0, 0, 0, 1, BspSparcLeon3ReqIrqmapGet_Post_Status_Ok,
    BspSparcLeon3ReqIrqmapGet_Post_ControllerLine_BusLine,
    BspSparcLeon3ReqIrqmapGet_Post_Map_NA,
    BspSparcLeon3ReqIrqmapGet_Post_MapRegisters_NA },
  { 0, 0, 0, 0, 1, BspSparcLeon3ReqIrqmapGet_Post_Status_InvNum,
    BspSparcLeon3ReqIrqmapGet_Post_ControllerLine_Invalid,
    BspSparcLeon3ReqIrqmapGet_Post_Map_Nop,
    BspSparcLeon3ReqIrqmapGet_Post_MapRegisters_Nop },
  { 0, 0, 0, 0, 1, BspSparcLeon3ReqIrqmapGet_Post_Status_InvNum,
    BspSparcLeon3ReqIrqmapGet_Post_ControllerLine_Invalid,
    BspSparcLeon3ReqIrqmapGet_Post_Map_NA,
    BspSparcLeon3ReqIrqmapGet_Post_MapRegisters_NA }
};

static const uint8_t
BspSparcLeon3ReqIrqmapGet_Map[] = {
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 5, 5, 5, 5, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

/* clang-format on */

static size_t BspSparcLeon3ReqIrqmapGet_Scope( void *arg, char *buf, size_t n )
{
  BspSparcLeon3ReqIrqmapGet_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      BspSparcLeon3ReqIrqmapGet_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture BspSparcLeon3ReqIrqmapGet_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = BspSparcLeon3ReqIrqmapGet_Scope,
  .initial_context = &BspSparcLeon3ReqIrqmapGet_Instance
};

static const uint8_t BspSparcLeon3ReqIrqmapGet_Weights[] = { 32, 16, 4, 1 };

static void BspSparcLeon3ReqIrqmapGet_Skip(
  BspSparcLeon3ReqIrqmapGet_Context *ctx,
  size_t                             index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pci[ 1 ] = BspSparcLeon3ReqIrqmapGet_Pre_ControllerLine_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pci[ 2 ] = BspSparcLeon3ReqIrqmapGet_Pre_BusLine_NA - 1;
      /* Fall through */
    case 3:
      ctx->Map.pci[ 3 ] = BspSparcLeon3ReqIrqmapGet_Pre_MapEntry_NA - 1;
      break;
  }
}

static inline BspSparcLeon3ReqIrqmapGet_Entry
BspSparcLeon3ReqIrqmapGet_PopEntry( BspSparcLeon3ReqIrqmapGet_Context *ctx )
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 4; ++i ) {
      index += BspSparcLeon3ReqIrqmapGet_Weights[ i ] * ctx->Map.pci[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return BspSparcLeon3ReqIrqmapGet_Entries
    [ BspSparcLeon3ReqIrqmapGet_Map[ index ] ];
}

static void BspSparcLeon3ReqIrqmapGet_SetPreConditionStates(
  BspSparcLeon3ReqIrqmapGet_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];

  if ( ctx->Map.entry.Pre_MapEntry_NA ) {
    ctx->Map.pcs[ 3 ] = BspSparcLeon3ReqIrqmapGet_Pre_MapEntry_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }
}

static void BspSparcLeon3ReqIrqmapGet_TestVariant(
  BspSparcLeon3ReqIrqmapGet_Context *ctx
)
{
  BspSparcLeon3ReqIrqmapGet_Pre_InterruptMap_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip ) {
    BspSparcLeon3ReqIrqmapGet_Skip( ctx, 0 );
    return;
  }

  BspSparcLeon3ReqIrqmapGet_Pre_ControllerLine_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  BspSparcLeon3ReqIrqmapGet_Pre_BusLine_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  BspSparcLeon3ReqIrqmapGet_Pre_MapEntry_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  BspSparcLeon3ReqIrqmapGet_Action( ctx );
  BspSparcLeon3ReqIrqmapGet_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  BspSparcLeon3ReqIrqmapGet_Post_ControllerLine_Check(
    ctx,
    ctx->Map.entry.Post_ControllerLine
  );
  BspSparcLeon3ReqIrqmapGet_Post_Map_Check( ctx, ctx->Map.entry.Post_Map );
  BspSparcLeon3ReqIrqmapGet_Post_MapRegisters_Check(
    ctx,
    ctx->Map.entry.Post_MapRegisters
  );
}

/**
 * @fn void T_case_body_BspSparcLeon3ReqIrqmapGet( void )
 */
T_TEST_CASE_FIXTURE(
  BspSparcLeon3ReqIrqmapGet,
  &BspSparcLeon3ReqIrqmapGet_Fixture
)
{
  BspSparcLeon3ReqIrqmapGet_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pci[ 0 ] = BspSparcLeon3ReqIrqmapGet_Pre_InterruptMap_Yes;
    ctx->Map.pci[ 0 ] < BspSparcLeon3ReqIrqmapGet_Pre_InterruptMap_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = BspSparcLeon3ReqIrqmapGet_Pre_ControllerLine_Valid;
      ctx->Map.pci[ 1 ] < BspSparcLeon3ReqIrqmapGet_Pre_ControllerLine_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = BspSparcLeon3ReqIrqmapGet_Pre_BusLine_First;
        ctx->Map.pci[ 2 ] < BspSparcLeon3ReqIrqmapGet_Pre_BusLine_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = BspSparcLeon3ReqIrqmapGet_Pre_MapEntry_First;
          ctx->Map.pci[ 3 ] < BspSparcLeon3ReqIrqmapGet_Pre_MapEntry_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = BspSparcLeon3ReqIrqmapGet_PopEntry( ctx );
          BspSparcLeon3ReqIrqmapGet_SetPreConditionStates( ctx );
          BspSparcLeon3ReqIrqmapGet_TestVariant( ctx );
          BspSparcLeon3ReqIrqmapGet_Cleanup( ctx );
        }
      }
    }
  }
}

/** @} */
