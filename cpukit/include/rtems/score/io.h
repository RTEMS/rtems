/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreIO
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreIO.
 */

/*
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_SCORE_IO_H
#define _RTEMS_SCORE_IO_H

#include <rtems/score/basedefs.h>

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSScoreIO IO Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the IO Handler implementation.
 *
 * @{
 */

typedef void ( *IO_Put_char )( int c, void *arg );

int _IO_Printf(
  IO_Put_char  put_char,
  void        *arg,
  char const  *fmt,
  ...
) RTEMS_PRINTFLIKE( 3, 4 );

int _IO_Vprintf(
  IO_Put_char  put_char,
  void        *arg,
  char const  *fmt,
  va_list      ap
);

/**
 * @brief Outputs the source buffer in base64 encoding.
 *
 * After word length of output characters produced by the encoding a word break
 * is produced.
 *
 * @param put_char is the put character function used to output the encoded
 *   source buffer.
 *
 * @param arg is the argument passed to the put character function.
 *
 * @param src is the pointer to the source buffer begin.
 *
 * @param srclen is the length of the source buffer in bytes.
 *
 * @param wordbreak is the word break string.
 *
 * @param wordlen is the word length in bytes.  If the word length is less than
 *   four, then a word length of four will be used.
 *
 * @return Returns the count of output characters.
 */
int _IO_Base64(
  IO_Put_char  put_char,
  void        *arg,
  const void  *src,
  size_t       len,
  const char  *wordbreak,
  int          wordlen
);

/**
 * @brief Outputs the source buffer in base64url encoding.
 *
 * After word length of output characters produced by the encoding a word break
 * is produced.
 *
 * @param put_char is the put character function used to output the encoded
 *   source buffer.
 *
 * @param arg is the argument passed to the put character function.
 *
 * @param src is the pointer to the source buffer begin.
 *
 * @param srclen is the length of the source buffer in bytes.
 *
 * @param wordbreak is the word break string.
 *
 * @param wordlen is the word length in bytes.  If the word length is less than
 *   four, then a word length of four will be used.
 *
 * @return Returns the count of output characters.
 */
int _IO_Base64url(
  IO_Put_char  put_char,
  void        *arg,
  const void  *src,
  size_t       len,
  const char  *wordbreak,
  int          wordlen
);

/**
 * @brief Issues a couple of no-operation instructions.
 *
 * This function may be used to burn a couple of processor cycles with minimum
 * impact on the system bus.  It may be used in busy wait loops.
 */
void _IO_Relax( void );

/** @} */ 

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_IO_H */
