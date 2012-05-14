/*  cpu_asm.s
 *
 *  This file contains the basic algorithms for all assembly code used
 *  in an specific CPU port of RTEMS.  These algorithms must be implemented
 *  in assembly language.
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995.
 *  European Space Agency.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>
#include <rtems/system.h>

#if (SPARC_HAS_FPU == 1)

/*
 *  void _CPU_Context_save_fp(
 *    void **fp_context_ptr
 *  )
 *
 *  This routine is responsible for saving the FP context
 *  at *fp_context_ptr.  If the point to load the FP context
 *  from is changed then the pointer is modified by this routine.
 *
 *  NOTE: See the README in this directory for information on the
 *        management of the "EF" bit in the PSR.
 */

        .align 4
        PUBLIC(_CPU_Context_save_fp)
SYM(_CPU_Context_save_fp):
        save    %sp, -CPU_MINIMUM_STACK_FRAME_SIZE, %sp

        /*
         *  The following enables the floating point unit.
         */

        mov     %psr, %l0
        sethi   %hi(SPARC_PSR_EF_MASK), %l1
        or      %l1, %lo(SPARC_PSR_EF_MASK), %l1
        or      %l0, %l1, %l0
        mov     %l0, %psr                  ! **** ENABLE FLOAT ACCESS ****
	nop; nop; nop;			   ! Need three nops before EF is
        ld      [%i0], %l0		   ! active due to pipeline delay!!!
        std     %f0, [%l0 + FO_F1_OFFSET]
        std     %f2, [%l0 + F2_F3_OFFSET]
        std     %f4, [%l0 + F4_F5_OFFSET]
        std     %f6, [%l0 + F6_F7_OFFSET]
        std     %f8, [%l0 + F8_F9_OFFSET]
        std     %f10, [%l0 + F1O_F11_OFFSET]
        std     %f12, [%l0 + F12_F13_OFFSET]
        std     %f14, [%l0 + F14_F15_OFFSET]
        std     %f16, [%l0 + F16_F17_OFFSET]
        std     %f18, [%l0 + F18_F19_OFFSET]
        std     %f20, [%l0 + F2O_F21_OFFSET]
        std     %f22, [%l0 + F22_F23_OFFSET]
        std     %f24, [%l0 + F24_F25_OFFSET]
        std     %f26, [%l0 + F26_F27_OFFSET]
        std     %f28, [%l0 + F28_F29_OFFSET]
        std     %f30, [%l0 + F3O_F31_OFFSET]
        st      %fsr, [%l0 + FSR_OFFSET]
        ret
        restore

/*
 *  void _CPU_Context_restore_fp(
 *    void **fp_context_ptr
 *  )
 *
 *  This routine is responsible for restoring the FP context
 *  at *fp_context_ptr.  If the point to load the FP context
 *  from is changed then the pointer is modified by this routine.
 *
 *  NOTE: See the README in this directory for information on the
 *        management of the "EF" bit in the PSR.
 */

        .align 4
        PUBLIC(_CPU_Context_restore_fp)
SYM(_CPU_Context_restore_fp):
        save    %sp, -CPU_MINIMUM_STACK_FRAME_SIZE , %sp

        /*
         *  The following enables the floating point unit.
         */

        mov     %psr, %l0
        sethi   %hi(SPARC_PSR_EF_MASK), %l1
        or      %l1, %lo(SPARC_PSR_EF_MASK), %l1
        or      %l0, %l1, %l0
        mov     %l0, %psr                  ! **** ENABLE FLOAT ACCESS ****
	nop; nop; nop;			   ! Need three nops before EF is
        ld      [%i0], %l0		   ! active due to pipeline delay!!!
        ldd     [%l0 + FO_F1_OFFSET], %f0
        ldd     [%l0 + F2_F3_OFFSET], %f2
        ldd     [%l0 + F4_F5_OFFSET], %f4
        ldd     [%l0 + F6_F7_OFFSET], %f6
        ldd     [%l0 + F8_F9_OFFSET], %f8
        ldd     [%l0 + F1O_F11_OFFSET], %f10
        ldd     [%l0 + F12_F13_OFFSET], %f12
        ldd     [%l0 + F14_F15_OFFSET], %f14
        ldd     [%l0 + F16_F17_OFFSET], %f16
        ldd     [%l0 + F18_F19_OFFSET], %f18
        ldd     [%l0 + F2O_F21_OFFSET], %f20
        ldd     [%l0 + F22_F23_OFFSET], %f22
        ldd     [%l0 + F24_F25_OFFSET], %f24
        ldd     [%l0 + F26_F27_OFFSET], %f26
        ldd     [%l0 + F28_F29_OFFSET], %f28
        ldd     [%l0 + F3O_F31_OFFSET], %f30
        ld      [%l0 + FSR_OFFSET], %fsr
        ret
        restore

