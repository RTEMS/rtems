/*  _load_segments
 *
 *  This file assists the board independent startup code by
 *  loading the proper segment register values.  The values
 *  loaded are board dependent.
 *
 *  NOTE:  No stack has been established when this routine
 *         is invoked.  It returns by jumping back to bspentry.
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

#include "asm.h"

BEGIN_CODE

/*
 *  FORCEBUG loads us into a virtual address space which
 *  really starts at PHYSICAL_ADDRESS_BASE.
 */

.set PHYSICAL_ADDRESS_BASE,    0x00002000

/*
 *  At reset time, FORCEBUG normally has the segment selectors preloaded.
 *  If a human resets the instruction pointer, this will not have occurred.
 *  However, no guarantee can be made of the other registers if cs:ip was
 *  modified to restart the program.  Because of this, the BSP reloads all
 *  segment registers (except cs) with the values they have following
 *  a reset.
 */


.set RESET_SS, 0x40        # initial value of stack segment register
.set RESET_DS, 0x40        # initial value of data segment register
.set RESET_ES, 0x40        # initial value of extra segment register
.set RESET_FS, 0x40        # initial value of "f" segment register
.set RESET_GS, 0x30        # initial value of "g" segment register


#define LOAD_SEGMENTS(_value,_segment) \
        movw      $ ## _value, ax ; \
        movw      ax, _segment

        EXTERN (establish_stack)

        PUBLIC (_load_segments)
SYM (_load_segments):
        LOAD_SEGMENTS( RESET_SS, ss )
        LOAD_SEGMENTS( RESET_DS, ds )
        LOAD_SEGMENTS( RESET_ES, es )
        LOAD_SEGMENTS( RESET_FS, fs )
        LOAD_SEGMENTS( RESET_GS, gs )

        jmp     SYM (_establish_stack)  # return to the bsp entry code

        PUBLIC (_return_to_monitor)
SYM (_return_to_monitor):

        call    SYM (Clock_exit)
        movb    $0,al
        int     $0x20                   # restart FORCEbug
        jmp     SYM (start)             # FORCEbug does not reset PC

END_CODE

BEGIN_DATA

        PUBLIC (_Do_Load_IDT)
SYM (_Do_Load_IDT):
       .byte 1

        PUBLIC (_Do_Load_GDT)
SYM (_Do_Load_GDT):
       .byte 0

END_DATA
END
