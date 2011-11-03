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

/**
 * The libio permissions for read/execute.
 */
#define RTEMS_LIBIO_PERMS_RX (RTEMS_LIBIO_PERMS_SEARCH | RTEMS_LIBIO_PERMS_READ)
/**
 * The libio permissions for write/execute.
 */
#define RTEMS_LIBIO_PERMS_WX (RTEMS_LIBIO_PERMS_SEARCH | RTEMS_LIBIO_PERMS_WRITE)

/**
 * Evaluate the path to a node that wishes to be accessed. The pathloc is
 * returned with the ino to the node to be accessed.
 *
 * The routine starts from the root stripping away any leading path separators
 * breaking the path up into the node names and checking an inode exists for
 * that node name. Permissions are checked to insure access to the node is
 * allowed. A path to a node must be accessable all the way even if the end
 * result is directly accessable. As a user on Linux try "ls /root/../tmp" and
 * you will see if fails.
 *
 * The whole process is complicated by crossmount paths where we head down into
 * this file system only to return to the top and out to a another mounted file
 * system. For example we are mounted on '/e' and the user enters "ls
 * /e/a/b/../../dev". We need to head down then back up.
 *
 * @param path
 * @param pathlen
 * @param flags
 * @param pathloc
 */
int
rtems_rfs_rtems_eval_path (const char*                       path,
                           size_t                            pathlen,
                           int                               flags,
                           rtems_filesystem_location_info_t* pathloc)
{
  rtems_rfs_file_system* fs = rtems_rfs_rtems_pathloc_dev (pathloc);
  rtems_rfs_inode_handle inode;
  rtems_rfs_ino          ino = rtems_rfs_rtems_get_pathloc_ino (pathloc);
  uint32_t               doff = 0;
  const char*            node;
  size_t                 node_len;
  int                    stripped;
  int                    rc;

  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_EVAL_PATH))
    printf ("rtems-rfs-rtems: eval-path: in: path:%s pathlen:%zi ino:%" PRId32 "\n",
            path, pathlen, ino);
  
  /*
   * Eat any separators at the start of the path.
   */
  stripped = rtems_filesystem_prefix_separators (path, pathlen);
  path += stripped;
  pathlen -= stripped;

  rtems_rfs_rtems_lock (fs);
  
  while (true)
  {
    /*
     * Open and load the inode.
     */
    rc = rtems_rfs_inode_open (fs, ino, &inode, true);
    if (rc > 0)
    {
      rtems_rfs_rtems_unlock (fs);
      return rtems_rfs_rtems_error ("eval_path: opening inode", rc);
    }
    
    /*
     * Is this the end of the pathname we where given ?
     */
    if ((*path == '\0') || (pathlen == 0))
      break;

    /*
     * If a directory the execute bit must be set for us to enter.
     */
    if (RTEMS_RFS_S_ISDIR (rtems_rfs_inode_get_mode (&inode)) &&
        !rtems_rfs_rtems_eval_perms (&inode, RTEMS_LIBIO_PERMS_SEARCH))
    {
      rtems_rfs_inode_close (fs, &inode);
      rtems_rfs_rtems_unlock (fs);
      return rtems_rfs_rtems_error ("eval_path: eval perms", EACCES);
    }
    
    /*
     * Extract the node name we will look for this time around.
     */
    node = path;
    node_len = 0;
    while (!rtems_filesystem_is_separator (*path) &&
           (*path != '\0') && pathlen &&
           ((node_len + 1) < rtems_rfs_fs_max_name (fs)))
    {
      path++;
      pathlen--;
      node_len++;
    }

    /*
     * Eat any separators at start of the path.
     */
    stripped = rtems_filesystem_prefix_separators (path, pathlen);
    path += stripped;
    pathlen -= stripped;
    node_len += stripped;

    /*
     * If the node is the current directory and there is more path to come move
     * on it else we are at the inode we want.
     */
    if (rtems_rfs_current_dir (node))
    {
      if (*path)
      {
        rtems_rfs_inode_close (fs, &inode);
        continue;
      }
      break;
    }

    /*
     * If the node is a parent we must move up one directory. If the location
     * is on another file system we have a crossmount so we call that file
     * system to handle the remainder of the path.
     */
    if (rtems_rfs_parent_dir (node))
    {
      /*
       * If we are at root inode of the file system we have a crossmount path.
       */
      if (ino == RTEMS_RFS_ROOT_INO)
      {
        if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_EVAL_PATH))
          printf("rtems-rfs-rtems: eval-path: crossmount: path:%s (%zd)\n",
                 path - node_len, pathlen + node_len);
        rtems_rfs_inode_close (fs, &inode);
        rtems_rfs_rtems_unlock (fs);
        *pathloc = pathloc->mt_entry->mt_point_node;
        return (*pathloc->ops->evalpath_h)(path - node_len, pathlen + node_len,
                                           flags, pathloc);
      }

      /*
       * We need to find the parent of this node.
       */
      rc = rtems_rfs_dir_lookup_ino (fs, &inode, "..", 2, &ino, &doff);
      if (rc > 0)
      {
        rtems_rfs_inode_close (fs, &inode);
        rtems_rfs_rtems_unlock (fs);
        return rtems_rfs_rtems_error ("eval_path: read parent inode", rc);
      }
      if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_EVAL_PATH))
        printf("rtems-rfs-rtems: eval-path: parent: ino:%" PRId32 "\n", ino);
    }
    else
    {
      /*
       * Look up the node name in this directory. If found drop through, close
       * the current inode and let the loop open the inode so the mode can be
       * read and handlers set.
       */
      rc = rtems_rfs_dir_lookup_ino (fs, &inode,
                                     node, node_len - stripped, &ino, &doff);
      if (rc > 0)
      {
        rtems_rfs_inode_close (fs, &inode);
        rtems_rfs_rtems_unlock (fs);
        return ((errno = rc) == 0) ? 0 : -1;
      }    
      if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_EVAL_PATH))
        printf("rtems-rfs-rtems: eval-path: down: path:%s ino:%" PRId32 "\n", node, ino);
    }

    rc = rtems_rfs_inode_close (fs, &inode);
    if (rc > 0)
    {
      rtems_rfs_inode_close (fs, &inode);
      rtems_rfs_rtems_unlock (fs);
      return rtems_rfs_rtems_error ("eval_path: closing node", rc);
    }    
  }
  
  rtems_rfs_rtems_set_pathloc_ino (pathloc, ino);
  rtems_rfs_rtems_set_pathloc_doff (pathloc, doff);

  rc = rtems_rfs_rtems_set_handlers (pathloc, &inode) ? 0 : EIO;
  
  rtems_rfs_inode_close (fs, &inode);
  rtems_rfs_rtems_unlock (fs);

  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_EVAL_PATH))
    printf("rtems-rfs-rtems: eval-path: ino:%" PRId32 "\n", ino);

  return rc;
}

