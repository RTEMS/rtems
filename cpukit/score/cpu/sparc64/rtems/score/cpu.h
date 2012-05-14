/**
 * @file rtems/score/cpu.h
 */

/*
 *  This include file contains information pertaining to the port of 
 *  the executive to the SPARC64 processor.
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  This file is based on the SPARC cpu.h file. Modifications are made 
 *  to support the SPARC64 processor.
 *    COPYRIGHT (c) 2010. Gedare Bloom.
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
#include <rtems/score/sparc64.h>

/* conditional compilation parameters */

/*
 *  Should the calls to _Thread_Enable_dispatch be inlined?
 *
 *  If TRUE, then they are inlined.
 *  If FALSE, then a subroutine call is made.
 */

#define CPU_INLINE_ENABLE_DISPATCH       TRUE

/*
 *  Should the body of the search loops in _Thread_queue_Enqueue_priority
 *  be unrolled one time?  In unrolled each iteration of the loop examines
 *  two "nodes" on the chain being searched.  Otherwise, only one node
 *  is examined per iteration.
 *
 *  If TRUE, then the loops are unrolled.
 *  If FALSE, then the loops are not unrolled.
 *
 *  This parameter could go either way on the SPARC.  The interrupt flash
 *  code is relatively lengthy given the requirements for nops following
 *  writes to the psr.  But if the clock speed were high enough, this would
 *  not represent a great deal of time.
 */

#define CPU_UNROLL_ENQUEUE_PRIORITY      TRUE

/*
 *  Does the executive manage a dedicated interrupt stack in software?
 *
 *  If TRUE, then a stack is allocated in _ISR_Handler_initialization.
 *  If FALSE, nothing is done.
 *
 *  The SPARC does not have a dedicated HW interrupt stack and one has
 *  been implemented in SW.
 */

#define CPU_HAS_SOFTWARE_INTERRUPT_STACK   TRUE

/*
 *  Does the CPU follow the simple vectored interrupt model?
 *
 *  If TRUE, then RTEMS allocates the vector table it internally manages.
 *  If FALSE, then the BSP is assumed to allocate and manage the vector
 *  table
 *
 *  SPARC Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */
#define CPU_SIMPLE_VECTORED_INTERRUPTS TRUE

/*
 *  Does this CPU have hardware support for a dedicated interrupt stack?
 *
 *  If TRUE, then it must be installed during initialization.
 *  If FALSE, then no installation is performed.
 *
 *  The SPARC does not have a dedicated HW interrupt stack.
 */

#define CPU_HAS_HARDWARE_INTERRUPT_STACK  FALSE

/*
 *  Do we allocate a dedicated interrupt stack in the Interrupt Manager?
 *
 *  If TRUE, then the memory is allocated during initialization.
 *  If FALSE, then the memory is allocated during initialization.
 */

#define CPU_ALLOCATE_INTERRUPT_STACK      TRUE

/*
 *  Does the RTEMS invoke the user's ISR with the vector number and
 *  a pointer to the saved interrupt frame (1) or just the vector 
 *  number (0)?
 */

#define CPU_ISR_PASSES_FRAME_POINTER 0

/*
 *  Does the CPU have hardware floating point?
 *
 *  If TRUE, then the FLOATING_POINT task attribute is supported.
 *  If FALSE, then the FLOATING_POINT task attribute is ignored.
 */

#if ( SPARC_HAS_FPU == 1 )
#define CPU_HARDWARE_FP     TRUE
#else
#define CPU_HARDWARE_FP     FALSE
#endif
#define CPU_SOFTWARE_FP     FALSE

/*
 *  Are all tasks FLOATING_POINT tasks implicitly?
 *
 *  If TRUE, then the FLOATING_POINT task attribute is assumed.
 *  If FALSE, then the FLOATING_POINT task attribute is followed.
 */

#define CPU_ALL_TASKS_ARE_FP     FALSE

