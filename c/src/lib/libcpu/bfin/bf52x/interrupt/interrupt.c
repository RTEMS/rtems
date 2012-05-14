/**
 *@file interrupt.c
 *
 *@brief
 * - This file implements interrupt dispatcher. Most of the code is taken from
 *  the 533 implementation for blackfin. Since 52X supports 56 line and 2 ISR
 *  registers some portion is written twice.
 *
 * Target:   TLL6527v1-0
 * Compiler:
 *
 * COPYRIGHT (c) 2010 by ECE Northeastern University.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license
 *
 * @author Rohan Kangralkar, ECE, Northeastern University
 *         (kangralkar.r@husky.neu.edu)
 *
 * LastChange:
 */

#include <rtems.h>
#include <rtems/libio.h>

#include <bsp.h>
#include <libcpu/cecRegs.h>
#include <libcpu/sicRegs.h>
#include "interrupt.h"

#define SIC_IAR_COUNT_SET0		  4
#define SIC_IAR_BASE_ADDRESS_0  0xFFC00150

/**
 * There are two implementations for the interrupt handler.
 * 1. INTERRUPT_USE_TABLE: uses tables for finding the right ISR.
 * 2. Uses link list to find the user ISR.
 *
 *
 * 1. INTERRUPT_USE_TABLE
 * Space requirement:
 *  - Array to hold CEC masks size: CEC_INTERRUPT_COUNT(9)*(2*int).9*2*4= 72B
 *  - Array to hold isr function pointers IRQ_MAX(56)*sizeof(bfin_isr_t)= 896B
 *  - Array for bit twidlling 32 bytes.
 *  - Global Mask 8 bytes.
 *  - Total = 1008 Bytes Aprox
 *
 * Time requirements
 *    The worst case time is about the same for jumping to the user ISR. With a
 *    variance of one conditional statement.
 *
 * 2. Using link list.
 * Space requirement:
 *  - Array to hold CEC mask CEC_INTERRUPT_COUNT(9)*(sizeof(vectors)).
 *                                                                 9*3*4= 108B
 *  - Array to hold isr IRQ_MAX(56)*sizeof(bfin_isr_t) The structure has
 *    additional pointers                                         56*7*4=1568B
 *  - Global Mask 8 bytes.
 *    Total = 1684.
 * Time requirements
 *    In the worst case all the lines can be on one CEC line to 56 entries have
 *    to be traversed to find the right user ISR.
 *    But this implementation has benefit of being flexible, Providing
 *    additional user assigned priority. and may consume less space
 *    if all devices are not supported.
 */

/**
 * TODO: To place the dispatcher routine code in L1.
 */

#if INTERRUPT_USE_TABLE


/******************************************************************************
 * Static variables
 *****************************************************************************/
/**
 * @var sic_isr0_mask
 * @brief copy of the mask of SIC ISR. The SIC ISR is cleared by the device
 * the relevant SIC_ISRx bit is not cleared unless the interrupt
 * service routine clears the mechanism that generated interrupt
 */
static uint32_t sic_isr0_mask = 0;

/**
 * @var sic_isr0_mask
 * @brief copy of the mask of SIC ISR. The SIC ISR is cleared by the device
 * the relevant SIC_ISRx bit is not cleared unless the interrupt
 * service routine clears the mechanism that generated interrupt
 */
static uint32_t sic_isr1_mask = 0;


/**
 * @var sic_isr
 * @brief An array of sic register mask for each of the 16 core interrupt lines
 */
static struct {
  uint32_t mask0;
  uint32_t mask1;
} vectors[CEC_INTERRUPT_COUNT];

/**
 * @var ivt
 * @brief Contains a table of ISR and arguments. The ISR jumps directly to
 * these ISR.
 */
static bfin_isr_t ivt[IRQ_MAX];

/**
 * http://graphics.stanford.edu/~seander/bithacks.html for more details
 */
static const char clz_table[32] =
{
    0, 31, 9, 30, 3, 8, 18, 29, 2, 5, 7, 14, 12, 17,
    22, 28, 1, 10, 4, 19, 6, 15, 13, 23, 11, 20, 16,
    24, 21, 25, 26, 27
};

/**
 * finds the first bit set from the left. look at
 * http://graphics.stanford.edu/~seander/bithacks.html for more details
 * @param n
 * @return
 */
