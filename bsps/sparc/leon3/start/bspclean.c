/**
 * @file
 * @ingroup sparc_leon3
 * @brief LEON3 BSP fatal extension
 *
 *  Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 *  COPYRIGHT (c) 2014
 *  Aeroflex Gaisler
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <rtems/bspIo.h>
#include <rtems/score/smpimpl.h>

void bsp_fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code code
)
{
  /* On SMP we must wait for all other CPUs not requesting a fatal halt, they
   * are responding to another CPU's fatal request. These CPUs goes into
   * power-down. The CPU requesting fatal halt waits for the others and then
   * handles the system shutdown via the normal procedure.
   */
  #ifdef RTEMS_SMP
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

  #if (BSP_PRINT_EXCEPTION_CONTEXT)
    if ( source == RTEMS_FATAL_SOURCE_EXCEPTION ) {
      rtems_exception_frame_print( (const rtems_exception_frame *) code );
    }
  #endif

  /*
   *  If user wants to implement custom reset/reboot it can be done here
   */
  #if (BSP_RESET_BOARD_AT_EXIT)
    bsp_reset();
  #endif
}
