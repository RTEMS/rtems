/**
 * @file rtems/score/cpu.h
 *
 *  This include file contains information pertaining to the port of
 *  the executive to the SPARC processor.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CPU_H
#define _RTEMS_SCORE_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/types.h>
#include <rtems/score/sparc.h>

/* conditional compilation parameters */

/**
 *  Should the calls to _Thread_Enable_dispatch be inlined?
 *
 *  - If TRUE, then they are inlined.
 *  - If FALSE, then a subroutine call is made.
 *
 *  On this port, it is faster to inline _Thread_Enable_dispatch.
 */
#define CPU_INLINE_ENABLE_DISPATCH       TRUE

/**
 *  Should the body of the search loops in _Thread_queue_Enqueue_priority
 *  be unrolled one time?  In unrolled each iteration of the loop examines
 *  two "nodes" on the chain being searched.  Otherwise, only one node
 *  is examined per iteration.
 *
 *  - If TRUE, then the loops are unrolled.
 *  - If FALSE, then the loops are not unrolled.
 *
 *  This parameter could go either way on the SPARC.  The interrupt flash
 *  code is relatively lengthy given the requirements for nops following
 *  writes to the psr.  But if the clock speed were high enough, this would
 *  not represent a great deal of time.
 */
#define CPU_UNROLL_ENQUEUE_PRIORITY      TRUE

/**
 *  Does the executive manage a dedicated interrupt stack in software?
 *
 *  If TRUE, then a stack is allocated in _ISR_Handler_initialization.
 *  If FALSE, nothing is done.
 *
 *  The SPARC does not have a dedicated HW interrupt stack and one has
 *  been implemented in SW.
 */
#define CPU_HAS_SOFTWARE_INTERRUPT_STACK   TRUE

/**
 *  Does the CPU follow the simple vectored interrupt model?
 *
 *  - If TRUE, then RTEMS allocates the vector table it internally manages.
 *  - If FALSE, then the BSP is assumed to allocate and manage the vector
 *    table
 *
 *  THe SPARC is a simple vectored architecture.  Usually there is no
 *  PIC and the CPU directly vectors the interrupts.
 */
#define CPU_SIMPLE_VECTORED_INTERRUPTS TRUE

/** 
 *  Does this CPU have hardware support for a dedicated interrupt stack?
 *
 *  - If TRUE, then it must be installed during initialization.
 *  - If FALSE, then no installation is performed.
 *
 *  The SPARC does not have a dedicated HW interrupt stack.
 */
#define CPU_HAS_HARDWARE_INTERRUPT_STACK  FALSE

/**
 *  Do we allocate a dedicated interrupt stack in the Interrupt Manager?
 *
 *  - If TRUE, then the memory is allocated during initialization.
 *  - If FALSE, then the memory is allocated during initialization.
 *
 *  The SPARC does not have hardware support for switching to a 
 *  dedicated interrupt stack.  The port includes support for doing this
 *  in software.
 *
 */
#define CPU_ALLOCATE_INTERRUPT_STACK      TRUE

/**
 *  Does the RTEMS invoke the user's ISR with the vector number and
 *  a pointer to the saved interrupt frame (1) or just the vector
 *  number (0)?
 *
 *  The SPARC port does not pass an Interrupt Stack Frame pointer to
 *  interrupt handlers.
 */
#define CPU_ISR_PASSES_FRAME_POINTER 0

/**
 *  Does the CPU have hardware floating point?
 *
 *  - If TRUE, then the FLOATING_POINT task attribute is supported.
 *  - If FALSE, then the FLOATING_POINT task attribute is ignored.
 *
 *  This is set based upon the multilib settings.
 */
#if ( SPARC_HAS_FPU == 1 )
  #define CPU_HARDWARE_FP     TRUE
#else
  #define CPU_HARDWARE_FP     FALSE
#endif

/**
 *  The SPARC GCC port does not have a software floating point library
 *  that requires RTEMS assistance.
 */
#define CPU_SOFTWARE_FP     FALSE

/**
 *  Are all tasks FLOATING_POINT tasks implicitly?
 *
 *  - If TRUE, then the FLOATING_POINT task attribute is assumed.
 *  - If FALSE, then the FLOATING_POINT task attribute is followed.
 *
 *  The SPARC GCC port does not implicitly use floating point registers.
 */
#define CPU_ALL_TASKS_ARE_FP     FALSE

/**
 *  Should the IDLE task have a floating point context?
 *
 *  - If TRUE, then the IDLE task is created as a FLOATING_POINT task
 *    and it has a floating point context which is switched in and out.
 *  - If FALSE, then the IDLE task does not have a floating point context.
 *
 *  The IDLE task does not have to be floating point on the SPARC.
 */
#define CPU_IDLE_TASK_IS_FP      FALSE

/**
 *  Should the saving of the floating point registers be deferred
 *  until a context switch is made to another different floating point
 *  task?
 *
 *  - If TRUE, then the floating point context will not be stored until
 *  necessary.  It will remain in the floating point registers and not
 *  disturned until another floating point task is switched to.
 *
 *  - If FALSE, then the floating point context is saved when a floating
 *  point task is switched out and restored when the next floating point
 *  task is restored.  The state of the floating point registers between
 *  those two operations is not specified.
 *
 *  On the SPARC, we can disable the FPU for integer only tasks so
 *  it is safe to defer floating point context switches.
 */
#define CPU_USE_DEFERRED_FP_SWITCH       TRUE

