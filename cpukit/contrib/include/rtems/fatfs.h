/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup FatFS
 *
 * @brief RTEMS FatFS Filesystem Public API
 */

/*
 * Copyright (C) 2025 Sepehr Ganji <sepehrganji79@gmail.com>
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

#ifndef _RTEMS_FATFS_H
#define _RTEMS_FATFS_H

#include <rtems.h>
#include <rtems/libio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup FATFS
 *
 * @{
 */

/**
 * @brief RTEMS FatFS filesystem mount options.
 */
typedef struct {
  /** Currently no options defined */
  int reserved;
} rtems_fatfs_mount_options;

/**
 * @brief Initialize FatFS filesystem.
 *
 * This function is the mount handler for the FatFS filesystem. It is called
 * by the mount() system call when the filesystem type is
 * RTEMS_FILESYSTEM_TYPE_FATFS.
 *
 * @param[in] mt_entry The mount table entry.
 * @param[in] data Mount options (rtems_fatfs_mount_options or NULL).
 *
 * @return 0 on success, -1 on error with errno set.
 */
int rtems_fatfs_initialize(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void                           *data
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_FATFS_H */
