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
#include <rtems/shellconfig.h>
#include <rtems/dosfs.h>
#include <rtems/fsmount.h>
#include "internal.h"

int rtems_shell_libc_mounter(const char*                driver,
                             const char*                path,
                             rtems_shell_filesystems_t* fs,
                             rtems_filesystem_options_t options)
{
  rtems_filesystem_mount_table_entry_t* mt_entry;
  /*
   * Mount the disk.
   */
  
  if (mount (&mt_entry, fs->fs_ops, options, (char*) driver, (char*) path) < 0)
  {
    printf ("error: mount failed: %s\n", strerror (errno));
    return 1;
  }
  
  return 0;
}

#define NUMOF(_i) (sizeof (_i) / sizeof (_i[0]))

int rtems_shell_main_mount(int argc, char *argv[])
{
  rtems_filesystem_options_t options = RTEMS_FILESYSTEM_READ_WRITE;
  rtems_shell_filesystems_t* fs = NULL;
  char*                      driver = NULL;
  char*                      mount_point = NULL;
  int                        arg;
  
  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      if (argv[arg][1] == 't')
      {
        rtems_shell_filesystems_t** a;
        
        arg++;
        if (arg == argc)
        {
          fprintf (stdout, "error: -t needs a type of file-system;; see -L.\n");
          return 1;
        }

        for (a = rtems_shell_Mount_filesystems; *a; a++)
        {
          if (strcmp (argv[arg], (*a)->name) == 0)
          {
            fs = *a;
            break;
          }
        }
      }
      else if (argv[arg][1] == 'r')
      {
        options = RTEMS_FILESYSTEM_READ_ONLY;
      }
      else if (argv[arg][1] == 'L')
      {
        rtems_shell_filesystems_t** a;
        fprintf (stdout, "File systems: ");
        for (a = rtems_shell_Mount_filesystems; *a; a++)
          if (*a)
            fprintf (stdout, "%s ", (*a)->name);
        fprintf (stdout, "\n");
        return 1;
      }
      else
      {
        fprintf (stdout, "unknown option: %s\n", argv[arg]);
        return 1;
      }
    }
    else
    {
      if (!driver)
        driver = argv[arg];
      else if (!mount_point)
        mount_point = argv[arg];
      else
      {
        printf ("error: driver and mount only require: %s\n", argv[arg]);
        return 1;
      }
    }
  }

  if (fs == NULL)
  {
    fprintf (stdout, "error: no file-system; see the -L option\n");
    return 1;
  }
  
  if (fs->driver_needed && !driver)
  {
    fprintf (stdout, "error: no driver\n");
    return 1;
  }
  
  if (!mount_point)
  {
    printf ("error: no mount point\n");
    return 1;
  }
  
  /*
   * Mount the disk.
   */
  
  if (fs->mounter (driver, mount_point, fs, options))
    return 1;

  printf ("mounted %s -> %s\n", driver, mount_point);
  
  return 0;
}

rtems_shell_cmd_t rtems_shell_MOUNT_Command = {
  "mount",                                                /* name */
  "mount [-t fstype] [-r] [-L] device path # mount disk", /* usage */
  "files",                                                /* topic */
  rtems_shell_main_mount,                                 /* command */
  NULL,                                                   /* alias */
  NULL                                                    /* next */
};
