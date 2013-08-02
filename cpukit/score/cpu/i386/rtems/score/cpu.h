/**
 * @file
 * 
 * @brief Intel I386 CPU Dependent Source
 * 
 * This include file contains information pertaining to the Intel
 * i386 processor.
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

#ifndef ASM
#include <string.h> /* for memcpy */
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/types.h>
#include <rtems/score/i386.h>

#ifndef ASM
#include <rtems/score/interrupts.h>	/* formerly in libcpu/cpu.h> */
#include <rtems/score/registers.h>	/* formerly part of libcpu */
#endif

/* conditional compilation parameters */

#define CPU_INLINE_ENABLE_DISPATCH       TRUE
#define CPU_UNROLL_ENQUEUE_PRIORITY      FALSE

/*
 *  Does the CPU follow the simple vectored interrupt model?
 *
 *  If TRUE, then RTEMS allocates the vector table it internally manages.
 *  If FALSE, then the BSP is assumed to allocate and manage the vector
 *  table
 *
 *  PowerPC Specific Information:
 *
 *  The PowerPC and x86 were the first to use the PIC interrupt model.
 *  They do not use the simple vectored interrupt model.
 */
#define CPU_SIMPLE_VECTORED_INTERRUPTS FALSE

/*
 *  i386 has an RTEMS allocated and managed interrupt stack.
 */

#define CPU_HAS_SOFTWARE_INTERRUPT_STACK TRUE
#define CPU_HAS_HARDWARE_INTERRUPT_STACK FALSE
#define CPU_ALLOCATE_INTERRUPT_STACK     TRUE

/*
 *  Does the RTEMS invoke the user's ISR with the vector number and
 *  a pointer to the saved interrupt frame (1) or just the vector
 *  number (0)?
 */

#define CPU_ISR_PASSES_FRAME_POINTER 0

/*
 *  Some family members have no FP, some have an FPU such as the i387
 *  for the i386, others have it built in (i486DX, Pentium).
 */

#ifdef __SSE__
#define CPU_HARDWARE_FP                  TRUE
#define CPU_SOFTWARE_FP                  FALSE

#define CPU_ALL_TASKS_ARE_FP             TRUE
#define CPU_IDLE_TASK_IS_FP              TRUE
#define CPU_USE_DEFERRED_FP_SWITCH       FALSE
#else /* __SSE__ */

#if ( I386_HAS_FPU == 1 )
#define CPU_HARDWARE_FP     TRUE    /* i387 for i386 */
#else
#define CPU_HARDWARE_FP     FALSE
#endif
#define CPU_SOFTWARE_FP     FALSE

#define CPU_ALL_TASKS_ARE_FP             FALSE
#define CPU_IDLE_TASK_IS_FP              FALSE
#if defined(RTEMS_SMP)
  #define CPU_USE_DEFERRED_FP_SWITCH     FALSE
#else
  #define CPU_USE_DEFERRED_FP_SWITCH     TRUE
#endif
#endif /* __SSE__ */

#define CPU_STACK_GROWS_UP               FALSE
#define CPU_STRUCTURE_ALIGNMENT

#define CPU_TIMESTAMP_USE_INT64_INLINE TRUE

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

#define CPU_PROVIDES_IDLE_THREAD_BODY    TRUE

/*
 *  Define what is required to specify how the network to host conversion
 *  routines are handled.
 */

#define CPU_BIG_ENDIAN                           FALSE
#define CPU_LITTLE_ENDIAN                        TRUE

#define CPU_PER_CPU_CONTROL_SIZE 0

/* structures */

#ifndef ASM

typedef struct {
  /* There is no CPU specific per-CPU state */
} CPU_Per_CPU_control;

/*
 *  Basic integer context for the i386 family.
 */

typedef struct {
  uint32_t    eflags;   /* extended flags register                   */
  void       *esp;      /* extended stack pointer register           */
  void       *ebp;      /* extended base pointer register            */
  uint32_t    ebx;      /* extended bx register                      */
  uint32_t    esi;      /* extended source index register            */
  uint32_t    edi;      /* extended destination index flags register */
}   Context_Control;