/**
 *  Does this port provide a CPU dependent IDLE task implementation?
 *
 *  - If TRUE, then the routine _CPU_Thread_Idle_body
 *  must be provided and is the default IDLE thread body instead of
 *  _CPU_Thread_Idle_body.
 *
 *  - If FALSE, then use the generic IDLE thread body if the BSP does
 *  not provide one.
 *
 *  The SPARC architecture does not have a low power or halt instruction.
 *  It is left to the BSP and/or CPU specific code to provide an IDLE
 *  thread body which is aware of low power modes.
 */
#define CPU_PROVIDES_IDLE_THREAD_BODY    FALSE

/**
 *  Does the stack grow up (toward higher addresses) or down
 *  (toward lower addresses)?
 *
 *  - If TRUE, then the grows upward.
 *  - If FALSE, then the grows toward smaller addresses.
 *
 *  The stack grows to lower addresses on the SPARC.
 */
#define CPU_STACK_GROWS_UP               FALSE

/**
 *  The following is the variable attribute used to force alignment
 *  of critical data structures.  On some processors it may make
 *  sense to have these aligned on tighter boundaries than
 *  the minimum requirements of the compiler in order to have as
 *  much of the critical data area as possible in a cache line.
 *
 *  The SPARC does not appear to have particularly strict alignment
 *  requirements.  This value was chosen to take advantages of caches.
 */
#define CPU_STRUCTURE_ALIGNMENT          __attribute__ ((aligned (16)))

#define CPU_TIMESTAMP_USE_INT64_INLINE TRUE

/**
 *  Define what is required to specify how the network to host conversion
 *  routines are handled.
 *
 *  The SPARC is big endian.
 */
#define CPU_BIG_ENDIAN                           TRUE

/**
 *  Define what is required to specify how the network to host conversion
 *  routines are handled.
 *
 *  The SPARC is NOT little endian.
 */
#define CPU_LITTLE_ENDIAN                        FALSE

/**
 *  The following defines the number of bits actually used in the
 *  interrupt field of the task mode.  How those bits map to the
 *  CPU interrupt levels is defined by the routine _CPU_ISR_Set_level().
 *
 *  The SPARC has 16 interrupt levels in the PIL field of the PSR.
 */
#define CPU_MODES_INTERRUPT_MASK   0x0000000F

#ifndef ASM
/**
 *  This structure represents the organization of the minimum stack frame
 *  for the SPARC.  More framing information is required in certain situaions
 *  such as when there are a large number of out parameters or when the callee
 *  must save floating point registers.
 */
typedef struct {
  /** This is the offset of the l0 register. */
  uint32_t    l0;
  /** This is the offset of the l1 register. */
  uint32_t    l1;
  /** This is the offset of the l2 register. */
  uint32_t    l2;
  /** This is the offset of the l3 register. */
  uint32_t    l3;
  /** This is the offset of the l4 register. */
  uint32_t    l4;
  /** This is the offset of the l5 register. */
  uint32_t    l5;
  /** This is the offset of the l6 register. */
  uint32_t    l6;
  /** This is the offset of the l7 register. */
  uint32_t    l7;
  /** This is the offset of the l0 register. */
  uint32_t    i0;
  /** This is the offset of the i1 register. */
  uint32_t    i1;
  /** This is the offset of the i2 register. */
  uint32_t    i2;
  /** This is the offset of the i3 register. */
  uint32_t    i3;
  /** This is the offset of the i4 register. */
  uint32_t    i4;
  /** This is the offset of the i5 register. */
  uint32_t    i5;
  /** This is the offset of the i6 register. */
  uint32_t    i6_fp;
  /** This is the offset of the i7 register. */
  uint32_t    i7;
  /** This is the offset of the register used to return structures. */
  void       *structure_return_address;

  /*
   *  The following are for the callee to save the register arguments in
   *  should this be necessary.
   */
  /** This is the offset of the register for saved argument 0. */
  uint32_t    saved_arg0;
  /** This is the offset of the register for saved argument 1. */
  uint32_t    saved_arg1;
  /** This is the offset of the register for saved argument 2. */
  uint32_t    saved_arg2;
  /** This is the offset of the register for saved argument 3. */
  uint32_t    saved_arg3;
  /** This is the offset of the register for saved argument 4. */
  uint32_t    saved_arg4;
  /** This is the offset of the register for saved argument 5. */
  uint32_t    saved_arg5;
  /** This field pads the structure so ldd and std instructions can be used. */
  uint32_t    pad0;
}  CPU_Minimum_stack_frame;

#endif /* ASM */

/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STACK_FRAME_L0_OFFSET             0x00
/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STACK_FRAME_L1_OFFSET             0x04
/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STACK_FRAME_L2_OFFSET             0x08
/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STACK_FRAME_L3_OFFSET             0x0c
/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STACK_FRAME_L4_OFFSET             0x10
/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STACK_FRAME_L5_OFFSET             0x14
/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STACK_FRAME_L6_OFFSET             0x18
/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STACK_FRAME_L7_OFFSET             0x1c
/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STACK_FRAME_I0_OFFSET             0x20
/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STACK_FRAME_I1_OFFSET             0x24
/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STACK_FRAME_I2_OFFSET             0x28
/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STACK_FRAME_I3_OFFSET             0x2c
/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STACK_FRAME_I4_OFFSET             0x30
/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STACK_FRAME_I5_OFFSET             0x34
/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STACK_FRAME_I6_FP_OFFSET          0x38
/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STACK_FRAME_I7_OFFSET             0x3c
/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STRUCTURE_RETURN_ADDRESS_OFFSET   0x40
/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STACK_FRAME_SAVED_ARG0_OFFSET     0x44
/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STACK_FRAME_SAVED_ARG1_OFFSET     0x48
/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STACK_FRAME_SAVED_ARG2_OFFSET     0x4c
/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STACK_FRAME_SAVED_ARG3_OFFSET     0x50
/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STACK_FRAME_SAVED_ARG4_OFFSET     0x54
/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STACK_FRAME_SAVED_ARG5_OFFSET     0x58
/** This macro defines an offset into the stack frame for use in assembly. */
#define CPU_STACK_FRAME_PAD0_OFFSET           0x5c

