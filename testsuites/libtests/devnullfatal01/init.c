#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../../sptests/spfatal_support/spfatal.h"

/*
 *  COPYRIGHT (c) 1989-2010, 2017.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#define FATAL_ERROR_TEST_NAME            "/DEV/NULL 1"
#define FATAL_ERROR_DESCRIPTION          "/dev/null already registered"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_RTEMS_API
#define FATAL_ERROR_EXPECTED_ERROR       RTEMS_TOO_MANY

#include <rtems/devnull.h>
#include "tmacros.h"

static void force_error(void)
{
  int status;

  status = rtems_io_register_name( "/dev/null", 0, 0 );
  directive_failed( status, "io register" );

  status = null_initialize( 0, 0, NULL );

  /* A fatal error would be raised in previous call */
  /* we will not run this far */
}

#include "../../sptests/spfatal_support/spfatalimpl.h"
