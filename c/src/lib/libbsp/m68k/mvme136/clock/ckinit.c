/*  Clock_init()
 *
 *  This routine initializes the Z80386 1 on the MVME136 board.
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
 *  $Id$
 */

#include <stdlib.h>

#include <rtems.h>
#include <bsp.h>
#include <clockdrv.h>
#include <z8036.h>

#define MICRVAL     0xe2            /* disable lower chain, no vec */
                                    /*  set right justified addr */
                                    /*  and master int enable */
#define MCCRVAL     0xc4            /* enable T1 and port B */
                                    /*   timers independent */
#define MS_COUNT    0x07d0          /* T1's countdown constant (1 ms) */
#define T1MSRVAL    0x80            /* T1 cont. cycle/pulse output */
#define T1CSRVAL    0xc6            /* enable interrupt, allow and */
                                    /*   and trigger countdown */

rtems_unsigned32 Clock_isrs;        /* ISRs until next tick */
volatile rtems_unsigned32 Clock_driver_ticks;
                                    /* ticks since initialization */
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

void ReInstall_clock(
  rtems_isr_entry clock_isr
)
{
  rtems_unsigned32 isrlevel;

  rtems_interrupt_disable( isrlevel );
   (void) set_vector( clock_isr, 66, 1 );
  rtems_interrupt_enable( isrlevel );
}

void Install_clock(
  rtems_isr_entry clock_isr
)
{
  volatile struct z8036_map *timer;

  Clock_driver_ticks = 0;
  Clock_isrs = BSP_Configuration.microseconds_per_tick / 1000;

  if ( BSP_Configuration.ticks_per_timeslice ) {
    Old_ticker = (rtems_isr_entry) set_vector( clock_isr, 66, 1 );
    timer = (struct z8036_map *) 0xfffb0000;
    timer->MASTER_INTR        = MICRVAL;
    timer->CT1_MODE_SPEC      = T1MSRVAL;

  *((rtems_unsigned16 *)0xfffb0016) = MS_COUNT;  /* write countdown value */
/*
    timer->CT1_TIME_CONST_MSB = (MS_COUNT >> 8);
    timer->CT1_TIME_CONST_LSB = (MS_COUNT &  0xff);
*/
    timer->MASTER_CFG         = MCCRVAL;
    timer->CT1_CMD_STATUS     = T1CSRVAL;

/*
 * Enable interrupt via VME interrupt mask register
 */
    (*(rtems_unsigned8 *)0xfffb0038) &= 0xfd;


    atexit( Clock_exit );
  }

}

void Clock_exit( void )
{
  volatile struct z8036_map *timer;

  if ( BSP_Configuration.ticks_per_timeslice ) {
    timer = (struct z8036_map *) 0xfffb0000;
    timer->MASTER_INTR        = 0x62;
    timer->CT1_MODE_SPEC      = 0x00;
    timer->MASTER_CFG         = 0xf4;
    timer->CT1_CMD_STATUS     = 0x00;
    /* do not restore old vector */
  }
}
