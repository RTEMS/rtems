/*  cpu.h
 *
 *  This include file contains information pertaining to the HP
 *  PA-RISC processor (Level 1.1).
 *
 *  COPYRIGHT (c) 1994 by Division Incorporated
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 * Note:
 *      This file is included by both C and assembler code ( -DASM )
 *
 *  $Id$
 */

#ifndef __CPU_h
#define __CPU_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/hppa.h>              /* pick up machine definitions */
#ifndef ASM
#include <rtems/score/types.h>
#endif

/* conditional compilation parameters */

#define CPU_INLINE_ENABLE_DISPATCH       FALSE
#define CPU_UNROLL_ENQUEUE_PRIORITY      TRUE

/*
 *  RTEMS manages an interrupt stack in software for the HPPA.
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
 *  HPPA has hardware FP, it is assumed to exist by GCC so all tasks
 *  may implicitly use it (especially for integer multiplies).  Because
 *  the FP context is technically part of the basic integer context
 *  on this CPU, we cannot use the deferred FP context switch algorithm.
 */

#define CPU_HARDWARE_FP                  TRUE
#define CPU_SOFTWARE_FP                  FALSE
#define CPU_ALL_TASKS_ARE_FP             TRUE
#define CPU_IDLE_TASK_IS_FP              FALSE
#define CPU_USE_DEFERRED_FP_SWITCH       FALSE

#define CPU_PROVIDES_IDLE_THREAD_BODY    FALSE
#define CPU_STACK_GROWS_UP               TRUE
#define CPU_STRUCTURE_ALIGNMENT          __attribute__ ((__aligned__ (32)))

/*
 *  Define what is required to specify how the network to host conversion
 *  routines are handled.
 */

#define CPU_HAS_OWN_HOST_TO_NETWORK_ROUTINES     FALSE
#define CPU_BIG_ENDIAN                           TRUE
#define CPU_LITTLE_ENDIAN                        FALSE

/* constants */

#define CPU_MODES_INTERRUPT_LEVEL  0x00000001 /* interrupt level in mode */
#define CPU_MODES_INTERRUPT_MASK   0x00000001 /* interrupt level in mode */

/*
 * PSW contstants
 */

#define CPU_PSW_BASE (HPPA_PSW_C | HPPA_PSW_Q | HPPA_PSW_P | HPPA_PSW_D)
#define CPU_PSW_INTERRUPTS_ON    (CPU_PSW_BASE | HPPA_PSW_I)
#define CPU_PSW_INTERRUPTS_OFF   (CPU_PSW_BASE)

#define CPU_PSW_DEFAULT     CPU_PSW_BASE


#ifndef ASM

/*
 * Contexts
 *
 *  This means we have the following context items:
 *    1. task level context stuff::  Context_Control
 *    2. floating point task stuff:: Context_Control_fp
 *
 *  The PA-RISC is very fast so the expense of saving an extra register
 *  or two is not of great concern at the present.  So we are not making
 *  a distinction between what is saved during a task switch and what is
 *  saved at each interrupt.  Plus saving the entire context should make
 *  it easier to make gdb aware of RTEMS tasks.
 */

typedef struct {
    unsigned32 flags;      /* whatever */
    unsigned32 gr1;        /* scratch -- caller saves */
    unsigned32 gr2;        /* RP -- return pointer */
    unsigned32 gr3;        /* scratch -- callee saves */
    unsigned32 gr4;        /* scratch -- callee saves */
    unsigned32 gr5;        /* scratch -- callee saves */
    unsigned32 gr6;        /* scratch -- callee saves */
    unsigned32 gr7;        /* scratch -- callee saves */
    unsigned32 gr8;        /* scratch -- callee saves */
    unsigned32 gr9;        /* scratch -- callee saves */
    unsigned32 gr10;       /* scratch -- callee saves */
    unsigned32 gr11;       /* scratch -- callee saves */
    unsigned32 gr12;       /* scratch -- callee saves */
    unsigned32 gr13;       /* scratch -- callee saves */
    unsigned32 gr14;       /* scratch -- callee saves */
    unsigned32 gr15;       /* scratch -- callee saves */
    unsigned32 gr16;       /* scratch -- callee saves */
    unsigned32 gr17;       /* scratch -- callee saves */
    unsigned32 gr18;       /* scratch -- callee saves */
    unsigned32 gr19;       /* scratch -- caller saves */
    unsigned32 gr20;       /* scratch -- caller saves */
    unsigned32 gr21;       /* scratch -- caller saves */
    unsigned32 gr22;       /* scratch -- caller saves */
    unsigned32 gr23;       /* argument 3 */
    unsigned32 gr24;       /* argument 2 */
    unsigned32 gr25;       /* argument 1 */
    unsigned32 gr26;       /* argument 0 */
    unsigned32 gr27;       /* DP -- global data pointer */
    unsigned32 gr28;       /* return values -- caller saves */
    unsigned32 gr29;       /* return values -- caller saves */
    unsigned32 sp;         /* gr30 */
    unsigned32 gr31;

    /* Various control registers */

    unsigned32 sar;         /* cr11 */
    unsigned32 ipsw;        /* cr22; full 32 bits of psw */
    unsigned32 iir;         /* cr19; interrupt instruction register */
    unsigned32 ior;         /* cr21; interrupt offset register */
    unsigned32 isr;         /* cr20; interrupt space register (not used) */
    unsigned32 pcoqfront;   /* cr18; front que offset */
    unsigned32 pcoqback;    /* cr18; back que offset */
    unsigned32 pcsqfront;   /* cr17; front que space (not used) */
    unsigned32 pcsqback;    /* cr17; back que space (not used) */
    unsigned32 itimer;      /* cr16; itimer value */

} Context_Control;