static unsigned long clz(unsigned long n)
{
  unsigned long c = 0x7dcd629;       /* magic constant... */

  n |= (n >> 1);
  n |= (n >> 2);
  n |= (n >> 4);
  n |= (n >> 8);
  n |= (n >> 16);
  if (n == 0) return 32;
  n = c + (c * n);
  return 31 - clz_table[n >> 27];       /* For little endian    */
}



/**
 * Centralized Interrupt dispatcher routine. This routine dispatches interrupts
 * to the user ISR. The priority is according to the blackfin SIC.
 * The first level of priority is handled in the hardware at the core event
 * controller. The second level of interrupt is handled according to the line
 * number that goes in to the SIC.
 * * SIC_0 has higher priority than SIC 1.
 * * Inside the SIC the priority is assigned according to the line number.
 *   Lower the line number higher the priority.
 *
 *   In order to change the interrupt priority we may
 *   1. change the SIC IAR registers or
 *   2. Assign priority and extract it inside this function and call the ISR
 *   according tot the priority.
 *
 * @param vector IVG number.
 * @return
 */
static rtems_isr interruptHandler(rtems_vector_number vector) {
  uint32_t mask = 0;
  int id = 0;
  /**
   * Enable for debugging
   *
   * static volatile uint32_t spurious_sic0    = 0;
   * static volatile uint32_t spurious_source  = 0;
   * static volatile uint32_t spurious_sic1    = 0;
   */

  /**
   * Extract the vector number relative to the SIC start line
   */
  vector -= CEC_INTERRUPT_BASE_VECTOR;

  /**
   * Check for bounds
   */
  if (vector >= 0 && vector < CEC_INTERRUPT_COUNT) {

    /**
     * Extract information and execute ISR from SIC 0
     */
    mask = *(uint32_t volatile *) SIC_ISR &
        *(uint32_t volatile *) SIC_IMASK & vectors[vector].mask0;
    id      = clz(mask);
    if ( SIC_ISR0_MAX > id ) {
      /** Parameter check */
      if( NULL != ivt[id].pFunc) {
        /** Call the relevant function with argument */
        ivt[id].pFunc( ivt[id].pArg );
      } else {
        /**
         * spurious interrupt we should not be getting this
         * spurious_sic0++;
         * spurious_source = id;
         */
      }
    } else {
      /**
       * we look at SIC 1
       */
    }


    /**
     * Extract information and execute ISR from SIC 1
     */
    mask    = *(uint32_t volatile *) (SIC_ISR + SIC_ISR_PITCH) &
        *(uint32_t volatile *) (SIC_IMASK + SIC_IMASK_PITCH) &
        vectors[vector].mask1;
    id      = clz(mask)+SIC_ISR0_MAX;
    if ( IRQ_MAX > id ) {
      /** Parameter Check */
      if( NULL != ivt[id].pFunc ) {
        /** Call the relevant function with argument */
        ivt[id].pFunc( ivt[id].pArg );
      } else {
        /**
         * spurious interrupt we should not be getting this
         *
         * spurious_sic1++;
         * spurious_source = id;
         */
      }
    } else {
      /**
       * we continue
       */
    }

  }
}



/**
 * This routine registers a new ISR. It will write a new entry to the IVT table
 * @param isr contains a callback function and source
 * @return rtems status code
 */
rtems_status_code bfin_interrupt_register(bfin_isr_t *isr) {
  rtems_interrupt_level isrLevel;
  int               id        = 0;
  int               position  = 0;

  /**
   * Sanity Check
   */
  if ( NULL == isr ){
    return RTEMS_UNSATISFIED;
  }

  /**
   * Sanity check. The register function should at least provide callback func
   */
  if ( NULL == isr->pFunc ) {
    return RTEMS_UNSATISFIED;
  }

  id = isr->source;

  /**
   * Parameter Check. We already have a function registered here. First
   * unregister and then a new function can be allocated.
   */
  if ( NULL != ivt[id].pFunc ) {
    return RTEMS_UNSATISFIED;
  }

  rtems_interrupt_disable(isrLevel);
  /**
   * Assign the new function pointer to the ISR Dispatcher
   * */
  ivt[id].pFunc    = isr->pFunc;
  ivt[id].pArg     = isr->pArg;


  /** find out which isr mask has to be set to enable the interrupt */
  if ( SIC_ISR0_MAX > id ) {
    sic_isr0_mask |= 0x1<<id;
    *(uint32_t volatile *) SIC_IMASK  |= 0x1<<id;
  } else {
    position = id - SIC_ISR0_MAX;
    sic_isr1_mask |= 0x1<<position;
    *(uint32_t volatile *) (SIC_IMASK + SIC_IMASK_PITCH)  |= 0x1<<position;
  }

  rtems_interrupt_enable(isrLevel);

  return RTEMS_SUCCESSFUL;
}


