/*===============================================================*\
| Project: RTEMS generic MPC5200 BSP                              |
+-----------------------------------------------------------------+
|                    Copyright (c) 2004-2005                      |
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
| this file contains glue functions to the Freescale BestComm API |
\*===============================================================*/
#include <rtems.h>
#include <rtems/error.h>
#include <bsp.h>
#include <bsp/irq.h>
#include "../include/mpc5200.h"
#include "include/ppctypes.h"			/* uint32, et. al.		   */
#include "dma_image.h"
#include "task_api/bestcomm_cntrl.h"
#include "bestcomm_api.h"
#include "bestcomm_glue.h"
#include "include/mgt5200/sdma.h"

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void bestcomm_glue_irq_enable
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   enable interrupt for given task number                                  |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int bestcomm_taskno                           /* task number to enable    */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    none                                                                   |
\*=========================================================================*/
{
  rtems_interrupt_level level;
  if (0 != ((1UL<<bestcomm_taskno) & SDMA_INT_BIT_IMPL)) {
    rtems_interrupt_disable(level);
    /*
     * valid task number
     * enable interrupt in bestcomm mask
     */
    SDMA_INT_ENABLE(&mpc5200.IntMask,bestcomm_taskno);
    rtems_interrupt_enable(level);
  }
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void bestcomm_glue_irq_disable
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   disable interrupt for given task number                                 |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int bestcomm_taskno                           /* task number to disable   */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    none                                                                   |
\*=========================================================================*/
{
  rtems_interrupt_level level;
  if (0 != ((1UL<<bestcomm_taskno) & SDMA_INT_BIT_IMPL)) {
    rtems_interrupt_disable(level);
    /*
     * valid task number
     * disable interrupt in bestcomm mask
     */
    SDMA_INT_DISABLE(&mpc5200.IntMask,bestcomm_taskno);
    rtems_interrupt_enable(level);
  }
}

typedef struct {
  void                 (*the_handler)(rtems_irq_hdl_param);
  rtems_irq_hdl_param  the_param;
} bestcomm_glue_irq_handlers_t;

bestcomm_glue_irq_handlers_t bestcomm_glue_irq_handlers[32];

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void bestcomm_glue_irq_install
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   install given function as bestcomm interrupt handler                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int bestcomm_taskno,                          /* task number for handler  */
 void (*the_handler)(rtems_irq_hdl_param),     /* function to call         */
 rtems_irq_hdl_param the_param
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    none                                                                   |
\*=========================================================================*/
{
  if (0 != ((1UL<<bestcomm_taskno) & SDMA_INT_BIT_IMPL)) {
    /*
     * valid task number
     * install handler
     */
    bestcomm_glue_irq_handlers[bestcomm_taskno].the_handler = the_handler;
    bestcomm_glue_irq_handlers[bestcomm_taskno].the_param   = the_param;
  }
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void bestcomm_glue_irq_dispatcher
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   general bestcomm interrupt handler/dispatcher                           |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_irq_hdl_param handle              /* irq specific handle (not used) */

)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    none                                                                   |
\*=========================================================================*/
{
  uint32_t pending;
  int curr_taskno;

  pending = mpc5200.IntPend & ~mpc5200.IntMask;
  curr_taskno = 0;
  while (pending != 0) {
    if ((pending & (1UL<<curr_taskno)) != 0) {
      if (bestcomm_glue_irq_handlers[curr_taskno].the_handler == NULL) {
	/*
	 * This should never happen. we have a pending IRQ but no handler
	 * let's clear this pending bit
	 */
	SDMA_CLEAR_IEVENT(&mpc5200.IntPend,curr_taskno);
      }
      else {
	/*
	 * call proper handler
	 */
	bestcomm_glue_irq_handlers[curr_taskno].the_handler
	  (bestcomm_glue_irq_handlers[curr_taskno].the_param);
      }
      /*
       * clear this bit in our pending copy
       * and go to next bit
       */
      pending &= ~(1<<curr_taskno);
    }
    curr_taskno++;
  }
}

void bestcomm_glue_on(const rtems_irq_connect_data* ptr)
  {
  }


void bestcomm_glue_isOn(const rtems_irq_connect_data* ptr)
  {
  /*return BSP_irq_enabled_at_cpm(ptr->name);*/
  }


void bestcomm_glue_off(const rtems_irq_connect_data* ptr)
  {
  }

static rtems_irq_connect_data bestcomm_glue_irq_data =
  {

  BSP_SIU_IRQ_SMARTCOMM,
  (rtems_irq_hdl)        bestcomm_glue_irq_dispatcher,
  (rtems_irq_hdl_param)  NULL,
  (rtems_irq_enable)     bestcomm_glue_on,
  (rtems_irq_disable)    bestcomm_glue_off,
  (rtems_irq_is_enabled) bestcomm_glue_isOn
  };

static bool bestcomm_glue_is_initialized = false;
/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void bestcomm_glue_init
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   initialize the bestcomm module (if not yet done):                       |
|   - load code                                                             |
|   - initialize registers                                                  |
|   - initialize bus arbiter                                                |
|   - initialize interrupt control                                          |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 void /* none */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    none                                                                   |
\*=========================================================================*/
{
  if (!bestcomm_glue_is_initialized) {
    bestcomm_glue_is_initialized = true;
    /*
     * Set task bar to begin of sram
     */
    mpc5200.taskBar = (uint32_t)(&(mpc5200.sram[0]));

#if 0
    /*
     * Set core and BestComm XLB priority the same.
     */
    mpc5200.priority_enable |= 0x5;
    mpc5200.priority = 0x77777171;
#endif

    /*
     * Turn off COMM bus prefetch. This affects all data movements on
     * the COMM bus. (Yes, _PE -- prefetch enable -- should probably be
     * named _PD.)
     */
    mpc5200.PtdCntrl |= SDMA_PTDCNTRL_PE;

    TasksInitAPI((uint8*)&mpc5200);

    TasksLoadImage( (void *)&(mpc5200.taskBar));

    /*
     * initialize interrupt dispatcher
     */
      if(!BSP_install_rtems_irq_handler (&bestcomm_glue_irq_data)) {
	rtems_panic ("Can't attach MPC5x00 BestComm interrupt handler\n");
      }

  }
}