/*
 *  Should the IDLE task have a floating point context?
 *
 *  If TRUE, then the IDLE task is created as a FLOATING_POINT task
 *  and it has a floating point context which is switched in and out.
 *  If FALSE, then the IDLE task does not have a floating point context.
 */

#define CPU_IDLE_TASK_IS_FP      FALSE

/*
 *  Should the saving of the floating point registers be deferred
 *  until a context switch is made to another different floating point
 *  task?
 *
 *  If TRUE, then the floating point context will not be stored until
 *  necessary.  It will remain in the floating point registers and not
 *  disturned until another floating point task is switched to.
 *
 *  If FALSE, then the floating point context is saved when a floating
 *  point task is switched out and restored when the next floating point
 *  task is restored.  The state of the floating point registers between
 *  those two operations is not specified.
 */

#define CPU_USE_DEFERRED_FP_SWITCH       TRUE

/*
 *  Does this port provide a CPU dependent IDLE task implementation?
 *
 *  If TRUE, then the routine _CPU_Thread_Idle_body
 *  must be provided and is the default IDLE thread body instead of
 *  _CPU_Thread_Idle_body.
 *
 *  If FALSE, then use the generic IDLE thread body if the BSP does
 *  not provide one.
 */

#define CPU_PROVIDES_IDLE_THREAD_BODY    FALSE

/*
 *  Does the stack grow up (toward higher addresses) or down
 *  (toward lower addresses)?
 *
 *  If TRUE, then the grows upward.
 *  If FALSE, then the grows toward smaller addresses.
 *
 *  The stack grows to lower addresses on the SPARC.
 */

#define CPU_STACK_GROWS_UP               FALSE

/*
 *  The following is the variable attribute used to force alignment
 *  of critical data structures.  On some processors it may make
 *  sense to have these aligned on tighter boundaries than
 *  the minimum requirements of the compiler in order to have as
 *  much of the critical data area as possible in a cache line.
 *
 *  The SPARC does not appear to have particularly strict alignment 
 *  requirements.  This value (16) was chosen to take advantages of caches.
 *
 *  SPARC 64 requirements on floating point alignment is at least 8, 
 *  and is 16 if quad-word fp instructions are available (e.g. LDQF).
 */

#define CPU_STRUCTURE_ALIGNMENT          __attribute__ ((aligned (16)))

#define CPU_TIMESTAMP_USE_INT64_INLINE TRUE

/*
 *  Define what is required to specify how the network to host conversion
 *  routines are handled.
 */

#define CPU_BIG_ENDIAN                           TRUE
#define CPU_LITTLE_ENDIAN                        FALSE

/*
 *  The following defines the number of bits actually used in the
 *  interrupt field of the task mode.  How those bits map to the
 *  CPU interrupt levels is defined by the routine _CPU_ISR_Set_level().
 *
 *  The SPARC v9 has 16 interrupt levels in the PIL field of the PSR.
 */

#define CPU_MODES_INTERRUPT_MASK   0x0000000F

/*
 *  This structure represents the organization of the minimum stack frame 
 *  for the SPARC.  More framing information is required in certain situaions
 *  such as when there are a large number of out parameters or when the callee
 *  must save floating point registers.
 */

#ifndef ASM

typedef struct {
  uint64_t    l0;
  uint64_t    l1;
  uint64_t    l2;
  uint64_t    l3;
  uint64_t    l4;
  uint64_t    l5;
  uint64_t    l6;
  uint64_t    l7;
  uint64_t    i0;
  uint64_t    i1;
  uint64_t    i2;
  uint64_t    i3;
  uint64_t    i4;
  uint64_t    i5;
  uint64_t    i6_fp;
  uint64_t    i7;
  void       *structure_return_address;
  /*
   *  The following are for the callee to save the register arguments in
   *  should this be necessary.
   */
  uint64_t    saved_arg0;
  uint64_t    saved_arg1;
  uint64_t    saved_arg2;
  uint64_t    saved_arg3;
  uint64_t    saved_arg4;
  uint64_t    saved_arg5;
  uint64_t    pad0;
}  CPU_Minimum_stack_frame;

