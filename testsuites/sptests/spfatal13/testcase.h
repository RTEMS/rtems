/*  Test __assert_func with NULL function name
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#define FATAL_ERROR_TEST_NAME            "13"
#define FATAL_ERROR_DESCRIPTION  \
        "Configuration of unlimited and maximum of 0"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_IS_INTERNAL TRUE
#define FATAL_ERROR_EXPECTED_ERROR     INTERNAL_ERROR_UNLIMITED_AND_MAXIMUM_IS_0

/*
 *  Actual object does not matter.  Important thing is zero and unlimited.
 */
#define CONFIGURE_MAXIMUM_PARTITIONS rtems_resource_unlimited(0)

void force_error()
{
  /* we should not run this far */
}
