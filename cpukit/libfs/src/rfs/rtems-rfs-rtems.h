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
 * RTEMS File System RTEMS Header file.
 *
 * This file is not to be installed. It binds the RFS file system to RTEMS.
 */

#if !defined(RTEMS_RFS_RTEMS_DEFINED)
#define RTEMS_RFS_RTEMS_DEFINED

#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

/**
 * RTEMS RFS RTEMS Error Enable. Set to 1 for printing of errors. Default is off.
 */
#define RTEMS_RFS_RTEMS_ERROR 0

/**
 * RTEMS RFS RTEMS Trace Enable. Set to 1 for printing of errors. Default is off.
 */
#define RTEMS_RFS_RTEMS_TRACE 0

/**
 * If we are not handling errors provide a define that removes the strings from
 * the code.
 */
#if !RTEMS_RFS_RTEMS_ERROR
#define rtems_rfs_rtems_error(_m, _e) \
  (((errno = (_e)) == 0) ? 0 : -1)
#else
/**
 * Take the result code and set errno with it and if non-zero return -1 else
 * return 0.
 *
 * @param what The message to print is the error is not zero.
 * @param error The error code.
 * @retval -1 An error has occured.
 * @retval 0 No error.
 */
int rtems_rfs_rtems_error (const char* mesg, int error);
#endif

/**
 * Trace message defines the RTEMS bindings of the RTEMS RFS. This is a
 * development tool where can edit the values below to control the various trace
 * output.
 */
#define RTEMS_RFS_RTEMS_DEBUG_ALL           (0xffffffff)
#define RTEMS_RFS_RTEMS_DEBUG_ERROR_MSGS    (1 << 0)
#define RTEMS_RFS_RTEMS_DEBUG_EVAL_PATH     (1 << 1)
#define RTEMS_RFS_RTEMS_DEBUG_EVAL_FOR_MAKE (1 << 2)
#define RTEMS_RFS_RTEMS_DEBUG_EVAL_PERMS    (1 << 3)
#define RTEMS_RFS_RTEMS_DEBUG_MKNOD         (1 << 4)
#define RTEMS_RFS_RTEMS_DEBUG_RMNOD         (1 << 5)
#define RTEMS_RFS_RTEMS_DEBUG_LINK          (1 << 6)
#define RTEMS_RFS_RTEMS_DEBUG_UNLINK        (1 << 7)
#define RTEMS_RFS_RTEMS_DEBUG_CHOWN         (1 << 8)
#define RTEMS_RFS_RTEMS_DEBUG_READLINK      (1 << 9)
#define RTEMS_RFS_RTEMS_DEBUG_FCHMOD        (1 << 10)
#define RTEMS_RFS_RTEMS_DEBUG_STAT          (1 << 11)
#define RTEMS_RFS_RTEMS_DEBUG_RENAME        (1 << 12)
#define RTEMS_RFS_RTEMS_DEBUG_DIR_RMNOD     (1 << 13)
#define RTEMS_RFS_RTEMS_DEBUG_FILE_OPEN     (1 << 14)
#define RTEMS_RFS_RTEMS_DEBUG_FILE_CLOSE    (1 << 15)
#define RTEMS_RFS_RTEMS_DEBUG_FILE_READ     (1 << 16)
#define RTEMS_RFS_RTEMS_DEBUG_FILE_WRITE    (1 << 17)
#define RTEMS_RFS_RTEMS_DEBUG_FILE_LSEEK    (1 << 18)
#define RTEMS_RFS_RTEMS_DEBUG_FILE_FTRUNC   (1 << 19)

/**
 * Call to check if this part is bring traced. If RTEMS_RFS_RTEMS_TRACE is
 * defined to 0 the code is dead code elminiated when built with -Os, -O2, or
 * higher.
 *
 * @param mask The part of the API to trace.
 * @retval true Tracing is active for the mask.
 * @retval false Do not trace.
 */
#if RTEMS_RFS_RTEMS_TRACE
bool rtems_rfs_rtems_trace (uint32_t mask);
#else
#define rtems_rfs_rtems_trace(_m) (0)
#endif

/**
 * Set the mask.
 *
 * @param mask The mask bits to set.
 * @return The previous mask.
 */
#if RTEMS_RFS_RTEMS_TRACE
void rtems_rfs_rtems_trace_set_mask (uint32_t mask);
#else
#define rtems_rfs_rtems_trace_set_mask(_m)
#endif

/**
 * Clear the mask.
 *
 * @param mask The mask bits to clear.
 * @return The previous mask.
 */
#if RTEMS_RFS_RTEMS_TRACE
void rtems_rfs_rtems_trace_clear_mask (uint32_t mask);
#else
#define rtems_rfs_rtems_trace_clear_mask(_m)
#endif

/**
 * Add shell trace shell command.
 */
#if RTEMS_RFS_RTEMS_TRACE
int rtems_rfs_rtems_trace_shell_command (int argc, char *argv[]);
#endif

#include <rtems/rfs/rtems-rfs-file-system.h>
#include <rtems/rfs/rtems-rfs-inode.h>
#include <rtems/rfs/rtems-rfs-mutex.h>
#include <rtems/libio_.h>
#include <rtems/fs.h>

/**
 * Private RFS RTEMS Port data.
 */