#endif /* !ASM */

#define CPU_STACK_FRAME_L0_OFFSET             0x00
#define CPU_STACK_FRAME_L1_OFFSET             0x08
#define CPU_STACK_FRAME_L2_OFFSET             0x10
#define CPU_STACK_FRAME_L3_OFFSET             0x18
#define CPU_STACK_FRAME_L4_OFFSET             0x20
#define CPU_STACK_FRAME_L5_OFFSET             0x28
#define CPU_STACK_FRAME_L6_OFFSET             0x30
#define CPU_STACK_FRAME_L7_OFFSET             0x38
#define CPU_STACK_FRAME_I0_OFFSET             0x40
#define CPU_STACK_FRAME_I1_OFFSET             0x48
#define CPU_STACK_FRAME_I2_OFFSET             0x50
#define CPU_STACK_FRAME_I3_OFFSET             0x58
#define CPU_STACK_FRAME_I4_OFFSET             0x60
#define CPU_STACK_FRAME_I5_OFFSET             0x68
#define CPU_STACK_FRAME_I6_FP_OFFSET          0x70
#define CPU_STACK_FRAME_I7_OFFSET             0x78
#define CPU_STRUCTURE_RETURN_ADDRESS_OFFSET   0x80
#define CPU_STACK_FRAME_SAVED_ARG0_OFFSET     0x88
#define CPU_STACK_FRAME_SAVED_ARG1_OFFSET     0x90
#define CPU_STACK_FRAME_SAVED_ARG2_OFFSET     0x98
#define CPU_STACK_FRAME_SAVED_ARG3_OFFSET     0xA0
#define CPU_STACK_FRAME_SAVED_ARG4_OFFSET     0xA8
#define CPU_STACK_FRAME_SAVED_ARG5_OFFSET     0xB0
#define CPU_STACK_FRAME_PAD0_OFFSET           0xB8

#define CPU_MINIMUM_STACK_FRAME_SIZE          0xC0

/*
 * Contexts
 *
 *  Generally there are 2 types of context to save.
 *     1. Interrupt registers to save
 *     2. Task level registers to save
 *
 *  This means we have the following 3 context items:
 *     1. task level context stuff::  Context_Control
 *     2. floating point task stuff:: Context_Control_fp
 *     3. special interrupt level context :: Context_Control_interrupt
 *
 *  On the SPARC, we are relatively conservative in that we save most
 *  of the CPU state in the context area.  The ET (enable trap) bit and
 *  the CWP (current window pointer) fields of the PSR are considered
 *  system wide resources and are not maintained on a per-thread basis.
 */

#ifndef ASM

typedef struct {
    uint64_t   g1;
    uint64_t   g2;
    uint64_t   g3;
    uint64_t   g4;
    uint64_t   g5;
    uint64_t   g6;
    uint64_t   g7;

    uint64_t   l0;
    uint64_t   l1;
    uint64_t   l2;
    uint64_t   l3;
    uint64_t   l4;
    uint64_t   l5;
    uint64_t   l6;
    uint64_t   l7;

    uint64_t   i0;
    uint64_t   i1;
    uint64_t   i2;
    uint64_t   i3;
    uint64_t   i4;
    uint64_t   i5;
    uint64_t   i6_fp;
    uint64_t   i7;

    uint64_t   o0;
    uint64_t   o1;
    uint64_t   o2;
    uint64_t   o3;
    uint64_t   o4;
    uint64_t   o5;
    uint64_t   o6_sp;
    uint64_t   o7;

    uint32_t   isr_dispatch_disable;
    uint32_t   pad;
} Context_Control;

#define _CPU_Context_Get_SP( _context ) \
  (_context)->o6_sp

#endif /* ASM */

/*
 *  Offsets of fields with Context_Control for assembly routines.
 */

