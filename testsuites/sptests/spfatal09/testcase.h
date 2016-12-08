/*
 *  Malloc Initialization Error
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/malloc.h>
#include <rtems/libcsupport.h>

#define FATAL_ERROR_TEST_NAME            "9"
#define FATAL_ERROR_DESCRIPTION          "Bad heap address to malloc"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_ERROR       INTERNAL_ERROR_NO_MEMORY_FOR_HEAP

void force_error()
{
  RTEMS_Malloc_Initialize( NULL, 0, NULL );
  /* we will not run this far */
}
