/*
 *  mcf52235 startup code
 *
 *  This file contains the entry point for the application.
 *  The name of this entry point is compiler dependent.
 *  It jumps to the BSP which is responsible for performing
 *  all initialization.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/asm.h>

.extern _StackInit

BEGIN_CODE

    PUBLIC (_INTERRUPT_VECTOR)
SYM(_INTERRUPT_VECTOR):

    .long   _StackInit /* 00 Initial 'SSP' */
    .long   SYM(start) /* 01 Initial PC */
    .long   SYM(_uhoh) /* 02 Access Error */
    .long   SYM(_uhoh) /* 03 Address Error */
    .long   SYM(_uhoh) /* 04 Illegal Instruction */
    .long   SYM(_uhoh) /* 05 Divide by Zero */
    .long   SYM(_uhoh) /* 06 Reserved */
    .long   SYM(_uhoh) /* 07 Reserved */
    .long   SYM(_uhoh) /* 08 Privilege Violation */
    .long   SYM(_uhoh) /* 09 Trace */
    .long   SYM(_uhoh) /* 10 Unimplemented A-Line */
    .long   SYM(_uhoh) /* 11 Unimplemented F-Line */
    .long   SYM(_uhoh) /* 12 Debug Interrupt */
    .long   SYM(_uhoh) /* 13 Reserved */
    .long   SYM(_uhoh) /* 14 Format Error */
    .long   SYM(_uhoh) /* 15 Reserved */
    .long   SYM(_uhoh) /* 16 Reserved */
    .long   SYM(_uhoh) /* 17 Reserved */
    .long   SYM(_uhoh) /* 18 Reserved */
    .long   SYM(_uhoh) /* 19 Reserved */
    .long   SYM(_uhoh) /* 20 Reserved */
    .long   SYM(_uhoh) /* 21 Reserved */
    .long   SYM(_uhoh) /* 22 Reserved */
    .long   SYM(_uhoh) /* 23 Reserved */
    .long   SYM(_spuriousInterrupt) /* 24 Spurious Interrupt */
    .long   SYM(_uhoh) /* Reserved */
    .long   SYM(_uhoh) /* Reserved */
    .long   SYM(_uhoh) /* Reserved */
    .long   SYM(_uhoh) /* Reserved */
    .long   SYM(_uhoh) /* Reserved */
    .long   SYM(_uhoh) /* Reserved */
    .long   SYM(_uhoh) /* Reserved */
    .long   SYM(_uhoh) /* 32 TRAP #0 */
    .long   SYM(_uhoh) /* 33 TRAP #1 */
    .long   SYM(_uhoh) /* 34 TRAP #2 */
    .long   SYM(_uhoh) /* 35 TRAP #3 */
    .long   SYM(_uhoh) /* 36 TRAP #4 */
    .long   SYM(_uhoh) /* 37 TRAP #5 */
    .long   SYM(_uhoh) /* 38 TRAP #6 */
    .long   SYM(_uhoh) /* 39 TRAP #7 */
    .long   SYM(_uhoh) /* 40 TRAP #8 */
    .long   SYM(_uhoh) /* 41 TRAP #9 */
    .long   SYM(_uhoh) /* 42 TRAP #10 */
    .long   SYM(_uhoh) /* 43 TRAP #11 */
    .long   SYM(_uhoh) /* 44 TRAP #12 */
    .long   SYM(_uhoh) /* 45 TRAP #13 */
    .long   SYM(_uhoh) /* 46 TRAP #14 */
    .long   SYM(_uhoh) /* 47 TRAP #15 */
    .long   SYM(_uhoh) /* 48 Reserved */
    .long   SYM(_uhoh) /* 49 Reserved */
    .long   SYM(_uhoh) /* 50 Reserved */
    .long   SYM(_uhoh) /* 51 Reserved */
    .long   SYM(_uhoh) /* 52 Reserved */
    .long   SYM(_uhoh) /* 53 Reserved */
    .long   SYM(_uhoh) /* 54 Reserved */
    .long   SYM(_uhoh) /* 55 Reserved */
    .long   SYM(_uhoh) /* 56 Reserved */
    .long   SYM(_uhoh) /* 57 Reserved */
    .long   SYM(_uhoh) /* 58 Reserved */
    .long   SYM(_uhoh) /* 59 Reserved */
    .long   SYM(_uhoh) /* 60 Reserved */
    .long   SYM(_uhoh) /* 61 Reserved */
    .long   SYM(_uhoh) /* 62 Reserved */
    .long   SYM(_uhoh) /* 63 Reserved */

    /* INTC0 */

    .long   SYM(_uhoh) /* 64*/
    .long   SYM(_uhoh) /* 65*/
    .long   SYM(_uhoh) /* 66*/
    .long   SYM(_uhoh) /* 67*/
    .long   SYM(_uhoh) /* 68*/
    .long   SYM(_uhoh) /* 69*/
    .long   SYM(_uhoh) /* 70*/
    .long   SYM(_uhoh) /* 71*/
    .long   SYM(_uhoh) /* 72*/
    .long   SYM(_uhoh) /* 73*/
    .long   SYM(_uhoh) /* 74*/
    .long   SYM(_uhoh) /* 75*/
    .long   SYM(_uhoh) /* 76*/
    .long   SYM(_uhoh) /* 77*/
    .long   SYM(_uhoh) /* 78*/
    .long   SYM(_uhoh) /* 79*/
    .long   SYM(_uhoh) /* 80*/
    .long   SYM(_uhoh) /* 81*/
    .long   SYM(_uhoh) /* 82*/
    .long   SYM(_uhoh) /* 83*/
    .long   SYM(_uhoh) /* 84*/
    .long   SYM(_uhoh) /* 85*/
    .long   SYM(_uhoh) /* 86*/
    .long   SYM(_uhoh) /* 87*/
    .long   SYM(_uhoh) /* 88*/
    .long   SYM(_uhoh) /* 89*/
    .long   SYM(_uhoh) /* 90*/
    .long   SYM(_uhoh) /* 91*/
    .long   SYM(_uhoh) /* 92*/
    .long   SYM(_uhoh) /* 93*/
    .long   SYM(_uhoh) /* 94*/
    .long   SYM(_uhoh) /* 95*/
    .long   SYM(_uhoh) /* 96*/
    .long   SYM(_uhoh) /* 97*/
    .long   SYM(_uhoh) /* 98*/
    .long   SYM(_uhoh) /* 99*/
    .long   SYM(_uhoh) /* 100*/
    .long   SYM(_uhoh) /* 101*/
    .long   SYM(_uhoh) /* 102*/
    .long   SYM(_uhoh) /* 103*/
    .long   SYM(_uhoh) /* 104*/
    .long   SYM(_uhoh) /* 105*/
    .long   SYM(_uhoh) /* 106*/
    .long   SYM(_uhoh) /* 107*/
    .long   SYM(_uhoh) /* 108*/
    .long   SYM(_uhoh) /* 109*/
    .long   SYM(_uhoh) /* 110*/
    .long   SYM(_uhoh) /* 111*/
    .long   SYM(_uhoh) /* 112*/
    .long   SYM(_uhoh) /* 113*/
    .long   SYM(_uhoh) /* 114*/
    .long   SYM(_uhoh) /* 115*/
    .long   SYM(_uhoh) /* 116*/
    .long   SYM(_uhoh) /* 117*/
    .long   SYM(_uhoh) /* 118*/
    .long   SYM(_uhoh) /* 119*/
    .long   SYM(_uhoh) /* 120*/
    .long   SYM(_uhoh) /* 121*/
    .long   SYM(_uhoh) /* 122*/
    .long   SYM(_uhoh) /* 123*/
    .long   SYM(_uhoh) /* 124*/
    .long   SYM(_uhoh) /* 125*/
    .long   SYM(_uhoh) /* 126*/
    .long   SYM(_uhoh) /* 127*/

    /* INTC1 */

    .long   SYM(_uhoh) /* 128*/
    .long   SYM(_uhoh) /* 129*/
    .long   SYM(_uhoh) /* 130*/
    .long   SYM(_uhoh) /* 131*/
    .long   SYM(_uhoh) /* 132*/
    .long   SYM(_uhoh) /* 133*/
    .long   SYM(_uhoh) /* 134*/
    .long   SYM(_uhoh) /* 135*/
    .long   SYM(_uhoh) /* 136*/
    .long   SYM(_uhoh) /* 137*/
    .long   SYM(_uhoh) /* 138*/
    .long   SYM(_uhoh) /* 139*/
    .long   SYM(_uhoh) /* 140*/
    .long   SYM(_uhoh) /* 141*/
    .long   SYM(_uhoh) /* 142*/
    .long   SYM(_uhoh) /* 143*/
    .long   SYM(_uhoh) /* 144*/
    .long   SYM(_uhoh) /* 145*/
    .long   SYM(_uhoh) /* 146*/
    .long   SYM(_uhoh) /* 147*/
    .long   SYM(_uhoh) /* 148*/
    .long   SYM(_uhoh) /* 149*/
    .long   SYM(_uhoh) /* 150*/
    .long   SYM(_uhoh) /* 151*/
    .long   SYM(_uhoh) /* 152*/
    .long   SYM(_uhoh) /* 153*/
    .long   SYM(_uhoh) /* 154*/
    .long   SYM(_uhoh) /* 155*/
    .long   SYM(_uhoh) /* 156*/
    .long   SYM(_uhoh) /* 157*/
    .long   SYM(_uhoh) /* 158*/
    .long   SYM(_uhoh) /* 159*/
    .long   SYM(_uhoh) /* 160*/
    .long   SYM(_uhoh) /* 161*/
    .long   SYM(_uhoh) /* 162*/
    .long   SYM(_uhoh) /* 163*/
    .long   SYM(_uhoh) /* 164*/
    .long   SYM(_uhoh) /* 165*/
    .long   SYM(_uhoh) /* 166*/
    .long   SYM(_uhoh) /* 167*/
    .long   SYM(_uhoh) /* 168*/
    .long   SYM(_uhoh) /* 169*/
    .long   SYM(_uhoh) /* 170*/
    .long   SYM(_uhoh) /* 171*/
    .long   SYM(_uhoh) /* 172*/
    .long   SYM(_uhoh) /* 173*/
    .long   SYM(_uhoh) /* 174*/
    .long   SYM(_uhoh) /* 175*/
    .long   SYM(_uhoh) /* 176*/
    .long   SYM(_uhoh) /* 177*/
    .long   SYM(_uhoh) /* 178*/
    .long   SYM(_uhoh) /* 179*/
    .long   SYM(_uhoh) /* 180*/
    .long   SYM(_uhoh) /* 181*/
    .long   SYM(_uhoh) /* 182*/
    .long   SYM(_uhoh) /* 183*/
    .long   SYM(_uhoh) /* 184*/
    .long   SYM(_uhoh) /* 185*/
    .long   SYM(_uhoh) /* 186*/
    .long   SYM(_uhoh) /* 187*/
    .long   SYM(_uhoh) /* 188*/
    .long   SYM(_uhoh) /* 189*/
    .long   SYM(_uhoh) /* 190*/
    .long   SYM(_uhoh) /* 191*/
    .long   SYM(_uhoh) /* 192*/

    /* */

    .long   SYM(_uhoh) /* 193*/
    .long   SYM(_uhoh) /* 194*/
    .long   SYM(_uhoh) /* 195*/
    .long   SYM(_uhoh) /* 196*/
    .long   SYM(_uhoh) /* 197*/
    .long   SYM(_uhoh) /* 198*/
    .long   SYM(_uhoh) /* 199*/
    .long   SYM(_uhoh) /* 200*/
    .long   SYM(_uhoh) /* 201*/
    .long   SYM(_uhoh) /* 202*/
    .long   SYM(_uhoh) /* 203*/
    .long   SYM(_uhoh) /* 204*/
    .long   SYM(_uhoh) /* 205*/
    .long   SYM(_uhoh) /* 206*/
    .long   SYM(_uhoh) /* 207*/
    .long   SYM(_uhoh) /* 208*/
    .long   SYM(_uhoh) /* 209*/
    .long   SYM(_uhoh) /* 210*/
    .long   SYM(_uhoh) /* 211*/
    .long   SYM(_uhoh) /* 212*/
    .long   SYM(_uhoh) /* 213*/
    .long   SYM(_uhoh) /* 214*/
    .long   SYM(_uhoh) /* 215*/
    .long   SYM(_uhoh) /* 216*/
    .long   SYM(_uhoh) /* 217*/
    .long   SYM(_uhoh) /* 218*/
    .long   SYM(_uhoh) /* 219*/
    .long   SYM(_uhoh) /* 220*/
    .long   SYM(_uhoh) /* 221*/
    .long   SYM(_uhoh) /* 222*/
    .long   SYM(_uhoh) /* 223*/
    .long   SYM(_uhoh) /* 224*/
    .long   SYM(_uhoh) /* 225*/
    .long   SYM(_uhoh) /* 226*/
    .long   SYM(_uhoh) /* 227*/
    .long   SYM(_uhoh) /* 228*/
    .long   SYM(_uhoh) /* 229*/
    .long   SYM(_uhoh) /* 230*/
    .long   SYM(_uhoh) /* 231*/
    .long   SYM(_uhoh) /* 232*/
    .long   SYM(_uhoh) /* 233*/
    .long   SYM(_uhoh) /* 234*/
    .long   SYM(_uhoh) /* 235*/
    .long   SYM(_uhoh) /* 236*/
    .long   SYM(_uhoh) /* 237*/
    .long   SYM(_uhoh) /* 238*/
    .long   SYM(_uhoh) /* 239*/
    .long   SYM(_uhoh) /* 240*/
    .long   SYM(_uhoh) /* 241*/
    .long   SYM(_uhoh) /* 242*/
    .long   SYM(_uhoh) /* 243*/
    .long   SYM(_uhoh) /* 244*/
    .long   SYM(_uhoh) /* 245*/
    .long   SYM(_uhoh) /* 246*/
    .long   SYM(_uhoh) /* 247*/
    .long   SYM(_uhoh) /* 248*/
    .long   SYM(_uhoh) /* 249*/
    .long   SYM(_uhoh) /* 250*/
    .long   SYM(_uhoh) /* 251*/
    .long   SYM(_uhoh) /* 252*/
    .long   SYM(_uhoh) /* 253*/
    .long   SYM(_uhoh) /* 254*/
    .long   SYM(_uhoh) /* 255*/

