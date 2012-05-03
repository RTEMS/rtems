/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/stringto.h>
#include <rtems/shellconfig.h>
#include <rtems/rtems-rfs-shell.h>
#include <rtems/fsmount.h>
#include "internal.h"

#define OPTIONS "[-h]"

rtems_shell_cmd_t rtems_shell_DEBUGRFS_Command = {
  "debugrfs",                                /* name */
  "debugrfs " OPTIONS,                       /* usage */
  "files",                                   /* topic */
  rtems_shell_debugrfs,                      /* command */
  NULL,                                      /* alias */
  NULL                                       /* next */
};
