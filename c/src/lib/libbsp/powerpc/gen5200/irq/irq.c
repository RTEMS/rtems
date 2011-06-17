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
| this file contains the irq controller handler                   |
\*===============================================================*/
/***********************************************************************/
/*                                                                     */
/*   Module:       irq.c                                               */
/*   Date:         07/17/2003                                          */
/*   Purpose:      RTEMS MPC5x00 CPU main interrupt handler & routines */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Description:  This file contains the implementation of the        */
/*                 functions described in irq.h                        */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Code                                                              */
/*   References:   MPC8260ads main interrupt handler & routines        */
/*   Module:       irc.c                                               */
/*   Project:      RTEMS 4.6.0pre1 / MCF8260ads BSP                    */
/*   Version       1.2                                                 */
/*   Date:         04/18/2002                                          */
/*                                                                     */
/*   Author(s) / Copyright(s):                                         */
/*                                                                     */
/*   Copyright (C) 1998, 1999 valette@crf.canon.fr                     */
/*                                                                     */
/*   Modified for mpc8260 Andy Dachs <a.dachs@sstl.co.uk>              */
/*   Surrey Satellite Technology Limited, 2000                         */
/*   Nested exception handlers not working yet.                        */
/*                                                                     */
/*   The license and distribution terms for this file may be           */
/*   found in the file LICENSE in this distribution or at     */
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

#include <rtems.h>

#include <libcpu/powerpc-utility.h>
#include <bsp/vectors.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/mpc5200.h>

/*
 * bit in the SIU mask registers (PPC bit numbering) that should
 * be set to enable the relevant interrupt, mask of 32 is for unused entries
 *
 */
const static unsigned int SIU_MaskBit [BSP_SIU_IRQ_NUMBER] = {
  0, 1, 2, 3,                   /* smart_comm, psc1, psc2, psc3            */
  4, 5, 6, 7,                   /* irda/psc6, eth, usb, ata                */
  8, 9, 10, 11,                 /* pci_ctrl, pci_sc_rx, pci_sc_tx, psc4    */
  12, 13, 14, 15,               /* psc5,spi_modf, spi_spif, i2c1           */
  16, 17, 18, 19,               /* i2c, can1, can2, ir_rx                  */
  20, 21, 15, 16,               /* ir_rx, xlb_arb, slice_tim2, irq1,       */
  17, 18, 19, 20,               /* irq2, irq3, lo_int, rtc_pint            */
  21, 22, 23, 24,               /* rtc_sint, gpio_std, gpio_wkup, tmr0     */
  25, 26, 27, 28,               /* tmr1, tmr2, tmr3, tmr4                  */
  29, 30, 31, 32,               /* tmr5, tmr6, tmr7, res                   */
  32, 32, 32                    /* res, res, res                           */
};

static unsigned char irqPrioTable [BSP_SIU_IRQ_NUMBER] = {
/* per. int. priorities (0-7) / 4bit coding / msb is HI/LO selection               */
/* msb = 0 -> non-critical per. int. is routed to main int. (LO_int)               */
/* msb = 1 -> critical per. int. is routed to critical int. (HI_int)               */
  0xF, 0, 0, 0,                 /* smart_comm (do not change!), psc1, psc2, psc3             */
  0, 0, 0, 0,                   /* irda, eth, usb, ata                                       */
  0, 0, 0, 0,                   /* pci_ctrl, pci_sc_rx, pci_sc_tx, res                       */
  0, 0, 0, 0,                   /* res, spi_modf, spi_spif, i2c1                             */
  0, 0, 0, 0,                   /* i2c, can1, can2, ir_rx                                    */
  0, 0,                         /* ir_rx, xlb_arb                                            */
/* main interrupt priorities (0-7) / 4bit coding / msb is INT/SMI selection        */
/* msb = 0 -> main int. is routed to processor INT (low vector base 0x500 )        */
/* msb = 1 -> main int. is routed to processor SMI (low vector base 0x1400 )       */
  0, 0,                         /* slice_tim2, irq1                                          */
  0, 0, 0, 0,                   /* irq2, irq3, lo_int, rtc_pint                              */
  0, 0, 0, 0,                   /* rtc_sint, gpio_std, gpio_wkup, tmr0                       */
  0, 0, 0, 0,                   /* tmr1, tmr2, tmr3, tmr4                                    */
  0, 0, 0,                      /* tmr5, tmr6, tmr7                                          */
  /* critical interrupt priorities (0-3) / 2bit coding / no special purpose of msb */
  0,                            /* irq0                                                      */
  0, 0, 0                       /* slice_tim1, hi_int, ccs_wkup                              */
};

