/*  cpu.h
 *
 *  This include file contains information pertaining to the Motorola
 *  m68xxx processor family.
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

/*
 *  If defined, this causes some of the macros to initialize their
 *  variables to zero before doing inline assembly.  This gets rid
 *  of compile time warnings at the cost of a little execution time
 *  in some time critical routines.
 */

#define NO_UNINITIALIZED_WARNINGS

#include <rtems/score/m68k.h>
#ifndef ASM
#include <rtems/score/m68ktypes.h>
#endif

/* conditional compilation parameters */

#define CPU_INLINE_ENABLE_DISPATCH       TRUE
#define CPU_UNROLL_ENQUEUE_PRIORITY      FALSE

/*
 *  Use the m68k's hardware interrupt stack support and have the
 *  interrupt manager allocate the memory for it.
 *
 *  NOTE:  The definitions when M68K_HAS_SEPARATE_STACKS is 0 should
 *         change when the software interrupt stack support is implemented.
 */

#if ( M68K_HAS_SEPARATE_STACKS == 1)
#define CPU_HAS_SOFTWARE_INTERRUPT_STACK FALSE
#define CPU_HAS_HARDWARE_INTERRUPT_STACK TRUE
#define CPU_ALLOCATE_INTERRUPT_STACK     TRUE
#else
#define CPU_HAS_SOFTWARE_INTERRUPT_STACK FALSE
#define CPU_HAS_HARDWARE_INTERRUPT_STACK FALSE
#define CPU_ALLOCATE_INTERRUPT_STACK     FALSE
#endif

/*
 *  Some family members have no FP, some have an FPU such as the
 *  MC68881/MC68882 for the MC68020, others have it built in (MC68030, 040).
 */

#if ( M68K_HAS_FPU == 1 )
#define CPU_HARDWARE_FP     TRUE
#else
#define CPU_HARDWARE_FP     FALSE
#endif

/*
 *  All tasks are not by default floating point tasks on this CPU.
 *  The IDLE task does not have a floating point context on this CPU.
 *  It is safe to use the deferred floating point context switch
 *  algorithm on this CPU.
 */

#define CPU_ALL_TASKS_ARE_FP             FALSE
#define CPU_IDLE_TASK_IS_FP              FALSE
#define CPU_USE_DEFERRED_FP_SWITCH       TRUE

#define CPU_PROVIDES_IDLE_THREAD_BODY    FALSE
#define CPU_STACK_GROWS_UP               FALSE
#define CPU_STRUCTURE_ALIGNMENT

/* structures */

/*
 *  Basic integer context for the m68k family.
 */

typedef struct {
  unsigned32  sr;                /* (sr) status register */
  unsigned32  d2;                /* (d2) data register 2 */
  unsigned32  d3;                /* (d3) data register 3 */
  unsigned32  d4;                /* (d4) data register 4 */
  unsigned32  d5;                /* (d5) data register 5 */
  unsigned32  d6;                /* (d6) data register 6 */
  unsigned32  d7;                /* (d7) data register 7 */
  void       *a2;                /* (a2) address register 2 */
  void       *a3;                /* (a3) address register 3 */
  void       *a4;                /* (a4) address register 4 */
  void       *a5;                /* (a5) address register 5 */
  void       *a6;                /* (a6) address register 6 */
  void       *a7_msp;            /* (a7) master stack pointer */
}   Context_Control;

/*
 *  FP context save area for the M68881/M68882 numeric coprocessors.
 */

