/*  cpu.h
 *
 *  This include file contains information pertaining to the Intel
 *  i386 processor.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __CPU_h
#define __CPU_h

#ifdef __cplusplus
extern "C" {
#endif

#include <i386.h>

/* conditional compilation parameters */

#define CPU_INLINE_ENABLE_DISPATCH       TRUE
#define CPU_UNROLL_ENQUEUE_PRIORITY      FALSE

/*
 *  i386 has an RTEMS allocated and managed interrupt stack.
 */

#define CPU_HAS_SOFTWARE_INTERRUPT_STACK TRUE
#define CPU_HAS_HARDWARE_INTERRUPT_STACK FALSE
#define CPU_ALLOCATE_INTERRUPT_STACK     TRUE

/*
 *  Some family members have no FP, some have an FPU such as the i387
 *  for the i386, others have it built in (i486DX, Pentium).
 */

#if ( I386_HAS_FPU == 1 )
#define CPU_HARDWARE_FP     TRUE    /* i387 for i386 */
#else
#define CPU_HARDWARE_FP     FALSE
#endif

#define CPU_ALL_TASKS_ARE_FP             FALSE
#define CPU_IDLE_TASK_IS_FP              FALSE
#define CPU_USE_DEFERRED_FP_SWITCH       TRUE

#define CPU_PROVIDES_IDLE_THREAD_BODY    FALSE
#define CPU_STACK_GROWS_UP               FALSE
#define CPU_STRUCTURE_ALIGNMENT

/* structures */

/*
 *  Basic integer context for the i386 family.
 */

typedef struct {
  unsigned32  eflags;   /* extended flags register                   */
  void       *esp;      /* extended stack pointer register           */
  void       *ebp;      /* extended base pointer register            */
  unsigned32  ebx;      /* extended bx register                      */
  unsigned32  esi;      /* extended source index register            */
  unsigned32  edi;      /* extended destination index flags register */
}   Context_Control;

/*
 *  FP context save area for the i387 numeric coprocessors.
 */

typedef struct {
  unsigned8   fp_save_area[108];    /* context size area for I80387 */
                                    /*  28 bytes for environment    */
} Context_Control_fp;

/*
 *  The following structure defines the set of information saved
 *  on the current stack by RTEMS upon receipt of each interrupt.
 */

typedef struct {
  unsigned32   TBD;   /* XXX Fix for this CPU */
} CPU_Interrupt_frame;

/*
 *  The following table contains the information required to configure
 *  the i386 specific parameters.
 */

typedef struct {
  void       (*pretasking_hook)( void );
  void       (*predriver_hook)( void );
  void       (*postdriver_hook)( void );
  void       (*idle_task)( void );
  boolean      do_zero_of_workspace;
  unsigned32   interrupt_stack_size;
  unsigned32   extra_system_initialization_stack;

  unsigned32   interrupt_table_segment;
  void        *interrupt_table_offset;
}   rtems_cpu_table;

/*
 *  context size area for floating point
 *
 *  NOTE:  This is out of place on the i386 to avoid a forward reference.
 */

#define CPU_CONTEXT_FP_SIZE sizeof( Context_Control_fp )

/* variables */

EXTERN unsigned8  _CPU_Null_fp_context[ CPU_CONTEXT_FP_SIZE ];
EXTERN void      *_CPU_Interrupt_stack_low;
EXTERN void      *_CPU_Interrupt_stack_high;

/* constants */

/*
 *  This defines the number of levels and the mask used to pick those
 *  bits out of a thread mode.
 */

#define CPU_MODES_INTERRUPT_LEVEL  0x00000001 /* interrupt level in mode */
#define CPU_MODES_INTERRUPT_MASK   0x00000001 /* interrupt level in mode */

/*
 *  extra stack required by system initialization thread
 */

#define CPU_SYSTEM_INITIALIZATION_THREAD_EXTRA_STACK 1024

/*
 *  i386 family supports 256 distinct vectors.
 */

