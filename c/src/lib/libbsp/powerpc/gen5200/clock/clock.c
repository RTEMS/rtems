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
/*   COPYRIGHT (c) 1989-1999.                                          */
/*   On-Line Applications Research Corporation (OAR).                  */
/*                                                                     */
/*   The license and distribution terms for this file may be           */
/*   found in the file LICENSE in this distribution or at              */
/*   http://www.rtems.com/license/LICENSE.                        */
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
#include "../irq/irq.h"

#include <rtems.h>
#include <rtems/clockdrv.h>
#include <rtems/libio.h>

#include <stdlib.h>                     /* for atexit() */
#include "../include/mpc5200.h"

volatile uint32_t Clock_driver_ticks;

void Clock_exit(void);

uint32_t counter_value;

volatile int ClockInitialized = 0;


/*
 * These are set by clock driver during its init
 */
rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;


/*
 *  ISR Handlers
 */
void mpc5200_gpt_clock_isr(rtems_irq_hdl_param handle)
  {
  uint32_t status;
  struct mpc5200_gpt *gpt = (struct mpc5200_gpt *)handle;

  status = gpt->status;


  if(ClockInitialized  && (status & GPT_STATUS_TEXP))
    {

    gpt->status |= GPT_STATUS_TEXP;


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
  gpt->emsel  = GPT_EMSEL_CE | GPT_EMSEL_ST_CONT | GPT_EMSEL_INTEN | GPT_EMSEL_GPIO_OUT_HIGH | GPT_EMSEL_TIMER_MS_GPIO;

  }


/*
 *  Set MPC5x00 GPT counter
 */
void mpc5200_set_gpt_count(uint32_t counter_value, uint32_t gpt_no)
  {
  uint32_t prescaler_value = 1;
  uint32_t counter = counter_value;
  struct mpc5200_gpt *gpt = (struct mpc5200_gpt *)(&mpc5200.gpt[gpt_no]);

  /* Calculate counter/prescaler value, e.g. IPB_Clock=33MHz -> Int. every 0,3 nsecs. - 130 secs.*/
  while((counter >= (1 << 16)) && (prescaler_value < (1 << 16)))
    {
      prescaler_value++;
      counter = counter_value / prescaler_value;
    }

  counter = (uint16_t)counter;

  gpt->count_in = (prescaler_value << 16) + counter;

  }


/*
 *  Enable MPC5x00 GPT interrupt
 */
void mpc5200_enable_gpt_int(uint32_t gpt_no)
  {
  struct mpc5200_gpt *gpt = (struct mpc5200_gpt *)(&mpc5200.gpt[gpt_no]);

  gpt->emsel |= GPT_EMSEL_CE | GPT_EMSEL_INTEN;

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


void clockOn(const rtems_irq_connect_data* irq)
  {
  uint32_t gpt_no;


  gpt_no = BSP_SIU_IRQ_TMR0 - (irq->name);

  counter_value = rtems_configuration_get_microseconds_per_tick() *
                      rtems_cpu_configuration_get_clicks_per_usec();

  mpc5200_set_gpt_count(counter_value, (uint32_t)gpt_no);
  mpc5200_enable_gpt_int((uint32_t)gpt_no);

  Clock_driver_ticks = 0;
  ClockInitialized = 1;

  }


void clockOff(const rtems_irq_connect_data* irq)
  {
  uint32_t gpt_no;

  gpt_no = BSP_SIU_IRQ_TMR0 - (irq->name);

  mpc5200_disable_gpt_int((uint32_t)gpt_no);

  ClockInitialized = 0;

  }


int clockIsOn(const rtems_irq_connect_data* irq)
  {
  uint32_t gpt_no;

  gpt_no = BSP_SIU_IRQ_TMR0 - (irq->name);

  if(mpc5200_check_gpt_status(gpt_no) && ClockInitialized)
    return ClockInitialized;
  else
    return 0;
  }


int BSP_get_clock_irq_level()
  {

  /*
   * Caution : if you change this, you must change the
   * definition of BSP_PERIODIC_TIMER accordingly
    */
  return BSP_PERIODIC_TIMER;
  }


int BSP_disconnect_clock_handler (void)
  {
  rtems_irq_connect_data clockIrqData;
  clockIrqData.name   = BSP_PERIODIC_TIMER;


  if (!BSP_get_current_rtems_irq_handler(&clockIrqData))
    {

    printk("Unable to stop system clock\n");
    rtems_fatal_error_occurred(1);

    }

  return BSP_remove_rtems_irq_handler (&clockIrqData);

  }


int BSP_connect_clock_handler (uint32_t gpt_no)
  {

  rtems_irq_hdl hdl = 0;
  rtems_irq_connect_data clockIrqData;


  /*
   * Reinit structure
   */

  clockIrqData.name   = BSP_PERIODIC_TIMER;

  if(!BSP_get_current_rtems_irq_handler(&clockIrqData))
    {

    printk("Unable to get system clock handler\n");
    rtems_fatal_error_occurred(1);

    }

  if(!BSP_remove_rtems_irq_handler (&clockIrqData))
    {

    printk("Unable to remove current system clock handler\n");
    rtems_fatal_error_occurred(1);

    }

  if ((gpt_no >= GPT0) ||
      (gpt_no <= GPT7)) {
    hdl = (rtems_irq_hdl_param )&mpc5200.gpt[gpt_no];
  }
  else {
    printk("Unable to set system clock handler\n");
    rtems_fatal_error_occurred(1);
  }

  clockIrqData.hdl    = mpc5200_gpt_clock_isr;
  clockIrqData.handle = (rtems_irq_hdl_param) hdl;
  clockIrqData.on     = clockOn;
  clockIrqData.off    = clockOff;
  clockIrqData.isOn   = clockIsOn;

  return BSP_install_rtems_irq_handler (&clockIrqData);

  }


/*
 * Called via atexit()
 * Remove the clock interrupt handler by setting handler to NULL
 */
void Clock_exit(void)
  {

  (void) BSP_disconnect_clock_handler ();

  }


void Install_clock(rtems_device_minor_number gpt_no)
  {

  Clock_driver_ticks = 0;

  counter_value = rtems_configuration_get_microseconds_per_tick() *
                      rtems_cpu_configuration_get_clicks_per_usec();

  mpc5200_init_gpt((uint32_t)gpt_no);
  mpc5200_set_gpt_count(counter_value, (uint32_t)gpt_no);


  BSP_connect_clock_handler(gpt_no);

  ClockInitialized = 1;

  atexit(Clock_exit);

  }

void ReInstall_clock(uint32_t gpt_no)
  {

  BSP_connect_clock_handler(gpt_no);

  }


rtems_device_driver Clock_initialize
  (
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
  )
  {
  /* force minor according to definitions in irq.h */
  minor = BSP_PERIODIC_TIMER - BSP_SIU_IRQ_TMR0;

  Install_clock((uint32_t)minor);

  /*
   * make major/minor avail to others such as shared memory driver
   */
  rtems_clock_major = major;
  rtems_clock_minor = minor;

  return RTEMS_SUCCESSFUL;

  }

rtems_device_driver Clock_control
  (
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
  )  {

  rtems_libio_ioctl_args_t *args = pargp;

  /* forec minor according to definitions in irq.h */
  minor = BSP_PERIODIC_TIMER - BSP_SIU_IRQ_TMR0;

  if(args != 0) {
    /*
     * This is hokey, but until we get a defined interface
     * to do this, it will just be this simple...
     */
    if(args->command == rtems_build_name('I', 'S', 'R', ' ')) {
      if ((minor >= GPT0) ||
	  (minor <= GPT7)) {
	mpc5200_gpt_clock_isr((rtems_irq_hdl_param )&mpc5200.gpt[minor]);
      }
      else {
	printk("Unable to call system clock handler\n");
	rtems_fatal_error_occurred(1);
      }
    }
    else if(args->command == rtems_build_name('N', 'E', 'W', ' ')) {
      ReInstall_clock((uint32_t)minor);
    }
  }
  return RTEMS_SUCCESSFUL;

}
