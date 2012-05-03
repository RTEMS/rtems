/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems-rfs
 *
 * Set of utility functions to support RTEMS RFS on RTEMS.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "rtems-rfs-rtems.h"

/*
 * The following sets the handlers based on the type of inode.
 */

bool
rtems_rfs_rtems_set_handlers (rtems_filesystem_location_info_t* loc,
                              rtems_rfs_inode_handle*           inode)
{
  uint16_t mode = rtems_rfs_inode_get_mode (inode);
  loc->handlers = NULL;
  if (RTEMS_RFS_S_ISDIR (mode))
    loc->handlers = rtems_rfs_rtems_handlers (dir);
  else if (RTEMS_RFS_S_ISCHR (mode) || RTEMS_RFS_S_ISBLK(mode))
    loc->handlers = rtems_rfs_rtems_handlers (device);
  else if (RTEMS_RFS_S_ISLNK (mode))
    loc->handlers = rtems_rfs_rtems_handlers (link);
  else if (RTEMS_RFS_S_ISREG (mode))
    loc->handlers = rtems_rfs_rtems_handlers (file);
  else
  {
    printf ("rtems-rfs: mode type unknown: %04x\n", mode);
    return false;
  }
  return true;
}

uint16_t
rtems_rfs_rtems_imode (mode_t mode)
{
  /*
   * Mapping matches RTEMS so no need to change.
   */
  return mode;
}

mode_t
rtems_rfs_rtems_mode (int imode)
{
  /*
   * Mapping matches RTEMS so no need to change.
   */
  return imode;
}

/*
 * Only provide if there is no macro version.
 */
#if !defined (rtems_rfs_rtems_error)
int
rtems_rfs_rtems_error (const char* mesg, int error)
{
  if (error)
    printf ("rtems-rfs: %s: %d: %s\n", mesg, error, strerror (error));
  errno = error;
  return error == 0 ? 0 : -1;
}
#endif

#if RTEMS_RFS_RTEMS_TRACE
static uint32_t rtems_rfs_rtems_trace_mask;

bool
rtems_rfs_rtems_trace (uint32_t mask)
{
  bool result = false;
  if (mask & rtems_rfs_rtems_trace_mask)
    result = true;
  return result;
}

void
rtems_rfs_trace_rtems_set_mask (uint32_t mask)
{
  rtems_rfs_rtems_trace_mask |= mask;
}

void
rtems_rfs_trace_rtems_clear_mask (uint32_t mask)
{
  rtems_rfs_rtems_trace_mask &= ~mask;
}

int
rtems_rfs_rtems_trace_shell_command (int argc, char *argv[])
{
  const char* table[] =
  {
    "error-msgs",
    "eval-path"
    "eval-for-make",
    "eval-perms",
    "mknod",
    "rmnod",
    "link",
    "unlink",
    "chown",
    "readlink",
    "fchmod",
    "stat",
    "dir-rmnod",
    "file-open",
    "file-close",
    "file-read",
    "file-write",
    "file-lseek",
    "file-ftrunc"
  };

  bool set = true;
  int  arg;
  int  t;

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      switch (argv[arg][1])
      {
        case 'h':
          printf ("usage: %s [-hl] [set/clear] [flags]\n", argv[0]);
          return 0;
        case 'l':
          printf ("%s: valid flags to set or clear are:\n", argv[0]);
          for (t = 0; t < (sizeof (table) / sizeof (const char*)); t++)
            printf ("  %s\n", table[t]);
          return 0;
        default:
          printf ("error: unknown option\n");
          return 1;
      }
    }
    else
    {
      uint32_t value = 0;
      if (strcmp (argv[arg], "set") == 0)
        set = true;
      if (strcmp (argv[arg], "clear") == 0)
        set = false;
      else if (strcmp (argv[arg], "all") == 0)
        value = RTEMS_RFS_RTEMS_DEBUG_ALL;
      else
      {
        for (t = 0; t < (sizeof (table) / sizeof (const char*)); t++)
        {
          if (strcmp (argv[arg], table[t]) == 0)
          {
            value = 1 << t;
            break;
          }
        }
      }

      if (set)
        rtems_rfs_rtems_trace_mask |= value;
      else
        rtems_rfs_rtems_trace_mask &= ~value;
    }
  }

  return 0;
}

#endif
