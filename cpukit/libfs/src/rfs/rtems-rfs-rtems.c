/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH.
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
 * RTEMS File System Interface for RTEMS.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <inttypes.h>
#include <stdlib.h>

#if SIZEOF_MODE_T == 8
#define PRIomode_t PRIo64
#elif SIZEOF_MODE_T == 4
#define PRIomode_t PRIo32
#else
#error "unsupport size of mode_t"
#endif

#include <rtems/rfs/rtems-rfs-file.h>
#include <rtems/rfs/rtems-rfs-dir.h>
#include <rtems/rfs/rtems-rfs-link.h>
#include "rtems-rfs-rtems.h"

static bool
rtems_rfs_rtems_eval_perms (rtems_filesystem_eval_path_context_t *ctx,
                            int eval_flags,
                            rtems_rfs_inode_handle* inode)
{
  return rtems_filesystem_eval_path_check_access(
    ctx,
    eval_flags,
    rtems_rfs_inode_get_mode (inode),
    rtems_rfs_inode_get_uid (inode),
    rtems_rfs_inode_get_gid (inode)
  );
}

static rtems_filesystem_node_types_t
rtems_rfs_rtems_node_type_by_inode (rtems_rfs_inode_handle* inode)
{
  /*
   * Do not return RTEMS_FILESYSTEM_HARD_LINK because this would result in an
   * eval link which does not make sense in the case of the RFS file
   * system. All directory entries are links to an inode. A link such as a HARD
   * link is actually the normal path to a regular file, directory, device
   * etc's inode. Links to inodes can be considered "the real" one, yet they
   * are all links.
   */
  uint16_t mode = rtems_rfs_inode_get_mode (inode);
  if (RTEMS_RFS_S_ISDIR (mode))
    return RTEMS_FILESYSTEM_DIRECTORY;
  else if (RTEMS_RFS_S_ISLNK (mode))
    return RTEMS_FILESYSTEM_SYM_LINK;
  else if (RTEMS_RFS_S_ISBLK (mode) || RTEMS_RFS_S_ISCHR (mode))
    return RTEMS_FILESYSTEM_DEVICE;
  else
    return RTEMS_FILESYSTEM_MEMORY_FILE;
}

static void
rtems_rfs_rtems_lock_by_mt_entry (
  const rtems_filesystem_mount_table_entry_t *mt_entry
)
{
  rtems_rfs_file_system* fs = mt_entry->fs_info;

  rtems_rfs_rtems_lock (fs);
}

static void
rtems_rfs_rtems_unlock_by_mt_entry (
  const rtems_filesystem_mount_table_entry_t *mt_entry
)
{
  rtems_rfs_file_system* fs = mt_entry->fs_info;

  rtems_rfs_rtems_unlock (fs);
}

static bool
rtems_rfs_rtems_is_directory(
  rtems_filesystem_eval_path_context_t *ctx,
  void *arg
)
{
  rtems_rfs_inode_handle* inode = arg;

  return rtems_rfs_rtems_node_type_by_inode (inode)
    == RTEMS_FILESYSTEM_DIRECTORY;
}

static void rtems_rfs_rtems_follow_link(
  rtems_filesystem_eval_path_context_t* ctx,
  rtems_rfs_file_system* fs,
  rtems_rfs_ino ino
)
{
  size_t len = MAXPATHLEN;
  char *link = malloc(len + 1);

  if (link != NULL) {
    int rc = rtems_rfs_symlink_read (fs, ino, link, len, &len);

    if (rc == 0) {
      rtems_filesystem_eval_path_recursive (ctx, link, len);
    } else {
      rtems_filesystem_eval_path_error (ctx, 0);
    }

    free(link);
  } else {
    rtems_filesystem_eval_path_error (ctx, ENOMEM);
  }
}

