/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp/bootcard.h>
#include <bsp.h>

void bsp_reset( void )
{
  TRAP0(SYS_exit, 0, 0, 0);
}
