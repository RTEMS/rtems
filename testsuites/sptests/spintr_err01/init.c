/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
