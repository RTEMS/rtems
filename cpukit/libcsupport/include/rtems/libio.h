/**
 * @file
 *
 * @ingroup LibIO
 *
 * @brief Basic IO API.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_RTEMS_LIBIO_H
#define _RTEMS_RTEMS_LIBIO_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/statvfs.h>

#include <unistd.h>
#include <termios.h>

#include <rtems.h>
#include <rtems/fs.h>
#include <rtems/chain.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LibIO IO Library
 *
 * @brief Provides system call and file system interface definitions.
 *
 * General purpose communication channel for RTEMS to allow UNIX/POSIX
 * system call behavior under RTEMS.  Initially this supported only
 * IO to devices but has since been enhanced to support networking
 * and support for mounted file systems.
 *
 * @{
 */

typedef off_t rtems_off64_t __attribute__((deprecated));

/**
 * @brief File system node types.
 */
typedef enum {
  RTEMS_FILESYSTEM_INVALID_NODE_TYPE,
  RTEMS_FILESYSTEM_DIRECTORY,
  RTEMS_FILESYSTEM_DEVICE,
  RTEMS_FILESYSTEM_HARD_LINK,
  RTEMS_FILESYSTEM_SYM_LINK,
  RTEMS_FILESYSTEM_MEMORY_FILE
} rtems_filesystem_node_types_t;

/**
 * @name File System Node Operations
 *
 * @{
 */

/**
 *  This type defines the interface to the open(2) system call 
 *  support which is provided by a file system implementation.
 */
typedef int (*rtems_filesystem_open_t)(
  rtems_libio_t *iop,
  const char    *pathname,
  uint32_t       flag,
  uint32_t       mode
);

/**
 *  This type defines the interface to the close(2) system call 
 *  support which is provided by a file system implementation.
 */
typedef int (*rtems_filesystem_close_t)(
  rtems_libio_t *iop
);

/**
 *  This type defines the interface to the read(2) system call 
 *  support which is provided by a file system implementation.
 */
typedef ssize_t (*rtems_filesystem_read_t)(
  rtems_libio_t *iop,
  void          *buffer,
  size_t         count
);

/**
 *  This type defines the interface to the write(2) system call 
 *  support which is provided by a file system implementation.
 */
typedef ssize_t (*rtems_filesystem_write_t)(
  rtems_libio_t *iop,
  const void    *buffer,
  size_t         count
);

/**
 *  This type defines the interface to the ioctl(2) system call 
 *  support which is provided by a file system implementation.
 */
typedef int (*rtems_filesystem_ioctl_t)(
  rtems_libio_t *iop,
  uint32_t       command,
  void          *buffer
);

/**
 *  This type defines the interface to the lseek(2) system call 
 *  support which is provided by a file system implementation.
 */
typedef off_t (*rtems_filesystem_lseek_t)(
  rtems_libio_t *iop,
  off_t          length,
  int            whence
);

/**
 *  This type defines the interface to the fstat(2) system call 
 *  support which is provided by a file system implementation.
 */
typedef int (*rtems_filesystem_fstat_t)(
  rtems_filesystem_location_info_t *loc,
  struct stat                      *buf
);

/**
 *  This type defines the interface to the fchmod(2) system call 
 *  support which is provided by a file system implementation.
 */
typedef int (*rtems_filesystem_fchmod_t)(
  rtems_filesystem_location_info_t *loc,
  mode_t                            mode
);

/**
 *  This type defines the interface to the ftruncate(2) system call 
 *  support which is provided by a file system implementation.
 */
typedef int (*rtems_filesystem_ftruncate_t)(
  rtems_libio_t *iop,
  off_t          length
);

/**
 *  This type defines the interface to the fpathconf(2) system call 
 *  support which is provided by a file system implementation.
 */
typedef int (*rtems_filesystem_fpathconf_t)(
  rtems_libio_t *iop,
  int name
);

/**
 *  This type defines the interface to the fsync(2) system call 
 *  support which is provided by a file system implementation.
 */
typedef int (*rtems_filesystem_fsync_t)(
  rtems_libio_t *iop
);

/**
 *  This type defines the interface to the fdatasync(2) system call 
 *  support which is provided by a file system implementation.
 */
typedef int (*rtems_filesystem_fdatasync_t)(
  rtems_libio_t *iop
);

/**
 *  This type defines the interface to the fnctl(2) system call 
 *  support which is provided by a file system implementation.
 */
typedef int (*rtems_filesystem_fcntl_t)(
  int            cmd,
  rtems_libio_t *iop
);

typedef int (*rtems_filesystem_rmnod_t)(
 rtems_filesystem_location_info_t      *parent_loc,   /* IN */
 rtems_filesystem_location_info_t      *pathloc       /* IN */
);

/** @} */

/**
 * @brief File system node operations table.
 */
struct _rtems_filesystem_file_handlers_r {
  /**
   *  This field points to the file system specific implementation
   *  of the support routine for the open(2) system call 
   *
   *  @note This method must have a filesystem specific implementation.
   *
   *  @note There is no default implementation.
   */
  rtems_filesystem_open_t         open_h;

  /**
   *  This field points to the file system specific implementation
   *  of the support routine for the close(2) system call 
   *
   *  @note This method is REQUIRED by all file systems.
   *
   *  @note There is no default implementation.
   */
  rtems_filesystem_close_t        close_h;

  /**
   *  This field points to the file system specific implementation
   *  of the support routine for the read(2) system call 
   *
   *  @note This method must have a filesystem specific implementation.
   *
   *  @note The default implementation returns -1 and sets
   *  errno to ENOTSUP.
   */
  rtems_filesystem_read_t         read_h;

  /**
   *  This field points to the file system specific implementation
   *  of the support routine for the write(2) system call 
   *
   *  @note This method may use a default implementation.
   *
   *  @note The default implementation returns -1 and sets
   *  errno to ENOTSUP.
   */
  rtems_filesystem_write_t        write_h;

