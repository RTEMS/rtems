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
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/bspIo.h>
#if BSP_ENABLE_VGA
  #include <rtems/keyboard.h>
#endif
#include <bsp.h>
#include <libchip/serial.h>
#include <libchip/ns16550.h>
#include "../../shared/dev/serial/legacy-console.h"

rtems_device_minor_number BSPPrintkPort = 0;

static bool serialInit;
static bool serialOK;

static bool serialValid(console_tbl *port)
{
  if (port->pDeviceFns) {
    if (!serialInit) {
      serialOK = true;
      if (port->pDeviceFns->deviceProbe != NULL) {
        if (!port->pDeviceFns->deviceProbe( BSPPrintkPort ))
          serialOK = false;
        else if (port->pDeviceFns->deviceInitialize != NULL)
          port->pDeviceFns->deviceInitialize( BSPPrintkPort );
        else
          serialOK = false;
      }
      serialInit = true;
    }
  }
  return serialOK;
}

void BSP_outch(char ch);
int BSP_inch(void);

void BSP_outch(char ch)
{
  #if BSP_ENABLE_VGA
    bool isVga =  BSPPrintkPort == BSP_CONSOLE_VGA;
  #else
    bool isVga = false;
  #endif

  if ( !isVga ) {
    console_tbl *port = Console_Port_Tbl[BSPPrintkPort];
    if (serialValid(port)) {
      if (port->pDeviceFns->deviceWritePolled) {
        port->pDeviceFns->deviceWritePolled( BSPPrintkPort, ch );
      }
      return;
    }
  }

  #if BSP_ENABLE_VGA
    _IBMPC_outch( ch );
  #endif
}

int BSP_inch(void)
{
  #if BSP_ENABLE_VGA
    bool isVga =  BSPPrintkPort == BSP_CONSOLE_VGA;
  #else
    bool isVga = false;
  #endif

  int result = -1;

  if ( !isVga ) {
    console_tbl *port = Console_Port_Tbl[BSPPrintkPort];
    if (serialValid(port)) {
      if (port->pDeviceFns->deviceRead) {
        do {
          result = port->pDeviceFns->deviceRead( BSPPrintkPort );
        } while (result == -1);
        return result;
      }
    }
  }

  #if BSP_ENABLE_VGA
    result = BSP_wait_polled_input();
  #endif

  return result;
}

BSP_output_char_function_type     BSP_output_char = BSP_outch;
BSP_polling_getchar_function_type BSP_poll_char = BSP_inch;
