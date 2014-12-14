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
#include "../../../shared/console_private.h"

rtems_device_minor_number         BSPPrintkPort = 0;

#if (BSP_IS_EDISON == 1)
void edison_write_polled(int minor, char cChar); /* XXX */
int edison_inbyte_nonblocking_polled(int minor);
#endif

#if BSP_ENABLE_COM1_COM4
int ns16550_inbyte_nonblocking_polled( int minor );
#endif

void BSP_outch(char ch);
int BSP_inch(void);

void BSP_outch(char ch)
{
  #if BSP_ENABLE_VGA
    if ( BSPPrintkPort == BSP_CONSOLE_VGA ) {
      _IBMPC_outch( ch );
      return;
    }
  #endif
  console_tbl *cptr;

  cptr = &Console_Configuration_Ports[BSPPrintkPort];
  cptr->pDeviceFns->deviceWritePolled( BSPPrintkPort, ch );
}

int BSP_inch(void) 
{
  int           result = -1;

  #if BSP_ENABLE_VGA
    if ( BSPPrintkPort == BSP_CONSOLE_VGA ) {
      result = BSP_wait_polled_input();
    } else
  #endif
  #if BSP_ENABLE_COM1_COM4
    {
      do {
        result = ns16550_inbyte_nonblocking_polled( BSPPrintkPort );
      } while (result == -1);
    }
  #endif
  #if (BSP_IS_EDISON == 1)
    do {
      result = edison_inbyte_nonblocking_polled( BSPPrintkPort );
    } while (result == -1);
  #endif
  return result;
}

BSP_output_char_function_type     BSP_output_char = BSP_outch;
BSP_polling_getchar_function_type BSP_poll_char = BSP_inch;