static rtems_filesystem_eval_path_generic_status
rtems_rfs_rtems_eval_token(
  rtems_filesystem_eval_path_context_t *ctx,
  void *arg,
  const char *token,
  size_t tokenlen
)
{
  rtems_filesystem_eval_path_generic_status status =
    RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_DONE;
  rtems_rfs_inode_handle* inode = arg;
  bool access_ok = rtems_rfs_rtems_eval_perms (ctx, RTEMS_FS_PERMS_EXEC, inode);

  if (access_ok) {
    if (rtems_filesystem_is_current_directory (token, tokenlen)) {
      rtems_filesystem_eval_path_clear_token (ctx);
    } else {
      rtems_filesystem_location_info_t *currentloc =
        rtems_filesystem_eval_path_get_currentloc( ctx );
      rtems_rfs_file_system* fs = rtems_rfs_rtems_pathloc_dev (currentloc);
      rtems_rfs_ino entry_ino;
      uint32_t entry_doff;
      int rc = rtems_rfs_dir_lookup_ino (
        fs,
        inode,
        token,
        tokenlen,
        &entry_ino,
        &entry_doff
      );

      if (rc == 0) {
        rc = rtems_rfs_inode_close (fs, inode);
        if (rc == 0) {
          rc = rtems_rfs_inode_open (fs, entry_ino, inode, true);
        }

        if (rc != 0) {
          /*
           * This prevents the rtems_rfs_inode_close() from doing something in
           * rtems_rfs_rtems_eval_path().
           */
          memset (inode, 0, sizeof(*inode));
        }
      } else {
        status = RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_NO_ENTRY;
        rc = -1;
      }

      if (rc == 0) {
        bool is_sym_link = rtems_rfs_rtems_node_type_by_inode (inode)
          == RTEMS_FILESYSTEM_SYM_LINK;
        int eval_flags = rtems_filesystem_eval_path_get_flags (ctx);
        bool follow_sym_link = (eval_flags & RTEMS_FS_FOLLOW_SYM_LINK) != 0;
        bool terminal = !rtems_filesystem_eval_path_has_path (ctx);

        rtems_filesystem_eval_path_clear_token (ctx);

        if (is_sym_link && (follow_sym_link || !terminal)) {
          rtems_rfs_rtems_follow_link (ctx, fs, entry_ino);
        } else {
          rc = rtems_rfs_rtems_set_handlers (currentloc, inode) ? 0 : EIO;
          if (rc == 0) {
            rtems_rfs_rtems_set_pathloc_ino (currentloc, entry_ino);
            rtems_rfs_rtems_set_pathloc_doff (currentloc, entry_doff);

            if (!terminal) {
              status = RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_CONTINUE;
            }
          } else {
            rtems_filesystem_eval_path_error (
              ctx,
              rtems_rfs_rtems_error ("eval_path: set handlers", rc)
            );
          }
        }
      }
    }
  }

  return status;
}

static const rtems_filesystem_eval_path_generic_config
rtems_rfs_rtems_eval_config = {
  .is_directory = rtems_rfs_rtems_is_directory,
  .eval_token = rtems_rfs_rtems_eval_token
};

static void
rtems_rfs_rtems_eval_path (rtems_filesystem_eval_path_context_t *ctx)
{
  rtems_filesystem_location_info_t *currentloc =
    rtems_filesystem_eval_path_get_currentloc (ctx);
  rtems_rfs_file_system* fs = rtems_rfs_rtems_pathloc_dev (currentloc);
  rtems_rfs_ino ino = rtems_rfs_rtems_get_pathloc_ino (currentloc);
  rtems_rfs_inode_handle inode;
  int rc;

  rc = rtems_rfs_inode_open (fs, ino, &inode, true);
  if (rc == 0) {
    rtems_filesystem_eval_path_generic (
      ctx,
      &inode,
      &rtems_rfs_rtems_eval_config
    );
    rc = rtems_rfs_inode_close (fs, &inode);
    if (rc != 0) {
      rtems_filesystem_eval_path_error (
        ctx,
        rtems_rfs_rtems_error ("eval_path: closing inode", rc)
      );
    }
  } else {
    rtems_filesystem_eval_path_error (
      ctx,
      rtems_rfs_rtems_error ("eval_path: opening inode", rc)
    );
  }
}

/**
 * The following rouine creates a new link node under parent with the name
 * given in name. The link node is set to point to the node at targetloc.
 */
