/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#define FATAL_ERROR_TEST_NAME            "/DEV/NULL FATAL 01"
#define FATAL_ERROR_DESCRIPTION          "/dev/null already registered"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_RTEMS_API
#define FATAL_ERROR_EXPECTED_IS_INTERNAL FALSE
#define FATAL_ERROR_EXPECTED_ERROR       RTEMS_TOO_MANY

#include <rtems/devnull.h>
#include "tmacros.h"

void force_error()
{
  int status;

  status = rtems_io_register_name( "/dev/null", 0, 0 );
  directive_failed( status, "io register" );

  status = null_initialize( 0, 0, NULL );

  /* A fatal error would be raised in previous call */
  /* we will not run this far */
}