#define _CPU_Context_Get_SP( _context ) \
  (_context)->esp

/*
 *  FP context save area for the i387 numeric coprocessors.
 */
#ifdef __SSE__
/* All FPU and SSE registers are volatile; hence, as long
 * as we are within normally executing C code (including
 * a task switch) there is no need for saving/restoring
 * any of those registers.
 * We must save/restore the full FPU/SSE context across
 * interrupts and exceptions, however:
 *   -  after ISR execution a _Thread_Dispatch() may happen
 *      and it is therefore necessary to save the FPU/SSE
 *      registers to be restored when control is returned
 *      to the interrupted task.
 *   -  gcc may implicitly use FPU/SSE instructions in
 *      an ISR.
 *
 * Even though there is no explicit mentioning of the FPU
 * control word in the SYSV ABI (i386) being non-volatile
 * we maintain MXCSR and the FPU control-word for each task.
 */
typedef struct {
	uint32_t  mxcsr;
	uint16_t  fpucw;
} Context_Control_fp;

#else

typedef struct {
  uint8_t     fp_save_area[108];    /* context size area for I80387 */
                                    /*  28 bytes for environment    */
} Context_Control_fp;

#endif


/*
 *  The following structure defines the set of information saved
 *  on the current stack by RTEMS upon receipt of execptions.
 *
 * idtIndex is either the interrupt number or the trap/exception number.
 * faultCode is the code pushed by the processor on some exceptions.
 *
 * Since the first registers are directly pushed by the CPU they
 * may not respect 16-byte stack alignment, which is, however,
 * mandatory for the SSE register area.
 * Therefore, these registers are stored at an aligned address
 * and a pointer is stored in the CPU_Exception_frame.
 * If the executive was compiled without SSE support then
 * this pointer is NULL.
 */

struct Context_Control_sse;

typedef struct {
  struct Context_Control_sse *fp_ctxt;
  uint32_t    edi;
  uint32_t    esi;
  uint32_t    ebp;
  uint32_t    esp0;
  uint32_t    ebx;
  uint32_t    edx;
  uint32_t    ecx;
  uint32_t    eax;
  uint32_t    idtIndex;
  uint32_t    faultCode;
  uint32_t    eip;
  uint32_t    cs;
  uint32_t    eflags;
} CPU_Exception_frame;

#ifdef __SSE__
typedef struct Context_Control_sse {
  uint16_t  fcw;
  uint16_t  fsw;
  uint8_t   ftw;
  uint8_t   res_1;
  uint16_t  fop;
  uint32_t  fpu_ip;
  uint16_t  cs;
  uint16_t  res_2;
  uint32_t  fpu_dp;
  uint16_t  ds;
  uint16_t  res_3;
  uint32_t  mxcsr;
  uint32_t  mxcsr_mask;
  struct {
  	uint8_t fpreg[10];
  	uint8_t res_4[ 6];
  } fp_mmregs[8];
  uint8_t   xmmregs[8][16];
  uint8_t   res_5[224];
} Context_Control_sse
__attribute__((aligned(16)))
;
#endif

typedef void (*cpuExcHandlerType) (CPU_Exception_frame*);
extern cpuExcHandlerType _currentExcHandler;
extern void rtems_exception_init_mngt(void);

/*
 * This port does not pass any frame info to the
 * interrupt handler.
 */

typedef void CPU_Interrupt_frame;