/**
 * The following routine evaluates a path for a new node to be created. The
 * pathloc is returned with a pointer to the parent of the new node. The name
 * is returned with a pointer to the first character in the new node name. The
 * parent node is verified to be a directory.
 *
 * @param path
 * @param pathloc
 * @param name
 * @return int
 */
int
rtems_rfs_rtems_eval_for_make (const char*                       path,
                               rtems_filesystem_location_info_t* pathloc,
                               const char**                      name)
{
  rtems_rfs_file_system* fs = rtems_rfs_rtems_pathloc_dev (pathloc);
  rtems_rfs_inode_handle inode;
  rtems_rfs_ino          ino = rtems_rfs_rtems_get_pathloc_ino (pathloc);
  rtems_rfs_ino          node_ino;
  uint32_t               doff = 0;
  const char*            node;
  int                    node_len;
  int                    stripped;
  int                    rc;

  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_EVAL_FOR_MAKE))
    printf ("rtems-rfs-rtems: eval-for-make: path:%s ino:%" PRId32 "\n", path, ino);
  
  *name = path + strlen (path);

  while (*name != path)
  {
    (*name)--;
    if (rtems_filesystem_is_separator (**name))
    {
      (*name)++;
      break;
    }
  }
      
  /*
   * Eat any separators at start of the path.
   */
  stripped = rtems_filesystem_prefix_separators (path, strlen(path));
  path += stripped;

  rtems_rfs_rtems_lock (fs);
  
  while (true)
  {
    /*
     * Open and load the inode.
     */
    rc = rtems_rfs_inode_open (fs, ino, &inode, true);
    if (rc > 0)
    {
      rtems_rfs_rtems_unlock (fs);
      return rtems_rfs_rtems_error ("eval_for_make: read ino", rc);
    }
  
    /*
     * If a directory the execute bit must be set for us to enter.
     */
    if (RTEMS_RFS_S_ISDIR (rtems_rfs_inode_get_mode (&inode)) &&
        !rtems_rfs_rtems_eval_perms (&inode, RTEMS_LIBIO_PERMS_SEARCH))
    {
      rtems_rfs_inode_close (fs, &inode);
      rtems_rfs_rtems_unlock (fs);
      return rtems_rfs_rtems_error ("eval_for_make: eval perms", EACCES);
    }
    
    /*
     * Is this the end of the pathname we where given ?
     */
    if (path == *name)
      break;
    
    /*
     * Extract the node name we will look for this time around.
     */
    node = path;
    node_len = 0;
    while (!rtems_filesystem_is_separator(*path) &&
           (*path != '\0') &&
           (node_len < (rtems_rfs_fs_max_name (fs) - 1)))
    {
      node_len++;
      path++;
    }

    /*
     * Eat any separators at start of the new path.
     */
    stripped = rtems_filesystem_prefix_separators (path, strlen (path));
    path += stripped;
    node_len += stripped;

    /*
     * If the node is the current directory and there is more path to come move
     * on it else we are at the inode we want.
     */
    if (rtems_rfs_current_dir (node))
    {
      if (*path)
      {
        rtems_rfs_inode_close (fs, &inode);
        continue;
      }
      break;
    }
     
    /*
     * If the node is a parent we must move up one directory. If the location
     * is on another file system we have a crossmount so we call that file
     * system to handle the remainder of the path.
     */
    if (rtems_rfs_parent_dir (path))
    {
      /*
       * If we are at the root inode of the file system we have a crossmount
       * path.
       */
      if (ino == RTEMS_RFS_ROOT_INO)
      {
        if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_EVAL_FOR_MAKE))
          printf("rtems-rfs-rtems: eval-for-make: crossmount: path:%s\n",
                 path - node_len);

        rtems_rfs_inode_close (fs, &inode);
        rtems_rfs_rtems_unlock (fs);
        *pathloc = pathloc->mt_entry->mt_point_node;
        return (*pathloc->ops->evalformake_h)(path + 2, pathloc, name);
      }

      /*
       * If not a directory give and up return. We cannot change dir from a
       * regular file or device node.
       */
      if (!RTEMS_RFS_S_ISDIR (rtems_rfs_inode_get_mode (&inode)))
      {
        rtems_rfs_inode_close (fs, &inode);
        rtems_rfs_rtems_unlock (fs);
        return rtems_rfs_rtems_error ("eval_for_make: not dir", ENOTSUP);
      }
      
      /*
       * We need to find the parent of this node.
       */
      rc = rtems_rfs_dir_lookup_ino (fs, &inode, "..", 2, &ino, &doff);
      if (rc > 0)
      {
        rtems_rfs_inode_close (fs, &inode);
        rtems_rfs_rtems_unlock (fs);
        return rtems_rfs_rtems_error ("eval_for_make: read parent inode", rc);
      }
      if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_EVAL_FOR_MAKE))
        printf ("rtems-rfs-rtems: eval-for-make: parent: ino:%" PRId32 "\n", ino);
    }
    else
    {
      /*
       * Read the inode so we know it exists and what type it is.
       */
      rc = rtems_rfs_dir_lookup_ino (fs, &inode,
                                     node, node_len - stripped, &ino, &doff);
      if (rc > 0)
      {
        rtems_rfs_inode_close (fs, &inode);
        rtems_rfs_rtems_unlock (fs);
        return rtems_rfs_rtems_error ("eval_for_make: reading inode", rc);
      }
      if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_EVAL_FOR_MAKE))
        printf("rtems-rfs-rtems: eval-for-make: down: path:%s ino:%" PRId32 "\n",
               node, ino);
    }

    rc = rtems_rfs_inode_close (fs, &inode);
    if (rc > 0)
    {
      rtems_rfs_rtems_unlock (fs);
      return rtems_rfs_rtems_error ("eval_for_make: closing node", rc);
    }    
  }

  if (!RTEMS_RFS_S_ISDIR (rtems_rfs_inode_get_mode (&inode)))
  {
    rtems_rfs_inode_close (fs, &inode);
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("eval_for_make: not dir", ENOTDIR);
  }

  if (!rtems_rfs_rtems_eval_perms (&inode, RTEMS_LIBIO_PERMS_WX))
  {
    rtems_rfs_inode_close (fs, &inode);
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("eval_for_make: cannot write", EACCES);
  }
  
  /*
   * Make sure the name does not already exists in the directory.
   */
  rc = rtems_rfs_dir_lookup_ino (fs, &inode, *name, strlen (*name),
                                 &node_ino, &doff);
  if (rc == 0)
  {
    rtems_rfs_inode_close (fs, &inode);
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("eval_for_make: found name", EEXIST);
  }

  if (rc != ENOENT)
  {
    rtems_rfs_inode_close (fs, &inode);
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("eval_for_make: look up", rc);
  }
  
  /*
   * Set the parent ino in the path location.
   */

  rtems_rfs_rtems_set_pathloc_ino (pathloc, ino);
  rtems_rfs_rtems_set_pathloc_doff (pathloc, doff);

  rc = rtems_rfs_rtems_set_handlers (pathloc, &inode) ? 0 : EIO;

  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_EVAL_FOR_MAKE))
    printf("rtems-rfs-rtems: eval-for-make: parent ino:%" PRId32 " name:%s\n",
           ino, *name);

  rtems_rfs_inode_close (fs, &inode);
  rtems_rfs_rtems_unlock (fs);
  
  return rc;
}