/**
 * This function unregisters a registered interrupt handler.
 * @param isr
 */
rtems_status_code bfin_interrupt_unregister(bfin_isr_t *isr) {
  rtems_interrupt_level isrLevel;
  int               id        = 0;
  int               position  = 0;

  /**
   * Sanity Check
   */
  if ( NULL == isr ){
    return RTEMS_UNSATISFIED;
  }

  id = isr->source;

  rtems_interrupt_disable(isrLevel);
  /**
   * Assign the new function pointer to the ISR Dispatcher
   * */
  ivt[id].pFunc    = NULL;
  ivt[id].pArg     = NULL;


  /** find out which isr mask has to be set to enable the interrupt */
  if ( SIC_ISR0_MAX > id ) {
    sic_isr0_mask &= ~(0x1<<id);
    *(uint32_t volatile *) SIC_IMASK  &= ~(0x1<<id);
  } else {
    position = id - SIC_ISR0_MAX;
    sic_isr1_mask &= ~(0x1<<position);
    *(uint32_t volatile *) (SIC_IMASK + SIC_IMASK_PITCH)  &= ~(0x1<<position);
  }

  rtems_interrupt_enable(isrLevel);

  return RTEMS_SUCCESSFUL;
}




/**
 * blackfin interrupt initialization routine. It initializes the bfin ISR
 * dispatcher. It will also create SIC CEC map which will be used for
 * identifying the ISR.
 */
void bfin_interrupt_init(void) {
  int source;
  int vector;
  uint32_t r;
  int i;
  int j;

  *(uint32_t volatile *) SIC_IMASK = 0;
  *(uint32_t volatile *) (SIC_IMASK + SIC_IMASK_PITCH) = 0;

  memset(vectors, 0, sizeof(vectors));
  /* build mask0 showing what SIC sources drive each CEC vector */
  source = 0;

  /**
   * The bf52x has 8 IAR registers but they do not have a constant pitch.
   *
   */
  for (i = 0; i < SIC_IAR_COUNT; i++) {
    if ( SIC_IAR_COUNT_SET0 > i ) {
      r = *(uint32_t volatile *) (SIC_IAR_BASE_ADDRESS + i * SIC_IAR_PITCH);
    } else {
      r = *(uint32_t volatile *) (SIC_IAR_BASE_ADDRESS_0 +
          ((i-SIC_IAR_COUNT_SET0) * SIC_IAR_PITCH));
    }

    for (j = 0; j < 8; j++) {
      vector = r & 0x0f;
      if (vector >= 0 && vector < CEC_INTERRUPT_COUNT) {
        /* install our local handler */
        if (vectors[vector].mask0 == 0 && vectors[vector].mask1 == 0){
          set_vector(interruptHandler, vector + CEC_INTERRUPT_BASE_VECTOR, 1);
        }
        if ( SIC_ISR0_MAX > source ) {
          vectors[vector].mask0 |= (1 << source);
        } else {
          vectors[vector].mask1 |= (1 << (source - SIC_ISR0_MAX));
        }
      }
      r >>= 4;
      source++;
    }
  }
}





#else

static struct {
  uint32_t mask0;
  uint32_t mask1;
  bfin_isr_t *head;
} vectors[CEC_INTERRUPT_COUNT];

static uint32_t globalMask0;
static uint32_t globalMask1;

