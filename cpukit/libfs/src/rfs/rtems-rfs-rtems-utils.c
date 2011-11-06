/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
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

bool
rtems_rfs_rtems_eval_perms (rtems_rfs_inode_handle* inode, int flags)
{
  uid_t    st_uid;
  gid_t    st_gid;
  uint16_t uid;
  uint16_t gid;
  uint16_t mode;
  int      flags_to_test;

  uid = rtems_rfs_inode_get_uid (inode);
  gid = rtems_rfs_inode_get_gid (inode);
  mode = rtems_rfs_inode_get_mode (inode);

#if defined (RTEMS_POSIX_API)
  st_uid = geteuid ();
  st_gid = getegid ();
#else
  st_uid = uid;
  st_gid = gid;
#endif

  /*
   * Check if I am owner or a group member or someone else.
   */
  flags_to_test = flags;

  if ((st_uid == 0) || (st_uid == uid))
    flags_to_test |= flags << 6;
  if ((st_uid == 0) || (st_gid == gid))
    flags_to_test |= flags << 3;
  else
    /* must be other - already set above */;

  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_EVAL_PERMS))
    printf ("rtems-rfs: eval-perms: uid=%d gid=%d iuid=%d igid=%d " \
            "flags=%o flags_to_test=%o mode=%o (%o)\n",
            st_uid, st_gid, uid, gid,
            flags, flags_to_test, mode & 0777,
            flags_to_test & (mode & 0777));

  /*
   * If all of the flags are set we have permission
   * to do this.
   */
  if ((flags_to_test & (mode & 0777)) != 0)
    return true;

  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_EVAL_PERMS))
    printf("rtems-rfs: eval-perms: perms failed\n");

  return false;
}

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