/* Must be double word aligned.
 * This will be ok since our allocator returns 8 byte aligned chunks
 */

typedef struct {
    double      fr0;        /* status */
    double      fr1;        /* exception information */
    double      fr2;        /* exception information */
    double      fr3;        /* exception information */
    double      fr4;        /* argument */
    double      fr5;        /* argument */
    double      fr6;        /* argument */
    double      fr7;        /* argument */
    double      fr8;        /* scratch -- caller saves */
    double      fr9;        /* scratch -- caller saves */
    double      fr10;       /* scratch -- caller saves */
    double      fr11;       /* scratch -- caller saves */
    double      fr12;       /* callee saves -- (PA-RISC 1.1 CPUs) */
    double      fr13;       /* callee saves -- (PA-RISC 1.1 CPUs) */
    double      fr14;       /* callee saves -- (PA-RISC 1.1 CPUs) */
    double      fr15;       /* callee saves -- (PA-RISC 1.1 CPUs) */
    double      fr16;       /* callee saves -- (PA-RISC 1.1 CPUs) */
    double      fr17;       /* callee saves -- (PA-RISC 1.1 CPUs) */
    double      fr18;       /* callee saves -- (PA-RISC 1.1 CPUs) */
    double      fr19;       /* callee saves -- (PA-RISC 1.1 CPUs) */
    double      fr20;       /* callee saves -- (PA-RISC 1.1 CPUs) */
    double      fr21;       /* callee saves -- (PA-RISC 1.1 CPUs) */
    double      fr22;       /* caller saves -- (PA-RISC 1.1 CPUs) */
    double      fr23;       /* caller saves -- (PA-RISC 1.1 CPUs) */
    double      fr24;       /* caller saves -- (PA-RISC 1.1 CPUs) */
    double      fr25;       /* caller saves -- (PA-RISC 1.1 CPUs) */
    double      fr26;       /* caller saves -- (PA-RISC 1.1 CPUs) */
    double      fr27;       /* caller saves -- (PA-RISC 1.1 CPUs) */
    double      fr28;       /* caller saves -- (PA-RISC 1.1 CPUs) */
    double      fr29;       /* caller saves -- (PA-RISC 1.1 CPUs) */
    double      fr30;       /* caller saves -- (PA-RISC 1.1 CPUs) */
    double      fr31;       /* caller saves -- (PA-RISC 1.1 CPUs) */
} Context_Control_fp;

/*
 *  The following structure defines the set of information saved
 *  on the current stack by RTEMS upon receipt of each interrupt.
 */

typedef struct {
  Context_Control             Integer;
  Context_Control_fp          Floating_Point;
} CPU_Interrupt_frame;

/*
 * Our interrupt handlers take a 2nd argument:
 *   a pointer to a CPU_Interrupt_frame
 * So we use our own prototype instead of rtems_isr_entry
 */

typedef void ( *hppa_rtems_isr_entry )(
    unsigned32,
    CPU_Interrupt_frame *
 );

/*
 * The following table contains the information required to configure
 * the HPPA specific parameters.
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
  void       (*stack_free_hook)( void * );
  /* end of fields required on all CPUs */

  hppa_rtems_isr_entry spurious_handler;

  unsigned32   itimer_clicks_per_microsecond; /* for use by Clock driver */
}   rtems_cpu_table;

/*
 *  Macros to access required entires in the CPU Table are in 
 *  the file rtems/system.h.
 */

/*
 *  Macros to access HPPA specific additions to the CPU Table
 */

#define rtems_cpu_configuration_get_spurious_handler() \
   (_CPU_Table.spurious_handler)

#define rtems_cpu_configuration_get_itimer_clicks_per_microsecond() \
   (_CPU_Table.itimer_clicks_per_microsecond)

/* variables */

