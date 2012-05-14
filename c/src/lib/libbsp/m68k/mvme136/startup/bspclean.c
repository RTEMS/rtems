/*
 *  This routine returns control to 135Bug.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/clockdrv.h>
#include <rtems/zilog/z8036.h>

extern void start( void  );

void bsp_return_to_monitor_trap(void)
{
  register volatile void *start_addr;

  m68k_set_vbr( 0 );                /* restore 135Bug vectors */
  __asm__ volatile( "trap   #15"  );    /* trap to 135Bug */
  __asm__ volatile( ".short 0x63" );    /* return to 135Bug (.RETURN) */
                                    /* restart program */
  start_addr = start;

  __asm__ volatile ( "jmp %0@" : "=a" (start_addr) : "0" (start_addr) );
}

#define TIMER   0xfffb0000

void bsp_cleanup(
  uint32_t status
)
{
   Z8x36_WRITE( TIMER, MASTER_INTR,    0x62 );   /* redo timer */
   Z8x36_WRITE( TIMER, CT1_MODE_SPEC,  0x00 );
   Z8x36_WRITE( TIMER, MASTER_CFG,     0xf4 );
   Z8x36_WRITE( TIMER, CT1_CMD_STATUS, 0x00 );

   M68Kvec[ 45 ] = bsp_return_to_monitor_trap;   /* install handler */
   __asm__ volatile( "trap #13" );  /* insures SUPV mode */
}
