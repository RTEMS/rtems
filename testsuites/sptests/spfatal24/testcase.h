/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/gxx_wrappers.h>
#include <rtems/malloc.h>

#define FATAL_ERROR_TEST_NAME            "GXX KEY ADD FAILURE"
#define FATAL_ERROR_DESCRIPTION          "GXX KEY ADD FAILURE"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_ERROR       INTERNAL_ERROR_GXX_KEY_ADD_FAILED

void force_error()
{
  pthread_key_t key = -1;

  rtems_gxx_setspecific( &key, NULL );
}
