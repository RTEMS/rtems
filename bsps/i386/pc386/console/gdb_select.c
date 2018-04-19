/**
 * @file
 *
 * @ingroup GDB
 *
 * @brief pc386 gdb select
 *
 * This file contains a routine to enable and select the UART the gdb stub
 * connects too. Currently limited to COM1 and COM2. See
 * shared/comm/i386-stub-glue.c file.
 */

/*
 *  COPYRIGHT (c) 2016.
 *  Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <stdlib.h>
#include <limits.h>

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/console.h>
#include <rtems/termiostypes.h>
#include <libchip/serial.h>
#include <libchip/ns16550.h>
#include <bsp/bspimpl.h>

#include "../../shared/dev/serial/legacy-console.h"

/*
 * Used in the stub to print output.
 */
int remote_debug;
/*
 * Defined in the stub, used here.
 */
void set_debug_traps(void);

/*
 * Added here to get a valid baudrate. Needs to go once we
 * move to the standard UART driver.
 */
int BSPBaseBaud;

static bool gdb_port_probe(int minor)
{
  /* Return false as GDB has claimed the port */
  return false;
}

void pc386_parse_gdb_arguments(void)
{
  static const char *opt;

  /*
   * Check the command line to see if com1-com4 are disabled.
   */
  opt = bsp_cmdline_arg("--gdb=");
  if ( opt ) {
    const char                *option;
    const char                *comma;
    size_t                     length;
    size_t                     index;
    rtems_device_minor_number  minor;
    uint32_t                   baudrate = 115200;
    bool                       halt = false;
    console_tbl               *port;

    /*
     * Fine the length, there can be more command line visible.
     */
    length = 0;
    while ((opt[length] != ' ') && (opt[length] != '\0')) {
      ++length;
      if (length > NAME_MAX) {
        printk("invalid option (--gdb): too long\n");
        return;
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
      printk("invalid option (--gdb): no equals\n");
      return;
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

    port = console_find_console_entry( option, length, &minor );

    if ( port == NULL ) {
      printk("invalid option (--gdb): port not found\n");
      return;
    }

    if (comma) {
      option = comma + 1;
      baudrate = strtoul(option, 0, 10);
      switch (baudrate) {
        case 115200:
        case 57600:
        case 38400:
        case 19200:
        case 9600:
        case 4800:
          port->pDeviceParams = (void*) baudrate;
          BSPBaseBaud = baudrate; /* REMOVE ME */
          break;
        default:
          printk("invalid option (--gdb): bad baudrate\n");
          return;
      }
    }

    /*
     * Provide a probe that fails so the device is not part of termios. All
     * functions are polling.
     */
    port->deviceProbe = gdb_port_probe;
    port->pDeviceFns = &ns16550_fns_polled;

    opt = bsp_cmdline_arg("--gdb-remote-debug");
    if ( opt ) {
      remote_debug = 1;
    }

    opt = bsp_cmdline_arg("--gdb-break");
    if ( opt ) {
      halt = true;
    }

    printk("GDB stub: enable %s%s%s\n",
           port->sDeviceName,
           remote_debug ? ", remote-debug" : "",
           halt ? ", halting" : "");

    i386_stub_glue_init(minor);
    set_debug_traps();
    i386_stub_glue_init_breakin();

    if ( halt ) {
      printk("GDB stub: waiting for remote connection..\n");
      breakpoint();
    }
  }
}