/** This defines the size of the minimum stack frame. */
#define CPU_MINIMUM_STACK_FRAME_SIZE          0x60

/**
 * @defgroup Contexts SPARC Context Structures
 *
 *  Generally there are 2 types of context to save.
 *     + Interrupt registers to save
 *     + Task level registers to save
 *
 *  This means we have the following 3 context items:
 *     + task level context stuff::  Context_Control
 *     + floating point task stuff:: Context_Control_fp
 *     + special interrupt level context :: Context_Control_interrupt
 *
 *  On the SPARC, we are relatively conservative in that we save most
 *  of the CPU state in the context area.  The ET (enable trap) bit and
 *  the CWP (current window pointer) fields of the PSR are considered
 *  system wide resources and are not maintained on a per-thread basis.
 */

#ifndef ASM
/**
 *  @brief SPARC Basic Context
 *
 *  @ingroup Contexts
 *
 *  This structure defines the basic integer and processor state context
 *  for the SPARC architecture.
 */
typedef struct {
  /**
   *  Using a double g0_g1 will put everything in this structure on a
   *  double word boundary which allows us to use double word loads
   *  and stores safely in the context switch.
   */
  double     g0_g1;
  /** This will contain the contents of the g2 register. */
  uint32_t   g2;
  /** This will contain the contents of the g3 register. */
  uint32_t   g3;
  /** This will contain the contents of the g4 register. */
  uint32_t   g4;
  /** This will contain the contents of the g5 register. */
  uint32_t   g5;
  /** This will contain the contents of the g6 register. */
  uint32_t   g6;
  /** This will contain the contents of the g7 register. */
  uint32_t   g7;

  /** This will contain the contents of the l0 register. */
  uint32_t   l0;
  /** This will contain the contents of the l1 register. */
  uint32_t   l1;
  /** This will contain the contents of the l2 register. */
  uint32_t   l2;
  /** This will contain the contents of the l3 register. */
  uint32_t   l3;
  /** This will contain the contents of the l4 register. */
  uint32_t   l4;
  /** This will contain the contents of the l5 registeer.*/
  uint32_t   l5;
  /** This will contain the contents of the l6 register. */
  uint32_t   l6;
  /** This will contain the contents of the l7 register. */
  uint32_t   l7;

  /** This will contain the contents of the i0 register. */
  uint32_t   i0;
  /** This will contain the contents of the i1 register. */
  uint32_t   i1;
  /** This will contain the contents of the i2 register. */
  uint32_t   i2;
  /** This will contain the contents of the i3 register. */
  uint32_t   i3;
  /** This will contain the contents of the i4 register. */
  uint32_t   i4;
  /** This will contain the contents of the i5 register. */
  uint32_t   i5;
  /** This will contain the contents of the i6 (e.g. frame pointer) register. */
  uint32_t   i6_fp;
  /** This will contain the contents of the i7 register. */
  uint32_t   i7;

  /** This will contain the contents of the o0 register. */
  uint32_t   o0;
  /** This will contain the contents of the o1 register. */
  uint32_t   o1;
  /** This will contain the contents of the o2 register. */
  uint32_t   o2;
  /** This will contain the contents of the o3 register. */
  uint32_t   o3;
  /** This will contain the contents of the o4 register. */
  uint32_t   o4;
  /** This will contain the contents of the o5 register. */
  uint32_t   o5;
  /** This will contain the contents of the o6 (e.g. frame pointer) register. */
  uint32_t   o6_sp;
  /** This will contain the contents of the o7 register. */
  uint32_t   o7;

  /** This will contain the contents of the processor status register. */
  uint32_t   psr;
  /**
   * This field is used to prevent heavy nesting of calls to _Thread_Dispatch
   * on an interrupted  task's stack.  This is problematic on the slower
   * SPARC CPU models at high interrupt rates.
   */
  uint32_t   isr_dispatch_disable;
} Context_Control;

/**
 *  This macro provides a CPU independent way for RTEMS to access the
 *  stack pointer in a context structure. The actual name and offset is
 *  CPU architecture dependent.
 */
#define _CPU_Context_Get_SP( _context ) \
  (_context)->o6_sp

#endif /* ASM */

/*
 *  Offsets of fields with Context_Control for assembly routines.
 */

