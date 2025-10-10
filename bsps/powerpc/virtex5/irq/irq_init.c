/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * RTEMS virtex BSP
 *
 * This file contains the irq controller handler.
 */

/*
 * Copyright (c) 2007 embedded brains GmbH & Co. KG
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

#include <libcpu/spr.h>
#include <bsp/irq.h>
#include <bsp.h>
#include <rtems/bspIo.h>
#include <rtems/powerpc/powerpc.h>
#include <bsp/vectors.h>

static rtems_irq_connect_data rtemsIrqTbl[BSP_IRQ_NUMBER];
rtems_irq_connect_data *BSP_rtems_irq_tbl;
rtems_irq_global_settings* BSP_rtems_irq_config;

/***********************************************************
 * dummy functions for on/off/isOn calls
 * these functions just do nothing fulfill the semantic
 * requirements to enable/disable a certain interrupt or exception
 */
static void BSP_irq_nop_func(const rtems_irq_connect_data *unused)
{
  (void) unused;

  /*
   * nothing to do
   */
}

static void BSP_irq_nop_hdl(void *hdl)
{
  (void) hdl;

  /*
   * nothing to do
   */
}

static int BSP_irq_isOn_func(const rtems_irq_connect_data *unused)
{
  (void) unused;

  /*
   * nothing to do
   */
  return 0;
}

/***********************************************************
 * interrupt handler and its enable/disable functions
 ***********************************************************/

/***********************************************************
 * functions to enable/disable/query external/critical interrupts
 */
void BSP_irqexc_on_fnc(const rtems_irq_connect_data *conn_data)
{
  (void) conn_data;

  uint32_t msr_value;
  /*
   * get current MSR value
   */
  _CPU_MSR_GET(msr_value);

  msr_value |= PPC_MSR_EE;
  _CPU_MSR_SET(msr_value);
}

void BSP_irqexc_off_fnc(const rtems_irq_connect_data *unused)
{
  (void) unused;

  uint32_t msr_value;
  /*
   * get current MSR value
   */
  _CPU_MSR_GET(msr_value);

  msr_value &= ~PPC_MSR_EE;
  _CPU_MSR_SET(msr_value);
}

SPR_RW(BOOKE_TSR)

static int C_dispatch_dec_handler (BSP_Exception_frame *frame, unsigned int excNum)
{
  (void) frame;
  (void) excNum;

  /* Acknowledge the interrupt */
  _write_BOOKE_TSR( BOOKE_TSR_DIS );

  /* Handle the interrupt */
  BSP_rtems_irq_tbl[BSP_DEC].hdl(BSP_rtems_irq_tbl[BSP_DEC].handle);

  return 0;
}


/***********************************************************
 * High level IRQ handler called from shared_raw_irq_code_entry
 */
static int C_dispatch_irq_handler (BSP_Exception_frame *frame, unsigned int excNum)
{
  (void) frame;

  /*
   * Handle interrupt
   */
  switch(excNum) {
  case ASM_EXT_VECTOR:
    BSP_rtems_irq_tbl[BSP_EXT].hdl(BSP_rtems_irq_tbl[BSP_EXT].handle);
    break;
#if 0 /* Dealt with by C_dispatch_dec_handler(), above */
  case ASM_BOOKE_DEC_VECTOR:
    _write_BOOKE_TSR( BOOKE_TSR_DIS );
    BSP_rtems_irq_tbl[BSP_DEC].hdl(BSP_rtems_irq_tbl[BSP_DEC].handle);
    break;
#endif
#if 0 /* Critical interrupts not yet supported */
  case ASM_BOOKE_CRIT_VECTOR:
    BSP_rtems_irq_tbl[BSP_CRIT].hdl(BSP_rtems_irq_tbl[BSP_CRIT].handle);
    break;
#endif
  }

  return 0;
}

/***********************************************************
 * functions to set/get/remove interrupt handlers
 ***********************************************************/
int BSP_install_rtems_irq_handler  (const rtems_irq_connect_data* irq)
{
  rtems_interrupt_level level;

  /*
   * check for valid irq name
   * if invalid, print error and return 0
   */
  if (!BSP_IS_VALID_IRQ(irq->name)) {
    printk("Invalid interrupt vector %d\n",irq->name);
    return 0;
  }

  /*
   * disable interrupts
   */
  rtems_interrupt_disable(level);

  /*
   * check, that default handler is installed now
   */
  if (rtemsIrqTbl[irq->name].hdl != BSP_rtems_irq_config->defaultEntry.hdl) {
    rtems_interrupt_enable(level);
    printk("IRQ vector %d already connected\n",irq->name);
    return 0;
  }

  /*
   * store new handler data
   */
  rtemsIrqTbl[irq->name] = *irq;

  /*
   * call "on" function to enable interrupt at device
   */
  irq->on(irq);

  /*
   * reenable interrupts
   */
  rtems_interrupt_enable(level);

  return 1;
}