#define G1_OFFSET    0x00
#define G2_OFFSET    0x08
#define G3_OFFSET    0x10
#define G4_OFFSET    0x18
#define G5_OFFSET    0x20
#define G6_OFFSET    0x28
#define G7_OFFSET    0x30

#define L0_OFFSET    0x38
#define L1_OFFSET    0x40
#define L2_OFFSET    0x48
#define L3_OFFSET    0x50
#define L4_OFFSET    0x58
#define L5_OFFSET    0x60
#define L6_OFFSET    0x68
#define L7_OFFSET    0x70

#define I0_OFFSET    0x78
#define I1_OFFSET    0x80
#define I2_OFFSET    0x88
#define I3_OFFSET    0x90
#define I4_OFFSET    0x98
#define I5_OFFSET    0xA0
#define I6_FP_OFFSET    0xA8
#define I7_OFFSET 0xB0

#define O0_OFFSET    0xB8
#define O1_OFFSET    0xC0
#define O2_OFFSET    0xC8
#define O3_OFFSET    0xD0
#define O4_OFFSET    0xD8
#define O5_OFFSET    0xE0
#define O6_SP_OFFSET    0xE8
#define O7_OFFSET 0xF0

#define ISR_DISPATCH_DISABLE_STACK_OFFSET 0xF8
#define ISR_PAD_OFFSET 0xFC

#define CONTEXT_CONTROL_SIZE 0x100

/*
 *  The floating point context area.
 */

#ifndef ASM

typedef struct {
    double      f0;	/* f0-f1 */
    double      f2;	/* f2-f3 */
    double      f4;	/* f4-f5 */
    double      f6;	/* f6-f7 */
    double      f8;	/* f8-f9 */
    double      f10;	/* f10-f11 */
    double      f12;	/* f12-f13 */
    double      f14;	/* f14-f15 */
    double      f16;	/* f16-f17 */
    double      f18;	/* f18-f19 */
    double      f20;	/* f20-f21 */
    double      f22;	/* f22-f23 */
    double      f24;	/* f24-f25 */
    double      f26;	/* f26-f27 */
    double      f28;	/* f28-f29 */
    double      f30;	/* f30-f31 */
    double      f32;
    double      f34;
    double      f36;
    double      f38;
    double      f40;
    double      f42;
    double      f44;
    double      f46;
    double      f48;
    double      f50;
    double      f52;
    double      f54;
    double      f56;
    double      f58;
    double      f60;
    double      f62;
    uint64_t    fsr;
} Context_Control_fp;

#endif /* !ASM */

/*
 *  Offsets of fields with Context_Control_fp for assembly routines.
 */

#define FO_OFFSET    0x00
#define F2_OFFSET    0x08
#define F4_OFFSET    0x10
#define F6_OFFSET    0x18
#define F8_OFFSET    0x20
#define F1O_OFFSET   0x28
#define F12_OFFSET   0x30
#define F14_OFFSET   0x38
#define F16_OFFSET   0x40
#define F18_OFFSET   0x48
#define F2O_OFFSET   0x50
#define F22_OFFSET   0x58
#define F24_OFFSET   0x60
#define F26_OFFSET   0x68
#define F28_OFFSET   0x70
#define F3O_OFFSET   0x78
#define F32_OFFSET   0x80
#define F34_OFFSET   0x88
#define F36_OFFSET   0x90
#define F38_OFFSET   0x98
#define F4O_OFFSET   0xA0
#define F42_OFFSET   0xA8
#define F44_OFFSET   0xB0
#define F46_OFFSET   0xB8
#define F48_OFFSET   0xC0
#define F5O_OFFSET   0xC8
#define F52_OFFSET   0xD0
#define F54_OFFSET   0xD8
#define F56_OFFSET   0xE0
#define F58_OFFSET   0xE8
#define F6O_OFFSET   0xF0
#define F62_OFFSET   0xF8
#define FSR_OFFSET   0x100

#define CONTEXT_CONTROL_FP_SIZE 0x108

#ifndef ASM