/** This macro defines an offset into the context for use in assembly. */
#define G0_OFFSET    0x00
/** This macro defines an offset into the context for use in assembly. */
#define G1_OFFSET    0x04
/** This macro defines an offset into the context for use in assembly. */
#define G2_OFFSET    0x08
/** This macro defines an offset into the context for use in assembly. */
#define G3_OFFSET    0x0C
/** This macro defines an offset into the context for use in assembly. */
#define G4_OFFSET    0x10
/** This macro defines an offset into the context for use in assembly. */
#define G5_OFFSET    0x14
/** This macro defines an offset into the context for use in assembly. */
#define G6_OFFSET    0x18
/** This macro defines an offset into the context for use in assembly. */
#define G7_OFFSET    0x1C

/** This macro defines an offset into the context for use in assembly. */
#define L0_OFFSET    0x20
/** This macro defines an offset into the context for use in assembly. */
#define L1_OFFSET    0x24
/** This macro defines an offset into the context for use in assembly. */
#define L2_OFFSET    0x28
/** This macro defines an offset into the context for use in assembly. */
#define L3_OFFSET    0x2C
/** This macro defines an offset into the context for use in assembly. */
#define L4_OFFSET    0x30
/** This macro defines an offset into the context for use in assembly. */
#define L5_OFFSET    0x34
/** This macro defines an offset into the context for use in assembly. */
#define L6_OFFSET    0x38
/** This macro defines an offset into the context for use in assembly. */
#define L7_OFFSET    0x3C

/** This macro defines an offset into the context for use in assembly. */
#define I0_OFFSET    0x40
/** This macro defines an offset into the context for use in assembly. */
#define I1_OFFSET    0x44
/** This macro defines an offset into the context for use in assembly. */
#define I2_OFFSET    0x48
/** This macro defines an offset into the context for use in assembly. */
#define I3_OFFSET    0x4C
/** This macro defines an offset into the context for use in assembly. */
#define I4_OFFSET    0x50
/** This macro defines an offset into the context for use in assembly. */
#define I5_OFFSET    0x54
/** This macro defines an offset into the context for use in assembly. */
#define I6_FP_OFFSET 0x58
/** This macro defines an offset into the context for use in assembly. */
#define I7_OFFSET    0x5C

/** This macro defines an offset into the context for use in assembly. */
#define O0_OFFSET    0x60
/** This macro defines an offset into the context for use in assembly. */
#define O1_OFFSET    0x64
/** This macro defines an offset into the context for use in assembly. */
#define O2_OFFSET    0x68
/** This macro defines an offset into the context for use in assembly. */
#define O3_OFFSET    0x6C
/** This macro defines an offset into the context for use in assembly. */
#define O4_OFFSET    0x70
/** This macro defines an offset into the context for use in assembly. */
#define O5_OFFSET    0x74
/** This macro defines an offset into the context for use in assembly. */
#define O6_SP_OFFSET 0x78
/** This macro defines an offset into the context for use in assembly. */
#define O7_OFFSET    0x7C

/** This macro defines an offset into the context for use in assembly. */
#define PSR_OFFSET   0x80
/** This macro defines an offset into the context for use in assembly. */
#define ISR_DISPATCH_DISABLE_STACK_OFFSET 0x84

/** This defines the size of the context area for use in assembly. */
#define CONTEXT_CONTROL_SIZE 0x88

#ifndef ASM
/**
 *  @brief SPARC Basic Context
 *
 *  @ingroup Contexts
 *
 *  This structure defines floating point context area.
 */
typedef struct {
  /** This will contain the contents of the f0 and f1 register. */
  double      f0_f1;
  /** This will contain the contents of the f2 and f3 register. */
  double      f2_f3;
  /** This will contain the contents of the f4 and f5 register. */
  double      f4_f5;
  /** This will contain the contents of the f6 and f7 register. */
  double      f6_f7;
  /** This will contain the contents of the f8 and f9 register. */
  double      f8_f9;
  /** This will contain the contents of the f10 and f11 register. */
  double      f10_f11;
  /** This will contain the contents of the f12 and f13 register. */
  double      f12_f13;
  /** This will contain the contents of the f14 and f15 register. */
  double      f14_f15;
  /** This will contain the contents of the f16 and f17 register. */
  double      f16_f17;
  /** This will contain the contents of the f18 and f19 register. */
  double      f18_f19;
  /** This will contain the contents of the f20 and f21 register. */
  double      f20_f21;
  /** This will contain the contents of the f22 and f23 register. */
  double      f22_f23;
  /** This will contain the contents of the f24 and f25 register. */
  double      f24_f25;
  /** This will contain the contents of the f26 and f27 register. */
  double      f26_f27;
  /** This will contain the contents of the f28 and f29 register. */
  double      f28_f29;
  /** This will contain the contents of the f30 and f31 register. */
  double      f30_f31;
  /** This will contain the contents of the floating point status register. */
  uint32_t    fsr;
} Context_Control_fp;

#endif /* ASM */

/*
 *  Offsets of fields with Context_Control_fp for assembly routines.
 */