static int
rtems_rfs_rtems_link (const rtems_filesystem_location_info_t *parentloc,
                      const rtems_filesystem_location_info_t *targetloc,
                      const char *name,
                      size_t namelen)
{
  rtems_rfs_file_system* fs = rtems_rfs_rtems_pathloc_dev (targetloc);
  rtems_rfs_ino          target = rtems_rfs_rtems_get_pathloc_ino (targetloc);
  rtems_rfs_ino          parent = rtems_rfs_rtems_get_pathloc_ino (parentloc);
  int                    rc;

  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_LINK))
    printf ("rtems-rfs-rtems: link: in: parent:%" PRId32 " target:%" PRId32 "\n",
            parent, target);

  rc = rtems_rfs_link (fs, name, namelen, parent, target, false);
  if (rc)
  {
    return rtems_rfs_rtems_error ("link: linking", rc);
	}


	return 0;
}

/**
 * The following verifies that and returns the type of node that the loc refers
 * to.
 *
 * @param pathloc
 * @return rtems_filesystem_node_types_t
 */

static rtems_filesystem_node_types_t
rtems_rfs_rtems_node_type (const rtems_filesystem_location_info_t* pathloc)
{
  rtems_rfs_file_system*        fs = rtems_rfs_rtems_pathloc_dev (pathloc);
  rtems_rfs_ino                 ino = rtems_rfs_rtems_get_pathloc_ino (pathloc);
  rtems_filesystem_node_types_t type;
  rtems_rfs_inode_handle        inode;
  int                           rc;

  rc = rtems_rfs_inode_open (fs, ino, &inode, true);
  if (rc > 0)
  {
    return rtems_rfs_rtems_error ("node_type: opening inode", rc);
  }

  type = rtems_rfs_rtems_node_type_by_inode (&inode);

  rc = rtems_rfs_inode_close (fs, &inode);
  if (rc > 0)
  {
    return rtems_rfs_rtems_error ("node_type: closing inode", rc);
  }

  return type;
}

/**
 * This routine is the implementation of the chown() system call for the
 * RFS.
 *
 * @param pathloc
 * @param owner
 * @param group
 * return int
 */

static int
rtems_rfs_rtems_chown (const rtems_filesystem_location_info_t *pathloc,
                       uid_t                                   owner,
                       gid_t                                   group)
{
  rtems_rfs_file_system* fs = rtems_rfs_rtems_pathloc_dev (pathloc);
  rtems_rfs_ino          ino = rtems_rfs_rtems_get_pathloc_ino (pathloc);
  rtems_rfs_inode_handle inode;
#if defined (RTEMS_POSIX_API)
  uid_t                  uid;
#endif
  int                    rc;

  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_CHOWN))
    printf ("rtems-rfs-rtems: chown: in: ino:%" PRId32 " uid:%d gid:%d\n",
            ino, owner, group);

  rc = rtems_rfs_inode_open (fs, ino, &inode, true);
  if (rc > 0)
  {
    return rtems_rfs_rtems_error ("chown: opening inode", rc);
  }

  /*
   *  Verify I am the owner of the node or the super user.
   */

#if defined (RTEMS_POSIX_API)
  uid = geteuid();

  if ((uid != rtems_rfs_inode_get_uid (&inode)) && (uid != 0))
  {
    rtems_rfs_inode_close (fs, &inode);
    return rtems_rfs_rtems_error ("chown: not able", EPERM);
  }
#endif

  rtems_rfs_inode_set_uid_gid (&inode, owner, group);

  rc = rtems_rfs_inode_close (fs, &inode);
  if (rc)
  {
    return rtems_rfs_rtems_error ("chown: closing inode", rc);
  }

  return 0;
}

/**
 * This routine is the implementation of the utime() system call for the
 * RFS.
 *
 * @param pathloc
 * @param atime
 * @param mtime
 * return int
 */