SCORE_EXTERN Context_Control_fp  _CPU_Null_fp_context;
SCORE_EXTERN unsigned32          _CPU_Default_gr27;
SCORE_EXTERN void               *_CPU_Interrupt_stack_low;
SCORE_EXTERN void               *_CPU_Interrupt_stack_high;

#endif          /* ! ASM */

/*
 *  context sizes
 */

#ifndef ASM
#define CPU_CONTEXT_SIZE     sizeof( Context_Control )
#define CPU_CONTEXT_FP_SIZE  sizeof( Context_Control_fp )
#endif

/*
 *  size of a frame on the stack
 */

#define CPU_FRAME_SIZE (16 * 4)

/*
 * (Optional) # of bytes for libmisc/stackchk to check
 * If not specifed, then it defaults to something reasonable
 * for most architectures.
 */

#define CPU_STACK_CHECK_SIZE    (CPU_FRAME_SIZE * 2)

/*
 *  extra stack required by the MPCI receive server thread
 */

#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 0

/*
 * HPPA has 32 traps, then 32 external interrupts
 * Rtems (_ISR_Vector_Table) is aware ONLY of the first 32
 * The BSP is aware of the external interrupts and possibly more.
 *
 */

#define CPU_INTERRUPT_NUMBER_OF_VECTORS      (HPPA_INTERNAL_TRAPS)
#define CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER  (CPU_INTERRUPT_NUMBER_OF_VECTORS - 1)

/*
 *  This is defined if the port has a special way to report the ISR nesting
 *  level.  Most ports maintain the variable _ISR_Nest_level.
 */

#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE

/*
 * Don't be chintzy here; we don't want to debug these problems
 * Some of the tests eat almost 4k.
 * Plus, the HPPA always allocates chunks of 64 bytes for stack
 *       growth.
 */

#define CPU_STACK_MINIMUM_SIZE          (8 * 1024)

/*
 * HPPA double's must be on 8 byte boundary
 */

#define CPU_ALIGNMENT              8

/*
 * just follow the basic HPPA alignment for the heap and partition
 */

#define CPU_HEAP_ALIGNMENT         CPU_ALIGNMENT
#define CPU_PARTITION_ALIGNMENT    CPU_ALIGNMENT

/*
 * HPPA stack is best when 64 byte aligned.
 */

#define CPU_STACK_ALIGNMENT        64

#ifndef ASM

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

/*
 *  Support routine to initialize the RTEMS vector table after it is allocated.
 */

#define _CPU_Initialize_vectors()

/* Disable interrupts; returning previous psw bits in _isr_level */

#define _CPU_ISR_Disable( _isr_level ) \
  do { \
         HPPA_ASM_RSM(HPPA_PSW_I, _isr_level);         \
         if (_isr_level & HPPA_PSW_I) _isr_level = 0;  \
         else                          _isr_level = 1; \
  } while(0)

/* Enable interrupts to previous level from _CPU_ISR_Disable
 * does not change 'level'
 */

#define _CPU_ISR_Enable( _isr_level )  \
  { \
        register int _ignore; \
        if (_isr_level == 0) HPPA_ASM_SSM(HPPA_PSW_I, _ignore); \
        else                 HPPA_ASM_RSM(HPPA_PSW_I, _ignore); \
  }

/* restore, then disable interrupts; does not change level */
#define _CPU_ISR_Flash( _isr_level ) \
  { \
        if (_isr_level == 0) \
        { \
              register int _ignore;  \
              HPPA_ASM_SSM(HPPA_PSW_I, _ignore); \
              HPPA_ASM_RSM(HPPA_PSW_I, _ignore); \
        } \
  }

/*
 * Interrupt task levels
 *
 * Future scheme proposal
 *      level will be an index into a array.
 *      Each entry of array will be the interrupt bits
 *        enabled for that level.  There will be 32 bits of external
 *        interrupts (to be placed in EIEM) and some (optional) bsp
 *        specific bits
 *
 * For pixel flow this *may* mean something like:
 *      level 0:   all interrupts enabled (external + rhino)
 *      level 1:   rhino disabled
 *      level 2:   all io interrupts disabled (timer still enabled)
 *      level 7:   *ALL* disabled (timer disabled)
 */

/* set interrupts on or off; does not return new level */
#define _CPU_ISR_Set_level( new_level ) \
  { \
        volatile int ignore; \
        if ( new_level )  HPPA_ASM_RSM(HPPA_PSW_I, ignore); \
        else              HPPA_ASM_SSM(HPPA_PSW_I, ignore); \
  }

/* return current level */
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
 *
 *  HPPA port adds two macros which hide the "indirectness" of the
 *  pointer passed the save/restore FP context assembly routines.
 */

