/*  rtems.s
 *
 *  This file contains the single entry point code for
 *  the SPARC port of RTEMS.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space 
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995. 
 *  European Space Agency.
 *
 *  $Id$
 */

#include <asm.h>

/*
 *  RTEMS
 *
 *  This routine jumps to the directive indicated in the
 *  CPU defined register.  This routine is used when RTEMS is
 *  linked by itself and placed in ROM.  This routine is the
 *  first address in the ROM space for RTEMS.  The user "calls"
 *  this address with the directive arguments in the normal place.
 *  This routine then jumps indirectly to the correct directive
 *  preserving the arguments.  The directive should not realize
 *  it has been "wrapped" in this way.  The table "_Entry_points"
 *  is used to look up the directive.
 *  
 *  void RTEMS()
 */

        .align 4
        PUBLIC(RTEMS)
SYM(RTEMS):
        /*
         *  g2 was chosen because gcc uses it as a scratch register in 
         *  similar code scenarios and the other locals, ins, and outs
         *  are off limits to this routine unless it does a "save" and
         *  copies its in registers to the outs which only works up until
         *  6 parameters.  Best to take the simple approach in this case.
         */
        sethi     SYM(_Entry_points), %g2
        or        %g2, %lo(SYM(_Entry_points)), %g2
        sll       %g1, 2,  %g1
        add       %g1, %g2, %g2
        jmp       %g2
        nop