  /**
   *  This field points to the file system specific implementation
   *  of the support routine for the ioctl(2) system call 
   *
   *  @note This method may use a default implementation.
   *
   *  @note The default implementation returns -1 and sets
   *  errno to ENOTSUP.
   */
  rtems_filesystem_ioctl_t        ioctl_h;

  /**
   *  This field points to the file system specific implementation
   *  of the support routine for the lseek(2) system call 
   *
   *  @note This method may use a default implementation.
   *
   *  @note The default implementation returns -1 and sets
   *  errno to ENOTSUP.
   */
  rtems_filesystem_lseek_t        lseek_h;

  /**
   *  This field points to the file system specific implementation
   *  of the support routine for the fstat(2) system call 
   *
   *  @note This method may use a default implementation.
   *
   *  @note The default implementation returns -1 and sets
   *  errno to ENOTSUP.
   */
  rtems_filesystem_fstat_t        fstat_h;

  /**
   *  This field points to the file system specific implementation
   *  of the support routine for the fchmod(2) system call 
   *
   *  @note This method may use a default implementation.
   *
   *  @note The default implementation returns -1 and sets
   *  errno to ENOTSUP.
   */
  rtems_filesystem_fchmod_t       fchmod_h;

  /**
   *  This field points to the file system specific implementation
   *  of the support routine for the ftruncate(2) system call 
   *
   *  @note This method may use a default implementation.
   *
   *  @note The default implementation returns -1 and sets
   *  errno to ENOTSUP.
   */
  rtems_filesystem_ftruncate_t    ftruncate_h;

  /**
   *  This field points to the file system specific implementation
   *  of the support routine for the fpathconf(2) system call 
   *
   *  @note This method may use a default implementation.
   *
   *  @note The default implementation returns -1 and sets
   *  errno to ENOTSUP.
   */
  rtems_filesystem_fpathconf_t    fpathconf_h;

  /**
   *  This field points to the file system specific implementation
   *  of the support routine for the fsync(2) system call 
   *
   *  @note This method may use a default implementation.
   *
   *  @note The default implementation returns -1 and sets
   *  errno to ENOTSUP.
   */
  rtems_filesystem_fsync_t        fsync_h;

  /**
   *  This field points to the file system specific implementation
   *  of the support routine for the fdatasync(2) system call 
   *
   *  @note This method may use a default implementation.
   *
   *  @note The default implementation returns -1 and sets
   *  errno to ENOTSUP.
   */
  rtems_filesystem_fdatasync_t    fdatasync_h;

  /**
   *  This field points to the file system specific implementation
   *  of the support routine for the fcntl(2) system call 
   *
   *  @note This method may use a default implementation.
   *
   *  @note The default implementation returns -1 and sets
   *  errno to ENOTSUP.
   */
  rtems_filesystem_fcntl_t        fcntl_h;

  /**
   *  This field points to the file system specific implementation
   *  of the support routine for the rmnod(2) system call 
   *
   *  @note This method may use a default implementation.
   *
   *  @note The default implementation returns -1 and sets
   *  errno to ENOTSUP.
   */
  rtems_filesystem_rmnod_t        rmnod_h;
};

extern const rtems_filesystem_file_handlers_r
rtems_filesystem_handlers_default;

/**
 *  This method defines the interface to the default open(2) 
 *  system call support which is provided by a file system 
 *  implementation.
 */
int rtems_filesystem_default_open(
  rtems_libio_t *iop,
  const char    *pathname,
  uint32_t       flag,
  uint32_t       mode
);

/**
 *  This method defines the interface to the default close(2) 
 *  system call support which is provided by a file system 
 *  implementation.
 */
int rtems_filesystem_default_close(
  rtems_libio_t *iop
);


/**
 *  This method defines the interface to the default read(2) 
 *  system call support which is provided by a file system 
 *  implementation.
 */
ssize_t rtems_filesystem_default_read(
  rtems_libio_t *iop,
  void          *buffer,
  size_t         count
);

/**
 *  This method defines the interface to the default write(2) system call 
 *  support which is provided by a file system implementation.
 */
ssize_t rtems_filesystem_default_write(
  rtems_libio_t *iop,
  const void    *buffer,
  size_t         count
);

/**
 *  This method defines the interface to the default ioctl(2) system call 
 *  support which is provided by a file system implementation.
 */
int rtems_filesystem_default_ioctl(
  rtems_libio_t *iop,
  uint32_t       command,
  void          *buffer
);

/**
 *  This method defines the interface to the default lseek(2) system call 
 *  support which is provided by a file system implementation.
 */
off_t rtems_filesystem_default_lseek(
  rtems_libio_t *iop,
  off_t          length,
  int            whence
);

/**
 *  This method defines the interface to the default fstat(2) system call 
 *  support which is provided by a file system implementation.
 */
int rtems_filesystem_default_fstat(
  rtems_filesystem_location_info_t *loc,
  struct stat                      *buf
);

/**
 *  This method defines the interface to the default fchmod(2) system call 
 *  support which is provided by a file system implementation.
 */
int rtems_filesystem_default_fchmod(
  rtems_filesystem_location_info_t *loc,
  mode_t                            mode
);

/**
 *  This method defines the interface to the default ftruncate(2) system call 
 *  support which is provided by a file system implementation.
 */
int rtems_filesystem_default_ftruncate(
  rtems_libio_t *iop,
  off_t          length
);

/**
 *  This method defines the interface to the default fpathconf(2) system call 
 *  support which is provided by a file system implementation.
 */
int rtems_filesystem_default_fpathconf(
  rtems_libio_t *iop,
  int name
);

/**
 *  This method defines the interface to the default fsync(2) system call 
 *  support which is provided by a file system implementation.
 */
int rtems_filesystem_default_fsync(
  rtems_libio_t *iop
);

