/*  cpu_asm.s
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
 *  Ported to Niagara and UltraSPARC III (US3) implementations of the SPARC-v9.
 *  Niagara and US3 modifications of respective RTEMS file: 
 *    COPYRIGHT (c) 2010. Gedare Bloom.
 */

#include <rtems/asm.h>
#include <rtems/score/percpu.h>


/* 
 *  The assembler needs to be told that we know what to do with 
 *  the global registers.
 */
.register %g2, #scratch
.register %g3, #scratch
.register %g6, #scratch
.register %g7, #scratch


    /*
     *  void _ISR_Handler()
     *
     *  This routine provides the RTEMS interrupt management.
     *
     *  We enter this handler from the 8 instructions in the trap table with
     *  the following registers assumed to be set as shown:
     *
     *    g4 = tstate (old l0)
     *    g2 = trap type (vector) (old l3)
     *
     *  NOTE: By an executive defined convention:
     *    if trap type is between 0 and 511 it is an asynchronous trap
     *    if trap type is between 512 and 1023 it is an asynchonous trap
     */

  .align 4
PUBLIC(_ISR_Handler)
  SYM(_ISR_Handler):

    /* 
     * The ISR is called at TL = 1. 
     * On sun4u we use the alternate globals set.     
     *
     * On entry:
     *   g4 = tstate (from trap table)
     *   g2 = trap vector #
     * 
     * In either case, note that trap handlers share a register window with 
     * the interrupted context, unless we explicitly enter a new window. This 
     * differs from Sparc v8, in which a dedicated register window is saved 
     * for trap handling.  This means we have to avoid overwriting any registers
     * that we don't save.
     *
     */


    /*
     *  save some or all context on stack
     */

    /*
     *  Save the state of the interrupted task -- especially the global
     *  registers -- in the Interrupt Stack Frame.  Note that the ISF
     *  includes a regular minimum stack frame which will be used if
     *  needed by register window overflow and underflow handlers.
     *
     *  This is slightly wasteful, since the stack already has the window
     *  overflow space reserved, but there is no obvious way to ensure 
     *  we can store the interrupted state and still handle window 
     *  spill/fill correctly, since there is no room for the ISF.
     *
     */

    /* this is for debugging purposes, make sure that TL = 1, otherwise 
     * things might get dicey */
    rdpr %tl, %g1
    cmp %g1, 1
    be 1f
    nop

    0: ba 0b
    nop

    1:
    /* first store the sp of the interrupted task temporarily in g1 */
    mov   %sp, %g1

    sub     %sp, CONTEXT_CONTROL_INTERRUPT_FRAME_SIZE, %sp
    ! make space for Stack_Frame||ISF

    /* save tstate, tpc, tnpc, pil */
    stx   %g4, [%sp + STACK_BIAS + ISF_TSTATE_OFFSET]  
    rdpr  %pil, %g3
    rdpr  %tpc, %g4
    rdpr  %tnpc, %g5
    stx   %g3, [%sp + STACK_BIAS + ISF_PIL_OFFSET]
    stx   %g4, [%sp + STACK_BIAS + ISF_TPC_OFFSET]
    stx   %g5, [%sp + STACK_BIAS + ISF_TNPC_OFFSET]
    stx   %g2, [%sp + STACK_BIAS + ISF_TVEC_OFFSET]

    rd  %y, %g4        ! save y
    stx   %g4, [%sp + STACK_BIAS + ISF_Y_OFFSET]

    ! save interrupted frame's output regs
    stx     %o0, [%sp + STACK_BIAS + ISF_O0_OFFSET]     ! save o0
    stx     %o1, [%sp + STACK_BIAS + ISF_O1_OFFSET]     ! save o1
    stx     %o2, [%sp + STACK_BIAS + ISF_O2_OFFSET]     ! save o2
    stx     %o3, [%sp + STACK_BIAS + ISF_O3_OFFSET]     ! save o3
    stx     %o4, [%sp + STACK_BIAS + ISF_O4_OFFSET]     ! save o4
    stx     %o5, [%sp + STACK_BIAS + ISF_O5_OFFSET]     ! save o5
    stx     %g1, [%sp + STACK_BIAS + ISF_O6_SP_OFFSET]  ! save o6/sp
    stx     %o7, [%sp + STACK_BIAS + ISF_O7_OFFSET]     ! save o7

    mov  %g1, %o5    ! hold the old sp here for now
    mov  %g2, %o1    ! we'll need trap # later

    /* switch to TL[0] */
    wrpr  %g0, 0, %tl

    /* switch to normal globals */
