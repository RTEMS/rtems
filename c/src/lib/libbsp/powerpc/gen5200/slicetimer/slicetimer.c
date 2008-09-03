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
| this file contains functions to implement a slice timer         |
\*===============================================================*/
/***********************************************************************/
/*                                                                     */
/*   Module:       slicetimer.c                                        */
/*   Date:         07/17/2003                                          */
/*   Purpose:      RTEMS MPC5x00 slicetimer driver                     */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Description:  MPC5x00 slice timer routines for cyclic short time  */
/*                 trigger                                             */
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
#include <bsp/irq.h>

#include <rtems.h>
#include <rtems/clockdrv.h>
#include <rtems/libio.h>

#include <bsp/irq.h>
#include "../include/mpc5200.h"
#include "../slicetimer/slicetimer.h"
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

  mpc5200_slt_isr(SLT0);

  }


rtems_isr mpc5200_slt1_isr(rtems_irq_hdl_param unused)
  {

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
void sltOn(const rtems_irq_connect_data* irq)
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
void sltOff(const rtems_irq_connect_data* irq)
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
int sltIsOn(const rtems_irq_connect_data* irq)
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

  /* force minor according to definitions in bsp.h */
  if(USE_SLICETIMER_0)
    {

    Install_slt(0);

    if(!BSP_install_rtems_irq_handler(&slt0_IrqData))
      {

      printk("Unable to connect PSC Irq handler\n");
      rtems_fatal_error_occurred(1);

      }

    }

  if(USE_SLICETIMER_1)
    {

    Install_slt(1);

    if(!BSP_install_rtems_irq_handler(&slt1_IrqData))
      {

      printk("Unable to connect PSC Irq handler\n");
      rtems_fatal_error_occurred(1);

      }

    }

  return RTEMS_SUCCESSFUL;

  }

