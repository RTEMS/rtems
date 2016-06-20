/*  cpu_asm.s
 *
 *  This file contains the basic algorithms for all assembly code used
 *  in an specific CPU port of RTEMS.  These algorithms must be implemented
 *  in assembly language.
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2014-2015 embedded brains GmbH
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995.
 *  European Space Agency.
 */

#include <rtems/asm.h>
#include <rtems/score/percpu.h>
#include <bspopts.h>

#if SPARC_HAS_FPU == 1 && defined(SPARC_USE_SAFE_FP_SUPPORT)
  #define FP_FRAME_OFFSET_FO_F1 (CPU_MINIMUM_STACK_FRAME_SIZE + 0)
  #define FP_FRAME_OFFSET_F2_F3 (FP_FRAME_OFFSET_FO_F1 + 8)
  #define FP_FRAME_OFFSET_F4_F5 (FP_FRAME_OFFSET_F2_F3 + 8)
  #define FP_FRAME_OFFSET_F6_F7 (FP_FRAME_OFFSET_F4_F5 + 8)
  #define FP_FRAME_OFFSET_F8_F9 (FP_FRAME_OFFSET_F6_F7 + 8)
  #define FP_FRAME_OFFSET_F1O_F11 (FP_FRAME_OFFSET_F8_F9 + 8)
  #define FP_FRAME_OFFSET_F12_F13 (FP_FRAME_OFFSET_F1O_F11 + 8)
  #define FP_FRAME_OFFSET_F14_F15 (FP_FRAME_OFFSET_F12_F13 + 8)
  #define FP_FRAME_OFFSET_F16_F17 (FP_FRAME_OFFSET_F14_F15 + 8)
  #define FP_FRAME_OFFSET_F18_F19 (FP_FRAME_OFFSET_F16_F17 + 8)
  #define FP_FRAME_OFFSET_F2O_F21 (FP_FRAME_OFFSET_F18_F19 + 8)
  #define FP_FRAME_OFFSET_F22_F23 (FP_FRAME_OFFSET_F2O_F21 + 8)
  #define FP_FRAME_OFFSET_F24_F25 (FP_FRAME_OFFSET_F22_F23 + 8)
  #define FP_FRAME_OFFSET_F26_F27 (FP_FRAME_OFFSET_F24_F25 + 8)
  #define FP_FRAME_OFFSET_F28_F29 (FP_FRAME_OFFSET_F26_F27 + 8)
  #define FP_FRAME_OFFSET_F3O_F31 (FP_FRAME_OFFSET_F28_F29 + 8)
  #define FP_FRAME_OFFSET_FSR (FP_FRAME_OFFSET_F3O_F31 + 8)
  #define FP_FRAME_SIZE (FP_FRAME_OFFSET_FSR + 8)
#endif

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
        st      %g5, [%o0 + G5_OFFSET]       ! save the global registers
        st      %g7, [%o0 + G7_OFFSET]

        std     %l0, [%o0 + L0_OFFSET]       ! save the local registers
        std     %l2, [%o0 + L2_OFFSET]
        std     %l4, [%o0 + L4_OFFSET]
        std     %l6, [%o0 + L6_OFFSET]

        std     %i0, [%o0 + I0_OFFSET]       ! save the input registers
        std     %i2, [%o0 + I2_OFFSET]
        std     %i4, [%o0 + I4_OFFSET]
        std     %i6, [%o0 + I6_FP_OFFSET]

        std     %o6, [%o0 + O6_SP_OFFSET]    ! save the output registers

        ! load the ISR stack nesting prevention flag
        ld      [%g6 + SPARC_PER_CPU_ISR_DISPATCH_DISABLE], %o4
        ! save it a bit later so we do not waste a couple of cycles

        rd      %psr, %o2
        st      %o2, [%o0 + PSR_OFFSET]      ! save status register

        ! Now actually save ISR stack nesting prevention flag
        st       %o4, [%o0 + ISR_DISPATCH_DISABLE_STACK_OFFSET]

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

        and     %o2, SPARC_PSR_CWP_MASK, %g3  ! g3 = CWP
        andn    %o2, SPARC_PSR_ET_MASK, %g1   ! g1 = psr with traps disabled
        mov     %g1, %psr                     ! **** DISABLE TRAPS ****
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

        ! Wait three instructions after the write to PSR before using
        ! non-global registers or instructions affecting the CWP
        mov     %g1, %psr                     ! restore cwp
        add     %g3, 1, %g2                   ! calculate desired WIM
        and     %g2, SPARC_NUMBER_OF_REGISTER_WINDOWS - 1, %g2
        mov     1, %g4
        sll     %g4, %g2, %g4                 ! g4 = new WIM
        mov     %g4, %wim

