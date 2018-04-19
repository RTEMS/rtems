/**
 * @file
 *
 * @ingroup raspberrypi_console
 *
 * @brief console select
 */

/*
 * Copyright (c) 2015 Yang Qiao
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.org/license/LICENSE
 *
 */

#include <bsp.h>
#include <bsp/fatal.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <termios.h>

#include <rtems/termiostypes.h>
#include <libchip/serial.h>
#include "../../shared/dev/serial/legacy-console.h"
#include <bsp/rpi-fb.h>

rtems_device_minor_number BSPPrintkPort = 0;

/*
 * Method to return true if the device associated with the
 * minor number probs available.
 */
static bool bsp_Is_Available( rtems_device_minor_number minor )
{
  console_tbl *cptr = Console_Port_Tbl[ minor ];

  /*
   * First perform the configuration dependent probe, then the
   * device dependent probe
   */
  if ( ( !cptr->deviceProbe || cptr->deviceProbe( minor ) ) &&
       cptr->pDeviceFns->deviceProbe( minor ) ) {
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

  for ( minor = 0; minor < Console_Port_Count; minor++ ) {
    console_tbl *cptr = Console_Port_Tbl[ minor ];

    /*
     * First perform the configuration dependent probe, then the
     * device dependent probe
     */

    if ( ( !cptr->deviceProbe || cptr->deviceProbe( minor ) ) &&
         cptr->pDeviceFns->deviceProbe( minor ) ) {
      return minor;
    }
  }

  /*
   *  Error No devices were found.  We will want to bail here.
   */
  bsp_fatal( BSP_FATAL_CONSOLE_NO_DEV );
}

void bsp_console_select( void )
{
  /*
   *  Reset Console_Port_Minor and
   *  BSPPrintkPort here if desired.
   *
   *  This default version allows the bsp to set these
   *  values at creation and will not touch them again
   *  unless the selected port number is not available.
   */
  const char *opt;

  Console_Port_Minor = BSP_CONSOLE_UART0;
  BSPPrintkPort = BSP_CONSOLE_UART0;

  opt = rpi_cmdline_get_arg( "--console=" );

  if ( opt ) {
    if ( strncmp( opt, "fbcons", sizeof( "fbcons" ) - 1 ) == 0 ) {
      if ( rpi_video_is_initialized() > 0 ) {
        Console_Port_Minor = BSP_CONSOLE_FB;
        BSPPrintkPort = BSP_CONSOLE_FB;
      }
    }
  }

  /*
   * If the device that was selected isn't available then
   * let the user know and select the first available device.
   */
  if ( !bsp_Is_Available( Console_Port_Minor ) ) {
    Console_Port_Minor = bsp_First_Available_Device();
  }
}
