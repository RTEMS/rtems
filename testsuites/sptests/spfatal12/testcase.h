/*  Test __assert_func with NULL function name
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/score/wkspace.h>

#define FATAL_ERROR_TEST_NAME            "12"
#define FATAL_ERROR_DESCRIPTION  \
        "_Heap_Initialize fails during RTEMS initialization"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_IS_INTERNAL TRUE
#define FATAL_ERROR_EXPECTED_ERROR       INTERNAL_ERROR_TOO_LITTLE_WORKSPACE

void force_error()
{
  Heap_Area area = { .begin = NULL, .size = 0 };

  _Workspace_Handler_initialization( &area, 1, NULL );
  /* we will not run this far */
}
