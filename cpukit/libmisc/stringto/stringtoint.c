/* SPDX-License-Identifier: GPL-2.0-with-RTEMS-exception */

/**
 * @file
 *
 * @ingroup libmisc_conv_help Conversion Helpers
 *
 * @brief Convert String to Int (with validation)
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
 *  Instantiate an error checking wrapper for strtol (int)
 */
#include <rtems/stringto.h>
#define STRING_TO_INTEGER
#define STRING_TO_TYPE   int
#define STRING_TO_NAME   rtems_string_to_int
#define STRING_TO_METHOD strtol
#define STRING_TO_MAX    INT_MAX
#define STRING_TO_MIN    INT_MIN
#include "stringto_template.h"
