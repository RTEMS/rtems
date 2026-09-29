/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup BspSparcLeon3ReqExtIrqInit
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
#include <bsp/irqimpl.h>
#include <bsp/leon3.h>

#include <rtems/test.h>

/**
 * @defgroup BspSparcLeon3ReqExtIrqInit spec:/bsp/sparc/leon3/req/ext-irq-init
 *
 * @ingroup TestsuitesBspsValidationBsp0
 *
 * @{
 */

typedef enum {
  BspSparcLeon3ReqExtIrqInit_Pre_InterruptMap_Yes,
  BspSparcLeon3ReqExtIrqInit_Pre_InterruptMap_No,
  BspSparcLeon3ReqExtIrqInit_Pre_InterruptMap_NA
} BspSparcLeon3ReqExtIrqInit_Pre_InterruptMap;

typedef enum {
  BspSparcLeon3ReqExtIrqInit_Pre_ExtendedInterrupt_Fixed,
  BspSparcLeon3ReqExtIrqInit_Pre_ExtendedInterrupt_Detected,
  BspSparcLeon3ReqExtIrqInit_Pre_ExtendedInterrupt_NA
} BspSparcLeon3ReqExtIrqInit_Pre_ExtendedInterrupt;

typedef enum {
  BspSparcLeon3ReqExtIrqInit_Pre_Ipi_Yes,
  BspSparcLeon3ReqExtIrqInit_Pre_Ipi_No,
  BspSparcLeon3ReqExtIrqInit_Pre_Ipi_NA
} BspSparcLeon3ReqExtIrqInit_Pre_Ipi;

typedef enum {
  BspSparcLeon3ReqExtIrqInit_Pre_MapFields_None,
  BspSparcLeon3ReqExtIrqInit_Pre_MapFields_Some,
  BspSparcLeon3ReqExtIrqInit_Pre_MapFields_All,
  BspSparcLeon3ReqExtIrqInit_Pre_MapFields_NA
} BspSparcLeon3ReqExtIrqInit_Pre_MapFields;

typedef enum {
  BspSparcLeon3ReqExtIrqInit_Pre_ProcessorMask_None,
  BspSparcLeon3ReqExtIrqInit_Pre_ProcessorMask_Some,
  BspSparcLeon3ReqExtIrqInit_Pre_ProcessorMask_All,
  BspSparcLeon3ReqExtIrqInit_Pre_ProcessorMask_NA
} BspSparcLeon3ReqExtIrqInit_Pre_ProcessorMask;

typedef enum {
  BspSparcLeon3ReqExtIrqInit_Pre_ProcessorForce_None,
  BspSparcLeon3ReqExtIrqInit_Pre_ProcessorForce_Some,
  BspSparcLeon3ReqExtIrqInit_Pre_ProcessorForce_All,
  BspSparcLeon3ReqExtIrqInit_Pre_ProcessorForce_NA
} BspSparcLeon3ReqExtIrqInit_Pre_ProcessorForce;

typedef enum {
  BspSparcLeon3ReqExtIrqInit_Post_ProcessorMask_Zero,
  BspSparcLeon3ReqExtIrqInit_Post_ProcessorMask_NA
} BspSparcLeon3ReqExtIrqInit_Post_ProcessorMask;

typedef enum {
  BspSparcLeon3ReqExtIrqInit_Post_ProcessorForce_Zero,
  BspSparcLeon3ReqExtIrqInit_Post_ProcessorForce_NA
} BspSparcLeon3ReqExtIrqInit_Post_ProcessorForce;

typedef enum {
  BspSparcLeon3ReqExtIrqInit_Post_InterruptClear_All,
  BspSparcLeon3ReqExtIrqInit_Post_InterruptClear_NA
} BspSparcLeon3ReqExtIrqInit_Post_InterruptClear;

typedef enum {
  BspSparcLeon3ReqExtIrqInit_Post_OtherRegisters_Nop,
  BspSparcLeon3ReqExtIrqInit_Post_OtherRegisters_NA
} BspSparcLeon3ReqExtIrqInit_Post_OtherRegisters;

typedef enum {
  BspSparcLeon3ReqExtIrqInit_Post_InRangeEntries_Field,
  BspSparcLeon3ReqExtIrqInit_Post_InRangeEntries_NA
} BspSparcLeon3ReqExtIrqInit_Post_InRangeEntries;

typedef enum {
  BspSparcLeon3ReqExtIrqInit_Post_OutOfRangeEntries_Masked,
  BspSparcLeon3ReqExtIrqInit_Post_OutOfRangeEntries_NA
} BspSparcLeon3ReqExtIrqInit_Post_OutOfRangeEntries;

