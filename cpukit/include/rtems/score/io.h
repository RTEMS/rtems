/**
 * @file
 *
 * @ingroup RTEMSScoreIO
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreIO.
 */

/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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

/** @} */ 

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_IO_H */