/**
 * The following rouine creates a new link node under parent with the name
 * given in name. The link node is set to point to the node at to_loc.
 *
 * @param to_loc
 * @param parent_loc
 * @param name
 * @return int
 */
int
rtems_rfs_rtems_link (rtems_filesystem_location_info_t* to_loc,
                      rtems_filesystem_location_info_t* parent_loc,
                      const char*                       name)
{
  rtems_rfs_file_system* fs = rtems_rfs_rtems_pathloc_dev (to_loc);
  rtems_rfs_ino          target = rtems_rfs_rtems_get_pathloc_ino (to_loc);
  rtems_rfs_ino          parent = rtems_rfs_rtems_get_pathloc_ino (parent_loc);
  int                    rc;

  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_LINK))
    printf ("rtems-rfs-rtems: link: in: parent:%" PRId32 " target:%" PRId32 "\n",
            parent, target);
  
  rtems_rfs_rtems_lock (fs);
  
  rc = rtems_rfs_link (fs, name, strlen (name), parent, target, false);
  if (rc)
  {
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("link: linking", rc);
	}

  rtems_rfs_rtems_unlock (fs);
  
	return 0;
}

/**
 * Routine to remove a link node from the file system.
 *
 * @param parent_loc
 * @param loc
 * @return int
 */

