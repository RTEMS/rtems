/*===============================================================*\
| Project: RTEMS generic MPC83xx BSP                              |
+-----------------------------------------------------------------+
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
| this file integrates the IPIC irq controller                    |
\*===============================================================*/

#include <mpc83xx/mpc83xx.h>
#include <rtems.h>
#include <rtems/bspIo.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/powerpc/powerpc.h>

typedef struct {
  volatile uint32_t *pend_reg;
  volatile uint32_t *mask_reg;
  const    uint32_t  bit_num;
} BSP_isrc_rsc_t;

const BSP_isrc_rsc_t BSP_ipic_isrc_rsc[] = {
  /* vector 0 */
  {&mpc83xx.ipic.sersr,&mpc83xx.ipic.sermr,31},
  {NULL,NULL,0},
  {NULL,NULL,0},
  {NULL,NULL,0},
  {NULL,NULL,0},
  {NULL,NULL,0},
  {NULL,NULL,0},
  {NULL,NULL,0},
  /* vector  8 */
  {NULL,NULL,0}, /* reserved vector  8 */
  /* vector  9: UART1 SIxxR_H, Bit 24 */
  {&mpc83xx.ipic.sipnr[0],&mpc83xx.ipic.simsr[0],24},
  /* vector 10: UART2 SIxxR_H, Bit 25 */
  {&mpc83xx.ipic.sipnr[0],&mpc83xx.ipic.simsr[0],25},
  /* vector 11: SEC   SIxxR_H, Bit 26 */
  {&mpc83xx.ipic.sipnr[0],&mpc83xx.ipic.simsr[0],26},
  {NULL,NULL,0}, /* reserved vector 12 */
  {NULL,NULL,0}, /* reserved vector 13 */
  /* vector 14: I2C1 SIxxR_H, Bit 29 */
  {&mpc83xx.ipic.sipnr[0],&mpc83xx.ipic.simsr[0],29},
  /* vector 15: I2C2 SIxxR_H, Bit 30 */
  {&mpc83xx.ipic.sipnr[0],&mpc83xx.ipic.simsr[0],30},
  /* vector 16: SPI  SIxxR_H, Bit 31 */
  {&mpc83xx.ipic.sipnr[0],&mpc83xx.ipic.simsr[0],31},
  /* vector 17: IRQ1 SExxR  , Bit  1 */
  {&mpc83xx.ipic.sepnr   ,&mpc83xx.ipic.semsr   , 1},
  /* vector 18: IRQ2 SExxR  , Bit  2 */
  {&mpc83xx.ipic.sepnr   ,&mpc83xx.ipic.semsr   , 2},
  /* vector 19: IRQ3 SExxR  , Bit  3 */
  {&mpc83xx.ipic.sepnr   ,&mpc83xx.ipic.semsr   , 3},
  /* vector 20: IRQ4 SExxR  , Bit  4 */
  {&mpc83xx.ipic.sepnr   ,&mpc83xx.ipic.semsr   , 4},
  /* vector 21: IRQ5 SExxR  , Bit  5 */
  {&mpc83xx.ipic.sepnr   ,&mpc83xx.ipic.semsr   , 5},
  /* vector 22: IRQ6 SExxR  , Bit  6 */
  {&mpc83xx.ipic.sepnr   ,&mpc83xx.ipic.semsr   , 6},
  /* vector 23: IRQ7 SExxR  , Bit  7 */
  {&mpc83xx.ipic.sepnr   ,&mpc83xx.ipic.semsr   , 7},
  {NULL,NULL,0}, /* reserved vector 24 */
  {NULL,NULL,0}, /* reserved vector 25 */
  {NULL,NULL,0}, /* reserved vector 26 */
  {NULL,NULL,0}, /* reserved vector 27 */
  {NULL,NULL,0}, /* reserved vector 28 */
  {NULL,NULL,0}, /* reserved vector 29 */
  {NULL,NULL,0}, /* reserved vector 30 */
  {NULL,NULL,0}, /* reserved vector 31 */
  /* vector 32: TSEC1 Tx  SIxxR_H  , Bit  0 */
  {&mpc83xx.ipic.sipnr[0],&mpc83xx.ipic.simsr[0], 0},
  /* vector 33: TSEC1 Rx  SIxxR_H  , Bit  1 */
  {&mpc83xx.ipic.sipnr[0],&mpc83xx.ipic.simsr[0], 1},
  /* vector 34: TSEC1 Err SIxxR_H  , Bit  2 */
  {&mpc83xx.ipic.sipnr[0],&mpc83xx.ipic.simsr[0], 2},
  /* vector 35: TSEC2 Tx  SIxxR_H  , Bit  3 */
  {&mpc83xx.ipic.sipnr[0],&mpc83xx.ipic.simsr[0], 3},
  /* vector 36: TSEC2 Rx  SIxxR_H  , Bit  4 */
  {&mpc83xx.ipic.sipnr[0],&mpc83xx.ipic.simsr[0], 4},
  /* vector 37: TSEC2 Err SIxxR_H  , Bit  5 */
  {&mpc83xx.ipic.sipnr[0],&mpc83xx.ipic.simsr[0], 5},
  /* vector 38: USB DR    SIxxR_H  , Bit  6 */
  {&mpc83xx.ipic.sipnr[0],&mpc83xx.ipic.simsr[0], 6},
  /* vector 39: USB MPH   SIxxR_H  , Bit  7 */
  {&mpc83xx.ipic.sipnr[0],&mpc83xx.ipic.simsr[0], 7},
  {NULL,NULL,0}, /* reserved vector 40 */
  {NULL,NULL,0}, /* reserved vector 41 */
  {NULL,NULL,0}, /* reserved vector 42 */
  {NULL,NULL,0}, /* reserved vector 43 */
  {NULL,NULL,0}, /* reserved vector 44 */
  {NULL,NULL,0}, /* reserved vector 45 */
  {NULL,NULL,0}, /* reserved vector 46 */
  {NULL,NULL,0}, /* reserved vector 47 */
  /* vector 48: IRQ0 SExxR  , Bit  0 */
  {&mpc83xx.ipic.sepnr   ,&mpc83xx.ipic.semsr   , 0},
  {NULL,NULL,0}, /* reserved vector 49 */
  {NULL,NULL,0}, /* reserved vector 50 */
  {NULL,NULL,0}, /* reserved vector 51 */
  {NULL,NULL,0}, /* reserved vector 52 */
  {NULL,NULL,0}, /* reserved vector 53 */
  {NULL,NULL,0}, /* reserved vector 54 */
  {NULL,NULL,0}, /* reserved vector 55 */
  {NULL,NULL,0}, /* reserved vector 56 */
  {NULL,NULL,0}, /* reserved vector 57 */
  {NULL,NULL,0}, /* reserved vector 58 */
  {NULL,NULL,0}, /* reserved vector 59 */
  {NULL,NULL,0}, /* reserved vector 60 */
  {NULL,NULL,0}, /* reserved vector 61 */
  {NULL,NULL,0}, /* reserved vector 62 */
  {NULL,NULL,0}, /* reserved vector 63 */
  /* vector 64: RTC SEC   SIxxR_L  , Bit  0 */
  {&mpc83xx.ipic.sipnr[1],&mpc83xx.ipic.simsr[1], 0},
  /* vector 65: PIT       SIxxR_L  , Bit  1 */
  {&mpc83xx.ipic.sipnr[1],&mpc83xx.ipic.simsr[1], 1},
  /* vector 66: PCI1      SIxxR_L  , Bit  2 */
  {&mpc83xx.ipic.sipnr[1],&mpc83xx.ipic.simsr[1], 2},
  /* vector 67: PCI2      SIxxR_L  , Bit  3 */
  {&mpc83xx.ipic.sipnr[1],&mpc83xx.ipic.simsr[1], 3},
  /* vector 68: RTC ALR   SIxxR_L  , Bit  4 */
  {&mpc83xx.ipic.sipnr[1],&mpc83xx.ipic.simsr[1], 4},
  /* vector 69: MU        SIxxR_L  , Bit  5 */
  {&mpc83xx.ipic.sipnr[1],&mpc83xx.ipic.simsr[1], 5},
  /* vector 70: SBA       SIxxR_L  , Bit  6 */
  {&mpc83xx.ipic.sipnr[1],&mpc83xx.ipic.simsr[1], 6},
  /* vector 71: DMA       SIxxR_L  , Bit  7 */
  {&mpc83xx.ipic.sipnr[1],&mpc83xx.ipic.simsr[1], 7},
  /* vector 72: GTM4      SIxxR_L  , Bit  8 */
  {&mpc83xx.ipic.sipnr[1],&mpc83xx.ipic.simsr[1], 8},
  /* vector 73: GTM8      SIxxR_L  , Bit  9 */
  {&mpc83xx.ipic.sipnr[1],&mpc83xx.ipic.simsr[1], 9},
  /* vector 74: GPIO1     SIxxR_L  , Bit 10 */
  {&mpc83xx.ipic.sipnr[1],&mpc83xx.ipic.simsr[1],10},
  /* vector 75: GPIO2     SIxxR_L  , Bit 11 */
  {&mpc83xx.ipic.sipnr[1],&mpc83xx.ipic.simsr[1],11},
  /* vector 76: DDR       SIxxR_L  , Bit 12 */
  {&mpc83xx.ipic.sipnr[1],&mpc83xx.ipic.simsr[1],12},
  /* vector 77: LBC       SIxxR_L  , Bit 13 */
  {&mpc83xx.ipic.sipnr[1],&mpc83xx.ipic.simsr[1],13},
  /* vector 78: GTM2      SIxxR_L  , Bit 14 */
  {&mpc83xx.ipic.sipnr[1],&mpc83xx.ipic.simsr[1],14},
  /* vector 79: GTM6      SIxxR_L  , Bit 15 */
  {&mpc83xx.ipic.sipnr[1],&mpc83xx.ipic.simsr[1],15},
  /* vector 80: PMC       SIxxR_L  , Bit 16 */
  {&mpc83xx.ipic.sipnr[1],&mpc83xx.ipic.simsr[1],16},
  {NULL,NULL,0}, /* reserved vector 81 */
  {NULL,NULL,0}, /* reserved vector 82 */
  {NULL,NULL,0}, /* reserved vector 63 */
  /* vector 84: GTM3      SIxxR_L  , Bit 20 */
  {&mpc83xx.ipic.sipnr[1],&mpc83xx.ipic.simsr[1],20},
  /* vector 85: GTM7      SIxxR_L  , Bit 21 */
  {&mpc83xx.ipic.sipnr[1],&mpc83xx.ipic.simsr[1],21},
  {NULL,NULL,0}, /* reserved vector 81 */
  {NULL,NULL,0}, /* reserved vector 82 */
  {NULL,NULL,0}, /* reserved vector 63 */
  {NULL,NULL,0}, /* reserved vector 63 */
  /* vector 90: GTM1      SIxxR_L  , Bit 26 */
  {&mpc83xx.ipic.sipnr[1],&mpc83xx.ipic.simsr[1],26},
  /* vector 91: GTM5      SIxxR_L  , Bit 27 */
  {&mpc83xx.ipic.sipnr[1],&mpc83xx.ipic.simsr[1],27}
};