typedef enum {
  BspSparcLeon3ReqExtIrqInit_Post_IpiEntry_IpiLine,
  BspSparcLeon3ReqExtIrqInit_Post_IpiEntry_NA
} BspSparcLeon3ReqExtIrqInit_Post_IpiEntry;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_InterruptMap_NA : 1;
  uint16_t Pre_ExtendedInterrupt_NA : 1;
  uint16_t Pre_Ipi_NA : 1;
  uint16_t Pre_MapFields_NA : 1;
  uint16_t Pre_ProcessorMask_NA : 1;
  uint16_t Pre_ProcessorForce_NA : 1;
  uint16_t Post_ProcessorMask : 1;
  uint16_t Post_ProcessorForce : 1;
  uint16_t Post_InterruptClear : 1;
  uint16_t Post_OtherRegisters : 1;
  uint16_t Post_InRangeEntries : 1;
  uint16_t Post_OutOfRangeEntries : 1;
  uint16_t Post_IpiEntry : 1;
} BspSparcLeon3ReqExtIrqInit_Entry;

#if LEON3_IRQMAP_BUS_LINE_COUNT != 0
#define BUS_LINE_COUNT LEON3_IRQMAP_BUS_LINE_COUNT
#else
#define BUS_LINE_COUNT 32
#endif

/* GR740 User's Manual, section 21.3.19 */
#define CONTROLLER_LINE_COUNT 32

typedef enum { MAP_FIELDS_NONE, MAP_FIELDS_SOME, MAP_FIELDS_ALL } MapFields;

/**
 * @brief Test context for spec:/bsp/sparc/leon3/req/ext-irq-init test case.
 */
typedef struct {
  /**
   * @brief This member provides the register block in memory which the test
   *   passes to the function.
   */
  irqamp regs;

  /**
   * @brief This member contains the register block before the call.
   */
  irqamp regs_before;

  /**
   * @brief This member specifies which map fields select a controller line.
   */
  MapFields map_fields;

  /**
   * @brief This member specifies the processor interrupt mask of the boot
   *   processor before the call.
   */
  uint32_t mask_before;

  /**
   * @brief This member specifies the processor interrupt force register of the
   *   boot processor before the call.
   */
  uint32_t force_before;

  /**
   * @brief This member contains the interrupt map copy before the call.
   */
  uint8_t mapping_saved[ BUS_LINE_COUNT ];

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 6 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 6 ];

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
    BspSparcLeon3ReqExtIrqInit_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} BspSparcLeon3ReqExtIrqInit_Context;

static BspSparcLeon3ReqExtIrqInit_Context BspSparcLeon3ReqExtIrqInit_Instance;

static const char *const BspSparcLeon3ReqExtIrqInit_PreDesc_InterruptMap[] =
  { "Yes", "No", "NA" };

static const char *const
  BspSparcLeon3ReqExtIrqInit_PreDesc_ExtendedInterrupt[] =
    { "Fixed", "Detected", "NA" };

static const char *const BspSparcLeon3ReqExtIrqInit_PreDesc_Ipi[] =
  { "Yes", "No", "NA" };

static const char *const BspSparcLeon3ReqExtIrqInit_PreDesc_MapFields[] =
  { "None", "Some", "All", "NA" };

static const char *const BspSparcLeon3ReqExtIrqInit_PreDesc_ProcessorMask[] =
  { "None", "Some", "All", "NA" };

static const char *const BspSparcLeon3ReqExtIrqInit_PreDesc_ProcessorForce[] =
  { "None", "Some", "All", "NA" };

static const char *const *const BspSparcLeon3ReqExtIrqInit_PreDesc[] = {
  BspSparcLeon3ReqExtIrqInit_PreDesc_InterruptMap,
  BspSparcLeon3ReqExtIrqInit_PreDesc_ExtendedInterrupt,
  BspSparcLeon3ReqExtIrqInit_PreDesc_Ipi,
  BspSparcLeon3ReqExtIrqInit_PreDesc_MapFields,
  BspSparcLeon3ReqExtIrqInit_PreDesc_ProcessorMask,
  BspSparcLeon3ReqExtIrqInit_PreDesc_ProcessorForce,
  NULL
};

typedef BspSparcLeon3ReqExtIrqInit_Context Context;

#if defined( RTEMS_SMP ) || defined( RTEMS_MULTIPROCESSING )
#define FIRST_CHECKED_BUS_LINE 1
#else
#define FIRST_CHECKED_BUS_LINE 0
#endif

static uint32_t GetNoLineField( uint32_t bus_line )
{
  if ( bus_line == 0 ) {
    return CONTROLLER_LINE_COUNT;
  }

  if ( bus_line == BUS_LINE_COUNT - 1 ) {
    return 255;
  }

  return CONTROLLER_LINE_COUNT + ( 7 * bus_line ) % 224;
}