typedef enum {
  I386_EXCEPTION_DIVIDE_BY_ZERO      = 0,
  I386_EXCEPTION_DEBUG               = 1,
  I386_EXCEPTION_NMI                 = 2,
  I386_EXCEPTION_BREAKPOINT          = 3,
  I386_EXCEPTION_OVERFLOW            = 4,
  I386_EXCEPTION_BOUND               = 5,
  I386_EXCEPTION_ILLEGAL_INSTR       = 6,
  I386_EXCEPTION_MATH_COPROC_UNAVAIL = 7,
  I386_EXCEPTION_DOUBLE_FAULT        = 8,
  I386_EXCEPTION_I386_COPROC_SEG_ERR = 9,
  I386_EXCEPTION_INVALID_TSS         = 10,
  I386_EXCEPTION_SEGMENT_NOT_PRESENT = 11,
  I386_EXCEPTION_STACK_SEGMENT_FAULT = 12,
  I386_EXCEPTION_GENERAL_PROT_ERR    = 13,
  I386_EXCEPTION_PAGE_FAULT          = 14,
  I386_EXCEPTION_INTEL_RES15         = 15,
  I386_EXCEPTION_FLOAT_ERROR         = 16,
  I386_EXCEPTION_ALIGN_CHECK         = 17,
  I386_EXCEPTION_MACHINE_CHECK       = 18,
  I386_EXCEPTION_ENTER_RDBG          = 50     /* to enter manually RDBG */

} Intel_symbolic_exception_name;


/*
 *  context size area for floating point
 *
 *  NOTE:  This is out of place on the i386 to avoid a forward reference.
 */

#define CPU_CONTEXT_FP_SIZE sizeof( Context_Control_fp )

/* variables */

SCORE_EXTERN Context_Control_fp  _CPU_Null_fp_context;

#endif /* ASM */

/* constants */

/*
 *  This defines the number of levels and the mask used to pick those
 *  bits out of a thread mode.
 */

#define CPU_MODES_INTERRUPT_LEVEL  0x00000001 /* interrupt level in mode */
#define CPU_MODES_INTERRUPT_MASK   0x00000001 /* interrupt level in mode */

/*
 *  extra stack required by the MPCI receive server thread
 */

#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 1024

/*
 *  i386 family supports 256 distinct vectors.
 */

#define CPU_INTERRUPT_NUMBER_OF_VECTORS      256
#define CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER  (CPU_INTERRUPT_NUMBER_OF_VECTORS - 1)

/*
 *  This is defined if the port has a special way to report the ISR nesting
 *  level.  Most ports maintain the variable _ISR_Nest_level.
 */

#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE

/*
 *  Minimum size of a thread's stack.
 */

#define CPU_STACK_MINIMUM_SIZE          4096

#define CPU_SIZEOF_POINTER 4

/*
 *  i386 is pretty tolerant of alignment.  Just put things on 4 byte boundaries.
 */

#define CPU_ALIGNMENT                    4
#define CPU_HEAP_ALIGNMENT               CPU_ALIGNMENT
#define CPU_PARTITION_ALIGNMENT          CPU_ALIGNMENT

/*
 *  On i386 thread stacks require no further alignment after allocation
 *  from the Workspace. However, since gcc maintains 16-byte alignment
 *  we try to respect that. If you find an option to let gcc squeeze
 *  the stack more tightly then setting CPU_STACK_ALIGNMENT to 16 still
 *  doesn't waste much space since this only determines the *initial*
 *  alignment.
 */

#define CPU_STACK_ALIGNMENT             16

/* macros */

#ifndef ASM
/*
 *  ISR handler macros
 *
 *  These macros perform the following functions:
 *     + initialize the RTEMS vector table
 *     + disable all maskable CPU interrupts
 *     + restore previous interrupt level (enable)
 *     + temporarily restore interrupts (flash)
 *     + set a particular level
 */

#define _CPU_ISR_Disable( _level ) i386_disable_interrupts( _level )

#define _CPU_ISR_Enable( _level )  i386_enable_interrupts( _level )

#define _CPU_ISR_Flash( _level )   i386_flash_interrupts( _level )

#define _CPU_ISR_Set_level( _new_level ) \
  { \
    if ( _new_level ) __asm__ volatile ( "cli" ); \
    else              __asm__ volatile ( "sti" ); \
  }

uint32_t   _CPU_ISR_Get_level( void );

/*  Make sure interrupt stack has space for ISR
 *  'vector' arg at the top and that it is aligned
 *  properly.
 */

#define _CPU_Interrupt_stack_setup( _lo, _hi )  \
	do {                                        \
		_hi = (void*)(((uintptr_t)(_hi) - 4) & ~ (CPU_STACK_ALIGNMENT - 1)); \
	} while (0)