/** This macro defines an offset into the FPU context for use in assembly. */
#define FO_F1_OFFSET     0x00
/** This macro defines an offset into the FPU context for use in assembly. */
#define F2_F3_OFFSET     0x08
/** This macro defines an offset into the FPU context for use in assembly. */
#define F4_F5_OFFSET     0x10
/** This macro defines an offset into the FPU context for use in assembly. */
#define F6_F7_OFFSET     0x18
/** This macro defines an offset into the FPU context for use in assembly. */
#define F8_F9_OFFSET     0x20
/** This macro defines an offset into the FPU context for use in assembly. */
#define F1O_F11_OFFSET   0x28
/** This macro defines an offset into the FPU context for use in assembly. */
#define F12_F13_OFFSET   0x30
/** This macro defines an offset into the FPU context for use in assembly. */
#define F14_F15_OFFSET   0x38
/** This macro defines an offset into the FPU context for use in assembly. */
#define F16_F17_OFFSET   0x40
/** This macro defines an offset into the FPU context for use in assembly. */
#define F18_F19_OFFSET   0x48
/** This macro defines an offset into the FPU context for use in assembly. */
#define F2O_F21_OFFSET   0x50
/** This macro defines an offset into the FPU context for use in assembly. */
#define F22_F23_OFFSET   0x58
/** This macro defines an offset into the FPU context for use in assembly. */
#define F24_F25_OFFSET   0x60
/** This macro defines an offset into the FPU context for use in assembly. */
#define F26_F27_OFFSET   0x68
/** This macro defines an offset into the FPU context for use in assembly. */
#define F28_F29_OFFSET   0x70
/** This macro defines an offset into the FPU context for use in assembly. */
#define F3O_F31_OFFSET   0x78
/** This macro defines an offset into the FPU context for use in assembly. */
#define FSR_OFFSET       0x80

/** This defines the size of the FPU context area for use in assembly. */
#define CONTEXT_CONTROL_FP_SIZE 0x84

#ifndef ASM

/**
 *  @brief Interrupt Stack Frame (ISF)
 *
 *  Context saved on stack for an interrupt.
 *
 *  @note The PSR, PC, and NPC are only saved in this structure for the
 *        benefit of the user's handler.
 */
typedef struct {
  /** On an interrupt, we must save the minimum stack frame. */
  CPU_Minimum_stack_frame  Stack_frame;
  /** This is the offset of the PSR on an ISF. */
  uint32_t                 psr;
  /** This is the offset of the XXX on an ISF. */
  uint32_t                 pc;
  /** This is the offset of the XXX on an ISF. */
  uint32_t                 npc;
  /** This is the offset of the g1 register on an ISF. */
  uint32_t                 g1;
  /** This is the offset of the g2 register on an ISF. */
  uint32_t                 g2;
  /** This is the offset of the g3 register on an ISF. */
  uint32_t                 g3;
  /** This is the offset of the g4 register on an ISF. */
  uint32_t                 g4;
  /** This is the offset of the g5 register on an ISF. */
  uint32_t                 g5;
  /** This is the offset of the g6 register on an ISF. */
  uint32_t                 g6;
  /** This is the offset of the g7 register on an ISF. */
  uint32_t                 g7;
  /** This is the offset of the i0 register on an ISF. */
  uint32_t                 i0;
  /** This is the offset of the i1 register on an ISF. */
  uint32_t                 i1;
  /** This is the offset of the i2 register on an ISF. */
  uint32_t                 i2;
  /** This is the offset of the i3 register on an ISF. */
  uint32_t                 i3;
  /** This is the offset of the i4 register on an ISF. */
  uint32_t                 i4;
  /** This is the offset of the i5 register on an ISF. */
  uint32_t                 i5;
  /** This is the offset of the i6 register on an ISF. */
  uint32_t                 i6_fp;
  /** This is the offset of the i7 register on an ISF. */
  uint32_t                 i7;
  /** This is the offset of the y register on an ISF. */
  uint32_t                 y;
  /** This is the offset of the tpc register on an ISF. */
  uint32_t                 tpc;
} CPU_Interrupt_frame;

#endif /* ASM */

/*
 *  Offsets of fields with CPU_Interrupt_frame for assembly routines.
 */

/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_STACK_FRAME_OFFSET 0x00
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_PSR_OFFSET         CPU_MINIMUM_STACK_FRAME_SIZE + 0x00
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_PC_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x04
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_NPC_OFFSET         CPU_MINIMUM_STACK_FRAME_SIZE + 0x08
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_G1_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x0c
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_G2_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x10
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_G3_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x14
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_G4_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x18
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_G5_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x1c
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_G6_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x20
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_G7_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x24
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I0_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x28
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I1_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x2c
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I2_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x30
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I3_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x34
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I4_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x38
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I5_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x3c
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I6_FP_OFFSET       CPU_MINIMUM_STACK_FRAME_SIZE + 0x40
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_I7_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x44
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_Y_OFFSET           CPU_MINIMUM_STACK_FRAME_SIZE + 0x48
/** This macro defines an offset into the ISF for use in assembly. */
#define ISF_TPC_OFFSET         CPU_MINIMUM_STACK_FRAME_SIZE + 0x4c

/** This defines the size of the ISF area for use in assembly. */
#define CONTEXT_CONTROL_INTERRUPT_FRAME_SIZE \
        CPU_MINIMUM_STACK_FRAME_SIZE + 0x50

#ifndef ASM
/**
 *  This variable is contains the initialize context for the FP unit.
 *  It is filled in by _CPU_Initialize and copied into the task's FP
 *  context area during _CPU_Context_Initialize.
 */
SCORE_EXTERN Context_Control_fp  _CPU_Null_fp_context CPU_STRUCTURE_ALIGNMENT;

/**
 *  This flag is context switched with each thread.  It indicates
 *  that THIS thread has an _ISR_Dispatch stack frame on its stack.
 *  By using this flag, we can avoid nesting more interrupt dispatching
 *  attempts on a previously interrupted thread's stack.
 */
SCORE_EXTERN volatile uint32_t _CPU_ISR_Dispatch_disable;

