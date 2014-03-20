/**
 * @file rtems/stringto.h
 *
 * @defgroup libmisc_conv_help Conversion Helpers
 *
 * @ingroup libmisc
 * @brief Convert String to Pointer (with validation)
 *
 * This file defines the interface to a set of string conversion helpers.
 */

/*
 * COPYRIGHT (c) 2009-2011.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_STRINGTO_H
#define _RTEMS_STRINGTO_H
/**
 *  @defgroup libmisc_conv_help Conversion Helpers
 *
 *  @ingroup libmisc
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
