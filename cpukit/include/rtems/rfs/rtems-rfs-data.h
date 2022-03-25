/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief RTEMS File System Data
 *
 * @ingroup rtems_rfs
 *
 * RTEMS File System Data.
 *
 * Access data in the correct byte order for the specific target we are running
 * on.
 *
 * @todo Make direct access on matching byte ordered targets.
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


#if !defined (_RTEMS_RFS_DATA_H_)
#define _RTEMS_RFS_DATA_H_

#include <stdint.h>

/**
 * Helper function to make sure we have a byte pointer.
 */
#define rtems_rfs_data_ptr(_d) ((uint8_t*)(_d))

/**
 * Helper function to get the data shifted in the correctly sized type.
 */
#define rtems_rfs_data_get(_d, _t, _o, _s) \
  (((_t)(rtems_rfs_data_ptr (_d)[_o])) << (_s))

/**
 * RFS Read Unsigned 8bit Integer
 */
#define rtems_rfs_read_u8(_d) \
  (*rtems_rfs_data_ptr (_d))

/**
 * RFS Read Unsigned 16bit Integer
 */
#define rtems_rfs_read_u16(_d) \
  (rtems_rfs_data_get (_d, uint16_t, 0, 8) | \
   rtems_rfs_data_get (_d, uint16_t, 1, 0))

/**
 * RFS Read Unsigned 32bit Integer
 */
#define rtems_rfs_read_u32(_d) \
  (rtems_rfs_data_get (_d, uint32_t, 0, 24) | \
   rtems_rfs_data_get (_d, uint32_t, 1, 16) | \
   rtems_rfs_data_get (_d, uint32_t, 2, 8) | \
   rtems_rfs_data_get (_d, uint32_t, 3, 0))

/**
 * RFS Write Unsigned 8bit Integer
 */
#define rtems_rfs_write_u8(_d, _v) \
  (*rtems_rfs_data_ptr (_d) = (uint8_t)(_v))

/**
 * RFS Write Unsigned 16bit Integer
 */
#define rtems_rfs_write_u16(_d, _v) \
  do { \
    rtems_rfs_data_ptr (_d)[0] = (uint8_t)(((uint16_t)(_v)) >> 8); \
    rtems_rfs_data_ptr (_d)[1] = (uint8_t)((_v)); \
  } while (0)

/**
 * RFS Write Unsigned 32bit Integer
 */
#define rtems_rfs_write_u32(_d, _v) \
  do { \
    rtems_rfs_data_ptr (_d)[0] = (uint8_t)(((uint32_t)(_v)) >> 24); \
    rtems_rfs_data_ptr (_d)[1] = (uint8_t)(((uint32_t)(_v)) >> 16); \
    rtems_rfs_data_ptr (_d)[2] = (uint8_t)(((uint32_t)(_v)) >> 8);  \
    rtems_rfs_data_ptr (_d)[3] = (uint8_t)((uint32_t)(_v)); \
  } while (0)

#endif
