/* SPDX-License-Identifier: BSD-2-Clause */

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
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <rtems/asm.h>

#if (M68K_COLDFIRE_ARCH == 0) /* All ColdFire BSPs must provide their own start vector */

BEGIN_CODE
                                        | Default entry points for:
         PUBLIC (start)                 |   GNU
         PUBLIC (M68Kvec)               |   Vector Table

SYM (start):
SYM (M68Kvec):                          | standard location for vectors
        nop                             | for linkers with problem
                                        | location zero
        jmp      SYM (start_around)

     /*
      *  We can use the following space as our vector table
      *  if the CPU has a VBR or we can save vector table in it
      *  if the CPU does not.
      */

        .space   4088                   | to avoid initial intr stack
                                        |   from 135BUG on MVME13?
                                        |   and start code at 0x4000
SYM (vectors):
        .space   1016                   | reserve space for rest of vectors

#if ( M68K_HAS_SEPARATE_STACKS == 1 )
SYM (lowintstack):
        .space   4092                   | reserve for interrupt stack
SYM (hiintstack):
        .space   4                      | end of interrupt stack
#endif

	PUBLIC (start_around)
SYM (start_around):
        move.w  sr, SYM (initial_sr)
        oriw    #0x3700,sr              | SUPV MODE,INTERRUPTS OFF!!!
#if ( M68K_HAS_SEPARATE_STACKS == 1 )
        movec   isp,a0
        move.l  a0, SYM (initial_isp)
        movec   usp,a0
        move.l  a0, SYM (initial_usp)
        movec   msp,a0
        move.l  a0, SYM (initial_msp)
#else
        move.l  a7, SYM (initial_msp)
#endif

        |
        | zero out uninitialized data area
        |
zerobss:
        moveal  # SYM (bsp_section_bss_end),a0     | find end of .bss
        moveal  # SYM (bsp_section_bss_begin),a1   | find beginning of .bss
        movel   #0,d0

loop:   movel   #0,a1@+                 | to zero out uninitialized
        cmpal   a0,a1
        jlt     loop                    | loop until _end reached

        movel   # SYM (_ISR_Stack_area_end),d0 | d0 = stop of stack
        movw    #0x3700,sr              | SUPV MODE,INTERRUPTS OFF!!!
        movel   d0,a7                   | set master stack pointer
        movel   d0,a6                   | set base pointer

      /*
       *  RTEMS should maintain a separate interrupt stack on CPUs
       *  without one in hardware.  This is currently not supported
       *  on versions of the m68k without a HW intr stack.
       */

#if ( M68K_HAS_SEPARATE_STACKS == 1 )
        lea     SYM (hiintstack),a0   | a0 = high end of intr stack
        movec   a0,isp                | set interrupt stack
#endif

        movel   #0,a7@-               | push command line
        jsr     SYM (boot_card)
        addl    #12,a7

#if ( M68K_HAS_SEPARATE_STACKS == 1 )
        move.l  SYM (initial_isp),a0
        movec   a0,isp
        move.l  SYM (initial_usp),a0
        movec   a0,usp
        move.l  SYM (initial_msp),a0
        movec   a0,msp
#else
        movea.l SYM (initial_msp),a7
#endif
        move.w  SYM (initial_sr),sr
        rts

END_CODE

BEGIN_DATA

	PUBLIC (start_frame)
SYM (start_frame):
        .space  4,0

END_DATA

BEGIN_BSS

	PUBLIC (initial_isp)
SYM (initial_isp):
        .space  4

	PUBLIC (initial_msp)
SYM (initial_msp):
        .space  4

	PUBLIC (initial_usp)
SYM (initial_usp):
        .space  4

         PUBLIC (initial_sr)
SYM (initial_sr):
        .space  2

END_DATA
#endif
END