/*
 * Default trap handler
 * With an oscilloscope you can see AS* stop
 */
.align 4
    PUBLIC (_uhoh)
SYM(_uhoh):
    nop                     | Leave spot for breakpoint
    stop    #0x2700         | Stop with interrupts disabled
    bra.w   SYM(_uhoh)      | Stuck forever

/*
 * Spurious Interrupt Handler
 */
.align 4
    PUBLIC (_spuriousInterrupt)
SYM(_spuriousInterrupt):
    addql   #1, SYM(_M68kSpuriousInterruptCount)
    rte

/*
 * Write VBR Register
 */
.align 4
    PUBLIC (_wr_vbr)
SYM(_wr_vbr):
    move.l  4(sp), d0
    movec   d0, vbr
    nop
    rts

/*
 * Board startup
 * Disable watchdog, interrupts
 * Enable sram
 */
.align 4
    PUBLIC (start)
SYM(start):

    /* Mask off interupts */
    move.w   #0x2700,sr

    /* Save off reset values of D0 and D1 */
    move.l  d0,d6
    move.l  d1,d7

    /* Initialize RAMBAR1: locate SRAM and validate it */
    move.l  #_CoreSRamBase,d0
    add.l   #0x221,d0
    movec   d0,%rambar

    /* Save off intial D0 and D1 to RAM */
    move.l  d6, SYM(_d0_reset)
    move.l  d7, SYM(_d1_reset)

    /* Locate Stack Pointer */
    move.l  #_StackInit,sp

    /*
     * Remainder of the startup code is handled by C code
     * This never returns
     */
    jmp SYM(Init5329)

END_CODE


BEGIN_DATA_DCL

    .align 4

PUBLIC (_M68kSpuriousInterruptCount)
SYM (_M68kSpuriousInterruptCount):
    .long   0

PUBLIC (_d0_reset)
SYM (_d0_reset):
    .long   0

PUBLIC (_d1_reset)
SYM (_d1_reset):
    .long   0

END_DATA_DCL

END

