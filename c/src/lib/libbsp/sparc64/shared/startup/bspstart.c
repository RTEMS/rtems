/*
 *  This routine does the bulk of the system initialization.
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>

extern void sparc64_install_isr_entries(void);

void bsp_start( void )
{
  sparc64_install_isr_entries();
}