static uint32_t irqMaskTable [BSP_PER_IRQ_NUMBER + BSP_MAIN_IRQ_NUMBER];

/*
 * Check if symbolic IRQ name is a Processor IRQ
 */
static inline bool is_processor_irq( rtems_vector_number irqLine)
{

  return ((irqLine <= BSP_PROCESSOR_IRQ_MAX_OFFSET)
    && (irqLine >= BSP_PROCESSOR_IRQ_LOWEST_OFFSET));
}

/*
 * Check for SIU IRQ and return base index
 */
static inline bool is_siu_irq( rtems_vector_number irqLine)
{

  return ((irqLine <= BSP_SIU_IRQ_MAX_OFFSET)
    && (irqLine >= BSP_SIU_IRQ_LOWEST_OFFSET));
}

/*
 * Check for SIU IRQ and return base index
 */
static inline int get_siu_irq_base_index( rtems_vector_number irqLine)
{
  if (irqLine <= BSP_PER_IRQ_MAX_OFFSET)
    return BSP_PER_IRQ_LOWEST_OFFSET;

  if (irqLine <= BSP_MAIN_IRQ_MAX_OFFSET)
    return BSP_MAIN_IRQ_LOWEST_OFFSET;

  if (irqLine <= BSP_CRIT_IRQ_MAX_OFFSET)
    return BSP_CRIT_IRQ_LOWEST_OFFSET;

  return -1;
}

static inline void BSP_enable_per_irq_at_siu(
  rtems_vector_number irqLine
)
{
  uint8_t lo_hi_ind = 0,
    prio_index_offset;
  uint32_t *reg;

  /* calculate the index offset of priority value bit field */
  prio_index_offset = (irqLine - BSP_PER_IRQ_LOWEST_OFFSET) % 8;

  /* set interrupt priorities */
  if (irqPrioTable [irqLine] <= 15) {

    /* set peripheral int priority */
    reg = (uint32_t *) (&(mpc5200.per_pri_1));

    /* choose proper register */
    reg += (irqLine >> 3);

    /* set priority as given in priority table */
    *reg |= (irqPrioTable [irqLine] << (28 - (prio_index_offset << 2)));

    /* test msb (hash-bit) and set LO_/HI_int indicator */
    if ((lo_hi_ind = (irqPrioTable [irqLine] >> 3))) {

      /* set critical HI_int priority */
      reg = (uint32_t *) (&(mpc5200.crit_pri_main_mask));
      *reg |= (irqPrioTable [BSP_SIU_IRQ_HI_INT] << 26);

      /*
       * critical interrupt handling for the 603le core is not
       * yet supported, routing of critical interrupts is forced
       * to core_int (bit 31 / CEb)
       */
      mpc5200.ext_en_type |= 1;

    } else {
      if (irqPrioTable [irqLine] <= 15) {
        /* set main LO_int priority */
        reg = (uint32_t *) (&(mpc5200.main_pri_1));
        *reg |= (irqPrioTable [BSP_SIU_IRQ_LO_INT] << 16);
      }
    }
  }

  /* if LO_int ind., enable (unmask) main interrupt */
  if (!lo_hi_ind) {
    mpc5200.crit_pri_main_mask &=
      ~(0x80000000 >> SIU_MaskBit [BSP_SIU_IRQ_LO_INT]);
  }

  /* enable (unmask) peripheral interrupt */
  mpc5200.per_mask &= ~(0x80000000 >> SIU_MaskBit [irqLine]);

  /* FIXME: Why? */
  mpc5200.main_pri_1;
  mpc5200.crit_pri_main_mask;
  mpc5200.per_pri_1;
  mpc5200.per_mask;
}