static int
rtems_rfs_rtems_utime(const rtems_filesystem_location_info_t* pathloc,
                      time_t                                  atime,
                      time_t                                  mtime)
{
  rtems_rfs_file_system* fs = rtems_rfs_rtems_pathloc_dev (pathloc);
  rtems_rfs_ino          ino = rtems_rfs_rtems_get_pathloc_ino (pathloc);
  rtems_rfs_inode_handle inode;
  int                    rc;

  rc = rtems_rfs_inode_open (fs, ino, &inode, true);
  if (rc)
  {
    return rtems_rfs_rtems_error ("utime: read inode", rc);
  }

  rtems_rfs_inode_set_atime (&inode, atime);
  rtems_rfs_inode_set_mtime (&inode, mtime);

  rc = rtems_rfs_inode_close (fs, &inode);
  if (rc)
  {
    return rtems_rfs_rtems_error ("utime: closing inode", rc);
  }

  return 0;
}

/**
 * The following routine creates a new symbolic link node under parent with the
 * name given in node_name.
 */

static int
rtems_rfs_rtems_symlink (const rtems_filesystem_location_info_t* parent_loc,
                         const char*                             node_name,
                         size_t                                  node_name_len,
                         const char*                             target)
{
  rtems_rfs_file_system* fs = rtems_rfs_rtems_pathloc_dev (parent_loc);
  rtems_rfs_ino          parent = rtems_rfs_rtems_get_pathloc_ino (parent_loc);
  int                    rc;

  rc = rtems_rfs_symlink (fs, node_name, node_name_len,
                          target, strlen (target),
                          geteuid(), getegid(), parent);
  if (rc)
  {
    return rtems_rfs_rtems_error ("symlink: linking", rc);
  }

  return 0;
}

/**
 * The following rouine puts the symblic links destination name into buf.
 */

static ssize_t
rtems_rfs_rtems_readlink (const rtems_filesystem_location_info_t* pathloc,
                          char*                                   buf,
                          size_t                                  bufsize)
{
  rtems_rfs_file_system*  fs = rtems_rfs_rtems_pathloc_dev (pathloc);
  rtems_rfs_ino           ino = rtems_rfs_rtems_get_pathloc_ino (pathloc);
  size_t                  length;
  int                     rc;

  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_READLINK))
    printf ("rtems-rfs-rtems: readlink: in: ino:%" PRId32 "\n", ino);

  rc = rtems_rfs_symlink_read (fs, ino, buf, bufsize, &length);
  if (rc)
  {
    return rtems_rfs_rtems_error ("readlink: reading link", rc);
  }

  return (ssize_t) length;
}

static int
rtems_rfs_rtems_fchmod (const rtems_filesystem_location_info_t* pathloc,
                        mode_t                                  mode)
{
  rtems_rfs_file_system*  fs = rtems_rfs_rtems_pathloc_dev (pathloc);
  rtems_rfs_ino           ino = rtems_rfs_rtems_get_pathloc_ino (pathloc);
  rtems_rfs_inode_handle  inode;
  uint16_t                imode;
#if defined (RTEMS_POSIX_API)
  uid_t                   uid;
#endif
  int                     rc;

  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_FCHMOD))
    printf ("rtems-rfs-rtems: fchmod: in: ino:%" PRId32 " mode:%06" PRIomode_t "\n",
            ino, mode);

  rc = rtems_rfs_inode_open (fs, ino, &inode, true);
  if (rc)
  {
    return rtems_rfs_rtems_error ("fchmod: opening inode", rc);
  }

  imode = rtems_rfs_inode_get_mode (&inode);

  /*
   *  Verify I am the owner of the node or the super user.
   */
#if defined (RTEMS_POSIX_API)
  uid = geteuid();

  if ((uid != rtems_rfs_inode_get_uid (&inode)) && (uid != 0))
  {
    rtems_rfs_inode_close (fs, &inode);
    return rtems_rfs_rtems_error ("fchmod: checking uid", EPERM);
  }
#endif

  imode &= ~(S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX);
  imode |= mode & (S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX);

  rtems_rfs_inode_set_mode (&inode, imode);

  rc = rtems_rfs_inode_close (fs, &inode);
  if (rc > 0)
  {
    return rtems_rfs_rtems_error ("fchmod: closing inode", rc);
  }

  return 0;
}

