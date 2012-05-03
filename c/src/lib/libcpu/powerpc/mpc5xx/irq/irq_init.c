/*
 * irq_init.c
 *
 *  This file contains the implementation of rtems initialization
 *  related to interrupt handling.
 *
 *
 *  MPC5xx port sponsored by Defence Research and Development Canada - Suffield
 *  Copyright (C) 2004, Real-Time Systems Inc. (querbach@realtime.bc.ca)
 *
 *  Derived from libbsp/powerpc/mbx8xx/irq/irq_init.c:
 *
 *  CopyRight (C) 2001 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <mpc5xx.h>
#include <libcpu/vectors.h>
#include <libcpu/raw_exception.h>
#include <bsp/irq.h>

extern rtems_exception_handler_t dispatch_irq_handler;

volatile unsigned int ppc_cached_irq_mask;

/*
 * default methods
 */
static void nop_hdl(rtems_irq_hdl_param ignored)
{
}

static void nop_irq_enable(const struct __rtems_irq_connect_data__*ignored)
{
}

static void nop_raw_enable(
  const struct __rtems_raw_except_connect_data__*ignored
)
{
}

static int irq_is_connected(const struct __rtems_irq_connect_data__*ignored)
{
  return 0;
}

static int raw_is_connected(const struct __rtems_raw_except_connect_data__*ignored)
{
  return 0;
}

static rtems_irq_connect_data     rtemsIrq[CPU_IRQ_COUNT];
static rtems_irq_global_settings  initial_config;
static rtems_irq_connect_data     defaultIrq = {
  0,                /* vector */
  nop_hdl,          /* hdl */
  NULL,             /* handle */
  nop_irq_enable,   /* on */
  nop_irq_enable,   /* off */
  irq_is_connected  /* isOn */
};

static rtems_irq_prio irqPrioTable[CPU_IRQ_COUNT]={
  /*
   * actual priorities for interrupt :
   *   0   means that only current interrupt is masked
   *   255 means all other interrupts are masked
   */
  /*
   * USIU interrupts.
   */
  7,7, 6,6, 5,5, 4,4, 3,3, 2,2, 1,1, 0,0,
  /*
   * UIMB Interrupts
   *
   * Note that the first 8 UIMB interrupts overlap the 8 external USIU
   * interrupts.
   */
                          0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  /*
   * Processor exceptions handled as interrupts
   */
  0
};

void CPU_USIU_irq_init(void)
{
  /*
   * In theory we should initialize two registers at least : SIMASK and
   * SIEL. SIMASK is reset at 0 value meaning no interrupts.  If someone
   * find a reasonnable value for SIEL, and the need to change it, please
   * feel free to add it here.
   */
  ppc_cached_irq_mask = 0;
  usiu.simask = ppc_cached_irq_mask;
  usiu.sipend = 0xffff0000;
  usiu.siel = usiu.siel;
}

/*
 * Initialize UIMB interrupt management
 */
void
CPU_UIMB_irq_init(void)
{
}

void CPU_rtems_irq_mng_init(unsigned cpuId)
{
  rtems_raw_except_connect_data vectorDesc;
  int i;

  CPU_USIU_irq_init();
  CPU_UIMB_irq_init();
  /*
   * Initialize Rtems management interrupt table
   */
    /*
     * re-init the rtemsIrq table
     */
    for (i = 0; i < CPU_IRQ_COUNT; i++) {
      rtemsIrq[i]      = defaultIrq;
      rtemsIrq[i].name = i;
    }
    /*
     * Init initial Interrupt management config
     */
    initial_config.irqNb        = CPU_IRQ_COUNT;
    initial_config.defaultEntry = defaultIrq;
    initial_config.irqHdlTbl    = rtemsIrq;
    initial_config.irqBase      = CPU_ASM_IRQ_VECTOR_BASE;
    initial_config.irqPrioTbl   = irqPrioTable;

    if (!CPU_rtems_irq_mngt_set(&initial_config)) {
      /*
       * put something here that will show the failure...
       */
      BSP_panic("Unable to initialize RTEMS interrupt Management\n");
    }

  /*
   * We must connect the raw irq handler for the two
   * expected interrupt sources : decrementer and external interrupts.
   */
    vectorDesc.exceptIndex = ASM_DEC_VECTOR;
    vectorDesc.hdl.vector  = ASM_DEC_VECTOR;
    vectorDesc.hdl.raw_hdl = dispatch_irq_handler;
    vectorDesc.on          = nop_raw_enable;
    vectorDesc.off         = nop_raw_enable;
    vectorDesc.isOn        = raw_is_connected;
    if (!mpc5xx_set_exception (&vectorDesc)) {
      BSP_panic("Unable to initialize RTEMS decrementer raw exception\n");
    }
    vectorDesc.exceptIndex = ASM_EXT_VECTOR;
    vectorDesc.hdl.vector  = ASM_EXT_VECTOR;
    if (!mpc5xx_set_exception (&vectorDesc)) {
      BSP_panic("Unable to initialize RTEMS external raw exception\n");
    }
}