static inline void BSP_enable_main_irq_at_siu(
  rtems_vector_number irqLine
)
{

  uint8_t prio_index_offset;
  uint32_t *reg;

  /* calculate the index offset of priority value bit field */
  prio_index_offset = (irqLine - BSP_MAIN_IRQ_LOWEST_OFFSET) % 8;

  /* set main interrupt priority */
  if (irqPrioTable [irqLine] <= 15) {

    /* set main int priority */
    reg = (uint32_t *) (&(mpc5200.main_pri_1));

    /* choose proper register */
    reg += (irqLine >> 3);

    /* set priority as given in priority table */
    *reg |= (irqPrioTable [irqLine] << (28 - (prio_index_offset << 2)));

    if ((irqLine >= BSP_SIU_IRQ_IRQ1) && (irqLine <= BSP_SIU_IRQ_IRQ3)) {
      /* enable external irq-pin */
      mpc5200.ext_en_type |= (0x80000000 >> (20 + prio_index_offset));
    }
  }

  /* enable (unmask) main interrupt */
  mpc5200.crit_pri_main_mask &= ~(0x80000000 >> SIU_MaskBit [irqLine]);

}

static inline void BSP_enable_crit_irq_at_siu(
  rtems_vector_number irqLine
)
{
  uint8_t prio_index_offset;
  uint32_t *reg;

  prio_index_offset = irqLine - BSP_CRIT_IRQ_LOWEST_OFFSET;

  /*
   * critical interrupt handling for the 603Le core is not
   * yet supported, routing of critical interrupts is forced
   * to core_int (bit 31 / CEb)
   */
  mpc5200.ext_en_type |= 1;

  /* set critical interrupt priorities */
  if (irqPrioTable [irqLine] <= 3) {

    /* choose proper register */
    reg = (uint32_t *) (&(mpc5200.crit_pri_main_mask));

    /* set priority as given in priority table */
    *reg |= (irqPrioTable [irqLine] << (30 - (prio_index_offset << 1)));

    /* external irq0-pin */
    if (irqLine == BSP_SIU_IRQ_IRQ1) {
      /* enable external irq-pin */
      mpc5200.ext_en_type |= (0x80000000 >> (20 + prio_index_offset));
    }
  }
}

static inline void BSP_disable_per_irq_at_siu(
  rtems_vector_number irqLine
)
{
  uint8_t prio_index_offset;
  uint32_t *reg;

  /* calculate the index offset of priority value bit field */
  prio_index_offset = (irqLine - BSP_PER_IRQ_LOWEST_OFFSET) % 8;

  /* disable (mask) peripheral interrupt */
  mpc5200.per_mask |= (0x80000000 >> SIU_MaskBit [irqLine]);

  /* reset priority to lowest level (reset value) */
  reg = (uint32_t *) (&(mpc5200.per_pri_1));
  reg += (irqLine >> 3);
  *reg &= ~(15 << (28 - (prio_index_offset << 2)));
}

static inline void BSP_disable_main_irq_at_siu(
  rtems_vector_number irqLine
)
{
  uint8_t prio_index_offset;
  uint32_t *reg;

  /* calculate the index offset of priority value bit field */
  prio_index_offset = (irqLine - BSP_MAIN_IRQ_LOWEST_OFFSET) % 8;

  /* disable (mask) main interrupt */
  mpc5200.crit_pri_main_mask |= (0x80000000 >> SIU_MaskBit [irqLine]);

  if ((irqLine >= BSP_SIU_IRQ_IRQ1) && (irqLine <= BSP_SIU_IRQ_IRQ3)) {
    /* disable external irq-pin */
    mpc5200.ext_en_type &= ~(0x80000000 >> (20 + prio_index_offset));
  }

  /* reset priority to lowest level (reset value) */
  reg = (uint32_t *) (&(mpc5200.main_pri_1));
  reg += (irqLine >> 3);
  *reg &= ~(15 << (28 - (prio_index_offset << 2)));
}

