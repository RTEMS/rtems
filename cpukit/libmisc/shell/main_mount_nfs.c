/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * 
 * @brief Mount NFS Shell Command Implementation
 */

/*
 * Copyright (C) 2001 Fernando Ruiz Casas <fruizcasas@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
  rtems_shell_filesystems_t* fs RTEMS_UNUSED,
  rtems_filesystem_options_t options RTEMS_UNUSED)
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
