/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Basic Filesystem Types
 *
 * This file defines basic filesystem types
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTEMS_FS_H
#define _RTEMS_FS_H

#include <rtems/chain.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  File descriptor Table Information
 */

/* Forward declarations */

/* FIXME: shouldn't this better not be here? */
typedef struct rtems_libio_tt rtems_libio_t;

struct rtems_filesystem_mount_table_entry_tt;
typedef struct rtems_filesystem_mount_table_entry_tt
    rtems_filesystem_mount_table_entry_t;

typedef struct _rtems_filesystem_file_handlers_r
    rtems_filesystem_file_handlers_r;
typedef struct _rtems_filesystem_operations_table
    rtems_filesystem_operations_table;

/**
 * @brief File system location.
 *
 * @ingroup LibIO
 */
typedef struct rtems_filesystem_location_info_tt {
   rtems_chain_node                         mt_entry_node;
   void                                    *node_access;
   void                                    *node_access_2;
   const rtems_filesystem_file_handlers_r  *handlers;
   rtems_filesystem_mount_table_entry_t    *mt_entry;
} rtems_filesystem_location_info_t;

/**
 * @brief Global file system location.
 *
 * @ingroup LibIO
 *
 * The global file system locations are used for
 * - the mount point location in the mount table entry,
 * - the file system root location in the mount table entry,
 * - the root directory location in the user environment, and
 * - the current directory location in the user environment.
 *
 * During the path evaluation global start locations are obtained to ensure
 * that the current file system will be not unmounted in the meantime.
 *
 * To support a release within critical sections of the operating system a
 * deferred release is supported.  This is similar to malloc() and free().
 *
 * @see rtems_filesystem_global_location_obtain() and
 * rtems_filesystem_global_location_release().
 */
typedef struct rtems_filesystem_global_location_t {
  rtems_filesystem_location_info_t location;
  int reference_count;

  /**
   * A release within a critical section of the operating system will add this
   * location to a list of deferred released locations.  This list is processed
   * in the next rtems_filesystem_global_location_obtain() in FIFO order.
   */
  struct rtems_filesystem_global_location_t *deferred_released_next;

  /**
   * A release within a critical section can happen multiple times.  This field
   * counts the deferred releases.
   */
  int deferred_released_count;
} rtems_filesystem_global_location_t;

/*
 * Return the mount table entry for a path location.
 */
#define rtems_filesystem_location_mount(_pl) ((_pl)->mt_entry)

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