/*
 * data structure to handle all mask registers in the IPIC
 */
typedef struct {
  uint32_t simsr_mask[2];
  uint32_t semsr_mask;
  uint32_t sermr_mask;
} BSP_ipic_mask_t;

/*
 * this array will be filled with mask values needed 
 * to temporarily disable all IRQ soures with lower or same
 * priority of the current source (whose vector is the array index)
 */
BSP_ipic_mask_t BSP_ipic_prio2mask[BSP_ARRAY_CNT(BSP_ipic_isrc_rsc)];


/*
 * functions to enable/disable a source at the ipic
 */
void BSP_irq_enable_at_ipic (rtems_irq_number irqnum)
{
  uint32_t vecnum = irqnum - BSP_IPIC_IRQ_LOWEST_OFFSET;
  const BSP_isrc_rsc_t *rsc_ptr;
  
  if ((vecnum >= 0) 
      && (vecnum < BSP_ARRAY_CNT(BSP_ipic_isrc_rsc))) {
    rsc_ptr = &BSP_ipic_isrc_rsc[vecnum];
    if (rsc_ptr->mask_reg != NULL) {
      *(rsc_ptr->mask_reg) |= 1 << (31-rsc_ptr->bit_num);
    }
  }
}

void BSP_irq_disable_at_ipic (rtems_irq_number irqnum)
{
  uint32_t vecnum = irqnum - BSP_IPIC_IRQ_LOWEST_OFFSET;
  const BSP_isrc_rsc_t *rsc_ptr;
  
  if ((vecnum >= 0) 
      && (vecnum < BSP_ARRAY_CNT(BSP_ipic_isrc_rsc))) {
    rsc_ptr = &BSP_ipic_isrc_rsc[vecnum];
    if (rsc_ptr->mask_reg != NULL) {
      *(rsc_ptr->mask_reg) &= ~(1 << (31-rsc_ptr->bit_num));
    }
  }
}