static uint32_t GetField( const Context *ctx, uint32_t bus_line )
{
  if ( ctx->map_fields == MAP_FIELDS_NONE ) {
    return GetNoLineField( bus_line );
  }

  if ( ctx->map_fields == MAP_FIELDS_SOME && ( bus_line % 2 ) != 0 ) {
    return GetNoLineField( bus_line );
  }

  return ( CONTROLLER_LINE_COUNT - 1 - bus_line ) % CONTROLLER_LINE_COUNT;
}

static void PrepareRegisters( Context *ctx )
{
  uint32_t *words;
  size_t    i;

  words = (uint32_t *) &ctx->regs;

  for ( i = 0; i < sizeof( ctx->regs ) / sizeof( *words ); ++i ) {
    words[ i ] = 0x5a5a0000 + (uint32_t) i;
  }

  for ( i = 0; i < BUS_LINE_COUNT / 4; ++i ) {
    ctx->regs.irqmap[ i ] = ( GetField( ctx, 4 * i ) << 24 ) |
                            ( GetField( ctx, 4 * i + 1 ) << 16 ) |
                            ( GetField( ctx, 4 * i + 2 ) << 8 ) |
                            GetField( ctx, 4 * i + 3 );
  }

  ctx->regs.pimask[ LEON3_Cpu_Index ] = ctx->mask_before;
  ctx->regs.piforce[ LEON3_Cpu_Index ] = ctx->force_before;

#if LEON3_IRQMAP_BUS_LINE_COUNT != 0
  for ( i = 0; i < BUS_LINE_COUNT; ++i ) {
    LEON3_IrqCtrl_Mapping[ i ] = 0x55;
  }
#endif
}

static void CheckOtherRegisters( const Context *ctx )
{
  irqamp after;

  after = ctx->regs;
  after.pimask[ LEON3_Cpu_Index ] = ctx->regs_before.pimask[ LEON3_Cpu_Index ];
  after.piforce[ LEON3_Cpu_Index ] = ctx->regs_before
                                       .piforce[ LEON3_Cpu_Index ];
  after.iclear = ctx->regs_before.iclear;
  T_eq_mem( &after, &ctx->regs_before, sizeof( after ) );
}

static void SaveMapCopy( Context *ctx )
{
#if LEON3_IRQMAP_BUS_LINE_COUNT != 0
  memcpy(
    ctx->mapping_saved,
    LEON3_IrqCtrl_Mapping,
    sizeof( ctx->mapping_saved )
  );
#else
  (void) ctx;
#endif
}

static void RestoreMapCopy( const Context *ctx )
{
#if LEON3_IRQMAP_BUS_LINE_COUNT != 0
  memcpy(
    LEON3_IrqCtrl_Mapping,
    ctx->mapping_saved,
    sizeof( ctx->mapping_saved )
  );
#else
  (void) ctx;
#endif
}

static void CheckInRangeEntries( const Context *ctx )
{
#if LEON3_IRQMAP_BUS_LINE_COUNT != 0
  uint32_t i;

  for ( i = FIRST_CHECKED_BUS_LINE; i < BUS_LINE_COUNT; ++i ) {
    uint32_t field;

    field = GetField( ctx, i );

    if ( field < CONTROLLER_LINE_COUNT ) {
      T_eq_u32( LEON3_IrqCtrl_Mapping[ i ], field );
    }
  }
#else
  (void) ctx;
#endif
}

static void CheckOutOfRangeEntries( const Context *ctx )
{
#if LEON3_IRQMAP_BUS_LINE_COUNT != 0
  uint32_t i;

  for ( i = FIRST_CHECKED_BUS_LINE; i < BUS_LINE_COUNT; ++i ) {
    uint32_t field;

    field = GetField( ctx, i );

    if ( field >= CONTROLLER_LINE_COUNT ) {
      T_eq_u32(
        LEON3_IrqCtrl_Mapping[ i ],
        field & ( CONTROLLER_LINE_COUNT - 1 )
      );
    }
  }
#else
  (void) ctx;
#endif
}

static void BspSparcLeon3ReqExtIrqInit_Pre_InterruptMap_Prepare(
  BspSparcLeon3ReqExtIrqInit_Context         *ctx,
  BspSparcLeon3ReqExtIrqInit_Pre_InterruptMap state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqExtIrqInit_Pre_InterruptMap_Yes: {
      /*
       * Where the BSP uses the interrupt map.
       */
      #if LEON3_IRQMAP_BUS_LINE_COUNT == 0
      ctx->Map.skip = true;
      #endif
      break;
    }

    case BspSparcLeon3ReqExtIrqInit_Pre_InterruptMap_No: {
      /*
       * Where the BSP does not use the interrupt map.
       */
      #if LEON3_IRQMAP_BUS_LINE_COUNT != 0
      ctx->Map.skip = true;
      #endif
      break;
    }

    case BspSparcLeon3ReqExtIrqInit_Pre_InterruptMap_NA:
      break;
  }
}

