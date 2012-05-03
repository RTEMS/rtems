/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#define FATAL_ERROR_TEST_NAME            "SHUTDOWN WHEN NOT UP"
#define FATAL_ERROR_DESCRIPTION          "SHUTDOWN WHEN NOT UP"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_IS_INTERNAL TRUE
#define FATAL_ERROR_EXPECTED_ERROR       INTERNAL_ERROR_SHUTDOWN_WHEN_NOT_UP

void force_error()
{
  _System_state_Set( SYSTEM_STATE_SHUTDOWN );
  rtems_shutdown_executive( 0 );
  /* we will not run this far */
}
