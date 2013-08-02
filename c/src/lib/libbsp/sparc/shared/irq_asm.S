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

#include <rtems/asm.h>
#include <rtems/score/percpu.h>
#include <bspopts.h>

.macro GET_SELF_CPU_CONTROL REG, TMP
        sethi    %hi(_Per_CPU_Information), \REG
        add      \REG, %lo(_Per_CPU_Information), \REG

#if defined( RTEMS_SMP )
#if BSP_LEON3_SMP
        /* LEON3 SMP support */
        rd       %asr17, \TMP
        srl      \TMP, 28, \TMP /* CPU number is upper 4 bits so shift */
#else
        mov      0, \TMP
        nop
#endif
        sll      \TMP, PER_CPU_CONTROL_SIZE_LOG2, \TMP
        add      \REG, \TMP, \REG
#endif /* defined( RTEMS_SMP ) */
.endm

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

        std     %l0, [%o0 + L0_OFFSET]       ! save the local registers
        std     %l2, [%o0 + L2_OFFSET]
        std     %l4, [%o0 + L4_OFFSET]
        std     %l6, [%o0 + L6_OFFSET]

        std     %i0, [%o0 + I0_OFFSET]       ! save the input registers
        std     %i2, [%o0 + I2_OFFSET]
        std     %i4, [%o0 + I4_OFFSET]
        std     %i6, [%o0 + I6_FP_OFFSET]

        std     %o0, [%o0 + O0_OFFSET]       ! save the output registers
        std     %o2, [%o0 + O2_OFFSET]
        std     %o4, [%o0 + O4_OFFSET]
        std     %o6, [%o0 + O6_SP_OFFSET]

        ! o3 = self per-CPU control
        GET_SELF_CPU_CONTROL %o3, %o4

        ! load the ISR stack nesting prevention flag
        ld      [%o3 + SPARC_PER_CPU_ISR_DISPATCH_DISABLE], %o4
        ! save it a bit later so we do not waste a couple of cycles

        rd      %psr, %o2
        st      %o2, [%o0 + PSR_OFFSET]      ! save status register

        ! Now actually save ISR stack nesting prevention flag
        st       %o4, [%o0 + ISR_DISPATCH_DISABLE_STACK_OFFSET]

        /*
         *  This is entered from _CPU_Context_restore with:
         *    o1 = context to restore
         *    o2 = psr
         *    o3 = self per-CPU control
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
        ! Store it to memory later to use the cycles

        ldd     [%o1 + L0_OFFSET], %l0        ! restore the local registers
        ldd     [%o1 + L2_OFFSET], %l2
        ldd     [%o1 + L4_OFFSET], %l4
        ldd     [%o1 + L6_OFFSET], %l6

        ! Now restore thread specific ISR dispatch prevention flag
        st      %o2, [%o3 + SPARC_PER_CPU_ISR_DISPATCH_DISABLE]

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
        GET_SELF_CPU_CONTROL %o3, %o4
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

save_isf:

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
        std     %g6, [%sp + ISF_G6_OFFSET]     ! save g6, g7

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
         *    l4 = _Thread_Dispatch_disable_level pointer
         *    l5 = per cpu info pointer
         *    l6 = _Thread_Dispatch_disable_level value
         *    l7 = _ISR_Nest_level value
         *
         *  NOTE: It is assumed that l4 - l7 will be preserved until the ISR
         *        nest and thread dispatch disable levels are unnested.
         */

	GET_SELF_CPU_CONTROL %l5, %l7

	ld       [%l5 + PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL], %l6
	ld       [%l5 + PER_CPU_ISR_NEST_LEVEL], %l7

        add      %l6, 1, %l6
        st       %l6, [%l5 + PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]

        add      %l7, 1, %l7
        st       %l7, [%l5 + PER_CPU_ISR_NEST_LEVEL]

        /*
         *  If ISR nest level was zero (now 1), then switch stack.
         */

        mov      %sp, %fp
        subcc    %l7, 1, %l7             ! outermost interrupt handler?
        bnz      dont_switch_stacks      ! No, then do not switch stacks

        nop
	ld       [%l5 + PER_CPU_INTERRUPT_STACK_HIGH], %sp

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
	and	 %l3, 0x0ff, %g4

/* This is a fix for ERC32 with FPU rev.B or rev.C */

#if defined(FPU_REVB)


        subcc    %g4, 0x08, %g0
	be	 fpu_revb
        subcc    %g4, 0x11, %g0
        bl       dont_fix_pil
        subcc    %g4, 0x1f, %g0
        bg       dont_fix_pil
        sll      %g4, 8, %g4
        and      %g4, SPARC_PSR_PIL_MASK, %g4
        andn     %l0, SPARC_PSR_PIL_MASK, %g5
        or       %g4, %g5, %g5
	srl	 %l0, 12, %g4
	andcc	 %g4, 1, %g0
        be       dont_fix_pil
	nop
	ba,a	 enable_irq


fpu_revb:
	srl	 %l0, 12, %g4	! check if EF is set in %psr
	andcc	 %g4, 1, %g0
	be	 dont_fix_pil	! if FPU disabled than continue as normal
	and	 %l3, 0xff, %g4
        subcc    %g4, 0x08, %g0
        bne      enable_irq	! if not a FPU exception then do two fmovs
	set	 __sparc_fq, %g4
	st	 %fsr, [%g4]	! if FQ is not empty and FQ[1] = fmovs
	ld	 [%g4], %g4	! than this is bug 3.14
	srl 	 %g4, 13, %g4
	andcc 	 %g4, 1, %g0
        be       dont_fix_pil
	set	 __sparc_fq, %g4
	std	 %fq, [%g4]
	ld	 [%g4+4], %g4
	set	 0x81a00020, %g5
	subcc	 %g4, %g5, %g0
	bne,a	 dont_fix_pil2
        wr       %l0, SPARC_PSR_ET_MASK, %psr ! **** ENABLE TRAPS ****
	ba,a	 simple_return
	
