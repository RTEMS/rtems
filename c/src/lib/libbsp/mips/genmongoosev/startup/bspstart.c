/**
 *  @file
 *  
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  Modification History:
 *        12/10/01  A.Ferrer, NASA/GSFC, Code 582
 *           Set interrupt mask to 0xAF00 (Line 139).
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
 
#include <string.h>

#include <bsp.h>
#include <libcpu/mongoose-v.h>
#include <libcpu/isr_entries.h>
#include <bsp/irq-generic.h>

void bsp_start( void );
void clear_cache( void );
extern void _sys_exit(int);
extern void mips_gdb_stub_install(void);

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */
void bsp_start( void )
{
 /* mask off any interrupts */
 MONGOOSEV_WRITE( MONGOOSEV_PERIPHERAL_FUNCTION_INTERRUPT_MASK_REGISTER, 0 );

 /* reset the config register & clear any pending peripheral interrupts */
 MONGOOSEV_WRITE( MONGOOSEV_PERIPHERAL_COMMAND_REGISTER, 0 );
 MONGOOSEV_WRITE(
   MONGOOSEV_PERIPHERAL_COMMAND_REGISTER, MONGOOSEV_UART_CMD_RESET_BOTH_PORTS );
 MONGOOSEV_WRITE( MONGOOSEV_PERIPHERAL_COMMAND_REGISTER, 0 );

 /* reset both timers */
 MONGOOSEV_WRITE_REGISTER(
   MONGOOSEV_TIMER1_BASE, MONGOOSEV_TIMER_INITIAL_COUNTER_REGISTER, 0xffffffff);
 MONGOOSEV_WRITE_REGISTER(
   MONGOOSEV_TIMER1_BASE, MONGOOSEV_TIMER_CONTROL_REGISTER, 0);

 MONGOOSEV_WRITE_REGISTER(
   MONGOOSEV_TIMER2_BASE, MONGOOSEV_TIMER_INITIAL_COUNTER_REGISTER, 0xffffffff);
 MONGOOSEV_WRITE_REGISTER(
   MONGOOSEV_TIMER2_BASE, MONGOOSEV_TIMER_CONTROL_REGISTER, 0);

 /* clear any pending interrupts */
 MONGOOSEV_WRITE( MONGOOSEV_PERIPHERAL_STATUS_REGISTER, 0xffffffff );

 /* clear any writable bits in the cause register */
 mips_set_cause( 0 );

 /* set interrupt mask, but globally off. */

 /*
  *  Bit 15 | Bit 14 | Bit 13 | Bit 12 | Bit 11 | Bit 10 | Bit  9 | Bit  8 |
  *  periph | unused |  FPU   | unused | timer2 | timer1 | swint1 | swint2 |
  *  extern |        |        |        |        |        |        |        |
  *
  *    1        0        1        0        0        1        0        0
  *
  *    0x8C00   Enable only internal Mongoose V timers.
  *    0xA400   Enable Peripherial ints, FPU and timer1
  *    0x0400   Timer1 only
  */

  /* mips_set_sr( (SR_CU0 | SR_CU1 | 0xA400) ); */

  /* to start up, only enable coprocessor 0 & timer int. per-task
   * processor settings will be applied as they are created, this
   * is just to configure the processor for startup
   */
  mips_set_sr( (SR_CU0 | 0x400) );

  bsp_interrupt_initialize();
}

void clear_cache( void )
{
   promCopyIcacheFlush();
   promCopyDcacheFlush();
}
