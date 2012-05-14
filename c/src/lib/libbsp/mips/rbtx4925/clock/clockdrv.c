/**
 *  @file
 *  
 *  Instantiate the clock driver shell.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp/irq.h>
#include <bsp.h>

/* #define CLOCK_DRIVER_USE_FAST_IDLE */

#define CLOCK_VECTOR TX4925_IRQ_TMR0

#define TX4925_TIMER_INTERVAL_MODE 1
#define TX4925_TIMER_PULSE_MODE 2
#define TX4925_TIMER_MODE TX4925_TIMER_INTERVAL_MODE

#if (TX4925_TIMER_MODE == TX4925_TIMER_INTERVAL_MODE)
#define TX4925_TIMER_INTERRUPT_FLAG TIIS
#define Clock_driver_support_initialize_hardware() \
          Initialize_timer0_in_interval_mode()
#elif (TX4925_TIMER_MODE == TX4925_TIMER_PULSE_MODE)
#define TX4925_TIMER_INTERRUPT_FLAG TPIBS
#define Clock_driver_support_initialize_hardware() \
          Initialize_timer0_in_pulse_mode()
#else
#error "Build Error: need to select timer mode"
#endif


#define Clock_driver_support_install_isr( _new, _old ) \
  do { \
    rtems_interrupt_handler_install( \
      CLOCK_VECTOR, \
      "clock", \
      0, \
      _new, \
      NULL \
    ); \
 } while(0)


#define Clock_driver_support_at_tick() \
  do { \
	uint32_t interrupt_flag; \
  	do { \
  		int loop_count; \
  		TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_TIMER0_BASE + TX4925_TIMER_TISR, 0x0 ); /* Clear timer 0 interrupt */ \
    		loop_count = 0; \
    		do { /* Wait until interrupt flag is cleared (this prevents re-entering interrupt) */ \
    		 	/* Read back interrupt status register and isolate interval timer flag */ \
    			interrupt_flag = TX4925_REG_READ( TX4925_REG_BASE, TX4925_TIMER0_BASE + TX4925_TIMER_TISR ) & TX4925_TIMER_INTERRUPT_FLAG; \
    		} while (interrupt_flag && (++loop_count < 10)); /* Loop while timer interrupt bit is set, or loop count is lees than 10 */ \
  	} while(interrupt_flag); \
  } while(0)


/* Setup timer in interval mode to generate peiodic interrupts */
#define Initialize_timer0_in_interval_mode() \
  do { \
  	uint32_t temp; \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_TIMER0_BASE + TX4925_TIMER_TCR, 0x0 ); /* Disable timer */ \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_TIMER0_BASE + TX4925_TIMER_CCDR, 0x0 ); /* Set register for divide by 2 clock */ \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_TIMER0_BASE + TX4925_TIMER_ITMR, TIMER_CLEAR_ENABLE_MASK ); /* Set interval timer mode register */ \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_TIMER0_BASE + TX4925_TIMER_CPRA, 0x30d40 ); /* Set tmier period ,10.0 msec (20 MHz timer clock) */ \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_TIMER0_BASE + TX4925_TIMER_TCR, 0xC0 ); /* Enable timer in interval mode */ \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_IRQCTL_DM0, 0x0 ); /* Set interrupt controller detection mode */ \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_IRQCTL_LVL2, 0x1000000 ); /* Set interrupt controller level */ \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_IRQCTL_MSK, 0x0 ); /* Set interrupt controller mask */ \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_IRQCTL_DEN, 0x1 ); /* Enable interrupts from controller */ \
    temp = TX4925_REG_READ( TX4925_REG_BASE, TX4925_TIMER0_BASE + TX4925_TIMER_ITMR ); /* Enable interval timer interrupts */ \
    temp |= TIMER_INT_ENABLE_MASK; \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_TIMER0_BASE + TX4925_TIMER_ITMR, temp ); \
  } while(0)


/* This mode is used to generate periodic interrupts and also output a pulse on PIO20 pin */
#define Initialize_timer0_in_pulse_mode() \
  do { \
  	uint32_t temp; \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_TIMER0_BASE + TX4925_TIMER_TCR, 0x0 ); /* Disable timer */ \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_TIMER0_BASE + TX4925_TIMER_CCDR, 0x0 ); /* Set register for divide by 2 clock */ \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_TIMER0_BASE + TX4925_TIMER_PGMR, FFI ); /* Set pulse generator mode register */ \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_TIMER0_BASE + TX4925_TIMER_CPRA, 0x3e8 ); /* Set pulse high duration ,0.05 msec (20 MHz timer clock) */ \
/*    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_TIMER0_BASE + TX4925_TIMER_CPRB, 0x1388 ); */ /* Set pulse total period, 0.25 msec (20 MHz timer clock) */ \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_TIMER0_BASE + TX4925_TIMER_CPRB, 0x30d40 ); /* Set pulse total period, 10 msec (20 MHz timer clock) */ \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_TIMER0_BASE + TX4925_TIMER_TCR, 0xC1 ); /* Enable timer in pulse generator mode */ \
 \
 	/* Enable timer 0 output pulses on PIO20 */ \
    temp = TX4925_REG_READ( TX4925_REG_BASE, TX4925_CFG_PCFG ); \
    temp = (temp & ~ SELCHI) | SELTMR0;	/* Enable timer 0 pulses on PIO20 */ \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_CFG_PCFG, temp ); \
 \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_IRQCTL_DM0, 0x0 ); /* Set interrupt controller detection mode */ \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_IRQCTL_LVL2, 0x1000000 ); /* Set interrupt controller level */ \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_IRQCTL_MSK, 0x0 ); /* Set interrupt controller mask */ \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_IRQCTL_DEN, 0x1 ); /* Enable interrupts from controller */ \
    temp = TX4925_REG_READ( TX4925_REG_BASE, TX4925_TIMER0_BASE + TX4925_TIMER_PGMR ); /* Enable pulse generator interrupt */ \
    temp |= TPIBE;	/* Only want interrupts on B compare (where clock count is cleared) */ \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_TIMER0_BASE + TX4925_TIMER_PGMR, temp ); \
  } while(0)


#define Clock_driver_support_shutdown_hardware() \
  do { \
  	uint32_t temp; \
    temp = TX4925_REG_READ( TX4925_REG_BASE, TX4925_TIMER0_BASE + TX4925_TIMER_ITMR ); /* Disable interval timer interrupt */ \
    temp &= ~TIMER_INT_ENABLE_MASK; \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_TIMER0_BASE + TX4925_TIMER_ITMR, temp ); \
    temp = TX4925_REG_READ( TX4925_REG_BASE, TX4925_TIMER0_BASE + TX4925_TIMER_PGMR ); /* Disable pulse generator interrupt */ \
    temp &= ~(TPIAE | TPIBE); \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_TIMER0_BASE + TX4925_TIMER_PGMR, temp ); \
    TX4925_REG_WRITE( TX4925_REG_BASE, TX4925_TIMER0_BASE + TX4925_TIMER_TCR, 0x0 ); /* Disable timer */ \
  } while(0)


#include "../../../shared/clockdrv_shell.h"
