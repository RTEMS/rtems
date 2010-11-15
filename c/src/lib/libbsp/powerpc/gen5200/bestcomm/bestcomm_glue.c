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
#include <assert.h>
#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>
#include "../include/mpc5200.h"
#include "include/ppctypes.h"			/* uint32, et. al.		   */
#include "dma_image.h"
#include "task_api/bestcomm_cntrl.h"
#include "bestcomm_api.h"
#include "bestcomm_glue.h"
#include "include/mgt5200/sdma.h"

extern const uint32 taskTableBytes;

static Heap_Control bestcomm_heap;

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
  if (0 != ((1UL<<bestcomm_taskno) & SDMA_INT_BIT_IMPL)) {
    /*
     * valid task number
     * enable interrupt in bestcomm mask
     */
    SDMA_INT_ENABLE(&mpc5200.sdma.IntMask,bestcomm_taskno);
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
  if (0 != ((1UL<<bestcomm_taskno) & SDMA_INT_BIT_IMPL)) {
    /*
     * valid task number
     * disable interrupt in bestcomm mask
     */
    SDMA_INT_DISABLE(&mpc5200.sdma.IntMask,bestcomm_taskno);
  }
}

typedef struct {
  rtems_interrupt_handler handler;
  void *arg;
} bestcomm_glue_irq_handlers_t;

static bestcomm_glue_irq_handlers_t bestcomm_glue_irq_handlers[32];

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
 rtems_interrupt_handler handler,              /* function to call         */
 void *arg
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
    bestcomm_glue_irq_handlers[bestcomm_taskno].handler = handler;
    bestcomm_glue_irq_handlers[bestcomm_taskno].arg = arg;
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
 void *arg                               /* irq specific handle (not used) */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    none                                                                   |
\*=========================================================================*/
{
  uint32_t pending;
  int curr_taskno;

  pending = mpc5200.sdma.IntPend & ~mpc5200.sdma.IntMask;
  curr_taskno = 0;
  while (pending != 0) {
    if ((pending & (1UL<<curr_taskno)) != 0) {
      if (bestcomm_glue_irq_handlers[curr_taskno].handler != NULL) {
	/*
	 * call proper handler
	 */
	bestcomm_glue_irq_handlers[curr_taskno].handler
	  (bestcomm_glue_irq_handlers[curr_taskno].arg);
      }
      else {
	/*
	 * This should never happen. we have a pending IRQ but no handler
	 * let's clear this pending bit
	 */
	SDMA_CLEAR_IEVENT(&mpc5200.sdma.IntPend,curr_taskno);
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
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  uintptr_t heap_status = 0;

  if (!bestcomm_glue_is_initialized) {
    bestcomm_glue_is_initialized = true;

    heap_status = _Heap_Initialize(
      &bestcomm_heap,
      (char *) &mpc5200.sram [0] + taskTableBytes,
      sizeof(mpc5200.sram) - taskTableBytes,
      4
    );
    assert(heap_status != 0);

    /*
     * Set task bar to begin of sram
     */
    mpc5200.sdma.taskBar = (uint32_t)(&(mpc5200.sram[0]));

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
    mpc5200.sdma.PtdCntrl |= SDMA_PTDCNTRL_PE;

    TasksInitAPI((uint8*)&mpc5200);

    TasksLoadImage( (void *)&(mpc5200.sdma.taskBar));

    /*
     * initialize interrupt dispatcher
     */
    sc = rtems_interrupt_handler_install(
      BSP_SIU_IRQ_SMARTCOMM,
      "BESTCOMM",
      RTEMS_INTERRUPT_UNIQUE,
      bestcomm_glue_irq_dispatcher,
      NULL
    );
    assert(sc == RTEMS_SUCCESSFUL);
  }
}

void *bestcomm_malloc(size_t size)
{
  return _Heap_Allocate(&bestcomm_heap, size);
}

void bestcomm_free(void *ptr)
{
  if (ptr != NULL) {
    bool ok = _Heap_Free(&bestcomm_heap, ptr);
    assert(ok);
  }
}
