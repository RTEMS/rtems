/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup libmisc_conv_help
 *
 * @brief Convert String to Pointer (with validation)
 *
 * This file defines the interface to a set of string conversion helpers.
 */

/*
 * COPYRIGHT (c) 2009-2011.
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

#ifndef _RTEMS_STRINGTO_H
#define _RTEMS_STRINGTO_H
/**
 *  @defgroup libmisc_conv_help Conversion Helpers
 *
 *  @ingroup RTEMSAPIClassic
 */
/**@{*/

#include <rtems.h>

/**
 * @brief Convert String to Pointer (with validation).
 *
 * This method converts a string to a pointer (void *) with
 * basic numeric validation.
 *
 * @param[in] s is the string to convert
 * @param[in] n points to the variable to place the converted output in
 * @param[in] endptr is used to keep track of the position in the string
 *
 * @retval This method returns RTEMS_SUCCESSFUL on successful conversion
 *         and *n is filled in. Otherwise, the status indicates the
 *         source of the error.
 */
rtems_status_code rtems_string_to_pointer(
  const char     *s,
  void          **n,
  char          **endptr
);

/**
 * @brief Convert String to Unsigned Character (with validation).
 *
 * This method converts a string to an unsigned character with
 * range validation.
 *
 * @param[in] s is the string to convert
 * @param[in] n points to the variable to place the converted output in
 * @param[in] endptr is used to keep track of the position in the string
 * @param[in] base is the expected base of the number
 *
 * @retval This method returns RTEMS_SUCCESSFUL on successful conversion
 *         and *n is filled in. Otherwise, the status indicates the
 *         source of the error.
 */
rtems_status_code rtems_string_to_unsigned_char(
  const char     *s,
  unsigned char  *n,
  char          **endptr,
  int             base
);

/**
 * @brief Convert String to Int (with validation).
 *
 * This method converts a string to an int with range validation.
 *
 * @param[in] s is the string to convert
 * @param[in] n points to the variable to place the converted output in
 * @param[in] endptr is used to keep track of the position in the string
 * @param[in] base is the expected base of the number
 *
 * @retval This method returns RTEMS_SUCCESSFUL on successful conversion
 *         and *n is filled in. Otherwise, the status indicates the
 *         source of the error.
 */
rtems_status_code rtems_string_to_int(
  const char  *s,
  int         *n,
  char       **endptr,
  int          base
);

/**
 * @brief Convert String to Unsigned Int (with validation).
 *
 * This method converts a string to an unsigned int with range validation.
 *
 * @param[in] s is the string to convert
 * @param[in] n points to the variable to place the converted output in
 * @param[in] endptr is used to keep track of the position in the string
 * @param[in] base is the expected base of the number
 *
 * @retval This method returns RTEMS_SUCCESSFUL on successful conversion
 *         and *n is filled in. Otherwise, the status indicates the
 *         source of the error.
 */
rtems_status_code rtems_string_to_unsigned_int(
  const char    *s,
  unsigned int  *n,
  char         **endptr,
  int            base
);

/**
 * @brief Convert String to Long (with validation).
 *
 * This method converts a string to a long with
 * range validation.
 *
 * @param[in] s is the string to convert
 * @param[in] n points to the variable to place the converted output in
 * @param[in] endptr is used to keep track of the position in the string
 * @param[in] base is the expected base of the number
 *
 * @retval This method returns RTEMS_SUCCESSFUL on successful conversion
 *         and *n is filled in. Otherwise, the status indicates the
 *         source of the error.
 */
rtems_status_code rtems_string_to_long(
  const char  *s,
  long        *n,
  char       **endptr,
  int          base
);

/**
 * @brief Convert String to Unsigned Long (with validation).
 *
 * This method converts a string to an unsigned long with
 * range validation.
 *
 * @param[in] s is the string to convert
 * @param[in] n points to the variable to place the converted output in
 * @param[in] endptr is used to keep track of the position in the string
 * @param[in] base is the expected base of the number
 *
 * @retval This method returns RTEMS_SUCCESSFUL on successful conversion
 *         and *n is filled in. Otherwise, the status indicates the
 *         source of the error.
 */
rtems_status_code rtems_string_to_unsigned_long(
  const char     *s,
  unsigned long  *n,
  char          **endptr,
  int             base
);

/**
 * @brief Convert String to Long Long (with validation).
 *
 * This method converts a string to a long long with
 * range validation.
 *
 * @param[in] s is the string to convert
 * @param[in] n points to the variable to place the converted output in
 * @param[in] endptr is used to keep track of the position in the string
 * @param[in] base is the expected base of the number
 *
 * @retval This method returns RTEMS_SUCCESSFUL on successful conversion
 *         and *n is filled in. Otherwise, the status indicates the
 *         source of the error.
 */
rtems_status_code rtems_string_to_long_long(
  const char  *s,
  long long   *n,
  char       **endptr,
  int          base
);

/**
 * @brief Convert String to Unsigned Long Long (with validation).
 *
 * This method converts a string to an unsigned character with
 * range validation.
 *
 * @param[in] s is the string to convert
 * @param[in] n points to the variable to place the converted output in
 * @param[in] endptr is used to keep track of the position in the string
 * @param[in] base is the expected base of the number
 *
 * @retval This method returns RTEMS_SUCCESSFUL on successful conversion
 *         and *n is filled in. Otherwise, the status indicates the
 *         source of the error.
 */
rtems_status_code rtems_string_to_unsigned_long_long(
  const char           *s,
  unsigned long long   *n,
  char                **endptr,
  int                   base
);

/**
 * @brief Convert String to Float (with validation).
 *
 * This method converts a string to a float with range validation.
 *
 * @param[in] s is the string to convert
 * @param[in] n points to the variable to place the converted output in
 * @param[in] endptr is used to keep track of the position in the string
 *
 * @retval This method returns RTEMS_SUCCESSFUL on successful conversion
 *         and *n is filled in. Otherwise, the status indicates the
 *         source of the error.
 */
rtems_status_code rtems_string_to_float(
  const char   *s,
  float        *n,
  char        **endptr
);

/**
 * @brief Convert String to Double (with validation).
 *
 * This method converts a string to a double with range validation.
 *
 * @param[in] s is the string to convert
 * @param[in] n points to the variable to place the converted output in
 * @param[in] endptr is used to keep track of the position in the string
 *
 * @retval This method returns RTEMS_SUCCESSFUL on successful conversion
 *         and *n is filled in. Otherwise, the status indicates the
 *         source of the error.
 */
rtems_status_code rtems_string_to_double(
  const char   *s,
  double       *n,
  char        **endptr
);

/**
 * @brief Convert String to long double (with validation).
 *
 * This method converts a string to a long double with range validation.
 *
 * @param[in] s is the string to convert
 * @param[in] n points to the variable to place the converted output in
 * @param[in] endptr is used to keep track of the position in the string
 *
 * @retval This method returns RTEMS_SUCCESSFUL on successful conversion
 *         and *n is filled in. Otherwise, the status indicates the
 *         source of the error.
 */
rtems_status_code rtems_string_to_long_double(
  const char   *s,
  long double  *n,
  char        **endptr
);

#endif
/**@}*/
