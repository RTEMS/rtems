/**
 * @file
 *
 * @ingroup Console
 *
 * @brief pc397 console select
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

#include <stdlib.h>
#include <termios.h>

#include <bsp.h>
#include <libchip/serial.h>
#include <rtems/libio.h>
#include <rtems/console.h>
#include <rtems/termiostypes.h>
#include <bsp/bspimpl.h>

#include "../../../shared/console_private.h"
#ifdef RTEMS_RUNTIME_CONSOLE_SELECT
  #include <crt.h>
#endif

/*
 * Forward prototype
 */
extern bool pc386_com1_com4_enabled(int);

/*
 * This method is used to determine if COM1-COM4 are enabled based upon
 * boot command line arguments.
 */
static bool are_com1_com4_enabled;

bool pc386_com1_com4_enabled(int minor)
{
  return are_com1_com4_enabled;
}

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

static bool bsp_find_console_entry(
  const char                *match,
  size_t                     length,
  rtems_device_minor_number *match_minor
)
{
  rtems_device_minor_number  minor;
  const char                *name;

  for (minor=0; minor < Console_Port_Count ; minor++) {
    console_tbl  *cptr = Console_Port_Tbl[minor];

    /*
     * Console table entries include /dev/ prefix, device names passed
     * in on command line do not.
     */
    name = cptr->sDeviceName  + sizeof("/dev");
    if ( !strncmp( name, match, length ) ) {
      *match_minor = minor;
      return true;
    }
  }

  return false;
}

static void parse_com1_com4_enable(void)
{
  static const char *opt;

  /*
   * Check the command line to see if com1-com4 are disabled.
   */
  opt = bsp_cmdline_arg("--disable-com1-com4");
  if ( opt ) {
    printk( "Disable COM1-COM4 per boot argument\n" );
    are_com1_com4_enabled = false;
  } else {
    are_com1_com4_enabled = true;
  }
}

static bool parse_printk_or_console(
  const char                *param,
  rtems_device_minor_number *minor_out
)
{
  static const char *opt;
  char               working[64] = "";
  char              *p;

  /*
   * Check the command line for the type of mode the console is.
   */
  opt = bsp_cmdline_arg(param);
  if ( !opt ) {
    return false;
  }

  /*
   * bsp_cmdline_arg() returns pointer to a string. It may not be the
   * last string on the command line.
   */
  strncpy( working, opt, sizeof(working) );
  p = strchr( working, ' ' );
  if ( p ) {
    *p = '\0';
  }

  const char                *comma;
  size_t                     length = NAME_MAX;
  rtems_device_minor_number  minor;
  char                      *option = working;

  /*
   * Only match up to a comma or NULL
   */
  comma = strchr (option, ',');

  if ( comma ) {
    length = comma - option;
  }

  option += strnlen(param, 32);

  if ( !bsp_find_console_entry( option, length, &minor ) ) {
    return false;
  }

  *minor_out = minor;
  if (comma) {
    console_tbl *conscfg;

    comma += 1;
    conscfg = &Console_Configuration_Ports[minor];
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
  rtems_device_minor_number  minor;

  /*
   * The console device driver must have its data structures initialized
   * before we can iterate the table of devices for names.
   */
  console_initialize_data();

  /*
   * Determine if COM1-COM4 were disabled.
   */
  parse_com1_com4_enable();

  /*
   * Assume that if only --console is specified, that printk() should
   * follow that selection by default.
   */
  if ( parse_printk_or_console( "--console=", &minor ) ) {
    Console_Port_Minor = minor;
    BSPPrintkPort = minor;
  }

  /*
   * But if explicitly specified, attempt to honor it.
   */
  if ( parse_printk_or_console( "--printk=",  &minor ) ) {
    BSPPrintkPort = minor;
  }

#if 0
  printk( "Console device: %s\n", get_name(Console_Port_Minor) );
  printk( "printk device:  %s\n", get_name(BSPPrintkPort) );
#endif
}

/*
 *  This handles the selection of the console after the devices are
 *  initialized.
 */
void bsp_console_select(void)
{
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
      get_name(Console_Port_Minor)
    );
    Console_Port_Minor = bsp_First_Available_Device();
  }
}