int
rtems_rfs_rtems_fstat (const rtems_filesystem_location_info_t* pathloc,
                       struct stat*                            buf)
{
  rtems_rfs_file_system* fs = rtems_rfs_rtems_pathloc_dev (pathloc);
  rtems_rfs_ino          ino = rtems_rfs_rtems_get_pathloc_ino (pathloc);
  rtems_rfs_inode_handle inode;
  rtems_rfs_file_shared* shared;
  uint16_t               mode;
  int                    rc;

  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_STAT))
    printf ("rtems-rfs-rtems: stat: in: ino:%" PRId32 "\n", ino);

  rc = rtems_rfs_inode_open (fs, ino, &inode, true);
  if (rc)
  {
    return rtems_rfs_rtems_error ("stat: opening inode", rc);
  }

  mode = rtems_rfs_inode_get_mode (&inode);

  if (RTEMS_RFS_S_ISCHR (mode) || RTEMS_RFS_S_ISBLK (mode))
  {
    buf->st_rdev =
      rtems_filesystem_make_dev_t (rtems_rfs_inode_get_block (&inode, 0),
                                   rtems_rfs_inode_get_block (&inode, 1));
  }

  buf->st_dev     = rtems_rfs_fs_device (fs);
  buf->st_ino     = rtems_rfs_inode_ino (&inode);
  buf->st_mode    = rtems_rfs_rtems_mode (mode);
  buf->st_nlink   = rtems_rfs_inode_get_links (&inode);
  buf->st_uid     = rtems_rfs_inode_get_uid (&inode);
  buf->st_gid     = rtems_rfs_inode_get_gid (&inode);

  /*
   * Need to check is the ino is an open file. If so we take the values from
   * the open file rather than the inode.
   */
  shared = rtems_rfs_file_get_shared (fs, rtems_rfs_inode_ino (&inode));

  if (shared)
  {
    buf->st_atime   = rtems_rfs_file_shared_get_atime (shared);
    buf->st_mtime   = rtems_rfs_file_shared_get_mtime (shared);
    buf->st_ctime   = rtems_rfs_file_shared_get_ctime (shared);
    buf->st_blocks  = rtems_rfs_file_shared_get_block_count (shared);

    if (S_ISLNK (buf->st_mode))
      buf->st_size = rtems_rfs_file_shared_get_block_offset (shared);
    else
      buf->st_size = rtems_rfs_file_shared_get_size (fs, shared);
  }
  else
  {
    buf->st_atime   = rtems_rfs_inode_get_atime (&inode);
    buf->st_mtime   = rtems_rfs_inode_get_mtime (&inode);
    buf->st_ctime   = rtems_rfs_inode_get_ctime (&inode);
    buf->st_blocks  = rtems_rfs_inode_get_block_count (&inode);

    if (S_ISLNK (buf->st_mode))
      buf->st_size = rtems_rfs_inode_get_block_offset (&inode);
    else
      buf->st_size = rtems_rfs_inode_get_size (fs, &inode);
  }

  buf->st_blksize = rtems_rfs_fs_block_size (fs);

  rc = rtems_rfs_inode_close (fs, &inode);
  if (rc > 0)
  {
    return rtems_rfs_rtems_error ("stat: closing inode", rc);
  }

  return 0;
}

/**
 * Routine to create a node in the RFS file system.
 */

