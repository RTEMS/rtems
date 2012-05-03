/**
 *  @file
 *  
 *  Au1x00 Interrupt Vectoring
 */

/*
 *  Copyright (c) 2005 by Cogent Computer Systems
 *  Written by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <stdlib.h>
#include <libcpu/au1x00.h>
#include <rtems/irq.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>

void mips_vector_isr_handlers( CPU_Interrupt_frame *frame );
static void call_vectored_isr(CPU_Interrupt_frame *, uint32_t , void *);

#include <rtems/bspIo.h>  /* for printk */

void mips_vector_isr_handlers( CPU_Interrupt_frame *frame )
{
  unsigned int sr;
  unsigned int cause;

  mips_get_sr( sr );
  mips_get_cause( cause );

  cause &= (sr & SR_IMASK);
  cause >>= CAUSE_IPSHIFT;

  /* count/compare interrupt */
  if ( cause & 0x80 ) {
      unsigned long zero = 0;
      /*
       * I don't see a good way to disable the compare
       * interrupt, so let's just ignore it.
       */
      __asm__ volatile ("mtc0 %0, $11\n" :: "r" (zero));
  }

  /* Performance counter */
  if ( cause & 0x40 ) {
      bsp_interrupt_handler_dispatch(AU1X00_IRQ_PERF);
  }

  /* Interrupt controller 0 */
  if ( cause & 0x0c ) {
      call_vectored_isr(frame, cause, (void *)AU1X00_IC0_ADDR);
  }

  /* Interrupt controller 1 */
  if ( cause & 0x30 ) {
      call_vectored_isr(frame, cause, (void *)AU1X00_IC1_ADDR);
  }

  /* SW[0] */
  if ( cause & 0x01 )
      bsp_interrupt_handler_dispatch( AU1X00_IRQ_SW0 );

  /* SW[1] */
  if ( cause & 0x02 )
      bsp_interrupt_handler_dispatch( AU1X00_IRQ_SW1 );
}

void mips_default_isr( int vector )
{
  unsigned int sr;
  unsigned int cause;

  mips_get_sr( sr );
  mips_get_cause( cause );

  printk( "Unhandled isr exception: vector 0x%02x, cause 0x%08X, sr 0x%08X\n",
      vector, cause, sr );
  rtems_fatal_error_occurred(1);
}

static void call_vectored_isr(
    CPU_Interrupt_frame *frame,
    uint32_t cause,
    void *ctrlr
    )
{
    uint32_t src;
    uint32_t mask;
    int index;

    /* get mask register */
    mask = AU1X00_IC_MASKRD(ctrlr);

    /* check request 0 */
    src = AU1X00_IC_REQ0INT(ctrlr);
    src = src & mask;
    index = 0;
    while (src) {
        /* check LSB */
        if (src & 1) {
            /* clear rising/falling edge detects */
            AU1X00_IC_RISINGCLR(ctrlr) = (1 << index);
            AU1X00_IC_FALLINGCLR(ctrlr) = (1 << index);
            au_sync();
            bsp_interrupt_handler_dispatch(AU1X00_IRQ_IC0_BASE + index);
        }
        index ++;

        /* shift, and make sure MSB is clear */
        src = (src >> 1) & 0x7fffffff;
    }

    /* check request 1 */
    src = AU1X00_IC_REQ1INT(ctrlr);
    src = src & mask;
    index = 0;
    while (src) {
        /* check LSB */
        if (src & 1) {
            /* clear rising/falling edge detects */
            AU1X00_IC_RISINGCLR(ctrlr) = (1 << index);
            AU1X00_IC_FALLINGCLR(ctrlr) = (1 << index);
            au_sync();
            bsp_interrupt_handler_dispatch(AU1X00_IRQ_IC0_BASE + index);
        }
        index ++;

        /* shift, and make sure MSB is clear */
        src = (src >> 1) & 0x7fffffff;
    }
}

/* Generate a software interrupt */
int assert_sw_irq(uint32_t irqnum)
{
    uint32_t cause;

    if (irqnum <= 1) {
        mips_get_cause(cause);
        cause = cause | ((irqnum + 1) << CAUSE_IPSHIFT);
        mips_set_cause(cause);

        return irqnum;
    } else {
        return -1;
    }
}

/* Clear a software interrupt */
int negate_sw_irq(uint32_t irqnum)
{
    uint32_t cause;

    if (irqnum <= 1) {
        mips_get_cause(cause);
        cause = cause & ~((irqnum + 1) << CAUSE_IPSHIFT);
        mips_set_cause(cause);

        return irqnum;
    } else {
        return -1;
    }
}
