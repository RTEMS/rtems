/*
 *  This routine returns control to 147Bug.
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  MVME147 port for TNI - Telecom Bretagne
 *  by Dominique LE CAMPION (Dominique.LECAMPION@enst-bretagne.fr)
 *  May 1996
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/clockdrv.h>

extern void start( void  );

void bsp_return_to_monitor_trap(void)
{
  register volatile void *start_addr;

  m68k_set_vbr( 0 );                /* restore 147Bug vectors */
  __asm__ volatile( "trap   #15"  );    /* trap to 147Bug */
  __asm__ volatile( ".short 0x63" );    /* return to 147Bug (.RETURN) */
                                    /* restart program */
  start_addr = start;

  __asm__ volatile ( "jmp %0@" : "=a" (start_addr) : "0" (start_addr) );
}

void bsp_cleanup(
  uint32_t status
)
{
   pcc->timer1_int_control = 0; /* Disable Timer 1 */
   pcc->timer2_int_control = 0; /* Disable Timer 2 */

   M68Kvec[ 45 ] = bsp_return_to_monitor_trap;   /* install handler */
   __asm__ volatile( "trap #13" );  /* ensures SUPV mode */
}
