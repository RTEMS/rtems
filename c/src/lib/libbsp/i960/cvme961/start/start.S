/*  entry.s
 *
 *  This file contains the entry point for the application.
 *  The name of this entry point is compiler dependent.
 *  It jumps to the BSP which is responsible for performing
 *  all initialization.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include "asm.h"

        BEGIN_CODE
        PUBLIC(start)                 # GNU960 default entry point

SYM(start):
        mov     3, r12
        modpc   r12, r12, r12         # enable tracing/trace faults
        mov     g5, g5                # NOP
        mov     0, g14                # initialize constant for C

        /*
         * zero out uninitialized data area
         */
zerobss:
        lda     _end, r4        /* find end of .bss */
        lda     _bss_start, r5  /* find beginning of .bss */
        ldconst 0, r6

loop:   st      r6, (r5)        /* to zero out uninitialized */
        addo    4, r5, r5       /* data area                 */
        cmpobl  r5, r4, loop    /* loop until _end reached   */

/* set up stack pointer:
 *   The heap will begin at '_end';  its length is 'heap_size'
 *   bytes.  The stack will begin at the first 64-byte-aligned
 *   block after the heap.
 *
 *   A default value of 'heap_size' is set by linking with libnindy.a
 *   The default can be overridden by redefining this symbol at link
 *   time (with a line of the form 'heap_size=XXXX;' in the lnk960
 *   linker specification file; or one of the form
 *   "-defsym heap_size=XXXX" on the gld960 invocation line).
 */

        ldconst _end, sp         /* set sp = address of end of heap */
        lda     heap_size(sp),sp
        lda     64(sp), sp       /* Now round up to 64-byte boundary */
        ldconst 0xffffffc0, r12
        and     r12, sp, sp
        st      sp, _stack_start /* Save for brk() routine */

        call    init_frames
        ret                      /* return to monitor */

init_frames:
        mov     0, g14           /* initialize constant for C */
        ldconst 0x3b001000, g0
        ldconst 0x00009107, g1
        modac   g1, g0, g0       /* set AC controls */

        /*
         * remember the frame, so that we can set it up if necessary
         */

        st      fp, _start_frame

        /*
         * Call application boot_card.
         *      Someday, real values of argc and argv will be set up.
         *      For now, they are set to 0.
         */
        ldconst 0,g0
        ldconst 0,g1
        ldconst 0,g2
        call    _boot_card
        ret

END_CODE

         BEGIN_DATA

        PUBLIC(_start_frame)
        PUBLIC(start_frame)
SYM (_start_frame):
SYM (start_frame):
        .word   0                  # addr of first user frame: for gdb960

        PUBLIC(_stack_start)
        PUBLIC(stack_start)
SYM (_stack_start):
SYM (stack_start):
        .word   0                  # addr of first user frame: for gdb960

END_DATA

BEGIN_BSS
        PUBLIC(heap_size)
        .set    heap_size,0x2000
END_BSS

END