static inline void BSP_disable_crit_irq_at_siu( rtems_vector_number
                                               irqLine)
{
  uint8_t prio_index_offset;
  uint32_t *reg;

  prio_index_offset = irqLine - BSP_CRIT_IRQ_LOWEST_OFFSET;

  /* reset critical int priority to lowest level (reset value) */
  reg = (uint32_t *) (&(mpc5200.crit_pri_main_mask));
  *reg &= ~(3 << (30 - (prio_index_offset << 1)));

  if (irqLine == BSP_SIU_IRQ_IRQ1) {
    /* disable external irq0-pin */
    mpc5200.ext_en_type &= ~(0x80000000 >> (20 + prio_index_offset));
  }
}

/*
 * This function enables a given siu interrupt
 */
rtems_status_code bsp_interrupt_vector_enable( rtems_vector_number irqLine)
{
  int base_index = get_siu_irq_base_index( irqLine);

  if (is_siu_irq( irqLine)) {
    rtems_interrupt_level level;

    rtems_interrupt_disable( level);

    switch (base_index) {
      case BSP_PER_IRQ_LOWEST_OFFSET:
        BSP_enable_per_irq_at_siu( irqLine);
        break;
      case BSP_MAIN_IRQ_LOWEST_OFFSET:
        BSP_enable_main_irq_at_siu( irqLine);
        break;
      case BSP_CRIT_IRQ_LOWEST_OFFSET:
        BSP_enable_crit_irq_at_siu( irqLine);
        break;
      default:
        rtems_interrupt_enable( level);
        printk( "No valid base index\n");
        return RTEMS_INVALID_NUMBER;
    }

    rtems_interrupt_enable( level);
  }

  return RTEMS_SUCCESSFUL;
}

/*
 * This function disables a given siu interrupt
 */
rtems_status_code bsp_interrupt_vector_disable( rtems_vector_number irqLine)
{
  int base_index = get_siu_irq_base_index( irqLine);

  if (is_siu_irq( irqLine)) {
    rtems_interrupt_level level;

    rtems_interrupt_disable( level);

    switch (base_index) {
      case BSP_PER_IRQ_LOWEST_OFFSET:
        BSP_disable_per_irq_at_siu( irqLine);
        break;
      case BSP_MAIN_IRQ_LOWEST_OFFSET:
        BSP_disable_main_irq_at_siu( irqLine);
        break;
      case BSP_CRIT_IRQ_LOWEST_OFFSET:
        BSP_disable_crit_irq_at_siu( irqLine);
        break;
      default:
        rtems_interrupt_enable( level);
        printk( "No valid base index\n");
        return RTEMS_INVALID_NUMBER;
    }

    rtems_interrupt_enable( level);
  }

  return RTEMS_SUCCESSFUL;
}

#if (BENCHMARK_IRQ_PROCESSING == 0)
void BSP_IRQ_Benchmarking_Reset( void)
{
}
void BSP_IRQ_Benchmarking_Report( void)
{
}
#else
#include <stdio.h>
uint64_t BSP_Starting_TBR;
uint64_t BSP_Total_in_ISR;
uint32_t BSP_ISR_Count;
uint32_t BSP_Worst_ISR;

#define BSP_COUNTED_IRQ 16
uint32_t BSP_ISR_Count_Per [BSP_COUNTED_IRQ + 1];

void BSP_IRQ_Benchmarking_Reset( void)
{
  int i;

  BSP_Starting_TBR = PPC_Get_timebase_register();
  BSP_Total_in_ISR = 0;
  BSP_ISR_Count = 0;
  BSP_Worst_ISR = 0;
  for (i = 0; i < BSP_COUNTED_IRQ; i++)
    BSP_ISR_Count_Per [i] = 0;
}

static const char *u64tostring( char *buffer, uint64_t v)
{
  sprintf( buffer, "%lld cycles %lld usecs", v, (v / 33));
  return buffer;
}