/**
 *  This method defines the interface to the default fdatasync(2) system call 
 *  support which is provided by a file system implementation.
 */
int rtems_filesystem_default_fdatasync(
  rtems_libio_t *iop
);

/**
 *  This method defines the interface to the default fnctl(2) system call 
 *  support which is provided by a file system implementation.
 */
int rtems_filesystem_default_fcntl(
  int            cmd,
  rtems_libio_t *iop
);

/**
 *  This method defines the interface to the default rmnod(2) system call 
 *  support which is provided by a file system implementation.
 */
int rtems_filesystem_default_rmnod(
 rtems_filesystem_location_info_t      *parent_loc,   /* IN */
 rtems_filesystem_location_info_t      *pathloc       /* IN */
);

/**
 * @name File System Operations
 *
 * @{
 */

/**
 *  This type defines the interface to the mknod(2) system call 
 *  support which is provided by a file system implementation.
 *  
 *  @note This routine does not allocate any space and 
 *  rtems_filesystem_freenode_t is not called by the generic 
 *  after calling this routine. ie. node_access does not have 
 *  to contain valid data when the routine returns.
 */
typedef int (*rtems_filesystem_mknod_t)(
   const char                        *path,       /* IN */
   mode_t                             mode,       /* IN */
   dev_t                              dev,        /* IN */
   rtems_filesystem_location_info_t  *pathloc     /* IN/OUT */
);

/**
 *  This type defines the interface that allows the  
 *  file system implementation to parse a path and 
 *  allocate any memory necessary for tracking purposes.
 *
 *  @note rtems_filesystem_freenode_t must be called by 
 *  the generic after calling this routine
 */
typedef int (*rtems_filesystem_evalpath_t)(
  const char                        *pathname,      /* IN     */
  size_t                             pathnamelen,   /* IN     */
  int                                flags,         /* IN     */
  rtems_filesystem_location_info_t  *pathloc        /* IN/OUT */
);

/**
 *  This type defines the interface that allows the  
 *  file system implementation to parse a path with the
 *  intent of creating a new node and to  
 *  allocate any memory necessary for tracking purposes.
 *
 *  @note rtems_filesystem_freenode_t must be called by 
 *  the generic after calling this routine
 */
typedef int (*rtems_filesystem_evalmake_t)(
   const char                       *path,       /* IN */
   rtems_filesystem_location_info_t *pathloc,    /* IN/OUT */
   const char                      **name        /* OUT    */
);

/**
 *  This type defines the interface to the link(2) system call 
 *  support which is provided by a file system implementation.
 */  
typedef int (*rtems_filesystem_link_t)(
  rtems_filesystem_location_info_t  *to_loc,      /* IN */
  rtems_filesystem_location_info_t  *parent_loc,  /* IN */
  const char                        *name         /* IN */
);

/**
 *  This type defines the interface to the unlink(2) system call 
 *  support which is provided by a file system implementation.
 */ 
typedef int (*rtems_filesystem_unlink_t)(
  rtems_filesystem_location_info_t  *parent_pathloc, /* IN */
  rtems_filesystem_location_info_t  *pathloc         /* IN */
);

/**
 *  This type defines the interface to the chown(2) system call 
 *  support which is provided by a file system implementation.
 */ 
typedef int (*rtems_filesystem_chown_t)(
  rtems_filesystem_location_info_t  *pathloc,       /* IN */
  uid_t                              owner,         /* IN */
  gid_t                              group          /* IN */
);

/**
 *  This type defines the interface to the freenod(2) system call 
 *  support which is provided by a file system implementation.
 */ 
typedef int (*rtems_filesystem_freenode_t)(
 rtems_filesystem_location_info_t      *pathloc       /* IN */
);

/**
 *  This type defines the interface that allows the implemented
 *  filesystem ot mount another filesystem at the given location.
 */ 
typedef int (* rtems_filesystem_mount_t ) (
   rtems_filesystem_mount_table_entry_t *mt_entry     /* IN */
);

/**
 *  This type defines the interface that allows a file system 
 *  implementation to do any necessary work that is needed when
 *  it is being mounted.
 */  
typedef int (* rtems_filesystem_fsmount_me_t )(
  rtems_filesystem_mount_table_entry_t *mt_entry,     /* IN */
  const void                           *data          /* IN */
);

/**
 *  This type defines the interface allow the filesystem to
 *  unmount a filesystem that was mounted at one of its node
 *  locations.
 */ 
typedef int (* rtems_filesystem_unmount_t ) (
  rtems_filesystem_mount_table_entry_t *mt_entry     /* IN */
);

/**
 *  This type defines the interface that allows a file system 
 *  implementation to do any necessary work that is needed when
 *  it is being unmounted.
 */ 
typedef int (* rtems_filesystem_fsunmount_me_t ) (
   rtems_filesystem_mount_table_entry_t *mt_entry    /* IN */
);

/**
 *  This type defines the interface that will return the 
 *  type of a filesystem implementations node.
 */ 
typedef rtems_filesystem_node_types_t (* rtems_filesystem_node_type_t) (
  rtems_filesystem_location_info_t    *pathloc      /* IN */
);

/**
 *  This type defines the interface to the time(2) system call 
 *  support which is provided by a file system implementation.
 */ 
typedef int (* rtems_filesystem_utime_t)(
  rtems_filesystem_location_info_t  *pathloc,       /* IN */
  time_t                             actime,        /* IN */
  time_t                             modtime        /* IN */
);

/**
 *  This type defines the interface to the link(2) system call 
 *  support which is provided by a file system implementation.
 */ 
typedef int (*rtems_filesystem_evaluate_link_t)(
  rtems_filesystem_location_info_t *pathloc,     /* IN/OUT */
  int                               flags        /* IN     */
);

/**
 *  This type defines the interface to the symlink(2) system call 
 *  support which is provided by a file system implementation.
 */ 
