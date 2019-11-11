#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../spfatal_support/spfatal.h"

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/gxx_wrappers.h>

#define FATAL_ERROR_TEST_NAME            "GXX MUTEX INIT FAILED"
#define FATAL_ERROR_DESCRIPTION          "GXX MUTEX INIT FAILED"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_ERROR       INTERNAL_ERROR_GXX_MUTEX_INIT_FAILED

static void force_error(void)
{
 __gthread_mutex_t mutex;

  while ( true ) {
    rtems_gxx_mutex_init( &mutex );
    rtems_test_assert( mutex != 0 );
  }
}

#include "../spfatal_support/spfatalimpl.h"