#endif /* ASM */

/* end of ISR handler macros */

/*
 *  Context handler macros
 *
 *  These macros perform the following functions:
 *     + initialize a context area
 *     + restart the current thread
 *     + calculate the initial pointer into a FP context area
 *     + initialize an FP context area
 */

#define CPU_EFLAGS_INTERRUPTS_ON  0x00003202
#define CPU_EFLAGS_INTERRUPTS_OFF 0x00003002

#ifndef ASM

/*
 * Stack alignment note:
 *
 * We want the stack to look to the '_entry_point' routine
 * like an ordinary stack frame as if '_entry_point' was
 * called from C-code.
 * Note that '_entry_point' is jumped-to by the 'ret'
 * instruction returning from _CPU_Context_switch() or
 * _CPU_Context_restore() thus popping the _entry_point
 * from the stack.
 * However, _entry_point expects a frame to look like this:
 *
 *      args        [_Thread_Handler expects no args, however]
 *      ------      (alignment boundary)
 * SP-> return_addr return here when _entry_point returns which (never happens)
 *
 *
 * Hence we must initialize the stack as follows
 *
 *         [arg1          ]:  n/a
 *         [arg0 (aligned)]:  n/a
 *         [ret. addr     ]:  NULL
 * SP->    [jump-target   ]:  _entry_point
 *
 * When Context_switch returns it pops the _entry_point from
 * the stack which then finds a standard layout.
 */


#define _CPU_Context_Initialize( _the_context, _stack_base, _size, \
                                   _isr, _entry_point, _is_fp ) \
  do { \
    uint32_t   _stack; \
    \
    if ( (_isr) ) (_the_context)->eflags = CPU_EFLAGS_INTERRUPTS_OFF; \
    else          (_the_context)->eflags = CPU_EFLAGS_INTERRUPTS_ON; \
    \
    _stack  = ((uint32_t)(_stack_base)) + (_size); \
	_stack &= ~ (CPU_STACK_ALIGNMENT - 1); \
    _stack -= 2*sizeof(proc_ptr*); /* see above for why we need to do this */ \
    *((proc_ptr *)(_stack)) = (_entry_point); \
    (_the_context)->ebp     = (void *) 0; \
    (_the_context)->esp     = (void *) _stack; \
  } while (0)

#define _CPU_Context_Restart_self( _the_context ) \
   _CPU_Context_restore( (_the_context) );

#if defined(RTEMS_SMP)
  #define _CPU_Context_switch_to_first_task_smp( _the_context ) \
     _CPU_Context_restore( (_the_context) );

  RTEMS_COMPILER_PURE_ATTRIBUTE uint32_t _CPU_SMP_Get_current_processor( void );

  void _CPU_SMP_Send_interrupt( uint32_t target_processor_index );

  static inline void _CPU_SMP_Processor_event_broadcast( void )
  {
    __asm__ volatile ( "" : : : "memory" );
  }

  static inline void _CPU_SMP_Processor_event_receive( void )
  {
    __asm__ volatile ( "" : : : "memory" );
  }
#endif

#define _CPU_Context_Fp_start( _base, _offset ) \
   ( (void *) _Addresses_Add_offset( (_base), (_offset) ) )

#define _CPU_Context_Initialize_fp( _fp_area ) \
  { \
    memcpy( *_fp_area, &_CPU_Null_fp_context, CPU_CONTEXT_FP_SIZE ); \
  }

/* end of Context handler macros */

/*
 *  Fatal Error manager macros
 *
 *  These macros perform the following functions:
 *    + disable interrupts and halt the CPU
 */

#define _CPU_Fatal_halt( _error ) \
  { \
    uint32_t _error_lvalue = ( _error ); \
    __asm__ volatile ( "cli ; \
                    movl %0,%%eax ; \
                    hlt" \
                    : "=r" ((_error_lvalue)) : "0" ((_error_lvalue)) \
    ); \
  }

#endif /* ASM */

/* end of Fatal Error manager macros */

/*
 *  Bitfield handler macros
 *
 *  These macros perform the following functions:
 *     + scan for the highest numbered (MSB) set in a 16 bit bitfield
 */