typedef int (*rtems_filesystem_symlink_t)(
 rtems_filesystem_location_info_t  *loc,         /* IN */
 const char                        *link_name,   /* IN */
 const char                        *node_name
);

/**
 *  This type defines the interface to the readlink(2) system call 
 *  support which is provided by a file system implementation.
 */ 
typedef ssize_t (*rtems_filesystem_readlink_t)(
 rtems_filesystem_location_info_t  *loc,     /* IN  */
 char                              *buf,     /* OUT */
 size_t                             bufsize
);

/**
 *  This type defines the interface to the name(2) system call 
 *  support which is provided by a file system implementation.
 */ 
typedef int (*rtems_filesystem_rename_t)(
 rtems_filesystem_location_info_t  *old_parent_loc,  /* IN */
 rtems_filesystem_location_info_t  *old_loc,         /* IN */
 rtems_filesystem_location_info_t  *new_parent_loc,  /* IN */
 const char                        *name             /* IN */
);

/**
 *  This type defines the interface to the statvfs(2) system call 
 *  support which is provided by a file system implementation.
 */ 
typedef int (*rtems_filesystem_statvfs_t)(
 rtems_filesystem_location_info_t  *loc,     /* IN  */
 struct statvfs                    *buf      /* OUT */
);

/** @} */

/**
 * @brief File system operations table.
 */
struct _rtems_filesystem_operations_table {

    /**
     *  This field points to the file system specific implementation
     *  of the support routine that evaluates a character path and
     *  returns the node assocated with the last node in the path. 
     *
     *  @note This method must have a filesystem specific implementation.
     *
     *  @note There is no default implementation.
     */
    rtems_filesystem_evalpath_t      evalpath_h;

    /**
     *  This field points to the file system specific implementation
     *  of the support routine that evaluates a character path and
     *  returns the node assocated with next to the last node in 
     *  the path.  The last node will be the new node to be created.
     *
     *  @note This method must have a filesystem specific implementation.
     *
     *  @note There is no default implementation.
     */
    rtems_filesystem_evalmake_t      evalformake_h;

    /**
     *  This field points to the file system specific implementation
     *  of the support routine for the link(2) system call 
     *
     *  @note This method may use a default implementation.
     *
     *  @note The default implementation returns -1 and sets
     *  errno to ENOTSUP.
     */
    rtems_filesystem_link_t          link_h;

    /**
     *  This field points to the file system specific implementation
     *  of the support routine for the unlink(2) system call 
     *
     *  @note This method may use a default implementation.
     *
     *  @note The default implementation returns -1 and sets
     *  errno to ENOTSUP.
     */
    rtems_filesystem_unlink_t        unlink_h;

    /**
     *  This field points to the file system specific implementation
     *  of a method that returns the node type of the given node. 
     *
     *  @note This method may use a default implementation.
     *
     *  @note The default implementation returns -1 and sets
     *  errno to ENOTSUP.
     */
    rtems_filesystem_node_type_t     node_type_h;

    /**
     *  This field points to the file system specific implementation
     *  of the support routine for the link(2) system call 
     *
     *  @note This method may use a mknod implementation.
     *
     *  @note The default implementation returns -1 and sets
     *  errno to ENOTSUP.
     */
    rtems_filesystem_mknod_t         mknod_h;

    /**
     *  This field points to the file system specific implementation
     *  of the support routine for the link(2) system call 
     *
     *  @note This method may use a default implementation.
     *
     *  @note The default implementation returns -1 and sets
     *  errno to ENOTSUP.
     */
    rtems_filesystem_chown_t         chown_h;

    /**
     *  This field points to the file system specific implementation
     *  of the support routine for the freenod(2) system call 
     *
     *  @note This method may use a default implementation.
     *
     *  @note The default implementation returns -1 and sets
     *  errno to ENOTSUP.
     */
    rtems_filesystem_freenode_t      freenod_h;

    /**
     *  This field points to the file system specific implementation
     *  of the support routine for the mount(2) system call 
     *
     *  @note This method may use a default implementation.
     *
     *  @note The default implementation returns -1 and sets
     *  errno to ENOTSUP.
     */
    rtems_filesystem_mount_t         mount_h;

    /**
     *  This field points to the file system specific implementation
     *  of the support routine for the fsmount(2) system call 
     *
     *  @note This method may use a default implementation.
     *
     *  @note The default implementation returns -1 and sets
     *  errno to ENOTSUP.
     */
    rtems_filesystem_fsmount_me_t    fsmount_me_h;

    /**
     *  This field points to the file system specific implementation
     *  of the support routine for the unmount(2) system call 
     *
     *  @note This method may use a default implementation.
     *
     *  @note The default implementation returns -1 and sets
     *  errno to ENOTSUP.
     */
    rtems_filesystem_unmount_t       unmount_h;

    /**
     *  This field points to the file system specific implementation
     *  of the support routine for the fsunmount(2) system call 
     *
     *  @note This method may use a default implementation.
     *
     *  @note The default implementation returns -1 and sets
     *  errno to ENOTSUP.
     */
    rtems_filesystem_fsunmount_me_t  fsunmount_me_h;

    /**
     *  This field points to the file system specific implementation
     *  of the support routine for the utime(2) system call 
     *
     *  @note This method may use a default implementation.
     *
     *  @note The default implementation returns -1 and sets
     *  errno to ENOTSUP.
     */
    rtems_filesystem_utime_t         utime_h;

    /**
     *  This field points to the file system specific implementation
     *  of the support routine for the eval_link(2) system call 
     *
     *  @note This method may use a default implementation.
     *
     *  @note The default implementation returns -1 and sets
     *  errno to ENOTSUP.
     */
    rtems_filesystem_evaluate_link_t eval_link_h;

