/*  Spurious_driver
 *
 *  This routine installs spurious interrupt handlers for the DMV152.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993.
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
#include <stdio.h>

rtems_isr Spurious_Isr(
  rtems_vector_number vector
)
{
  void *sp = 0;

  asm volatile ( "movea.l   %%sp,%0 " : "=a" (sp) : "0" (sp) );

  fprintf( stderr, "Vector 0x%x sp=0x%p\n", vector, sp );
}

rtems_device_driver Spurious_Initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp,
  rtems_id tid,
  rtems_unsigned32 *rval
)
{
  rtems_vector_number vector;

  for ( vector = 0x40 ; vector <= 0xFF ; vector++ )
    (void) set_vector( Spurious_Isr, vector, 1 );

  return 0;
}