typedef struct {
  unsigned8   fp_save_area[332];    /*   216 bytes for FSAVE/FRESTORE    */
                                    /*    96 bytes for FMOVEM FP0-7      */
                                    /*    12 bytes for FMOVEM CREGS      */
                                    /*     4 bytes for non-null flag     */
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
 *  the m68k specific parameters.
 */

typedef struct {
  void       (*pretasking_hook)( void );
  void       (*predriver_hook)( void );
  void       (*postdriver_hook)( void );
  void       (*idle_task)( void );
  boolean      do_zero_of_workspace;
  unsigned32   interrupt_stack_size;
  unsigned32   extra_mpci_receive_server_stack;
  void *     (*stack_allocate_hook)( unsigned32 );
  void       (*stack_free_hook)( void* );
  /* end of fields required on all CPUs */

  m68k_isr    *interrupt_vector_table;
}   rtems_cpu_table;

/* variables */

SCORE_EXTERN void               *_CPU_Interrupt_stack_low;
SCORE_EXTERN void               *_CPU_Interrupt_stack_high;
      /* points to jsr-exception-table in targets wo/ VBR register */
extern char                      _VBR[]; 

/* constants */

/*
 *  This defines the number of levels and the mask used to pick those
 *  bits out of a thread mode.
 */

#define CPU_MODES_INTERRUPT_LEVEL  0x00000007 /* interrupt level in mode */
#define CPU_MODES_INTERRUPT_MASK   0x00000007 /* interrupt level in mode */

/*
 *  context size area for floating point
 */

#define CPU_CONTEXT_FP_SIZE sizeof( Context_Control_fp )

/*
 *  extra stack required by the MPCI receive server thread
 */

#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 1024

/*
 *  m68k family supports 256 distinct vectors.
 */

#define CPU_INTERRUPT_NUMBER_OF_VECTORS      256
#define CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER  (CPU_INTERRUPT_NUMBER_OF_VECTORS - 1)

/*
 *  Minimum size of a thread's stack.
 */

#define CPU_STACK_MINIMUM_SIZE           2048

/*
 *  m68k is pretty tolerant of alignment.  Just put things on 4 byte boundaries.
 */

#define CPU_ALIGNMENT                    4
#define CPU_HEAP_ALIGNMENT               CPU_ALIGNMENT
#define CPU_PARTITION_ALIGNMENT          CPU_ALIGNMENT

/*
 *  On m68k thread stacks require no further alignment after allocation
 *  from the Workspace.
 */

#define CPU_STACK_ALIGNMENT        0

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

#define _CPU_ISR_Disable( _level ) \
  m68k_disable_interrupts( _level )

#define _CPU_ISR_Enable( _level ) \
  m68k_enable_interrupts( _level )

#define _CPU_ISR_Flash( _level ) \
  m68k_flash_interrupts( _level )

#define _CPU_ISR_Set_level( _newlevel ) \
   m68k_set_interrupt_level( _newlevel )

unsigned32 _CPU_ISR_Get_level( void );

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

#define _CPU_Context_Initialize( _the_context, _stack_base, _size, \
                                 _isr, _entry_point, _is_fp ) \
   do { \
     void   *_stack; \
     \
     (_the_context)->sr      = 0x3000 | ((_isr) << 8); \
     _stack                  = (void *)(_stack_base) + (_size) - 4; \
     (_the_context)->a7_msp  = _stack; \
     *(void **)_stack = (_entry_point); \
   } while ( 0 )

#define _CPU_Context_Restart_self( _the_context ) \
  { asm volatile( "movew %0,%%sr ; " \
                  "moval %1,%%a7 ; " \
                  "rts"  \
        : "=d" ((_the_context)->sr), "=d" ((_the_context)->a7_msp) \
        : "0" ((_the_context)->sr), "1" ((_the_context)->a7_msp) ); \
  }

#define _CPU_Context_Fp_start( _base, _offset ) \
   ((void *) \
     _Addresses_Add_offset( \
        (_base), \
        (_offset) + CPU_CONTEXT_FP_SIZE - 4 \
     ) \
   )

#define _CPU_Context_Initialize_fp( _fp_area ) \
   { unsigned32 *_fp_context = (unsigned32 *)*(_fp_area); \
     \
     *(--(_fp_context)) = 0; \
     *(_fp_area) = (unsigned8 *)(_fp_context); \
   }

/* end of Context handler macros */

/*
 *  Fatal Error manager macros
 *
 *  These macros perform the following functions:
 *    + disable interrupts and halt the CPU
 */

#define _CPU_Fatal_halt( _error ) \
  { asm volatile( "movl  %0,%%d0; " \
                  "orw   #0x0700,%%sr; " \
                  "stop  #0x2700" : "=d" ((_error)) : "0" ((_error)) ); \
  }

/* end of Fatal Error manager macros */

/*
 *  Bitfield handler macros
 *
 *  These macros perform the following functions:
 *     + scan for the highest numbered (MSB) set in a 16 bit bitfield
 *
 *  NOTE:
 *
 *    It appears that on the M68020 bitfield are always 32 bits wide
 *    when in a register.  This code forces the bitfield to be in
 *    memory (it really always is anyway). This allows us to
 *    have a real 16 bit wide bitfield which operates "correctly."
 */

