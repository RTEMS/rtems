/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

const char rtems_test_name[] = "SP INTERRUPT ERROR 1";

rtems_task Init(
  rtems_task_argument argument
)
{
  TEST_BEGIN();
  #if (CPU_SIMPLE_VECTORED_INTERRUPTS == FALSE)
    puts(
      "TA1 - rtems_interrupt_catch - "
      "bad handler RTEMS_INVALID_ADDRESS -- SKIPPED"
    );
    puts(
      "TA1 - rtems_interrupt_catch - "
      "old isr RTEMS_INVALID_ADDRESS - SKIPPED" );
  #else
    rtems_status_code status;

    rtems_isr_entry   old_service_routine;
      status = rtems_interrupt_catch(
        Service_routine,
        CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER + 1,
        &old_service_routine
      );
      fatal_directive_status(
        status,
        RTEMS_INVALID_NUMBER,
        "rtems_interrupt_catch with invalid vector"
      );
      puts( "TA1 - rtems_interrupt_catch - RTEMS_INVALID_NUMBER" );

      status = rtems_interrupt_catch( NULL, 3, &old_service_routine );
      fatal_directive_status(
        status,
        RTEMS_INVALID_ADDRESS,
        "rtems_interrupt_catch with invalid handler"
      );
      puts( "TA1 - rtems_interrupt_catch - bad handler RTEMS_INVALID_ADDRESS" );

      status = rtems_interrupt_catch( Service_routine, 3, NULL );
      fatal_directive_status(
        status,
        RTEMS_INVALID_ADDRESS,
        "rtems_interrupt_catch with invalid old isr pointer"
      );
      puts( "TA1 - rtems_interrupt_catch - old isr RTEMS_INVALID_ADDRESS" );
  #endif

  TEST_END();
}
