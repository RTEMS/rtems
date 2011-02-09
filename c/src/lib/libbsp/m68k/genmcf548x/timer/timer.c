/*===============================================================*\
| Project: RTEMS generic mcf548x BSP                              |
+-----------------------------------------------------------------+
| File: timer.c                                                   |
+-----------------------------------------------------------------+
| The file contains the diagnostic timer code of generic MCF548x  |
| BSP.                                                            |
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
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
|                                                                 |
|   date                      history                        ID   |
| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |
| 12.11.07                    1.0                            ras  |
|                                                                 |
\*===============================================================*/

/*
 *  Timer Init
 *
 *  Use the second slice timer (SLT1) as the diagnostic timer.
 */

#include <rtems.h>
#include <rtems/btimer.h>
#include <bsp.h>
#include <mcf548x/mcf548x.h>

bool benchmark_timer_find_average_overhead;
static uint32_t microsecond_timer_value = 0;

void benchmark_timer_initialize(void)
{
    MCF548X_SLT_SLTCNT1 = 0xFFFFFFFF;
    MCF548X_SLT_SCR1 |= (MCF548X_SLT_SCR_TEN | MCF548X_SLT_SCR_RUN);
}

/*
 * Return timer value in microsecond units
 * XLB clock 100 MHz / 1us is equal to 100 counts
 */
uint32_t
benchmark_timer_read(void)
{
    microsecond_timer_value = (0xFFFFFFFF - MCF548X_SLT_SCNT1)/100;

    return microsecond_timer_value;
}

void
benchmark_timer_disable_subtracting_average_overhead(bool find_flag)
{
    benchmark_timer_find_average_overhead = find_flag;
}
