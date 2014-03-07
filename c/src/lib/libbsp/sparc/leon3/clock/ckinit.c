/*
 *  Clock Tick Device Driver
 *
 *  This routine initializes LEON timer 1 which used for the clock tick.
 *
 *  The tick frequency is directly programmed to the configured number of
 *  microseconds per tick.
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON3 BSP.
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bspopts.h>
#include <ambapp.h>
#include <rtems/score/profiling.h>

#if SIMSPARC_FAST_IDLE==1
#define CLOCK_DRIVER_USE_FAST_IDLE 1
#endif

/*
 *  The Real Time Clock Counter Timer uses this trap type.
 */

volatile struct gptimer_regs *LEON3_Timer_Regs = 0;
static int clkirq;

#define CLOCK_VECTOR LEON_TRAP_TYPE( clkirq )

static void leon3_clock_profiling_interrupt_delay(void)
{
#ifdef RTEMS_PROFILING
  /*
   * We need a small state machine to ignore the first clock interrupt, since
   * it contains the sequential system initialization time.  Do the timestamp
   * initialization on the fly.
   */
  static int state = 1;

  volatile struct irqmp_timestamp_regs *irqmp_ts =
    &LEON3_IrqCtrl_Regs->timestamp[0];
  unsigned int s1_s2 = (1U << 25) | (1U << 26);

  if (state == 0) {
    unsigned int first = irqmp_ts->assertion;
    unsigned int second = irqmp_ts->counter;

    irqmp_ts->control |= s1_s2;

    _Profiling_Update_max_interrupt_delay(_Per_CPU_Get(), second - first);
  } else if (state == 1 && leon3_irqmp_has_timestamp(irqmp_ts)) {
    unsigned int ks = 1U << 5;

    state = 0;

    irqmp_ts->control = ks | s1_s2 | (unsigned int) clkirq;
  } else if (state == 1) {
    state = 2;
  }
#endif
}

#define Clock_driver_support_at_tick() \
  do { \
    leon3_clock_profiling_interrupt_delay(); \
  } while (0)

#if defined(RTEMS_MULTIPROCESSING)
  #define Adjust_clkirq_for_node() \
    do { \
      if (rtems_configuration_get_user_multiprocessing_table() != NULL) { \
        clkirq += LEON3_Cpu_Index; \
      } \
    } while(0)
#else
  #define Adjust_clkirq_for_node() do { clkirq += LEON3_CLOCK_INDEX; } while(0)
#endif

#define Clock_driver_support_find_timer() \
  do { \
    struct ambapp_dev *adev; \
    \
    /* Find first LEON3 GP Timer */ \
    adev = (void *)ambapp_for_each(&ambapp_plb, (OPTIONS_ALL|OPTIONS_APB_SLVS),\
              VENDOR_GAISLER, GAISLER_GPTIMER, ambapp_find_by_idx, NULL); \
    if (adev) { \
      /* Found APB GPTIMER Timer */ \
      LEON3_Timer_Regs = (volatile struct gptimer_regs *) \
                         DEV_TO_APB(adev)->start; \
      clkirq = (LEON3_Timer_Regs->cfg & 0xf8) >> 3; \
      \
      Adjust_clkirq_for_node(); \
    } \
  } while (0)

#define Clock_driver_support_install_isr( _new, _old ) \
  do { \
    _old = set_vector( _new, CLOCK_VECTOR, 1 ); \
  } while(0)

#define Clock_driver_support_initialize_hardware() \
  do { \
    LEON3_Timer_Regs->timer[LEON3_CLOCK_INDEX].reload = \
      rtems_configuration_get_microseconds_per_tick() - 1; \
    \
    LEON3_Timer_Regs->timer[LEON3_CLOCK_INDEX].ctrl = \
      LEON3_GPTIMER_EN | LEON3_GPTIMER_RL | \
        LEON3_GPTIMER_LD | LEON3_GPTIMER_IRQEN; \
  } while (0)

#define Clock_driver_support_shutdown_hardware() \
  do { \
    LEON_Mask_interrupt(LEON_TRAP_TYPE(clkirq)); \
    LEON3_Timer_Regs->timer[LEON3_CLOCK_INDEX].ctrl = 0; \
  } while (0)

static uint32_t bsp_clock_nanoseconds_since_last_tick(void)
{
  uint32_t clicks;
  uint32_t usecs;

  if ( !LEON3_Timer_Regs )
    return 0;

  clicks = LEON3_Timer_Regs->timer[LEON3_CLOCK_INDEX].value;

  if ( LEON_Is_interrupt_pending( clkirq ) ) {
    clicks = LEON3_Timer_Regs->timer[LEON3_CLOCK_INDEX].value;
    usecs = (2*rtems_configuration_get_microseconds_per_tick() - clicks);
  } else {
    usecs = (rtems_configuration_get_microseconds_per_tick() - clicks);
  }
  return usecs * 1000;
}

#define Clock_driver_nanoseconds_since_last_tick \
        bsp_clock_nanoseconds_since_last_tick

#include "../../../shared/clockdrv_shell.h"
