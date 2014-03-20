/**
 * @file
 *
 * @brief Manages the Trace and Debugging Features of the
 * RTEMS RFS File System
 *
 * @ingroup rtems_rfs
 *
 * RTEMS File Systems Trace manages the trace and debugging features of the
 * RTEMS RFS file system. The design allows all tracing code and strings to be
 * removed from the target code for small footprint systems.
 */

/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */


#if !defined (_RTEMS_RFS_TRACE_H_)
#define _RTEMS_RFS_TRACE_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**
 * Is tracing enabled ?
 */
#if !defined (RTEMS_RFS_TRACE)
#define RTEMS_RFS_TRACE 1
#endif

/**
 * The type of the mask.
 */
typedef uint64_t rtems_rfs_trace_mask;

/**
 * List of tracing bits for the various parts of the file system.
 */
#define RTEMS_RFS_TRACE_ALL                    (0xffffffffffffffffULL)
#define RTEMS_RFS_TRACE_OPEN                   (1ULL << 0)
#define RTEMS_RFS_TRACE_CLOSE                  (1ULL << 1)
#define RTEMS_RFS_TRACE_MUTEX                  (1ULL << 2)
#define RTEMS_RFS_TRACE_BUFFER_OPEN            (1ULL << 3)
#define RTEMS_RFS_TRACE_BUFFER_CLOSE           (1ULL << 4)
#define RTEMS_RFS_TRACE_BUFFER_SYNC            (1ULL << 5)
#define RTEMS_RFS_TRACE_BUFFER_RELEASE         (1ULL << 6)
#define RTEMS_RFS_TRACE_BUFFER_CHAINS          (1ULL << 7)
#define RTEMS_RFS_TRACE_BUFFER_HANDLE_REQUEST  (1ULL << 8)
#define RTEMS_RFS_TRACE_BUFFER_HANDLE_RELEASE  (1ULL << 9)
#define RTEMS_RFS_TRACE_BUFFER_SETBLKSIZE      (1ULL << 10)
#define RTEMS_RFS_TRACE_BUFFERS_RELEASE        (1ULL << 11)
#define RTEMS_RFS_TRACE_BLOCK_FIND             (1ULL << 12)
#define RTEMS_RFS_TRACE_BLOCK_MAP_GROW         (1ULL << 13)
#define RTEMS_RFS_TRACE_BLOCK_MAP_SHRINK       (1ULL << 14)
#define RTEMS_RFS_TRACE_GROUP_OPEN             (1ULL << 15)
#define RTEMS_RFS_TRACE_GROUP_CLOSE            (1ULL << 16)
#define RTEMS_RFS_TRACE_GROUP_BITMAPS          (1ULL << 17)
#define RTEMS_RFS_TRACE_INODE_OPEN             (1ULL << 18)
#define RTEMS_RFS_TRACE_INODE_CLOSE            (1ULL << 19)
#define RTEMS_RFS_TRACE_INODE_LOAD             (1ULL << 20)
#define RTEMS_RFS_TRACE_INODE_UNLOAD           (1ULL << 21)
#define RTEMS_RFS_TRACE_INODE_CREATE           (1ULL << 22)
#define RTEMS_RFS_TRACE_INODE_DELETE           (1ULL << 23)
#define RTEMS_RFS_TRACE_LINK                   (1ULL << 24)
#define RTEMS_RFS_TRACE_UNLINK                 (1ULL << 25)
#define RTEMS_RFS_TRACE_DIR_LOOKUP_INO         (1ULL << 26)
#define RTEMS_RFS_TRACE_DIR_LOOKUP_INO_CHECK   (1ULL << 27)
#define RTEMS_RFS_TRACE_DIR_LOOKUP_INO_FOUND   (1ULL << 28)
#define RTEMS_RFS_TRACE_DIR_ADD_ENTRY          (1ULL << 29)
#define RTEMS_RFS_TRACE_DIR_DEL_ENTRY          (1ULL << 30)
#define RTEMS_RFS_TRACE_DIR_READ               (1ULL << 31)
#define RTEMS_RFS_TRACE_DIR_EMPTY              (1ULL << 32)
#define RTEMS_RFS_TRACE_SYMLINK                (1ULL << 33)
#define RTEMS_RFS_TRACE_SYMLINK_READ           (1ULL << 34)
#define RTEMS_RFS_TRACE_FILE_OPEN              (1ULL << 35)
#define RTEMS_RFS_TRACE_FILE_CLOSE             (1ULL << 36)
#define RTEMS_RFS_TRACE_FILE_IO                (1ULL << 37)
#define RTEMS_RFS_TRACE_FILE_SET               (1ULL << 38)

/**
 * Call to check if this part is bring traced. If RTEMS_RFS_TRACE is defined to
 * 0 the code is dead code elminiated when built with -Os, -O2, or higher.
 *
 * @param[in] mask is the part of the API to trace.
 *
 * @retval true Tracing is active for the mask.
 * @retval false Do not trace.
 */
#if RTEMS_RFS_TRACE
bool rtems_rfs_trace (rtems_rfs_trace_mask mask);
#else
#define rtems_rfs_trace(_m) (0)
#endif

/**
 * Set the mask.
 *
 * @param[in] mask are the mask bits to set.
 *
 * @retval mask The previous mask.
 */
#if RTEMS_RFS_TRACE
rtems_rfs_trace_mask rtems_rfs_trace_set_mask (rtems_rfs_trace_mask mask);
#else
#define rtems_rfs_trace_set_mask(_m)
#endif

/**
 * Clear the mask.
 *
 * @param[in] mask are the mask bits to clear.
 *
 * @retval mask The previous mask.
 */
#if RTEMS_RFS_TRACE
rtems_rfs_trace_mask rtems_rfs_trace_clear_mask (rtems_rfs_trace_mask mask);
#else
#define rtems_rfs_trace_clear_mask(_m)
#endif

/**
 * Add shell trace shell command.
 */
#if RTEMS_RFS_TRACE
int rtems_rfs_trace_shell_command (int argc, char *argv[]);
#endif

#endif
