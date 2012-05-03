/*
 *  window.s
 *
 *  This file contains the register window management routines for the
 *  SPARC architecture.  Trap handlers for the following capabilities
 *  are included in this file:
 *
 *    + Window Overflow
 *    + Window Underflow
 *    + Flushing All Windows
 *
 *  COPYRIGHT:
 *
 *  This file includes the window overflow and underflow handlers from
 *  the file srt0.s provided with the binary distribution of the SPARC
 *  Instruction Simulator (SIS) found at
 *  ftp://ftp.estec.esa.nl/pub/ws/wsd/erc32.
 *
 *  COPYRIGHT (c) 1995. European Space Agency.
 *
 *  This terms of the RTEMS license apply to this file.
 */

#include <rtems/asm.h>

        .seg    "text"
        /*
         *  Window overflow trap handler.
         *
         *  On entry:
         *
         *    l0 = psr (from trap table)
         *    l1 = pc
         *    l2 = npc
         */

        PUBLIC(window_overflow_trap_handler)

SYM(window_overflow_trap_handler):

        /*
         *  Calculate new WIM by "rotating" the valid bits in the WIM right
         *  by one position.  The following shows how the bits move for a SPARC
         *  cpu implementation where SPARC_NUMBER_OF_REGISTER_WINDOWS is 8.
         *
         *    OLD WIM = 76543210
         *    NEW WIM = 07654321
         *
         *  NOTE: New WIM must be stored in a global register since the
         *        "save" instruction just prior to the load of the wim
         *        register will result in the local register set changing.
         */

        mov  %wim, %l3                   ! l3 = WIM
        mov  %g1, %l7                    ! save g1
        srl  %l3, 1, %g1                 ! g1 = WIM >> 1
        sll  %l3, SPARC_NUMBER_OF_REGISTER_WINDOWS-1 , %l4
                                         ! l4 = WIM << (Number Windows - 1)
        or   %l4, %g1, %g1               ! g1 = (WIM >> 1) |
                                         !      (WIM << (Number Windows - 1))

        save                             ! Get into window to be saved.
        mov  %g1, %wim                   ! load new WIM
        nop; nop; nop                    ! 3 slot delay
        std  %l0, [%sp + 0x00]           ! save local register set
        std  %l2, [%sp + 0x08]
        std  %l4, [%sp + 0x10]
        std  %l6, [%sp + 0x18]
        std  %i0, [%sp + 0x20]           ! save input register set
        std  %i2, [%sp + 0x28]
        std  %i4, [%sp + 0x30]
        std  %i6, [%sp + 0x38]
        restore                          ! Go back to trap window.
        mov  %l7, %g1                    ! restore g1
        jmp  %l1                         ! Re-execute save.
        rett %l2

        /*
         *  Window underflow trap handler.
         *
         *  On entry:
         *
         *    l0 = psr (from trap table)
         *    l1 = pc
         *    l2 = npc
         */

        PUBLIC(window_underflow_trap_handler)

