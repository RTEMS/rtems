/*===============================================================*\
| Project: RTEMS generic MPC5200 BSP                              |
+-----------------------------------------------------------------+
| Partially based on the code references which are named below.   |
| Adaptions, modifications, enhancements and any recent parts of  |
| the code are:                                                   |
|                    Copyright (c) 2005                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
|                                                                 |
| Reworked by Joel Sherrill to use clockdrv_shell.c               |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the clock driver functions                   |
\*===============================================================*/
/***********************************************************************/
/*                                                                     */
/*   Module:       clock.c                                             */
/*   Date:         07/17/2003                                          */
/*   Purpose:      RTEMS MPC5x00 clock driver                          */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Description:  Use one of the GPTs for time base generation        */
/*                 instead of the decrementer. The routine initializes */
/*                 the General Purpose Timer GPT6 on the MPC5x00.      */
/*                 The tick frequency is specified by the bsp.         */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Code                                                              */
/*   References:   Clock driver for PPC403                             */
/*   Module:       clock.c                                             */
/*   Project:      RTEMS 4.6.0pre1 / PPC403 BSP                        */
/*   Version       1.16                                                */
/*   Date:         2002/11/01                                          */
/*   Author(s) / Copyright(s):                                         */
/*                                                                     */
/*   Author: Jay Monkman (jmonkman@frasca.com)                         */
/*   Copyright (C) 1998 by Frasca International, Inc.                  */
/*                                                                     */
/*   Derived from c/src/lib/libcpu/ppc/ppc403/clock/clock.c:           */
/*                                                                     */
/*   Author: Andrew Bray <andy@i-cubed.co.uk>                          */
/*                                                                     */
/*   COPYRIGHT (c) 1995 by i-cubed ltd.                                */
/*                                                                     */
/*   To anyone who acknowledges that this file is provided "AS IS"     */
/*   without any express or implied warranty:                          */
/*      permission to use, copy, modify, and distribute this file      */
/*      for any purpose is hereby granted without fee, provided that   */
/*      the above copyright notice and this notice appears in all      */
/*      copies, and that the name of i-cubed limited not be used in    */
/*      advertising or publicity pertaining to distribution of the     */
/*      software without specific, written prior permission.           */
/*      i-cubed limited makes no representations about the suitability */
/*      of this software for any purpose.                              */
/*                                                                     */
/*   Derived from c/src/lib/libcpu/hppa1.1/clock/clock.c:              */
/*                                                                     */
/*   Modifications for deriving timer clock from cpu system clock by   */
/*              Thomas Doerfler <td@imd.m.isar.de>                     */
/*   for these modifications:                                          */
/*   COPYRIGHT (c) 1997 by IMD, Puchheim, Germany.                     */
/*                                                                     */
/*   COPYRIGHT (c) 1989-2007.                                          */
/*   On-Line Applications Research Corporation (OAR).                  */
/*                                                                     */
/*   The license and distribution terms for this file may be           */
/*   found in the file LICENSE in this distribution or at              */
/*   http://www.rtems.com/license/LICENSE.                             */
/*                                                                     */
/*   Modifications for PPC405GP by Dennis Ehlin                        */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Partially based on the code references which are named above.     */
/*   Adaptions, modifications, enhancements and any recent parts of    */
/*   the code are under the right of                                   */
/*                                                                     */
/*         IPR Engineering, Dachauer Straße 38, D-80335 München        */
/*                        Copyright(C) 2003                            */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   IPR Engineering makes no representation or warranties with        */
/*   respect to the performance of this computer program, and          */
/*   specifically disclaims any responsibility for any damages,        */
/*   special or consequential, connected with the use of this program. */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Version history:  1.0                                             */
/*                                                                     */
/***********************************************************************/

#include <bsp.h>
#include <rtems/bspIo.h>
#include <bsp/irq.h>

#include <rtems.h>
#include <rtems/clockdrv.h>
#include <rtems/libio.h>

#include <stdlib.h>                     /* for atexit() */
#include "../include/mpc5200.h"

#define GPT (BSP_PERIODIC_TIMER - BSP_SIU_IRQ_TMR0)

extern uint32_t bsp_clicks_per_usec;

/* this lets us do nanoseconds since last tick */
uint64_t Clock_last_TBR;
volatile uint32_t counter_value;
volatile int ClockInitialized = 0;

/*
 *  ISR Handlers
 */
void mpc5200_gpt_clock_isr(rtems_vector_number vector, void *handle)
{
  uint32_t status;
  struct mpc5200_gpt *gpt = (struct mpc5200_gpt *)handle;

  status = gpt->status;

  if (ClockInitialized  && (status & GPT_STATUS_TEXP)) {
    gpt->status |= GPT_STATUS_RESET;
    Clock_last_TBR = PPC_Get_timebase_register();

    Clock_driver_ticks++;
    rtems_clock_tick();
  }
}

/*
 *  Initialize MPC5x00 GPT
 */
void mpc5200_init_gpt(uint32_t gpt_no)
{
  struct mpc5200_gpt *gpt = (struct mpc5200_gpt *)(&mpc5200.gpt[gpt_no]);

  gpt->status = GPT_STATUS_RESET;
  gpt->emsel  = GPT_EMSEL_CE | GPT_EMSEL_ST_CONT | GPT_EMSEL_INTEN | 
                GPT_EMSEL_GPIO_OUT_HIGH | GPT_EMSEL_TIMER_MS_GPIO;

}

/*
 *  Set MPC5x00 GPT counter
 */