/**
 *  The following type defines an entry in the SPARC's trap table.
 *
 *  @note The instructions chosen are RTEMS dependent although one is
 *        obligated to use two of the four instructions to perform a
 *        long jump.  The other instructions load one register with the
 *        trap type (a.k.a. vector) and another with the psr.
 */
typedef struct {
  /** This will contain a "mov %psr, %l0" instruction. */
  uint32_t     mov_psr_l0;
  /** This will contain a "sethi %hi(_handler), %l4" instruction. */
  uint32_t     sethi_of_handler_to_l4;
  /** This will contain a "jmp %l4 + %lo(_handler)" instruction. */
  uint32_t     jmp_to_low_of_handler_plus_l4;
  /** This will contain a " mov _vector, %l3" instruction. */
  uint32_t     mov_vector_l3;
} CPU_Trap_table_entry;

/**
 *  This is the set of opcodes for the instructions loaded into a trap
 *  table entry.  The routine which installs a handler is responsible
 *  for filling in the fields for the _handler address and the _vector
 *  trap type.
 *
 *  The constants following this structure are masks for the fields which
 *  must be filled in when the handler is installed.
 */
extern const CPU_Trap_table_entry _CPU_Trap_slot_template;

/**
 *  The size of the floating point context area.
 */
#define CPU_CONTEXT_FP_SIZE sizeof( Context_Control_fp )

#endif

/**
 *  Amount of extra stack (above minimum stack size) required by
 *  MPCI receive server thread.  Remember that in a multiprocessor
 *  system this thread must exist and be able to process all directives.
 */
#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 1024

/**
 *  This defines the number of entries in the ISR_Vector_table managed
 *  by the executive.
 *
 *  On the SPARC, there are really only 256 vectors.  However, the executive
 *  has no easy, fast, reliable way to determine which traps are synchronous
 *  and which are asynchronous.  By default, synchronous traps return to the
 *  instruction which caused the interrupt.  So if you install a software
 *  trap handler as an executive interrupt handler (which is desirable since
 *  RTEMS takes care of window and register issues), then the executive needs
 *  to know that the return address is to the trap rather than the instruction
 *  following the trap.
 *
 *  So vectors 0 through 255 are treated as regular asynchronous traps which
 *  provide the "correct" return address.  Vectors 256 through 512 are assumed
 *  by the executive to be synchronous and to require that the return address
 *  be fudged.
 *
 *  If you use this mechanism to install a trap handler which must reexecute
 *  the instruction which caused the trap, then it should be installed as
 *  an asynchronous trap.  This will avoid the executive changing the return
 *  address.
 */
#define CPU_INTERRUPT_NUMBER_OF_VECTORS     256

/**
 * The SPARC has 256 vectors but the port treats 256-512 as synchronous
 * traps.
 */
#define CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER 511

/**
 *  This is the bit step in a vector number to indicate it is being installed
 *  as a synchronous trap.
 */
#define SPARC_SYNCHRONOUS_TRAP_BIT_MASK     0x100

/**
 *  This macro indicates that @a _trap as an asynchronous trap.
 */
#define SPARC_ASYNCHRONOUS_TRAP( _trap )    (_trap)

/**
 *  This macro indicates that @a _trap as a synchronous trap.
 */
#define SPARC_SYNCHRONOUS_TRAP( _trap )     ((_trap) + 256 )

/**
 * This macro returns the real hardware vector number associated with @a _trap.
 */
#define SPARC_REAL_TRAP_NUMBER( _trap )     ((_trap) % 256)

/**
 *  This is defined if the port has a special way to report the ISR nesting
 *  level.  Most ports maintain the variable _ISR_Nest_level.
 */
#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE

/**
 *  Should be large enough to run all tests.  This ensures
 *  that a "reasonable" small application should not have any problems.
 *
 *  This appears to be a fairly generous number for the SPARC since
 *  represents a call depth of about 20 routines based on the minimum
 *  stack frame.
 */
#define CPU_STACK_MINIMUM_SIZE  (1024*4)

/**
 *  CPU's worst alignment requirement for data types on a byte boundary.  This
 *  alignment does not take into account the requirements for the stack.
 *
 *  On the SPARC, this is required for double word loads and stores.
 */
#define CPU_ALIGNMENT      8

/**
 *  This number corresponds to the byte alignment requirement for the
 *  heap handler.  This alignment requirement may be stricter than that
 *  for the data types alignment specified by CPU_ALIGNMENT.  It is
 *  common for the heap to follow the same alignment requirement as
 *  CPU_ALIGNMENT.  If the CPU_ALIGNMENT is strict enough for the heap,
 *  then this should be set to CPU_ALIGNMENT.
 *
 *  @note  This does not have to be a power of 2.  It does have to
 *         be greater or equal to than CPU_ALIGNMENT.
 */
#define CPU_HEAP_ALIGNMENT         CPU_ALIGNMENT

/**
 *  This number corresponds to the byte alignment requirement for memory
 *  buffers allocated by the partition manager.  This alignment requirement
 *  may be stricter than that for the data types alignment specified by
 *  CPU_ALIGNMENT.  It is common for the partition to follow the same
 *  alignment requirement as CPU_ALIGNMENT.  If the CPU_ALIGNMENT is strict
 *  enough for the partition, then this should be set to CPU_ALIGNMENT.
 *
 *  @note  This does not have to be a power of 2.  It does have to
 *         be greater or equal to than CPU_ALIGNMENT.
 */