typedef struct rtems_rfs_rtems_private
{
  /**
   * The access lock.
   */
  rtems_rfs_mutex access;
} rtems_rfs_rtems_private;
/**
 * Return the file system structure given a path location.
 *
 * @param _loc Pointer to the path location.
 * @return rtems_rfs_file_system*
 */
#define rtems_rfs_rtems_pathloc_dev(_loc) \
  ((rtems_rfs_file_system*)((_loc)->mt_entry->fs_info))

/**
 * Set the inode number (ino) into the path location.
 *
 * @param _loc Pointer to the path location.
 * @param _ino The ino to set in the path location.
 */
#define rtems_rfs_rtems_set_pathloc_ino(_loc, _ino) \
  (_loc)->node_access = (void*)((intptr_t)(_ino))

/**
 * Get the inode number (ino) given a path location.
 *
 * @param _loc Pointer to the path location.
 * @return rtems_rfs_ino The inode number in the path location.
 */
#define rtems_rfs_rtems_get_pathloc_ino(_loc) \
  ((rtems_rfs_ino) (intptr_t)((_loc)->node_access))

/**
 * Set the directory offset (doff) into the path location.
 *
 * @param _loc Pointer to the path location.
 * @param _doff The doff to set in the path location.
 */
#define rtems_rfs_rtems_set_pathloc_doff(_loc, _doff) \
  (_loc)->node_access_2 = (void*)((intptr_t)(_doff))

/**
 * Get the directory offset (doff) given a path location.
 *
 * @param _loc Pointer to the path location.
 * @return uin32_t The doff in the path location.
 */
#define rtems_rfs_rtems_get_pathloc_doff(_loc) \
  ((uint32_t) (intptr_t)((_loc)->node_access_2))

/**
 * Get the ino from the I/O pointer.
 *
 * @param _iop The I/O pointer.
 * @return ino
 */
#define rtems_rfs_rtems_get_iop_ino(_iop) \
  ((intptr_t)(_iop)->pathinfo.node_access)

/**
 * Get the file handle from the I/O pointer.
 *
 * @param _iop The I/O pointer.
 * @return filehandle The file handle
 */
#define rtems_rfs_rtems_get_iop_file_handle(_iop) \
  ((rtems_rfs_file_handle*)(_iop)->pathinfo.node_access_2)

/**
 * Set the file handle in the I/O pointer.
 *
 * @param _iop The I/O pointer.
 * @param _fh The file handle.
 */
#define rtems_rfs_rtems_set_iop_file_handle(_iop, _fh) \
  (_iop)->pathinfo.node_access_2 = (_fh)

/**
 * Create the name of the handler's table given the type of handlers.
 *
 * @param _h The name of the handlers.
 * @return label The name of the handler's table.
 */
#define rtems_rfs_rtems_handlers(_h) \
  &rtems_rfs_rtems_ ## _h ## _handlers

/**
 * Set the handlers in the path location based on the mode of the inode.
 *
 * @param loc Pointer to the path location to set the handlers in.
 * @param inode The inode handle to check the mode of for the type of handlers.
 * @retval true The handlers have been set.
 * @retval false There are no handlers for the mode.
 */
bool rtems_rfs_rtems_set_handlers (rtems_filesystem_location_info_t* pathloc,
                                   rtems_rfs_inode_handle*           inode);

/**
 * Convert the system mode flags to inode mode flags.
 *
 * @param mode The system mode flags.
 * @return uint16_t The inode mode flags.
 */
uint16_t rtems_rfs_rtems_imode (mode_t mode);

/**
 * Convert the inode mode flags to system mode flags.
 *
 * @param imode The inode mode flags
 * @return mode_t The system mode flags.
 */
mode_t rtems_rfs_rtems_mode (int imode);

/**
 * Lock the RFS file system.
 */
static inline void
 rtems_rfs_rtems_lock (rtems_rfs_file_system* fs)
{
  rtems_rfs_rtems_private* rtems = rtems_rfs_fs_user (fs);
  rtems_rfs_mutex_lock (&rtems->access);
}

/**
 * Unlock the RFS file system.
 */
static inline void
 rtems_rfs_rtems_unlock (rtems_rfs_file_system* fs)
{
  rtems_rfs_rtems_private* rtems = rtems_rfs_fs_user (fs);
  rtems_rfs_buffers_release (fs);
  rtems_rfs_mutex_unlock (&rtems->access);
}

/**
 * The handlers.
 */
extern const rtems_filesystem_file_handlers_r rtems_rfs_rtems_dir_handlers;
extern const rtems_filesystem_file_handlers_r rtems_rfs_rtems_device_handlers;
extern const rtems_filesystem_file_handlers_r rtems_rfs_rtems_link_handlers;
extern const rtems_filesystem_file_handlers_r rtems_rfs_rtems_file_handlers;

/**
 * The following routine does a stat on a node.
 */
int rtems_rfs_rtems_fstat (const rtems_filesystem_location_info_t* pathloc,
                           struct stat*                            buf);

/**
 * Routine to remove a node from the RFS file system.
 *
 * @param parent_pathloc
 * @param pathloc
 */
int rtems_rfs_rtems_rmnod (const rtems_filesystem_location_info_t* parent_pathloc,
                           const rtems_filesystem_location_info_t* pathloc);

/**
 * The following routine does a sync on an inode node. Currently it flushes
 * everything related to this device.
 *
 * @param iop
 */
int rtems_rfs_rtems_fdatasync (rtems_libio_t* iop);

#endif
