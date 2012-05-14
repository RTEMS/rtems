/*  Support for Blackfin interrupt controller
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */


#include <rtems.h>
#include <rtems/libio.h>

#include <bsp.h>
#include <libcpu/cecRegs.h>
#include <libcpu/sicRegs.h>
#include "interrupt.h"


static struct {
  uint32_t mask;
  bfin_isr_t *head;
} vectors[CEC_INTERRUPT_COUNT];

static uint32_t globalMask;


static rtems_isr interruptHandler(rtems_vector_number vector) {
  bfin_isr_t *isr;
  uint32_t sourceMask;

  vector -= CEC_INTERRUPT_BASE_VECTOR;
  if (vector >= 0 && vector < CEC_INTERRUPT_COUNT) {
    isr = vectors[vector].head;
    sourceMask = *(uint32_t volatile *) SIC_ISR &
                 *(uint32_t volatile *) SIC_IMASK;
    while (isr) {
      if (sourceMask & isr->mask) {
        isr->isr(isr->source);
        sourceMask = *(uint32_t volatile *) SIC_ISR &
                     *(uint32_t volatile *) SIC_IMASK;
      }
      isr = isr->next;
    }
  }
}

void bfin_interrupt_init(void) {
  int source;
  int vector;
  uint32_t r;
  int i;
  int j;

  globalMask = ~(uint32_t) 0;
  *(uint32_t volatile *) SIC_IMASK = 0;
  memset(vectors, 0, sizeof(vectors));
  /* build mask showing what SIC sources drive each CEC vector */
  source = 0;
  for (i = 0; i < SIC_IAR_COUNT; i++) {
    r = *(uint32_t volatile *) (SIC_IAR_BASE_ADDRESS + i * SIC_IAR_PITCH);
    for (j = 0; j < 8; j++) {
      vector = r & 0x0f;
      if (vector >= 0 && vector < CEC_INTERRUPT_COUNT) {
        if (vectors[vector].mask == 0)
          /* install our local handler */
          set_vector(interruptHandler, vector + CEC_INTERRUPT_BASE_VECTOR, 1);
        vectors[vector].mask |= (1 << source);
      }
      r >>= 4;
      source++;
    }
  }
}

/* modify SIC_IMASK based on ISR list for a particular CEC vector */
static void setMask(int vector) {
  bfin_isr_t *isr;
  uint32_t mask;
  uint32_t r;

  mask = 0;
  isr = vectors[vector].head;
  while (isr) {
    mask |= isr->mask;
    isr = isr->next;
  }
  r = *(uint32_t volatile *) SIC_IMASK;
  r &= ~vectors[vector].mask;
  r |= mask;
  r &= globalMask;
  *(uint32_t volatile *) SIC_IMASK = r;
}

/* add an ISR to the list for whichever vector it belongs to */
void bfin_interrupt_register(bfin_isr_t *isr) {
  bfin_isr_t *walk;
  rtems_interrupt_level isrLevel;

  /* find the appropriate vector */
  for (isr->vector = 0; isr->vector < CEC_INTERRUPT_COUNT; isr->vector++)
    if (vectors[isr->vector].mask & (1 << isr->source))
      break;
  if (isr->vector < CEC_INTERRUPT_COUNT) {
    isr->next = NULL;
    isr->mask = 0;
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
}

void bfin_interrupt_unregister(bfin_isr_t *isr) {
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
}

void bfin_interrupt_enable(bfin_isr_t *isr, bool enable) {
  rtems_interrupt_level isrLevel;

  rtems_interrupt_disable(isrLevel);
  isr->mask = enable ? (1 << isr->source) : 0;
  setMask(isr->vector);
  rtems_interrupt_enable(isrLevel);
}

void bfin_interrupt_enable_all(int source, bool enable) {
  rtems_interrupt_level isrLevel;
  int vector;
  bfin_isr_t *walk;

  for (vector = 0; vector < CEC_INTERRUPT_COUNT; vector++)
    if (vectors[vector].mask & (1 << source))
      break;
  if (vector < CEC_INTERRUPT_COUNT) {
    rtems_interrupt_disable(isrLevel);
    walk = vectors[vector].head;
    while (walk) {
      walk->mask = enable ? (1 << source) : 0;
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
    if (vectors[vector].mask & (1 << source))
      break;
  if (vector < CEC_INTERRUPT_COUNT) {
    rtems_interrupt_disable(isrLevel);
    if (enable)
      globalMask |= 1 << source;
    else
      globalMask &= ~(1 << source);
    setMask(vector);
    rtems_interrupt_enable(isrLevel);
  }
}

