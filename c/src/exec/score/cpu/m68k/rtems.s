/*  rtems.s
 *
 *  This file contains the single entry point code for
 *  the m68k implementation of RTEMS.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */


#include <asm.h>

/*
 *  There seems to be no reason to have two versions of this.
 *  The following version should work across the entire family.
 *  The worst assumption is that gcc will put entry in a scratch
 *  register and not screw up the stack.
 *
 *  NOTE:  This is a 68020 version:
 *
 *    jmpl    @(%%d0:l:4)@(__Entry_points)
 */

        EXTERN (_Entry_points)

        BEGIN_CODE

        .align  4
        .global SYM (RTEMS)

SYM (RTEMS):
        moveal    SYM (_Entry_points), a0
        lsll      #2, d0
        addal     d0, a0
        moveal    @(a0),a0
        jmpl      @(a0)

        END_CODE
END
