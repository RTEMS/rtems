/*  rtems.s
 *
 *  This file contains the single entry point code for
 *  the SPARC port of RTEMS.
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
 *  {
 *  }
 */

        .align 4
        PUBLIC(RTEMS)
SYM(RTEMS):
        ret