#define CPU_PARTITION_ALIGNMENT    CPU_ALIGNMENT

/**
 *  This number corresponds to the byte alignment requirement for the
 *  stack.  This alignment requirement may be stricter than that for the
 *  data types alignment specified by CPU_ALIGNMENT.  If the CPU_ALIGNMENT
 *  is strict enough for the stack, then this should be set to 0.
 *
 *  @note  This must be a power of 2 either 0 or greater than CPU_ALIGNMENT.
 *
 *  The alignment restrictions for the SPARC are not that strict but this
 *  should unsure that the stack is always sufficiently alignment that the
 *  window overflow, underflow, and flush routines can use double word loads
 *  and stores.
 */
#define CPU_STACK_ALIGNMENT        16

#ifndef ASM

/*
 *  ISR handler macros
 */

/**
 *  Support routine to initialize the RTEMS vector table after it is allocated.
 */
#define _CPU_Initialize_vectors()

/**
 *  Disable all interrupts for a critical section.  The previous
 *  level is returned in _level.
 */
#define _CPU_ISR_Disable( _level ) \
  (_level) = sparc_disable_interrupts()

/**
 *  Enable interrupts to the previous level (returned by _CPU_ISR_Disable).
 *  This indicates the end of a critical section.  The parameter
 *  _level is not modified.
 */
#define _CPU_ISR_Enable( _level ) \
  sparc_enable_interrupts( _level )

/**
 *  This temporarily restores the interrupt to _level before immediately
 *  disabling them again.  This is used to divide long critical
 *  sections into two or more parts.  The parameter _level is not
 *  modified.
 */
#define _CPU_ISR_Flash( _level ) \
  sparc_flash_interrupts( _level )

/**
 *  Map interrupt level in task mode onto the hardware that the CPU
 *  actually provides.  Currently, interrupt levels which do not
 *  map onto the CPU in a straight fashion are undefined.
 */
#define _CPU_ISR_Set_level( _newlevel ) \
   sparc_enable_interrupts( _newlevel << 8)

/**
 *  @brief Obtain the Current Interrupt Disable Level
 *
 *  This method is invoked to return the current interrupt disable level.
 *
 *  @return This method returns the current interrupt disable level.
 */ 
uint32_t   _CPU_ISR_Get_level( void );

/* end of ISR handler macros */

/* Context handler macros */

/**
 *  Initialize the context to a state suitable for starting a
 *  task after a context restore operation.  Generally, this
 *  involves:
 *
 *  - setting a starting address
 *  - preparing the stack
 *  - preparing the stack and frame pointers
 *  - setting the proper interrupt level in the context
 *  - initializing the floating point context
 *
 * @param[in] the_context points to the context area
 * @param[in] stack_base is the low address of the allocated stack area
 * @param[in] size is the size of the stack area in bytes
 * @param[in] new_level is the interrupt level for the task
 * @param[in] entry_point is the task's entry point
 * @param[in] is_fp is set to TRUE if the task is a floating point task
 *
 *  @note  Implemented as a subroutine for the SPARC port.
 */
void _CPU_Context_Initialize(
  Context_Control  *the_context,
  uint32_t         *stack_base,
  uint32_t          size,
  uint32_t          new_level,
  void             *entry_point,
  bool              is_fp
);

/**
 *  This macro is invoked from _Thread_Handler to do whatever CPU
 *  specific magic is required that must be done in the context of
 *  the thread when it starts.
 *
 *  On the SPARC, this is setting the frame pointer so GDB is happy.
 *  Make GDB stop unwinding at _Thread_Handler, previous register window
 *  Frame pointer is 0 and calling address must be a function with starting
 *  with a SAVE instruction. If return address is leaf-function (no SAVE)
 *  GDB will not look at prev reg window fp.
 *
 *  _Thread_Handler is known to start with SAVE.
 */
#define _CPU_Context_Initialization_at_thread_begin() \
  do { \
    __asm__ volatile ("set _Thread_Handler,%%i7\n"::); \
  } while (0)

/**
 *  This routine is responsible for somehow restarting the currently
 *  executing task.
 *
 *  On the SPARC, this is is relatively painless but requires a small
 *  amount of wrapper code before using the regular restore code in
 *  of the context switch.
 */
#define _CPU_Context_Restart_self( _the_context ) \
   _CPU_Context_restore( (_the_context) );

/**
 *  The FP context area for the SPARC is a simple structure and nothing
 *  special is required to find the "starting load point"
 */
#define _CPU_Context_Fp_start( _base, _offset ) \
   ( (void *) _Addresses_Add_offset( (_base), (_offset) ) )

/**
 *  This routine initializes the FP context area passed to it to.
 *
 *  The SPARC allows us to use the simple initialization model
 *  in which an "initial" FP context was saved into _CPU_Null_fp_context
 *  at CPU initialization and it is simply copied into the destination
 *  context.
 */
#define _CPU_Context_Initialize_fp( _destination ) \
  do { \
   *(*(_destination)) = _CPU_Null_fp_context; \
  } while (0)

/* end of Context handler macros */

/* Fatal Error manager macros */

/**
 *  This routine copies _error into a known place -- typically a stack
 *  location or a register, optionally disables interrupts, and
 *  halts/stops the CPU.
 */
#define _CPU_Fatal_halt( _error ) \
  do { \
    uint32_t   level; \
    \
    level = sparc_disable_interrupts(); \
    __asm__ volatile ( "mov  %0, %%g1 " : "=r" (level) : "0" (level) ); \
    while (1); /* loop forever */ \
  } while (0)

