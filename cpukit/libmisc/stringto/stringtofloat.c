/* SPDX-License-Identifier: GPL-2.0-with-RTEMS-exception */

/**
 * @file
 *
 * @ingroup libmisc_conv_help Conversion Helpers
 *
 * @brief Convert String to Float (with validation)
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
 *  Instantiate an error checking wrapper for strtof (float)
 */
#include <rtems/stringto.h>
#define STRING_TO_FLOAT
#define STRING_TO_TYPE float
#define STRING_TO_NAME rtems_string_to_float
#define STRING_TO_METHOD strtof
#define STRING_TO_MAX HUGE_VALF
#define STRING_TO_MIN ( -HUGE_VALF )
#include "stringto_template.h"
