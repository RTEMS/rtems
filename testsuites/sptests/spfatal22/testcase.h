/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#define FATAL_ERROR_TEST_NAME            "FATAL 22"
#define FATAL_ERROR_DESCRIPTION          "libio init out of memory"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_RTEMS_API
#define FATAL_ERROR_EXPECTED_IS_INTERNAL FALSE
#define FATAL_ERROR_EXPECTED_ERROR       RTEMS_NO_MEMORY

#include <rtems/libio.h>
#include <rtems/malloc.h>

void force_error()
{
  rtems_heap_greedy_allocate( NULL, 0 );

  rtems_libio_init();
}