#if defined (SUN4U)
    /* the assignment to pstate below will mask out the AG bit */
#elif defined (SUN4V)
    wrpr  %g0, 0, %gl
#endif
    /* get pstate to known state */
    wrpr  %g0, SPARC_PSTATE_PRIV_MASK | SPARC_PSTATE_PEF_MASK, %pstate

    ! save globals
    stx     %g1, [%sp + STACK_BIAS + ISF_G1_OFFSET]     ! save g1
    stx     %g2, [%sp + STACK_BIAS + ISF_G2_OFFSET]     ! save g2
    stx     %g3, [%sp + STACK_BIAS + ISF_G3_OFFSET]     ! save g3
    stx     %g4, [%sp + STACK_BIAS + ISF_G4_OFFSET]     ! save g4
    stx     %g5, [%sp + STACK_BIAS + ISF_G5_OFFSET]     ! save g5
    stx     %g6, [%sp + STACK_BIAS + ISF_G6_OFFSET]     ! save g6
    stx     %g7, [%sp + STACK_BIAS + ISF_G7_OFFSET]     ! save g7


  mov  %o1, %g2  ! get the trap #
  mov  %o5, %g7  ! store the interrupted %sp (preserve)
  mov  %sp, %o1  ! 2nd arg to ISR Handler = address of ISF
  add  %o1, STACK_BIAS, %o1 ! need to adjust for stack bias, 2nd arg = ISF

  /*
   *  Increment ISR nest level and Thread dispatch disable level.
   *
   *  Register usage for this section: (note, these are used later)
   *
   *    g3 = _Thread_Dispatch_disable_level pointer
   *    g5 = _Thread_Dispatch_disable_level value (uint32_t)
   *    g6 = _ISR_Nest_level pointer
   *    g4 = _ISR_Nest_level value (uint32_t)
   *    o5 = temp
   *
   *  NOTE: It is assumed that g6 - g7 will be preserved until the ISR
   *        nest and thread dispatch disable levels are unnested.
   */

  setx  SYM(_Thread_Dispatch_disable_level), %o5, %g3
  lduw  [%g3], %g5
  setx  ISR_NEST_LEVEL, %o5, %g6
  lduw  [%g6], %g4

  add      %g5, 1, %g5
  stuw     %g5, [%g3]

  add      %g4, 1, %g4
  stuw     %g4, [%g6]

  /*
   *  If ISR nest level was zero (now 1), then switch stack.
   */

  subcc    %g4, 1, %g4             ! outermost interrupt handler?
  bnz      dont_switch_stacks      ! No, then do not switch stacks

  setx  SYM(INTERRUPT_STACK_HIGH), %o5, %g1
  ldx  [%g1], %sp

  /*
   * Adjust the stack for the stack bias
   */
  sub     %sp, STACK_BIAS, %sp

  /*
   *  Make sure we have a place on the stack for the window overflow
   *  trap handler to write into.  At this point it is safe to
   *  enable traps again.
   */

  sub      %sp, CPU_MINIMUM_STACK_FRAME_SIZE, %sp

  dont_switch_stacks:
  /*
   *  Check if we have an external interrupt (trap 0x41 - 0x4f). If so,
   *  set the PIL to mask off interrupts with lower priority.
   *
   *  The original PIL is not modified since it will be restored
   *  when the interrupt handler returns.
   */

  and      %g2, 0x0ff, %g1 ! is bottom byte of vector number [0x41,0x4f]?

  subcc    %g1, 0x41, %g0
  bl       dont_fix_pil
  subcc    %g1, 0x4f, %g0
  bg       dont_fix_pil
  nop
  wrpr     %g0, %g1, %pil

  dont_fix_pil:
  /* We need to be careful about enabling traps here.
   *
   * We already stored off the tstate, tpc, and tnpc, and switched to
   * TL = 0, so it should be safe.
   */

  /* zero out g4 so that ofw calls work */
  mov  %g0, %g4

  ! **** ENABLE TRAPS ****
  wrpr  %g0, SPARC_PSTATE_PRIV_MASK | SPARC_PSTATE_PEF_MASK | \
    SPARC_PSTATE_IE_MASK, %pstate 

    /*
     *  Vector to user's handler.
     *
     *  NOTE: TBR may no longer have vector number in it since
     *        we just enabled traps.  It is definitely in g2.
     */
    setx  SYM(_ISR_Vector_table), %o5, %g1
    ldx      [%g1], %g1
    and      %g2, 0x1FF, %o5        ! remove synchronous trap indicator
    sll      %o5, 3, %o5            ! o5 = offset into table
    ldx      [%g1 + %o5], %g1       ! g1 = _ISR_Vector_table[ vector ]


    ! o1 = 2nd arg = address of the ISF
    !   WAS LOADED WHEN ISF WAS SAVED!!!
    mov      %g2, %o0               ! o0 = 1st arg = vector number
    call     %g1, 0
    nop                             ! delay slot

    /*
     *  Redisable traps so we can finish up the interrupt processing.
     *  This is a conservative place to do this.
     */
    ! **** DISABLE TRAPS ****
    wrpr  %g0, SPARC_PSTATE_PRIV_MASK, %pstate

    /* 
     * We may safely use any of the %o and %g registers, because 
     * we saved them earlier (and any other interrupt that uses 
     * them will also save them).  Right now, the state of those
     * registers are as follows:
     *  %o registers: unknown (user's handler may have destroyed)
     *  %g1,g4,g5: scratch
     *  %g2: unknown: was trap vector
     *  %g3: uknown: was _Thread_Dispatch_Disable_level pointer
     *  %g6: _ISR_Nest_level
     *  %g7: interrupted task's sp
     */

    /*
     *  Increment ISR nest level and Thread dispatch disable level.
     *
     *  Register usage for this section: (note: as used above)
     *
     *    g3 = _Thread_Dispatch_disable_level pointer
     *    g5 = _Thread_Dispatch_disable_level value
     *    g6 = _ISR_Nest_level pointer
     *    g4 = _ISR_Nest_level value
     *    o5 = temp
     */

    /* We have to re-load the values from memory, because there are
     * not enough registers that we know will be preserved across the
     * user's handler. If this is a problem, we can create a register
     * window for _ISR_Handler.
     */

    setx  SYM(_Thread_Dispatch_disable_level), %o5, %g3
    lduw  [%g3],%g5
    lduw  [%g6],%g4
    sub   %g5, 1, %g5
    stuw  %g5, [%g3]
    sub   %g4, 1, %g4
    stuw  %g4, [%g6]

    orcc  %g4, %g0, %g0           ! ISRs still nested?
    bnz   dont_restore_stack      ! Yes then don't restore stack yet
    nop

    /*
     *  This is the outermost interrupt handler. Need to get off the
     *  CPU Interrupt Stack and back to the tasks stack.
     *
     *  The following subtract should get us back on the interrupted
     *  tasks stack and add enough room to invoke the dispatcher.
     *  When we enable traps, we are mostly back in the context
     *  of the task and subsequent interrupts can operate normally.
     *
     *  Now %sp points to the bottom of the ISF.
     *  
     */

    sub      %g7,   CONTEXT_CONTROL_INTERRUPT_FRAME_SIZE, %sp

    dont_restore_stack:

    /*
     *  If dispatching is disabled (includes nested interrupt case),
     *  then do a "simple" exit.
     */

    orcc     %g5, %g0, %g0   ! Is dispatching disabled?
    bnz      simple_return   ! Yes, then do a "simple" exit
    ! NOTE: Use the delay slot
    mov      %g0, %g4  ! clear g4 for ofw

    ! Are we dispatching from a previous ISR in the interrupted thread?
    setx  SYM(_CPU_ISR_Dispatch_disable), %o5, %g5
    lduw     [%g5], %o5
    orcc     %o5, %g0, %g0   ! Is this thread already doing an ISR?
    bnz      simple_return   ! Yes, then do a "simple" exit
    nop

    setx    DISPATCH_NEEDED, %o5, %g7


    /*
     *  If a context switch is necessary, then do fudge stack to
     *  return to the interrupt dispatcher.
     */

    ldub     [%g7], %o5

    orcc     %o5, %g0, %g0   ! Is thread switch necessary?
    bz       simple_return   ! no, then do a simple return. otherwise fallthru
    nop

    /*
     *  Invoke interrupt dispatcher.
     */