void mpc5200_set_gpt_count(uint32_t counter_value, uint32_t gpt_no)
{
  uint32_t prescaler_value = 1;
  uint32_t counter = counter_value;
  struct mpc5200_gpt *gpt = (struct mpc5200_gpt *)(&mpc5200.gpt[gpt_no]);

  /* Calculate counter/prescaler value, e.g.
   *   IPB_Clock=33MHz -> Int. every 0,3 nsecs. - 130 secs
   */
  while ((counter >= (1 << 16)) && (prescaler_value < (1 << 16))) {
    prescaler_value++;
    counter = counter_value / prescaler_value;
  }

  counter = (uint16_t)counter;

  gpt->count_in = (prescaler_value << 16) + counter;

}

uint32_t bsp_clock_nanoseconds_since_last_tick(void)
{
  uint64_t new_tbr;
  uint64_t bus_cycles;
  uint32_t nsecs;

  new_tbr = PPC_Get_timebase_register();
  bus_cycles = (new_tbr - Clock_last_TBR) * 4;
  nsecs =  (uint32_t) (bus_cycles / (XLB_CLOCK / 1000000)) * 1000;

  return nsecs;
}

/*
 *  Enable MPC5x00 GPT interrupt
 */
void mpc5200_enable_gpt_int(uint32_t gpt_no)
{
  struct mpc5200_gpt *gpt = (struct mpc5200_gpt *)(&mpc5200.gpt[gpt_no]);

  gpt->emsel |= GPT_EMSEL_CE | GPT_EMSEL_INTEN;
  Clock_last_TBR = PPC_Get_timebase_register();
}

/*
 *  Disable MPC5x00 GPT interrupt
 */
void mpc5200_disable_gpt_int(uint32_t gpt_no)
{
  struct mpc5200_gpt *gpt = (struct mpc5200_gpt *)(&mpc5200.gpt[gpt_no]);

  gpt->emsel &= ~(GPT_EMSEL_CE | GPT_EMSEL_INTEN);
}

/*
 *  Check MPC5x00 GPT status
 */
uint32_t mpc5200_check_gpt_status(uint32_t gpt_no)
{
  struct mpc5200_gpt *gpt = (struct mpc5200_gpt *)(&mpc5200.gpt[gpt_no]);

  return ((gpt->emsel) & (GPT_EMSEL_CE | GPT_EMSEL_INTEN));
}

void clockOn()
{
  uint32_t gpt_no;

  gpt_no = BSP_SIU_IRQ_TMR0 - BSP_PERIODIC_TIMER;

  counter_value = rtems_configuration_get_microseconds_per_tick() *
                      bsp_clicks_per_usec;

  mpc5200_set_gpt_count(counter_value, gpt_no);
  mpc5200_enable_gpt_int(gpt_no);

  ClockInitialized = 1;
}

void clockOff()
{
  uint32_t gpt_no;

  gpt_no = BSP_SIU_IRQ_TMR0 - BSP_PERIODIC_TIMER;

  mpc5200_disable_gpt_int(gpt_no);

  ClockInitialized = 0;
}

int BSP_get_clock_irq_level(void)
{
  /*
   * Caution : if you change this, you must change the
   * definition of BSP_PERIODIC_TIMER accordingly
    */
  return BSP_PERIODIC_TIMER;
}

int BSP_disconnect_clock_handler (unsigned gpt_no)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if ((gpt_no < GPT0) || (gpt_no > GPT7)) {
    return 0;
  }

  clockOff( BSP_PERIODIC_TIMER);

  sc = rtems_interrupt_handler_remove(
    BSP_PERIODIC_TIMER,
    mpc5200_gpt_clock_isr,
    &mpc5200.gpt [gpt_no]
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return 0;
  }

  return 1;
}

int BSP_connect_clock_handler (unsigned gpt_no)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if ((gpt_no < GPT0) || (gpt_no > GPT7)) {
    printk("Unable to set system clock handler\n");
    rtems_fatal_error_occurred(1);
  }

  sc = rtems_interrupt_handler_install(
    BSP_PERIODIC_TIMER,
    "Clock",
    RTEMS_INTERRUPT_UNIQUE,
    mpc5200_gpt_clock_isr,
    &mpc5200.gpt [gpt_no]
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return 0;
  }

  clockOn();

  return 1;
}

#define CLOCK_VECTOR 0

#define Clock_driver_support_at_tick()                                   \
  do {                                                                   \
    uint32_t status;                                                     \
    struct mpc5200_gpt *gpt = (struct mpc5200_gpt *)(&mpc5200.gpt[GPT]); \
    \
    status = gpt->status;                                                \
    \
    if (ClockInitialized  && (status & GPT_STATUS_TEXP)) {               \
      gpt->status |= GPT_STATUS_RESET;                                   \
      Clock_last_TBR = PPC_Get_timebase_register();                      \
    }                                                                    \
  } while(0)

#define Clock_driver_support_install_isr( _new, _old ) \
  do {                                                 \
      (_old) = NULL; /* avoid warning */;              \
      BSP_connect_clock_handler(GPT);                  \
  } while(0)

/* This driver does this in clockOn called at connection time */
#define Clock_driver_support_initialize_hardware() \
  do {        \
    counter_value = rtems_configuration_get_microseconds_per_tick() * \
                    bsp_clicks_per_usec; \
    mpc5200_init_gpt(GPT); \
    mpc5200_set_gpt_count(counter_value, GPT); \
  } while (0)

#define Clock_driver_nanoseconds_since_last_tick \
    bsp_clock_nanoseconds_since_last_tick

#define Clock_driver_support_shutdown_hardware() \
  do { \
    (void) BSP_disconnect_clock_handler (GPT); \
  } while (0)

#include "../../../shared/clockdrv_shell.c"