#if defined(RTEMS_SMP)
        ! The executing context no longer executes on this processor
        st      %g0, [%o0 + SPARC_CONTEXT_CONTROL_IS_EXECUTING_OFFSET]

        ! Try to update the is executing indicator of the heir context
        mov     1, %g1

try_update_is_executing:

        swap    [%o1 + SPARC_CONTEXT_CONTROL_IS_EXECUTING_OFFSET], %g1
        cmp     %g1, 0
        bne     check_is_executing

        ! The next load is in a delay slot, which is all right
#endif

        ld      [%o1 + PSR_OFFSET], %g1       ! g1 = heir psr with traps enabled
        andn    %g1, SPARC_PSR_CWP_MASK, %g1  ! g1 = heir psr w/o cwp
        or      %g1, %g3, %g1                 ! g1 = heir psr with cwp
        mov     %g1, %psr                     ! restore status register and
                                              ! **** ENABLE TRAPS ****

        ld      [%o1 + G5_OFFSET], %g5        ! restore the global registers
        ld      [%o1 + G7_OFFSET], %g7

        ! Load thread specific ISR dispatch prevention flag
        ld      [%o1 + ISR_DISPATCH_DISABLE_STACK_OFFSET], %o2
        ! Store it to memory later to use the cycles

        ldd     [%o1 + L0_OFFSET], %l0        ! restore the local registers
        ldd     [%o1 + L2_OFFSET], %l2
        ldd     [%o1 + L4_OFFSET], %l4
        ldd     [%o1 + L6_OFFSET], %l6

        ! Now restore thread specific ISR dispatch prevention flag
        st      %o2, [%g6 + SPARC_PER_CPU_ISR_DISPATCH_DISABLE]

        ldd     [%o1 + I0_OFFSET], %i0        ! restore the input registers
        ldd     [%o1 + I2_OFFSET], %i2
        ldd     [%o1 + I4_OFFSET], %i4
        ldd     [%o1 + I6_FP_OFFSET], %i6

        ldd     [%o1 + O6_SP_OFFSET], %o6     ! restore the output registers

        jmp     %o7 + 8                       ! return
        nop                                   ! delay slot

#if defined(RTEMS_SMP)
check_is_executing:

        ! Check the is executing indicator of the heir context
        ld      [%o1 + SPARC_CONTEXT_CONTROL_IS_EXECUTING_OFFSET], %g1
        cmp     %g1, 0
        beq     try_update_is_executing
         mov    1, %g1

        ! We may have a new heir

        ! Read the executing and heir
        ld      [%g6 + PER_CPU_OFFSET_EXECUTING], %g2
        ld      [%g6 + PER_CPU_OFFSET_HEIR], %g4

        ! Update the executing only if necessary to avoid cache line
        ! monopolization.
        cmp     %g2, %g4
        beq     try_update_is_executing
         mov    1, %g1

        ! Calculate the heir context pointer
        sub     %o1, %g2, %g2
        add     %g2, %g4, %o1

        ! Update the executing
        st      %g4, [%g6 + PER_CPU_OFFSET_EXECUTING]

        ba      try_update_is_executing
         mov    1, %g1
#endif

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
#if defined(RTEMS_SMP)
        ! On SPARC the restore path needs also a valid executing context on SMP
        ! to update the is executing indicator.
        mov     %i0, %o0
#endif
        ba      SYM(_CPU_Context_restore_heir)
        mov     %i0, %o1                      ! in the delay slot

/*
 *  void _ISR_Handler()
 *
 *  This routine provides the RTEMS interrupt management.
 *
 *  We enter this handler from the 4 instructions in the trap table with
 *  the following registers assumed to be set as shown:
 *
 *    l0 = PSR
 *    l1 = PC
 *    l2 = nPC
 *    l3 = trap type
 *
 *  NOTE: By an executive defined convention, trap type is between 0 and 255 if
 *        it is an asynchonous trap and 256 and 511 if it is synchronous.
 */

        .align 4
        PUBLIC(_ISR_Handler)
