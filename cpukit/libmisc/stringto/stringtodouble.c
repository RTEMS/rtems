/* SPDX-License-Identifier: GPL-2.0-with-RTEMS-exception */

/**
 * @file
 *
 * @ingroup libmisc_conv_help Conversion Helpers
 *
 * @brief Convert String to Double (with validation)
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
 *  Instantiate an error checking wrapper for strtod (double)
 */
#include <rtems/stringto.h>
#define STRING_TO_FLOAT
#define STRING_TO_TYPE   double
#define STRING_TO_NAME   rtems_string_to_double
#define STRING_TO_METHOD strtod
#define STRING_TO_MAX    HUGE_VAL
#define STRING_TO_MIN    ( -HUGE_VAL )
#include "stringto_template.h"