#define _CPU_Context_Initialize( _the_context, _stack_base, _size, \
                                  _new_level, _entry_point, _is_fp ) \
  do { \
    unsigned32 _stack; \
    \
    (_the_context)->flags = 0xfeedf00d; \
    (_the_context)->pcoqfront = (unsigned32)(_entry_point); \
    (_the_context)->pcoqback  = (unsigned32)(_entry_point) + 4; \
    (_the_context)->pcsqfront = 0; \
    (_the_context)->pcsqback  = 0; \
    if ( (_new_level) ) \
        (_the_context)->ipsw = CPU_PSW_INTERRUPTS_OFF; \
    else \
        (_the_context)->ipsw = CPU_PSW_INTERRUPTS_ON; \
    \
    _stack = ((unsigned32)(_stack_base) + (CPU_STACK_ALIGNMENT - 1)); \
    _stack &= ~(CPU_STACK_ALIGNMENT - 1); \
    if ((_stack - (unsigned32) (_stack_base)) < CPU_FRAME_SIZE) \
       _stack += CPU_FRAME_SIZE; \
    \
    (_the_context)->sp = (_stack); \
    (_the_context)->gr27 = _CPU_Default_gr27; \
  } while (0)

#define _CPU_Context_Restart_self( _the_context ) \
    do { \
         _CPU_Context_restore( (_the_context) ); \
    } while (0)

#define _CPU_Context_Fp_start( _base, _offset ) \
   ( (void *) _Addresses_Add_offset( (_base), (_offset) ) )

#define _CPU_Context_Initialize_fp( _destination ) \
  do { \
    *((Context_Control_fp *) *((void **) _destination)) = _CPU_Null_fp_context;\
  } while(0)

#define _CPU_Context_save_fp( _fp_context ) \
   _CPU_Save_float_context( *(Context_Control_fp **)(_fp_context) )

#define _CPU_Context_restore_fp( _fp_context ) \
   _CPU_Restore_float_context( *(Context_Control_fp **)(_fp_context) )

/* end of Context handler macros */

/*
 *  Fatal Error manager macros
 *
 *  These macros perform the following functions:
 *    + disable interrupts and halt the CPU
 */

void    hppa_cpu_halt(unsigned32 the_error);
#define _CPU_Fatal_halt( _error ) \
    hppa_cpu_halt(_error)

/* end of Fatal Error manager macros */

/*
 *  Bitfield handler macros
 *
 *  These macros perform the following functions:
 *     + scan for the highest numbered (MSB) set in a 16 bit bitfield
 *
 *  NOTE:
 *
 *  The HPPA does not have a scan instruction.  This functionality
 *  is implemented in software.
 */

#define CPU_USE_GENERIC_BITFIELD_CODE FALSE
#define CPU_USE_GENERIC_BITFIELD_DATA FALSE

int hppa_rtems_ffs(unsigned int value);
#define _CPU_Bitfield_Find_first_bit( _value, _output ) \
    _output = hppa_rtems_ffs(_value)

/* end of Bitfield handler macros */

/*
 *  Priority handler macros
 *
 *  These macros perform the following functions:
 *    + return a mask with the bit for this major/minor portion of
 *      of thread priority set.
 *    + translate the bit number returned by "Bitfield_find_first_bit"
 *      into an index into the thread ready chain bit maps
 *
 *  Note: 255 is the lowest priority
 */

#define _CPU_Priority_Mask( _bit_number ) \
  ( 1 << (_bit_number) )

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
 *  This routine is generally used only to restart self in an
 *  efficient manner and avoid stack conflicts.
 */

void _CPU_Context_restore(
  Context_Control *new_context
);

/*
 *  _CPU_Save_float_context
 *
 *  This routine saves the floating point context passed to it.
 *
 *  NOTE:  _CPU_Context_save_fp is implemented as a macro on the HPPA
 *         which dereferences the pointer before calling this.
 */

void _CPU_Save_float_context(
  Context_Control_fp *fp_context
);

/*
 *  _CPU_Restore_float_context
 *
 *  This routine restores the floating point context passed to it.
 *
 *  NOTE:  _CPU_Context_save_fp is implemented as a macro on the HPPA
 *         which dereferences the pointer before calling this.
 */

void _CPU_Restore_float_context(
  Context_Control_fp *fp_context
);


/*
 * The raw interrupt handler for external interrupts
 */

extern void _Generic_ISR_Handler(
    void
);


/*  The following routine swaps the endian format of an unsigned int.
 *  It must be static so it can be referenced indirectly.
 */

static inline unsigned int
CPU_swap_u32(unsigned32 value)
{
  unsigned32 swapped;

  HPPA_ASM_SWAPBYTES(value, swapped);

  return( swapped );
}

#define CPU_swap_u16( value ) \
  (((value&0xff) << 8) | ((value >> 8)&0xff))

#endif   /* ! ASM */

#ifdef __cplusplus
}
#endif

#endif   /* ! __CPU_h */