/*
 *  IRQ Handler: this is called from the primary exception dispatcher
 */
rtems_status_code BSP_irq_handle_at_ipic(uint32_t excNum)
{
  rtems_status_code rc = RTEMS_SUCCESSFUL;
  rtems_irq_connect_data *tbl_entry;
  int32_t vecnum;
  uint32_t msr_value;
  uint32_t msr_save;
  uint32_t msr_enable = 0;
  BSP_ipic_mask_t mask_save;
  const BSP_ipic_mask_t *mask_ptr;
  /*
   * get vector
   */
  switch(excNum) {
  case ASM_EXT_VECTOR:
    vecnum = MPC83xx_VCR_TO_VEC(mpc83xx.ipic.sivcr);
    msr_enable = PPC_MSR_EE;
    break;
  case ASM_60X_SYSMGMT_VECTOR:
    vecnum = MPC83xx_VCR_TO_VEC(mpc83xx.ipic.smvcr);
    msr_enable = PPC_MSR_EE;
    break;
#if defined(ASM_BOOKE_CRIT_VECTOR)
  case ASM_BOOKE_CRIT_VECTOR:
    vecnum = MPC83xx_VCR_TO_VEC(mpc83xx.ipic.scvcr);
    break;
#endif
  default:
    vecnum = -1;
  }
  /*
   * check vector number
   */
  if ((vecnum >= 0) 
      && (vecnum < BSP_ARRAY_CNT(BSP_ipic_isrc_rsc))) {
    /*
     * save current mask registers
     */
    mask_save.simsr_mask[0] = mpc83xx.ipic.simsr[0];
    mask_save.simsr_mask[1] = mpc83xx.ipic.simsr[1];
    mask_save.semsr_mask    = mpc83xx.ipic.semsr   ;
    mask_save.sermr_mask    = mpc83xx.ipic.sermr   ;
    /*
     * mask all lower prio interrupts
     */
    mask_ptr = &BSP_ipic_prio2mask[vecnum];
    mpc83xx.ipic.simsr[0] &= mask_ptr->simsr_mask[0];
    mpc83xx.ipic.simsr[1] &= mask_ptr->simsr_mask[1];
    mpc83xx.ipic.semsr    &= mask_ptr->semsr_mask   ;
    mpc83xx.ipic.sermr    &= mask_ptr->sermr_mask   ;

    /* 
     * make sure, that the masking operations in 
     * ICTL and MSR are executed in order
     */
    asm volatile("sync":::"memory");
    
    /*
     * reenable msr_ee
     */
    _CPU_MSR_GET(msr_value);
    msr_save   = msr_value;
    msr_value |= msr_enable;
    _CPU_MSR_SET(msr_value);
    /*
     * call handler
     */
    tbl_entry = &BSP_rtems_irq_tbl[vecnum+BSP_IPIC_IRQ_LOWEST_OFFSET];
    if (tbl_entry->hdl != NULL) {
      (tbl_entry->hdl) (tbl_entry->handle);
    } else {
      printk("IPIC: Spurious interrupt; excNum=0x%x, vector=0x%02x\n\r",
	     excNum,vecnum);
    }
    /*
     * disable msr_enable
     */
    _CPU_MSR_SET(msr_save);

    /* 
     * make sure, that the masking operations in 
     * ICTL and MSR are executed in order
     */
    asm volatile("sync":::"memory");

    /*
     * restore initial masks
     */
    mpc83xx.ipic.simsr[0] = mask_save.simsr_mask[0];
    mpc83xx.ipic.simsr[1] = mask_save.simsr_mask[1];
    mpc83xx.ipic.semsr    = mask_save.semsr_mask   ;
    mpc83xx.ipic.sermr    = mask_save.sermr_mask   ;
  }
  return rc;
}