SYM(_ISR_Handler):
        /*
         *  Fix the return address for synchronous traps.
         */

        andcc   %l3, SPARC_SYNCHRONOUS_TRAP_BIT_MASK, %g0
                                      ! Is this a synchronous trap?
        be,a    win_ovflow            ! No, then skip the adjustment
        nop                           ! DELAY
        mov     %l1, %l6              ! save trapped pc for debug info
        mov     %l2, %l1              ! do not return to the instruction
        add     %l2, 4, %l2           ! indicated

win_ovflow:
        /*
         *  Save the globals this block uses.
         *
         *  These registers are not restored from the locals.  Their contents
         *  are saved directly from the locals into the ISF below.
         */

        mov     %g4, %l4                 ! save the globals this block uses
        mov     %g5, %l5

        /*
         *  When at a "window overflow" trap, (wim == (1 << cwp)).
         *  If we get here like that, then process a window overflow.
         */

        rd      %wim, %g4
        srl     %g4, %l0, %g5            ! g5 = win >> cwp ; shift count and CWP
                                         !   are LS 5 bits ; how convenient :)
        cmp     %g5, 1                   ! Is this an invalid window?
        bne     dont_do_the_window       ! No, then skip all this stuff
        ! we are using the delay slot

        /*
         *  The following is same as a 1 position right rotate of WIM
         */

        srl     %g4, 1, %g5              ! g5 = WIM >> 1
        sll     %g4, SPARC_NUMBER_OF_REGISTER_WINDOWS-1 , %g4
                                         ! g4 = WIM << (Number Windows - 1)
        or      %g4, %g5, %g4            ! g4 = (WIM >> 1) |
                                         !      (WIM << (Number Windows - 1))

        /*
         *  At this point:
         *
         *    g4 = the new WIM
         *    g5 is free
         */

        /*
         *  Since we are tinkering with the register windows, we need to
         *  make sure that all the required information is in global registers.
         */

        save                          ! Save into the window
        wr      %g4, 0, %wim          ! WIM = new WIM
        nop                           ! delay slots
        nop
        nop

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

        restore
        nop

dont_do_the_window:
        /*
         *  Global registers %g4 and %g5 are saved directly from %l4 and
         *  %l5 directly into the ISF below.
         */

        /*
         *  Save the state of the interrupted task -- especially the global
         *  registers -- in the Interrupt Stack Frame.  Note that the ISF
         *  includes a regular minimum stack frame which will be used if
         *  needed by register window overflow and underflow handlers.
         *
         *  REGISTERS SAME AS AT _ISR_Handler
         */

        sub     %fp, CONTEXT_CONTROL_INTERRUPT_FRAME_SIZE, %sp
                                               ! make space for ISF

        std     %l0, [%sp + ISF_PSR_OFFSET]    ! save psr, PC
        st      %l2, [%sp + ISF_NPC_OFFSET]    ! save nPC
        st      %g1, [%sp + ISF_G1_OFFSET]     ! save g1
        std     %g2, [%sp + ISF_G2_OFFSET]     ! save g2, g3
        std     %l4, [%sp + ISF_G4_OFFSET]     ! save g4, g5 -- see above
        st      %g7, [%sp + ISF_G7_OFFSET]     ! save g7

        std     %i0, [%sp + ISF_I0_OFFSET]     ! save i0, i1
        std     %i2, [%sp + ISF_I2_OFFSET]     ! save i2, i3
        std     %i4, [%sp + ISF_I4_OFFSET]     ! save i4, i5
        std     %i6, [%sp + ISF_I6_FP_OFFSET]  ! save i6/fp, i7

        rd      %y, %g1
        st      %g1, [%sp + ISF_Y_OFFSET]      ! save y
        st      %l6, [%sp + ISF_TPC_OFFSET]    ! save real trapped pc

        mov     %sp, %o1                       ! 2nd arg to ISR Handler

        /*
         *  Increment ISR nest level and Thread dispatch disable level.
         *
         *  Register usage for this section:
         *
         *    l6 = _Thread_Dispatch_disable_level value
         *    l7 = _ISR_Nest_level value
         *
         *  NOTE: It is assumed that l6 - l7 will be preserved until the ISR
         *        nest and thread dispatch disable levels are unnested.
         */

        ld       [%g6 + PER_CPU_ISR_NEST_LEVEL], %l7
        ld       [%g6 + PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL], %l6

        add      %l7, 1, %l7
        st       %l7, [%g6 + PER_CPU_ISR_NEST_LEVEL]

        add      %l6, 1, %l6
        st       %l6, [%g6 + PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]