static rtems_isr interruptHandler(rtems_vector_number vector) {
  bfin_isr_t *isr = NULL;
  uint32_t sourceMask0 = 0;
  uint32_t sourceMask1 = 0;
  rtems_interrupt_level isrLevel;

  rtems_interrupt_disable(isrLevel);
  vector -= CEC_INTERRUPT_BASE_VECTOR;
  if (vector >= 0 && vector < CEC_INTERRUPT_COUNT) {
    isr = vectors[vector].head;
    sourceMask0 = *(uint32_t volatile *) SIC_ISR &
        *(uint32_t volatile *) SIC_IMASK;
    sourceMask1 = *(uint32_t volatile *) (SIC_ISR + SIC_ISR_PITCH) &
        *(uint32_t volatile *) (SIC_IMASK + SIC_IMASK_PITCH);
    while (isr) {
      if ((sourceMask0 & isr->mask0) || (sourceMask1 & isr->mask1)) {
        isr->isr(isr->_arg);
        sourceMask0 = *(uint32_t volatile *) SIC_ISR &
            *(uint32_t volatile *) SIC_IMASK;
        sourceMask1 = *(uint32_t volatile *) (SIC_ISR + SIC_ISR_PITCH) &
            *(uint32_t volatile *) (SIC_IMASK + SIC_IMASK_PITCH);
      }
      isr = isr->next;
    }
  }
  rtems_interrupt_enable(isrLevel);
}

/**
 * Initializes the interrupt module
 */
void bfin_interrupt_init(void) {
  int source;
  int vector;
  uint32_t r;
  int i;
  int j;

  globalMask0 = ~(uint32_t) 0;
  globalMask1 = ~(uint32_t) 0;
  *(uint32_t volatile *) SIC_IMASK = 0;
  *(uint32_t volatile *) (SIC_IMASK + SIC_IMASK_PITCH) = 0;

  memset(vectors, 0, sizeof(vectors));
  /* build mask0 showing what SIC sources drive each CEC vector */
  source = 0;

  /**
   * The bf52x has 8 IAR registers but they do not have a constant pitch.
   *
   */
  for (i = 0; i < SIC_IAR_COUNT; i++) {
    if ( SIC_IAR_COUNT_SET0 > i ) {
      r = *(uint32_t volatile *) (SIC_IAR_BASE_ADDRESS + i * SIC_IAR_PITCH);
    } else {
      r = *(uint32_t volatile *) (SIC_IAR_BASE_ADDRESS_0 +
          ((i-SIC_IAR_COUNT_SET0) * SIC_IAR_PITCH));
    }
    for (j = 0; j < 8; j++) {
      vector = r & 0x0f;
      if (vector >= 0 && vector < CEC_INTERRUPT_COUNT) {
        /* install our local handler */
        if (vectors[vector].mask0 == 0 && vectors[vector].mask1 == 0){
          set_vector(interruptHandler, vector + CEC_INTERRUPT_BASE_VECTOR, 1);
        }
        if ( SIC_ISR0_MAX > source ) {
          vectors[vector].mask0 |= (1 << source);
        } else {
          vectors[vector].mask1 |= (1 << (source - SIC_ISR0_MAX));
        }
      }
      r >>= 4;
      source++;
    }
  }
}

/* modify SIC_IMASK based on ISR list for a particular CEC vector */
static void setMask(uint32_t vector) {
  bfin_isr_t *isr = NULL;
  uint32_t mask = 0;
  uint32_t r    = 0;

  mask = 0;
  isr = vectors[vector].head;
  while (isr) {
    mask |= isr->mask0;
    isr = isr->next;
  }
  r = *(uint32_t volatile *) SIC_IMASK;
  r &= ~vectors[vector].mask0;
  r |= mask;
  r &= globalMask0;
  *(uint32_t volatile *) SIC_IMASK = r;


  mask = 0;
  isr = vectors[vector].head;
  while (isr) {
    mask |= isr->mask1;
    isr = isr->next;
  }
  r = *(uint32_t volatile *) (SIC_IMASK+ SIC_IMASK_PITCH);
  r &= ~vectors[vector].mask1;
  r |= mask;
  r &= globalMask1;
  *(uint32_t volatile *) (SIC_IMASK+ SIC_IMASK_PITCH) = r;
}

