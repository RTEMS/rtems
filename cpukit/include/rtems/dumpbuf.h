/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Print a Memory Buffer
 * 
 * This file defines the interface to the RTEMS methods to print a
 * memory buffer in a style similar to many ROM monitors and debuggers.
 */

/*
 * COPYRIGHT (c) 1997-2011.
 * On-Line Applications Research Corporation (OAR).
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

#ifndef __DUMP_BUFFER_h
#define __DUMP_BUFFER_h

/**
 *  @defgroup libmisc_dumpbuf Print Memory Buffer
 *
 *  @ingroup RTEMSAPIPrintSupport
 */
/**@{*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Print memory buffer.
 *
 * This method prints @a length bytes beginning at @a buffer in
 * a nice format similar to what one would expect from a debugger
 * or ROM monitor.
 *
 * @param[in] buffer is the address of the buffer
 * @param[in] length is the length of the buffer
 */
void rtems_print_buffer(
  const unsigned char *buffer,
  int                  length
);

#ifdef __cplusplus
}
#endif
/**@}*/
#endif
/* end of include file */
