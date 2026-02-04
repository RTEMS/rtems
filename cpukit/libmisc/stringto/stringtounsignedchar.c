/* SPDX-License-Identifier: GPL-2.0-with-RTEMS-exception */

/**
 * @file
 *
 * @ingroup libmisc_conv_help Conversion Helpers
 *
 * @brief Convert String to Unsigned Character (with validation)
 */

/*
 *  COPYRIGHT (c) 2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

/*
 *  Instantiate an error checking wrapper for strtoul (unsigned char)
 */
#include <rtems/stringto.h>
#define STRING_TO_INTEGER
#define STRING_TO_TYPE        unsigned char
#define STRING_TO_NAME        rtems_string_to_unsigned_char
#define STRING_TO_METHOD      strtoul
#define STRING_TO_MAX         ULONG_MAX
#define STRING_TO_UCHAR_MAX   UCHAR_MAX
#define STRING_TO_RESULT_TYPE unsigned long
#include "stringto_template.h"
