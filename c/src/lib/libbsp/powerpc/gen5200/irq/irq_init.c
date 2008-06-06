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
| this file contains the IRQ controller/system initialization     |
\*===============================================================*/
/***********************************************************************/
/*                                                                     */
/*   Module:       irq_init.c                                          */
/*   Date:         07/17/2003                                          */
/*   Purpose:      RTEMS MPC5x00 CPU interrupt initialization          */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Description:  This file contains the implementation of rtems      */
/*                 initialization related to interrupt handling.       */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Code                                                              */
/*   References:   MBX8xx CPU interrupt initialization                 */
/*   Module:       irq_init.c                                          */
/*   Project:      RTEMS 4.6.0pre1 / MBX8xx BSP                        */
/*   Version       1.1                                                 */
/*   Date:         04/06/2001                                          */
/*                                                                     */
/*   Author(s) / Copyright(s):                                         */
/*                                                                     */
/*   CopyRight (C) 2001 valette@crf.canon.fr                           */
/*                                                                     */
/*   The license and distribution terms for this file may be           */
/*   found in found in the file LICENSE in this distribution or at     */
/*   http://www.rtems.com/license/LICENSE.                        */
/*                                                                     */
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
#include <rtems.h>
#include "../irq/irq.h"
#include <rtems/bspIo.h>
#include <libcpu/raw_exception.h>
#include "../include/mpc5200.h"

extern unsigned int external_exception_vector_prolog_code_size;
extern void external_exception_vector_prolog_code();
extern unsigned int decrementer_exception_vector_prolog_code_size;
extern void decrementer_exception_vector_prolog_code();
extern unsigned int system_management_exception_vector_prolog_code_size;
extern void system_management_exception_vector_prolog_code();

extern void BSP_panic(char *s);
extern void _BSP_Fatal_error(unsigned int v);
/*
volatile unsigned int ppc_cached_irq_mask;
*/

/*
 * default on/off function
 */
static void nop_func(){}
/*
 * default isOn function
 */
static int not_connected() {return 0;}
/*
 * default possible isOn function
 */
static int connected() {return 1;}

static rtems_irq_connect_data     rtemsIrq[BSP_IRQ_NUMBER];
static rtems_irq_global_settings  initial_config;

static rtems_irq_connect_data     defaultIrq =
  {
  /* vectorIdex, hdl     , param, on      , off     , isOn */
  0, 		 nop_func, NULL , nop_func, nop_func, not_connected };

static rtems_irq_prio irqPrioTable[BSP_SIU_IRQ_NUMBER] =
  {
/* per. int. priorities (0-7) / 4bit coding / msb is HI/LO selection               */
/* msb = 0 -> non-critical per. int. is routed to main int. (LO_int)               */
/* msb = 1 -> critical per. int. is routed to critical int. (HI_int)               */
  0xF, 0,   0,   0,   /* smart_comm (do not change!), psc1, psc2, psc3             */
  0,   0,   0,   0,   /* irda, eth, usb, ata                                       */
  0,   0,   0,   0,   /* pci_ctrl, pci_sc_rx, pci_sc_tx, res                       */
  0,   0,   0,   0,   /* res, spi_modf, spi_spif, i2c1                             */
  0,   0,   0,   0,   /* i2c, can1, can2, ir_rx                                    */
  0,   0,             /* ir_rx, xlb_arb                                            */
/* main interrupt priorities (0-7) / 4bit coding / msb is INT/SMI selection        */
/* msb = 0 -> main int. is routed to processor INT (low vector base 0x500 )        */
/* msb = 1 -> main int. is routed to processor SMI (low vector base 0x1400 )       */
            0,   0,   /* slice_tim2, irq1                                          */
  0,   0,   0,   0,   /* irq2, irq3, lo_int, rtc_pint                              */
  0,   0,   0,   0,   /* rtc_sint, gpio_std, gpio_wkup, tmr0                       */
  0,   0,   0,   0,   /* tmr1, tmr2, tmr3, tmr4                                    */
  0,   0,   0,        /* tmr5, tmr6, tmr7                                          */
  /* critical interrupt priorities (0-3) / 2bit coding / no special purpose of msb */
                 0,   /* irq0                                                      */
  0,   0,   0         /* slice_tim1, hi_int, ccs_wkup                              */
  };

uint32_t irqMaskTable[BSP_PER_IRQ_NUMBER + BSP_MAIN_IRQ_NUMBER];


/*
 * setup irqMaskTable to support a priorized/nested interrupt environment
 */
