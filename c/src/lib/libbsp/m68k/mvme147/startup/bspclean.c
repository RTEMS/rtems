/*
 *  This routine returns control to 147Bug.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  MVME147 port for TNI - Telecom Bretagne
 *  by Dominique LE CAMPION (Dominique.LECAMPION@enst-bretagne.fr)
 *  May 1996
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>
#include <clockdrv.h>

void bsp_return_to_monitor_trap()
{
  extern void start( void  );

  register volatile void *start_addr;

  m68k_set_vbr( 0 );                /* restore 147Bug vectors */
  asm volatile( "trap   #15"  );    /* trap to 147Bug */
  asm volatile( ".short 0x63" );    /* return to 147Bug (.RETURN) */
                                    /* restart program */
  start_addr = start;

  asm volatile ( "jmp %0@" : "=a" (start_addr) : "0" (start_addr) );
}

void bsp_cleanup( void )
{
   pcc->timer1_int_control = 0; /* Disable Timer 1 */
   pcc->timer2_int_control = 0; /* Disable Timer 2 */

   M68Kvec[ 45 ] = bsp_return_to_monitor_trap;   /* install handler */
   asm volatile( "trap #13" );  /* insures SUPV mode */
}
