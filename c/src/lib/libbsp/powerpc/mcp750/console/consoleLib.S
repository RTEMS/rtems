/*
 *  consoleLib.S  -- console I/O package utility functions.
 *
 *  Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 * $Id$
 */

/* A few utility functions, some copied from arch/ppc/lib/string.S */

#include <libcpu/cpu.h>
#include <rtems/score/targopts.h>
#include "asm.h"
		
	.text
	
        .globl  strlen
	.type	strlen,@function
strlen:
        addi    r4,r3,-1
1:      lbzu    r0,1(r4)
        cmpwi   0,r0,0
        bne     1b
        subf    r3,r3,r4
        blr