SYM(window_underflow_trap_handler):

        /*
         *  Calculate new WIM by "rotating" the valid bits in the WIM left
         *  by one position.  The following shows how the bits move for a SPARC
         *  cpu implementation where SPARC_NUMBER_OF_REGISTER_WINDOWS is 8.
         *
         *    OLD WIM = 76543210
         *    NEW WIM = 07654321
         *
         *  NOTE: New WIM must be stored in a global register since the
         *        "save" instruction just prior to the load of the wim
         *        register will result in the local register set changing.
         */

        mov  %wim, %l3                  ! Calculate new WIM
        sll  %l3, 1, %l4                ! l4 = WIM << 1
        srl  %l3, SPARC_NUMBER_OF_REGISTER_WINDOWS-1, %l5
                                        ! l5 = WIM >> (Number Windows-1)
        or   %l5, %l4, %l5              ! l5 = (WIM << 1) |
                                        !      (WIM >> (Number Windows-1))
        mov  %l5, %wim                  ! load the new WIM
        nop; nop; nop
        restore                         ! Two restores to get into the
        restore                         ! window to restore
        ldd  [%sp + 0x00], %l0          ! First the local register set
        ldd  [%sp + 0x08], %l2
        ldd  [%sp + 0x10], %l4
        ldd  [%sp + 0x18], %l6
        ldd  [%sp + 0x20], %i0          ! Then the input registers
        ldd  [%sp + 0x28], %i2
        ldd  [%sp + 0x30], %i4
        ldd  [%sp + 0x38], %i6
        save                            ! Get back to the trap window.
        save
        jmp  %l1                        ! Re-execute restore.
        rett  %l2

        /*
         *  Flush All Windows trap handler.
         *
         *  Flush all windows with valid contents except the current one
         *  and the one we will be returning to.
         *
         *  In examining the set register windows, one may logically divide
         *  the windows into sets (some of which may be empty) based on their
         *  current status:
         *
         *    + current (i.e. in use),
         *    + used (i.e. a restore would not trap)
         *    + invalid (i.e. 1 in corresponding bit in WIM)
         *    + unused
         *
         *  Either the used or unused set of windows may be empty.
         *
         *  NOTE: We assume only one bit is set in the WIM at a time.
         *
         *  Given a CWP of 5 and a WIM of 0x1, the registers are divided
         *  into sets as follows:
         *
         *    + 0   - invalid
         *    + 1-4 - unused
         *    + 5   - current
         *    + 6-7 - used
         *
         *  In this case, we only would save the used windows which we
         *  will not be returning to -- 6.
         *
         *    Register Usage while saving the windows:
         *      g1 = current PSR
         *      g2 = current wim
         *      g3 = CWP
         *      g4 = wim scratch
         *      g5 = scratch
         *
         *  On entry:
         *
         *    l0 = psr (from trap table)
         *    l1 = pc
         *    l2 = npc
         */

        PUBLIC(window_flush_trap_handler)

SYM(window_flush_trap_handler):
        /*
         *  Save the global registers we will be using
         */

        mov     %g1, %l3
        mov     %g2, %l4
        mov     %g3, %l5
        mov     %g4, %l6
        mov     %g5, %l7

        mov     %l0, %g1                      ! g1 = psr
        mov     %wim, %g2                     ! g2 = wim
        and     %l0, SPARC_PSR_CWP_MASK, %g3  ! g3 = CWP

        add     %g3, 1, %g5                   ! g5 = CWP + 1
        and     %g5, SPARC_NUMBER_OF_REGISTER_WINDOWS - 1, %g5

        mov     1, %g4
        sll     %g4, %g5, %g4                 ! g4 = WIM mask for CWP+1 invalid

        restore                               ! go back one register window

save_frame_loop:
        sll     %g4, 1, %g5                   ! rotate the "wim" left 1
        srl     %g4, SPARC_NUMBER_OF_REGISTER_WINDOWS - 1, %g4
        or      %g4, %g5, %g4                 ! g4 = wim if we do one restore

        /*
         *  If a restore would not underflow, then continue.
         */

        andcc   %g4, %g2, %g0                 ! Any windows to flush?
        bnz     done_flushing                 ! No, then continue
        nop

        restore                               ! back one window

        /*
         *  Now save the window just as if we overflowed to it.
         */

        std     %l0, [%sp + CPU_STACK_FRAME_L0_OFFSET]
        std     %l2, [%sp + CPU_STACK_FRAME_L2_OFFSET]
        std     %l4, [%sp + CPU_STACK_FRAME_L4_OFFSET]
        std     %l6, [%sp + CPU_STACK_FRAME_L6_OFFSET]

        std     %i0, [%sp + CPU_STACK_FRAME_I0_OFFSET]
        std     %i2, [%sp + CPU_STACK_FRAME_I2_OFFSET]
        std     %i4, [%sp + CPU_STACK_FRAME_I4_OFFSET]
        std     %i6, [%sp + CPU_STACK_FRAME_I6_FP_OFFSET]

        ba      save_frame_loop
        nop

done_flushing:

        add     %g3, 2, %g3                   ! calculate desired WIM
        and     %g3, SPARC_NUMBER_OF_REGISTER_WINDOWS - 1, %g3
        mov     1, %g4
        sll     %g4, %g3, %g4                 ! g4 = new WIM
        mov     %g4, %wim

        mov     %g1, %psr                     ! restore PSR
        nop
        nop
        nop

        /*
         *  Restore the global registers we used
         */

        mov     %l3, %g1
        mov     %l4, %g2
        mov     %l5, %g3
        mov     %l6, %g4
        mov     %l7, %g5

        jmpl    %l2, %g0
        rett    %l2 + 4

/* end of file */