/* add an ISR to the list for whichever vector it belongs to */
rtems_status_code bfin_interrupt_register(bfin_isr_t *isr) {
  bfin_isr_t *walk;
  rtems_interrupt_level isrLevel;

  /* find the appropriate vector */
  for (isr->vector = 0; isr->vector < CEC_INTERRUPT_COUNT; isr->vector++)
    if ( (vectors[isr->vector].mask0 & (1 << isr->source) ) || \
        (vectors[isr->vector].mask1 & (1 << (isr->source - SIC_ISR0_MAX)) ))
      break;
  if (isr->vector < CEC_INTERRUPT_COUNT) {
    isr->next = NULL;
    isr->mask0 = 0;
    isr->mask1 = 0;
    rtems_interrupt_disable(isrLevel);
    /* find the current end of the list */
    walk = vectors[isr->vector].head;
    while (walk && walk->next)
      walk = walk->next;
    /* append new isr to list */
    if (walk)
      walk->next = isr;
    else
      vectors[isr->vector].head = isr;
    rtems_interrupt_enable(isrLevel);
  } else
    /* we failed, but make vector a legal value so other calls into
	       this module with this isr descriptor won't do anything bad */
    isr->vector = 0;
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bfin_interrupt_unregister(bfin_isr_t *isr) {
  bfin_isr_t *walk, *prev;
  rtems_interrupt_level isrLevel;

  rtems_interrupt_disable(isrLevel);
  walk = vectors[isr->vector].head;
  prev = NULL;
  /* find this isr in our list */
  while (walk && walk != isr) {
    prev = walk;
    walk = walk->next;
  }
  if (walk) {
    /* if found, remove it */
    if (prev)
      prev->next = walk->next;
    else
      vectors[isr->vector].head = walk->next;
    /* fix up SIC_IMASK if necessary */
    setMask(isr->vector);
  }
  rtems_interrupt_enable(isrLevel);
  return RTEMS_SUCCESSFUL;
}

void bfin_interrupt_enable(bfin_isr_t *isr, bool enable) {
  rtems_interrupt_level isrLevel;

  rtems_interrupt_disable(isrLevel);
  if ( SIC_ISR0_MAX > isr->source ) {
    isr->mask0 = enable ? (1 << isr->source) : 0;
    *(uint32_t volatile *) SIC_IMASK |= isr->mask0;
  }  else {
    isr->mask1 = enable ? (1 << (isr->source - SIC_ISR0_MAX)) : 0;
    *(uint32_t volatile *) (SIC_IMASK + SIC_IMASK_PITCH) |= isr->mask1;
  }

  //setMask(isr->vector);
  rtems_interrupt_enable(isrLevel);
}

void bfin_interrupt_enable_all(int source, bool enable) {
  rtems_interrupt_level isrLevel;
  int vector;
  bfin_isr_t *walk;

  for (vector = 0; vector < CEC_INTERRUPT_COUNT; vector++)
    if ( (vectors[vector].mask0 & (1 << source) ) || \
        (vectors[vector].mask1 & (1 << (source - SIC_ISR0_MAX)) ))
      break;
  if (vector < CEC_INTERRUPT_COUNT) {
    rtems_interrupt_disable(isrLevel);
    walk = vectors[vector].head;
    while (walk) {
      walk->mask0 = enable ? (1 << source) : 0;
      walk = walk->next;
    }

    walk = vectors[vector].head;
    while (walk) {
      walk->mask1 = enable ? (1 << (source - SIC_ISR0_MAX)) : 0;
      walk = walk->next;
    }
    setMask(vector);
    rtems_interrupt_enable(isrLevel);
  }
}

void bfin_interrupt_enable_global(int source, bool enable) {
  int vector;
  rtems_interrupt_level isrLevel;

  for (vector = 0; vector < CEC_INTERRUPT_COUNT; vector++)
    if ( (vectors[vector].mask0 & (1 << source) ) || \
        (vectors[vector].mask1 & (1 << (source - SIC_ISR0_MAX)) ))
      break;
  if (vector < CEC_INTERRUPT_COUNT) {
    rtems_interrupt_disable(isrLevel);
    if ( SIC_ISR0_MAX > source ) {
      if (enable)
        globalMask0 |= 1 << source;
      else
        globalMask0 &= ~(1 << source);
    }else {
      if (enable)
        globalMask1 |= 1 << (source - SIC_ISR0_MAX);
      else
        globalMask1 &= ~(1 << (source - SIC_ISR0_MAX));
    }
    setMask(vector);
    rtems_interrupt_enable(isrLevel);
  }
}

#endif