#define CPU_USE_GENERIC_BITFIELD_CODE FALSE
#define CPU_USE_GENERIC_BITFIELD_DATA FALSE

#if ( M68K_HAS_BFFFO == 1 )
#ifdef NO_UNINITIALIZED_WARNINGS

#define _CPU_Bitfield_Find_first_bit( _value, _output ) \
  { \
    register void *__base = (void *)&(_value); \
    \
    (_output) = 0;  /* avoids warnings */ \
    asm volatile( "bfffo (%0),#0,#16,%1" \
                   : "=a" (__base), "=d" ((_output)) \
                   : "0"  (__base), "1" ((_output))  ) ; \
  }
#else
#define _CPU_Bitfield_Find_first_bit( _value, _output ) \
  { \
    register void *__base = (void *)&(_value); \
    \
    asm volatile( "bfffo (%0),#0,#16,%1" \
                   : "=a" (__base), "=d" ((_output)) \
                   : "0"  (__base), "1" ((_output))  ) ; \
  }
#endif

#else

/* duplicates BFFFO results for 16 bits (i.e., 15-(_priority) in
   _CPU_Priority_bits_index is not needed), handles the 0 case, and
   does not molest _value -- jsg */
#if ( M68K_HAS_EXTB_L == 1 )
#define _CPU_Bitfield_Find_first_bit( _value, _output ) \
  { \
    extern const unsigned char __BFFFOtable[256]; \
    register int dumby; \
    \
    asm volatile ( "   move.w  %2,%1\n"        \
       "   lsr.w   #8,%1\n"        \
       "   beq.s   1f\n"           \
       "   move.b  (%3,%1.w),%0\n" \
       "   extb.l  %0\n"           \
       "   bra.s   0f\n"           \
       "1: moveq.l #8,%0\n"        \
       "   add.b   (%3,%2.w),%0\n" \
       "0:\n"                      \
       : "=&d" ((_output)), "=&d" ((dumby)) \
       : "d" ((_value)), "ao" ((__BFFFOtable)) \
       : "cc" ) ; \
  }
#else
#define _CPU_Bitfield_Find_first_bit( _value, _output ) \
  { \
    extern const unsigned char __BFFFOtable[256]; \
    register int dumby; \
    \
    asm volatile ( "   move.w  %2,%1\n"        \
       "   lsr.w   #8,%1\n"        \
       "   beq.s   1f\n"           \
       "   move.b  (%3,%1.w),%0\n" \
       "   and.l   #0x000000ff,%0\n"\
       "   bra.s   0f\n"           \
       "1: moveq.l #8,%0\n"        \
       "   add.b   (%3,%2.w),%0\n" \
       "0:\n"                      \
       : "=&d" ((_output)), "=&d" ((dumby)) \
       : "d" ((_value)), "ao" ((__BFFFOtable)) \
       : "cc" ) ; \
  }
#endif /* M68K_HAS_EXTB_L */

#endif

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
  ( 0x8000 >> (_bit_number) )

#define _CPU_Priority_bits_index( _priority ) \
  (_priority)

/* end of Priority handler macros */

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
 *  _CPU_ISR_install_raw_handler
 *
 *  This routine installs a "raw" interrupt handler directly into the 
 *  processor's vector table.
 */
 
void _CPU_ISR_install_raw_handler(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
);

/*
 *  _CPU_ISR_install_vector
 *
 *  This routine installs an interrupt vector.
 */

void _CPU_ISR_install_vector(
  unsigned32       vector,
  proc_ptr         new_handler,
  proc_ptr        *old_handler
);

/*
 *  _CPU_Install_interrupt_stack
 *
 *  This routine installs the hardware interrupt stack pointer.
 */

void _CPU_Install_interrupt_stack( void );

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
 *  _CPU_Context_save_fp
 *
 *  This routine saves the floating point context passed to it.
 */

void _CPU_Context_restore_fp(
  void **fp_context_ptr
);

/*
 *  _CPU_Context_restore_fp
 *
 *  This routine restores the floating point context passed to it.
 */

void _CPU_Context_save_fp(
  void **fp_context_ptr
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
