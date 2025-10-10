/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * RTEMS generic MPC5200 BSP
 *
 * This file contains functions to implement a slice timer.
 *
 * References: Clock driver for PPC403
 */

/*
 * Copyright (c) 1995 by i-cubed ltd.
 *
 * To anyone who acknowledges that this file is provided "AS IS"
 * without any express or implied warranty:
 *    permission to use, copy, modify, and distribute this file
 *    for any purpose is hereby granted without fee, provided that
 *    the above copyright notice and this notice appears in all
 *    copies, and that the name of i-cubed limited not be used in
 *    advertising or publicity pertaining to distribution of the
 *    software without specific, written prior permission.
 *    i-cubed limited makes no representations about the suitability
 *    of this software for any purpose.
*/
/*
 * Copyright (C) 1989, 1999 On-Line Applications Research Corporation (OAR).
 *
 * Copyright (C) 1997 IMD Ingenieurbuero fuer Microcomputertechnik
 * Copyright (C) 1998 by Frasca International, Inc.
 * Copyright (C) 2003 IPR Engineering
 * Copyright (C) 2005 embedded brains GmbH & Co. KG
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


/*
 * Author: Jay Monkman (jmonkman@frasca.com)
 * Modifications for deriving timer clock from cpu system clock by
 * IMD Ingenieurbuero fuer Microcomputertechnik
 *
 * Derived from c/src/lib/libcpu/ppc/ppc403/clock/clock.c:
 * Author: Andrew Bray <andy@i-cubed.co.uk>
 *
 * Modifications for PPC405GP by Dennis Ehlin
 */

#include <bsp.h>
#include <rtems/bspIo.h>
#include <bsp/fatal.h>
#include <bsp/irq.h>

#include <rtems.h>
#include <rtems/clockdrv.h>
#include <rtems/libio.h>

#include <bsp/irq.h>
#include <bsp/mpc5200.h>
#include <bsp/slicetimer.h>
#include <stdio.h>

uint32_t value0 = 0;
uint32_t value1 = 0;

/*
 *  ISR Handlers
 */
void mpc5200_slt_isr(uint32_t slt_no)
  {
  uint32_t status;
  struct mpc5200_slt *slt = (struct mpc5200_slt *)(&mpc5200.slt[slt_no]);

  status = slt->tsr;

  if(status & SLT_TSR_ST)
    {

    slt->tsr |= SLT_TSR_ST;

    /*if(slt_no == SLT0)
        slt0_user_defined_handler */

    /*if(slt_no == SLT1)
        slt1_user_defined_handler */

    }

  }


rtems_isr mpc5200_slt0_isr(rtems_irq_hdl_param unused)
  {
  (void) unused;

  mpc5200_slt_isr(SLT0);

  }


rtems_isr mpc5200_slt1_isr(rtems_irq_hdl_param unused)
  {
  (void) unused;

  mpc5200_slt_isr(SLT1);

  }


/*
 *  Initialize MPC5x00 slt
 */
void mpc5200_init_slt(uint32_t slt_no)
  {
  struct mpc5200_slt *slt = (struct mpc5200_slt *)(&mpc5200.slt[slt_no]);

  slt->tsr    = SLT_TSR_ST;
  slt->cntrl  = SLT_CNTRL_RW;

  }


/*
 *  Set MPC5x00 slt counter
 */
void mpc5200_set_slt_count(uint32_t slt_no)
  {
  struct mpc5200_slt *slt = (struct mpc5200_slt *)(&mpc5200.slt[slt_no]);

  if(slt_no == SLT0)
    /* Calculate counter value 24 bit (must be greater than 255) => IPB_Clock=33MHz -> Int. every 7,75us - 508ms */
    if((SLT_TSR_COUNT(SLT0_INT_FREQUENCY) > 0xFF) && (SLT_TSR_COUNT(SLT0_INT_FREQUENCY) < 0x1000000))
      slt->tcr = SLT_TSR_COUNT(SLT0_INT_FREQUENCY);

  if(slt_no == SLT1)
    /* Calculate counter value 24 bit (must be greater than 255) => IPB_Clock=33MHz -> Int. every 7,75us - 508ms */
    if((SLT_TSR_COUNT(SLT1_INT_FREQUENCY) > 0xFF) && (SLT_TSR_COUNT(SLT1_INT_FREQUENCY) < 0x1000000))
      slt->tcr = SLT_TSR_COUNT(SLT1_INT_FREQUENCY);

  }


/*
 *  Enable MPC5x00 slt interrupt
 */