enable_irq:
        or       %g5, SPARC_PSR_PIL_MASK, %g4
        wr       %g4, SPARC_PSR_ET_MASK, %psr ! **** ENABLE TRAPS ****
	nop; nop; nop
	fmovs	 %f0, %f0
	ba	 dont_fix_pil
	fmovs	 %f0, %f0

	.data
	.global	__sparc_fq
	.align 8
__sparc_fq:
	.word 0,0

	.text
/* end of ERC32 FPU rev.B/C fix */

#else

        subcc    %g4, 0x11, %g0
        bl       dont_fix_pil
        subcc    %g4, 0x1f, %g0
        bg       dont_fix_pil
        sll      %g4, 8, %g4
        and      %g4, SPARC_PSR_PIL_MASK, %g4
        andn     %l0, SPARC_PSR_PIL_MASK, %g5
        ba       pil_fixed
        or       %g4, %g5, %g5
#endif

dont_fix_pil:
        or       %g5, SPARC_PSR_PIL_MASK, %g5
pil_fixed:
        wr       %g5, SPARC_PSR_ET_MASK, %psr ! **** ENABLE TRAPS ****
dont_fix_pil2:

        /*
         *  Vector to user's handler.
         *
         *  NOTE: TBR may no longer have vector number in it since
         *        we just enabled traps.  It is definitely in l3.
         */

        sethi    %hi(SYM(_ISR_Vector_table)), %g4
        ld       [%g4+%lo(SYM(_ISR_Vector_table))], %g4
        and      %l3, 0xFF, %g5         ! remove synchronous trap indicator
        sll      %g5, 2, %g5            ! g5 = offset into table
        ld       [%g4 + %g5], %g4       ! g4 = _ISR_Vector_table[ vector ]


                                        ! o1 = 2nd arg = address of the ISF
                                        !   WAS LOADED WHEN ISF WAS SAVED!!!
        mov      %l3, %o0               ! o0 = 1st arg = vector number
        call     %g4, 0
        nop                             ! delay slot

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
         *    l4 = _Thread_Dispatch_disable_level pointer
         *    l5 = per cpu info pointer
         *    l6 = _Thread_Dispatch_disable_level value
         *    l7 = _ISR_Nest_level value
         */

        sub      %l6, 1, %l6
        st       %l6, [%l5 + PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]

        st       %l7, [%l5 + PER_CPU_ISR_NEST_LEVEL]

        /*
         *  If dispatching is disabled (includes nested interrupt case),
         *  then do a "simple" exit.
         */

        orcc     %l6, %g0, %g0   ! Is dispatching disabled?
        bnz      simple_return   ! Yes, then do a "simple" exit
        nop

        ! Are we dispatching from a previous ISR in the interrupted thread?
        ld       [%l5 + SPARC_PER_CPU_ISR_DISPATCH_DISABLE], %l7
        orcc     %l7, %g0, %g0   ! Is this thread already doing an ISR?
        bnz      simple_return   ! Yes, then do a "simple" exit
        nop


        /*
         *  If a context switch is necessary, then do fudge stack to
         *  return to the interrupt dispatcher.
         */

        ldub     [%l5 + PER_CPU_DISPATCH_NEEDED], %l6

        orcc     %l6, %g0, %g0   ! Is thread switch necessary?
        bz       simple_return   ! no, then do a simple return
        nop

        /*
         *  Invoke interrupt dispatcher.
         */

        ! Set ISR dispatch nesting prevention flag
        mov      1,%l6
        st       %l6, [%l5 + SPARC_PER_CPU_ISR_DISPATCH_DISABLE]

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
        call    SYM(_Thread_Dispatch), 0
        nop

	/*
	 *  We invoked _Thread_Dispatch in a state similar to the interrupted
	 *  task.  In order to safely be able to tinker with the register
	 *  windows and get the task back to its pre-interrupt state,
	 *  we need to disable interrupts disabled so we can safely tinker
	 *  with the register windowing.  In particular, the CWP in the PSR
	 *  is fragile during this period. (See PR578.)
	 */
	mov	2,%g1				! syscall (disable interrupts)
	ta	0				! syscall (disable interrupts)

        /*
         *  While we had ISR dispatching disabled in this thread,
         *  did we miss anything.  If so, then we need to do another
         *  _Thread_Dispatch before leaving this ISR Dispatch context.
         */

	GET_SELF_CPU_CONTROL %l5, %l7

        ldub     [%l5 + PER_CPU_DISPATCH_NEEDED], %l7

        orcc     %l7, %g0, %g0    ! Is thread switch necesary?
        bz       allow_nest_again ! No, then clear out and return
        nop

        ! Yes, then invoke the dispatcher
dispatchAgain:
	mov	3,%g1				! syscall (enable interrupts)
	ta	0				! syscall (enable interrupts)
        ba      isr_dispatch
        nop

allow_nest_again:

        ! Zero out ISR stack nesting prevention flag
        st       %g0, [%l5 + SPARC_PER_CPU_ISR_DISPATCH_DISABLE]

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
        ldd     [%fp + ISF_G6_OFFSET], %g6    ! restore g6, g7

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
