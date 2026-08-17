/* SPDX-License-Identifier: BSD-2-Clause */

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

#include <rtems.h>
#include <rtems/bspIo.h>
#if BSP_ENABLE_VGA
  #include <rtems/keyboard.h>
#endif
#include <bsp.h>
#include <bsp/bspimpl.h>
#include <libchip/serial.h>
#include <libchip/ns16550.h>
#include <libchip/ns16550_p.h>
#include "../../shared/dev/serial/legacy-console.h"

#if USE_COM1_AS_CONSOLE
rtems_device_minor_number BSPPrintkPort = BSP_CONSOLE_COM1;
#else
rtems_device_minor_number BSPPrintkPort = BSP_CONSOLE_VGA;
#endif

static bool serialInit;
static bool serialOK;

/*
 * The console driver builds its port table with a memory allocation in
 * bsp_start().  Everything it offers uses that table, so nothing of it works
 * before the allocation, and a system which terminates in an early system
 * initialization item would print nothing at all.  Drive the port directly
 * until the table is there.  The port keeps the settings of the firmware,
 * since programming it would need the same table.
 */
static bool early_write_polled(const console_tbl *port, char ch)
{
  uint8_t status;

  if (port == NULL || port->deviceType != SERIAL_NS16550) {
    return false;
  }

  do {
    inport_byte(port->ulCtrlPort1 + NS16550_LINE_STATUS, status);
  } while ((status & SP_LSR_THOLD) == 0);

  outport_byte(port->ulDataPort + NS16550_TRANSMIT_BUFFER, ch);
  return true;
}

static bool serialValid(console_tbl *port)
{
  if (port != NULL && port->pDeviceFns) {
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
    console_tbl *port = pc386_console_port(BSPPrintkPort);

    if (Console_Port_Tbl == NULL) {
      if (early_write_polled(port, ch)) {
        return;
      }
    } else if (serialValid(port)) {
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

  if ( !isVga && Console_Port_Tbl != NULL ) {
    console_tbl *port = pc386_console_port(BSPPrintkPort);
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