static void BspSparcLeon3ReqExtIrqInit_Pre_ExtendedInterrupt_Prepare(
  BspSparcLeon3ReqExtIrqInit_Context              *ctx,
  BspSparcLeon3ReqExtIrqInit_Pre_ExtendedInterrupt state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqExtIrqInit_Pre_ExtendedInterrupt_Fixed: {
      /*
       * Where the BSP configuration fixes the extended controller line.
       */
      #if !defined( LEON3_IRQAMP_EXTENDED_INTERRUPT )
      ctx->Map.skip = true;
      #endif
      break;
    }

    case BspSparcLeon3ReqExtIrqInit_Pre_ExtendedInterrupt_Detected: {
      /*
       * Where the BSP determines the extended controller line at run time.
       */
      #if defined( LEON3_IRQAMP_EXTENDED_INTERRUPT )
      ctx->Map.skip = true;
      #endif
      break;
    }

    case BspSparcLeon3ReqExtIrqInit_Pre_ExtendedInterrupt_NA:
      break;
  }
}

static void BspSparcLeon3ReqExtIrqInit_Pre_Ipi_Prepare(
  BspSparcLeon3ReqExtIrqInit_Context *ctx,
  BspSparcLeon3ReqExtIrqInit_Pre_Ipi  state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqExtIrqInit_Pre_Ipi_Yes: {
      /*
       * Where the system is configured with SMP or multiprocessing support.
       */
      #if !defined( RTEMS_SMP ) && !defined( RTEMS_MULTIPROCESSING )
      ctx->Map.skip = true;
      #endif
      break;
    }

    case BspSparcLeon3ReqExtIrqInit_Pre_Ipi_No: {
      /*
       * Where the system is configured without SMP and multiprocessing
       * support.
       */
      #if defined( RTEMS_SMP ) || defined( RTEMS_MULTIPROCESSING )
      ctx->Map.skip = true;
      #endif
      break;
    }

    case BspSparcLeon3ReqExtIrqInit_Pre_Ipi_NA:
      break;
  }
}

static void BspSparcLeon3ReqExtIrqInit_Pre_MapFields_Prepare(
  BspSparcLeon3ReqExtIrqInit_Context      *ctx,
  BspSparcLeon3ReqExtIrqInit_Pre_MapFields state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqExtIrqInit_Pre_MapFields_None: {
      /*
       * While the interrupt map entry of no bus line selects a controller
       * line.
       */
      ctx->map_fields = MAP_FIELDS_NONE;
      break;
    }

    case BspSparcLeon3ReqExtIrqInit_Pre_MapFields_Some: {
      /*
       * While the interrupt map entry of some but not all bus lines selects a
       * controller line.
       */
      ctx->map_fields = MAP_FIELDS_SOME;
      break;
    }

    case BspSparcLeon3ReqExtIrqInit_Pre_MapFields_All: {
      /*
       * While the interrupt map entry of each bus line selects a controller
       * line.
       */
      ctx->map_fields = MAP_FIELDS_ALL;
      break;
    }

    case BspSparcLeon3ReqExtIrqInit_Pre_MapFields_NA:
      break;
  }
}

static void BspSparcLeon3ReqExtIrqInit_Pre_ProcessorMask_Prepare(
  BspSparcLeon3ReqExtIrqInit_Context          *ctx,
  BspSparcLeon3ReqExtIrqInit_Pre_ProcessorMask state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqExtIrqInit_Pre_ProcessorMask_None: {
      /*
       * While the processor interrupt mask of the boot processor enables no
       * controller line.
       */
      ctx->mask_before = 0;
      break;
    }

    case BspSparcLeon3ReqExtIrqInit_Pre_ProcessorMask_Some: {
      /*
       * While the processor interrupt mask of the boot processor enables some
       * but not all controller lines.
       */
      ctx->mask_before = 0x5554;
      break;
    }

    case BspSparcLeon3ReqExtIrqInit_Pre_ProcessorMask_All: {
      /*
       * While the processor interrupt mask of the boot processor enables all
       * controller lines.
       */
      ctx->mask_before = 0xfffe;
      break;
    }

    case BspSparcLeon3ReqExtIrqInit_Pre_ProcessorMask_NA:
      break;
  }
}