#endif /* SPARC_HAS_FPU */

/*
 *  void _CPU_Context_switch(
 *    Context_Control  *run,
 *    Context_Control  *heir
 *  )
 *
 *  This routine performs a normal non-FP context switch.
 */

        .align 4
        PUBLIC(_CPU_Context_switch)
SYM(_CPU_Context_switch):
        ! skip g0
        st      %g1, [%o0 + G1_OFFSET]       ! save the global registers
        std     %g2, [%o0 + G2_OFFSET]
        std     %g4, [%o0 + G4_OFFSET]
        std     %g6, [%o0 + G6_OFFSET]

        ! load the address of the ISR stack nesting prevention flag
        sethi    %hi(SYM(_CPU_ISR_Dispatch_disable)), %g2
        ld       [%g2 + %lo(SYM(_CPU_ISR_Dispatch_disable))], %g2
        ! save it a bit later so we do not waste a couple of cycles

        std     %l0, [%o0 + L0_OFFSET]       ! save the local registers
        std     %l2, [%o0 + L2_OFFSET]
        std     %l4, [%o0 + L4_OFFSET]
        std     %l6, [%o0 + L6_OFFSET]

        ! Now actually save ISR stack nesting prevention flag
        st       %g2, [%o0 + ISR_DISPATCH_DISABLE_STACK_OFFSET]

        std     %i0, [%o0 + I0_OFFSET]       ! save the input registers
        std     %i2, [%o0 + I2_OFFSET]
        std     %i4, [%o0 + I4_OFFSET]
        std     %i6, [%o0 + I6_FP_OFFSET]

        std     %o0, [%o0 + O0_OFFSET]       ! save the output registers
        std     %o2, [%o0 + O2_OFFSET]
        std     %o4, [%o0 + O4_OFFSET]
        std     %o6, [%o0 + O6_SP_OFFSET]

        rd      %psr, %o2
        st      %o2, [%o0 + PSR_OFFSET]      ! save status register

        /*
         *  This is entered from _CPU_Context_restore with:
         *    o1 = context to restore
         *    o2 = psr
         */

        PUBLIC(_CPU_Context_restore_heir)
