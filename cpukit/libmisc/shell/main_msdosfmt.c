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

int rtems_shell_main_msdos_format(int argc, char *argv[])
{
  msdos_format_request_param_t rqdata = {
    OEMName:             "RTEMS",
    VolLabel:            "RTEMSDisk",
    sectors_per_cluster: 0,
    fat_num:             0,
    files_per_root_dir:  0,
    fattype:             MSDOS_FMT_FATANY,
    media:               0,
    quick_format:        TRUE,
    cluster_align:       0
  };
  
  const char* driver = NULL;
  int         arg;
  
  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      switch (argv[arg][1])
      {
        case 'v':
          arg++;
          if (arg == argc)
          {
            printf ("error: no volume label.\n");
            return 1;
          }
          rqdata.VolLabel = argv[arg];
          break;
          
        case 'r':
          arg++;
          if (arg == argc)
          {
            printf ("error: no root directory size.\n");
            return 1;
          }
          rqdata.files_per_root_dir = rtems_shell_str2int(argv[arg]);
          break;
          
        case 't':
          arg++;
          if (arg == argc)
          {
            printf ("error: no FAT type.\n");
            return 1;
          }

          if (strcmp (argv[arg], "any") == 0)
            rqdata.fattype = MSDOS_FMT_FATANY;
          else if (strcmp (argv[arg], "12") == 0)
            rqdata.fattype = MSDOS_FMT_FAT12;
          else if (strcmp (argv[arg], "16") == 0)
            rqdata.fattype = MSDOS_FMT_FAT16;
          else if (strcmp (argv[arg], "32") == 0)
            rqdata.fattype = MSDOS_FMT_FAT32;
          else
          {
            printf ("error: invalid type, can any, 12, 16, or 32\n");
            return 1;
          }
          break;

        default:
          printf ("error: invalid option: %s\n", argv[arg]);
          return 1;
          
      }
    }
    else
    {
      if (!driver)
        driver = argv[arg];
      else
      {
        printf ("error: only one driver allowed: %s\n", argv[arg]);
        return 1;
      }
    }
  }

  if (!driver)
  {
    printf ("error: no driver\n");
    return 1;
  }
  
  printf ("msdos format: %s\n", driver);
  
  if (msdos_format (driver, &rqdata) < 0)
  {
    printf ("error: format failed: %s\n", strerror (errno));
    return 1;
  }

  printf ("msdos format successful\n");

  return 0;
}

#define OPTIONS "[-v label] [-r size] [-t any/12/16/32]"

rtems_shell_cmd_t rtems_Shell_MSDOSFMT_Command = {
  "msdosfmt",                                /* name */
  "msdosfmt " OPTIONS " path # format disk", /* usage */
  "files",                                   /* topic */
  rtems_shell_main_msdos_format,             /* command */
  NULL,                                      /* alias */
  NULL                                       /* next */
};
