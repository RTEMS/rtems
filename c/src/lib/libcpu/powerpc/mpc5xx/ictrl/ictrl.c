/*
 * mpc505/509 external interrupt controller management.
 */

#include "ictrl.h"

#include <rtems.h>
#include <rtems/score/powerpc.h>

/*
 * Internal routines.
 */

static unsigned long volatile *const IRQAND     =
              (unsigned long volatile *const)0x8007EFA4;

static void nullHandler()
{
}

/* Interrupt dispatch table. */
static ExtIsrHandler extIrqHandlers[NUM_IRQS] =
{
  nullHandler,
  nullHandler,
  nullHandler,
  nullHandler,
  nullHandler,
  nullHandler,
  nullHandler
};


/* RTEMS external interrupt handler. Calls installed external interrupt
   handlers for every pending external interrupt in turn. */
static rtems_isr extIsr_( rtems_vector_number i )
{
#define BIT_NUMBER(val, bit) \
    __asm__ volatile ( "cntlzw %0, %1; srawi %0, %0, 1": "=r" (bit) : "r" (val) );

  int bit;
  (void)i;

  BIT_NUMBER(*IRQAND & IMASK_ALL, bit);
  while ( bit < NUM_IRQS ) {
    extIrqHandlers[bit]();
    BIT_NUMBER(*IRQAND & IMASK_ALL, bit);
  }
}

/*
 * Public routines
 */

void extIrqSetHandler(ExtInt interrupt,ExtIsrHandler handler)
{
  extIrqHandlers[interrupt] = handler;
}

void extIsrInit( void )
{
  int i = 0;

  extIrqDisable(IMASK_ALL);
  for( i = 0; i < NUM_IRQS; i++)
    extIrqHandlers[i] = nullHandler;
  set_vector(extIsr_,PPC_IRQ_EXTERNAL,1);
}
