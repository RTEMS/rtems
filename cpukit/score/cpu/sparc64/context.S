/*  context.S
 *
 *  This file contains the basic algorithms for all assembly code used
 *  in an specific CPU port of RTEMS.  These algorithms must be implemented
 *  in assembly language. 
 *
 *  COPYRIGHT (c) 1989-2007.
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
 *
 *  Ported to UltraSPARC T1 Niagara implementation of the SPARC-v9.
 *  Niagara modifications of respective RTEMS file: 
 *    COPYRIGHT (c) 2010. Gedare Bloom.
 */

#include <rtems/asm.h>


/* 
 *  The assembler needs to be told that we know what to do with 
 *  the global registers.
 */
.register %g2, #scratch
.register %g3, #scratch
.register %g6, #scratch
.register %g7, #scratch

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
 */

  .align 4
PUBLIC(_CPU_Context_save_fp)
  SYM(_CPU_Context_save_fp):
    save    %sp, -CPU_MINIMUM_STACK_FRAME_SIZE, %sp

    /*
     *  The following enables the floating point unit.
     */

    sparc64_enable_FPU(%l0)

  /*
   *  Although sun4v supports alternate register names for double-
   *  and quad-word floating point, SPARC v9 only uses f[#]
   *
   *  Because quad-word fp is not supported by the hardware in 
   *  many situations, we stick with double-word fp operations
   */
  ldx    [%i0], %l0     
  std     %f0, [%l0]
  std     %f2, [%l0 + F2_OFFSET]
  std     %f4, [%l0 + F4_OFFSET]
  std     %f6, [%l0 + F6_OFFSET]
  std     %f8, [%l0 + F8_OFFSET]
  std     %f10, [%l0 + F1O_OFFSET]
  std     %f12, [%l0 + F12_OFFSET]
  std     %f14, [%l0 + F14_OFFSET]
  std     %f16, [%l0 + F16_OFFSET]
  std     %f18, [%l0 + F18_OFFSET]
  std     %f20, [%l0 + F2O_OFFSET]
  std     %f22, [%l0 + F22_OFFSET]
  std     %f24, [%l0 + F24_OFFSET]
  std     %f26, [%l0 + F26_OFFSET]
  std     %f28, [%l0 + F28_OFFSET]
  std     %f30, [%l0 + F3O_OFFSET]
  std     %f32, [%l0 + F32_OFFSET]
  std     %f34, [%l0 + F34_OFFSET]
  std     %f36, [%l0 + F36_OFFSET]
  std     %f38, [%l0 + F38_OFFSET]
  std     %f40, [%l0 + F4O_OFFSET]
  std     %f42, [%l0 + F42_OFFSET]
  std     %f44, [%l0 + F44_OFFSET]
  std     %f46, [%l0 + F46_OFFSET]
  std     %f48, [%l0 + F48_OFFSET]
  std     %f50, [%l0 + F5O_OFFSET]
  std     %f52, [%l0 + F52_OFFSET]
  std     %f54, [%l0 + F54_OFFSET]
  std     %f56, [%l0 + F56_OFFSET]
  std     %f58, [%l0 + F58_OFFSET]
  std     %f60, [%l0 + F6O_OFFSET]       
  std     %f62, [%l0 + F62_OFFSET]
  stx     %fsr, [%l0 + FSR_OFFSET]
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
   */

  .align 4