static int
rtems_rfs_rtems_mknod (const rtems_filesystem_location_info_t *parentloc,
                       const char                             *name,
                       size_t                                  namelen,
                       mode_t                                  mode,
                       dev_t                                   dev)
{
  rtems_rfs_file_system*  fs = rtems_rfs_rtems_pathloc_dev (parentloc);
  rtems_rfs_ino           parent = rtems_rfs_rtems_get_pathloc_ino (parentloc);
  rtems_rfs_ino           ino;
  rtems_rfs_inode_handle  inode;
  uid_t                   uid;
  gid_t                   gid;
  int                     rc;

#if defined(RTEMS_POSIX_API)
  uid = geteuid ();
  gid = getegid ();
#else
  uid = 0;
  gid = 0;
#endif

  rc = rtems_rfs_inode_create (fs, parent, name, namelen,
                               rtems_rfs_rtems_imode (mode),
                               1, uid, gid, &ino);
  if (rc > 0)
  {
    return rtems_rfs_rtems_error ("mknod: inode create", rc);
  }

  rc = rtems_rfs_inode_open (fs, ino, &inode, true);
  if (rc > 0)
  {
    return rtems_rfs_rtems_error ("mknod: inode open", rc);
  }

  if (S_ISDIR(mode) || S_ISREG(mode))
  {
  }
  else if (S_ISCHR (mode) || S_ISBLK (mode))
  {
    int major;
    int minor;
    rtems_filesystem_split_dev_t (dev, major, minor);
    rtems_rfs_inode_set_block (&inode, 0, major);
    rtems_rfs_inode_set_block (&inode, 1, minor);
  }
  else
  {
    rtems_rfs_inode_close (fs, &inode);
    return rtems_rfs_rtems_error ("mknod: bad mode", EINVAL);
  }

  rc = rtems_rfs_inode_close (fs, &inode);
  if (rc > 0)
  {
    return rtems_rfs_rtems_error ("mknod: closing inode", rc);
  }

  return 0;
}

/**
 * Routine to remove a node from the RFS file system.
 *
 * @param parent_pathloc
 * @param pathloc
 * @return int
 */
int
rtems_rfs_rtems_rmnod (const rtems_filesystem_location_info_t* parent_pathloc,
                       const rtems_filesystem_location_info_t* pathloc)
{
  rtems_rfs_file_system* fs = rtems_rfs_rtems_pathloc_dev (pathloc);
  rtems_rfs_ino          parent = rtems_rfs_rtems_get_pathloc_ino (parent_pathloc);
  rtems_rfs_ino          ino = rtems_rfs_rtems_get_pathloc_ino (pathloc);
  uint32_t               doff = rtems_rfs_rtems_get_pathloc_doff (pathloc);
  int                    rc;

  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_RMNOD))
    printf ("rtems-rfs: rmnod: parent:%" PRId32 " doff:%" PRIu32 ", ino:%" PRId32 "\n",
            parent, doff, ino);

  if (ino == RTEMS_RFS_ROOT_INO)
    return rtems_rfs_rtems_error ("rmnod: root inode", EBUSY);

  rc = rtems_rfs_unlink (fs, parent, ino, doff, rtems_rfs_unlink_dir_if_empty);
  if (rc)
  {
    return rtems_rfs_rtems_error ("rmnod: unlinking", rc);
  }

  return 0;
}

/**
 * The following routine does a sync on an inode node. Currently it flushes
 * everything related to this device.
 *
 * @param iop
 * @return int
 */
int
rtems_rfs_rtems_fdatasync (rtems_libio_t* iop)
{
  int rc;

  rc = rtems_rfs_buffer_sync (rtems_rfs_rtems_pathloc_dev (&iop->pathinfo));
  if (rc)
    return rtems_rfs_rtems_error ("fdatasync: sync", rc);

  return 0;
}

/**
 * Rename the node.
 */