int
rtems_rfs_rtems_unlink (rtems_filesystem_location_info_t* parent_loc,
                        rtems_filesystem_location_info_t* loc)
{
  rtems_rfs_file_system* fs = rtems_rfs_rtems_pathloc_dev (parent_loc);
  rtems_rfs_ino          parent = rtems_rfs_rtems_get_pathloc_ino (parent_loc);
  rtems_rfs_ino          ino = rtems_rfs_rtems_get_pathloc_ino (loc);
  uint32_t               doff = rtems_rfs_rtems_get_pathloc_doff (loc);
  int                    rc;

  rtems_rfs_rtems_lock (fs);
  
  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_UNLINK))
    printf("rtems-rfs-rtems: unlink: parent:%" PRId32 " doff:%" PRIu32 " ino:%" PRId32 "\n",
           parent, doff, ino);
  
  rc = rtems_rfs_unlink (fs, parent, ino, doff, rtems_rfs_unlink_dir_denied);
  if (rc)
  {
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("unlink: unlink inode", rc);
  }
  
  rtems_rfs_rtems_unlock (fs);
  
  return 0;
}

/**
 * The following verifies that and returns the type of node that the loc refers
 * to.
 *
 * @param pathloc
 * @return rtems_filesystem_node_types_t
 */

rtems_filesystem_node_types_t
rtems_rfs_rtems_node_type (rtems_filesystem_location_info_t* pathloc)
{
  rtems_rfs_file_system*        fs = rtems_rfs_rtems_pathloc_dev (pathloc);
  rtems_rfs_ino                 ino = rtems_rfs_rtems_get_pathloc_ino (pathloc);
  rtems_filesystem_node_types_t type;
  rtems_rfs_inode_handle        inode;
  uint16_t                      mode;
  int                           rc;

  rtems_rfs_rtems_lock (fs);

  rc = rtems_rfs_inode_open (fs, ino, &inode, true);
  if (rc > 0)
  {
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("node_type: opening inode", rc);
  }
  
  /*
   * Do not return RTEMS_FILESYSTEM_HARD_LINK because this would result in an
   * eval link which does not make sense in the case of the RFS file
   * system. All directory entries are links to an inode. A link such as a HARD
   * link is actually the normal path to a regular file, directory, device
   * etc's inode. Links to inodes can be considered "the real" one, yet they
   * are all links.
   */
  mode = rtems_rfs_inode_get_mode (&inode);
  if (RTEMS_RFS_S_ISDIR (mode))
    type = RTEMS_FILESYSTEM_DIRECTORY;
  else if (RTEMS_RFS_S_ISLNK (mode))
    type = RTEMS_FILESYSTEM_SYM_LINK;
  else if (RTEMS_RFS_S_ISBLK (mode) || RTEMS_RFS_S_ISCHR (mode))
    type = RTEMS_FILESYSTEM_DEVICE;
  else
    type = RTEMS_FILESYSTEM_MEMORY_FILE;

  rc = rtems_rfs_inode_close (fs, &inode);
  if (rc > 0)
  {
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("node_type: closing inode", rc);
  }
  
  rtems_rfs_rtems_unlock (fs);
  
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
rtems_rfs_rtems_chown (rtems_filesystem_location_info_t *pathloc,
                       uid_t                             owner,
                       gid_t                             group)
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
  
  rtems_rfs_rtems_lock (fs);
  
  rc = rtems_rfs_inode_open (fs, ino, &inode, true);
  if (rc > 0)
  {
    rtems_rfs_rtems_unlock (fs);
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
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("chown: not able", EPERM);
  }
#endif

  rtems_rfs_inode_set_uid_gid (&inode, owner, group);

  rc = rtems_rfs_inode_close (fs, &inode);
  if (rc)
  {
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("chown: closing inode", rc);
  }

  rtems_rfs_rtems_unlock (fs);
  
  return 0;
}