int BSP_get_current_rtems_irq_handler	(rtems_irq_connect_data* irq)
{
  rtems_interrupt_level level;

  /*
   * check for valid IRQ name
   */
  if (!BSP_IS_VALID_IRQ(irq->name)) {
    return 0;
  }
  rtems_interrupt_disable(level);

  /*
   * return current IRQ entry
   */
  *irq = rtemsIrqTbl[irq->name];
  rtems_interrupt_enable(level);
  return 1;
}

int BSP_remove_rtems_irq_handler  (const rtems_irq_connect_data* irq)
{
  rtems_interrupt_level level;

  /*
   * check for valid IRQ name
   */
  if (!BSP_IS_VALID_IRQ(irq->name)) {
    return 0;
  }
  rtems_interrupt_disable(level);

  /*
   * check, that specified handler is really connected now
   */
  if (rtemsIrqTbl[irq->name].hdl != irq->hdl) {
    rtems_interrupt_enable(level);
    return 0;
  }

  /*
   * disable interrupt at source
   */
  irq->off(irq);

  /*
   * restore default interrupt handler
   */
  rtemsIrqTbl[irq->name] = BSP_rtems_irq_config->defaultEntry;

  /*
   * reenable interrupts
   */
  rtems_interrupt_enable(level);

  return 1;
}

/***********************************************************
 * functions to set/get the basic interrupt management setup
 ***********************************************************/
/*
 * (Re) get info on current RTEMS interrupt management.
 */
int BSP_rtems_irq_mngt_get(rtems_irq_global_settings** ret_ptr)
{
  *ret_ptr = BSP_rtems_irq_config;
  return 0;
}


/*
 * set management stuff
 */
int BSP_rtems_irq_mngt_set(rtems_irq_global_settings* config)
{
  int                    i;
  rtems_interrupt_level  level;

  rtems_interrupt_disable(level);

  /*
   * store given configuration
   */
  BSP_rtems_irq_config = config;
  BSP_rtems_irq_tbl    = BSP_rtems_irq_config->irqHdlTbl;

  /*
   * store any irq-like processor exceptions
   */
  for (i = BSP_PROCESSOR_IRQ_LOWEST_OFFSET;
       i < BSP_PROCESSOR_IRQ_MAX_OFFSET;
       i++) {
    if (BSP_rtems_irq_tbl[i].hdl != config->defaultEntry.hdl) {
      if (BSP_rtems_irq_tbl[i].on != NULL) {
	BSP_rtems_irq_tbl[i].on
	  (&(BSP_rtems_irq_tbl[i]));
      }
    }
    else {
      if (BSP_rtems_irq_tbl[i].off != NULL) {
	BSP_rtems_irq_tbl[i].off
	  (&(BSP_rtems_irq_tbl[i]));
      }
    }
  }
  rtems_interrupt_enable(level);
  return 1;
}

/*
 * dummy for an empty IRQ handler entry
 */
static rtems_irq_connect_data emptyIrq = {
  0, 		         /* IRQ Name                 */
  BSP_irq_nop_hdl,       /* handler function         */
  NULL,                  /* handle passed to handler */
  BSP_irq_nop_func,      /* on function              */
  BSP_irq_nop_func,      /* off function             */
  BSP_irq_isOn_func      /* isOn function            */
};

static rtems_irq_global_settings initialConfig = {
  BSP_IRQ_NUMBER,           /* IRQ number               */
  {  0, 		    /* IRQ Name                 */
     BSP_irq_nop_hdl,       /* handler function         */
     NULL,                  /* handle passed to handler */
     BSP_irq_nop_func,      /* on function              */
     BSP_irq_nop_func,      /* off function             */
     BSP_irq_isOn_func      /* isOn function            */
  }, /* emptyIrq */
  rtemsIrqTbl, /* irqHdlTbl  */
  0,           /* irqBase    */
  NULL         /* irqPrioTbl */
};

void BSP_rtems_irq_mngt_init(unsigned cpuId)
{
  (void) cpuId;

  int i;

  /*
   * connect all exception vectors needed
   */
  ppc_exc_set_handler(ASM_EXT_VECTOR,       C_dispatch_irq_handler);
  ppc_exc_set_handler(ASM_BOOKE_DEC_VECTOR, C_dispatch_dec_handler);

  /*
   * setup interrupt handlers table
   */
  for (i = 0;
       i < BSP_IRQ_NUMBER;
       i++) {
    rtemsIrqTbl[i]      = emptyIrq;
    rtemsIrqTbl[i].name = i;
  }

  /*
   * initialize interrupt management
   */
  if (!BSP_rtems_irq_mngt_set(&initialConfig)) {
    rtems_panic("Unable to initialize RTEMS interrupt Management!!! System locked\n");
  }
}
