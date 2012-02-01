/**
 * @file
 *
 * @ingroup Console
 *
 * @brief Generic libchip console select
 *
 * This file contains a routine to select the console based upon a number
 * of criteria.
 */

/*
 *  COPYRIGHT (c) 2011-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>
#include <termios.h>

#include <rtems/termiostypes.h>
#include <libchip/serial.h>
#include "../../../shared/console_private.h"
#ifdef RTEMS_RUNTIME_CONSOLE_SELECT
  #include <crt.h>
#endif

/*
 * Method to return true if the device associated with the
 * minor number probs available.
 */
static bool bsp_Is_Available( rtems_device_minor_number minor )
{
  console_tbl  *cptr = Console_Port_Tbl[minor];

  /*
   * First perform the configuration dependent probe, then the
   * device dependent probe
   */
  if ((!cptr->deviceProbe || cptr->deviceProbe(minor)) &&
       cptr->pDeviceFns->deviceProbe(minor)) {
    return true;
  }
  return false;
}

/*
 * Method to return the first available device.
 */
static rtems_device_minor_number bsp_First_Available_Device( void )
{
  rtems_device_minor_number minor;

  for (minor=0; minor < Console_Port_Count ; minor++) {
    console_tbl  *cptr = Console_Port_Tbl[minor];

    /*
     * First perform the configuration dependent probe, then the
     * device dependent probe
     */

    if ((!cptr->deviceProbe || cptr->deviceProbe(minor)) &&
         cptr->pDeviceFns->deviceProbe(minor)) {
      return minor;
    }
  }

  /*
   *  Error No devices were found.  We will want to bail here.
   */
  rtems_fatal_error_occurred(RTEMS_IO_ERROR);
}

void bsp_console_select(void)
{
  static const char* opt;

  /*
   * Check the command line for the type of mode the console is.
   */
  opt = bsp_cmdline_arg ("--console=");

  if (opt) {
    const char* comma;

    opt += sizeof ("--console=") - 1;
    if (strncmp (opt, "console", sizeof ("console") - 1) == 0) {
      Console_Port_Minor = BSP_CONSOLE_VGA;
      BSPPrintkPort      = BSP_CONSOLE_VGA;
    } else if (strncmp (opt, "com1", sizeof ("com1") - 1) == 0) {
      Console_Port_Minor = BSP_CONSOLE_COM1;
      BSPPrintkPort      = BSP_CONSOLE_COM1;
    } else if (strncmp (opt, "com2", sizeof ("com2") - 1) == 0) {
      Console_Port_Minor = BSP_CONSOLE_COM2;
      BSPPrintkPort      = BSP_CONSOLE_COM2;
    }

    comma = strchr (opt, ',');

    if (comma) {
      console_tbl *conscfg;

      comma += 1;
      conscfg = &Console_Configuration_Ports[Console_Port_Minor];
      if (strncmp (opt, "115200", sizeof ("115200") - 1) == 0)
        conscfg->pDeviceParams = (void *)115200;
      else if (strncmp (opt, "57600", sizeof ("57600") - 1) == 0)
        conscfg->pDeviceParams = (void *)57600;
      else if (strncmp (opt, "38400", sizeof ("38400") - 1) == 0)
        conscfg->pDeviceParams = (void *)38400;
      else if (strncmp (opt, "19200", sizeof ("19200") - 1) == 0)
        conscfg->pDeviceParams = (void *)19200;
      else if (strncmp (opt, "9600", sizeof ("9600") - 1) == 0)
        conscfg->pDeviceParams = (void *)9600;
      else if (strncmp (opt, "4800", sizeof ("4800") - 1) == 0)
        conscfg->pDeviceParams = (void *)4800;
    }
  }

  #ifdef RTEMS_RUNTIME_CONSOLE_SELECT
    if ( BSP_runtime_console_select )
      BSP_runtime_console_select(&BSPPrintkPort, &Console_Port_Minor);

    /*
     * If no video card, fall back to serial port console
     */
    if((Console_Port_Minor == BSP_CONSOLE_VGA)
     && (*(unsigned char*) NB_MAX_ROW_ADDR == 0)
     && (*(unsigned short*)NB_MAX_COL_ADDR == 0)) {
      Console_Port_Minor = BSP_CONSOLE_COM2;
      BSPPrintkPort      = BSP_CONSOLE_COM1;
    }
  #endif

  /*
   * If the device that was selected isn't available then
   * let the user know and select the first available device.
   */
  if ( !bsp_Is_Available( Console_Port_Minor ) ) {
    printk(
      "Error finding %s setting console to first available\n",
      Console_Port_Tbl[Console_Port_Minor]->sDeviceName
    );
    Console_Port_Minor = bsp_First_Available_Device();
  }
}
