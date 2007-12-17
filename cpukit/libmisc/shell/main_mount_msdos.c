/*
 *   Shell Command Implmentation
 *
 *  Author: Fernando RUIZ CASAS
 *  Work: fernando.ruiz@ctv.es
 *  Home: correo@fernando-ruiz.com
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
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
#include <rtems/dosfs.h>
#include <rtems/fsmount.h>
#include "internal.h"

rtems_shell_filesystems_t rtems_shell_Mount_MSDOS = {
  name:          "msdos",
  driver_needed: 1,
  fs_ops:        &msdos_ops,
  mounter:       rtems_shell_libc_mounter
};