#if SPARC_HAS_FPU == 1
        /*
         * We cannot use an intermediate value for operations with the PSR[EF]
         * bit since they use a 13-bit sign extension and PSR[EF] is bit 12.
         */
        sethi    %hi(SPARC_PSR_EF_MASK), %l5
#endif

        /*
         *  If ISR nest level was zero (now 1), then switch stack.
         */

        mov      %sp, %fp
        subcc    %l7, 1, %l7             ! outermost interrupt handler?
        bnz      dont_switch_stacks      ! No, then do not switch stacks

#if defined(RTEMS_PROFILING)
         sethi   %hi(_SPARC_Counter), %o5
        ld       [%o5 + %lo(_SPARC_Counter)], %l4
        call     %l4, 0
         nop
        mov      %o0, %o5
#else
         nop
#endif

        ld       [%g6 + PER_CPU_INTERRUPT_STACK_HIGH], %sp

#if SPARC_HAS_FPU == 1
        /*
         * Test if the interrupted thread uses the floating point unit
         * (PSR[EF] == 1).  In case it uses the floating point unit, then store
         * the floating point status register.  This has the side-effect that
         * all pending floating point operations complete before the store
         * completes.  The PSR[EF] bit is restored after the call to the
         * interrupt handler.  Thus post-switch actions (e.g. signal handlers)
         * and context switch extensions may still corrupt the floating point
         * context.
         */
        andcc    %l0, %l5, %g0
        bne,a    dont_switch_stacks
         st      %fsr, [%g6 + SPARC_PER_CPU_FSR_OFFSET]
#endif

dont_switch_stacks:
        /*
         *  Make sure we have a place on the stack for the window overflow
         *  trap handler to write into.  At this point it is safe to
         *  enable traps again.
         */

        sub      %sp, CPU_MINIMUM_STACK_FRAME_SIZE, %sp

        /*
         *  Check if we have an external interrupt (trap 0x11 - 0x1f). If so,
         *  set the PIL in the %psr to mask off interrupts with lower priority.
         *  The original %psr in %l0 is not modified since it will be restored
         *  when the interrupt handler returns.
         */

        mov      %l0, %g5
        and      %l3, 0x0ff, %g4
        subcc    %g4, 0x11, %g0
        bl       dont_fix_pil
        subcc    %g4, 0x1f, %g0
        bg       dont_fix_pil
        sll      %g4, 8, %g4
        and      %g4, SPARC_PSR_PIL_MASK, %g4
        andn     %l0, SPARC_PSR_PIL_MASK, %g5
        ba       pil_fixed
        or       %g4, %g5, %g5
dont_fix_pil:
        or       %g5, SPARC_PSR_PIL_MASK, %g5
pil_fixed:

#if SPARC_HAS_FPU == 1
        /*
         * Clear the PSR[EF] bit of the interrupted context to ensure that
         * interrupt service routines cannot corrupt the floating point context.
         */
        andn     %g5, %l5, %g5
#endif

        wr       %g5, SPARC_PSR_ET_MASK, %psr ! **** ENABLE TRAPS ****

        /*
         *  Vector to user's handler.
         *
         *  NOTE: TBR may no longer have vector number in it since
         *        we just enabled traps.  It is definitely in l3.
         */

        sethi    %hi(SYM(_ISR_Vector_table)), %g4
        or       %g4, %lo(SYM(_ISR_Vector_table)), %g4
        and      %l3, 0xFF, %g5         ! remove synchronous trap indicator
        sll      %g5, 2, %g5            ! g5 = offset into table
        ld       [%g4 + %g5], %g4       ! g4 = _ISR_Vector_table[ vector ]


                                        ! o1 = 2nd arg = address of the ISF
                                        !   WAS LOADED WHEN ISF WAS SAVED!!!
        mov      %l3, %o0               ! o0 = 1st arg = vector number
        call     %g4, 0