/*
 * fill the array BSP_ipic_prio2mask to allow masking of lower prio sources
 * to implement nested interrupts
 */
rtems_status_code BSP_ipic_calc_prio2mask(void)
{
  rtems_status_code rc = RTEMS_SUCCESSFUL;
  /*
   * FIXME: fill the array
   */
  return rc;
}

/*
 * activate the interrupt controller
 */
rtems_status_code BSP_ipic_intc_init(void)
{
  uint32_t msr_value;
  rtems_status_code rc = RTEMS_SUCCESSFUL;

  /* 
   * mask off all interrupts 
   */
  mpc83xx.ipic.simsr[0] = 0;
  mpc83xx.ipic.simsr[1] = 0;
  mpc83xx.ipic.semsr    = 0;
  mpc83xx.ipic.sermr    = 0;
  /*
   * set desired configuration as defined in bspopts.h
   * normally, the default values should be fine
   */
#if defined(BSP_SICFR_VAL) /* defined in bspopts.h ? */
  mpc83xx.ipic.sicfr = BSP_SICFR_VAL;
#endif

  /*
   * set desired priorities as defined in bspopts.h
   * normally, the default values should be fine
   */
#if defined(BSP_SIPRR0_VAL) /* defined in bspopts.h ? */
  mpc83xx.ipic.siprr[0] = BSP_SIPRR0_VAL;
#endif

#if defined(BSP_SIPRR1_VAL) /* defined in bspopts.h ? */
  mpc83xx.ipic.siprr[1] = BSP_SIPRR1_VAL;
#endif

#if defined(BSP_SIPRR2_VAL) /* defined in bspopts.h ? */
  mpc83xx.ipic.siprr[2] = BSP_SIPRR2_VAL;
#endif

#if defined(BSP_SIPRR3_VAL) /* defined in bspopts.h ? */
  mpc83xx.ipic.siprr[3] = BSP_SIPRR3_VAL;
#endif

#if defined(BSP_SMPRR0_VAL) /* defined in bspopts.h ? */
  mpc83xx.ipic.smprr[0] = BSP_SMPRR0_VAL;
#endif

#if defined(BSP_SMPRR1_VAL) /* defined in bspopts.h ? */
  mpc83xx.ipic.smprr[1] = BSP_SMPRR1_VAL;
#endif

#if defined(BSP_SECNR_VAL) /* defined in bspopts.h ? */
  mpc83xx.ipic.secnr    = BSP_SECNR_VAL;
#endif

  /*
   * calculate priority masks
   */
  rc = BSP_ipic_calc_prio2mask();
  if (rc == RTEMS_SUCCESSFUL) {
    /*
     * enable (non-critical) exceptions
     */
    
    _CPU_MSR_GET(msr_value);
    msr_value |= PPC_MSR_EE;
    _CPU_MSR_SET(msr_value);
    
    /* install exit handler to close ipic when program atexit called */
    /* atexit(ipic_intc_exit); */
  }
  return rc;
}

