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
        moveal  # SYM (_clear_end),a0     | find end of .bss
        moveal  # SYM (_clear_start),a1   | find beginning of .bss
        movel   #0,d0

loop:   movel   #0,a1@+                 | to zero out uninitialized
        cmpal   a0,a1
        jlt     loop                    | loop until _end reached

        movel   # SYM (_stack_init),d0  | d0 = stop of stack
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
