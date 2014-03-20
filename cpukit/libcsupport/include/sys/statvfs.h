/**
 * @file 
 *
 * @brief Interface to the statvfs() Set of API Methods
 *
 * This include file defines the interface to the statvfs() set of
 * API methods. The statvfs as defined by the SUS:
 *
 * - http://www.opengroup.org/onlinepubs/009695399/basedefs/sys/statvfs.h.html
 */

/*
 *  COPYRIGHT (c) 2009 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

/*
 */

#ifndef _SYS_STATVFS_H_
#define _SYS_STATVFS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t fsblkcnt_t;
typedef uint32_t fsfilcnt_t;

struct statvfs
{
  unsigned long f_bsize;   /**< File system block size. */
  unsigned long f_frsize;  /**< Fundamental file system block size. */
  fsblkcnt_t    f_blocks;  /**< Total number of blocks on file system in units
                            * of f_frsize. */
  fsblkcnt_t    f_bfree;   /**< Total number of free blocks. */
  fsblkcnt_t    f_bavail;  /**< Number of free blocks available to
                            * non-privileged process. */
  fsfilcnt_t    f_files;   /**< Total number of file serial numbers. */
  fsfilcnt_t    f_ffree;   /**< Total number of free file serial numbers. */
  fsfilcnt_t    f_favail;  /**< Number of file serial numbers available to
                            * non-privileged process. */
  unsigned long f_fsid;    /**< File system ID. */
  unsigned long f_flag;    /**< Bit mask of f_flag values. */
  unsigned long f_namemax; /**< Maximum filename length. */
};

extern int statvfs(const char *__restrict , struct statvfs *__restrict);
extern int fstatvfs(int, struct statvfs *);

#ifdef __cplusplus
}
#endif

#endif
