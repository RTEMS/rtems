/*  cpu.h
 *
 *  This include file contains information pertaining to the Intel
 *  i960 processor family.
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

#ifndef __CPU_h
#define __CPU_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/i960.h>              /* pick up machine definitions */
#ifndef ASM
#include <rtems/score/i960types.h>
#endif

#define CPU_INLINE_ENABLE_DISPATCH       FALSE
#define CPU_UNROLL_ENQUEUE_PRIORITY      FALSE

/*
 *  Use the i960's hardware interrupt stack support and have the
 *  interrupt manager allocate the memory for it.
 */

#define CPU_HAS_SOFTWARE_INTERRUPT_STACK FALSE
#define CPU_HAS_HARDWARE_INTERRUPT_STACK TRUE
#define CPU_ALLOCATE_INTERRUPT_STACK     TRUE

/*
 *  Does the RTEMS invoke the user's ISR with the vector number and
 *  a pointer to the saved interrupt frame (1) or just the vector 
 *  number (0)?
 */

#define CPU_ISR_PASSES_FRAME_POINTER 0

/*
 *  Some family members have no FP (SA/KA/CA/CF), others have it built in
 *  (KB/MC/MX).  There does not appear to be an external coprocessor
 *  for this family.
 */

#if ( I960_HAS_FPU == 1 )
#define CPU_HARDWARE_FP     TRUE
#error "Floating point support for i960 family has been implemented!!!"
#else
#define CPU_HARDWARE_FP     FALSE
#endif

#define CPU_SOFTWARE_FP     FALSE

#define CPU_ALL_TASKS_ARE_FP             FALSE
#define CPU_IDLE_TASK_IS_FP              FALSE
#define CPU_USE_DEFERRED_FP_SWITCH       TRUE

#define CPU_PROVIDES_IDLE_THREAD_BODY    FALSE
#define CPU_STACK_GROWS_UP               TRUE
#define CPU_STRUCTURE_ALIGNMENT          /* __attribute__ ((aligned (16))) */

/*
 *  Define what is required to specify how the network to host conversion
 *  routines are handled.
 */

#define CPU_HAS_OWN_HOST_TO_NETWORK_ROUTINES     FALSE
#define CPU_BIG_ENDIAN                           TRUE
#define CPU_LITTLE_ENDIAN                        FALSE


/* structures */

/*
 *  Basic integer context for the i960 family.
 */

typedef struct {
  void       *r0_pfp;                 /* (r0)  Previous Frame Pointer */
  void       *r1_sp;                  /* (r1)  Stack Pointer */
  unsigned32  pc;                     /* (pc)  Processor Control */
  void       *g8;                     /* (g8)  Global Register 8 */
  void       *g9;                     /* (g9)  Global Register 9 */
  void       *g10;                    /* (g10) Global Register 10 */
  void       *g11;                    /* (g11) Global Register 11 */
  void       *g12;                    /* (g12) Global Register 12 */
  void       *g13;                    /* (g13) Global Register 13 */
  unsigned32  g14;                    /* (g14) Global Register 14 */
  void       *g15_fp;                 /* (g15) Frame Pointer */
}   Context_Control;

/*
 *  FP context save area for the i960 Numeric Extension
 */

typedef struct {
   unsigned32  fp0_1;                 /* (fp0) first word  */
   unsigned32  fp0_2;                 /* (fp0) second word */
   unsigned32  fp0_3;                 /* (fp0) third word  */
   unsigned32  fp1_1;                 /* (fp1) first word  */
   unsigned32  fp1_2;                 /* (fp1) second word */
   unsigned32  fp1_3;                 /* (fp1) third word  */
   unsigned32  fp2_1;                 /* (fp2) first word  */
   unsigned32  fp2_2;                 /* (fp2) second word */
   unsigned32  fp2_3;                 /* (fp2) third word  */
   unsigned32  fp3_1;                 /* (fp3) first word  */
   unsigned32  fp3_2;                 /* (fp3) second word */
   unsigned32  fp3_3;                 /* (fp3) third word  */
} Context_Control_fp;

/*
 *  The following structure defines the set of information saved
 *  on the current stack by RTEMS upon receipt of each interrupt.
 */

