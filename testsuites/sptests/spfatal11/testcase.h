/*  Test __assert_func with NULL function name
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <assert.h>

#define FATAL_ERROR_TEST_NAME            "11"
#define FATAL_ERROR_DESCRIPTION          "asserting with non-NULL strings..."
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_RTEMS_API
#define FATAL_ERROR_EXPECTED_IS_INTERNAL FALSE
#define FATAL_ERROR_EXPECTED_ERROR       0

void force_error()
{
  __assert_func( __FILE__, __LINE__, NULL, "forced" );

  /* we will not run this far */
}
