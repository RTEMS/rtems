/*
 *  This routine returns control to 162Bug.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  Modifications of respective RTEMS file: COPYRIGHT (c) 1994.
 *  EISCAT Scientific Association. M.Savitski
 *
 *  This material is a part of the MVME162 Board Support Package
 *  for the RTEMS executive. Its licensing policies are those of the
 *  RTEMS above.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <rtems/zilog/z8036.h>
#include <page_table.h>

static rtems_isr bsp_return_to_monitor_trap(
  rtems_vector_number vector
)
{
  page_table_teardown();

  lcsr->intr_ena = 0;                    /* disable interrupts */
  m68k_set_vbr(MOT_162BUG_VEC_ADDRESS);  /* restore 162Bug vectors */

  __asm__ volatile( "trap   #15"  );    /* trap to 162Bug */
  __asm__ volatile( ".short 0x63" );    /* return to 162Bug (.RETURN) */
}

void bsp_fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code error
)
{
   M68Kvec[ 45 ] = bsp_return_to_monitor_trap;   /* install handler */
   __asm__ volatile( "trap #13" );               /* ensures SUPV mode */
}
