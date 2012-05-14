/*
 *  This routine returns control to 162Bug.
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Modifications of respective RTEMS file: COPYRIGHT (c) 1994.
 *  EISCAT Scientific Association. M.Savitski
 *
 *  This material is a part of the MVME162 Board Support Package
 *  for the RTEMS executive. Its licensing policies are those of the
 *  RTEMS above.
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/zilog/z8036.h>
#include <page_table.h>

extern void start( void  );

void bsp_return_to_monitor_trap(void)
{
  page_table_teardown();

  lcsr->intr_ena = 0;                    /* disable interrupts */
  m68k_set_vbr(MOT_162BUG_VEC_ADDRESS);  /* restore 162Bug vectors */

  __asm__ volatile( "trap   #15"  );    /* trap to 162Bug */
  __asm__ volatile( ".short 0x63" );    /* return to 162Bug (.RETURN) */
                                    /* restart program */
  /*
   *  This does not work on the 162....
   */
#if 0
  {  register volatile void *start_addr;

     start_addr = start;

     __asm__ volatile ( "jmp %0@" : "=a" (start_addr) : "0" (start_addr) );
  }
#endif
}

void bsp_cleanup(
  uint32_t status
)
{
   M68Kvec[ 45 ] = bsp_return_to_monitor_trap;   /* install handler */
   __asm__ volatile( "trap #13" );  /* insures SUPV mode */
}
