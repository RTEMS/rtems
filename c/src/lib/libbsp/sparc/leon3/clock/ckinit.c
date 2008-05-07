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
 *
 *  $Id$
 */

#include <bsp.h>
#include <bspopts.h>

#if SIMSPARC_FAST_IDLE==1
#define CLOCK_DRIVER_USE_FAST_IDLE
#endif

/*
 *  The Real Time Clock Counter Timer uses this trap type.
 */

extern rtems_configuration_table Configuration;

#define LEON3_CLOCK_INDEX \
  (Configuration.User_multiprocessing_table ? LEON3_Cpu_Index : 0)

volatile LEON3_Timer_Regs_Map *LEON3_Timer_Regs = 0;
static int clkirq;

#define CLOCK_VECTOR LEON_TRAP_TYPE( clkirq )

#define Clock_driver_support_at_tick()

#define Clock_driver_support_find_timer() \
  do { \
    int cnt; \
    amba_apb_device dev; \
    \
    /* Find LEON3 GP Timer */ \
    cnt = amba_find_apbslv(&amba_conf,VENDOR_GAISLER,GAISLER_GPTIMER,&dev); \
    if ( cnt > 0 ){ \
      /* Found APB GPTIMER Timer */ \
      LEON3_Timer_Regs = (volatile LEON3_Timer_Regs_Map *) dev.start; \
      clkirq = (LEON3_Timer_Regs->status & 0xfc) >> 3; \
      \
      if (Configuration.User_multiprocessing_table != NULL) { \
        clkirq += LEON3_Cpu_Index; \
      } \
    } \
  } while (0)

#define Clock_driver_support_install_isr( _new, _old ) \
  do { \
    _old = set_vector( _new, CLOCK_VECTOR, 1 ); \
  } while(0)

#define Clock_driver_support_initialize_hardware() \
  do { \
    LEON3_Timer_Regs->timer[LEON3_CLOCK_INDEX].reload = \
      BSP_Configuration.microseconds_per_tick - 1; \
    \
    LEON3_Timer_Regs->timer[LEON3_CLOCK_INDEX].conf = \
      LEON3_GPTIMER_EN | LEON3_GPTIMER_RL | \
        LEON3_GPTIMER_LD | LEON3_GPTIMER_IRQEN; \
  } while (0)

#define Clock_driver_support_shutdown_hardware() \
  do { \
    LEON_Mask_interrupt(LEON_TRAP_TYPE(clkirq)); \
    LEON3_Timer_Regs->timer[LEON3_CLOCK_INDEX].conf = 0; \
  } while (0)

uint32_t bsp_clock_nanoseconds_since_last_tick(void)
{
  uint32_t clicks;
  if ( !LEON3_Timer_Regs )
    return 0;

  clicks = LEON3_Timer_Regs->timer[0].value;

  /* Down counter */
  return (uint32_t) (BSP_Configuration.microseconds_per_tick - clicks) * 1000;
}

#define Clock_driver_nanoseconds_since_last_tick bsp_clock_nanoseconds_since_last_tick

#include "../../../shared/clockdrv_shell.c"
