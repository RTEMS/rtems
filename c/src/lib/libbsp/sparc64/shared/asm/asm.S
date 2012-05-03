/*
 * Copyright (c) 2010
 * Gedare Bloom
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#include <rtems/asm.h>

.text

PUBLIC(halt)
  .global halt
  SYM(halt):
	b halt
	nop