#if defined(RTEMS_PROFILING)
         mov     %o5, %l3               ! save interrupt entry instant
        cmp      %l7, 0
        bne      profiling_not_outer_most_exit
         nop
        ta       SPARC_SWTRAP_IRQDIS    ! Call interrupt disable trap handler
        call     %l4, 0                 ! Call _SPARC_Counter.counter_read
         nop
        mov      %o0, %o2               ! o2 = 3rd arg = interrupt exit instant
        mov      %l3, %o1               ! o1 = 2nd arg = interrupt entry instant
        call     SYM(_Profiling_Outer_most_interrupt_entry_and_exit), 0
         mov     %g6, %o0               ! o0 = 1st arg = per-CPU control
profiling_not_outer_most_exit:
#else
         nop                            ! delay slot
#endif

        /*
         *  Redisable traps so we can finish up the interrupt processing.
         *  This is a VERY conservative place to do this.
         *
         *  NOTE: %l0 has the PSR which was in place when we took the trap.
         */

        mov      %l0, %psr             ! **** DISABLE TRAPS ****
        nop; nop; nop

        /*
         *  Decrement ISR nest level and Thread dispatch disable level.
         *
         *  Register usage for this section:
         *
         *    l6 = _Thread_Dispatch_disable_level value
         *    l7 = _ISR_Nest_level value
         */

        st       %l7, [%g6 + PER_CPU_ISR_NEST_LEVEL]

        sub      %l6, 1, %l6
        st       %l6, [%g6 + PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]

        /*
         *  If dispatching is disabled (includes nested interrupt case),
         *  then do a "simple" exit.
         */

        orcc     %l6, %g0, %g0   ! Is dispatching disabled?
        bnz      simple_return   ! Yes, then do a "simple" exit
        nop

        ! Are we dispatching from a previous ISR in the interrupted thread?
        ld       [%g6 + SPARC_PER_CPU_ISR_DISPATCH_DISABLE], %l7
        orcc     %l7, %g0, %g0   ! Is this thread already doing an ISR?
        bnz      simple_return   ! Yes, then do a "simple" exit
        nop


        /*
         *  If a context switch is necessary, then do fudge stack to
         *  return to the interrupt dispatcher.
         */

        ldub     [%g6 + PER_CPU_DISPATCH_NEEDED], %l6

        orcc     %l6, %g0, %g0   ! Is thread switch necessary?
        bz       simple_return   ! no, then do a simple return
        nop

        /*
         *  Invoke interrupt dispatcher.
         */

        ! Set ISR dispatch nesting prevention flag
        mov      1,%l6
        st       %l6, [%g6 + SPARC_PER_CPU_ISR_DISPATCH_DISABLE]

        /*
         *  The following subtract should get us back on the interrupted
         *  tasks stack and add enough room to invoke the dispatcher.
         *  When we enable traps, we are mostly back in the context
         *  of the task and subsequent interrupts can operate normally.
         */

        sub      %fp, CPU_MINIMUM_STACK_FRAME_SIZE, %sp

        or      %l0, SPARC_PSR_ET_MASK, %l7    ! l7 = PSR with ET=1
        mov     %l7, %psr                      !  **** ENABLE TRAPS ****
        nop
        nop
        nop
isr_dispatch:

#if SPARC_HAS_FPU == 1 && defined(SPARC_USE_SAFE_FP_SUPPORT)
        /* Test if we interrupted a floating point thread (PSR[EF] == 1) */
        andcc   %l0, %l5, %g0
        be      non_fp_thread_dispatch
         nop

        /*
         * Yes, this is a floating point thread, then save the floating point
         * context to a new stack frame.  Then do the thread dispatch.
         * Post-switch actions (e.g. signal handlers) and context switch
         * extensions may safely use the floating point unit.
         */
        sub     %sp, FP_FRAME_SIZE, %sp
        std     %f0, [%sp + FP_FRAME_OFFSET_FO_F1]
        std     %f2, [%sp + FP_FRAME_OFFSET_F2_F3]
        std     %f4, [%sp + FP_FRAME_OFFSET_F4_F5]
        std     %f6, [%sp + FP_FRAME_OFFSET_F6_F7]
        std     %f8, [%sp + FP_FRAME_OFFSET_F8_F9]
        std     %f10, [%sp + FP_FRAME_OFFSET_F1O_F11]
        std     %f12, [%sp + FP_FRAME_OFFSET_F12_F13]
        std     %f14, [%sp + FP_FRAME_OFFSET_F14_F15]
        std     %f16, [%sp + FP_FRAME_OFFSET_F16_F17]
        std     %f18, [%sp + FP_FRAME_OFFSET_F18_F19]
        std     %f20, [%sp + FP_FRAME_OFFSET_F2O_F21]
        std     %f22, [%sp + FP_FRAME_OFFSET_F22_F23]
        std     %f24, [%sp + FP_FRAME_OFFSET_F24_F25]
        std     %f26, [%sp + FP_FRAME_OFFSET_F26_F27]
        std     %f28, [%sp + FP_FRAME_OFFSET_F28_F29]
        std     %f30, [%sp + FP_FRAME_OFFSET_F3O_F31]
        call    SYM(_Thread_Dispatch), 0
         st     %fsr, [%sp + FP_FRAME_OFFSET_FSR]

        /*
         * Restore the floating point context from stack frame and release the
         * stack frame.
         */
        ldd     [%sp + FP_FRAME_OFFSET_FO_F1], %f0
        ldd     [%sp + FP_FRAME_OFFSET_F2_F3], %f2
        ldd     [%sp + FP_FRAME_OFFSET_F4_F5], %f4
        ldd     [%sp + FP_FRAME_OFFSET_F6_F7], %f6
        ldd     [%sp + FP_FRAME_OFFSET_F8_F9], %f8
        ldd     [%sp + FP_FRAME_OFFSET_F1O_F11], %f10
        ldd     [%sp + FP_FRAME_OFFSET_F12_F13], %f12
        ldd     [%sp + FP_FRAME_OFFSET_F14_F15], %f14
        ldd     [%sp + FP_FRAME_OFFSET_F16_F17], %f16
        ldd     [%sp + FP_FRAME_OFFSET_F18_F19], %f18
        ldd     [%sp + FP_FRAME_OFFSET_F2O_F21], %f20
        ldd     [%sp + FP_FRAME_OFFSET_F22_F23], %f22
        ldd     [%sp + FP_FRAME_OFFSET_F24_F25], %f24
        ldd     [%sp + FP_FRAME_OFFSET_F26_F27], %f26
        ldd     [%sp + FP_FRAME_OFFSET_F28_F29], %f28
        ldd     [%sp + FP_FRAME_OFFSET_F3O_F31], %f30
        ld      [%sp + FP_FRAME_OFFSET_FSR], %fsr
        ba      thread_dispatch_done
         add    %sp, FP_FRAME_SIZE, %sp

non_fp_thread_dispatch:
#endif

        call    SYM(_Thread_Dispatch), 0
         nop

#if SPARC_HAS_FPU == 1 && defined(SPARC_USE_SAFE_FP_SUPPORT)
thread_dispatch_done:
#endif

        /*
         *  We invoked _Thread_Dispatch in a state similar to the interrupted
         *  task.  In order to safely be able to tinker with the register
         *  windows and get the task back to its pre-interrupt state,
         *  we need to disable interrupts disabled so we can safely tinker
         *  with the register windowing.  In particular, the CWP in the PSR
         *  is fragile during this period. (See PR578.)
         */
        ta      SPARC_SWTRAP_IRQDIS  ! syscall (disable interrupts)

        /*
         *  While we had ISR dispatching disabled in this thread,
         *  did we miss anything.  If so, then we need to do another
         *  _Thread_Dispatch before leaving this ISR Dispatch context.
         */
        ldub    [%g6 + PER_CPU_DISPATCH_NEEDED], %l7

        orcc    %l7, %g0, %g0        ! Is thread switch necesary?
        bne,a   isr_dispatch         ! Yes, then invoke the dispatcher.
                                     ! g1 = Old PSR PIL returned from IRQDis
        ta      SPARC_SWTRAP_IRQEN   ! syscall (enable interrupts to same level)

        ! No, then clear out and return

        ! Zero out ISR stack nesting prevention flag
        st       %g0, [%g6 + SPARC_PER_CPU_ISR_DISPATCH_DISABLE]

        /*
         *  The CWP in place at this point may be different from
         *  that which was in effect at the beginning of the ISR if we
         *  have been context switched between the beginning of this invocation
         *  of _ISR_Handler and this point.  Thus the CWP and WIM should
         *  not be changed back to their values at ISR entry time.  Any
         *  changes to the PSR must preserve the CWP.
         */