/**
 * This routine is the RFS free node handler for the file system operations
 * table. The RFS does not need to free anything.
 *
 * @param pathloc
 * @retval 0 Always returned.
 */

int
rtems_rfs_rtems_freenodinfo (rtems_filesystem_location_info_t* pathloc)
{
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

int
rtems_rfs_rtems_utime(rtems_filesystem_location_info_t* pathloc,
                      time_t                            atime,
                      time_t                            mtime)
{
  rtems_rfs_file_system* fs = rtems_rfs_rtems_pathloc_dev (pathloc);
  rtems_rfs_ino          ino = rtems_rfs_rtems_get_pathloc_ino (pathloc);
  rtems_rfs_inode_handle inode;
  int                    rc;

  rtems_rfs_rtems_lock (fs);
  
  rc = rtems_rfs_inode_open (fs, ino, &inode, true);
  if (rc)
  {
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("utime: read inode", rc);
  }

  rtems_rfs_inode_set_atime (&inode, atime);
  rtems_rfs_inode_set_mtime (&inode, mtime);

  rc = rtems_rfs_inode_close (fs, &inode);
  if (rc)
  {
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("utime: closing inode", rc);
  }
  
  rtems_rfs_rtems_unlock (fs);
  
  return 0;
}

/**
 * The following rouine creates a new symbolic link node under parent with the
 * name given in name. The node is set to point to the node at to_loc.
 *
 * @param parent_loc
 * @param link_name
 * @param node_name
 * return int
 */

int
rtems_rfs_rtems_symlink (rtems_filesystem_location_info_t* parent_loc,
                         const char*                       link_name,
                         const char*                       node_name)
{
  rtems_rfs_file_system* fs = rtems_rfs_rtems_pathloc_dev (parent_loc);
  rtems_rfs_ino          parent = rtems_rfs_rtems_get_pathloc_ino (parent_loc);
  uid_t                  uid;
  gid_t                  gid;
  int                    rc;

#if defined(RTEMS_POSIX_API)
  uid = geteuid ();
  gid = getegid ();
#else
  uid = 0;
  gid = 0;
#endif

  rtems_rfs_rtems_lock (fs);
  
  rc = rtems_rfs_symlink (fs, node_name, strlen (node_name),
                          link_name, strlen (link_name),
                          uid, gid, parent);
  if (rc)
  {
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("symlink: linking", rc);
  }
  
  rtems_rfs_rtems_unlock (fs);
  
  return 0;
}

/**
 * The following rouine puts the symblic links destination name into buf.
 *
 * @param loc
 * @param buf
 * @param bufsize
 * @return int
 */

int
rtems_rfs_rtems_readlink (rtems_filesystem_location_info_t* pathloc,
                          char*                             buf,
                          size_t                            bufsize)
{
  rtems_rfs_file_system*  fs = rtems_rfs_rtems_pathloc_dev (pathloc);
  rtems_rfs_ino           ino = rtems_rfs_rtems_get_pathloc_ino (pathloc);
  size_t                  length;
  int                     rc;
  
  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_READLINK))
    printf ("rtems-rfs-rtems: readlink: in: ino:%" PRId32 "\n", ino);
  
  rtems_rfs_rtems_lock (fs);
  
  rc = rtems_rfs_symlink_read (fs, ino, buf, bufsize, &length);
  if (rc)
  {
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("readlink: reading link", rc);
  }

  rtems_rfs_rtems_unlock (fs);

  return (int) length;
}

