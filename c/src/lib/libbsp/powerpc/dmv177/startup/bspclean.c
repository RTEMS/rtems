/*
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

void bsp_cleanup( void )
{
#if 0
  asm volatile( "li  10,99" );  /* 0x63 */
  asm volatile( "sc" );
#endif
}
