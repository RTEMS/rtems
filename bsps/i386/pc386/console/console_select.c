/**
 * @file
 *
 * @ingroup Console
 *
 * @brief pc386 console select
 *
 * This file contains a routine to select the console based upon a number
 * of criteria.
 */

/*
 *  COPYRIGHT (c) 2011-2012, 2016.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <limits.h>
#include <stdlib.h>
#include <termios.h>

#include <bsp.h>
#include <libchip/serial.h>
#include <rtems/libio.h>
#include <rtems/console.h>
#include <rtems/termiostypes.h>
#include <bsp/bspimpl.h>

#include "../../shared/dev/serial/legacy-console.h"
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

static bool parse_printk_or_console(
  const char                *param,
  rtems_device_minor_number *minor_out
)
{
  static const char         *opt;
  const char                *option;
  const char                *comma;
  size_t                     length;
  size_t                     index;
  rtems_device_minor_number  minor;
  console_tbl               *conscfg;

  /*
   * Check the command line for the type of mode the console is.
   */
  opt = bsp_cmdline_arg(param);
  if ( !opt ) {
    return false;
  }

  /*
   * Fine the length, there can be more command line visible.
   */
  length = 0;
  while ((opt[length] != ' ') && (opt[length] != '\0')) {
    ++length;
    if (length > NAME_MAX) {
      printk("invalid option (%s): too long\n", param);
      return false;
    }
  }

  /*
   * Only match up to a comma or NULL
   */
  index = 0;
  while ((opt[index] != '=') && (index < length)) {
    ++index;
  }

  if (opt[index] != '=') {
    printk("invalid option (%s): no equals\n", param);
    return false;
  }

  ++index;
  option = &opt[index];

  while ((opt[index] != ',') && (index < length)) {
    ++index;
  }

  if (opt[index] == ',')
    comma = &opt[index];
  else
    comma = NULL;

  length = &opt[index] - option;

  conscfg = console_find_console_entry( option, length, &minor );
  if ( conscfg == NULL ) {
    return false;
  }

  *minor_out = minor;
  if (comma) {
    option = comma + 1;
    if (strncmp (option, "115200", sizeof ("115200") - 1) == 0)
      conscfg->pDeviceParams = (void *)115200;
    else if (strncmp (option, "57600", sizeof ("57600") - 1) == 0)
      conscfg->pDeviceParams = (void *)57600;
    else if (strncmp (option, "38400", sizeof ("38400") - 1) == 0)
      conscfg->pDeviceParams = (void *)38400;
    else if (strncmp (option, "19200", sizeof ("19200") - 1) == 0)
      conscfg->pDeviceParams = (void *)19200;
    else if (strncmp (option, "9600", sizeof ("9600") - 1) == 0)
      conscfg->pDeviceParams = (void *)9600;
    else if (strncmp (option, "4800", sizeof ("4800") - 1) == 0)
      conscfg->pDeviceParams = (void *)4800;
  }

  return true;
}

/*
 * Helper to retrieve device name
 */
static inline const char *get_name(
  rtems_device_minor_number  minor
)
{
  return Console_Port_Tbl[minor]->sDeviceName;
}

/*
 * Parse the arguments early so the printk and console ports are
 * set appropriately.
 */
void pc386_parse_console_arguments(void)
{
  rtems_device_minor_number minor;
  rtems_device_minor_number minor_console = 0;
  rtems_device_minor_number minor_printk = 0;

  /*
   * Assume that if only --console is specified, that printk() should
   * follow that selection by default.
   */
  if ( parse_printk_or_console( "--console=", &minor ) ) {
    minor_console = minor;
    minor_printk = minor;
  }

  /*
   * But if explicitly specified, attempt to honor it.
   */
  if ( parse_printk_or_console( "--printk=",  &minor ) ) {
    minor_printk = minor;
  }

  printk( "Console: %s printk: %s\n",
          get_name(minor_console),get_name(minor_printk) );

  /*
   * Any output after this can cause problems until termios is initialised.
   */
  Console_Port_Minor = minor_console;
  BSPPrintkPort = minor_printk;
}

/*
 *  This handles the selection of the console after the devices are
 *  initialized.
 */
void bsp_console_select(void)
{
  #ifdef RTEMS_RUNTIME_CONSOLE_SELECT
    /*
     * WARNING: This code is really needed any more and should be removed.
     *          references to COM1 and COM2 like they are wrong.
     */
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
      get_name(Console_Port_Minor)
    );
    Console_Port_Minor = bsp_First_Available_Device();
  }
}
