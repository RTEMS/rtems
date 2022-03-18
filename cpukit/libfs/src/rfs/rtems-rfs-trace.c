/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtems_rfs
 *
 * @brief RTEMS File Systems Trace Support
 */
/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
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