#define CPU_INTERRUPT_NUMBER_OF_VECTORS 256

/*
 *  Minimum size of a thread's stack.
 *
 *  NOTE:  256 bytes is probably too low in most cases.
 */

#define CPU_STACK_MINIMUM_SIZE          256

/*
 *  i386 is pretty tolerant of alignment.  Just put things on 4 byte boundaries.
 */

#define CPU_ALIGNMENT                    4
#define CPU_HEAP_ALIGNMENT               CPU_ALIGNMENT
#define CPU_PARTITION_ALIGNMENT          CPU_ALIGNMENT

/*
 *  On i386 thread stacks require no further alignment after allocation
 *  from the Workspace.
 */

#define CPU_STACK_ALIGNMENT             0

/* macros */

/*
 *  ISR handler macros
 *
 *  These macros perform the following functions:
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
    if ( _new_level ) asm volatile ( "cli" ); \
    else              asm volatile ( "sti" ); \
  }

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

#define _CPU_Context_Initialize( _the_context, _stack_base, _size, \
                                   _isr, _entry_point ) \
  do { \
    unsigned32 _stack; \
    \
    if ( (_isr) ) (_the_context)->eflags = CPU_EFLAGS_INTERRUPTS_OFF; \
    else          (_the_context)->eflags = CPU_EFLAGS_INTERRUPTS_ON; \
    \
    _stack = ((unsigned32)(_stack_base)) + (_size) - 4; \
    \
    *((proc_ptr *)(_stack)) = (_entry_point); \
    (_the_context)->ebp     = (void *) _stack; \
    (_the_context)->esp     = (void *) _stack; \
  } while (0)

#define _CPU_Context_Restart_self( _the_context ) \
   _CPU_Context_restore( (_the_context) );

#define _CPU_Context_Fp_start( _base, _offset ) \
   ( (void *) _Addresses_Add_offset( (_base), (_offset) ) )

#define _CPU_Context_Initialize_fp( _fp_area ) \
  { \
    unsigned32 *_source      = (unsigned32 *) _CPU_Null_fp_context; \
    unsigned32 *_destination = (unsigned32 *) *(_fp_area); \
    unsigned32  _index; \
    \
    for ( _index=0 ; _index < CPU_CONTEXT_FP_SIZE/4 ; _index++ ) \
      *_destination++ = *_source++; \
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
    asm volatile ( "cli ; \
                    movl %0,%%eax ; \
                    hlt" \
                    : "=r" ((_error)) : "0" ((_error)) \
    ); \
  }

/* end of Fatal Error manager macros */

/*
 *  Bitfield handler macros
 *
 *  These macros perform the following functions:
 *     + scan for the highest numbered (MSB) set in a 16 bit bitfield
 */

#define _CPU_Bitfield_Find_first_bit( _value, _output ) \
  { \
    register unsigned16 __value_in_register = (_value); \
    \
    _output = 0; \
    \
    asm volatile ( "bsfw    %0,%1 " \
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

#define _CPU_Priority_Bits_index( _priority ) \
  (_priority)

/* functions */

/*
 *  _CPU_Initialize
 *
 *  This routine performs CPU dependent initialization.
 */

void _CPU_Initialize(
  rtems_cpu_table  *cpu_table,
  void      (*thread_dispatch)
);

/*
 *  _CPU_ISR_install_vector
 *
 *  This routine installs an interrupt vector.
 */

void _CPU_ISR_install_vector(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
);

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
 *  This routine is generallu used only to restart self in an
 *  efficient manner and avoid stack conflicts.
 */

void _CPU_Context_restore(
  Context_Control *new_context
);

/*
 *  _CPU_Context_save_fp
 *
 *  This routine saves the floating point context passed to it.
 */

void _CPU_Context_save_fp(
  void **fp_context_ptr
);

/*
 *  _CPU_Context_restore_fp
 *
 *  This routine restores the floating point context passed to it.
 */

void _CPU_Context_restore_fp(
  void **fp_context_ptr
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