PUBLIC(_CPU_Context_restore_fp)
  SYM(_CPU_Context_restore_fp):
    save    %sp, -CPU_MINIMUM_STACK_FRAME_SIZE , %sp

    /*
     *  The following enables the floating point unit.
     */

    sparc64_enable_FPU(%l0)

  ldx     [%i0], %l0
  ldd     [%l0 + FO_OFFSET], %f0
  ldd     [%l0 + F2_OFFSET], %f2
  ldd     [%l0 + F4_OFFSET], %f4
  ldd     [%l0 + F6_OFFSET], %f6
  ldd     [%l0 + F8_OFFSET], %f8
  ldd     [%l0 + F1O_OFFSET], %f10
  ldd     [%l0 + F12_OFFSET], %f12
  ldd     [%l0 + F14_OFFSET], %f14
  ldd     [%l0 + F16_OFFSET], %f16
  ldd     [%l0 + F18_OFFSET], %f18
  ldd     [%l0 + F2O_OFFSET], %f20
  ldd     [%l0 + F22_OFFSET], %f22
  ldd     [%l0 + F24_OFFSET], %f24
  ldd     [%l0 + F26_OFFSET], %f26
  ldd     [%l0 + F28_OFFSET], %f28
  ldd     [%l0 + F3O_OFFSET], %f30
  ldd     [%l0 + F32_OFFSET], %f32
  ldd     [%l0 + F34_OFFSET], %f34
  ldd     [%l0 + F36_OFFSET], %f36
  ldd     [%l0 + F38_OFFSET], %f38
  ldd     [%l0 + F4O_OFFSET], %f40
  ldd     [%l0 + F42_OFFSET], %f42
  ldd     [%l0 + F44_OFFSET], %f44
  ldd     [%l0 + F46_OFFSET], %f46
  ldd     [%l0 + F48_OFFSET], %f48
  ldd     [%l0 + F5O_OFFSET], %f50
  ldd     [%l0 + F52_OFFSET], %f52
  ldd     [%l0 + F54_OFFSET], %f54
  ldd     [%l0 + F56_OFFSET], %f56
  ldd     [%l0 + F58_OFFSET], %f58
  ldd     [%l0 + F6O_OFFSET], %f60
  ldd     [%l0 + F62_OFFSET], %f62
  ldx     [%l0 + FSR_OFFSET], %fsr
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
      stx     %g1, [%o0 + G1_OFFSET]       ! save the global registers
      stx     %g2, [%o0 + G2_OFFSET]
      stx     %g3, [%o0 + G3_OFFSET]       
      stx     %g4, [%o0 + G4_OFFSET]
      stx     %g5, [%o0 + G5_OFFSET]       
      stx     %g6, [%o0 + G6_OFFSET]
      stx     %g7, [%o0 + G7_OFFSET]

      ! load the address of the ISR stack nesting prevention flag
      setx  SYM(_CPU_ISR_Dispatch_disable), %g1, %g2
      lduw  [%g2], %g2

      ! save it a bit later so we do not waste a couple of cycles

      stx     %l0, [%o0 + L0_OFFSET]       ! save the local registers
      stx     %l1, [%o0 + L1_OFFSET]
      stx     %l2, [%o0 + L2_OFFSET]
      stx     %l3, [%o0 + L3_OFFSET]
      stx     %l4, [%o0 + L4_OFFSET]
      stx     %l5, [%o0 + L5_OFFSET]
      stx     %l6, [%o0 + L6_OFFSET]
      stx     %l7, [%o0 + L7_OFFSET]

      ! Now actually save ISR stack nesting prevention flag
      stuw     %g2, [%o0 + ISR_DISPATCH_DISABLE_STACK_OFFSET]

      stx     %i0, [%o0 + I0_OFFSET]       ! save the input registers
      stx     %i1, [%o0 + I1_OFFSET]
      stx     %i2, [%o0 + I2_OFFSET]
      stx     %i3, [%o0 + I3_OFFSET]
      stx     %i4, [%o0 + I4_OFFSET]
      stx     %i5, [%o0 + I5_OFFSET]
      stx     %i6, [%o0 + I6_FP_OFFSET]
      stx     %i7, [%o0 + I7_OFFSET]

      stx     %o0, [%o0 + O0_OFFSET]       ! save the output registers
      stx     %o1, [%o0 + O1_OFFSET]
      stx     %o2, [%o0 + O2_OFFSET]
      stx     %o3, [%o0 + O3_OFFSET]
      stx     %o4, [%o0 + O4_OFFSET]
      stx     %o5, [%o0 + O5_OFFSET]
      stx     %o6, [%o0 + O6_SP_OFFSET]
      stx     %o7, [%o0 + O7_OFFSET]       ! o7 is the PC

