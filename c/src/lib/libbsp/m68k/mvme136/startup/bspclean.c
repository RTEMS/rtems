/*
 *  This routine returns control to 135Bug.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>
#include <clockdrv.h>
#include <zilog/z8036.h>

void bsp_return_to_monitor_trap()
{
  extern void start( void  );

  register volatile void *start_addr;

  m68k_set_vbr( 0 );                /* restore 135Bug vectors */
  asm volatile( "trap   #15"  );    /* trap to 135Bug */
  asm volatile( ".short 0x63" );    /* return to 135Bug (.RETURN) */
                                    /* restart program */
  start_addr = start;

  asm volatile ( "jmp %0@" : "=a" (start_addr) : "0" (start_addr) );
}

#define TIMER   0xfffb0000

void bsp_cleanup( void )
{
   Z8x36_WRITE( TIMER, MASTER_INTR,    0x62 );   /* redo timer */
   Z8x36_WRITE( TIMER, CT1_MODE_SPEC,  0x00 );
   Z8x36_WRITE( TIMER, MASTER_CFG,     0xf4 );
   Z8x36_WRITE( TIMER, CT1_CMD_STATUS, 0x00 );

   M68Kvec[ 45 ] = bsp_return_to_monitor_trap;   /* install handler */
   asm volatile( "trap #13" );  /* insures SUPV mode */
}