typedef struct {
  unsigned32   TBD;   /* XXX Fix for this CPU */
} CPU_Interrupt_frame;

/*
 *  Call frame for the i960 family.
 */

typedef struct {
  void       *r0_pfp;                 /* (r0)  Previous Frame Pointer */
  void       *r1_sp;                  /* (r1)  Stack Pointer */
  void       *r2_rip;                 /* (r2)  Return Instruction Pointer */
  void       *r3;                     /* (r3)  Local Register 3 */
  void       *r4;                     /* (r4)  Local Register 4 */
  void       *r5;                     /* (r5)  Local Register 5 */
  void       *r6;                     /* (r6)  Local Register 6 */
  void       *r7;                     /* (r7)  Local Register 7 */
  void       *r8;                     /* (r8)  Local Register 8 */
  void       *r9;                     /* (r9)  Local Register 9 */
  void       *r10;                    /* (r10) Local Register 10 */
  void       *r11;                    /* (r11) Local Register 11 */
  void       *r12;                    /* (r12) Local Register 12 */
  void       *r13;                    /* (r13) Local Register 13 */
  void       *r14;                    /* (r14) Local Register 14 */
  void       *r15;                    /* (r15) Local Register 15 */
  /* XXX Looks like sometimes there is FP stuff here (MC manual)? */
}   CPU_Call_frame;

/*
 *  The following table contains the information required to configure
 *  the i960 specific parameters.
 */

typedef struct {
  void       (*pretasking_hook)( void );
  void       (*predriver_hook)( void );
  void       (*postdriver_hook)( void );
  void       (*idle_task)( void );
  boolean      do_zero_of_workspace;
  unsigned32   idle_task_stack_size;
  unsigned32   interrupt_stack_size;
  unsigned32   extra_mpci_receive_server_stack;
  void *     (*stack_allocate_hook)( unsigned32 );
  void       (*stack_free_hook)( void* );
  /* end of fields required on all CPUs */
}   rtems_cpu_table;

/*
 *  Macros to access required entires in the CPU Table are in 
 *  the file rtems/system.h.
 */

/*
 *  Macros to access i960 specific additions to the CPU Table
 *
 *  NONE
 */

/* variables */

SCORE_EXTERN void               *_CPU_Interrupt_stack_low;
SCORE_EXTERN void               *_CPU_Interrupt_stack_high;

/* constants */

/*
 *  This defines the number of levels and the mask used to pick those
 *  bits out of a thread mode.
 */

#define CPU_MODES_INTERRUPT_LEVEL  0x0000001f  /* interrupt level in mode */
#define CPU_MODES_INTERRUPT_MASK   0x0000001f  /* interrupt level in mode */

/*
 *  context size area for floating point
 */

#define CPU_CONTEXT_FP_SIZE sizeof( Context_Control_fp )

/*
 *  extra stack required by the MPCI receive server thread
 */

#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK (CPU_STACK_MINIMUM_SIZE)

/*
 *  i960 family supports 256 distinct vectors.
 */

#define CPU_INTERRUPT_NUMBER_OF_VECTORS      256
#define CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER  (CPU_INTERRUPT_NUMBER_OF_VECTORS - 1)

/*
 *  Minimum size of a thread's stack.
 *
 *  NOTE:  See CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK
 */

#define CPU_STACK_MINIMUM_SIZE          2048

/*
 *  i960 is pretty tolerant of alignment but some CPU models do
 *  better with different default aligments so we use what the
 *  CPU model selected in rtems/score/i960.h.
 */

#define CPU_ALIGNMENT                   I960_CPU_ALIGNMENT
#define CPU_HEAP_ALIGNMENT              CPU_ALIGNMENT
#define CPU_PARTITION_ALIGNMENT         CPU_ALIGNMENT

/*
 * i960ca stack requires 16 byte alignment
 *
 *  NOTE:  This factor may need to be family member dependent.
 */

#define CPU_STACK_ALIGNMENT        16

/* macros */

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

#define _CPU_Initialize_vectors()
#define _CPU_ISR_Disable( _level ) i960_disable_interrupts( _level )
#define _CPU_ISR_Enable( _level )  i960_enable_interrupts( _level )
#define _CPU_ISR_Flash( _level )   i960_flash_interrupts( _level )