void mpc5200_enable_slt_int(uint32_t slt_no)
  {
  struct mpc5200_slt *slt = (struct mpc5200_slt *)(&mpc5200.slt[slt_no]);

  slt->cntrl  |= SLT_CNTRL_TIMEN | SLT_CNTRL_INTEN;

  }


/*
 *  Disable MPC5x00 slt interrupt
 */
void mpc5200_disable_slt_int(uint32_t slt_no)
  {
  struct mpc5200_slt *slt = (struct mpc5200_slt *)(&mpc5200.slt[slt_no]);

  slt->cntrl &= ~(SLT_CNTRL_TIMEN | SLT_CNTRL_INTEN);

  }


/*
 *  Check MPC5x00 slt status
 */
uint32_t mpc5200_check_slt_status(uint32_t slt_no)
  {
  struct mpc5200_slt *slt = (struct mpc5200_slt *)(&mpc5200.slt[slt_no]);

  if(((slt->cntrl) & (SLT_CNTRL_TIMEN | SLT_CNTRL_INTEN)) == (SLT_CNTRL_TIMEN | SLT_CNTRL_INTEN))
    return 1;
  else
    return 0;

  }

/*
 *  switch MPC5x00 slt on
 */
static void sltOn(const rtems_irq_connect_data* irq)
  {
  uint32_t slt_no = 0;

  if((irq->name) == BSP_SIU_IRQ_SL_TIMER0)
    slt_no = 0;

  if((irq->name) == BSP_SIU_IRQ_SL_TIMER1)
    slt_no = 1;

  mpc5200_set_slt_count((uint32_t)slt_no);
  mpc5200_enable_slt_int((uint32_t)slt_no);

  }

/*
 *  switch MPC5x00 slt off
 */
static void sltOff(const rtems_irq_connect_data* irq)
  {
  uint32_t slt_no = 0;

  if((irq->name) == BSP_SIU_IRQ_SL_TIMER0)
    slt_no = 0;

  if((irq->name) == BSP_SIU_IRQ_SL_TIMER1)
    slt_no = 1;

  mpc5200_disable_slt_int((uint32_t)slt_no);

  }

/*
 *  get status of MPC5x00 slt
 */
static int sltIsOn(const rtems_irq_connect_data* irq)
  {
  uint32_t slt_no = 0;

  if((irq->name) == BSP_SIU_IRQ_SL_TIMER0)
    slt_no = 0;

  if((irq->name) == BSP_SIU_IRQ_SL_TIMER1)
    slt_no = 1;

  if(mpc5200_check_slt_status(slt_no))
    return 1;
  else
    return 0;
  }

/*
 *  MPC5x00 slt0 irq connect data
 */
static rtems_irq_connect_data slt0_IrqData =
  {
  BSP_SIU_IRQ_SL_TIMER0,
  mpc5200_slt0_isr,
  (rtems_irq_hdl_param) NULL,
  (rtems_irq_enable)sltOn,
  (rtems_irq_disable)sltOff,
  (rtems_irq_is_enabled)sltIsOn
  };

/*
 *  MPC5x00 slt1 irq connect data
 */
static rtems_irq_connect_data slt1_IrqData =
  {
  BSP_SIU_IRQ_SL_TIMER1,
  mpc5200_slt1_isr,
  (rtems_irq_hdl_param) NULL,
  (rtems_irq_enable)sltOn,
  (rtems_irq_disable)sltOff,
  (rtems_irq_is_enabled)sltIsOn
  };

/*
 *  call MPC5x00 slt install routines
 */
void Install_slt(rtems_device_minor_number slt_no)
  {

  mpc5200_init_slt((uint32_t)slt_no);
  mpc5200_set_slt_count((uint32_t)slt_no);

  }

/*
 *  MPC5x00 slt device driver initialize
 */
rtems_device_driver slt_initialize
  (
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
  )
  {
  (void) major;
  (void) minor;
  (void) pargp;

  /* force minor according to definitions in bsp.h */
  if(USE_SLICETIMER_0)
    {

    Install_slt(0);

    if(!BSP_install_rtems_irq_handler(&slt0_IrqData))
      {

      printk("Unable to connect PSC Irq handler\n");
      bsp_fatal(MPC5200_FATAL_SLICETIMER_0_IRQ_INSTALL);

      }

    }

  if(USE_SLICETIMER_1)
    {

    Install_slt(1);

    if(!BSP_install_rtems_irq_handler(&slt1_IrqData))
      {

      printk("Unable to connect PSC Irq handler\n");
      bsp_fatal(MPC5200_FATAL_SLICETIMER_1_IRQ_INSTALL);

      }

    }

  return RTEMS_SUCCESSFUL;

  }

