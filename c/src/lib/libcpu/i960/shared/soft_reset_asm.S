/*
 *  void __i960_soft_reset_asm
 *  
 *  Flush the register cache and save the important (fp, pfp, sp) registers,
 *  which are clobbered by the reinit operation. (Not documented, but it
 *  happens).  
 *
 *  This may not be used any longer but it is not being purged completely
 *  in the move to multilib things.
 *
 *  $Id$
 */

        .data
        .align 4
_soft_reset_reg_save:
        .word  0
        .word  0
        .word  0
        .word  0
 
                .globl __i960_soft_reset_asm
__i960_soft_reset_asm: 
                flushreg                                                                # flush register cache                                                                  mov                     fp, r4
                mov                     pfp, r5
                mov                     sp, r6
                stt                     r4, _soft_reset_reg_save    # save fp, pfp, sp
                lda                     __i960_reset_done, r4
                ldconst         0x300, r5
                sysctl          r5, r4, g0                  # reinit: clobbers almost all registers
__i960_reset_done:
                ldt             _soft_reset_reg_save, r4    # restore fp, pfp, sp
                mov                     r4, fp
                mov                     r5, pfp
                mov                     r6, sp
                ret