PUBLIC(_ISR_Dispatch)
  SYM(_ISR_Dispatch):
    ! Set ISR dispatch nesting prevention flag
      mov      1, %o1
      setx     SYM(_CPU_ISR_Dispatch_disable), %o5, %o2
      stuw     %o1, [%o2]


      !  **** ENABLE TRAPS ****
      wrpr  %g0, SPARC_PSTATE_PRIV_MASK | SPARC_PSTATE_PEF_MASK | \
        SPARC_PSTATE_IE_MASK, %pstate
        isr_dispatch:
        call    SYM(_Thread_Dispatch), 0
        nop

        /*
         *  We invoked _Thread_Dispatch in a state similar to the interrupted
         *  task.  In order to safely be able to tinker with the register
         *  windows and get the task back to its pre-interrupt state, 
         *  we need to disable interrupts. 
         */
      mov   2, %g4        ! syscall (disable interrupts)
      ta    0             ! syscall (disable interrupts)
      mov   0, %g4

  /*
   *  While we had ISR dispatching disabled in this thread,
   *  did we miss anything.  If so, then we need to do another
   *  _Thread_Dispatch before leaving this ISR Dispatch context.
   */

  setx     DISPATCH_NEEDED, %o5, %o1
  ldub     [%o1], %o2

  orcc     %o2, %g0, %g0   ! Is thread switch necessary?
  bz       allow_nest_again ! No, then clear out and return
  nop

  ! Yes, then invoke the dispatcher
