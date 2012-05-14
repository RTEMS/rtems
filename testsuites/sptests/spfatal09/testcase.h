/*
 *  Malloc Initialization Error
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/malloc.h>
#include <rtems/libcsupport.h>

#define FATAL_ERROR_TEST_NAME            "9"
#define FATAL_ERROR_DESCRIPTION          "Bad heap address to malloc"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_RTEMS_API
#define FATAL_ERROR_EXPECTED_IS_INTERNAL FALSE
#define FATAL_ERROR_EXPECTED_ERROR       RTEMS_NO_MEMORY

char Malloc_Heap[ 1 ] CPU_STRUCTURE_ALIGNMENT;

void force_error()
{
  RTEMS_Malloc_Initialize( Malloc_Heap, sizeof(Malloc_Heap), 0 );

  /* we will not run this far */
}