/* end of Fatal Error manager macros */

/* Bitfield handler macros */

#if ( SPARC_HAS_BITSCAN == 0 )
  /**
   *  The SPARC port uses the generic C algorithm for bitfield scan if the
   *  CPU model does not have a scan instruction.
   */
  #define CPU_USE_GENERIC_BITFIELD_CODE TRUE
  /**
   *  The SPARC port uses the generic C algorithm for bitfield scan if the
   *  CPU model does not have a scan instruction.  Thus is needs the generic
   *  data table used by that algorithm.
   */
  #define CPU_USE_GENERIC_BITFIELD_DATA TRUE
#else
  #error "scan instruction not currently supported by RTEMS!!"
#endif

/* end of Bitfield handler macros */

/* functions */

/**
 *  @brief SPARC Specific Initialization
 *
 *  This routine performs CPU dependent initialization.
 */
void _CPU_Initialize(void);

/**
 *  @brief SPARC Specific Raw ISR Installer
 *
 *  This routine installs @a new_handler to be directly called from the trap
 *  table.
 *
 *  @param[in] vector is the vector number
 *  @param[in] new_handler is the new ISR handler
 *  @param[in] old_handler will contain the old ISR handler
 */
void _CPU_ISR_install_raw_handler(
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
);

/**
 *  @brief SPARC Specific RTEMS ISR Installer
 *
 *  This routine installs an interrupt vector.
 *
 *  @param[in] vector is the vector number
 *  @param[in] new_handler is the new ISR handler
 *  @param[in] old_handler will contain the old ISR handler
 */

void _CPU_ISR_install_vector(
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
);

/**
 *  @brief SPARC Specific Context Switch
 *
 *  This routine switches from the run context to the heir context.
 *
 *  @param[in] run is the currently executing thread
 *  @param[in] heir will become the currently executing thread
 */
void _CPU_Context_switch(
  Context_Control  *run,
  Context_Control  *heir
);

/**
 *  @brief SPARC Specific Context Restore
 *
 *  This routine is generally used only to restart self in an
 *  efficient manner.
 *
 *  @param[in] new_context is the context to restore
 */
void _CPU_Context_restore(
  Context_Control *new_context
) RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

#if defined(RTEMS_SMP)
  /**
   *  @brief SPARC Specific Method to Switch to First Task 
   *
   *  This routine is only used to switch to the first task on a
   *  secondary core in an SMP configuration.  We do not need to
   *  flush all the windows and, in fact, this can be dangerous
   *  as they may or may not be initialized properly.
   *
   *  @param[in] new_context is the context to restore
   */
  void _CPU_Context_switch_to_first_task_smp(
    Context_Control *new_context
  );

  /**
   * Macro to access memory and bypass the cache.
   *
   * @note address space 1 is uncacheable
   */
  #define SMP_CPU_SWAP( _address, _value, _previous ) \
    do { \
      register unsigned int _val = _value; \
      asm volatile( \
        "swapa [%2] %3, %0" : \
        "=r" (_val) : \
        "0" (_val), \
        "r" (_address), \
        "i" (1) \
      ); \
      _previous = _val; \
    } while (0)
#endif

/**
 *  @brief SPARC Specific Save FPU Method
 *
 *  This routine saves the floating point context passed to it.
 *
 *  @param[in] fp_context_ptr is the area to save into
 */
void _CPU_Context_save_fp(
  Context_Control_fp **fp_context_ptr
);

/**
 *  @brief SPARC Specific Rstore FPU Method
 *
 *  This routine restores the floating point context passed to it.
 *
 *  @param[in] fp_context_ptr is the area to restore from
 */
void _CPU_Context_restore_fp(
  Context_Control_fp **fp_context_ptr
);

/**
 *  @brief SPARC Specific Method to Endian Swap an uint32_t 
 *
 *  The following routine swaps the endian format of an unsigned int.
 *  It must be static because it is referenced indirectly.
 *
 *  @param[in] value is the value to endian swap
 *
 *  This version will work on any processor, but if you come across a better
 *  way for the SPARC PLEASE use it.  The most common way to swap a 32-bit
 *  entity as shown below is not any more efficient on the SPARC.
 *
 *     - swap least significant two bytes with 16-bit rotate
 *     - swap upper and lower 16-bits
 *     - swap most significant two bytes with 16-bit rotate
 *
 *  It is not obvious how the SPARC can do significantly better than the
 *  generic code.  gcc 2.7.0 only generates about 12 instructions for the
 *  following code at optimization level four (i.e. -O4).
 */
static inline uint32_t CPU_swap_u32(
  uint32_t value
)
{
  uint32_t   byte1, byte2, byte3, byte4, swapped;

  byte4 = (value >> 24) & 0xff;
  byte3 = (value >> 16) & 0xff;
  byte2 = (value >> 8)  & 0xff;
  byte1 =  value        & 0xff;

  swapped = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
  return( swapped );
}

/**
 *  @brief SPARC Specific Method to Endian Swap an uint16_t 
 *
 *  The following routine swaps the endian format of a uint16_t.
 *
 *  @param[in] value is the value to endian swap
 */
#define CPU_swap_u16( value ) \
  (((value&0xff) << 8) | ((value >> 8)&0xff))

#endif /* ASM */

#ifdef __cplusplus
}
#endif

#endif