dispatchAgain:
  mov      3, %g4        ! syscall (enable interrupts)
  ta       0             ! syscall (enable interrupts)
  ba       isr_dispatch
  mov      0, %g4

  allow_nest_again:

  ! Zero out ISR stack nesting prevention flag
  setx    SYM(_CPU_ISR_Dispatch_disable), %o5, %o1
  stuw    %g0,[%o1]

  /*
   *  The CWP in place at this point may be different from
   *  that which was in effect at the beginning of the ISR if we
   *  have been context switched between the beginning of this invocation
   *  of _ISR_Handler and this point.  Thus the CWP and WIM should
   *  not be changed back to their values at ISR entry time.  Any
   *  changes to the PSR must preserve the CWP.
   */

  simple_return:
  flushw          ! get register windows to a 'clean' state 

  ! **** DISABLE TRAPS ****
  wrpr    %g0, SPARC_PSTATE_PRIV_MASK, %pstate

  ldx     [%sp + STACK_BIAS + ISF_Y_OFFSET], %o1      ! restore y
  wr      %o1, 0, %y

  ldx  [%sp + STACK_BIAS + ISF_TSTATE_OFFSET], %g1

! see if cwp is proper (tstate.cwp == cwp)
  and  %g1, 0x1F, %g6
  rdpr  %cwp, %g7
  cmp  %g6, %g7
  bz  good_window
  nop

  /*
   * Fix the CWP. Need the cwp to be the proper cwp that
   * gets restored when returning from the trap via retry/done. Do 
   * this before reloading the task's output regs. Basically fake a
   * window spill/fill.
   *
   * Is this necessary on sun4v? Why not just re-write 
   * tstate.cwp to be equal to the current cwp?
   */
  mov  %sp, %g1
  stx  %l0, [%sp + STACK_BIAS + CPU_STACK_FRAME_L0_OFFSET]
  stx  %l1, [%sp + STACK_BIAS + CPU_STACK_FRAME_L1_OFFSET]
  stx  %l2, [%sp + STACK_BIAS + CPU_STACK_FRAME_L2_OFFSET]
  stx  %l3, [%sp + STACK_BIAS + CPU_STACK_FRAME_L3_OFFSET]
  stx  %l4, [%sp + STACK_BIAS + CPU_STACK_FRAME_L4_OFFSET]
  stx  %l5, [%sp + STACK_BIAS + CPU_STACK_FRAME_L5_OFFSET]
  stx  %l6, [%sp + STACK_BIAS + CPU_STACK_FRAME_L6_OFFSET]
  stx  %l7, [%sp + STACK_BIAS + CPU_STACK_FRAME_L7_OFFSET]
  stx  %i0, [%sp + STACK_BIAS + CPU_STACK_FRAME_I0_OFFSET]
  stx  %i1, [%sp + STACK_BIAS + CPU_STACK_FRAME_I1_OFFSET]
  stx  %i2, [%sp + STACK_BIAS + CPU_STACK_FRAME_I2_OFFSET]
  stx  %i3, [%sp + STACK_BIAS + CPU_STACK_FRAME_I3_OFFSET]
  stx  %i4, [%sp + STACK_BIAS + CPU_STACK_FRAME_I4_OFFSET]
  stx  %i5, [%sp + STACK_BIAS + CPU_STACK_FRAME_I5_OFFSET]
  stx  %i6, [%sp + STACK_BIAS + CPU_STACK_FRAME_I6_FP_OFFSET]
  stx  %i7, [%sp + STACK_BIAS + CPU_STACK_FRAME_I7_OFFSET]
  wrpr  %g0, %g6, %cwp
  mov  %g1, %sp
  ldx  [%sp + STACK_BIAS + CPU_STACK_FRAME_L0_OFFSET], %l0
  ldx  [%sp + STACK_BIAS + CPU_STACK_FRAME_L1_OFFSET], %l1
  ldx  [%sp + STACK_BIAS + CPU_STACK_FRAME_L2_OFFSET], %l2
  ldx  [%sp + STACK_BIAS + CPU_STACK_FRAME_L3_OFFSET], %l3
  ldx  [%sp + STACK_BIAS + CPU_STACK_FRAME_L4_OFFSET], %l4
  ldx  [%sp + STACK_BIAS + CPU_STACK_FRAME_L5_OFFSET], %l5
  ldx  [%sp + STACK_BIAS + CPU_STACK_FRAME_L6_OFFSET], %l6
  ldx  [%sp + STACK_BIAS + CPU_STACK_FRAME_L7_OFFSET], %l7
  ldx  [%sp + STACK_BIAS + CPU_STACK_FRAME_I0_OFFSET], %i0
  ldx  [%sp + STACK_BIAS + CPU_STACK_FRAME_I1_OFFSET], %i1
  ldx  [%sp + STACK_BIAS + CPU_STACK_FRAME_I2_OFFSET], %i2
  ldx  [%sp + STACK_BIAS + CPU_STACK_FRAME_I3_OFFSET], %i3
  ldx  [%sp + STACK_BIAS + CPU_STACK_FRAME_I4_OFFSET], %i4
  ldx  [%sp + STACK_BIAS + CPU_STACK_FRAME_I5_OFFSET], %i5
  ldx  [%sp + STACK_BIAS + CPU_STACK_FRAME_I6_FP_OFFSET], %i6
  ldx  [%sp + STACK_BIAS + CPU_STACK_FRAME_I7_OFFSET], %i7


  good_window:


  /*
   *  Restore tasks global and out registers
   */

  ldx     [%sp + STACK_BIAS + ISF_G1_OFFSET], %g1    ! restore g1
  ldx     [%sp + STACK_BIAS + ISF_G2_OFFSET], %g2    ! restore g2
  ldx     [%sp + STACK_BIAS + ISF_G3_OFFSET], %g3    ! restore g3
  ldx     [%sp + STACK_BIAS + ISF_G4_OFFSET], %g4    ! restore g4
  ldx     [%sp + STACK_BIAS + ISF_G5_OFFSET], %g5    ! restore g5
  ldx     [%sp + STACK_BIAS + ISF_G6_OFFSET], %g6    ! restore g6
  ldx     [%sp + STACK_BIAS + ISF_G7_OFFSET], %g7    ! restore g7

  ! Assume the interrupted context is in TL 0 with GL 0 / normal globals.
  ! When tstate is restored at done/retry, the interrupted context is restored.
  ! return to TL[1], GL[1], and restore TSTATE, TPC, and TNPC
  wrpr  %g0, 1, %tl

  ! return to GL=1 or AG
