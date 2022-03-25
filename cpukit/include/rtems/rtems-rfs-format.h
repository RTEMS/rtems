/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtems_rfs
 *
 * @brief RTEMS File System Format
 *
 * This function lets you format a disk in the RFS format.
 */

/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
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

#if !defined (_RTEMS_RFS_FORMAT_H_)
#define _RTEMS_RFS_FORMAT_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/rfs/rtems-rfs-trace.h>

/**
 * RFS File System Configuration data used to format the file system. For
 * default values leave the field set to 0.
 */
typedef struct _rtems_rfs_format_config
{
  /**
   * The size of a block.
   */
  size_t block_size;

  /**
   * The number of blocks in a group.
   */
  size_t group_blocks;

  /**
   * The number of inodes in a group.
   */
  size_t group_inodes;

  /**
   * The percentage overhead allocated to inodes.
   */
  int inode_overhead;

  /**
   * The maximum length of a name.
   */
  size_t max_name_length;

  /**
   * Initialise the inode tables to all ones.
   */
  bool initialise_inodes;

  /**
   * Is the format verbose.
   */
  bool verbose;

} rtems_rfs_format_config;

/**
 * RFS Format command.
 *
 * @param[in] name is the device name to format.
 * @param[in] config is a pointer to the configuration table.
 *
 * @retval -1 Error. See errno.
 * @retval 0 No error. Format successful.
 */
int rtems_rfs_format (const char* name, const rtems_rfs_format_config* config);

#ifdef __cplusplus
}
#endif

#endif
