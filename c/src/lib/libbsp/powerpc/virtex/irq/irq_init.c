/*===============================================================*\
| Project: RTEMS virtex BSP                                       |
+-----------------------------------------------------------------+
| Partially based on the code references which are named below.   |
| Adaptions, modifications, enhancements and any recent parts of  |
| the code are:                                                   |
|                    Copyright (c) 2007                           |
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
| this file contains the irq controller handler                   |
\*===============================================================*/
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
void BSP_irq_nop_func(const rtems_irq_connect_data *unused)
{
  /*
   * nothing to do
   */
}

void BSP_irq_nop_hdl(void *hdl)
{
  /*
   * nothing to do
   */
}

int BSP_irq_true_func(const rtems_irq_connect_data *unused)
{
  /*
   * nothing to do
   */
  return TRUE;
}

/***********************************************************
 * interrupt handler and its enable/disable functions
 ***********************************************************/

/***********************************************************
 * functions to enable/disable/query external/critical interrupts
 */
void BSP_irqexc_on_fnc(rtems_irq_connect_data *conn_data)
{
  uint32_t msr_value;
  /*
   * get current MSR value
   */
  _CPU_MSR_GET(msr_value);


   msr_value |= PPC_MSR_EE;
   _CPU_MSR_SET(msr_value);
}

void BSP_irqexc_off_fnc(rtems_irq_connect_data *unused)
{
  /*
   * nothing to do
   */
}

/***********************************************************
 * High level IRQ handler called from shared_raw_irq_code_entry
 */
int C_dispatch_irq_handler (BSP_Exception_frame *frame, unsigned int excNum)
{


  /*
   * Handle interrupt
   */
  switch(excNum) {
  case ASM_EXT_VECTOR:
    BSP_irq_handle_at_opbintc();
    break;
#if 0 /* We now let the clock driver hook the exception directly */
  case ASM_BOOKE_DEC_VECTOR:
    BSP_rtems_irq_tbl[BSP_PIT].hdl
      (BSP_rtems_irq_tbl[BSP_PIT].handle);
    break;
#endif
#if 0 /* Critical interrupts not yet supported */
  case ASM_BOOKE_CRIT_VECTOR:
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
   * enable irq at interrupt controller
   */
  if (BSP_IS_OPBINTC_IRQ(irq->name)) {
    BSP_irq_enable_at_opbintc(irq->name);
  }
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
   * disable interrupt at interrupt controller
   */
  if (BSP_IS_OPBINTC_IRQ(irq->name)) {
    BSP_irq_disable_at_opbintc(irq->name);
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
   * enable any non-empty IRQ entries at OPBINTC
   */
  for (i =  BSP_OPBINTC_IRQ_LOWEST_OFFSET;
       i <= BSP_OPBINTC_IRQ_MAX_OFFSET;
       i++) {
    if (BSP_rtems_irq_tbl[i].hdl != config->defaultEntry.hdl) {
      BSP_irq_enable_at_opbintc(i);
      BSP_rtems_irq_tbl[i].on((&BSP_rtems_irq_tbl[i]));
    }
    else {
      BSP_rtems_irq_tbl[i].off(&(BSP_rtems_irq_tbl[i]));
      BSP_irq_disable_at_opbintc(i);
    }
  }
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
  0, 		         /* Irq Name                 */
  BSP_irq_nop_hdl,       /* handler function         */
  NULL,                  /* handle passed to handler */
  BSP_irq_nop_func,      /* on function              */
  BSP_irq_nop_func,      /* off function             */
  BSP_irq_true_func      /* isOn function            */
};

static rtems_irq_global_settings initialConfig = {
  BSP_IRQ_NUMBER,    /* irqNb */
  {  0, 		         /* Irq Name                 */
     BSP_irq_nop_hdl,       /* handler function         */
     NULL,                  /* handle passed to handler */
     BSP_irq_nop_func,      /* on function              */
     BSP_irq_nop_func,      /* off function             */
     BSP_irq_true_func      /* isOn function            */
  }, /* emptyIrq */
  rtemsIrqTbl, /* irqHdlTbl  */
  0,           /* irqBase    */
  NULL         /* irqPrioTbl */
};

void BSP_rtems_irq_mng_init(unsigned cpuId)
{
  int i;

  /*
   * connect all exception vectors needed
   */
 ppc_exc_set_handler(ASM_EXT_VECTOR, C_dispatch_irq_handler);
 ppc_exc_set_handler(ASM_BOOKE_DEC_VECTOR, C_dispatch_irq_handler);

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
   * init interrupt controller
   */
  opb_intc_init();
  /*
   * initialize interrupt management
   */
  if (!BSP_rtems_irq_mngt_set(&initialConfig)) {
    BSP_panic("Unable to initialize RTEMS interrupt Management!!! System locked\n");
  }
}