SYM(_CPU_Context_restore_heir):
        /*
         *  Flush all windows with valid contents except the current one.
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
         *  In this case, we only would save the used windows -- 6 and 7.
         *
         *   Traps are disabled for the same logical period as in a
         *     flush all windows trap handler.
         *
         *    Register Usage while saving the windows:
         *      g1 = current PSR
         *      g2 = current wim
         *      g3 = CWP
         *      g4 = wim scratch
         *      g5 = scratch
         */

        ld      [%o1 + PSR_OFFSET], %g1       ! g1 = saved psr

        and     %o2, SPARC_PSR_CWP_MASK, %g3  ! g3 = CWP
                                              ! g1 = psr w/o cwp
        andn    %g1, SPARC_PSR_ET_MASK | SPARC_PSR_CWP_MASK, %g1
        or      %g1, %g3, %g1                 ! g1 = heirs psr
        mov     %g1, %psr                     ! restore status register and
                                              ! **** DISABLE TRAPS ****
        mov     %wim, %g2                     ! g2 = wim
        mov     1, %g4
        sll     %g4, %g3, %g4                 ! g4 = WIM mask for CW invalid

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

        add     %g3, 1, %g3                   ! calculate desired WIM
        and     %g3, SPARC_NUMBER_OF_REGISTER_WINDOWS - 1, %g3
        mov     1, %g4
        sll     %g4, %g3, %g4                 ! g4 = new WIM
        mov     %g4, %wim

        or      %g1, SPARC_PSR_ET_MASK, %g1
        mov     %g1, %psr                     ! **** ENABLE TRAPS ****
                                              !   and restore CWP
        nop
        nop
        nop

        ! skip g0
        ld      [%o1 + G1_OFFSET], %g1        ! restore the global registers
        ldd     [%o1 + G2_OFFSET], %g2
        ldd     [%o1 + G4_OFFSET], %g4
        ldd     [%o1 + G6_OFFSET], %g6

        ! Load thread specific ISR dispatch prevention flag
        ld      [%o1 + ISR_DISPATCH_DISABLE_STACK_OFFSET], %o2
        sethi    %hi(SYM(_CPU_ISR_Dispatch_disable)), %o3
        ! Store it to memory later to use the cycles

        ldd     [%o1 + L0_OFFSET], %l0        ! restore the local registers
        ldd     [%o1 + L2_OFFSET], %l2
        ldd     [%o1 + L4_OFFSET], %l4
        ldd     [%o1 + L6_OFFSET], %l6

        ! Now restore thread specific ISR dispatch prevention flag
        st       %o2,[%o3 + %lo(SYM(_CPU_ISR_Dispatch_disable))]

        ldd     [%o1 + I0_OFFSET], %i0        ! restore the output registers
        ldd     [%o1 + I2_OFFSET], %i2
        ldd     [%o1 + I4_OFFSET], %i4
        ldd     [%o1 + I6_FP_OFFSET], %i6

        ldd     [%o1 + O2_OFFSET], %o2        ! restore the output registers
        ldd     [%o1 + O4_OFFSET], %o4
        ldd     [%o1 + O6_SP_OFFSET], %o6
        ! do o0/o1 last to avoid destroying heir context pointer
        ldd     [%o1 + O0_OFFSET], %o0        ! overwrite heir pointer

        jmp     %o7 + 8                       ! return
        nop                                   ! delay slot

/*
 *  void _CPU_Context_restore(
 *    Context_Control *new_context
 *  )
 *
 *  This routine is generally used only to perform restart self.
 *
 *  NOTE: It is unnecessary to reload some registers.
 */
        .align 4
        PUBLIC(_CPU_Context_restore)
SYM(_CPU_Context_restore):
        save    %sp, -CPU_MINIMUM_STACK_FRAME_SIZE, %sp
        rd      %psr, %o2
        ba      SYM(_CPU_Context_restore_heir)
        mov     %i0, %o1                      ! in the delay slot
        .align 4

#if defined(RTEMS_SMP)
/*
 *  void _CPU_Context_switch_to_first_task_smp(
 *    Context_Control *new_context
 *  )
 *
 *  This routine is only used to switch to the first task on a
 *  secondary core in an SMP configuration.  We do not need to
 *  flush any windows and, in fact, this can be dangerous
 *  as they may or may not be initialized properly.  So we just
 *  reinitialize the PSR and WIM.
 */
        PUBLIC(_CPU_Context_switch_to_first_task_smp)
SYM(_CPU_Context_switch_to_first_task_smp):
	mov	%psr, %g1		! Turn of traps when modifying WIM
	andn	%g1, SPARC_PSR_ET_MASK, %g1
	mov	%g1, %psr
	/* WIM and PSR will be set in done_flushing, it need args:
	 * g1=PSR, g3=CWP, o1=Context
	 */
	and	%g1, SPARC_NUMBER_OF_REGISTER_WINDOWS - 1, %g3
	nop
	mov	%o0, %o1		! in the delay slot
	ba,a	done_flushing
#endif

/* end of file */
