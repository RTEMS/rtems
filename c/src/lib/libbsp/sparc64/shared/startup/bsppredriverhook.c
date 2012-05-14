/*
 *  This is a bsp_predriver_hook routine for sparc64
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp/bootcard.h>

extern void sparc64_install_isr_entries(void);

void bsp_predriver_hook( void )
{
  sparc64_install_isr_entries();
}