/*
 *  Context saved on stack for an interrupt.
 *
 *  NOTE:  The tstate, tpc, and tnpc are saved in this structure
 *  	   to allow resetting the TL while still being able to return
 *  	   from a trap later.  The PIL is saved because
 *         if this is an external interrupt, we will mask lower 
 *         priority interrupts until finishing. Even though the y register
 *         is deprecated, gcc still uses it.
 */

typedef struct {
  CPU_Minimum_stack_frame  Stack_frame;
  uint64_t                 tstate;
  uint64_t                 tpc;
  uint64_t                 tnpc;
  uint64_t                 pil;
  uint64_t                 y;
  uint64_t                 g1;
  uint64_t                 g2;
  uint64_t                 g3;
  uint64_t                 g4;
  uint64_t                 g5;
  uint64_t                 g6;
  uint64_t                 g7;
  uint64_t                 o0;
  uint64_t                 o1;
  uint64_t                 o2;
  uint64_t                 o3;
  uint64_t                 o4;
  uint64_t                 o5;
  uint64_t                 o6_sp;
  uint64_t                 o7;
  uint64_t                 tvec;
} CPU_Interrupt_frame;

#endif /* ASM */

/*
 *  Offsets of fields with CPU_Interrupt_frame for assembly routines.
 */

#define ISF_STACK_FRAME_OFFSET 0x00
#define ISF_TSTATE_OFFSET      CPU_MINIMUM_STACK_FRAME_SIZE + 0x00
#define ISF_TPC_OFFSET         CPU_MINIMUM_STACK_FRAME_SIZE + 0x08
#define ISF_TNPC_OFFSET        CPU_MINIMUM_STACK_FRAME_SIZE + 0x10
#define ISF_PIL_OFFSET         CPU_MINIMUM_STACK_FRAME_SIZE + 0x18
#define ISF_Y_OFFSET           CPU_MINIMUM_STACK_FRAME_SIZE + 0x20
#define ISF_G1_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x28
#define ISF_G2_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x30
#define ISF_G3_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x38
#define ISF_G4_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x40
#define ISF_G5_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x48
#define ISF_G6_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x50
#define ISF_G7_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x58
#define ISF_O0_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x60
#define ISF_O1_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x68
#define ISF_O2_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x70
#define ISF_O3_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x78
#define ISF_O4_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x80
#define ISF_O5_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x88
#define ISF_O6_SP_OFFSET       CPU_MINIMUM_STACK_FRAME_SIZE + 0x90
#define ISF_O7_OFFSET          CPU_MINIMUM_STACK_FRAME_SIZE + 0x98
#define ISF_TVEC_OFFSET        CPU_MINIMUM_STACK_FRAME_SIZE + 0xA0

#define CONTEXT_CONTROL_INTERRUPT_FRAME_SIZE CPU_MINIMUM_STACK_FRAME_SIZE + 0xA8 
#ifndef ASM
/*
 *  This variable is contains the initialize context for the FP unit.
 *  It is filled in by _CPU_Initialize and copied into the task's FP 
 *  context area during _CPU_Context_Initialize.
 */

SCORE_EXTERN Context_Control_fp  _CPU_Null_fp_context CPU_STRUCTURE_ALIGNMENT;

/*
 *  This stack is allocated by the Interrupt Manager and the switch
 *  is performed in _ISR_Handler.  These variables contain pointers
 *  to the lowest and highest addresses in the chunk of memory allocated
 *  for the interrupt stack.  Since it is unknown whether the stack
 *  grows up or down (in general), this give the CPU dependent
 *  code the option of picking the version it wants to use.  Thus
 *  both must be present if either is.
 *
 *  The SPARC supports a software based interrupt stack and these
 *  are required.
 */
/*
SCORE_EXTERN void *_CPU_Interrupt_stack_low;
SCORE_EXTERN void *_CPU_Interrupt_stack_high;
*/
/*
 *  This flag is context switched with each thread.  It indicates
 *  that THIS thread has an _ISR_Dispatch stack frame on its stack.
 *  By using this flag, we can avoid nesting more interrupt dispatching
 *  attempts on a previously interrupted thread's stack.
 */

