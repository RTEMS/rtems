/*  Clock_init()
 *
 *  This routine initializes the Tick Timer 2 on the MVME162 board.
 *  The tick frequency is 1 millisecond.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  Modifications of respective RTEMS file: COPYRIGHT (c) 1994.
 *  EISCAT Scientific Association. M.Savitski
 *
 *  This material is a part of the MVME162 Board Support Package
 *  for the RTEMS executive. Its licensing policies are those of the
 *  RTEMS above.
 *
 *  $Id$
 */

#include <stdlib.h>

#include <rtems.h>
#include <bsp.h>
#include <clockdrv.h>

#define MS_COUNT          1000            /* T2's countdown constant (1 ms) */
#define CLOCK_INT_LEVEL   6               /* T2's interrupt level */

rtems_unsigned32 Clock_isrs;                  /* ISRs until next tick */
volatile rtems_unsigned32 Clock_driver_ticks; /* ticks since initialization */
rtems_isr_entry  Old_ticker;

rtems_device_driver Clock_initialize( 
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp,
  rtems_id tid,
  rtems_unsigned32 *rval
)
{
  Install_clock( Clock_isr );
}

void ReInstall_clock(rtems_isr_entry clock_isr)
{
  rtems_unsigned32 isrlevel;

  rtems_interrupt_disable( isrlevel );
  (void) set_vector( clock_isr, VBR0 * 0x10 + 0x9, 1 );
  rtems_interrupt_enable( isrlevel );
}

void Install_clock(rtems_isr_entry clock_isr )
{

  Clock_driver_ticks = 0;
  Clock_isrs = BSP_Configuration.microseconds_per_tick / 1000;

  if ( BSP_Configuration.ticks_per_timeslice ) {
    Old_ticker = 
      (rtems_isr_entry) set_vector( clock_isr, VBR0 * 0x10 + 0x9, 1 );
    lcsr->vector_base |= MASK_INT;   /* unmask VMEchip2 interrupts */
    lcsr->to_ctl = 0xE7;             /* prescaler to 1 MHz (see Appendix A1) */
    lcsr->timer_cmp_2 = MS_COUNT;
    lcsr->timer_cnt_2 = 0;           /* clear counter */
    lcsr->board_ctl |= 0x700;        /* increment, reset-on-compare, and */
                                     /*   clear-overflow-cnt */

    lcsr->intr_level[0] |= CLOCK_INT_LEVEL * 0x10;      /* set int level */
    lcsr->intr_ena |= 0x02000000;       /* enable tick timer 2 interrupt */

    atexit( Clock_exit );
  } 

}

void Clock_exit( void )
{
/* Dummy for now. See other m68k BSP's for code examples */
}