#define CPU_USE_GENERIC_BITFIELD_CODE FALSE
#define CPU_USE_GENERIC_BITFIELD_DATA FALSE

#define _CPU_Bitfield_Find_first_bit( _value, _output ) \
  { \
    register uint16_t   __value_in_register = (_value); \
    \
    _output = 0; \
    \
    __asm__ volatile ( "bsfw    %0,%1 " \
                    : "=r" (__value_in_register), "=r" (_output) \
                    : "0"  (__value_in_register), "1"  (_output) \
    ); \
  }

/* end of Bitfield handler macros */

/*
 *  Priority handler macros
 *
 *  These macros perform the following functions:
 *    + return a mask with the bit for this major/minor portion of
 *      of thread priority set.
 *    + translate the bit number returned by "Bitfield_find_first_bit"
 *      into an index into the thread ready chain bit maps
 */

#define _CPU_Priority_Mask( _bit_number ) \
  ( 1 << (_bit_number) )

#define _CPU_Priority_bits_index( _priority ) \
  (_priority)

/* functions */

#ifndef ASM
/*
 *  _CPU_Initialize
 *
 *  This routine performs CPU dependent initialization.
 */

void _CPU_Initialize(void);

/*
 *  _CPU_ISR_install_raw_handler
 *
 *  This routine installs a "raw" interrupt handler directly into the
 *  processor's vector table.
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
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
);

/*
 *  _CPU_Thread_Idle_body
 *
 *  Use the halt instruction of low power mode of a particular i386 model.
 */

#if (CPU_PROVIDES_IDLE_THREAD_BODY == TRUE)

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
 *  efficient manner and avoid stack conflicts.
 */

void _CPU_Context_restore(
  Context_Control *new_context
) RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

/*
 *  _CPU_Context_save_fp
 *
 *  This routine saves the floating point context passed to it.
 */

#ifdef __SSE__
#define _CPU_Context_save_fp(fp_context_pp) \
  do {                                      \
    __asm__ __volatile__(                   \
      "fstcw %0"                            \
      :"=m"((*(fp_context_pp))->fpucw)      \
    );                                      \
	__asm__ __volatile__(                   \
      "stmxcsr %0"                          \
      :"=m"((*(fp_context_pp))->mxcsr)      \
    );                                      \
  } while (0)
#else
void _CPU_Context_save_fp(
  Context_Control_fp **fp_context_ptr
);
#endif

/*
 *  _CPU_Context_restore_fp
 *
 *  This routine restores the floating point context passed to it.
 */
#ifdef __SSE__
#define _CPU_Context_restore_fp(fp_context_pp) \
  do {                                         \
    __asm__ __volatile__(                      \
      "fldcw %0"                               \
      ::"m"((*(fp_context_pp))->fpucw)         \
      :"fpcr"                                  \
    );                                         \
    __builtin_ia32_ldmxcsr(_Thread_Executing->fp_context->mxcsr);  \
  } while (0)
#else
void _CPU_Context_restore_fp(
  Context_Control_fp **fp_context_ptr
);
#endif

#ifdef __SSE__
#define _CPU_Context_Initialization_at_thread_begin() \
  do {                                                \
    __asm__ __volatile__(                             \
      "finit"                                         \
      :                                               \
      :                                               \
      :"st","st(1)","st(2)","st(3)",                  \
       "st(4)","st(5)","st(6)","st(7)",               \
       "fpsr","fpcr"                                  \
    );                                                \
	if ( _Thread_Executing->fp_context ) {            \
	  _CPU_Context_restore_fp(&_Thread_Executing->fp_context); \
   }                                                  \
  } while (0)
#endif

static inline void _CPU_Context_volatile_clobber( uintptr_t pattern )
{
  /* TODO */
}

static inline void _CPU_Context_validate( uintptr_t pattern )
{
  while (1) {
    /* TODO */
  }
}

void _CPU_Exception_frame_print( const CPU_Exception_frame *frame );

#endif /* ASM */

#ifdef __cplusplus
}
#endif

#endif