static int
rtems_rfs_rtems_rename(const rtems_filesystem_location_info_t* old_parent_loc,
                       const rtems_filesystem_location_info_t* old_loc,
                       const rtems_filesystem_location_info_t* new_parent_loc,
                       const char*                             new_name,
                       size_t                                  new_name_len)
{
  rtems_rfs_file_system*  fs = rtems_rfs_rtems_pathloc_dev (old_loc);
  rtems_rfs_ino           old_parent;
  rtems_rfs_ino           new_parent;
  rtems_rfs_ino           ino;
  uint32_t                doff;
  int                     rc;

  old_parent = rtems_rfs_rtems_get_pathloc_ino (old_parent_loc);
  new_parent = rtems_rfs_rtems_get_pathloc_ino (new_parent_loc);

  ino  = rtems_rfs_rtems_get_pathloc_ino (old_loc);
  doff = rtems_rfs_rtems_get_pathloc_doff (old_loc);

  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_RENAME))
    printf ("rtems-rfs: rename: ino:%" PRId32 " doff:%" PRIu32 ", new parent:%" PRId32 "\n",
            ino, doff, new_parent);

  /*
   * Link to the inode before unlinking so the inode is not erased when
   * unlinked.
   */
  rc = rtems_rfs_link (fs, new_name, new_name_len, new_parent, ino, true);
  if (rc)
  {
    return rtems_rfs_rtems_error ("rename: linking", rc);
  }

  /*
   * Unlink all inodes even directories with the dir option as false because a
   * directory may not be empty.
   */
  rc = rtems_rfs_unlink (fs, old_parent, ino, doff,
                         rtems_rfs_unlink_dir_allowed);
  if (rc)
  {
    return rtems_rfs_rtems_error ("rename: unlinking", rc);
  }

  return 0;
}

/**
 * Return the file system stat data.
 *
 * @param pathloc
 * @param sb
 * @return int
 */
static int
rtems_rfs_rtems_statvfs (const rtems_filesystem_location_info_t* pathloc,
                         struct statvfs*                         sb)
{
  rtems_rfs_file_system* fs = rtems_rfs_rtems_pathloc_dev (pathloc);
  size_t                 blocks;
  size_t                 inodes;

  rtems_rfs_group_usage (fs, &blocks, &inodes);

  sb->f_bsize   = rtems_rfs_fs_block_size (fs);
  sb->f_frsize  = rtems_rfs_fs_media_block_size (fs);
  sb->f_blocks  = rtems_rfs_fs_media_blocks (fs);
  sb->f_bfree   = rtems_rfs_fs_blocks (fs) - blocks;
  sb->f_bavail  = sb->f_bfree;
  sb->f_files   = rtems_rfs_fs_inodes (fs);
  sb->f_ffree   = rtems_rfs_fs_inodes (fs) - inodes;
  sb->f_favail  = sb->f_ffree;
  sb->f_fsid    = RTEMS_RFS_SB_MAGIC;
  sb->f_flag    = rtems_rfs_fs_flags (fs);
  sb->f_namemax = rtems_rfs_fs_max_name (fs);

  return 0;
}

/**
 *  Handler table for RFS link nodes
 */
const rtems_filesystem_file_handlers_r rtems_rfs_rtems_link_handlers =
{
  .open_h      = rtems_filesystem_default_open,
  .close_h     = rtems_filesystem_default_close,
  .read_h      = rtems_filesystem_default_read,
  .write_h     = rtems_filesystem_default_write,
  .ioctl_h     = rtems_filesystem_default_ioctl,
  .lseek_h     = rtems_filesystem_default_lseek,
  .fstat_h     = rtems_rfs_rtems_fstat,
  .ftruncate_h = rtems_filesystem_default_ftruncate,
  .fsync_h     = rtems_filesystem_default_fsync_or_fdatasync,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fcntl_h     = rtems_filesystem_default_fcntl
};

/**
 * Forward decl for the ops table.
 */

int rtems_rfs_rtems_initialise (rtems_filesystem_mount_table_entry_t *mt_entry,
                                const void                           *data);
void rtems_rfs_rtems_shutdown (rtems_filesystem_mount_table_entry_t *mt_entry);

/**
 * RFS file system operations table.
 */
