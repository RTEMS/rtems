/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *  
 *  Instantiate the clock driver shell.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <rtems.h>
#include <bsp/irq.h>
#include <bsp.h>

#include <stdio.h>
#include <stdlib.h>

#include "yamon_api.h"


/* #define CLOCK_DRIVER_USE_FAST_IDLE 1 */

#define CLOCK_VECTOR TX4938_IRQ_TMR0

#define TX4938_TIMER_INTERVAL_MODE 1

#define TX4938_TIMER_MODE TX4938_TIMER_INTERVAL_MODE

#if (TX4938_TIMER_MODE == TX4938_TIMER_INTERVAL_MODE)
#define TX4938_TIMER_INTERRUPT_FLAG TIIS
#define Clock_driver_support_initialize_hardware() \
          Initialize_timer0_in_interval_mode()
#else
#error "Build Error: unsupported timer mode"
#endif

void new_brk_esr(void);

t_yamon_retfunc esr_retfunc = 0;
t_yamon_ref original_brk_esr = 0;
t_yamon_ref original_tmr0_isr = 0;

void new_brk_esr(void)
{
	if (original_tmr0_isr)
	{
		YAMON_FUNC_DEREGISTER_IC_ISR( original_tmr0_isr );
		original_tmr0_isr = 0;
	}
	if (esr_retfunc)
		esr_retfunc();
}


#define Clock_driver_support_install_isr( _new ) \
  do { \
    rtems_interrupt_handler_install( \
      CLOCK_VECTOR, \
      "clock", \
      0, \
      _new, \
      NULL \
    ); \
    YAMON_FUNC_REGISTER_IC_ISR(17,(t_yamon_isr)_new,0,&original_tmr0_isr); /* Call Yamon to enable interrupt */ \
  } while(0)


#define Clock_driver_support_at_tick() \
  do { \
	uint32_t interrupt_flag; \
  	do { \
  		int loop_count; \
  		TX4938_REG_WRITE( TX4938_REG_BASE, TX4938_TIMER0_BASE + TX4938_TIMER_TISR, 0x0 ); /* Clear timer 0 interrupt */ \
    		loop_count = 0; \
    		do { /* Wait until interrupt flag is cleared (this prevents re-entering interrupt) */ \
    		 	/* Read back interrupt status register and isolate interval timer flag */ \
    			interrupt_flag = TX4938_REG_READ( TX4938_REG_BASE, TX4938_TIMER0_BASE + TX4938_TIMER_TISR ) & TX4938_TIMER_INTERRUPT_FLAG; \
    		} while (interrupt_flag && (++loop_count < 10)); /* Loop while timer interrupt bit is set, or loop count is lees than 10 */ \
  	} while(interrupt_flag); \
  } while(0)


/* Setup timer in interval mode to generate peiodic interrupts */
#define Initialize_timer0_in_interval_mode() \
  do { \
  	uint32_t temp; \
    TX4938_REG_WRITE( TX4938_REG_BASE, TX4938_TIMER0_BASE + TX4938_TIMER_TCR, 0x0 ); /* Disable timer */ \
    TX4938_REG_WRITE( TX4938_REG_BASE, TX4938_TIMER0_BASE + TX4938_TIMER_CCDR, 0x0 ); /* Set register for divide by 2 clock */ \
    TX4938_REG_WRITE( TX4938_REG_BASE, TX4938_TIMER0_BASE + TX4938_TIMER_ITMR, TIMER_CLEAR_ENABLE_MASK ); /* Set interval timer mode register */ \
    TX4938_REG_WRITE( TX4938_REG_BASE, TX4938_TIMER0_BASE + TX4938_TIMER_CPRA, 0x3d090 ); /* Set tmier period ,10.0 msec (25 MHz timer clock) */ \
    TX4938_REG_WRITE( TX4938_REG_BASE, TX4938_TIMER0_BASE + TX4938_TIMER_TCR, 0xC0 ); /* Enable timer in interval mode */ \
    temp = TX4938_REG_READ( TX4938_REG_BASE, TX4938_TIMER0_BASE + TX4938_TIMER_ITMR ); /* Enable interval timer interrupts */ \
    temp |= TIMER_INT_ENABLE_MASK; \
    TX4938_REG_WRITE( TX4938_REG_BASE, TX4938_TIMER0_BASE + TX4938_TIMER_ITMR, temp ); \
  } while(0)


#define CLOCK_DRIVER_USE_DUMMY_TIMECOUNTER

#include "../../../shared/dev/clock/clockimpl.h"
