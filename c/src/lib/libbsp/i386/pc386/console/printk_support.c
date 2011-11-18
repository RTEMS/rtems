/*
 *  This file contains a stub for the required printk support.
 *  It is NOT functional!!!
 *
 *  This driver uses the termios pseudo driver.
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/keyboard.h>
#include <bsp.h>
#include <libchip/serial.h>
#include <libchip/ns16550.h>

BSP_output_char_function_type     BSP_output_char = _IBMPC_outch;
BSP_polling_getchar_function_type BSP_poll_char = BSP_wait_polled_input;

rtems_device_minor_number         BSPPrintkPort = 0;

int ns16550_inbyte_nonblocking_polled(
  int minor
);

void BSP_com_outch(char ch)
{
  console_tbl                   *cptr;

  cptr = &Console_Configuration_Ports[BSPPrintkPort];

  return cptr->pDeviceFns->deviceWritePolled( BSPPrintkPort, ch );
}

int BSP_com_inch( void ) 
{
  int           result;
  console_tbl   *cptr;

  cptr = &Console_Configuration_Ports[BSPPrintkPort];

  do {
    result = ns16550_inbyte_nonblocking_polled( BSPPrintkPort );
  } while (result == -1);

  return result;
}