void BSP_IRQ_Benchmarking_Report( void)
{
  uint64_t now;
  char buffer [96];
  int i;

  now = PPC_Get_timebase_register();
  printk( "Started at: %s\n", u64tostring( buffer, BSP_Starting_TBR));
  printk( "Current   : %s\n", u64tostring( buffer, now));
  printk( "System up : %s\n", u64tostring( buffer, now - BSP_Starting_TBR));
  printk( "ISRs      : %d\n", BSP_ISR_Count);
  printk( "ISRs ran  : %s\n", u64tostring( buffer, BSP_Total_in_ISR));
  printk( "Worst ISR : %s\n", u64tostring( buffer, BSP_Worst_ISR));
  for (i = 0; i < BSP_COUNTED_IRQ; i++)
    printk( "IRQ %d: %d\n", i, BSP_ISR_Count_Per [i]);
  printk( "Ticks     : %d\n", Clock_driver_ticks);
}
#endif

static void dispatch(uint32_t irq, uint32_t offset, volatile uint32_t *maskreg)
{
  #if (ALLOW_IRQ_NESTING == 1)
    uint32_t msr;
    uint32_t mask = *maskreg;
  #endif

  irq += offset;

  #if (ALLOW_IRQ_NESTING == 1)
    *maskreg = mask | irqMaskTable [irq];
    /* Make sure that the write operation completed (cache inhibited area) */
    *maskreg;
    msr = ppc_external_exceptions_enable();
  #endif

  bsp_interrupt_handler_dispatch(irq);

  #if (ALLOW_IRQ_NESTING == 1)
    ppc_external_exceptions_disable(msr);
    *maskreg = mask;
  #endif
}

/*
 * High level IRQ handler called from shared_raw_irq_code_entry
 */
int C_dispatch_irq_handler(BSP_Exception_frame *frame, unsigned excNum)
{
  uint32_t irq;
  uint32_t pmce;

#if (BENCHMARK_IRQ_PROCESSING == 1)
  uint64_t start,
    stop,
    thisTime;

  start = PPC_Get_timebase_register();
  BSP_ISR_Count++;
  if (excNum < BSP_COUNTED_IRQ)
    BSP_ISR_Count_Per [excNum]++;
  else
    printk( "not counting %d\n", excNum);
#endif

  /* get the content of main interrupt status register */
  pmce = mpc5200.pmce;

  /* critical interrupts are routed to the core_int, see premature
   * initialization
   */
  while ((pmce & (PMCE_CSE_STICKY | PMCE_MSE_STICKY)) != 0) {
    /* first: check for critical interrupt sources (hierarchical order)
     * -> HI_int indicates peripheral sources
     */
    if ((pmce & PMCE_CSE_STICKY) != 0) {
      /* get source of critical interrupt */
      irq = PMCE_CSE_SOURCE(pmce);

      switch (irq) {
          /* peripheral HI_int interrupt source detected */
        case 2:
          /* check for valid peripheral interrupt source */
          if ((pmce & PMCE_PSE_STICKY) != 0) {
            /* get source of peripheral interrupt */
            irq = PMCE_PSE_SOURCE(pmce);

            dispatch(irq, BSP_PER_IRQ_LOWEST_OFFSET, &mpc5200.per_mask);
          } else {
            /* this case may not occur: no valid peripheral
             * interrupt source */
            printk( "No valid peripheral HI_int interrupt source\n");
          }
          break;

          /* irq0, slice timer 1 or ccs wakeup detected */
        case 0:
        case 1:
        case 3:

          /* add proper offset for critical interrupts in the siu
           * handler array */
          irq += BSP_CRIT_IRQ_LOWEST_OFFSET;

          /* Dispatch interrupt handlers */
          bsp_interrupt_handler_dispatch( irq);

          break;

        default:
          /* error: unknown interrupt source */
          printk( "Unknown HI_int interrupt source\n");
          break;
      }
    }

    /* second: check for main interrupt sources (hierarchical order)
     * -> LO_int indicates peripheral sources */
    if ((pmce & PMCE_MSE_STICKY) != 0) {
      /* get source of main interrupt */
      irq = PMCE_MSE_SOURCE(pmce);

      if (irq == 4) {
          /* peripheral LO_int interrupt source detected */
          /* check for valid peripheral interrupt source */
          if ((pmce & PMCE_PSE_STICKY) != 0) {
            /* get source of peripheral interrupt */
            irq = PMCE_PSE_SOURCE(pmce);

            dispatch(irq, BSP_PER_IRQ_LOWEST_OFFSET, &mpc5200.per_mask);
          } else {
            /* this case may not occur: no valid peripheral
             * interrupt source */
            printk( "No valid peripheral LO_int interrupt source\n");
          }
      } else if (irq <= 16) {
          /* irq1-3, RTC, GPIO, TMR0-7 detected (attention: slice timer
           * 2 is always routed to SMI) */
          dispatch(irq, BSP_MAIN_IRQ_LOWEST_OFFSET, &mpc5200.crit_pri_main_mask);
      } else {
          /* error: unknown interrupt source */
          printk( "Unknown peripheral LO_int interrupt source\n");
      }
    }

    /* force re-evaluation of interrupts */
    mpc5200.pmce = PMCE_CSE_STICKY | PMCE_MSE_STICKY | PMCE_PSE_STICKY;

    /* get the content of main interrupt status register */
    pmce = mpc5200.pmce;
  }

#if (BENCHMARK_IRQ_PROCESSING == 1)
  stop = PPC_Get_timebase_register();
  thisTime = stop - start;
  BSP_Total_in_ISR += thisTime;
  if (thisTime > BSP_Worst_ISR)
    BSP_Worst_ISR = thisTime;
#endif

  return 0;
}

