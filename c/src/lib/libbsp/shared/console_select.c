/**
 * @file
 *
 * @ingroup Console
 *
 * @brief Generic libchip console select
 */

/*
 *  This file contains a routine to select the
 *  console based upon a number of criteria.
 *
 *  COPYRIGHT (c) 2011.
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
#include "console_private.h"

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

  /*
   *  Reset Console_Port_Minor and
   *  BSPPrintkPort here if desired.
   *
   *  This default version allows the bsp to set these
   *  values at creation and will not touch them again
   *  unless the selected port number is not available.
   */

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