static void BspSparcLeon3ReqExtIrqInit_Pre_ProcessorForce_Prepare(
  BspSparcLeon3ReqExtIrqInit_Context           *ctx,
  BspSparcLeon3ReqExtIrqInit_Pre_ProcessorForce state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqExtIrqInit_Pre_ProcessorForce_None: {
      /*
       * While the processor interrupt force register of the boot processor
       * forces no controller line.
       */
      ctx->force_before = 0;
      break;
    }

    case BspSparcLeon3ReqExtIrqInit_Pre_ProcessorForce_Some: {
      /*
       * While the processor interrupt force register of the boot processor
       * forces some but not all controller lines.
       */
      ctx->force_before = 0xaaa8;
      break;
    }

    case BspSparcLeon3ReqExtIrqInit_Pre_ProcessorForce_All: {
      /*
       * While the processor interrupt force register of the boot processor
       * forces all controller lines.
       */
      ctx->force_before = 0xfffe;
      break;
    }

    case BspSparcLeon3ReqExtIrqInit_Pre_ProcessorForce_NA:
      break;
  }
}

static void BspSparcLeon3ReqExtIrqInit_Post_ProcessorMask_Check(
  BspSparcLeon3ReqExtIrqInit_Context           *ctx,
  BspSparcLeon3ReqExtIrqInit_Post_ProcessorMask state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqExtIrqInit_Post_ProcessorMask_Zero: {
      /*
       * The processor interrupt mask of the boot processor shall be zero.
       */
      T_eq_u32( ctx->regs.pimask[ LEON3_Cpu_Index ], 0 );
      break;
    }

    case BspSparcLeon3ReqExtIrqInit_Post_ProcessorMask_NA:
      break;
  }
}

static void BspSparcLeon3ReqExtIrqInit_Post_ProcessorForce_Check(
  BspSparcLeon3ReqExtIrqInit_Context            *ctx,
  BspSparcLeon3ReqExtIrqInit_Post_ProcessorForce state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqExtIrqInit_Post_ProcessorForce_Zero: {
      /*
       * The processor interrupt force register of the boot processor shall be
       * zero.
       */
      T_eq_u32( ctx->regs.piforce[ LEON3_Cpu_Index ], 0 );
      break;
    }

    case BspSparcLeon3ReqExtIrqInit_Post_ProcessorForce_NA:
      break;
  }
}

static void BspSparcLeon3ReqExtIrqInit_Post_InterruptClear_Check(
  BspSparcLeon3ReqExtIrqInit_Context            *ctx,
  BspSparcLeon3ReqExtIrqInit_Post_InterruptClear state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqExtIrqInit_Post_InterruptClear_All: {
      /*
       * The function shall write all ones to the interrupt clear register.
       */
      T_eq_u32( ctx->regs.iclear, 0xffffffff );
      break;
    }

    case BspSparcLeon3ReqExtIrqInit_Post_InterruptClear_NA:
      break;
  }
}

static void BspSparcLeon3ReqExtIrqInit_Post_OtherRegisters_Check(
  BspSparcLeon3ReqExtIrqInit_Context            *ctx,
  BspSparcLeon3ReqExtIrqInit_Post_OtherRegisters state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqExtIrqInit_Post_OtherRegisters_Nop: {
      /*
       * The registers of the IRQ(A)MP other than the processor interrupt mask
       * and the processor interrupt force register of the boot processor and
       * the interrupt clear register shall not be modified by the function
       * call.
       */
      CheckOtherRegisters( ctx );
      break;
    }

    case BspSparcLeon3ReqExtIrqInit_Post_OtherRegisters_NA:
      break;
  }
}

static void BspSparcLeon3ReqExtIrqInit_Post_InRangeEntries_Check(
  BspSparcLeon3ReqExtIrqInit_Context            *ctx,
  BspSparcLeon3ReqExtIrqInit_Post_InRangeEntries state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqExtIrqInit_Post_InRangeEntries_Field: {
      /*
       * The interrupt map entry of the interrupt map copy of each bus line
       * whose interrupt map entry selects a controller line shall be set to
       * that interrupt map entry, except for the bus line of the
       * inter-processor interrupt.
       */
      CheckInRangeEntries( ctx );
      break;
    }

    case BspSparcLeon3ReqExtIrqInit_Post_InRangeEntries_NA:
      break;
  }
}

static void BspSparcLeon3ReqExtIrqInit_Post_OutOfRangeEntries_Check(
  BspSparcLeon3ReqExtIrqInit_Context               *ctx,
  BspSparcLeon3ReqExtIrqInit_Post_OutOfRangeEntries state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqExtIrqInit_Post_OutOfRangeEntries_Masked: {
      /*
       * The interrupt map entry of the interrupt map copy of each bus line
       * whose interrupt map entry selects no controller line shall be set to
       * that interrupt map entry masked by the count of controller lines minus
       * one, except for the bus line of the inter-processor interrupt.
       */
      CheckOutOfRangeEntries( ctx );
      break;
    }

    case BspSparcLeon3ReqExtIrqInit_Post_OutOfRangeEntries_NA:
      break;
  }
}

