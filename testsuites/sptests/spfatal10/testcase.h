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

static const char func [] = "Init";

#define FATAL_ERROR_TEST_NAME            "10"
#define FATAL_ERROR_DESCRIPTION          "asserting with non-NULL strings..."
#define FATAL_ERROR_EXPECTED_SOURCE      RTEMS_FATAL_SOURCE_ASSERT
#define FATAL_ERROR_EXPECTED_IS_INTERNAL FALSE
#define FATAL_ERROR_EXPECTED_ERROR       ((rtems_fatal_code) func)

void force_error()
{
  __assert_func( __FILE__, __LINE__, func, "forced" );

  /* we will not run this far */
}