/*
 * setup irqMaskTable to support a priorized/nested interrupt environment
 */
void setup_irqMaskTable( void)
{
  rtems_irq_prio prio = 0;
  uint32_t i = 0,
    j = 0,
    mask = 0;

  /* set up the priority dependent masks for peripheral interrupts */
  for (i = BSP_PER_IRQ_LOWEST_OFFSET; i <= BSP_PER_IRQ_MAX_OFFSET; i++) {
    prio = irqPrioTable [i];
    mask = 0;

    for (j = BSP_PER_IRQ_LOWEST_OFFSET; j <= BSP_PER_IRQ_MAX_OFFSET; j++) {
      if (prio > irqPrioTable [j]) {
        mask |= (1 << (31 - j + BSP_PER_IRQ_LOWEST_OFFSET));
      }

      if ((prio == irqPrioTable [j]) && (j >= i)) {
        mask |= (1 << (31 - j + BSP_PER_IRQ_LOWEST_OFFSET));
      }
    }

    irqMaskTable [i] = mask;
  }

  /* set up the priority dependent masks for main interrupts */
  for (i = BSP_MAIN_IRQ_LOWEST_OFFSET; i <= BSP_MAIN_IRQ_MAX_OFFSET; i++) {
    prio = irqPrioTable [i];
    mask = 0;

    for (j = BSP_MAIN_IRQ_LOWEST_OFFSET; j <= BSP_MAIN_IRQ_MAX_OFFSET; j++) {
      if (prio > irqPrioTable [j]) {
        mask |= (1 << (16 - j + BSP_MAIN_IRQ_LOWEST_OFFSET));
      }

      if ((prio == irqPrioTable [j]) && (j >= i)) {
        mask |= (1 << (16 - j + BSP_MAIN_IRQ_LOWEST_OFFSET));
      }
    }

    irqMaskTable [i] = mask;
  }
}

/*
 * Initialize MPC5x00 SIU interrupt management
 */
void BSP_SIU_irq_init( void)
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
  mpc5200.csa = 0x0001FFFF;
  mpc5200.msa = 0x0001FFFF;
  mpc5200.psa = 0x003FFFFF;
  mpc5200.psa_be = 0x03000000;

  setup_irqMaskTable();
}

rtems_status_code bsp_interrupt_facility_initialize( void)
{
  BSP_SIU_irq_init();

  /* Install exception handler */
  if (ppc_exc_set_handler( ASM_EXT_VECTOR, C_dispatch_irq_handler)) {
    return RTEMS_IO_ERROR;
  }
  if (ppc_exc_set_handler( ASM_E300_SYSMGMT_VECTOR, C_dispatch_irq_handler)) {
    return RTEMS_IO_ERROR;
  }

  return RTEMS_SUCCESSFUL;
}

void bsp_interrupt_handler_default( rtems_vector_number vector)
{
  if (vector != BSP_DECREMENTER) {
    printk( "Spurious interrupt: 0x%08x\n", vector);
  }
}