static void BspSparcLeon3ReqExtIrqInit_Post_IpiEntry_Check(
  BspSparcLeon3ReqExtIrqInit_Post_IpiEntry state
)
{
  switch ( state ) {
    case BspSparcLeon3ReqExtIrqInit_Post_IpiEntry_IpiLine: {
      /*
       * The interrupt map entry of the interrupt map copy of the first bus
       * line shall be set to the controller line defined by the BSP option
       * LEON3_IPI_CONTROLLER_LINE.
       */
      #if LEON3_IRQMAP_BUS_LINE_COUNT != 0
      T_eq_u32( LEON3_IrqCtrl_Mapping[ 0 ], LEON3_IPI_CONTROLLER_LINE );
      #endif
      break;
    }

    case BspSparcLeon3ReqExtIrqInit_Post_IpiEntry_NA:
      break;
  }
}

static void BspSparcLeon3ReqExtIrqInit_Action(
  BspSparcLeon3ReqExtIrqInit_Context *ctx
)
{
  SaveMapCopy( ctx );
  PrepareRegisters( ctx );
  ctx->regs_before = ctx->regs;
  leon3_ext_irq_init( &ctx->regs );
}

static void BspSparcLeon3ReqExtIrqInit_Cleanup(
  BspSparcLeon3ReqExtIrqInit_Context *ctx
)
{
  RestoreMapCopy( ctx );
}

/* clang-format off */

static const BspSparcLeon3ReqExtIrqInit_Entry
BspSparcLeon3ReqExtIrqInit_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, BspSparcLeon3ReqExtIrqInit_Post_ProcessorMask_NA,
    BspSparcLeon3ReqExtIrqInit_Post_ProcessorForce_NA,
    BspSparcLeon3ReqExtIrqInit_Post_InterruptClear_NA,
    BspSparcLeon3ReqExtIrqInit_Post_OtherRegisters_NA,
    BspSparcLeon3ReqExtIrqInit_Post_InRangeEntries_NA,
    BspSparcLeon3ReqExtIrqInit_Post_OutOfRangeEntries_NA,
    BspSparcLeon3ReqExtIrqInit_Post_IpiEntry_NA },
  { 0, 0, 0, 0, 1, 0, 0, BspSparcLeon3ReqExtIrqInit_Post_ProcessorMask_Zero,
    BspSparcLeon3ReqExtIrqInit_Post_ProcessorForce_Zero,
    BspSparcLeon3ReqExtIrqInit_Post_InterruptClear_All,
    BspSparcLeon3ReqExtIrqInit_Post_OtherRegisters_Nop,
    BspSparcLeon3ReqExtIrqInit_Post_InRangeEntries_NA,
    BspSparcLeon3ReqExtIrqInit_Post_OutOfRangeEntries_NA,
    BspSparcLeon3ReqExtIrqInit_Post_IpiEntry_NA },
  { 0, 0, 0, 0, 0, 0, 0, BspSparcLeon3ReqExtIrqInit_Post_ProcessorMask_Zero,
    BspSparcLeon3ReqExtIrqInit_Post_ProcessorForce_Zero,
    BspSparcLeon3ReqExtIrqInit_Post_InterruptClear_All,
    BspSparcLeon3ReqExtIrqInit_Post_OtherRegisters_Nop,
    BspSparcLeon3ReqExtIrqInit_Post_InRangeEntries_NA,
    BspSparcLeon3ReqExtIrqInit_Post_OutOfRangeEntries_Masked,
    BspSparcLeon3ReqExtIrqInit_Post_IpiEntry_IpiLine },
  { 0, 0, 0, 0, 0, 0, 0, BspSparcLeon3ReqExtIrqInit_Post_ProcessorMask_Zero,
    BspSparcLeon3ReqExtIrqInit_Post_ProcessorForce_Zero,
    BspSparcLeon3ReqExtIrqInit_Post_InterruptClear_All,
    BspSparcLeon3ReqExtIrqInit_Post_OtherRegisters_Nop,
    BspSparcLeon3ReqExtIrqInit_Post_InRangeEntries_Field,
    BspSparcLeon3ReqExtIrqInit_Post_OutOfRangeEntries_Masked,
    BspSparcLeon3ReqExtIrqInit_Post_IpiEntry_IpiLine },
  { 0, 0, 0, 0, 0, 0, 0, BspSparcLeon3ReqExtIrqInit_Post_ProcessorMask_Zero,
    BspSparcLeon3ReqExtIrqInit_Post_ProcessorForce_Zero,
    BspSparcLeon3ReqExtIrqInit_Post_InterruptClear_All,
    BspSparcLeon3ReqExtIrqInit_Post_OtherRegisters_Nop,
    BspSparcLeon3ReqExtIrqInit_Post_InRangeEntries_Field,
    BspSparcLeon3ReqExtIrqInit_Post_OutOfRangeEntries_NA,
    BspSparcLeon3ReqExtIrqInit_Post_IpiEntry_IpiLine },
  { 0, 0, 0, 0, 0, 0, 0, BspSparcLeon3ReqExtIrqInit_Post_ProcessorMask_Zero,
    BspSparcLeon3ReqExtIrqInit_Post_ProcessorForce_Zero,
    BspSparcLeon3ReqExtIrqInit_Post_InterruptClear_All,
    BspSparcLeon3ReqExtIrqInit_Post_OtherRegisters_Nop,
    BspSparcLeon3ReqExtIrqInit_Post_InRangeEntries_NA,
    BspSparcLeon3ReqExtIrqInit_Post_OutOfRangeEntries_Masked,
    BspSparcLeon3ReqExtIrqInit_Post_IpiEntry_NA },
  { 0, 0, 0, 0, 0, 0, 0, BspSparcLeon3ReqExtIrqInit_Post_ProcessorMask_Zero,
    BspSparcLeon3ReqExtIrqInit_Post_ProcessorForce_Zero,
    BspSparcLeon3ReqExtIrqInit_Post_InterruptClear_All,
    BspSparcLeon3ReqExtIrqInit_Post_OtherRegisters_Nop,
    BspSparcLeon3ReqExtIrqInit_Post_InRangeEntries_Field,
    BspSparcLeon3ReqExtIrqInit_Post_OutOfRangeEntries_Masked,
    BspSparcLeon3ReqExtIrqInit_Post_IpiEntry_NA },
  { 0, 0, 0, 0, 0, 0, 0, BspSparcLeon3ReqExtIrqInit_Post_ProcessorMask_Zero,
    BspSparcLeon3ReqExtIrqInit_Post_ProcessorForce_Zero,
    BspSparcLeon3ReqExtIrqInit_Post_InterruptClear_All,
    BspSparcLeon3ReqExtIrqInit_Post_OtherRegisters_Nop,
    BspSparcLeon3ReqExtIrqInit_Post_InRangeEntries_Field,
    BspSparcLeon3ReqExtIrqInit_Post_OutOfRangeEntries_NA,
    BspSparcLeon3ReqExtIrqInit_Post_IpiEntry_NA }
};

