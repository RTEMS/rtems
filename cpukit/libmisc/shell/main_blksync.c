/*
 *  RM Shell Command Implmentation
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
#include <fcntl.h>

#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/bdbuf.h>
#include <rtems/blkdev.h>
#include "internal.h"

int rtems_shell_main_blksync(int argc, char *argv[])
{
  const char* driver = NULL;
  int         arg;
  
  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      printf ("error: invalid option: %s\n", argv[arg]);
      return 1;
    }
    else
    {
      if (!driver)
        driver = argv[arg];
      else
      {
        printf ("error: only one driver name allowed: %s\n", argv[arg]);
        return 1;
      }
    }
  }
  
  int fd = open (driver, O_WRONLY, 0);
  if (fd < 0)
  {
    printf ("error: driver open failed: %s\n", strerror (errno));
    return 1;
  }
  
  if (ioctl (fd, BLKIO_SYNCDEV) < 0)
  {
    printf ("error: driver sync failed: %s\n", strerror (errno));
    return 1;
  }
  
  close (fd);
  
  return 0;
}

rtems_shell_cmd_t rtems_Shell_BLKSYNC_Command = {
  "blksync",                                 /* name */
  "blksync driver # sync the block driver",  /* usage */
  "files",                                   /* topic */
  rtems_shell_main_blksync,                  /* command */
  NULL,                                      /* alias */
  NULL                                       /* next */
};
