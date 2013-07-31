/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <libchip/serial.h>
#include <libchip/ns16550.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/alt_clock_manager.h>
#include "socal/alt_rstmgr.h"
#include "socal/socal.h"
#include "socal/alt_uart.h"
#include "socal/hps.h"

bool altera_cyclone_v_uart_probe( int minor );

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

console_tbl Console_Configuration_Ports[] = {
#ifdef CYCLONE_V_CONFIG_CONSOLE
  {
    .sDeviceName   = "/dev/ttyS0",
    .deviceType    = SERIAL_NS16550,
    .pDeviceFns    = &ns16550_fns,
    .deviceProbe   = altera_cyclone_v_uart_probe,
    .pDeviceFlow   = NULL,
    .ulMargin      = 16,
    .ulHysteresis  = 8,
    .pDeviceParams = (void *)CYCLONE_V_UART_BAUD,
    .ulCtrlPort1   = (uint32_t)ALT_UART0_ADDR,
    .ulCtrlPort2   = 0,
    .ulDataPort    = (uint32_t)ALT_UART0_ADDR,
    .getRegister   = altera_cyclone_v_uart_get_register,
    .setRegister   = altera_cyclone_v_uart_set_register,
    .getData       = NULL,
    .setData       = NULL,
    .ulClock       = 0,
    .ulIntVector   = ALT_INT_INTERRUPT_UART0
  },
#endif
#ifdef CYCLONE_V_CONFIG_UART_1
  {
    .sDeviceName   = "/dev/ttyS1",
    .deviceType    = SERIAL_NS16550,
    .pDeviceFns    = &ns16550_fns,
    .deviceProbe   = altera_cyclone_v_uart_probe,
    .pDeviceFlow   = NULL,
    .ulMargin      = 16,
    .ulHysteresis  = 8,
    .pDeviceParams = (void *)CYCLONE_V_UART_BAUD,
    .ulCtrlPort1   = (uint32_t)ALT_UART1_ADDR,
    .ulCtrlPort2   = 0,
    .ulDataPort    = (uint32_t)ALT_UART1_ADDR,
    .getRegister   = altera_cyclone_v_uart_get_register,
    .setRegister   = altera_cyclone_v_uart_set_register,
    .getData       = NULL,
    .setData       = NULL,
    .ulClock       = 0,
    .ulIntVector   = ALT_INT_INTERRUPT_UART1
  }
#endif
};


unsigned long Console_Configuration_Count =
  RTEMS_ARRAY_SIZE(Console_Configuration_Ports);

bool altera_cyclone_v_uart_probe(int minor)
{
  bool            ret           = true;
  uint32_t        uart_set_mask;
  uint32_t        ucr;
  ALT_STATUS_CODE sc;
  void*           location;

  /* The ALT_CLK_L4_SP is required for all SoCFPGA UARTs. 
   * Check that it's enabled. */
  assert( alt_clk_is_enabled(ALT_CLK_L4_SP) == ALT_E_TRUE );
  if ( alt_clk_is_enabled(ALT_CLK_L4_SP) != ALT_E_TRUE ) {
    ret = false;
  }

  if ( ret ) {
    switch(minor)
    {
      case(0):
        /* UART 0 */
        uart_set_mask = ALT_RSTMGR_PERMODRST_UART0_SET_MSK;
        location      = ALT_UART0_ADDR;
      break;
      case(1):
        /* UART 1 */
        uart_set_mask = ALT_RSTMGR_PERMODRST_UART1_SET_MSK;
        location      = ALT_UART1_ADDR;
      break;
      default:
        /* Unknown case */
        assert( minor == 0 || minor == 1 );
        ret = false;
      break;
    }
  }
  if ( ret ) {
    sc = alt_clk_freq_get(ALT_CLK_L4_SP, &Console_Configuration_Ports[minor].ulClock);
    assert( sc == ALT_E_SUCCESS );
    if ( sc != ALT_E_SUCCESS ) {
      ret = false;
    }
  }

  if ( ret ) {
    // Bring UART out of reset.
    alt_clrbits_word(ALT_RSTMGR_PERMODRST_ADDR, uart_set_mask);

    // Verify the UCR (UART Component Version)
    ucr = alt_read_word( ALT_UART_UCV_ADDR( location ) );

    assert( ucr == ALT_UART_UCV_UART_COMPONENT_VER_RESET );
    if ( ucr != ALT_UART_UCV_UART_COMPONENT_VER_RESET ) {
      ret = false;
    }
  }

  if ( ret ) {
    // Write SRR::UR (Shadow Reset Register :: UART Reset)
    alt_write_word( ALT_UART_SRR_ADDR( location ), ALT_UART_SRR_UR_SET_MSK );

    // Read the MSR to work around case:119085.
    (void)alt_read_word( ALT_UART_MSR_ADDR( location ) );
  }

  return ret;
}

static void output_char(char c)
{
  int minor = (int) Console_Port_Minor;
  const console_tbl *ct = Console_Port_Tbl != NULL ?
    Console_Port_Tbl[minor] : &Console_Configuration_Ports[minor];
  const console_fns *cf = ct->pDeviceFns;

  if (c == '\n') {
    (*cf->deviceWritePolled)(minor, '\r');
  }

  (*cf->deviceWritePolled)(minor, c);
}

static void output_char_init(char c)
{
  if (Console_Port_Tbl == NULL) {
    int minor;
    const console_fns *cf;

    bsp_console_select();

    minor = (int) Console_Port_Minor;
    cf = Console_Configuration_Ports[minor].pDeviceFns;

    (*cf->deviceInitialize)(minor);
  }

  BSP_output_char = output_char;
  output_char(c);
}

BSP_output_char_function_type BSP_output_char = output_char_init;

BSP_polling_getchar_function_type BSP_poll_char = NULL;
