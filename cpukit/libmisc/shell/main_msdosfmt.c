/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>

#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/stringto.h>
#include <rtems/shellconfig.h>
#include <rtems/dosfs.h>
#include <rtems/fsmount.h>
#include "internal.h"

static int rtems_shell_main_msdos_format(
  int   argc,
  char *argv[]
)
{
  msdos_format_request_param_t rqdata = {
    .OEMName =             "RTEMS",
    .VolLabel =            "RTEMSDisk",
    .sectors_per_cluster = 0,
    .fat_num =             0,
    .files_per_root_dir =  0,
    .media =               0,
    .quick_format =        TRUE,
    .skip_alignment =      0,
    .info_level =          0
  };

  unsigned long tmp;
  const char*   driver = NULL;
  int           arg;

  for (arg = 1; arg < argc; arg++) {
    if (argv[arg][0] == '-') {
      switch (argv[arg][1]) {
        case 'V':
          arg++;
          if (arg == argc) {
            fprintf (stderr, "error: no volume label.\n");
            return 1;
          }
          rqdata.VolLabel = argv[arg];
          break;

        case 's':
          arg++;
          if (arg == argc) {
            fprintf (stderr, "error: sectors per cluster count.\n");
            return 1;
          }

          if ( rtems_string_to_unsigned_long(argv[arg], &tmp, NULL, 0) ) {
            printf(
              "sector per cluster argument (%s) is not a number\n",
               argv[arg]
            );
            return -1;
          }

          rqdata.sectors_per_cluster = (uint32_t) tmp;
          break;

        case 'r':
          arg++;
          if (arg == argc) {
            fprintf (stderr, "error: no root directory size.\n");
            return 1;
          }

          if ( rtems_string_to_unsigned_long(argv[arg], &tmp, NULL, 0) ) {
            printf(
              "root directory size argument (%s) is not a number\n",
               argv[arg]
            );
            return -1;
          }

          rqdata.files_per_root_dir = (uint32_t) tmp;
          break;

        case 'v':
          rqdata.info_level++;
          break;

        default:
          fprintf (stderr, "error: invalid option: %s\n", argv[arg]);
          return 1;

      }
    } else {
      if (!driver)
        driver = argv[arg];
      else {
        fprintf (stderr, "error: only one driver allowed: %s\n", argv[arg]);
        return 1;
      }
    }
  }

  if (!driver) {
    fprintf (stderr, "error: no driver\n");
    return 1;
  }

  printf ("msdos format: %s\n", driver);

  if (rqdata.info_level)
  {
    printf (" %-20s: %s\n", "OEMName", "RTEMS");
    printf (" %-20s: %s\n", "VolLabel", "RTEMSDisk");
    printf (" %-20s: %" PRIu32 "\n", "sectors per cluster", rqdata.sectors_per_cluster);
    printf (" %-20s: %" PRIu32 "\n", "fats", rqdata.fat_num);
    printf (" %-20s: %" PRIu32 "\n", "files per root dir", rqdata.files_per_root_dir);
    printf (" %-20s: %d\n", "media", rqdata.media);
    printf (" %-20s: %d\n", "quick_format", rqdata.quick_format);
    printf (" %-20s: %s\n", "skip_alignment", (0 == rqdata.skip_alignment) ? "false" : "true");
  }

  if (msdos_format (driver, &rqdata) < 0) {
    fprintf (stderr, "error: format failed: %s\n", strerror (errno));
    return 1;
  }

  printf ("msdos format successful\n");

  return 0;
}

#define OPTIONS "[-V label] [-s sectors/cluster] [-r size] [-v]"

rtems_shell_cmd_t rtems_shell_MSDOSFMT_Command = {
  "mkdos",                                   /* name */
  "mkdos " OPTIONS " path # format disk",    /* usage */
  "files",                                   /* topic */
  rtems_shell_main_msdos_format,             /* command */
  NULL,                                      /* alias */
  NULL                                       /* next */
};

rtems_shell_cmd_t rtems_shell_MSDOSFMT_Alias = {
  "msdosfmt",                                /* name */
  NULL,                                      /* usage */
  "files",                                   /* topic */
  rtems_shell_main_msdos_format,             /* command */
  &rtems_shell_MSDOSFMT_Command,             /* alias */
  NULL                                       /* next */
};
