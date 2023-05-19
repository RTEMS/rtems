/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * RTEMS generic MPC5200 BSP
 *
 * This file contains glue functions to the Freescale BestComm API.
 */

/*
 * Copyright (C) 2004, 2005 embedded brains GmbH & Co. KG
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

#include <assert.h>
#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/mpc5200.h>
#include <bsp/bestcomm/include/ppctypes.h> /* uint32, et. al.		   */
#include <bsp/bestcomm/dma_image.h>
#include <bsp/bestcomm/task_api/bestcomm_cntrl.h>
#include <bsp/bestcomm/bestcomm_api.h>
#include <bsp/bestcomm/bestcomm_glue.h>
#include <bsp/bestcomm/include/mgt5200/sdma.h>
#include <rtems/score/heapimpl.h>

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
static void bestcomm_glue_irq_dispatcher
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