    /**
     *  This field points to the file system specific implementation
     *  of the support routine for the sumlink(2) system call 
     *
     *  @note This method may use a default implementation.
     *
     *  @note The default implementation returns -1 and sets
     *  errno to ENOTSUP.
     */
    rtems_filesystem_symlink_t       symlink_h;

    /**
     *  This field points to the file system specific implementation
     *  of the support routine for the readlink(2) system call 
     *
     *  @note This method may use a default implementation.
     *
     *  @note The default implementation returns -1 and sets
     *  errno to ENOTSUP.
     */
    rtems_filesystem_readlink_t      readlink_h;

    /**
     *  This field points to the file system specific implementation
     *  of the support routine for the rename(2) system call 
     *
     *  @note This method may use a default implementation.
     *
     *  @note The default implementation returns -1 and sets
     *  errno to ENOTSUP.
     */
    rtems_filesystem_rename_t        rename_h;

    /**
     *  This field points to the file system specific implementation
     *  of the support routine for the statvfs(2) system call 
     *
     *  @note This method may use a default implementation.
     *
     *  @note The default implementation returns -1 and sets
     *  errno to ENOTSUP.
     */
    rtems_filesystem_statvfs_t       statvfs_h;
};

extern const rtems_filesystem_operations_table
rtems_filesystem_operations_default;

/**
 * @brief Provides a defualt routine for filesystem
 * implementation of path evaluation.
 */
int rtems_filesystem_default_evalpath(
  const char *pathname,
  size_t pathnamelen,
  int flags,
  rtems_filesystem_location_info_t *pathloc
);

/**
 * @brief Provides a defualt routine for filesystem
 * implementation of path evaluation for make.
 */
int rtems_filesystem_default_evalformake(
   const char *path,
   rtems_filesystem_location_info_t *pathloc,
   const char **name
);

/**
 * @brief Provides a defualt routine for filesystem
 * implementation of a link command.
 */
int rtems_filesystem_default_link(
 rtems_filesystem_location_info_t  *to_loc,      /* IN */
 rtems_filesystem_location_info_t  *parent_loc,  /* IN */
 const char                        *name         /* IN */
);

/**
 * @brief Provides a defualt routine for filesystem
 * implementation of a unlink command.
 */
int rtems_filesystem_default_unlink(
 rtems_filesystem_location_info_t  *parent_pathloc, /* IN */
 rtems_filesystem_location_info_t  *pathloc         /* IN */
);

/**
 * @brief Provides a defualt routine for filesystem
 * implementation to determine the node type.
 */
rtems_filesystem_node_types_t rtems_filesystem_default_node_type(
  rtems_filesystem_location_info_t *pathloc
);

/**
 * @brief Provides a defualt routine for filesystem
 * implementation to create a new node.
 */
int rtems_filesystem_default_mknod(
   const char                        *path,       /* IN */
   mode_t                             mode,       /* IN */
   dev_t                              dev,        /* IN */
   rtems_filesystem_location_info_t  *pathloc     /* IN/OUT */
);

/**
 * @brief Provides a defualt routine for filesystem
 * implementation of a chown command.
 */
int rtems_filesystem_default_chown(
 rtems_filesystem_location_info_t  *pathloc,       /* IN */
 uid_t                              owner,         /* IN */
 gid_t                              group          /* IN */
);

/**
 * @brief Provides a defualt routine for filesystem
 * implementation of a freenode command.
 */
int rtems_filesystem_default_freenode(
 rtems_filesystem_location_info_t      *pathloc       /* IN */
);

/**
 * @brief Provides a defualt routine for filesystem
 * implementation of a mount command.
 */
int rtems_filesystem_default_mount (
   rtems_filesystem_mount_table_entry_t *mt_entry     /* IN */
);

/**
 * @brief Provides a defualt routine for filesystem
 * implementation of a fsmount command.
 */
int rtems_filesystem_default_fsmount(
  rtems_filesystem_mount_table_entry_t *mt_entry,     /* IN */
  const void                           *data          /* IN */
);

/**
 * @brief Provides a defualt routine for filesystem
 * implementation of a unmount command.
 */
int rtems_filesystem_default_unmount(
  rtems_filesystem_mount_table_entry_t *mt_entry     /* IN */
);

/**
 * @brief Provides a defualt routine for filesystem
 * implementation of a fsunmount command.
 */
int rtems_filesystem_default_fsunmount(
   rtems_filesystem_mount_table_entry_t *mt_entry    /* IN */
);


/**
 * @brief Provides a defualt routine for filesystem
 * implementation of a utime command.
 */
int rtems_filesystem_default_utime(
  rtems_filesystem_location_info_t  *pathloc,       /* IN */
  time_t                             actime,        /* IN */
  time_t                             modtime        /* IN */
);

/**
 * @brief Provides a defualt routine for filesystem
 * implementation of a link command.
 */
int rtems_filesystem_default_evaluate_link(
  rtems_filesystem_location_info_t *pathloc,     /* IN/OUT */
  int                               flags        /* IN     */
);

/**
 * @brief Provides a defualt routine for filesystem
 * implementation of a symlink command.
 */
int rtems_filesystem_default_symlink(
 rtems_filesystem_location_info_t  *loc,         /* IN */
 const char                        *link_name,   /* IN */
 const char                        *node_name
);

/**
 * @brief Provides a defualt routine for filesystem
 * implementation of a readlink command.
 */
ssize_t rtems_filesystem_default_readlink(
 rtems_filesystem_location_info_t  *loc,     /* IN  */
 char                              *buf,     /* OUT */
 size_t                             bufsize
);

/**
 * @brief Provides a defualt routine for filesystem
 * implementation of a rename command.
 */
int rtems_filesystem_default_rename(
 rtems_filesystem_location_info_t  *old_parent_loc,  /* IN */
 rtems_filesystem_location_info_t  *old_loc,         /* IN */
 rtems_filesystem_location_info_t  *new_parent_loc,  /* IN */
 const char                        *name             /* IN */
);

