/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup BspSparcLeon3ReqFatalCannotRaiseIpi
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

#include <bsp.h>
#include <rtems.h>
#include <bsp/fatal.h>
#include <bsp/irq.h>
#include <bsp/irqimpl.h>
#include <grlib/io.h>
#include <rtems/bspIo.h>
#include <rtems/sysinit.h>
#include <rtems/test-info.h>
#include <rtems/testopts.h>
#include <rtems/score/atomic.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup BspSparcLeon3ReqFatalCannotRaiseIpi \
 *   spec:/bsp/sparc/leon3/req/fatal-cannot-raise-ipi
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This validation test suite contains a test case which triggers a
 *   fatal error during system initialization.
 *
 * Where the BSP uses the interrupt map, the test defines the inter-processor
 * interrupt bus line by the bus line of TM27_IRQMAP_BUS_LINE. A system
 * initialization handler runs before the BSP reads the interrupt map. It maps
 * the bus line to the first extended controller line. Where the BSP uses no
 * interrupt map, the test defines the inter-processor interrupt bus line as
 * the first extended controller line. An extended controller line cannot be
 * raised on a processor.
 *
 * @{
 */

static void FatalErrorTestCase(
  rtems_fatal_source source,
  rtems_fatal_code   code
)
{
  T_eq_int( source, RTEMS_FATAL_SOURCE_BSP );
  T_eq_ulong( code, LEON3_FATAL_CANNOT_RAISE_IPI );
}

#define _RTEMS_TMTEST27
#include <tm27.h>

#if LEON3_IRQMAP_BUS_LINE_COUNT != 0
/*
 * The test selects a bus line which no driver uses for the
 * inter-processor interrupt.  The BSP defines the constant weakly.
 */
const unsigned char LEON3_mp_irq = TM27_IRQMAP_BUS_LINE;

static void MapInterProcessorInterrupt( void )
{
  uint32_t *reg;
  uint32_t  shift;
  uint32_t  value;

  reg = &LEON3_IrqCtrl_Regs->irqmap[ LEON3_mp_irq / 4 ];
  shift = 24 - 8 * ( LEON3_mp_irq % 4 );
  value = grlib_load_32( reg );
  value &= ~( UINT32_C( 0xff ) << shift );
  value |= (uint32_t) ( BSP_INTERRUPT_VECTOR_MAX_STD + 1 ) << shift;
  grlib_store_32( reg, value );
}

RTEMS_SYSINIT_ITEM(
  MapInterProcessorInterrupt,
  RTEMS_SYSINIT_BSP_EARLY,
  RTEMS_SYSINIT_ORDER_LAST
);
#else
/* The BSP defines the constant weakly. */
const unsigned char LEON3_mp_irq = BSP_INTERRUPT_VECTOR_MAX_STD + 1;
#endif

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_IDLE_TASK_STORAGE_SIZE RTEMS_MINIMUM_STACK_SIZE

static rtems_fatal_source fatal_error_test_source;

static rtems_fatal_code fatal_error_test_code;

T_TEST_CASE( BspSparcLeon3ReqFatalCannotRaiseIpi )
{
  FatalErrorTestCase( fatal_error_test_source, fatal_error_test_code );
}

const char rtems_test_name[] = "BspSparcLeon3ReqFatalCannotRaiseIpi";

static char fatal_error_test_buffer[ 512 ];

static const T_action fatal_error_test_actions[] = { T_report_hash_sha256 };

static const T_config fatal_error_test_config = {
  .name = rtems_test_name,
  .buf = fatal_error_test_buffer,
  .buf_size = sizeof( fatal_error_test_buffer ),
  .putchar = rtems_put_char,
  .verbosity = RTEMS_TEST_VERBOSITY,
#if defined( CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER )
  .now = T_now_clock,
#else
  .now = T_now_tick,
#endif
  .allocate = T_memory_allocate,
  .deallocate = T_memory_deallocate,
  .action_count = T_ARRAY_SIZE( fatal_error_test_actions ),
  .actions = fatal_error_test_actions
};

static bool fatal_error_test_initialized;

static void FatalErrorTestInitialize( void )
{
  if ( !fatal_error_test_initialized ) {
    fatal_error_test_initialized = true;
    rtems_test_begin( rtems_test_name, TEST_STATE );
    T_register();
    T_run_initialize( &fatal_error_test_config );
  }
}

static Atomic_Uint fatal_error_test_counter;

static void FatalErrorTestExtension(
  rtems_fatal_source source,
  bool               always_set_to_false,
  rtems_fatal_code   code
)
{
  rtems_fatal_code exit_code;

  (void) always_set_to_false;

  if ( source == RTEMS_FATAL_SOURCE_EXIT ) {
    return;
  }

  if (
    _Atomic_Fetch_add_uint(
      &fatal_error_test_counter,
      1,
      ATOMIC_ORDER_RELAXED
    ) != 0
  ) {
    return;
  }

  fatal_error_test_source = source;
  fatal_error_test_code = code;
  FatalErrorTestInitialize();
  T_make_runner();
  T_run_all();

  if ( T_run_finalize() ) {
    rtems_test_end( rtems_test_name );
    exit_code = 0;
  } else {
    exit_code = 1;
  }

  rtems_fatal( RTEMS_FATAL_SOURCE_EXIT, exit_code );
}

RTEMS_SYSINIT_ITEM(
  FatalErrorTestInitialize,
  RTEMS_SYSINIT_BSP_EARLY,
  RTEMS_SYSINIT_ORDER_FIRST
);

#if !defined( CONFIGURE_MAXIMUM_FILE_DESCRIPTORS )
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 0

#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM
#endif

#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

#ifdef FATAL_ERROR_TEST_INITIAL_EXTENSION
#define OPTIONAL_FATAL_ERROR_TEST_INITIAL_EXTENSION \
  FATAL_ERROR_TEST_INITIAL_EXTENSION,
#else
#define OPTIONAL_FATAL_ERROR_TEST_INITIAL_EXTENSION
#endif

#define CONFIGURE_INITIAL_EXTENSIONS           \
  OPTIONAL_FATAL_ERROR_TEST_INITIAL_EXTENSION{ \
    .fatal = FatalInitialExtension             \
  },                                           \
  {                                            \
    .fatal = FatalErrorTestExtension           \
  }

#if !defined( CONFIGURE_RTEMS_INIT_TASKS_TABLE )

#define CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION

#if !defined( CONFIGURE_IDLE_TASK_BODY )

#define CONFIGURE_IDLE_TASK_BODY IdleBody

void *IdleBody( uintptr_t ignored )
{
  (void) ignored;

  rtems_fatal( RTEMS_FATAL_SOURCE_EXIT, 1 );
}

#endif /* CONFIGURE_IDLE_TASK_BODY */

#endif /* CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION */

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/** @} */