SCORE_EXTERN volatile uint32_t _CPU_ISR_Dispatch_disable;

/*
 *  The following type defines an entry in the SPARC's trap table.
 *
 *  NOTE: The instructions chosen are RTEMS dependent although one is
 *        obligated to use two of the four instructions to perform a
 *        long jump.  The other instructions load one register with the
 *        trap type (a.k.a. vector) and another with the psr.
 */
/* For SPARC V9, we must use 6 of these instructions to perform a long 
 * jump, because the _handler value is now 64-bits. We also need to store 
 * temporary values in the global register set at this trap level. Because 
 * the handler runs at TL > 0 with GL > 0, it should be OK to use g2 and g3
 * to pass parameters to ISR_Handler.
 *
 * The instruction sequence is now more like:
 * 	rdpr %tstate, %g4
 * 	setx _handler, %g2, %g3
 * 	jmp %g3+0
 * 	mov _vector, %g2
 */
typedef struct {
  uint32_t     rdpr_tstate_g4;              	/* rdpr  %tstate, %g4        */
  uint32_t     sethi_of_hh_handler_to_g2;     	/* sethi %hh(_handler), %g2  */
  uint32_t     or_g2_hm_handler_to_g2;		/* or %l3, %hm(_handler), %g2 */
  uint32_t     sllx_g2_by_32_to_g2;		/* sllx   %g2, 32, %g2 */
  uint32_t     sethi_of_handler_to_g3;		/* sethi %hi(_handler), %g3  */
  uint32_t     or_g3_g2_to_g3;			/* or     %g3, %g2, %g3 */
  uint32_t     jmp_to_low_of_handler_plus_g3;	/* jmp   %g3 + %lo(_handler) */
  uint32_t     mov_vector_g2;			/* mov   _vector, %g2        */
} CPU_Trap_table_entry;
 
/*
 *  This is the set of opcodes for the instructions loaded into a trap
 *  table entry.  The routine which installs a handler is responsible
 *  for filling in the fields for the _handler address and the _vector
 *  trap type.
 *
 *  The constants following this structure are masks for the fields which
 *  must be filled in when the handler is installed.
 */
 
extern const CPU_Trap_table_entry _CPU_Trap_slot_template;

/*
 *  The size of the floating point context area.  
 */

#define CPU_CONTEXT_FP_SIZE sizeof( Context_Control_fp )

#endif

/*
 *  Amount of extra stack (above minimum stack size) required by
 *  MPCI receive server thread.  Remember that in a multiprocessor
 *  system this thread must exist and be able to process all directives.
 */

#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 1024

/*
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
/* On SPARC v9, there are 512 vectors. The same philosophy applies to 
 * vector installation and use, we just provide a larger table.
 */
#define CPU_INTERRUPT_NUMBER_OF_VECTORS     512
#define CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER 1023

#define SPARC_SYNCHRONOUS_TRAP_BIT_MASK     0x200
#define SPARC_ASYNCHRONOUS_TRAP( _trap )    (_trap)
#define SPARC_SYNCHRONOUS_TRAP( _trap )     ((_trap) + 512 )

#define SPARC_REAL_TRAP_NUMBER( _trap )     ((_trap) % 512)

/*
 *  This is defined if the port has a special way to report the ISR nesting
 *  level.  Most ports maintain the variable _ISR_Nest_level.
 */

#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE

/*
 *  Should be large enough to run all tests.  This ensures
 *  that a "reasonable" small application should not have any problems.
 *
 *  This appears to be a fairly generous number for the SPARC since
 *  represents a call depth of about 20 routines based on the minimum
 *  stack frame.
 */

#define CPU_STACK_MINIMUM_SIZE  (1024*8)

