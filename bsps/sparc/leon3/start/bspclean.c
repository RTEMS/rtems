/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup sparc_leon3
 * @brief LEON3 BSP fatal extension
 *
 *  Copyright (c) 2014 embedded brains GmbH & Co. KG
 *
 *  COPYRIGHT (c) 2014
 *  Aeroflex Gaisler
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

#include <bsp.h>
#include <bsp/bootcard.h>
#include <rtems/score/smpimpl.h>

#include <leon.h>

void bsp_fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code code
)
{
#if defined(RTEMS_SMP)
  /*
   * On SMP we must wait for all other CPUs not requesting a fatal halt, they
   * are responding to another CPU's fatal request. These CPUs goes into
   * power-down. The CPU requesting fatal halt waits for the others and then
   * handles the system shutdown via the normal procedure.
   */
  if ((source == RTEMS_FATAL_SOURCE_SMP) &&
      (code == SMP_FATAL_SHUTDOWN_RESPONSE)) {
    leon3_power_down_loop(); /* CPU didn't start shutdown sequence .. */
  } else {
    volatile struct irqmp_regs *irqmp = LEON3_IrqCtrl_Regs;

    if (irqmp != NULL) {
      /*
       * Value was chosen to get something in the magnitude of 1ms on a 200MHz
       * processor.
       */
      uint32_t max_wait = 1234567;
      uint32_t self_cpu = rtems_scheduler_get_processor();
      uint32_t cpu_count = rtems_scheduler_get_processor_maximum();
      uint32_t halt_mask = 0;
      uint32_t i;

      for (i = 0; i < cpu_count; ++i) {
        if ( (i != self_cpu) && _SMP_Should_start_processor( i ) ) {
          halt_mask |= UINT32_C(1) << i;
        }
      }

      /* Wait some time for secondary processors to halt */
      i = 0;
      while ((irqmp->mpstat & halt_mask) != halt_mask && i < max_wait) {
        ++i;
      }
    }
  }
#endif

#if BSP_PRINT_EXCEPTION_CONTEXT
  if ( source == RTEMS_FATAL_SOURCE_EXCEPTION ) {
    rtems_exception_frame_print( (const rtems_exception_frame *) code );
  }
#endif

#if BSP_RESET_BOARD_AT_EXIT
  /* If user wants to implement custom reset/reboot it can be done here */
  bsp_reset();
#endif
}
