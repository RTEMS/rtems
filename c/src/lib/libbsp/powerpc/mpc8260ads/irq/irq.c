/*
 *
 *  This file contains the implementation of the function described in irq.h
 *
 *  Copyright (C) 1998, 1999 valette@crf.canon.fr
 *
 *  Modified for mpc8260 Andy Dachs <a.dachs@sstl.co.uk>
 *  Surrey Satellite Technology Limited, 2000
 *    21/4/2002 Added support for nested interrupts and improved
 *    masking operations.  Now we compute priority mask based
 *     on table in irq_init.c
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <rtems.h>
#include <rtems/bspIo.h>
#include <bsp/vectors.h>
#include <mpc8260.h>

/*
 * Check if symbolic IRQ name is an CPM IRQ
 */
static inline int is_cpm_irq(const rtems_irq_number irqLine)
{
  return (((int) irqLine <= BSP_CPM_IRQ_MAX_OFFSET) &
      ((int) irqLine >= BSP_CPM_IRQ_LOWEST_OFFSET)
  );
}

typedef struct {
  uint32_t         mask_h;  /* mask for sipnr_h and simr_h */
  uint32_t         mask_l;  /* mask for sipnr_l and simr_l */
  uint32_t         priority_h;  /* mask this and lower priority ints */
  uint32_t         priority_l;
} m82xxIrqMasks_t;

static unsigned char irqPrioTable[BSP_CPM_IRQ_NUMBER]={
  /*
   * actual priorities for interrupt :
   */
  /*
   * CPM Interrupts
   */
  0,  45, 63, 44, 66, 68, 35, 39, 50, 62, 34,  0,  30, 40, 52, 58,
  2,  3,  0,  5,  15, 16, 17, 18, 49, 51,  0,  0,  0,  0,  0,  0,
  6,  7,  8,  0,  11, 12, 0,  0,  20, 21, 22,  23, 0,  0,  0,  0,
  29, 31, 33, 37, 38, 41, 47, 48, 55, 56, 57,  60, 64, 65, 69, 70,

};

/*
 *  Mask fields should have a '1' in the bit position for that
 *  interrupt.
 *  Priority masks calculated later based on priority table
 */

