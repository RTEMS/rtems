/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief Startup code.
 */

/*
 * Copyright (c) 2008-2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/io.h>
#include <bsp/irq.h>
#include <bsp/dma.h>
#include <bsp/bootcard.h>
#include <bsp/timer.h>
#include <bsp/irq-generic.h>
#include <bsp/system-clocks.h>
#include <bsp/linker-symbols.h>
#include <bsp/common-types.h>
#include <bsp/uart-output-char.h>

#ifdef LPC176X_HEAP_EXTEND
LINKER_SYMBOL( lpc176x_region_heap_0_begin );
LINKER_SYMBOL( lpc176x_region_heap_0_size );
LINKER_SYMBOL( lpc176x_region_heap_0_end );
LINKER_SYMBOL( lpc176x_region_heap_1_begin );
LINKER_SYMBOL( lpc176x_region_heap_1_size );
LINKER_SYMBOL( lpc176x_region_heap_1_end );
extern Heap_Control *RTEMS_Malloc_Heap;
#endif

static void heap_extend( void )
{
#ifdef LPC176X_HEAP_EXTEND
  _Heap_Extend( RTEMS_Malloc_Heap,
    lpc176x_region_heap_0_begin,
    (uintptr_t) lpc176x_region_heap_0_size,
    NULL );
  _Heap_Extend( RTEMS_Malloc_Heap,
    lpc176x_region_heap_1_begin,
    (uintptr_t) lpc176x_region_heap_1_size,
    NULL );
#endif
}

/**
 * @brief Console initialization
 */
static void initialize_console( void )
{
#ifdef LPC176X_CONFIG_CONSOLE

  lpc176x_module_enable( LPC176X_MODULE_UART_0, LPC176X_MODULE_PCLK_DEFAULT );

  lpc176x_pin_select( LPC176X_PIN_UART_0_TXD, LPC176X_PIN_FUNCTION_01 );
  lpc176x_pin_select( LPC176X_PIN_UART_0_RXD, LPC176X_PIN_FUNCTION_01 );

  BSP_CONSOLE_UART_INIT( LPC176X_PCLK / 16 / LPC176X_UART_BAUD );
#endif
}

void bsp_start( void )
{
  /* Initialize console */
  initialize_console();

  /*Initialize timer*/
  lpc176x_timer_init( LPC176X_TIMER_1 );
  lpc176x_timer_start( LPC176X_TIMER_1 );

  /* Interrupts */
  bsp_interrupt_initialize();

  /* DMA */
  lpc176x_dma_initialize();

  heap_extend();
}