#define _CPU_ISR_Set_level( newlevel ) \
  { \
    unsigned32 _mask = 0; \
    unsigned32 _level = (newlevel); \
    \
    __asm__ volatile ( "ldconst 0x1f0000,%0; \
                    modpc   0,%0,%1"     : "=d" (_mask), "=d" (_level) \
                                         : "0"  (_mask), "1" (_level) \
    ); \
  }

unsigned32 _CPU_ISR_Get_level( void );

/* ISR handler section macros */

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
                                  _isr, _entry, _is_fp ) \
 { CPU_Call_frame *_texit_frame; \
   unsigned32 _mask; \
   unsigned32 _base_pc; \
   unsigned32  _stack_tmp; \
   void       *_stack; \
   \
  _stack_tmp = (unsigned32)(_stack_base) + CPU_STACK_ALIGNMENT; \
  _stack_tmp &= ~(CPU_STACK_ALIGNMENT - 1); \
  _stack = (void *) _stack_tmp; \
   \
   __asm__ volatile ( "flushreg" : : );   /* flush register cache */ \
   \
   (_the_context)->r0_pfp = _stack; \
   (_the_context)->g15_fp = _stack + (1 * sizeof(CPU_Call_frame)); \
   (_the_context)->r1_sp  = _stack + (2 * sizeof(CPU_Call_frame)); \
   __asm__ volatile ( "ldconst 0x1f0000,%0 ; " \
                  "modpc   0,0,%1 ; " \
                  "andnot  %0,%1,%1 ; " \
                  : "=d" (_mask), "=d" (_base_pc) : ); \
   (_the_context)->pc     = _base_pc | ((_isr) << 16); \
   (_the_context)->g14    = 0; \
   \
   _texit_frame         = (CPU_Call_frame *)_stack; \
   _texit_frame->r0_pfp = NULL; \
   _texit_frame->r1_sp  = (_the_context)->g15_fp; \
   _texit_frame->r2_rip = (_entry); \
 }

#define _CPU_Context_Restart_self( _the_context ) \
   _CPU_Context_restore( (_the_context) );

#define _CPU_Context_Fp_start( _base, _offset )         NULL

#define _CPU_Context_Initialize_fp( _fp_area )

/* end of Context handler macros */

/*
 *  Fatal Error manager macros
 *
 *  These macros perform the following functions:
 *    + disable interrupts and halt the CPU
 */

#define _CPU_Fatal_halt( _errorcode ) \
  { unsigned32 _mask, _level; \
    unsigned32 _error = (_errorcode); \
    \
    __asm__ volatile ( "ldconst 0x1f0000,%0 ; \
                    mov     %0,%1 ; \
                    modpc   0,%0,%1 ; \
                    mov     %2,g0 ; \
            self:   b       self " \
                    : "=d" (_mask), "=d" (_level), "=d" (_error) : ); \
  }

/* end of Fatal Error Manager macros */

/*
 *  Bitfield handler macros
 *
 *  These macros perform the following functions:
 *     + scan for the highest numbered (MSB) set in a 16 bit bitfield
 */

#define CPU_USE_GENERIC_BITFIELD_CODE FALSE
#define CPU_USE_GENERIC_BITFIELD_DATA FALSE

#define _CPU_Bitfield_Find_first_bit( _value, _output ) \
  { unsigned32 _search = (_value); \
    \
    (_output) = 0; /* to prevent warnings */ \
    __asm__ volatile ( "scanbit   %0,%1  " \
                    : "=d" (_search), "=d" (_output) \
                    : "0"  (_search), "1"  (_output) ); \
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
   ( 0x8000 >> (_bit_number) )

#define _CPU_Priority_bits_index( _priority ) \
   ( 15 - (_priority) )

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
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
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
 *  _CPU_Context_restore
 *
 *  This routine is generally used only to restart self in an
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
  void        **fp_context_ptr
);

/*
 *  _CPU_Context_restore_fp
 *
 *  This routine restores the floating point context passed to it.
 */

void _CPU_Context_restore_fp(
  void        **fp_context_ptr
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