static m82xxIrqMasks_t SIU_MaskBit[BSP_CPM_IRQ_NUMBER] =
{
  { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* err */
  { 0x00000000, 0x00008000, 0x00000000, 0x00000000 }, /* i2c */
  { 0x00000000, 0x00004000, 0x00000000, 0x00000000 }, /* spi */
  { 0x00000000, 0x00002000, 0x00000000, 0x00000000 }, /* rtt */
  { 0x00000000, 0x00001000, 0x00000000, 0x00000000 }, /* smc1 */
  { 0x00000000, 0x00000800, 0x00000000, 0x00000000 }, /* smc2 */
  { 0x00000000, 0x00000400, 0x00000000, 0x00000000 }, /* idma1 */
  { 0x00000000, 0x00000200, 0x00000000, 0x00000000 }, /* idma2 */
  { 0x00000000, 0x00000100, 0x00000000, 0x00000000 }, /* idma3 */
  { 0x00000000, 0x00000080, 0x00000000, 0x00000000 }, /* idma4 */
  { 0x00000000, 0x00000040, 0x00000000, 0x00000000 }, /* sdma */
  { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
  { 0x00000000, 0x00000010, 0x00000000, 0x00000000 }, /* tmr1 */
  { 0x00000000, 0x00000008, 0x00000000, 0x00000000 }, /* tmr2 */
  { 0x00000000, 0x00000004, 0x00000000, 0x00000000 }, /* tmr3 */
  { 0x00000000, 0x00000002, 0x00000000, 0x00000000 }, /* tmr4 */
  { 0x00000004, 0x00000000, 0x00000000, 0x00000000 }, /* tmcnt */
  { 0x00000002, 0x00000000, 0x00000000, 0x00000000 }, /* pit */
  { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
  { 0x00004000, 0x00000000, 0x00000000, 0x00000000 }, /* irq1 */
  { 0x00002000, 0x00000000, 0x00000000, 0x00000000 }, /* irq2 */
  { 0x00001000, 0x00000000, 0x00000000, 0x00000000 }, /* irq3 */
  { 0x00000800, 0x00000000, 0x00000000, 0x00000000 }, /* irq4 */
  { 0x00000400, 0x00000000, 0x00000000, 0x00000000 }, /* irq5 */
  { 0x00000200, 0x00000000, 0x00000000, 0x00000000 }, /* irq6 */
  { 0x00000100, 0x00000000, 0x00000000, 0x00000000 }, /* irq7 */
  { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
  { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
  { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
  { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
  { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
  { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
  { 0x00000000, 0x80000000, 0x00000000, 0x00000000 }, /* fcc1 */
  { 0x00000000, 0x40000000, 0x00000000, 0x00000000 }, /* fcc2 */
  { 0x00000000, 0x20000000, 0x00000000, 0x00000000 }, /* fcc3 */
  { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
  { 0x00000000, 0x08000000, 0x00000000, 0x00000000 }, /* mcc1 */
  { 0x00000000, 0x04000000, 0x00000000, 0x00000000 }, /* mcc2 */
  { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
  { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
  { 0x00000000, 0x00800000, 0x00000000, 0x00000000 }, /* scc1 */
  { 0x00000000, 0x00400000, 0x00000000, 0x00000000 }, /* scc2 */
  { 0x00000000, 0x00200000, 0x00000000, 0x00000000 }, /* scc3 */
  { 0x00000000, 0x00100000, 0x00000000, 0x00000000 }, /* scc4 */
  { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
  { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
  { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
  { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
  { 0x00010000, 0x00000000, 0x00000000, 0x00000000 }, /* pc15 */
  { 0x00020000, 0x00000000, 0x00000000, 0x00000000 }, /* pc14 */
  { 0x00040000, 0x00000000, 0x00000000, 0x00000000 }, /* pc13 */
  { 0x00080000, 0x00000000, 0x00000000, 0x00000000 }, /* pc12 */
  { 0x00100000, 0x00000000, 0x00000000, 0x00000000 }, /* pc11 */
  { 0x00200000, 0x00000000, 0x00000000, 0x00000000 }, /* pc10 */
  { 0x00400000, 0x00000000, 0x00000000, 0x00000000 }, /* pc9 */
  { 0x00800000, 0x00000000, 0x00000000, 0x00000000 }, /* pc8 */
  { 0x01000000, 0x00000000, 0x00000000, 0x00000000 }, /* pc7 */
  { 0x02000000, 0x00000000, 0x00000000, 0x00000000 }, /* pc6 */
  { 0x04000000, 0x00000000, 0x00000000, 0x00000000 }, /* pc5 */
  { 0x08000000, 0x00000000, 0x00000000, 0x00000000 }, /* pc4 */
  { 0x10000000, 0x00000000, 0x00000000, 0x00000000 }, /* pc3 */
  { 0x20000000, 0x00000000, 0x00000000, 0x00000000 }, /* pc2 */
  { 0x40000000, 0x00000000, 0x00000000, 0x00000000 }, /* pc1 */
  { 0x80000000, 0x00000000, 0x00000000, 0x00000000 }, /* pc0 */

};

/*
 * ------------------------ RTEMS Irq helper functions ----------------
 */

/*
 * Caution : this function assumes the variable "internal_config"
 * is already set and that the tables it contains are still valid
 * and accessible.
 */
static void compute_SIU_IvectMask_from_prio (void)
{
  /*
   * The actual masks defined
   * correspond to the priorities defined
   * for the SIU in irq_init.c.
   */

   int i,j;

   for( i=0; i<BSP_CPM_IRQ_NUMBER; i++ )
   {
     for( j=0;j<BSP_CPM_IRQ_NUMBER; j++ )
       if( irqPrioTable[j] < irqPrioTable[i] )
       {
        SIU_MaskBit[i].priority_h |= SIU_MaskBit[j].mask_h;
        SIU_MaskBit[i].priority_l |= SIU_MaskBit[j].mask_l;
      }
   }

}


int BSP_irq_enable_at_cpm(const rtems_irq_number irqLine)
{
  int cpm_irq_index;

  if (!is_cpm_irq(irqLine))
    return 1;

  cpm_irq_index = ((int) (irqLine) - BSP_CPM_IRQ_LOWEST_OFFSET);

  m8260.simr_h |= SIU_MaskBit[cpm_irq_index].mask_h;
  m8260.simr_l |= SIU_MaskBit[cpm_irq_index].mask_l;

  return 0;
}

int BSP_irq_disable_at_cpm(const rtems_irq_number irqLine)
{
  int cpm_irq_index;

  if (!is_cpm_irq(irqLine))
    return 1;

  cpm_irq_index = ((int) (irqLine) - BSP_CPM_IRQ_LOWEST_OFFSET);

  m8260.simr_h &= ~(SIU_MaskBit[cpm_irq_index].mask_h);
  m8260.simr_l &= ~(SIU_MaskBit[cpm_irq_index].mask_l);

  return 0;
}

int BSP_irq_enabled_at_cpm(const rtems_irq_number irqLine)
{
       int cpm_irq_index;

       if (!is_cpm_irq(irqLine))
               return 0;

       cpm_irq_index = ((int) (irqLine) - BSP_CPM_IRQ_LOWEST_OFFSET);

       return ((m8260.simr_h & SIU_MaskBit[cpm_irq_index].mask_h) ||
                   (m8260.simr_l & SIU_MaskBit[cpm_irq_index].mask_l));
}

#ifdef DISPATCH_HANDLER_STAT
volatile unsigned int maxLoop = 0;
#endif

/*
 * High level IRQ handler called from shared_raw_irq_code_entry
 */
int C_dispatch_irq_handler (BSP_Exception_frame *frame, unsigned excNum)
{
  register unsigned int irq;
#if 0
  register unsigned oldMask;          /* old siu pic masks */
#endif
  register unsigned msr;
  register unsigned new_msr;
  register unsigned old_simr_h;
  register unsigned old_simr_l;
#ifdef DISPATCH_HANDLER_STAT
  unsigned loopCounter;
#endif

  /*
   * Handle decrementer interrupt
   */
  if (excNum == ASM_DEC_VECTOR) {
    _CPU_MSR_GET(msr);
    new_msr = msr | MSR_EE;
    _CPU_MSR_SET(new_msr);

    bsp_interrupt_handler_dispatch(BSP_DECREMENTER);

    _CPU_MSR_SET(msr);

    return 0;
  }

  /*
   * Handle external interrupt generated by SIU on PPC core
   */
#ifdef DISPATCH_HANDLER_STAT
  loopCounter = 0;
#endif

  while (1) {

    if( ((m8260.sipnr_h & m8260.simr_h) | (m8260.sipnr_l & m8260.simr_l)) == 0 ) {
#ifdef DISPATCH_HANDLER_STAT
      if (loopCounter >  maxLoop) maxLoop = loopCounter;
#endif
      break;
    }

    irq = (m8260.sivec >> 26) + BSP_CPM_IRQ_LOWEST_OFFSET;

    /* Clear mask and pending register */
    if( irq <= BSP_CPM_IRQ_MAX_OFFSET ) {
      /* save interrupt masks */
      old_simr_h = m8260.simr_h;
      old_simr_l = m8260.simr_l;

      /* mask off current interrupt and lower priority ones */
      m8260.simr_h &= SIU_MaskBit[irq].priority_h;
      m8260.simr_l &= SIU_MaskBit[irq].priority_l;

      /* clear pending bit */
      m8260.sipnr_h |= SIU_MaskBit[irq].mask_h;
      m8260.sipnr_l |= SIU_MaskBit[irq].mask_l;

      /*
       * make sure, that the masking operations in
       * ICTL and MSR are executed in order
       */
      __asm__ volatile("sync":::"memory");

      /* re-enable external exceptions */
      _CPU_MSR_GET(msr);
      new_msr = msr | MSR_EE;
      _CPU_MSR_SET(new_msr);

      /* call handler */
      bsp_interrupt_handler_dispatch(irq);

      /* disable exceptions again */
      _CPU_MSR_SET(msr);

      /*
       * make sure, that the masking operations in
       * ICTL and MSR are executed in order
       */
      __asm__ volatile("sync":::"memory");

      /* restore interrupt masks */
      m8260.simr_h = old_simr_h;
      m8260.simr_l = old_simr_l;

    }
#ifdef DISPATCH_HANDLER_STAT
    ++ loopCounter;
#endif
  }
  return 0;
}

/*
 * Initialize CPM interrupt management
 */
void
BSP_CPM_irq_init(void)
{
   m8260.simr_l = 0;
   m8260.simr_h = 0;
   m8260.sipnr_l = 0xffffffff;
   m8260.sipnr_h = 0xffffffff;
   m8260.sicr = 0;

  /*
   * Initialize the interrupt priorities.
   */
   m8260.siprr   = 0x05309770;	/* reset value */
   m8260.scprr_h = 0x05309770;	/* reset value */
   m8260.scprr_l = 0x05309770;	/* reset value */

}

rtems_status_code bsp_interrupt_vector_enable( rtems_vector_number irqnum)
{
  if (is_cpm_irq(irqnum)) {
    /*
     * Enable interrupt at PIC level
     */
    BSP_irq_enable_at_cpm (irqnum);
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable( rtems_vector_number irqnum)
{
  if (is_cpm_irq(irqnum)) {
    /*
     * disable interrupt at PIC level
     */
    BSP_irq_disable_at_cpm (irqnum);
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_facility_initialize()
{
  /* Install exception handler */
  if (ppc_exc_set_handler( ASM_EXT_VECTOR, C_dispatch_irq_handler)) {
    return RTEMS_IO_ERROR;
  }
  if (ppc_exc_set_handler( ASM_DEC_VECTOR, C_dispatch_irq_handler)) {
    return RTEMS_IO_ERROR;
  }

  /* Fill in priority masks */
  compute_SIU_IvectMask_from_prio();

  /* Initialize the interrupt controller */
  BSP_CPM_irq_init();

  return RTEMS_SUCCESSFUL;
}