/**
 * File change mode routine.
 *
 * @param pathloc
 * @param mode
 * @return int
 */
int
rtems_rfs_rtems_fchmod (rtems_filesystem_location_info_t* pathloc,
                        mode_t                            mode)
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
  
  rtems_rfs_rtems_lock (fs);
  
  rc = rtems_rfs_inode_open (fs, ino, &inode, true);
  if (rc)
  {
    rtems_rfs_rtems_unlock (fs);
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
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("fchmod: not owner", EPERM);
  }
#endif

  imode &= ~(S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX);
  imode |= mode & (S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX);

  rtems_rfs_inode_set_mode (&inode, imode);
  
  rc = rtems_rfs_inode_close (fs, &inode);
  if (rc > 0)
  {
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("fchmod: closing inode", rc);
  }
  
  rtems_rfs_rtems_unlock (fs);
  
  return 0;
}

/**
 * The following routine does a fcntl on an node.
 *
 * @param cmd
 * @param iop
 * @return int
 */

int
rtems_rfs_rtems_fcntl (int            cmd,
                       rtems_libio_t* iop)
{
  return 0;
}

/**
 * The following routine does a stat on a node.
 *
 * @param pathloc
 * @param buf
 * @return int
 */

int
rtems_rfs_rtems_stat (rtems_filesystem_location_info_t* pathloc,
                      struct stat*                      buf)
{
  rtems_rfs_file_system*  fs = rtems_rfs_rtems_pathloc_dev (pathloc);
  rtems_rfs_ino           ino = rtems_rfs_rtems_get_pathloc_ino (pathloc);
  rtems_rfs_inode_handle  inode;
  rtems_rfs_file_shared*  shared;
  uint16_t                mode;
  int                     rc;

  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_STAT))
    printf ("rtems-rfs-rtems: stat: in: ino:%" PRId32 "\n", ino);

  rtems_rfs_rtems_lock (fs);
  
  rc = rtems_rfs_inode_open (fs, ino, &inode, true);
  if (rc)
  {
    rtems_rfs_rtems_unlock (fs);
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
  buf->st_ino     = ino;
  buf->st_mode    = rtems_rfs_rtems_mode (mode);
  buf->st_nlink   = rtems_rfs_inode_get_links (&inode);
  buf->st_uid     = rtems_rfs_inode_get_uid (&inode);
  buf->st_gid     = rtems_rfs_inode_get_gid (&inode);

  /*
   * Need to check is the ino is an open file. If so we take the values from
   * the open file rather than the inode.
   */
  shared = rtems_rfs_file_get_shared (fs, ino);

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
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("stat: closing inode", rc);
  }
  
  rtems_rfs_rtems_unlock (fs);
  return 0;
}