/**
 * @brief Provides a defualt routine for filesystem
 * implementation of a statvfs command.
 */
int rtems_filesystem_default_statvfs(
 rtems_filesystem_location_info_t  *loc,     /* IN  */
 struct statvfs                    *buf      /* OUT */
);

/**
 * @brief Gets the mount handler for the file system @a type.
 *
 * @return The file system mount handler associated with the @a type, or
 * @c NULL if no such association exists.
 */
rtems_filesystem_fsmount_me_t
rtems_filesystem_get_mount_handler(
  const char *type
);

/**
 * @brief Contain file system specific information which is required to support
 * fpathconf().
 */
typedef struct {
  int    link_max;                 /* count */
  int    max_canon;                /* max formatted input line size */
  int    max_input;                /* max input line size */
  int    name_max;                 /* max name length */
  int    path_max;                 /* max path */
  int    pipe_buf;                 /* pipe buffer size */
  int    posix_async_io;           /* async IO supported on fs, 0=no, 1=yes */
  int    posix_chown_restrictions; /* can chown: 0=no, 1=yes */
  int    posix_no_trunc;           /* error on names > max name, 0=no, 1=yes */
  int    posix_prio_io;            /* priority IO, 0=no, 1=yes */
  int    posix_sync_io;            /* file can be sync'ed, 0=no, 1=yes */
  int    posix_vdisable;           /* special char processing, 0=no, 1=yes */
} rtems_filesystem_limits_and_options_t;

/**
 * @brief Default pathconf settings.
 *
 * Override in a filesystem.
 */
extern const rtems_filesystem_limits_and_options_t rtems_filesystem_default_pathconf;

/**
 * @brief An open file data structure.
 *
 * It will be indexed by 'fd'.
 *
 * @todo Should really have a separate per/file data structure that this points
 * to (eg: size, offset, driver, pathname should be in that)
 */
struct rtems_libio_tt {
  rtems_driver_name_t                    *driver;
  off_t                                   size;      /* size of file */
  off_t                                   offset;    /* current offset into file */
  uint32_t                                flags;
  rtems_filesystem_location_info_t        pathinfo;
  rtems_id                                sem;
  uint32_t                                data0;     /* private to "driver" */
  void                                   *data1;     /* ... */
};

/**
 * @brief Paramameter block for read/write.
 *
 * It must include 'offset' instead of using iop's offset since we can have
 * multiple outstanding i/o's on a device.
 */
typedef struct {
  rtems_libio_t          *iop;
  off_t                   offset;
  char                   *buffer;
  uint32_t                count;
  uint32_t                flags;
  uint32_t                bytes_moved;
} rtems_libio_rw_args_t;

/**
 * @brief Parameter block for open/close.
 */
typedef struct {
  rtems_libio_t          *iop;
  uint32_t                flags;
  uint32_t                mode;
} rtems_libio_open_close_args_t;

/**
 * @brief Parameter block for ioctl.
 */
typedef struct {
  rtems_libio_t          *iop;
  uint32_t                command;
  void                   *buffer;
  uint32_t                ioctl_return;
} rtems_libio_ioctl_args_t;

/**
 * @name Flag Values
 *
 * @{
 */

#define LIBIO_FLAGS_NO_DELAY      0x0001U  /* return immediately if no data */
#define LIBIO_FLAGS_READ          0x0002U  /* reading */
#define LIBIO_FLAGS_WRITE         0x0004U  /* writing */
#define LIBIO_FLAGS_OPEN          0x0100U  /* device is open */
#define LIBIO_FLAGS_APPEND        0x0200U  /* all writes append */
#define LIBIO_FLAGS_CREATE        0x0400U  /* create file */
#define LIBIO_FLAGS_CLOSE_ON_EXEC 0x0800U  /* close on process exec() */
#define LIBIO_FLAGS_READ_WRITE    (LIBIO_FLAGS_READ | LIBIO_FLAGS_WRITE)

/** @} */

void rtems_libio_init(void);

/**
 * @name External I/O Handlers
 *
 * @{
 */

typedef int (*rtems_libio_open_t)(
  const char  *pathname,
  uint32_t    flag,
  uint32_t    mode
);

typedef int (*rtems_libio_close_t)(
  int  fd
);

typedef ssize_t (*rtems_libio_read_t)(
  int         fd,
  void       *buffer,
  size_t    count
);

typedef ssize_t (*rtems_libio_write_t)(
  int         fd,
  const void *buffer,
  size_t      count
);

typedef int (*rtems_libio_ioctl_t)(
  int         fd,
  uint32_t    command,
  void       *buffer
);

typedef off_t (*rtems_libio_lseek_t)(
  int           fd,
  off_t         offset,
  int           whence
);

/** @} */

/**
 * @name Permission Macros
 *
 * @{
 */

/*
 *  The following macros are used to build up the permissions sets
 *  used to check permissions.  These are similar in style to the
 *  mode_t bits and should stay compatible with them.
 */
#define RTEMS_LIBIO_PERMS_READ   S_IROTH
#define RTEMS_LIBIO_PERMS_WRITE  S_IWOTH
#define RTEMS_LIBIO_PERMS_RDWR   (S_IROTH|S_IWOTH)
#define RTEMS_LIBIO_PERMS_EXEC   S_IXOTH
#define RTEMS_LIBIO_PERMS_SEARCH RTEMS_LIBIO_PERMS_EXEC
#define RTEMS_LIBIO_PERMS_RWX    S_IRWXO

/** @} */

union __rtems_dev_t {
  dev_t device;
  struct {
     rtems_device_major_number major;
     rtems_device_minor_number minor;
  } __overlay;
};

static inline dev_t rtems_filesystem_make_dev_t(
  rtems_device_major_number _major,
  rtems_device_minor_number _minor
)
{
  union __rtems_dev_t temp;

  temp.__overlay.major = _major;
  temp.__overlay.minor = _minor;
  return temp.device;
}