simple_return:
        ld      [%fp + ISF_Y_OFFSET], %l5      ! restore y
        wr      %l5, 0, %y

        ldd     [%fp + ISF_PSR_OFFSET], %l0    ! restore psr, PC
        ld      [%fp + ISF_NPC_OFFSET], %l2    ! restore nPC
        rd      %psr, %l3
        and     %l3, SPARC_PSR_CWP_MASK, %l3   ! want "current" CWP
        andn    %l0, SPARC_PSR_CWP_MASK, %l0   ! want rest from task
        or      %l3, %l0, %l0                  ! install it later...
        andn    %l0, SPARC_PSR_ET_MASK, %l0

        /*
         *  Restore tasks global and out registers
         */

        mov    %fp, %g1

                                              ! g1 is restored later
        ldd     [%fp + ISF_G2_OFFSET], %g2    ! restore g2, g3
        ldd     [%fp + ISF_G4_OFFSET], %g4    ! restore g4, g5
        ld      [%fp + ISF_G7_OFFSET], %g7    ! restore g7

        ldd     [%fp + ISF_I0_OFFSET], %i0    ! restore i0, i1
        ldd     [%fp + ISF_I2_OFFSET], %i2    ! restore i2, i3
        ldd     [%fp + ISF_I4_OFFSET], %i4    ! restore i4, i5
        ldd     [%fp + ISF_I6_FP_OFFSET], %i6 ! restore i6/fp, i7

        /*
         *  Registers:
         *
         *   ALL global registers EXCEPT G1 and the input registers have
         *   already been restored and thuse off limits.
         *
         *   The following is the contents of the local registers:
         *
         *     l0 = original psr
         *     l1 = return address (i.e. PC)
         *     l2 = nPC
         *     l3 = CWP
         */

        /*
         *  if (CWP + 1) is an invalid window then we need to reload it.
         *
         *  WARNING: Traps should now be disabled
         */

        mov     %l0, %psr                  !  **** DISABLE TRAPS ****
        nop
        nop
        nop
        rd      %wim, %l4
        add     %l0, 1, %l6                ! l6 = cwp + 1
        and     %l6, SPARC_PSR_CWP_MASK, %l6 ! do the modulo on it
        srl     %l4, %l6, %l5              ! l5 = win >> cwp + 1 ; shift count
                                           !  and CWP are conveniently LS 5 bits
        cmp     %l5, 1                     ! Is tasks window invalid?
        bne     good_task_window

        /*
         *  The following code is the same as a 1 position left rotate of WIM.
         */

        sll     %l4, 1, %l5                ! l5 = WIM << 1
        srl     %l4, SPARC_NUMBER_OF_REGISTER_WINDOWS-1 , %l4
                                           ! l4 = WIM >> (Number Windows - 1)
        or      %l4, %l5, %l4              ! l4 = (WIM << 1) |
                                           !      (WIM >> (Number Windows - 1))

        /*
         *  Now restore the window just as if we underflowed to it.
         */

        wr      %l4, 0, %wim               ! WIM = new WIM
        nop                                ! must delay after writing WIM
        nop
        nop
        restore                            ! now into the tasks window

        ldd     [%g1 + CPU_STACK_FRAME_L0_OFFSET], %l0
        ldd     [%g1 + CPU_STACK_FRAME_L2_OFFSET], %l2
        ldd     [%g1 + CPU_STACK_FRAME_L4_OFFSET], %l4
        ldd     [%g1 + CPU_STACK_FRAME_L6_OFFSET], %l6
        ldd     [%g1 + CPU_STACK_FRAME_I0_OFFSET], %i0
        ldd     [%g1 + CPU_STACK_FRAME_I2_OFFSET], %i2
        ldd     [%g1 + CPU_STACK_FRAME_I4_OFFSET], %i4
        ldd     [%g1 + CPU_STACK_FRAME_I6_FP_OFFSET], %i6
                                           ! reload of sp clobbers ISF
        save                               ! Back to ISR dispatch window

good_task_window:

        mov     %l0, %psr                  !  **** DISABLE TRAPS ****
        nop; nop; nop
                                           !  and restore condition codes.
        ld      [%g1 + ISF_G1_OFFSET], %g1 ! restore g1
        jmp     %l1                        ! transfer control and
        rett    %l2                        ! go back to tasks window

/* end of file */
