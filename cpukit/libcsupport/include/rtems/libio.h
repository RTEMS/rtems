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
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
 * @defgroup LibIOFSOps File System Operations
 *
 * @ingroup LibIO
 *
 * @brief File system operations.
 *
 * @{
 */

/**
 * @brief File system node types.
 */
typedef enum {
  RTEMS_FILESYSTEM_DIRECTORY,
  RTEMS_FILESYSTEM_DEVICE,
  RTEMS_FILESYSTEM_HARD_LINK,
  RTEMS_FILESYSTEM_SYM_LINK,
  RTEMS_FILESYSTEM_MEMORY_FILE,
  RTEMS_FILESYSTEM_INVALID_NODE_TYPE
} rtems_filesystem_node_types_t;

/**
 * @brief Locks a file system instance.
 *
 * This lock must allow nesting.
 *
 * @param[in, out] mt_entry The mount table entry of the file system instance.
 *
 * @see rtems_filesystem_default_lock().
 */
typedef void (*rtems_filesystem_mt_entry_lock_t)(
  const rtems_filesystem_mount_table_entry_t *mt_entry
);

/**
 * @brief Unlocks a file system instance.
 *
 * @param[in, out] mt_entry The mount table entry of the file system instance.
 *
 * @see rtems_filesystem_default_unlock().
 */
typedef void (*rtems_filesystem_mt_entry_unlock_t)(
  const rtems_filesystem_mount_table_entry_t *mt_entry
);

/**
 * @brief Path evaluation context.
 */
typedef struct {
  /**
   * The contents of the remaining path to be evaluated.
   */
  const char *path;

  /**
   * The length of the remaining path to be evaluated.
   */
  size_t pathlen;

  /**
   * The contents of the token to be evaluated with respect to the current
   * location.
   */
  const char *token;

  /**
   * The length of the token to be evaluated with respect to the current
   * location.
   */
  size_t tokenlen;

  /**
   * The path evaluation is controlled by the following flags
   *  - RTEMS_FS_PERMS_READ,
   *  - RTEMS_FS_PERMS_WRITE,
   *  - RTEMS_FS_PERMS_EXEC,
   *  - RTEMS_FS_FOLLOW_HARD_LINK,
   *  - RTEMS_FS_FOLLOW_SYM_LINK,
   *  - RTEMS_FS_MAKE,
   *  - RTEMS_FS_EXCLUSIVE,
   *  - RTEMS_FS_ACCEPT_RESIDUAL_DELIMITERS, and
   *  - RTEMS_FS_REJECT_TERMINAL_DOT.
   */
  int flags;

  /**
   * Symbolic link evaluation is a recursive operation.  This field helps to
   * limit the recursion level and thus prevents a stack overflow.  The
   * recursion level is limited by RTEMS_FILESYSTEM_SYMLOOP_MAX.
   */
  int recursionlevel;

  /**
   * This is the current file system location of the evaluation process.
   * Tokens are evaluated with respect to the current location.  The token
   * interpretation may change the current location.  The purpose of the path
   * evaluation is to change the start location into a final current location
   * according to the path.
   */
  rtems_filesystem_location_info_t currentloc;

  /**
   * The location of the root directory of the user environment during the
   * evaluation start.
   */
  rtems_filesystem_global_location_t *rootloc;

  /**
   * The start location of the evaluation process.  The start location my
   * change during symbolic link evaluation.
   */
  rtems_filesystem_global_location_t *startloc;
} rtems_filesystem_eval_path_context_t;

/**
 * @brief Path evaluation.
 *
 * @param[in, out] ctx The path evaluation context.
 *
 * @see rtems_filesystem_default_eval_path().
 */
typedef void (*rtems_filesystem_eval_path_t)(
  rtems_filesystem_eval_path_context_t *ctx
);

/**
 * @brief Creates a new link for the existing file.
 *
 * @param[in] parentloc The location of the parent of the new link.
 * @param[in] targetloc The location of the target file.
 * @param[in] name Name for the new link.
 * @param[in] namelen Length of the name for the new link in characters.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 *
 * @see rtems_filesystem_default_link().
 */
typedef int (*rtems_filesystem_link_t)(
  const rtems_filesystem_location_info_t *parentloc,
  const rtems_filesystem_location_info_t *targetloc,
  const char *name,
  size_t namelen
);

/**
 * @brief Changes the mode of a node.
 *
 * @param[in] loc The location of the node.
 * @param[in] mode The new mode of the node
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 *
 * @see rtems_filesystem_default_fchmod().
 */
typedef int (*rtems_filesystem_fchmod_t)(
  const rtems_filesystem_location_info_t *loc,
  mode_t mode
);

/**
 * @brief Changes owner and group of a node.
 *
 * @param[in] loc The location of the node.
 * @param[in] owner User ID for the node.
 * @param[in] group Group ID for the node.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 *
 * @see rtems_filesystem_default_chown().
 */
typedef int (*rtems_filesystem_chown_t)(
  const rtems_filesystem_location_info_t *loc,
  uid_t owner,
  gid_t group
);

/**
 * @brief Clones a location.
 *
 * The location is initialized with a bitwise copy of an existing location.
 * The caller must ensure that this location is protected from a release during
 * the clone operation.  After a successful clone operation the clone will be
 * added to the location chain of the corresponding mount table entry.
 *
 * @param[in, out] loc Location to clone.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 *
 * @see rtems_filesystem_default_clonenode().
 */
typedef int (*rtems_filesystem_clonenode_t)(
  rtems_filesystem_location_info_t *loc
);

/**
 * @brief Frees the location of a node.
 *
 * @param[in] loc The location of the node.
 *
 * @see rtems_filesystem_default_freenode().
 */
typedef void (*rtems_filesystem_freenode_t)(
  const rtems_filesystem_location_info_t *loc
);

/**
 * @brief Mounts a file system instance in a mount point (directory).
 *
 * The mount point belongs to the file system instance of the handler and is
 * specified by a field of the mount table entry.  The handler must check that
 * the mount point is capable of mounting a file system instance.  This is the
 * last step during the mount process.  The file system instance is fully
 * initialized at this point.
 *
 * @param[in] mt_entry The mount table entry.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 *
 * @see rtems_filesystem_default_mount().
 */
typedef int (*rtems_filesystem_mount_t) (
  rtems_filesystem_mount_table_entry_t *mt_entry
);

/**
 * @brief Initializes a file system instance.
 *
 * This function must initialize the file system root node in the mount table
 * entry.
 *
 * @param[in] mt_entry The mount table entry.
 * @param[in] data The data provided by the user.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 */
typedef int (*rtems_filesystem_fsmount_me_t)(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void *data
);

/**
 * @brief Unmounts a file system instance in a mount point (directory).
 *
 * In case this function is successful the file system instance will be marked
 * as unmounted.  The file system instance will be destroyed when the last
 * reference to it vanishes.
 *
 * @param[in] mt_entry The mount table entry.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 *
 * @see rtems_filesystem_default_unmount().
 */
typedef int (*rtems_filesystem_unmount_t) (
  rtems_filesystem_mount_table_entry_t *mt_entry
);

/**
 * @brief Destroys a file system instance.
 *
 * The mount point node location of the mount table entry is invalid.  This
 * handler must free the file system root location and all remaining resources
 * of the file system instance.
 *
 * @param[in] mt_entry The mount table entry.
 *
 * @see rtems_filesystem_default_fsunmount().
 */
typedef void (*rtems_filesystem_fsunmount_me_t)(
  rtems_filesystem_mount_table_entry_t *mt_entry
);

/**
 * @brief Tests if the node of one location is equal to the node of the other
 * location.
 *
 * The caller ensures that both nodes are within the same file system instance.
 *
 * @param[in] a The one location.
 * @param[in] b The other location.
 *
 * @retval true The nodes of the locations are equal.
 * @retval false Otherwise.
 *
 * @see rtems_filesystem_default_are_nodes_equal().
 */
typedef bool (*rtems_filesystem_are_nodes_equal_t)(
  const rtems_filesystem_location_info_t *a,
  const rtems_filesystem_location_info_t *b
);

/**
 * @brief Returns the node type.
 *
 * @param[in] loc The location of the node.
 *
 * @return Type of the node.
 *
 * @see rtems_filesystem_default_node_type().
 */
typedef rtems_filesystem_node_types_t (*rtems_filesystem_node_type_t)(
  const rtems_filesystem_location_info_t *loc
);

/**
 * @brief Creates a new node.
 *
 * This handler should create a new node according to the parameters.
 *
 * @param[in] parentloc The location of the parent of the new node.
 * @param[in] name Name for the new node.
 * @param[in] namelen Length of the name for the new node in characters.
 * @param[in] mode Mode for the new node.
 * @param[in] dev Optional device identifier for the new node.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 *
 * @see rtems_filesystem_default_mknod().
 */
typedef int (*rtems_filesystem_mknod_t)(
  const rtems_filesystem_location_info_t *parentloc,
  const char *name,
  size_t namelen,
  mode_t mode,
  dev_t dev
);

/**
 * @brief Removes a node.
 *
 * @param[in] parentloc The location of the parent of the node.
 * @param[in] loc The location of the node.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 *
 * @see rtems_filesystem_default_rmnod().
 */
typedef int (*rtems_filesystem_rmnod_t)(
  const rtems_filesystem_location_info_t *parentloc,
  const rtems_filesystem_location_info_t *loc
);

/**
 * @brief Set node access and modification times.
 *
 * @param[in] loc The location of the node.
 * @param[in] actime Access time for the node.
 * @param[in] modtime Modification for the node.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 *
 * @see rtems_filesystem_default_utime().
 */
typedef int (*rtems_filesystem_utime_t)(
  const rtems_filesystem_location_info_t *loc,
  time_t actime,
  time_t modtime
);

/**
 * @brief Makes a symbolic link to a node.
 *
 * @param[in] parentloc The location of the parent of the new symbolic link.
 * @param[in] name Name for the new symbolic link.
 * @param[in] namelen Length of the name for the new symbolic link in
 * characters.
 * @param[in] target Contents for the symbolic link.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 *
 * @see rtems_filesystem_default_symlink().
 */
typedef int (*rtems_filesystem_symlink_t)(
  const rtems_filesystem_location_info_t *parentloc,
  const char *name,
  size_t namelen,
  const char *target
);

/**
 * @brief Reads the contents of a symbolic link.
 *
 * @param[in] loc The location of the symbolic link.
 * @param[out] buf The buffer for the contents.
 * @param[in] bufsize The size of the buffer in characters.
 *
 * @retval non-negative Size of the actual contents in characters.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 *
 * @see rtems_filesystem_default_readlink().
 */
typedef ssize_t (*rtems_filesystem_readlink_t)(
  const rtems_filesystem_location_info_t *loc,
  char *buf,
  size_t bufsize
);

/**
 * @brief Renames a node.
 *
 * @param[in] oldparentloc The location of the parent of the old node.
 * @param[in] oldloc The location of the old node.
 * @param[in] newparentloc The location of the parent of the new node.
 * @param[in] name Name for the new node.
 * @param[in] namelen Length of the name for the new node in characters.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 *
 * @see rtems_filesystem_default_rename().
 */
typedef int (*rtems_filesystem_rename_t)(
  const rtems_filesystem_location_info_t *oldparentloc,
  const rtems_filesystem_location_info_t *oldloc,
  const rtems_filesystem_location_info_t *newparentloc,
  const char *name,
  size_t namelen
);

/**
 * @brief Gets file system information.
 *
 * @param[in] loc The location of a node.
 * @param[out] buf Buffer for file system information.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 *
 * @see rtems_filesystem_default_statvfs().
 */
typedef int (*rtems_filesystem_statvfs_t)(
  const rtems_filesystem_location_info_t *loc,
  struct statvfs *buf
);

/**
 * @brief File system operations table.
 */
struct _rtems_filesystem_operations_table {
  rtems_filesystem_mt_entry_lock_t lock_h;
  rtems_filesystem_mt_entry_unlock_t unlock_h;
  rtems_filesystem_eval_path_t eval_path_h;
  rtems_filesystem_link_t link_h;
  rtems_filesystem_are_nodes_equal_t are_nodes_equal_h;
  rtems_filesystem_node_type_t node_type_h;
  rtems_filesystem_mknod_t mknod_h;
  rtems_filesystem_rmnod_t rmnod_h;
  rtems_filesystem_fchmod_t fchmod_h;
  rtems_filesystem_chown_t chown_h;
  rtems_filesystem_clonenode_t clonenod_h;
  rtems_filesystem_freenode_t freenod_h;
  rtems_filesystem_mount_t mount_h;
  rtems_filesystem_fsmount_me_t fsmount_me_h;
  rtems_filesystem_unmount_t unmount_h;
  rtems_filesystem_fsunmount_me_t fsunmount_me_h;
  rtems_filesystem_utime_t utime_h;
  rtems_filesystem_symlink_t symlink_h;
  rtems_filesystem_readlink_t readlink_h;
  rtems_filesystem_rename_t rename_h;
  rtems_filesystem_statvfs_t statvfs_h;
};

/**
 * @brief File system operations table with default operations.
 */
extern const rtems_filesystem_operations_table
  rtems_filesystem_operations_default;

/**
 * @brief Obtains the IO library mutex.
 *
 * @see rtems_filesystem_mt_entry_lock_t.
 */
void rtems_filesystem_default_lock(
  const rtems_filesystem_mount_table_entry_t *mt_entry
);

/**
 * @brief Releases the IO library mutex.
 *
 * @see rtems_filesystem_mt_entry_unlock_t.
 */
void rtems_filesystem_default_unlock(
  const rtems_filesystem_mount_table_entry_t *mt_entry
);

/**
 * @brief Terminates the path evaluation and replaces the current location with
 * the null location.
 *
 * @see rtems_filesystem_eval_path_t.
 */
void rtems_filesystem_default_eval_path(
  rtems_filesystem_eval_path_context_t *ctx
);

/**
 * @retval -1 Always.  The errno is set to ENOTSUP.
 *
 * @see rtems_filesystem_link_t.
 */
int rtems_filesystem_default_link(
  const rtems_filesystem_location_info_t *parentloc,
  const rtems_filesystem_location_info_t *targetloc,
  const char *name,
  size_t namelen
);

/**
 * @brief Tests if the node access pointer of one location is equal to
 * the node access pointer of the other location.
 *
 * @param[in] a The one location.
 * @param[in] b The other location.
 *
 * @retval true The node access pointers of the locations are equal.
 * @retval false Otherwise.
 *
 * @see rtems_filesystem_are_nodes_equal_t.
 */
bool rtems_filesystem_default_are_nodes_equal(
  const rtems_filesystem_location_info_t *a,
  const rtems_filesystem_location_info_t *b
);

/**
 * @retval RTEMS_FILESYSTEM_INVALID_NODE_TYPE Always.
 *
 * @see rtems_filesystem_node_type_t.
 */
rtems_filesystem_node_types_t rtems_filesystem_default_node_type(
  const rtems_filesystem_location_info_t *pathloc
);

/**
 * @retval -1 Always.  The errno is set to ENOTSUP.
 *
 * @see rtems_filesystem_mknod_t.
 */
int rtems_filesystem_default_mknod(
  const rtems_filesystem_location_info_t *parentloc,
  const char *name,
  size_t namelen,
  mode_t mode,
  dev_t dev
);

/**
 * @retval -1 Always.  The errno is set to ENOTSUP.
 *
 * @see rtems_filesystem_rmnod_t.
 */
int rtems_filesystem_default_rmnod(
  const rtems_filesystem_location_info_t *parentloc,
  const rtems_filesystem_location_info_t *loc
);

/**
 * @retval -1 Always.  The errno is set to ENOTSUP.
 *
 * @see rtems_filesystem_fchmod_t.
 */
int rtems_filesystem_default_fchmod(
  const rtems_filesystem_location_info_t *loc,
  mode_t mode
);

/**
 * @retval -1 Always.  The errno is set to ENOTSUP.
 *
 * @see rtems_filesystem_chown_t.
 */
int rtems_filesystem_default_chown(
  const rtems_filesystem_location_info_t *loc,
  uid_t owner,
  gid_t group
);

/**
 * @retval 0 Always.
 *
 * @see rtems_filesystem_clonenode_t.
 */
int rtems_filesystem_default_clonenode(
  rtems_filesystem_location_info_t *loc
);

/**
 * @see rtems_filesystem_freenode_t.
 */
void rtems_filesystem_default_freenode(
  const rtems_filesystem_location_info_t *loc
);

/**
 * @retval -1 Always.  The errno is set to ENOTSUP.
 *
 * @see rtems_filesystem_mount_t.
 */
int rtems_filesystem_default_mount (
   rtems_filesystem_mount_table_entry_t *mt_entry     /* IN */
);

/**
 * @retval -1 Always.  The errno is set to ENOTSUP.
 *
 * @see rtems_filesystem_fsmount_me_t.
 */
int rtems_filesystem_default_fsmount(
  rtems_filesystem_mount_table_entry_t *mt_entry,     /* IN */
  const void                           *data          /* IN */
);

/**
 * @retval -1 Always.  The errno is set to ENOTSUP.
 *
 * @see rtems_filesystem_unmount_t.
 */
int rtems_filesystem_default_unmount(
  rtems_filesystem_mount_table_entry_t *mt_entry     /* IN */
);

/**
 * @retval -1 Always.  The errno is set to ENOTSUP.
 *
 * @see rtems_filesystem_fsunmount_me_t.
 */
void rtems_filesystem_default_fsunmount(
   rtems_filesystem_mount_table_entry_t *mt_entry    /* IN */
);

/**
 * @retval -1 Always.  The errno is set to ENOTSUP.
 *
 * @see rtems_filesystem_utime_t.
 */
int rtems_filesystem_default_utime(
  const rtems_filesystem_location_info_t *loc,
  time_t actime,
  time_t modtime
);

/**
 * @retval -1 Always.  The errno is set to ENOTSUP.
 *
 * @see rtems_filesystem_symlink_t.
 */
int rtems_filesystem_default_symlink(
  const rtems_filesystem_location_info_t *parentloc,
  const char *name,
  size_t namelen,
  const char *target
);

/**
 * @retval -1 Always.  The errno is set to ENOTSUP.
 *
 * @see rtems_filesystem_readlink_t.
 */
ssize_t rtems_filesystem_default_readlink(
  const rtems_filesystem_location_info_t *loc,
  char *buf,
  size_t bufsize
);

/**
 * @retval -1 Always.  The errno is set to ENOTSUP.
 *
 * @see rtems_filesystem_rename_t.
 */
int rtems_filesystem_default_rename(
  const rtems_filesystem_location_info_t *oldparentloc,
  const rtems_filesystem_location_info_t *oldloc,
  const rtems_filesystem_location_info_t *newparentloc,
  const char *name,
  size_t namelen
);

/**
 * @retval -1 Always.  The errno is set to ENOTSUP.
 *
 * @see rtems_filesystem_statvfs_t.
 */
int rtems_filesystem_default_statvfs(
  const rtems_filesystem_location_info_t *loc,
  struct statvfs *buf
);

/** @} */

/**
 * @defgroup LibIOFSHandler File System Node Handler
 *
 * @ingroup LibIO
 *
 * @brief File system node handler.
 *
 * @{
 */

/**
 * @brief Opens a node.
 *
 * @param[in, out] iop The IO pointer.
 * @param[in] path The path.
 * @param[in] oflag The open flags.
 * @param[in] mode Optional mode for node creation.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 *
 * @see rtems_filesystem_default_open().
 */
typedef int (*rtems_filesystem_open_t)(
  rtems_libio_t *iop,
  const char    *path,
  int            oflag,
  mode_t         mode
);

/**
 * @brief Closes a node.
 *
 * @param[in, out] iop The IO pointer.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 *
 * @see rtems_filesystem_default_close().
 */
typedef int (*rtems_filesystem_close_t)(
  rtems_libio_t *iop
);

/**
 * @brief Reads from a node.
 *
 * This handler is responsible to update the offset field of the IO descriptor.
 *
 * @param[in, out] iop The IO pointer.
 * @param[out] buffer The buffer for read data.
 * @param[in] count The size of the buffer in characters.
 *
 * @retval non-negative Count of read characters.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 *
 * @see rtems_filesystem_default_read().
 */
typedef ssize_t (*rtems_filesystem_read_t)(
  rtems_libio_t *iop,
  void          *buffer,
  size_t         count
);

/**
 * @brief Writes to a node.
 *
 * This handler is responsible to update the offset field of the IO descriptor.
 *
 * @param[in, out] iop The IO pointer.
 * @param[out] buffer The buffer for write data.
 * @param[in] count The size of the buffer in characters.
 *
 * @retval non-negative Count of written characters.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 *
 * @see rtems_filesystem_default_write().
 */
typedef ssize_t (*rtems_filesystem_write_t)(
  rtems_libio_t *iop,
  const void    *buffer,
  size_t         count
);

/**
 * @brief IO control of a node.
 *
 * @param[in, out] iop The IO pointer.
 * @param[in] request The IO control request.
 * @param[in, out] buffer The buffer for IO control request data.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 *
 * @see rtems_filesystem_default_ioctl().
 */
typedef int (*rtems_filesystem_ioctl_t)(
  rtems_libio_t   *iop,
  ioctl_command_t  request,
  void            *buffer
);

/**
 * @brief Moves the read/write file offset.
 *
 * @param[in, out] iop The IO pointer.
 * @param[in] offset The offset.
 * @param[in] whence The reference position for the offset.
 *
 * @retval non-negative The new offset from the beginning of the file.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 *
 * @see rtems_filesystem_default_lseek(),
 * rtems_filesystem_default_lseek_file(), and
 * rtems_filesystem_default_lseek_directory().
 */
typedef off_t (*rtems_filesystem_lseek_t)(
  rtems_libio_t *iop,
  off_t          offset,
  int            whence
);

/**
 * @brief Gets a node status.
 *
 * @param[in, out] iop The IO pointer.
 * @param[out] stat The buffer to status information.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 *
 * @see rtems_filesystem_default_fstat().
 */
typedef int (*rtems_filesystem_fstat_t)(
  const rtems_filesystem_location_info_t *loc,
  struct stat *buf
);

/**
 * @brief Truncates a file to a specified length.
 *
 * @param[in, out] iop The IO pointer.
 * @param[in] length The new length in characters.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 *
 * @see rtems_filesystem_default_ftruncate() and
 * rtems_filesystem_default_ftruncate_directory().
 */
typedef int (*rtems_filesystem_ftruncate_t)(
  rtems_libio_t *iop,
  off_t length
);

/**
 * @brief Synchronizes changes to a file.
 *
 * @param[in, out] iop The IO pointer.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 *
 * @see rtems_filesystem_default_fsync_or_fdatasync() and
 * rtems_filesystem_default_fsync_or_fdatasync_success().
 */
typedef int (*rtems_filesystem_fsync_t)(
  rtems_libio_t *iop
);

/**
 * @brief Synchronizes the data of a file.
 *
 * @param[in, out] iop The IO pointer.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occured.  The errno is set to indicate the error.
 *
 * @see rtems_filesystem_default_fsync_or_fdatasync() and
 * rtems_filesystem_default_fsync_or_fdatasync_success().
 */
typedef int (*rtems_filesystem_fdatasync_t)(
  rtems_libio_t *iop
);

/**
 * @brief File control.
 *
 * @param[in, out] iop The IO pointer.
 * @param[in] cmd Control command.
 *
 * @retval 0 Successful operation.
 * @retval errno An error occured.  This value is assigned to errno.
 *
 * @see rtems_filesystem_default_fcntl().
 */
typedef int (*rtems_filesystem_fcntl_t)(
  rtems_libio_t *iop,
  int cmd
);

/**
 * @brief File system node operations table.
 */
struct _rtems_filesystem_file_handlers_r {
  rtems_filesystem_open_t open_h;
  rtems_filesystem_close_t close_h;
  rtems_filesystem_read_t read_h;
  rtems_filesystem_write_t write_h;
  rtems_filesystem_ioctl_t ioctl_h;
  rtems_filesystem_lseek_t lseek_h;
  rtems_filesystem_fstat_t fstat_h;
  rtems_filesystem_ftruncate_t ftruncate_h;
  rtems_filesystem_fsync_t fsync_h;
  rtems_filesystem_fdatasync_t fdatasync_h;
  rtems_filesystem_fcntl_t fcntl_h;
};

/**
 * @brief File system node handler table with default node handlers.
 */
extern const rtems_filesystem_file_handlers_r
  rtems_filesystem_handlers_default;

/**
 * @retval 0 Always.
 *
 * @see rtems_filesystem_open_t.
 */
int rtems_filesystem_default_open(
  rtems_libio_t *iop,
  const char    *path,
  int            oflag,
  mode_t         mode
);

/**
 * @retval 0 Always.
 *
 * @see rtems_filesystem_close_t.
 */
int rtems_filesystem_default_close(
  rtems_libio_t *iop
);

/**
 * @retval -1 Always.  The errno is set to ENOTSUP.
 *
 * @see rtems_filesystem_read_t.
 */
ssize_t rtems_filesystem_default_read(
  rtems_libio_t *iop,
  void          *buffer,
  size_t         count
);

/**
 * @retval -1 Always.  The errno is set to ENOTSUP.
 *
 * @see rtems_filesystem_write_t.
 */
ssize_t rtems_filesystem_default_write(
  rtems_libio_t *iop,
  const void    *buffer,
  size_t         count
);

/**
 * @retval -1 Always.  The errno is set to ENOTSUP.
 *
 * @see rtems_filesystem_ioctl_t.
 */
int rtems_filesystem_default_ioctl(
  rtems_libio_t   *iop,
  ioctl_command_t  request,
  void            *buffer
);

/**
 * @retval -1 Always.  The errno is set to ESPIPE.
 *
 * @see rtems_filesystem_lseek_t.
 */
off_t rtems_filesystem_default_lseek(
  rtems_libio_t *iop,
  off_t          offset,
  int            whence
);

/**
 * @brief An offset 0 with a whence of SEEK_SET will perform a directory rewind
 * operation.
 *
 * This function has no protection against concurrent access.
 *
 * @retval -1 The offset is not zero or the whence is not SEEK_SET.
 * @retval 0 Successful rewind operation.
 *
 * @see rtems_filesystem_lseek_t.
 */
off_t rtems_filesystem_default_lseek_directory(
  rtems_libio_t *iop,
  off_t offset,
  int whence
);

/**
 * @brief Default lseek() handler for files.
 *
 * The fstat() handler will be used to obtain the file size in case whence is
 * SEEK_END.
 *
 * This function has no protection against concurrent access.
 *
 * @retval -1 An error occured.  In case an integer overflow occured, then the
 * errno will be set to EOVERFLOW.  In case the new offset is negative, then
 * the errno will be set to EINVAL.  In case the whence is SEEK_END and the
 * fstat() handler to obtain the current file size returned an error status,
 * then the errno will be set by the fstat() handler.
 * @retval offset The new offset.
 *
 * @see rtems_filesystem_lseek_t.
 */
off_t rtems_filesystem_default_lseek_file(
  rtems_libio_t *iop,
  off_t offset,
  int whence
);

/**
 * @brief Sets the mode to S_IRWXU | S_IRWXG | S_IRWXO.
 *
 * @retval 0 Always.
 *
 * @see rtems_filesystem_fstat_t.
 */
int rtems_filesystem_default_fstat(
  const rtems_filesystem_location_info_t *loc,
  struct stat *buf
);

/**
 * @retval -1 Always.  The errno is set to ENOTSUP.
 *
 * @see rtems_filesystem_ftruncate_t.
 */
int rtems_filesystem_default_ftruncate(
  rtems_libio_t *iop,
  off_t length
);

/**
 * @retval -1 Always.  The errno is set to EISDIR.
 *
 * @see rtems_filesystem_ftruncate_t.
 */
int rtems_filesystem_default_ftruncate_directory(
  rtems_libio_t *iop,
  off_t length
);

/**
 * @retval -1 Always.  The errno is set to EINVAL.
 *
 * @see rtems_filesystem_fsync_t and rtems_filesystem_fdatasync_t.
 */
int rtems_filesystem_default_fsync_or_fdatasync(
  rtems_libio_t *iop
);

/**
 * @retval 0 Always.
 *
 * @see rtems_filesystem_fsync_t and rtems_filesystem_fdatasync_t.
 */
int rtems_filesystem_default_fsync_or_fdatasync_success(
  rtems_libio_t *iop
);

/**
 * @retval 0 Always.
 *
 * @see rtems_filesystem_fcntl_t.
 */
int rtems_filesystem_default_fcntl(
  rtems_libio_t *iop,
  int cmd
);

/** @} */

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
extern const rtems_filesystem_limits_and_options_t
  rtems_filesystem_default_pathconf;

/**
 * @brief An open file data structure.
 *
 * It will be indexed by 'fd'.
 *
 * @todo Should really have a separate per/file data structure that this points
 * to (eg: offset, driver, pathname should be in that)
 */
struct rtems_libio_tt {
  rtems_driver_name_t                    *driver;
  off_t                                   offset;    /* current offset into file */
  uint32_t                                flags;
  rtems_filesystem_location_info_t        pathinfo;
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
  ioctl_command_t         command;
  void                   *buffer;
  int                     ioctl_return;
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
#define RTEMS_FS_PERMS_READ 0x4
#define RTEMS_FS_PERMS_WRITE 0x2
#define RTEMS_FS_PERMS_EXEC 0x1
#define RTEMS_FS_PERMS_RWX \
  (RTEMS_FS_PERMS_READ | RTEMS_FS_PERMS_WRITE | RTEMS_FS_PERMS_EXEC)
#define RTEMS_FS_FOLLOW_HARD_LINK 0x8
#define RTEMS_FS_FOLLOW_SYM_LINK 0x10
#define RTEMS_FS_FOLLOW_LINK \
  (RTEMS_FS_FOLLOW_HARD_LINK | RTEMS_FS_FOLLOW_SYM_LINK)
#define RTEMS_FS_MAKE 0x20
#define RTEMS_FS_EXCLUSIVE 0x40
#define RTEMS_FS_ACCEPT_RESIDUAL_DELIMITERS 0x80
#define RTEMS_FS_REJECT_TERMINAL_DOT 0x100

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
  rtems_chain_node                       mt_node;
  void                                  *fs_info;
  const rtems_filesystem_operations_table *ops;
  const void                            *immutable_fs_info;
  rtems_chain_control                    location_chain;
  rtems_filesystem_global_location_t    *mt_point_node;
  rtems_filesystem_global_location_t    *mt_fs_root;
  bool                                   mounted;
  bool                                   writeable;
  const rtems_filesystem_limits_and_options_t *pathconf_limits_and_options;

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

  /**
   * The task that initiated the unmount process.  After unmount process
   * completion this task will be notified via the
   * @ref RTEMS_FILESYSTEM_UNMOUNT_EVENT.
   */
  rtems_id                               unmount_task;
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

extern rtems_chain_control rtems_filesystem_mount_table;

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
 * The function waits for the unmount process completion.  In case the calling
 * thread uses resources of the unmounted file system the function may never
 * return.  In case the calling thread has its root or current directory in the
 * unmounted file system the function returns with an error status and errno is
 * set to EBUSY.
 *
 * The unmount process completion notification uses the RTEMS classic API
 * event @ref RTEMS_FILESYSTEM_UNMOUNT_EVENT.  It is a fatal error to terminate
 * the calling thread while waiting for this event.
 *
 * A concurrent unmount request for the same file system instance has
 * unpredictable effects.
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
 * @brief Mount table entry visitor.
 *
 * @retval true Stop the iteration.
 * @retval false Continue the iteration.
 *
 * @see rtems_filesystem_mount_iterate().
 */
typedef bool (*rtems_filesystem_mt_entry_visitor)(
  const rtems_filesystem_mount_table_entry_t *mt_entry,
  void *arg
);

/**
 * @brief Iterates over all file system mount entries.
 *
 * The iteration is protected by the IO library mutex.  Do not mount or unmount
 * file systems in the visitor function.
 *
 * @param[in] visitor For each file system mount entry the visitor function
 * will be called with the entry and the visitor argument as parameters.
 * @param[in] visitor_arg The second parameter for the visitor function.
 *
 * @retval true Iteration stopped due to visitor function return status.
 * @retval false Iteration through all entries.
 */
bool rtems_filesystem_mount_iterate(
  rtems_filesystem_mt_entry_visitor visitor,
  void *visitor_arg
);

typedef struct {
  const char *source;
  const char *target;
  const char *filesystemtype;
  rtems_filesystem_options_t options;
  const void *data;
} rtems_filesystem_mount_configuration;

extern const rtems_filesystem_mount_configuration
  rtems_filesystem_root_configuration;

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