/*
 *  CPU's worst alignment requirement for data types on a byte boundary.  This
 *  alignment does not take into account the requirements for the stack.
 *
 *  On the SPARC, this is required for double word loads and stores.
 *
 *  Note: quad-word loads/stores need alignment of 16, but currently supported
 *  architectures do not provide HW implemented quad-word operations.
 */

#define CPU_ALIGNMENT      8

/*
 *  This number corresponds to the byte alignment requirement for the
 *  heap handler.  This alignment requirement may be stricter than that
 *  for the data types alignment specified by CPU_ALIGNMENT.  It is
 *  common for the heap to follow the same alignment requirement as
 *  CPU_ALIGNMENT.  If the CPU_ALIGNMENT is strict enough for the heap,
 *  then this should be set to CPU_ALIGNMENT.
 *
 *  NOTE:  This does not have to be a power of 2.  It does have to
 *         be greater or equal to than CPU_ALIGNMENT.
 */

#define CPU_HEAP_ALIGNMENT         CPU_ALIGNMENT

/*
 *  This number corresponds to the byte alignment requirement for memory
 *  buffers allocated by the partition manager.  This alignment requirement
 *  may be stricter than that for the data types alignment specified by
 *  CPU_ALIGNMENT.  It is common for the partition to follow the same
 *  alignment requirement as CPU_ALIGNMENT.  If the CPU_ALIGNMENT is strict
 *  enough for the partition, then this should be set to CPU_ALIGNMENT.
 *
 *  NOTE:  This does not have to be a power of 2.  It does have to
 *         be greater or equal to than CPU_ALIGNMENT.
 */

#define CPU_PARTITION_ALIGNMENT    CPU_ALIGNMENT

/*
 *  This number corresponds to the byte alignment requirement for the
 *  stack.  This alignment requirement may be stricter than that for the
 *  data types alignment specified by CPU_ALIGNMENT.  If the CPU_ALIGNMENT
 *  is strict enough for the stack, then this should be set to 0.
 *
 *  NOTE:  This must be a power of 2 either 0 or greater than CPU_ALIGNMENT.
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

/*
 *  Support routine to initialize the RTEMS vector table after it is allocated.
 */

#define _CPU_Initialize_vectors()

/*
 *  Disable all interrupts for a critical section.  The previous
 *  level is returned in _level.
 */

 #define _CPU_ISR_Disable( _level ) \
  (_level) = sparc_disable_interrupts()

/*
 *  Enable interrupts to the previous level (returned by _CPU_ISR_Disable).
 *  This indicates the end of a critical section.  The parameter
 *  _level is not modified.
 */

#define _CPU_ISR_Enable( _level ) \
  sparc_enable_interrupts( _level )

/*
 *  This temporarily restores the interrupt to _level before immediately
 *  disabling them again.  This is used to divide long critical
 *  sections into two or more parts.  The parameter _level is not
 *  modified.
 */

#define _CPU_ISR_Flash( _level ) \
   sparc_flash_interrupts( _level )

/*
 *  Map interrupt level in task mode onto the hardware that the CPU
 *  actually provides.  Currently, interrupt levels which do not
 *  map onto the CPU in a straight fashion are undefined.  
 */

#define _CPU_ISR_Set_level( _newlevel ) \
   sparc_enable_interrupts( _newlevel)

uint32_t   _CPU_ISR_Get_level( void );
 
/* end of ISR handler macros */

/* Context handler macros */

/*
 *  Initialize the context to a state suitable for starting a
 *  task after a context restore operation.  Generally, this
 *  involves:
 *
 *     - setting a starting address
 *     - preparing the stack
 *     - preparing the stack and frame pointers
 *     - setting the proper interrupt level in the context
 *     - initializing the floating point context
 *
 *  NOTE:  Implemented as a subroutine for the SPARC port.
 */

void _CPU_Context_Initialize(
  Context_Control  *the_context,
  void         *stack_base,
  uint32_t          size,
  uint32_t          new_level,
  void             *entry_point,
  bool              is_fp
);