!      rdpr    %pil, %o2
!      stuw    %o2, [%o0 + PIL_OFFSET] ! save pil

!      rdpr    %pstate, %o2
!      stx     %o2, [%o0 + PSTATE_OFFSET]      ! save status register

      /*
       *  This is entered from _CPU_Context_restore with:
       *    o1 = context to restore
!       *    o2 = pstate
       *
       *  NOTE: Flushing the register windows is necessary, but it adds 
       *  an unpredictable (but bounded) overhead to context switching.
       */

PUBLIC(_CPU_Context_restore_heir)
  SYM(_CPU_Context_restore_heir):

    flushw



    ! skip g0
    ldx     [%o1 + G1_OFFSET], %g1        ! restore the global registers
    ldx     [%o1 + G2_OFFSET], %g2
    ldx     [%o1 + G3_OFFSET], %g3
    ldx     [%o1 + G4_OFFSET], %g4
    ldx     [%o1 + G5_OFFSET], %g5
    ldx     [%o1 + G6_OFFSET], %g6
    ldx     [%o1 + G7_OFFSET], %g7

    ! Load thread specific ISR dispatch prevention flag
    ldx    [%o1 + ISR_DISPATCH_DISABLE_STACK_OFFSET], %o2
    setx  SYM(_CPU_ISR_Dispatch_disable), %o5, %o3
    ! Store it to memory later to use the cycles

    ldx     [%o1 + L0_OFFSET], %l0        ! restore the local registers
    ldx     [%o1 + L1_OFFSET], %l1
    ldx     [%o1 + L2_OFFSET], %l2
    ldx     [%o1 + L3_OFFSET], %l3
    ldx     [%o1 + L4_OFFSET], %l4
    ldx     [%o1 + L5_OFFSET], %l5
    ldx     [%o1 + L6_OFFSET], %l6
    ldx     [%o1 + L7_OFFSET], %l7

    ! Now restore thread specific ISR dispatch prevention flag
    stuw  %o2, [%o3]

    ldx     [%o1 + I0_OFFSET], %i0        ! restore the input registers
    ldx     [%o1 + I1_OFFSET], %i1
    ldx     [%o1 + I2_OFFSET], %i2
    ldx     [%o1 + I3_OFFSET], %i3
    ldx     [%o1 + I4_OFFSET], %i4
    ldx     [%o1 + I5_OFFSET], %i5
    ldx     [%o1 + I6_FP_OFFSET], %i6
    ldx     [%o1 + I7_OFFSET], %i7

    ldx     [%o1 + O0_OFFSET], %o0
    ldx     [%o1 + O2_OFFSET], %o2        ! restore the output registers
    ldx     [%o1 + O3_OFFSET], %o3
    ldx     [%o1 + O4_OFFSET], %o4
    ldx     [%o1 + O5_OFFSET], %o5
    ldx     [%o1 + O6_SP_OFFSET], %o6
    ldx     [%o1 + O7_OFFSET], %o7       ! PC

    ! on a hunch... we should be able to use some of the %o regs
!    lduw    [%o1 + PIL_OFFSET], %o2
!    wrpr    %g0, %o2, %pil

!    ldx     [%o1 + PSTATE_OFFSET], %o2

    ! do o1 last to avoid destroying heir context pointer
    ldx     [%o1 + O1_OFFSET], %o1        ! overwrite heir pointer
!    wrpr    %g0, %o2, %pstate

    retl
    nop

    /*
     *  void _CPU_Context_restore(
     *    Context_Control *new_context
     *  )
     *
     *  This routine is generally used only to perform restart self.
     *
     *  NOTE: It is unnecessary to reload some registers.
     */
    /* if _CPU_Context_restore_heir does not flushw, then do it here */
  .align 4
PUBLIC(_CPU_Context_restore)
  SYM(_CPU_Context_restore):
    save    %sp, -CPU_MINIMUM_STACK_FRAME_SIZE, %sp
!    rdpr    %pstate, %o2
    ba      SYM(_CPU_Context_restore_heir)
    mov     %i0, %o1                      ! in the delay slot

/* end of file */
