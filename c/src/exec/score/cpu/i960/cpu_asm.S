/*
 *  This file contains all assembly code for the i960 port of RTEMS.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */
	.data
_ISR_reg_save:
	.word  0
	.word  0
	.word  0
	.word  0
	.word  0
	.word  0

          .text
/*
 * Format of i960ca Register structure
 */

.set REG_R0_PFP    , 0                # (r0)  Previous Frame Pointer
.set REG_R1_SP     , REG_R0_PFP+4     # (r1)  Stack Pointer
.set REG_PC        , REG_R1_SP+4      # (pc)  Processor Controls
.set REG_G8        , REG_PC+4         # (g8)  Global Register 8
.set REG_G9        , REG_G8+4         # (g9)  Global Register 9
.set REG_G10       , REG_G9+4         # (g10) Global Register 10
.set REG_G11       , REG_G10+4        # (g11) Global Register 11
.set REG_G12       , REG_G11+4        # (g12) Global Register 12
.set REG_G13       , REG_G12+4        # (g13) Global Register 13
.set REG_G14       , REG_G13+4        # (g14) Global Register 14
.set REG_G15_FP    , REG_G14+4        # (g15) Global Register 15
.set SIZE_REGS     , REG_G15_FP+4     # size of cpu_context_registers
                                      #    structure

/*
 *  void _CPU_Context_switch( run_context, heir_context )
 *
 *  This routine performs a normal non-FP context.
 */
          .align    4
          .globl    __CPU_Context_switch

__CPU_Context_switch:
          modpc     0,0,g2                   # get old intr level (PC)
          st        g2,REG_PC(g0)            # save pc
          stq       g8,REG_G8(g0)            # save g8-g11
          stq       g12,REG_G12(g0)          # save g12-g15
          stl       pfp,REG_R0_PFP(g0)       # save pfp, sp

restore:  flushreg                           # flush register cache
          ldconst   0x001f0000,g2            # g2 = PC mask
          ld        REG_PC(g1),g3            # thread->Regs.pc = pc;
          ldq       REG_G12(g1),g12          # restore g12-g15
          ldl       REG_R0_PFP(g1),pfp       # restore pfp, sp
          ldq       REG_G8(g1),g8            # restore g8-g11
          modpc     0,g2,g3                  # restore PC register
          ret

/*
 *  void _CPU_Context_restore( new_context )
 *
 *  This routine performs a normal non-FP context.
 */

        .globl __CPU_Context_restore
__CPU_Context_restore:
          mov       g0,g1                    # g0 = _Thread_executing
          b         restore

/*PAGE
 *  void _CPU_Context_save_fp_context( &fp_context_ptr )
 *  void _CPU_Context_restore_fp_context( &fp_context_ptr )
 *
 *  There is currently no hardware floating point for the i960.
 */

          .globl    __CPU_Context_save_fp
          .globl    __CPU_Context_restore_fp
__CPU_Context_save_fp:
__CPU_Context_restore_fp:
#if ( I960_HAS_FPU == 1 )
#error "Floating point support for i960 family has been implemented!!!"
#endif
          ret

/*PAGE
 *  void __ISR_Handler()
 *
 *  This routine provides the RTEMS interrupt management.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  NOTE:
 *    Upon entry, the supervisor stack will contain a stack frame
 *    back to the interrupted thread and the interrupt stack will contain
 *    an interrupt stack frame.  If dispatching is enabled, this
 *    is the outer most interrupt, and (a context switch is necessary or
 *    the current thread has signals), then set up the supervisor stack to
 *    transfer control to the interrupt dispatcher.
 */

          .globl    __ISR_Handler
__ISR_Handler:
          #ldconst 1,r8
          #modpc   0,r8,r8     # enable tracing

                               # r4 = &_Thread_Dispatch_disable_level
          ld         __Thread_Dispatch_disable_level,r4
          movl      g0,r8                    # save g0-g1

          ld        -16+8(fp),g0             # g0 = vector number
          movl      g2,r10                   # save g2-g3

          ld         __ISR_Nest_level,r5     # r5 = &_Isr_nest_level
          mov       g14,r7                   # save g14

          lda       0,g14                    # NOT Branch and Link
          movl      g4,r12                   # save g4-g5

          lda       1(r4),r4                 # increment dispatch disable level
          movl      g6,r14                   # save g6-g7

          ld        __ISR_Vector_table,g1    # g1 = base of vector table

	  stq       g8, _ISR_reg_save        # save g8-g11
	  stl       g12, _ISR_reg_save+16    # save g12-g13

          ld        (g1)[g0*4],g1            # g1 = Users handler
          addo      1,r5,r5                  # increment ISR level

          st        r4,__Thread_Dispatch_disable_level
                                             # one ISR nest level deeper
          subo      1,r4,r4                  # decrement dispatch disable level

          st        r5,__ISR_Nest_level      # disable multitasking
          subo      1,r5,r5                  # decrement ISR nest level

          callx     (g1)                     # invoke user ISR

                                             # unnest multitasking
          st        r5,__ISR_Nest_level      # one less ISR nest level
          cmpobne.f 0,r4,exit                # If dispatch disabled, exit
          ldl       -16(fp),g0               # g0 = threads PC reg
                                             # g1 = threads AC reg
          ld        __Context_Switch_necessary,r6
                                             # r6 = Is thread switch necessary?
          bbs.f     13,g0,exit               # not outer level, then exit
          cmpobne.f 0,r6,bframe              # Switch necessary?

          ld        __ISR_Signals_to_thread_executing,g2
                                             # signals sent to Run_thread
                                             #   while in interrupt handler?
          cmpobe.f  0,g2,exit                # No, then exit

bframe:   mov       0,g2
          st        g2,__ISR_Signals_to_thread_executing

          ldconst   0x1f0000,g2              # g2 = intr disable mask
          mov       g2,g3                    # g3 = new intr level
          modpc     0,g2,g3                  # set new level

          andnot    7,pfp,r4                 # r4 = pfp without ret type
          flushreg                           # flush registers
                                             # push _Isr_dispatch ret frame
                                             #   build ISF in r4-r6
          ldconst   64,g2                    # g2 = size of stack frame
          ld        4(r4),g3                 # g3 = previous sp
          addo      g2,g3,r5                 # r5 = _Isr_dispatch SP
          lda       __ISR_Dispatch,r6        # r6 = _Isr_dispatch entry
          stt       r4,(g3)                  # set _Isr_dispatch ret info
          st        g1,16(g3)                # set r4 = AC for ISR disp
          or        7,g3,pfp                 # pfp to _Isr_dispatch
          flushreg
          b         exit1
exit:     st        r4,__Thread_Dispatch_disable_level
exit1:    mov       r7,g14                   # restore g14
          movq      r8,g0                    # restore g0-g3
          movq      r12,g4                   # restore g4-g7
		  ldq		_ISR_reg_save, g8		 # restore g8-g11
		  ldl		_ISR_reg_save+16, g12    # restore g12-g13
          ret


/*PAGE
 *
 *  void __ISR_Dispatch()
 *
 *  Entry point from the outermost interrupt service routine exit.
 *  The current stack is the supervisor mode stack.
 */

          .globl    __ISR_Dispatch
__ISR_Dispatch:
        mov       g14,r7
        mov       0,g14
        movq      g0,r8
        movq      g4,r12
        call      __Thread_Dispatch

        ldconst   -1,r5                    # r5 = reload mask
        modac     r5,r4,r4                 # restore threads AC register
        mov       r7,g14
        movq      r8,g0
        movq      r12,g4
        ret