static inline rtems_device_major_number rtems_filesystem_dev_major_t(
  dev_t device
)
{
  union __rtems_dev_t temp;

  temp.device = device;
  return temp.__overlay.major;
}


static inline rtems_device_minor_number rtems_filesystem_dev_minor_t(
  dev_t device
)
{
  union __rtems_dev_t temp;

  temp.device = device;
  return temp.__overlay.minor;
}

#define rtems_filesystem_split_dev_t( _dev, _major, _minor ) \
  do { \
    (_major) = rtems_filesystem_dev_major_t ( _dev ); \
    (_minor) = rtems_filesystem_dev_minor_t( _dev ); \
  } while(0)

/*
 * Verifies that the permission flag is valid.
 */
#define rtems_libio_is_valid_perms( _perm )     \
 (((~RTEMS_LIBIO_PERMS_RWX) & _perm ) == 0)

/*
 *  Prototypes for filesystem
 */

void rtems_filesystem_initialize( void );

typedef void (*rtems_libio_init_functions_t)(void);
extern  rtems_libio_init_functions_t rtems_libio_init_helper;

void    open_dev_console(void);

typedef void (*rtems_libio_supp_functions_t)(void);
extern  rtems_libio_supp_functions_t rtems_libio_supp_helper;

typedef void (*rtems_fs_init_functions_t)(void);
extern  rtems_fs_init_functions_t    rtems_fs_init_helper;

/**
 * @brief Creates a directory and all its parent directories according to
 * @a path.
 *
 * The @a mode value selects the access permissions of the directory.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The @c errno indicates the error.
 */
extern int rtems_mkdir(const char *path, mode_t mode);

/** @} */

/**
 * @defgroup FileSystemTypesAndMount File System Types and Mount
 *
 * @ingroup LibIO
 *
 * @brief File system types and mount.
 *
 * @{
 */

/**
 * @name File System Types
 *
 * @{
 */

#define RTEMS_FILESYSTEM_TYPE_IMFS "imfs"
#define RTEMS_FILESYSTEM_TYPE_MINIIMFS "mimfs"
#define RTEMS_FILESYSTEM_TYPE_DEVFS "devfs"
#define RTEMS_FILESYSTEM_TYPE_FTPFS "ftpfs"
#define RTEMS_FILESYSTEM_TYPE_TFTPFS "tftpfs"
#define RTEMS_FILESYSTEM_TYPE_NFS "nfs"
#define RTEMS_FILESYSTEM_TYPE_DOSFS "dosfs"
#define RTEMS_FILESYSTEM_TYPE_RFS "rfs"

/** @} */

/**
 * @brief Mount table entry.
 */
struct rtems_filesystem_mount_table_entry_tt {
  rtems_chain_node                       Node;
  rtems_filesystem_location_info_t       mt_point_node;
  rtems_filesystem_location_info_t       mt_fs_root;
  int                                    options;
  void                                  *fs_info;

  rtems_filesystem_limits_and_options_t  pathconf_limits_and_options;

  /*
   * The target or mount point of the file system.
   */
  const char                            *target;

  /*
   * The type of filesystem or the name of the filesystem.
   */
  const char                            *type;

  /*
   *  When someone adds a mounted filesystem on a real device,
   *  this will need to be used.
   *
   *  The lower layers can manage how this is managed. Leave as a
   *  string.
   */
  char                                  *dev;
};

/**
 * @brief File system options.
 */
typedef enum {
  RTEMS_FILESYSTEM_READ_ONLY,
  RTEMS_FILESYSTEM_READ_WRITE,
  RTEMS_FILESYSTEM_BAD_OPTIONS
} rtems_filesystem_options_t;

/**
 * @brief File system table entry.
 */
typedef struct rtems_filesystem_table_t {
  const char                    *type;
  rtems_filesystem_fsmount_me_t  mount_h;
} rtems_filesystem_table_t;

/**
 * @brief Static table of file systems.
 *
 * Externally defined by confdefs.h or the user.
 */
extern const rtems_filesystem_table_t rtems_filesystem_table [];

/**
 * @brief Registers a file system @a type.
 *
 * The @a mount_h handler will be used to mount a file system of this @a type.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The @c errno indicates the error.
 */
int rtems_filesystem_register(
  const char                    *type,
  rtems_filesystem_fsmount_me_t  mount_h
);

/**
 * @brief Unregisters a file system @a type.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The @c errno indicates the error.
 */
int rtems_filesystem_unregister(
  const char *type
);

/**
 * @brief Unmounts the file system at @a mount_path.
 *
 * @todo Due to file system implementation shortcomings it is possible to
 * unmount file systems in use.  This likely leads to heap corruption.  Unmount
 * only file systems which are not in use by the application.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The @c errno indicates the error.
 */
int unmount(
  const char *mount_path
);

/**
 * @brief Mounts a file system at @a target.
 *
 * The @a source may be a path to the corresponding device file, or @c NULL.
 * The @a target path must lead to an existing directory, or @c NULL.  In case
 * @a target is @c NULL, the root file system will be mounted.  The @a data
 * parameter will be forwarded to the file system initialization handler.  The
 * file system type is selected by @a filesystemtype and may be one of
 * - RTEMS_FILESYSTEM_TYPE_DEVFS,
 * - RTEMS_FILESYSTEM_TYPE_DOSFS,
 * - RTEMS_FILESYSTEM_TYPE_FTPFS,
 * - RTEMS_FILESYSTEM_TYPE_IMFS,
 * - RTEMS_FILESYSTEM_TYPE_MINIIMFS,
 * - RTEMS_FILESYSTEM_TYPE_NFS,
 * - RTEMS_FILESYSTEM_TYPE_RFS, or
 * - RTEMS_FILESYSTEM_TYPE_TFTPFS.
 *
 * Only configured or registered file system types are available.  You can add
 * file system types to your application configuration with
 * - CONFIGURE_FILESYSTEM_DEVFS,
 * - CONFIGURE_FILESYSTEM_DOSFS,
 * - CONFIGURE_FILESYSTEM_FTPFS,
 * - CONFIGURE_FILESYSTEM_IMFS,
 * - CONFIGURE_FILESYSTEM_MINIIMFS,
 * - CONFIGURE_FILESYSTEM_NFS,
 * - CONFIGURE_FILESYSTEM_RFS, and
 * - CONFIGURE_FILESYSTEM_TFTPFS.
 *
 * @see rtems_filesystem_register() and mount_and_make_target_path().
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The @c errno indicates the error.
 */
