#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../spfatal_support/spfatal.h"

/*  Test __assert_func with NULL function name
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/score/wkspace.h>

#define FATAL_ERROR_TEST_NAME            "12"
#define FATAL_ERROR_DESCRIPTION  \
        "_Heap_Initialize fails during RTEMS initialization"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_ERROR       INTERNAL_ERROR_TOO_LITTLE_WORKSPACE

static void force_error(void)
{
  Heap_Area area = { .begin = NULL, .size = 0 };

  _Workspace_Handler_initialization( &area, 1, NULL );
  /* we will not run this far */
}

#include "../spfatal_support/spfatalimpl.h"
