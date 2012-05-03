/*
 *  Install trap handlers for handling software interrupts.
 *  This file is deprecated, as the trap handlers are needed before this 
 *  function is called. We still use this as for debugging purposes.
 *
 *  Copyright 2010 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/asm.h>
#include <rtems/score/sparc64.h>
#include <rtems/bspIo.h>

void sparc64_install_isr_entries( void )
{
  return;
}