void setup_irqMaskTable(void)
  {
  rtems_irq_prio prio = 0;
  uint32_t i = 0, j = 0, mask = 0;

  /* set up the priority dependent masks for peripheral interrupts */
  for(i = BSP_PER_IRQ_LOWEST_OFFSET; i <= BSP_PER_IRQ_MAX_OFFSET; i++)
    {

    prio = irqPrioTable[i];
    mask = 0;

    for(j = BSP_PER_IRQ_LOWEST_OFFSET; j <= BSP_PER_IRQ_MAX_OFFSET; j++)
      {

      if(prio > irqPrioTable[j])
	    mask |= (1 << (31 - j + BSP_PER_IRQ_LOWEST_OFFSET));

      if((prio == irqPrioTable[j]) && (j >= i))
        mask |= (1 << (31 - j + BSP_PER_IRQ_LOWEST_OFFSET));

      }

    irqMaskTable[i] = mask;

    }


  /* set up the priority dependent masks for main interrupts */
  for(i = BSP_MAIN_IRQ_LOWEST_OFFSET; i <= BSP_MAIN_IRQ_MAX_OFFSET; i++)
    {

    prio = irqPrioTable[i];
    mask = 0;

    for(j = BSP_MAIN_IRQ_LOWEST_OFFSET; j <= BSP_MAIN_IRQ_MAX_OFFSET; j++)
      {

      if(prio > irqPrioTable[j])
        mask |= (1 << (16 - j + BSP_MAIN_IRQ_LOWEST_OFFSET));

      if((prio == irqPrioTable[j]) && (j >= i))
        mask |= (1 << (16 - j + BSP_MAIN_IRQ_LOWEST_OFFSET));

      }

    irqMaskTable[i] = mask;

    }

  }


/*
 * Initialize MPC5x00 SIU interrupt management
 */
void BSP_SIU_irq_init(void)
  {

  /* disable all peripheral interrupts */
  mpc5200.per_mask = 0xFFFFFC00;

  /* peripheral interrupt priorities according to reset value */
  mpc5200.per_pri_1 = 0xF0000000;
  mpc5200.per_pri_2 = 0x00000000;
  mpc5200.per_pri_3 = 0x00000000;

  /* disable external interrupts IRQ0-4 / critical interrupts are routed to core_int */
  mpc5200.ext_en_type = 0x0F000001;

  /* disable main interrupts / crit. int. priorities according to reset values */
  mpc5200.crit_pri_main_mask = 0x0001FFFF;

  /* main priorities according to reset value */
  mpc5200.main_pri_1 = 0;
  mpc5200.main_pri_2 = 0;

  /* reset all status indicators */
  mpc5200.csa    = 0x0001FFFF;
  mpc5200.msa    = 0x0001FFFF;
  mpc5200.psa    = 0x003FFFFF;
  mpc5200.psa_be = 0x03000000;

  setup_irqMaskTable();

  }

void BSP_rtems_irq_mng_init(unsigned cpuId)
  {
  rtems_raw_except_connect_data vectorDesc;
  int i;
  #if (BENCHMARK_IRQ_PROCESSING == 1)
    extern void BSP_initialize_IRQ_Timing(void);

    void BSP_initialize_IRQ_Timing(void);
  #endif

  BSP_SIU_irq_init();

  /*
   * Initialize Rtems management interrupt table
   */

  /*
   * re-init the rtemsIrq table
   */
  for (i = 0; i < BSP_IRQ_NUMBER; i++) {
    rtemsIrq[i]      = defaultIrq;
    rtemsIrq[i].name = i;
  }
  /*
   * Init initial Interrupt management config
   */
  initial_config.irqNb        = BSP_IRQ_NUMBER;
  initial_config.defaultEntry = defaultIrq;
  initial_config.irqHdlTbl    = rtemsIrq;
  initial_config.irqBase      = BSP_LOWEST_OFFSET;
  initial_config.irqPrioTbl   = irqPrioTable;
 
  if (!BSP_rtems_irq_mngt_set(&initial_config)) {
    /*
     * put something here that will show the failure...
     */
    BSP_panic(
      "Unable to initialize RTEMS interrupt Management!!! System locked\n"
    );
  }

  /*
   * We must connect the raw irq handler for the two
   * expected interrupt sources : decrementer and external interrupts.
   */
  vectorDesc.exceptIndex       = ASM_DEC_VECTOR;
  vectorDesc.hdl.vector        = ASM_DEC_VECTOR;
  vectorDesc.hdl.raw_hdl	   = decrementer_exception_vector_prolog_code;
  vectorDesc.hdl.raw_hdl_size  = (unsigned) &decrementer_exception_vector_prolog_code_size;
  vectorDesc.on                = nop_func;
  vectorDesc.off               = nop_func;
  vectorDesc.isOn              = connected;

  if (!ppc_set_exception (&vectorDesc)) {
   BSP_panic("Unable to initialize RTEMS decrementer raw exception\n");
  }

  vectorDesc.exceptIndex	   = ASM_EXT_VECTOR;
  vectorDesc.hdl.vector        = ASM_EXT_VECTOR;
  vectorDesc.hdl.raw_hdl       = external_exception_vector_prolog_code;
  vectorDesc.hdl.raw_hdl_size  = (unsigned) &external_exception_vector_prolog_code_size;

  if (!ppc_set_exception (&vectorDesc)) {
    BSP_panic("Unable to initialize RTEMS external raw exception\n");
  }

  vectorDesc.exceptIndex	   = ASM_60X_SYSMGMT_VECTOR;
  vectorDesc.hdl.vector        = ASM_60X_SYSMGMT_VECTOR;
  vectorDesc.hdl.raw_hdl       = system_management_exception_vector_prolog_code;
  vectorDesc.hdl.raw_hdl_size  = (unsigned) &system_management_exception_vector_prolog_code_size;

  if (!ppc_set_exception (&vectorDesc)) {
    BSP_panic("Unable to initialize RTEMS system management raw exception\n");
  }
}