/**
 * Routine to create a node in the RFS file system.
 *
 * @param name
 * @param mode
 * @param dev
 * @param pathloc
 * @return int
 */

int
rtems_rfs_rtems_mknod (const char                       *name,
                       mode_t                            mode,
                       dev_t                             dev,
                       rtems_filesystem_location_info_t *pathloc)
{
  rtems_rfs_file_system*  fs = rtems_rfs_rtems_pathloc_dev (pathloc);
  rtems_rfs_ino           parent = rtems_rfs_rtems_get_pathloc_ino (pathloc);
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

  rtems_rfs_rtems_lock (fs);
  
  rc = rtems_rfs_inode_create (fs, parent, name, strlen (name),
                               rtems_rfs_rtems_imode (mode),
                               1, uid, gid, &ino);
  if (rc > 0)
  {
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("mknod: inode create", rc);
  }

  rc = rtems_rfs_inode_open (fs, ino, &inode, true);
  if (rc > 0)
  {
    rtems_rfs_rtems_unlock (fs);
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
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("mknod: bad mode", EINVAL);
  }

  rc = rtems_rfs_inode_close (fs, &inode);
  if (rc > 0)
  {
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("mknod: closing inode", rc);
  }
  
  rtems_rfs_rtems_unlock (fs);
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
rtems_rfs_rtems_rmnod (rtems_filesystem_location_info_t* parent_pathloc,
                       rtems_filesystem_location_info_t* pathloc)
{
  rtems_rfs_file_system* fs = rtems_rfs_rtems_pathloc_dev (pathloc);
  rtems_rfs_ino          parent = rtems_rfs_rtems_get_pathloc_ino (parent_pathloc);
  rtems_rfs_ino          ino = rtems_rfs_rtems_get_pathloc_ino (pathloc);
  uint32_t               doff = rtems_rfs_rtems_get_pathloc_doff (pathloc);
  int                    rc;

  if (rtems_rfs_rtems_trace (RTEMS_RFS_RTEMS_DEBUG_RMNOD))
    printf ("rtems-rfs: rmnod: parent:%" PRId32 " doff:%" PRIu32 ", ino:%" PRId32 "\n",
            parent, doff, ino);

  rtems_rfs_rtems_lock (fs);
  
  rc = rtems_rfs_unlink (fs, parent, ino, doff, rtems_rfs_unlink_dir_denied);
  if (rc)
  {
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("rmnod: unlinking", rc);
  }

  rtems_rfs_rtems_unlock (fs);
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
 *
 * @param old_parent_loc The old name's parent location.
 * @param old_loc The old name's location.
 * @param new_parent_loc The new name's parent location.
 * @param new_name The new name.
 * @return int 
 */
int
rtems_rfs_rtems_rename(rtems_filesystem_location_info_t* old_parent_loc,
                       rtems_filesystem_location_info_t* old_loc,
                       rtems_filesystem_location_info_t* new_parent_loc,
                       const char*                       new_name)
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
    printf ("rtems-rfs: rename: ino:%" PRId32 " doff:%" PRIu32 ", new parent:%" PRId32 " new name:%s\n",
            ino, doff, new_parent, new_name);

  rtems_rfs_rtems_lock (fs);

  /*
   * Link to the inode before unlinking so the inode is not erased when
   * unlinked.
   */
  rc = rtems_rfs_link (fs, new_name, strlen (new_name), new_parent, ino, true);
  if (rc)
  {
    rtems_rfs_rtems_unlock (fs);
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
    rtems_rfs_rtems_unlock (fs);
    return rtems_rfs_rtems_error ("rename: unlinking", rc);
  }

  rtems_rfs_rtems_unlock (fs);

  return 0;
}

/**
 * Return the file system stat data.
 *
 * @param pathloc
 * @param sb
 * @return int
 */
int
rtems_rfs_rtems_statvfs (rtems_filesystem_location_info_t* pathloc,
                         struct statvfs*                   sb)
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
  .open_h      = NULL,
  .close_h     = NULL,
  .read_h      = NULL,
  .write_h     = NULL,
  .ioctl_h     = NULL,
  .lseek_h     = NULL,
  .fstat_h     = rtems_rfs_rtems_stat,
  .fchmod_h    = NULL,
  .ftruncate_h = NULL,
  .fpathconf_h = NULL,
  .fsync_h     = NULL,
  .fdatasync_h = NULL,
  .fcntl_h     = NULL,
  .rmnod_h     = rtems_rfs_rtems_rmnod
};