#if defined(SUN4U)
    rdpr  %pstate, %o1
    or  %o1, SPARC_PSTATE_AG_MASK, %o1
    wrpr  %o1, %g0, %pstate                 ! go to AG.
#elif defined(SUN4V)
  wrpr  %g0, 1, %gl
#endif

! now we can use global registers (at gl=1 or AG)
  ldx   [%sp + STACK_BIAS + ISF_PIL_OFFSET], %g3
  ldx   [%sp + STACK_BIAS + ISF_TPC_OFFSET], %g4
  ldx   [%sp + STACK_BIAS + ISF_TNPC_OFFSET], %g5
  ldx   [%sp + STACK_BIAS + ISF_TSTATE_OFFSET], %g1
  ldx   [%sp + STACK_BIAS + ISF_TVEC_OFFSET], %g2
  wrpr  %g0, %g3, %pil
  wrpr  %g0, %g4, %tpc
  wrpr  %g0, %g5, %tnpc

  wrpr    %g0, %g1, %tstate

  ldx     [%sp + STACK_BIAS + ISF_O0_OFFSET], %o0    ! restore o0
  ldx     [%sp + STACK_BIAS + ISF_O1_OFFSET], %o1    ! restore o1
  ldx     [%sp + STACK_BIAS + ISF_O2_OFFSET], %o2    ! restore o2
  ldx     [%sp + STACK_BIAS + ISF_O3_OFFSET], %o3    ! restore o3
  ldx     [%sp + STACK_BIAS + ISF_O4_OFFSET], %o4    ! restore o4
  ldx     [%sp + STACK_BIAS + ISF_O5_OFFSET], %o5    ! restore o5
  ! sp is restored later
  ldx     [%sp + STACK_BIAS + ISF_O7_OFFSET], %o7    ! restore o7

  ldx     [%sp + STACK_BIAS + ISF_O6_SP_OFFSET], %o6 ! restore o6/sp

  /*
   *  Determine whether to re-execute the trapping instruction 
   *  (asynchronous trap) or to skip the trapping instruction
   *  (synchronous trap).
   */

  andcc   %g2, SPARC_SYNCHRONOUS_TRAP_BIT_MASK, %g0
  ! Is this a synchronous trap?
  be  not_synch             ! No, then skip trapping instruction
  mov  0, %g4
  retry        ! re-execute trapping instruction
  not_synch:
  done        ! skip trapping instruction

/* end of file */