int mount(
  const char                 *source,
  const char                 *target,
  const char                 *filesystemtype,
  rtems_filesystem_options_t options,
  const void                 *data
);

/**
 * @brief Mounts a file system and makes the @a target path.
 *
 * The @a target path will be created with rtems_mkdir() and must not be
 * @c NULL.
 *
 * @see mount().
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The @c errno indicates the error.
 */
int mount_and_make_target_path(
  const char                 *source,
  const char                 *target,
  const char                 *filesystemtype,
  rtems_filesystem_options_t options,
  const void                 *data
);

/**
 * @brief Per file system type routine.
 *
 * @see rtems_filesystem_iterate().
 *
 * @retval true Stop the iteration.
 * @retval false Continue the iteration.
 */
typedef bool (*rtems_per_filesystem_routine)(
  const rtems_filesystem_table_t *fs_entry,
  void *arg
);

/**
 * @brief Iterates over all file system types.
 *
 * For each file system type the @a routine will be called with the entry and
 * the @a routine_arg parameter.
 *
 * Do not register or unregister file system types in @a routine.
 *
 * The iteration is protected by the IO library mutex.
 *
 * @retval true Iteration stopped due to @a routine return status.
 * @retval false Iteration through all entries.
 */
bool rtems_filesystem_iterate(
  rtems_per_filesystem_routine routine,
  void *routine_arg
);

/**
 * @brief Per file system mount routine.
 *
 * @see rtems_filesystem_mount_iterate().
 *
 * @retval true Stop the iteration.
 * @retval false Continue the iteration.
 */
typedef bool (*rtems_per_filesystem_mount_routine)(
  const rtems_filesystem_mount_table_entry_t *mt_entry,
  void *arg
);

/**
 * @brief Iterates over all file system mounts.
 *
 * For each file system mount the @a routine will be called with the entry and
 * the @a routine_arg parameter.
 *
 * Do not mount or unmount file systems in @a routine.
 *
 * The iteration is protected by the IO library mutex.
 *
 * @retval true Iteration stopped due to @a routine return status.
 * @retval false Iteration through all entries.
 */
bool
rtems_filesystem_mount_iterate(
  rtems_per_filesystem_mount_routine routine,
  void *routine_arg
);

/**
 * @brief Boot time mount table entry.
 */
typedef struct {
  const char                              *type;
  rtems_filesystem_options_t               fsoptions;
  const char                              *device;
  const char                              *mount_point;
} rtems_filesystem_mount_table_t;

/**
 * @brief Boot time mount table.
 *
 * @todo Only the first entry will be evaluated.  Why do we need a table?
 */
extern const rtems_filesystem_mount_table_t *rtems_filesystem_mount_table;

/**
 * @brief Boot time mount table entry count.
 *
 * @todo Only the first entry will be evaluated.  Why do we need a table?
 */
extern const int rtems_filesystem_mount_table_size;

/** @} */

/**
 * @defgroup Termios Termios
 *
 * @ingroup LibIO
 *
 * @brief Termios
 *
 * @{
 */

typedef struct rtems_termios_callbacks {
  int    (*firstOpen)(int major, int minor, void *arg);
  int    (*lastClose)(int major, int minor, void *arg);
  int    (*pollRead)(int minor);
  ssize_t (*write)(int minor, const char *buf, size_t len);
  int    (*setAttributes)(int minor, const struct termios *t);
  int    (*stopRemoteTx)(int minor);
  int    (*startRemoteTx)(int minor);
  int    outputUsesInterrupts;
} rtems_termios_callbacks;

void rtems_termios_initialize (void);

/*
 * CCJ: Change before opening a tty. Newer code from Eric is coming
 * so extra work to handle an open tty is not worth it. If the tty
 * is open, close then open it again.
 */
rtems_status_code rtems_termios_bufsize (
  size_t cbufsize,     /* cooked buffer size */
  size_t raw_input,    /* raw input buffer size */
  size_t raw_output    /* raw output buffer size */
);

rtems_status_code rtems_termios_open (
  rtems_device_major_number      major,
  rtems_device_minor_number      minor,
  void                          *arg,
  const rtems_termios_callbacks *callbacks
);

rtems_status_code rtems_termios_close(
  void *arg
);

rtems_status_code rtems_termios_read(
  void *arg
);

rtems_status_code rtems_termios_write(
  void *arg
);

rtems_status_code rtems_termios_ioctl(
  void *arg
);

int rtems_termios_enqueue_raw_characters(
  void *ttyp,
  const char *buf,
  int   len
);

int rtems_termios_dequeue_characters(
  void *ttyp,
  int   len
);

/** @} */

/**
 * @brief The pathconf setting for a file system.
 */
#define rtems_filesystem_pathconf(_mte) ((_mte)->pathconf_limits_and_options)

/**
 * @brief The type of file system. Its name.
 */
#define rtems_filesystem_type(_mte) ((_mte)->type)

/**
 * @brief The mount point of a file system.
 */
#define rtems_filesystem_mount_point(_mte) ((_mte)->target)

/**
 * @brief The device entry of a file system.
 */
#define rtems_filesystem_mount_device(_mte) ((_mte)->dev)

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_LIBIO_H */