/*
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

/*
 *  This routine is responsible for somehow restarting the currently
 *  executing task.  
 *
 *  On the SPARC, this is is relatively painless but requires a small
 *  amount of wrapper code before using the regular restore code in
 *  of the context switch.
 */

#define _CPU_Context_Restart_self( _the_context ) \
   _CPU_Context_restore( (_the_context) );

/*
 *  The FP context area for the SPARC is a simple structure and nothing
 *  special is required to find the "starting load point"
 */

#define _CPU_Context_Fp_start( _base, _offset ) \
   ( (void *) _Addresses_Add_offset( (_base), (_offset) ) )

/*
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

/*
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

/*
 *  The SPARC port uses the generic C algorithm for bitfield scan if the
 *  CPU model does not have a scan instruction.
 */

#if ( SPARC_HAS_BITSCAN == 0 )
#define CPU_USE_GENERIC_BITFIELD_CODE TRUE
#define CPU_USE_GENERIC_BITFIELD_DATA TRUE
#else
#error "scan instruction not currently supported by RTEMS!!"
#endif

/* end of Bitfield handler macros */

/* Priority handler handler macros */

/*
 *  The SPARC port uses the generic C algorithm for bitfield scan if the
 *  CPU model does not have a scan instruction.
 */

#if ( SPARC_HAS_BITSCAN == 1 )
#error "scan instruction not currently supported by RTEMS!!"
#endif

/* end of Priority handler macros */

/* functions */

/*
 *  _CPU_Initialize
 *
 *  This routine performs CPU dependent initialization.
 */

void _CPU_Initialize(void);

/*
 *  _CPU_ISR_install_raw_handler
 *
 *  This routine installs new_handler to be directly called from the trap
 *  table.
 */
 
void _CPU_ISR_install_raw_handler(
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
);

/*
 *  _CPU_ISR_install_vector
 *
 *  This routine installs an interrupt vector.
 */

void _CPU_ISR_install_vector(
  uint64_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
);

#if (CPU_PROVIDES_IDLE_THREAD_BODY == TRUE)
 
/*
 *  _CPU_Thread_Idle_body
 *
 *  Some SPARC implementations have low power, sleep, or idle modes.  This
 *  tries to take advantage of those models.
 */
 
void *_CPU_Thread_Idle_body( uintptr_t ignored );

#endif /* CPU_PROVIDES_IDLE_THREAD_BODY */

/*
 *  _CPU_Context_switch
 *
 *  This routine switches from the run context to the heir context.
 */

void _CPU_Context_switch(
  Context_Control  *run,
  Context_Control  *heir
);

/*
 *  _CPU_Context_restore
 *
 *  This routine is generally used only to restart self in an
 *  efficient manner.
 */

void _CPU_Context_restore(
  Context_Control *new_context
) RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

/*
 *  _CPU_Context_save_fp
 *
 *  This routine saves the floating point context passed to it.
 */

void _CPU_Context_save_fp(
  Context_Control_fp **fp_context_ptr
);

/*
 *  _CPU_Context_restore_fp
 *
 *  This routine restores the floating point context passed to it.
 */

void _CPU_Context_restore_fp(
  Context_Control_fp **fp_context_ptr
);

/*
 *  CPU_swap_u32
 *
 *  The following routine swaps the endian format of an unsigned int.
 *  It must be static because it is referenced indirectly.
 *
 *  This version will work on any processor, but if you come across a better
 *  way for the SPARC PLEASE use it.  The most common way to swap a 32-bit 
 *  entity as shown below is not any more efficient on the SPARC.
 *
 *     swap least significant two bytes with 16-bit rotate
 *     swap upper and lower 16-bits
 *     swap most significant two bytes with 16-bit rotate
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

#define CPU_swap_u16( value ) \
  (((value&0xff) << 8) | ((value >> 8)&0xff))

#endif /* ASM */

#ifdef __cplusplus
}
#endif

#endif
