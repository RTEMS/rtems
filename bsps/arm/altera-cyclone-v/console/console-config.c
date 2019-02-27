/**
 * @file
 *
 * @ingroup RTEMSBSPsARMCycV
 */

/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <libchip/ns16550.h>

#include <rtems/bspIo.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/alt_clock_manager.h>
#include <bsp/console-termios.h>
#include <bsp/socal/alt_rstmgr.h>
#include <bsp/socal/socal.h>
#include <bsp/socal/alt_uart.h>
#include <bsp/socal/hps.h>

#ifdef BSP_USE_UART_INTERRUPTS
  #define DEVICE_FNS &ns16550_handler_interrupt
#else
  #define DEVICE_FNS &ns16550_handler_polled
#endif

static uint8_t altera_cyclone_v_uart_get_register(uintptr_t addr, uint8_t i)
{
  volatile uint32_t *reg = (volatile uint32_t *) addr;

  return (uint8_t) reg [i];
}

static void altera_cyclone_v_uart_set_register(uintptr_t addr, uint8_t i, uint8_t val)
{
  volatile uint32_t *reg = (volatile uint32_t *) addr;

  reg [i] = val;
}

static bool altera_cyclone_v_uart_probe(
  rtems_termios_device_context *base,
  uint32_t uart_set_mask
)
{
  ns16550_context *ctx = (ns16550_context *) base;
  bool            ret           = true;
  uint32_t        ucr;
  ALT_STATUS_CODE sc;
  void*           location = (void *) ctx->port;

  /* The ALT_CLK_L4_SP is required for all SoCFPGA UARTs. 
   * Check that it's enabled. */
  if ( alt_clk_is_enabled(ALT_CLK_L4_SP) != ALT_E_TRUE ) {
    ret = false;
  }

  if ( ret ) {
    sc = alt_clk_freq_get(ALT_CLK_L4_SP, &ctx->clock);
    if ( sc != ALT_E_SUCCESS ) {
      ret = false;
    }
  }

  if ( ret ) {
    // Bring UART out of reset.
    alt_clrbits_word(ALT_RSTMGR_PERMODRST_ADDR, uart_set_mask);

    // Verify the UCR (UART Component Version)
    ucr = alt_read_word( ALT_UART_UCV_ADDR( location ) );
    if ( ucr != ALT_UART_UCV_UART_COMPONENT_VER_RESET ) {
      ret = false;
    }
  }

  if ( ret ) {
    // Write SRR::UR (Shadow Reset Register :: UART Reset)
    alt_write_word( ALT_UART_SRR_ADDR( location ), ALT_UART_SRR_UR_SET_MSK );

    // Read the MSR to work around case:119085.
    (void)alt_read_word( ALT_UART_MSR_ADDR( location ) );

    ret = ns16550_probe( base );
  }

  return ret;
}

#ifdef CYCLONE_V_CONFIG_CONSOLE
static bool altera_cyclone_v_uart_probe_0(rtems_termios_device_context *base)
{
  return altera_cyclone_v_uart_probe(base, ALT_RSTMGR_PERMODRST_UART0_SET_MSK);
}

static ns16550_context altera_cyclone_v_uart_context_0 = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART 0"),
  .get_reg = altera_cyclone_v_uart_get_register,
  .set_reg = altera_cyclone_v_uart_set_register,
  .port = (uintptr_t) ALT_UART0_ADDR,
  .irq = ALT_INT_INTERRUPT_UART0,
  .initial_baud = CYCLONE_V_UART_BAUD
};
#endif

#ifdef CYCLONE_V_CONFIG_UART_1
static bool altera_cyclone_v_uart_probe_1(rtems_termios_device_context *base)
{
  return altera_cyclone_v_uart_probe(base, ALT_RSTMGR_PERMODRST_UART1_SET_MSK);
}

static ns16550_context altera_cyclone_v_uart_context_1 = {
  .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER("UART 1"),
  .get_reg = altera_cyclone_v_uart_get_register,
  .set_reg = altera_cyclone_v_uart_set_register,
  .port = (uintptr_t) ALT_UART1_ADDR,
  .irq = ALT_INT_INTERRUPT_UART1,
  .initial_baud = CYCLONE_V_UART_BAUD
};
#endif

const console_device console_device_table[] = {
  #ifdef CYCLONE_V_CONFIG_CONSOLE
    {
      .device_file = "/dev/ttyS0",
      .probe = altera_cyclone_v_uart_probe_0,
      .handler = DEVICE_FNS,
      .context = &altera_cyclone_v_uart_context_0.base
    },
  #endif
  #ifdef CYCLONE_V_CONFIG_UART_1
    {
      .device_file = "/dev/ttyS1",
      .probe = altera_cyclone_v_uart_probe_1,
      .handler = DEVICE_FNS,
      .context = &altera_cyclone_v_uart_context_1.base
    },
  #endif
};

const size_t console_device_count = RTEMS_ARRAY_SIZE(console_device_table);

static void output_char(char c)
{
  rtems_termios_device_context *ctx = console_device_table[0].context;

  ns16550_polled_putchar( ctx, c );
}

BSP_output_char_function_type BSP_output_char = output_char;

BSP_polling_getchar_function_type BSP_poll_char = NULL;
