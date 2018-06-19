/*
 * Classic API Init task create failure
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#define FATAL_ERROR_TEST_NAME            "7"
#define FATAL_ERROR_DESCRIPTION \
        "Core Configuration Invalid ISR stack size"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_ERROR       \
          INTERNAL_ERROR_INTERRUPT_STACK_TOO_SMALL

#define CONFIGURE_INTERRUPT_STACK_SIZE CPU_INTERRUPT_STACK_ALIGNMENT

void force_error()
{
  /* we will not run this far */
}
