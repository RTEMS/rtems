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
 * RTEMS File Systems Trace Support
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <rtems/rfs/rtems-rfs-trace.h>

#if RTEMS_RFS_TRACE
static rtems_rfs_trace_mask rtems_rfs_trace_flags;

bool
rtems_rfs_trace (rtems_rfs_trace_mask mask)
{
  bool result = false;
  if (mask & rtems_rfs_trace_flags)
    result = true;
  return result;
}

rtems_rfs_trace_mask
rtems_rfs_trace_set_mask (rtems_rfs_trace_mask mask)
{
  rtems_rfs_trace_mask state = rtems_rfs_trace_flags;
  rtems_rfs_trace_flags |= mask;
  return state;
}

rtems_rfs_trace_mask
rtems_rfs_trace_clear_mask (rtems_rfs_trace_mask mask)
{
  rtems_rfs_trace_mask state = rtems_rfs_trace_flags;
  rtems_rfs_trace_flags &= ~mask;
  return state;
}

int
rtems_rfs_trace_shell_command (int argc, char *argv[])
{
  const char* table[] =
  {
    "open",
    "close",
    "mutex",
    "buffer-open",
    "buffer-close",
    "buffer-sync",
    "buffer-release",
    "buffer-chains",
    "buffer-handle-request",
    "buffer-handle-release",
    "buffer-setblksize",
    "buffers-release",
    "block-find",
    "block-map-grow",
    "block-map-shrink",
    "group-open",
    "group-close",
    "group-bitmaps",
    "inode-open",
    "inode-close",
    "inode-load",
    "inode-unload",
    "inode-create",
    "inode-delete",
    "link",
    "unlink",
    "dir-lookup-ino",
    "dir-lookup-ino-check",
    "dir-lookup-ino-found",
    "dir-add-entry",
    "dir-del-entry",
    "dir-read",
    "dir-empty",
    "symlink",
    "symlink-read",
    "file-open",
    "file-close",
    "file-io",
    "file-set"
  };

  rtems_rfs_trace_mask set_value = 0;
  rtems_rfs_trace_mask clear_value = 0;
  bool                 set = true;
  int                  arg;
  int                  t;

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
      if (strcmp (argv[arg], "set") == 0)
        set = true;
      if (strcmp (argv[arg], "clear") == 0)
        set = false;
      else if (strcmp (argv[arg], "all") == 0)
      {
        if (set)
          set_value = RTEMS_RFS_TRACE_ALL;
        else
          clear_value = RTEMS_RFS_TRACE_ALL;
      }
      else
      {
        for (t = 0; t < (sizeof (table) / sizeof (const char*)); t++)
        {
          if (strcmp (argv[arg], table[t]) == 0)
          {
            if (set)
              set_value = 1ULL << t;
            else
              clear_value = 1ULL << t;
            break;
          }
        }
      }

      rtems_rfs_trace_flags |= set_value;
      rtems_rfs_trace_flags &= ~clear_value;
    }
  }

  return 0;
}

#endif