/**
 * Forward decl for the ops table.
 */

int rtems_rfs_rtems_initialise (rtems_filesystem_mount_table_entry_t *mt_entry,
                                const void                           *data);
int rtems_rfs_rtems_shutdown (rtems_filesystem_mount_table_entry_t *mt_entry);

/**
 * RFS file system operations table.
 */
const rtems_filesystem_operations_table rtems_rfs_ops =
{
  .evalpath_h     = rtems_rfs_rtems_eval_path,
  .evalformake_h  = rtems_rfs_rtems_eval_for_make,
  .link_h         = rtems_rfs_rtems_link,
  .unlink_h       = rtems_rfs_rtems_unlink,
  .node_type_h    = rtems_rfs_rtems_node_type,
  .mknod_h        = rtems_rfs_rtems_mknod,
  .chown_h        = rtems_rfs_rtems_chown,
  .freenod_h      = rtems_rfs_rtems_freenodinfo,
  .mount_h        = NULL,
  .fsmount_me_h   = rtems_rfs_rtems_initialise,
  .unmount_h      = NULL,
  .fsunmount_me_h = rtems_rfs_rtems_shutdown,
  .utime_h        = rtems_rfs_rtems_utime,
  .eval_link_h    = NULL, /* never called cause we lie in the node type */
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
  
  mt_entry->fs_info = fs;

  mt_entry->mt_fs_root.node_access = (void*) RTEMS_RFS_ROOT_INO;
  mt_entry->mt_fs_root.handlers    = &rtems_rfs_rtems_dir_handlers;
  mt_entry->mt_fs_root.ops         = &rtems_rfs_ops;

  rtems_rfs_rtems_unlock (fs);
  
  return 0;
}

/**
 * Shutdown the file system.
 */
int
rtems_rfs_rtems_shutdown (rtems_filesystem_mount_table_entry_t* mt_entry)
{
  rtems_rfs_file_system*   fs = mt_entry->fs_info;
  rtems_rfs_rtems_private* rtems;
  int                      rc;

  rtems = rtems_rfs_fs_user (fs);
  
  rc = rtems_rfs_fs_close(fs);
  
  rtems_rfs_mutex_destroy (&rtems->access);
  free (rtems);
  
  return rtems_rfs_rtems_error ("shutdown: close", rc);
}
