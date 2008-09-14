/*
 *  This is a dummy bsp_cleanup routine.
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

#include <bsp.h>
#include <bsp/bootcard.h>
#include <nds.h>

/*
 * reset the platform using bios call.
 */
void bsp_reset (void)
{
  swiSoftReset ();
}

void bsp_cleanup( void )
{
  printk ("[!] executive ended, rebooting\n");

  bsp_reset ();
}
