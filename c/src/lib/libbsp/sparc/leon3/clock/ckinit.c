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
  do {  \
    int i; \
    unsigned int iobar, conf; \
    \
    /* Find GP Timer */ \
    i = 0; \
    while (i < amba_conf.apbslv.devnr) { \
      conf = amba_get_confword(amba_conf.apbslv, i, 0); \
      if ((amba_vendor(conf) == VENDOR_GAISLER) && \
          (amba_device(conf) == GAISLER_GPTIMER)) { \
        iobar = amba_apb_get_membar(amba_conf.apbslv, i);       \
        LEON3_Timer_Regs = (volatile LEON3_Timer_Regs_Map *) \
           amba_iobar_start(amba_conf.apbmst, iobar); \
        break; \
      } \
      i++; \
    } \
    \
    clkirq = (LEON3_Timer_Regs->status & 0xfc) >> 3; \
    \
    /* MP */ \
    if (Configuration.User_multiprocessing_table != NULL) { \
      clkirq += LEON3_Cpu_Index; \
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

#include "../../../shared/clockdrv_shell.c"