const rtems_filesystem_operations_table rtems_rfs_ops =
{
  .lock_h         = rtems_rfs_rtems_lock_by_mt_entry,
  .unlock_h       = rtems_rfs_rtems_unlock_by_mt_entry,
  .are_nodes_equal_h = rtems_filesystem_default_are_nodes_equal,
  .eval_path_h    = rtems_rfs_rtems_eval_path,
  .link_h         = rtems_rfs_rtems_link,
  .node_type_h    = rtems_rfs_rtems_node_type,
  .fchmod_h       = rtems_rfs_rtems_fchmod,
  .mknod_h        = rtems_rfs_rtems_mknod,
  .rmnod_h        = rtems_rfs_rtems_rmnod,
  .chown_h        = rtems_rfs_rtems_chown,
  .clonenod_h     = rtems_filesystem_default_clonenode,
  .freenod_h      = rtems_filesystem_default_freenode,
  .mount_h        = rtems_filesystem_default_mount,
  .fsmount_me_h   = rtems_rfs_rtems_initialise,
  .unmount_h      = rtems_filesystem_default_unmount,
  .fsunmount_me_h = rtems_rfs_rtems_shutdown,
  .utime_h        = rtems_rfs_rtems_utime,
  .symlink_h      = rtems_rfs_rtems_symlink,
  .readlink_h     = rtems_rfs_rtems_readlink,
  .rename_h       = rtems_rfs_rtems_rename,
  .statvfs_h      = rtems_rfs_rtems_statvfs
};

/**
 * Open the file system.
 */

int
rtems_rfs_rtems_initialise (rtems_filesystem_mount_table_entry_t* mt_entry,
                            const void*                           data)
{
  rtems_rfs_rtems_private* rtems;
  rtems_rfs_file_system*   fs;
  uint32_t                 flags = 0;
  uint32_t                 max_held_buffers = RTEMS_RFS_FS_MAX_HELD_BUFFERS;
  const char*              options = data;
  int                      rc;

  /*
   * Parse the options the user specifiies.
   */
  while (options)
  {
    printf ("options=%s\n", options);
    if (strncmp (options, "hold-bitmaps",
                 sizeof ("hold-bitmaps") - 1) == 0)
      flags |= RTEMS_RFS_FS_BITMAPS_HOLD;
    else if (strncmp (options, "no-local-cache",
                      sizeof ("no-local-cache") - 1) == 0)
      flags |= RTEMS_RFS_FS_NO_LOCAL_CACHE;
    else if (strncmp (options, "max-held-bufs",
                      sizeof ("max-held-bufs") - 1) == 0)
    {
      max_held_buffers = strtoul (options + sizeof ("max-held-bufs"), 0, 0);
    }
    else
      return rtems_rfs_rtems_error ("initialise: invalid option", EINVAL);

    options = strchr (options, ',');
    if (options)
    {
      ++options;
      if (*options == '\0')
        options = NULL;
    }
  }

  rtems = malloc (sizeof (rtems_rfs_rtems_private));
  if (!rtems)
    return rtems_rfs_rtems_error ("initialise: local data", ENOMEM);

  memset (rtems, 0, sizeof (rtems_rfs_rtems_private));

  rc = rtems_rfs_mutex_create (&rtems->access);
  if (rc > 0)
  {
    free (rtems);
    return rtems_rfs_rtems_error ("initialise: cannot create mutex", rc);
  }

  rc = rtems_rfs_mutex_lock (&rtems->access);
  if (rc > 0)
  {
    rtems_rfs_mutex_destroy (&rtems->access);
    free (rtems);
    return rtems_rfs_rtems_error ("initialise: cannot lock access  mutex", rc);
  }

  rc = rtems_rfs_fs_open (mt_entry->dev, rtems, flags, max_held_buffers, &fs);
  if (rc)
  {
    free (rtems);
    return rtems_rfs_rtems_error ("initialise: open", rc);
  }

  mt_entry->fs_info                          = fs;
  mt_entry->ops                              = &rtems_rfs_ops;
  mt_entry->mt_fs_root->location.node_access = (void*) RTEMS_RFS_ROOT_INO;
  mt_entry->mt_fs_root->location.handlers    = &rtems_rfs_rtems_dir_handlers;

  rtems_rfs_rtems_unlock (fs);

  return 0;
}

/**
 * Shutdown the file system.
 */
void
rtems_rfs_rtems_shutdown (rtems_filesystem_mount_table_entry_t* mt_entry)
{
  rtems_rfs_file_system*   fs = mt_entry->fs_info;
  rtems_rfs_rtems_private* rtems;

  rtems = rtems_rfs_fs_user (fs);

  /* FIXME: Return value? */
  rtems_rfs_fs_close(fs);

  rtems_rfs_mutex_destroy (&rtems->access);
  free (rtems);
}
