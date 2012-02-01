/*
 * @file
 *
 * @ingroup Console
 *
 * @brief printk support routines
 *
 * This file contains the required printk support.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/keyboard.h>
#include <bsp.h>
#include <libchip/serial.h>
#include <libchip/ns16550.h>
#include "../../../shared/console_private.h"

rtems_device_minor_number         BSPPrintkPort = 0;

int ns16550_inbyte_nonblocking_polled(
  int minor
);

void BSP_outch(char ch);
int BSP_inch(void);

void BSP_outch(char ch)
{
  if ( BSPPrintkPort == BSP_CONSOLE_VGA ) {
    _IBMPC_outch( ch );
  } else {
    console_tbl *cptr;

    cptr = &Console_Configuration_Ports[BSPPrintkPort];
    cptr->pDeviceFns->deviceWritePolled( BSPPrintkPort, ch );
  }
}

int BSP_inch(void) 
{
  int           result;

  if ( BSPPrintkPort == BSP_CONSOLE_VGA ) {
    result = BSP_wait_polled_input();
  } else {
    do {
      result = ns16550_inbyte_nonblocking_polled( BSPPrintkPort );
    } while (result == -1);
  }
  return result;
}

BSP_output_char_function_type     BSP_output_char = BSP_outch;
BSP_polling_getchar_function_type BSP_poll_char = BSP_inch;

