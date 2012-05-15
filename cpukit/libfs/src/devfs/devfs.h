/**
*  @file  libfs/devfs/devfs.h
*
*  This include file contains all constants and structures associated
*  with the 'device-only' filesystem.
*/

#ifndef _RTEMS_DEVFS_H
#define _RTEMS_DEVFS_H

#include <rtems/libio_.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  This structure define the type of device table
 */
typedef struct {
  /** This member points to device name which is not a null-terminated string */
  const char               *name;
  /** This member is the name length of a device */
  size_t                    namelen;
  /** major number of a device */
  rtems_device_major_number major;
  /** minor number of a device */
  rtems_device_minor_number minor;
  /** device creation mode, only device file can be created */
  mode_t                    mode;
} devFS_node;

typedef struct {
  devFS_node *nodes;
  size_t count;
} devFS_data;

/**
 *  The following defines the device-only filesystem operating
 *  operations.
 */

extern const rtems_filesystem_operations_table devFS_ops;

/**
 *  The following defines the device-only filesystem operating
 *  handlers.
 */

extern const rtems_filesystem_file_handlers_r  devFS_file_handlers;

static inline const devFS_data *devFS_get_data(
  const rtems_filesystem_location_info_t *loc
)
{
  return (const devFS_data *) loc->mt_entry->immutable_fs_info;
}

extern void devFS_eval_path(
  rtems_filesystem_eval_path_context_t *ctx
);

/**
 *  This handler maps open operation to rtems_io_open.
 *  @param iop This is the RTEMS's internal representation of file.
 *  @param pathname a null-terminated string that starts with /dev.
 *  @param flag access flags
 *  @param mode access mode
 *  @retval the same as open
 */

extern int devFS_open(
  rtems_libio_t *iop,
  const char    *pathname,
  int            oflag,
  mode_t         mode
);


/**
 *  This handler maps close operation to rtems_io_close.
 *  @param iop This is the RTEMS's internal representation of file
 *  @retval the same as close
 */


extern int devFS_close(
  rtems_libio_t *iop
);


/**
 *  This handler maps read operation to rtems_io_read.
 *  @param iop This is the RTEMS's internal representation of file
 *  @param  buffer memory location to store read data
 *  @param  count  how many bytes to read
 *  @retval On successful, this routine returns total bytes read. On error
 *  it returns -1 and errno is set to proper value.
 */

extern ssize_t devFS_read(
  rtems_libio_t *iop,
  void          *buffer,
  size_t         count
);


/**
 *  This handler maps write operation to rtems_io_write.
 *  @param iop This is the RTEMS's internal representation of file
 *  @param buffer data to be written
 *  @param count  how many bytes to write
 *  @retval On successful, this routine returns total bytes written. On error
 *  it returns -1 and errno is set to proper value.
 */

extern ssize_t devFS_write(
  rtems_libio_t *iop,
  const void    *buffer,
  size_t         count
);


/**
 *  This handler maps ioctl operation to rtems_io_ioctl.
 *  @param iop This is the RTEMS's internal representation of file
 *  @param command io control command
 *  @param buffer  io control parameters
 *  @retval On successful, this routine returns total bytes written. On error
 *  it returns -1 and errno is set to proper value.
 */

extern int devFS_ioctl(
  rtems_libio_t   *iop,
  ioctl_command_t  command,
  void            *buffer
);




/**
 *  This handler gets the device file information. This routine only set the following member of struct stat:
 *  st_dev : device number
 *  st_mode: device file creation mode, only two mode are accepted:
 *           S_IFCHR: character device file
 *           S_IFBLK: block device file
 *  @param loc contains filesystem access information
 *  @param buf buffer to hold the device file's information
 *  @retval On successful, this routine returns 0. On error
 *  it returns -1 and errno is set to proper value.
 */

extern int devFS_stat(
  const rtems_filesystem_location_info_t *loc,
  struct stat *buf
);



/**
 *  This routine is invoked upon determination of a node type.
 *  Since this is a device-only filesystem, so there is only
 *  one node type in the system.
 *
 *  @param loc contains filesytem access information, this
 *         parameter is ignored
 *  @retval always returns RTEMS_FILESYSTEM_DEVICE
 */

extern rtems_filesystem_node_types_t devFS_node_type(
  const rtems_filesystem_location_info_t*loc
);

/**
 *  This routine is invoked upon registration of a new device
 *  file. It is responsible for creating a item in the main
 *  device table. This routine searches the device table in
 *  sequential order, when found a empty slot, it fills the slot
 *  with proper values.
 *
 *  @see rtems_filesystem_mknod_t.
 */

extern int devFS_mknod(
  const rtems_filesystem_location_info_t *parentloc,
  const char *name,
  size_t namelen,
  mode_t mode,
  dev_t dev
);


/**
 *  This routine is invoked upon rtems filesystem initialization.
 *  It is responsible for creating the main device table,
 *  initializing it to a known state, and set device file operation
 *  handlers. After this, the device-only filesytem is ready for use
 *
 *  @param  mt_entry The filesystem mount table entry.
 *  @param  data Filesystem specific data.
 *  @retval upon success, this routine returns 0; otherwise it returns
 *  -1 and errno is set to proper value. The only error is when malloc
 *  failed, and errno is set to NOMEM.
 */

extern int devFS_initialize(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void                           *data
);


/**
 *  This routine retrieves all the device registered in system, and
 *  prints out their detail information. For example, on one system,
 *  devFS_show will print out following message:
 *
 *  /dev/console     0  0
 *  /dev/clock       1  0
 *  /dev/tty0        0  0
 *  /flash           2  0
 *
 *  This routine is intended for debugging, and can be used by shell
 *  program to provide user with the system information.
 */

extern void devFS_Show(void);

#ifdef __cplusplus
}
#endif

#endif

