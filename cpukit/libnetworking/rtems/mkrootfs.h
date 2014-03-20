/**
 * @file rtems/mkrootfs.h
 *
 * RTEMS Root FS creation support.
 */

/*
  Copyright Cybertec Pty Ltd, 2000
  All rights reserved Cybertec Pty Ltd, 2000

  COPYRIGHT (c) 1989-1998.
  On-Line Applications Research Corporation (OAR).

  The license and distribution terms for this file may be
  found in the file LICENSE in this distribution or at
  http://www.rtems.org/license/LICENSE.

  This software with is provided ``as is'' and with NO WARRANTY.
*/

#ifndef _RTEMS_MKROOTFS_H
#define _RTEMS_MKROOTFS_H

#include <arpa/inet.h> /* in_addr_t */

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Appends the lines to the a file. Create the file
 *  and builds the path if it does not exist.
 *
 *  @param file
 *  @param omode
 *  @param line_cnt
 *  @param lines
 * 
 *  @return 0 on success, -1 on error
 */

int
rtems_rootfs_file_append (const char *file,
                          mode_t     omode,
                          const int  line_cnt,
                          const char **lines);

/**
 *  @brief Helper for bulding an /etc/hosts file.
 *
 *  @param cip
 *  @param cname
 *  @param dname
 *
 *  @return 0 on success, -1 on error
 */

int
rtems_rootfs_append_host_rec (in_addr_t cip,
                              const char    *cname,
                              const char    *dname);

/**
 * Create a few common directories, plus a:
 * /etc/passwd, /etc/group, /etc/host.conf, and
 * /etc/hosts file.
 *
 * @return 0 on success, -1 on error
 */

int
rtems_create_root_fs ( void );

#ifdef __cplusplus
}
#endif

#endif
