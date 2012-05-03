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
#include "internal.h"

#include <librtemsNfs.h>

static int
rtems_shell_nfs_mounter (
  const char*                device,
  const char*                mntpoint,
  rtems_shell_filesystems_t* fs __attribute__((unused)),
  rtems_filesystem_options_t options __attribute__((unused)))
{
  char* uidhost;
  char* path;
  int   ret;

  if (strchr (device, ':') == NULL) {
    fprintf (stderr, "error: nfs mount device is [uid.gid@]host:path\n");
    return -1;
  }

  if (rpcUdpInit () < 0) {
    fprintf (stderr, "error: initialising RPC\n");
    return -1;
  }

  nfsInit (0, 0);

  uidhost = strdup (device);
  path = strchr (uidhost, ':');
  *path = '\0';
  path++;

  ret = nfsMount(uidhost, path, (char*) mntpoint);

  free (uidhost);

  return ret;
}

rtems_shell_filesystems_t rtems_shell_Mount_NFS = {
  name:          "nfs",
  driver_needed: 1,
  fs_ops:        NULL,
  mounter:       rtems_shell_nfs_mounter
};