static const uint8_t
BspSparcLeon3ReqExtIrqInit_Map[] = {
  2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t BspSparcLeon3ReqExtIrqInit_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  BspSparcLeon3ReqExtIrqInit_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      BspSparcLeon3ReqExtIrqInit_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture BspSparcLeon3ReqExtIrqInit_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = BspSparcLeon3ReqExtIrqInit_Scope,
  .initial_context = &BspSparcLeon3ReqExtIrqInit_Instance
};

static const uint8_t BspSparcLeon3ReqExtIrqInit_Weights[] =
  { 108, 54, 27, 9, 3, 1 };

static void BspSparcLeon3ReqExtIrqInit_Skip(
  BspSparcLeon3ReqExtIrqInit_Context *ctx,
  size_t                              index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pci[ 1 ] = BspSparcLeon3ReqExtIrqInit_Pre_ExtendedInterrupt_NA -
                          1;
      /* Fall through */
    case 2:
      ctx->Map.pci[ 2 ] = BspSparcLeon3ReqExtIrqInit_Pre_Ipi_NA - 1;
      /* Fall through */
    case 3:
      ctx->Map.pci[ 3 ] = BspSparcLeon3ReqExtIrqInit_Pre_MapFields_NA - 1;
      /* Fall through */
    case 4:
      ctx->Map.pci[ 4 ] = BspSparcLeon3ReqExtIrqInit_Pre_ProcessorMask_NA - 1;
      /* Fall through */
    case 5:
      ctx->Map.pci[ 5 ] = BspSparcLeon3ReqExtIrqInit_Pre_ProcessorForce_NA - 1;
      break;
  }
}

