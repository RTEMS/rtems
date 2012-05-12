/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#define FATAL_ERROR_TEST_NAME            "FATAL 21"
#define FATAL_ERROR_DESCRIPTION          "sbrk during init fails"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_RTEMS_API
#define FATAL_ERROR_EXPECTED_IS_INTERNAL FALSE
#define FATAL_ERROR_EXPECTED_ERROR       RTEMS_NO_MEMORY

#include <rtems/libcsupport.h>
#include <rtems/malloc.h>
#include <unistd.h>

/* Safe information on real heap */
extern rtems_malloc_sbrk_functions_t *rtems_malloc_sbrk_helpers;
extern rtems_malloc_sbrk_functions_t  rtems_malloc_sbrk_helpers_table;

void * sbrk(ptrdiff_t incr)
{
  return (void *) -1;
}

void force_error()
{
  rtems_malloc_sbrk_helpers = &rtems_malloc_sbrk_helpers_table;
 
  RTEMS_Malloc_Initialize( NULL, 0, 64 );
}
