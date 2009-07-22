/*
 *  COPYRIGHT (c) 2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __STRING_TO_A_TYPE_h__
#define __STRING_TO_A_TYPE_h__

/**
 *  @brief Convert String to Unsigned Character (with validation)
 *
 *  This method converts a string to an unsigned character with
 *  range validation.
 *
 *  @param[in] s is the string to convert
 *  @param[in] n points to the variable to place the converted output in
 *  @param[in] endptr is used to keep track of the position in the string
 *  @param[in] base is the expected base of the number
 *
 *  @return This method returns true on successful conversion and *n is
 *          filled in.
 */
bool rtems_string_to_unsigned_char(
  const char     *s,
  unsigned char  *n,
  char          **endptr,
  int             base
);

/**
 *  @brief Convert String to Int (with validation)
 *
 *  This method converts a string to an int with range validation.
 *
 *  @param[in] s is the string to convert
 *  @param[in] n points to the variable to place the converted output in
 *  @param[in] endptr is used to keep track of the position in the string
 *  @param[in] base is the expected base of the number
 *
 *  @return This method returns true on successful conversion and *n is
 *          filled in.
 */
bool rtems_string_to_int(
  const char  *s,
  int         *n,
  char       **endptr,
  int          base
);

/**
 *  @brief Convert String to Long (with validation)
 *
 *  This method converts a string to a long with
 *  range validation.
 *
 *  @param[in] s is the string to convert
 *  @param[in] n points to the variable to place the converted output in
 *  @param[in] endptr is used to keep track of the position in the string
 *  @param[in] base is the expected base of the number
 *
 *  @return This method returns true on successful conversion and *n is
 *          filled in.
 */
bool rtems_string_to_long(
  const char  *s,
  long        *n,
  char       **endptr,
  int          base
);

/**
 *  @brief Convert String to Unsigned Long (with validation)
 *
 *  This method converts a string to an unsigned long with
 *  range validation.
 *
 *  @param[in] s is the string to convert
 *  @param[in] n points to the variable to place the converted output in
 *  @param[in] endptr is used to keep track of the position in the string
 *  @param[in] base is the expected base of the number
 *
 *  @return This method returns true on successful conversion and *n is
 *          filled in.
 */
bool rtems_string_to_unsigned_long(
  const char     *s,
  unsigned long  *n,
  char          **endptr,
  int             base
);

/**
 *  @brief Convert String to Long Long (with validation)
 *
 *  This method converts a string to a long long with
 *  range validation.
 *
 *  @param[in] s is the string to convert
 *  @param[in] n points to the variable to place the converted output in
 *  @param[in] endptr is used to keep track of the position in the string
 *  @param[in] base is the expected base of the number
 *
 *  @return This method returns true on successful conversion and *n is
 *          filled in.
 */
bool rtems_string_to_long_long(
  const char  *s,
  long long   *n,
  char       **endptr,
  int          base
);

/**
 *  @brief Convert String to Unsigned Long Long (with validation)
 *
 *  This method converts a string to an unsigned character with
 *  range validation.
 *
 *  @param[in] s is the string to convert
 *  @param[in] n points to the variable to place the converted output in
 *  @param[in] endptr is used to keep track of the position in the string
 *  @param[in] base is the expected base of the number
 *
 *  @return This method returns true on successful conversion and *n is
 *          filled in.
 */
bool rtems_string_to_unsigned_long_long(
  const char           *s,
  unsigned long long   *n,
  char                **endptr,
  int                   base
);

/**
 *  @brief Convert String to Float (with validation)
 *
 *  This method converts a string to a float with range validation.
 *
 *  @param[in] s is the string to convert
 *  @param[in] n points to the variable to place the converted output in
 *  @param[in] endptr is used to keep track of the position in the string
 *
 *  @return This method returns true on successful conversion and *n is
 *          filled in.
 */
bool rtems_string_to_float(
  const char   *s,
  float        *n,
  char        **endptr
);

/**
 *  @brief Convert String to Double (with validation)
 *
 *  This method converts a string to a double with range validation.
 *
 *  @param[in] s is the string to convert
 *  @param[in] n points to the variable to place the converted output in
 *  @param[in] endptr is used to keep track of the position in the string
 *
 *  @return This method returns true on successful conversion and *n is
 *          filled in.
 */
bool rtems_string_to_double(
  const char   *s,
  double       *n,
  char        **endptr
);

#endif