static inline BspSparcLeon3ReqExtIrqInit_Entry
BspSparcLeon3ReqExtIrqInit_PopEntry( BspSparcLeon3ReqExtIrqInit_Context *ctx )
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 6; ++i ) {
      index += BspSparcLeon3ReqExtIrqInit_Weights[ i ] * ctx->Map.pci[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return BspSparcLeon3ReqExtIrqInit_Entries
    [ BspSparcLeon3ReqExtIrqInit_Map[ index ] ];
}

static void BspSparcLeon3ReqExtIrqInit_SetPreConditionStates(
  BspSparcLeon3ReqExtIrqInit_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];

  if ( ctx->Map.entry.Pre_MapFields_NA ) {
    ctx->Map.pcs[ 3 ] = BspSparcLeon3ReqExtIrqInit_Pre_MapFields_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
}

static void BspSparcLeon3ReqExtIrqInit_TestVariant(
  BspSparcLeon3ReqExtIrqInit_Context *ctx
)
{
  BspSparcLeon3ReqExtIrqInit_Pre_InterruptMap_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );

  if ( ctx->Map.skip ) {
    BspSparcLeon3ReqExtIrqInit_Skip( ctx, 0 );
    return;
  }

  BspSparcLeon3ReqExtIrqInit_Pre_ExtendedInterrupt_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );

  if ( ctx->Map.skip ) {
    BspSparcLeon3ReqExtIrqInit_Skip( ctx, 1 );
    return;
  }

  BspSparcLeon3ReqExtIrqInit_Pre_Ipi_Prepare( ctx, ctx->Map.pcs[ 2 ] );

  if ( ctx->Map.skip ) {
    BspSparcLeon3ReqExtIrqInit_Skip( ctx, 2 );
    return;
  }

  BspSparcLeon3ReqExtIrqInit_Pre_MapFields_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  BspSparcLeon3ReqExtIrqInit_Pre_ProcessorMask_Prepare(
    ctx,
    ctx->Map.pcs[ 4 ]
  );
  BspSparcLeon3ReqExtIrqInit_Pre_ProcessorForce_Prepare(
    ctx,
    ctx->Map.pcs[ 5 ]
  );
  BspSparcLeon3ReqExtIrqInit_Action( ctx );
  BspSparcLeon3ReqExtIrqInit_Post_ProcessorMask_Check(
    ctx,
    ctx->Map.entry.Post_ProcessorMask
  );
  BspSparcLeon3ReqExtIrqInit_Post_ProcessorForce_Check(
    ctx,
    ctx->Map.entry.Post_ProcessorForce
  );
  BspSparcLeon3ReqExtIrqInit_Post_InterruptClear_Check(
    ctx,
    ctx->Map.entry.Post_InterruptClear
  );
  BspSparcLeon3ReqExtIrqInit_Post_OtherRegisters_Check(
    ctx,
    ctx->Map.entry.Post_OtherRegisters
  );
  BspSparcLeon3ReqExtIrqInit_Post_InRangeEntries_Check(
    ctx,
    ctx->Map.entry.Post_InRangeEntries
  );
  BspSparcLeon3ReqExtIrqInit_Post_OutOfRangeEntries_Check(
    ctx,
    ctx->Map.entry.Post_OutOfRangeEntries
  );
  BspSparcLeon3ReqExtIrqInit_Post_IpiEntry_Check(
    ctx->Map.entry.Post_IpiEntry
  );
}

/**
 * @fn void T_case_body_BspSparcLeon3ReqExtIrqInit( void )
 */
T_TEST_CASE_FIXTURE(
  BspSparcLeon3ReqExtIrqInit,
  &BspSparcLeon3ReqExtIrqInit_Fixture
)
{
  BspSparcLeon3ReqExtIrqInit_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pci[ 0 ] = BspSparcLeon3ReqExtIrqInit_Pre_InterruptMap_Yes;
    ctx->Map.pci[ 0 ] < BspSparcLeon3ReqExtIrqInit_Pre_InterruptMap_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] =
        BspSparcLeon3ReqExtIrqInit_Pre_ExtendedInterrupt_Fixed;
      ctx->Map.pci[ 1 ] < BspSparcLeon3ReqExtIrqInit_Pre_ExtendedInterrupt_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = BspSparcLeon3ReqExtIrqInit_Pre_Ipi_Yes;
        ctx->Map.pci[ 2 ] < BspSparcLeon3ReqExtIrqInit_Pre_Ipi_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = BspSparcLeon3ReqExtIrqInit_Pre_MapFields_None;
          ctx->Map.pci[ 3 ] < BspSparcLeon3ReqExtIrqInit_Pre_MapFields_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] =
              BspSparcLeon3ReqExtIrqInit_Pre_ProcessorMask_None;
            ctx->Map.pci[ 4 ] <
            BspSparcLeon3ReqExtIrqInit_Pre_ProcessorMask_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] =
                BspSparcLeon3ReqExtIrqInit_Pre_ProcessorForce_None;
              ctx->Map.pci[ 5 ] <
              BspSparcLeon3ReqExtIrqInit_Pre_ProcessorForce_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              ctx->Map.entry = BspSparcLeon3ReqExtIrqInit_PopEntry( ctx );

              if ( ctx->Map.entry.Skip ) {
                continue;
              }

              BspSparcLeon3ReqExtIrqInit_SetPreConditionStates( ctx );
              BspSparcLeon3ReqExtIrqInit_TestVariant( ctx );
              BspSparcLeon3ReqExtIrqInit_Cleanup( ctx );
            }
          }
        }
      }
    }
  }
}

/** @} */
