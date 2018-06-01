/*===============================================================*\
| Project: RTEMS generic mcf548x BSP                              |
+-----------------------------------------------------------------+
| File: clock.c                                                   |
+-----------------------------------------------------------------+
| The file contains the clock driver code of generic MCF548x BSP. |
+-----------------------------------------------------------------+
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
|                                                                 |
| Parts of the code has been derived from the "dBUG source code"  |
| package Freescale is providing for M548X EVBs. The usage of     |
| the modified or unmodified code and it's integration into the   |
| generic mcf548x BSP has been done according to the Freescale    |
| license terms.                                                  |
|                                                                 |
| The Freescale license terms can be reviewed in the file         |
|                                                                 |
|    Freescale_license.txt                                        |
|                                                                 |
+-----------------------------------------------------------------+
|                                                                 |
| The generic mcf548x BSP has been developed on the basic         |
| structures and modules of the av5282 BSP.                       |
|                                                                 |
+-----------------------------------------------------------------+
|                                                                 |
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.org/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
|                                                                 |
|   date                      history                        ID   |
| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |
| 12.11.07                    1.0                            ras  |
|                                                                 |
\*===============================================================*/

/*
 * Use first slice timer (SLT0) as the system clock.
 *
 */

#include <rtems.h>
#include <bsp.h>
#include <bsp/irq-generic.h>
#include <mcf548x/mcf548x.h>

/*
 * Use SLT 0
 */
#define CLOCK_IRQ MCF548X_IRQ_SLT0

/*
 * Periodic interval timer interrupt handler
 */
#define Clock_driver_support_at_tick()             \
    do {                                           \
        MCF548X_SLT_SSR0 = MCF548X_SLT_SSR_ST;     \
    } while (0)                                    \

/*
 * Attach clock interrupt handler
 */
#define Clock_driver_support_install_isr( _new ) \
    set_vector(_new, CLOCK_IRQ + 64, 1)

/*
 * Set up the clock hardware
 *
 * We need to have 1 interrupt every 10,000 microseconds
 * XLB clock 100 MHz / MCF548X_SLT_SLTCNT0 = XLB clock/100
 */
#define Clock_driver_support_initialize_hardware()			\
  do {									\
    bsp_interrupt_vector_enable(CLOCK_IRQ);				\
    MCF548X_SLT_SLTCNT0 = get_CPU_clock_speed()				\
      / 1000								\
      * rtems_configuration_get_microseconds_per_tick()			\
      / 1000;								\
    MCF548X_SLT_SCR0 |= (MCF548X_SLT_SCR_TEN | MCF548X_SLT_SCR_RUN | MCF548X_SLT_SCR_IEN); \
  } while (0)

#define CLOCK_DRIVER_USE_DUMMY_TIMECOUNTER

#include "../../../shared/dev/clock/clockimpl.h"

