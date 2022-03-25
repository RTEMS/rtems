/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2012, 2018 Chris Johns <chrisj@rtems.org>
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
/**
 * @file
 *
 * @ingroup rtems_rtl
 *
 * @brief RTEMS Run-Time Linker Object File Compression manages a
 *        compressed stream of data.
 *
 * This is a simple interface to the object file cache to stream data from
 * from a compressed object file. There is no ability to seek with the
 * data from a compressed file. The module exists to allocate the output
 * buffer when the loader starts and use the cache buffers will have been
 * allocated.
 */

#if !defined (_RTEMS_RTL_OBJ_COMP_H_)
#define _RTEMS_RTL_OBJ_COMP_H_

#include <rtems/rtl/rtl-obj-cache.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * The amount of input data read at a time from the file.
 */
#define RTEMS_RTL_DECOMP_INPUT_SIZE (256)

/**
 * The types of supported compression.
 */
#define RTEMS_RTL_COMP_NONE (0)
#define RTEMS_RTL_COMP_LZ77 (1)

/**
 * The compressed file.
 */
typedef struct rtems_rtl_obj_cpmp
{
  rtems_rtl_obj_cache* cache;       /**< The cache provides the input
                                       *   buffer. */
  int                  fd;          /**< The file descriptor. */
  int                  compression; /**< The type of compression. */
  off_t                offset;      /**< The base offset of the buffer. */
  size_t               size;        /**< The size of the output buffer. */
  size_t               level;       /**< The amount of data in the buffer. */
  uint8_t*             buffer;      /**< The buffer */
  uint32_t             read;        /**< The amount of data read. */
} rtems_rtl_obj_comp;

/**
 * Return the input level.
 */
static inline uint32_t rtems_rtl_obj_comp_input (rtems_rtl_obj_comp* comp)
{
  return comp->read;
}

/**
 * Open a compressor allocating the output buffer.
 *
 * @param comp The compressor  to initialise.
 * @param size The size of the compressor's output buffer.
 * @retval true The compressor is open.
 * @retval false The compressor is not open. The RTL error is set.
 */
bool rtems_rtl_obj_comp_open (rtems_rtl_obj_comp*  comp,
                              size_t               size);

/**
 * Close a compressor.
 *
 * @param comp The compressor to close.
 */
void rtems_rtl_obj_comp_close (rtems_rtl_obj_comp* comp);

/**
 * Set the cache and offset in the file the compressed stream starts.
 *
 * @param comp The compressor to set the offset in.
 * @param cache The cache to read the file in by.
 * @param fd The file descriptor. Must be an open file.
 * @param compression The type of compression being streamed.
 * @param offset The offset in the file the compressed stream starts.
 */
void rtems_rtl_obj_comp_set (rtems_rtl_obj_comp*  comp,
                             rtems_rtl_obj_cache* cache,
                             int                  fd,
                             int                  compression,
                             off_t                offset);

/**
 * Read decompressed data. The length contains the amount of data that should
 * be available in the cache and referenced by the buffer handle. It must be
 * less than or equal to the size of the cache. This call will return the
 * amount of data that is available. It can be less than you ask if the offset
 * and size is past the end of the file.
 *
 * @param comp The compressor to read data from.
 * @param buffer The buffer the output is written too.
 * @param length The length of data to read. Can be modified to a
 *               lesser value and true is still returned so check it.
 * @retval true The data referenced is in the cache.
 * @retval false The read failed and the RTL error has been set.
 */
bool rtems_rtl_obj_comp_read (rtems_rtl_obj_comp* comp,
                              void*               buffer,
                              size_t              length);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
