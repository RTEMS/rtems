/**
 * @file
 * 
 * @brief Motorola M68K CPU Dependent Source
 * 
 * @addtogroup RTEMSScoreCPUm68k
 * 
 * This include file contains information pertaining to the Motorola
 * m68xxx processor family.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CPU_H
#define _RTEMS_SCORE_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/basedefs.h>
#include <rtems/score/m68k.h>

/* conditional compilation parameters */

/*
 *  Does the CPU follow the simple vectored interrupt model?
 *
 *  If TRUE, then RTEMS allocates the vector table it internally manages.
 *  If FALSE, then the BSP is assumed to allocate and manage the vector
 *  table
 *
 *  M68K Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */
#define CPU_SIMPLE_VECTORED_INTERRUPTS TRUE

/*
 *  Does the RTEMS invoke the user's ISR with the vector number and
 *  a pointer to the saved interrupt frame (1) or just the vector
 *  number (0)?
 */

#define CPU_ISR_PASSES_FRAME_POINTER FALSE

/*
 *  Some family members have no FP, some have an FPU such as the
 *  MC68881/MC68882 for the MC68020, others have it built in (MC68030, 040).
 *
 *  NOTE:  If on a CPU without hardware FP, then one can use software
 *         emulation.  The gcc software FP emulation code has data which
 *         must be contexted switched on a per task basis.
 */

#if ( M68K_HAS_FPU == 1 ) || ( M68K_HAS_EMAC == 1 )
  #define CPU_HARDWARE_FP TRUE
  #define CPU_SOFTWARE_FP FALSE
#else
  #define CPU_HARDWARE_FP FALSE
  #if defined( __GNUC__ )
    #define CPU_SOFTWARE_FP TRUE
  #else
    #define CPU_SOFTWARE_FP FALSE
  #endif
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
#define CPU_ENABLE_ROBUST_THREAD_DISPATCH FALSE

#define CPU_STACK_GROWS_UP               FALSE

/* FIXME: Is this the right value? */
#define CPU_CACHE_LINE_BYTES 16

#define CPU_STRUCTURE_ALIGNMENT RTEMS_ALIGNED( CPU_CACHE_LINE_BYTES )

#define CPU_MAXIMUM_PROCESSORS 32

#if ( CPU_HARDWARE_FP == TRUE ) && !defined( __mcoldfire__ )
  #if defined( __mc68060__ )
    #define M68K_FP_STATE_SIZE 16
  #else
    #define M68K_FP_STATE_SIZE 216
  #endif
#endif

#ifndef ASM

/* structures */

/*
 *  Basic integer context for the m68k family.
 */

typedef struct {
  uint32_t    sr;                /* (sr) status register */
  uint32_t    d2;                /* (d2) data register 2 */
  uint32_t    d3;                /* (d3) data register 3 */
  uint32_t    d4;                /* (d4) data register 4 */
  uint32_t    d5;                /* (d5) data register 5 */
  uint32_t    d6;                /* (d6) data register 6 */
  uint32_t    d7;                /* (d7) data register 7 */
  void       *a2;                /* (a2) address register 2 */
  void       *a3;                /* (a3) address register 3 */
  void       *a4;                /* (a4) address register 4 */
  void       *a5;                /* (a5) address register 5 */
  void       *a6;                /* (a6) address register 6 */
  void       *a7_msp;            /* (a7) master stack pointer */
  #if defined( __mcoldfire__ ) && ( M68K_HAS_FPU == 1 )
    uint8_t   fpu_dis;
  #endif
} Context_Control;

#define _CPU_Context_Get_SP( _context ) \
  (_context)->a7_msp

/*
 *  Floating point context areas and support routines
 */

#if ( CPU_SOFTWARE_FP == TRUE )
  /*
   *  This is the same as gcc's view of the software FP condition code
   *  register _fpCCR.  The implementation of the emulation code is
   *  in the gcc-VERSION/config/m68k directory.  This structure is
   *  correct as of gcc 2.7.2.2.
   */
  typedef struct {
    uint16_t _exception_bits;
    uint16_t _trap_enable_bits;
    uint16_t _sticky_bits;
    uint16_t _rounding_mode;
    uint16_t _format;
    uint16_t _last_operation;
    union {
      float sf;
      double df;
    } _operand1;
    union {
      float sf;
      double df;
    } _operand2;
  } Context_Control_fp;

  #define _CPU_Context_Initialize_fp( _fp_area ) \
     { \
       Context_Control_fp *_fp; \
       _fp = *(Context_Control_fp **)_fp_area; \
       _fp->_exception_bits = 0; \
       _fp->_trap_enable_bits = 0; \
       _fp->_sticky_bits = 0; \
       _fp->_rounding_mode = 0;  /* ROUND_TO_NEAREST */ \
       _fp->_format = 0;         /* NIL */ \
       _fp->_last_operation = 0; /* NOOP */ \
       _fp->_operand1.df = 0; \
       _fp->_operand2.df = 0; \
     }
#endif

#if ( CPU_HARDWARE_FP == TRUE )
  #if defined( __mcoldfire__ )
    /* We need memset() to initialize the FP context */
    #include <string.h>

    #if ( M68K_HAS_FPU == 1 )
      /*
       * The Cache Control Register (CACR) has write-only access.  It is also
       * used to enable and disable the FPU.  We need to maintain a copy of
       * this register to allow per thread values.
       */
      extern uint32_t _CPU_cacr_shadow;
    #endif

    /* We assume that each ColdFire core with a FPU has also an EMAC unit */
    typedef struct {
      uint32_t emac_macsr;
      uint32_t emac_acc0;
      uint32_t emac_acc1;
      uint32_t emac_acc2;
      uint32_t emac_acc3;
      uint32_t emac_accext01;
      uint32_t emac_accext23;
      uint32_t emac_mask;
      #if ( M68K_HAS_FPU == 1 )
        uint16_t fp_state_format;
        uint16_t fp_state_fpcr;
        double fp_state_op;
        uint32_t fp_state_fpsr;

        /*
         * We need to save the FP Instruction Address Register (FPIAR), because
         * a context switch can occur within a FP exception before the handler
         * was able to save this register.
         */
        uint32_t fp_fpiar;

        double fp_data [8];
      #endif
    } Context_Control_fp;

    /*
     * The reset value for all context relevant registers except the FP data
     * registers is zero.  The reset value of the FP data register is NAN.  The
     * restore of the reset FP state will reset the FP data registers, so the
     * initial value of them can be arbitrary here.
     */
    #define _CPU_Context_Initialize_fp( _fp_area ) \
      memset( *(_fp_area), 0, sizeof( Context_Control_fp ) )
  #else
    /*
     *  FP context save area for the M68881/M68882 and 68060 numeric
     *  coprocessors.
     */
    typedef struct {
      /*
       * M68K_FP_STATE_SIZE bytes for FSAVE/FRESTORE
       * 96 bytes for FMOVEM FP0-7
       * 12 bytes for FMOVEM CREGS
       * 4 bytes for non-null flag
       */
      uint8_t fp_save_area [M68K_FP_STATE_SIZE + 112];
    } Context_Control_fp;

    /*
     * The floating-point context is saved/restored via FSAVE/FRESTORE which
     * use a growing down stack.  Initialize the stack and adjust the FP area
     * pointer accordingly.
     */
    #define _CPU_Context_Initialize_fp( _fp_area ) \
       { \
         uint32_t *_fp_context = (uint32_t *) \
           ( (uintptr_t) *( _fp_area ) + CPU_CONTEXT_FP_SIZE - 4 ); \
         *(--(_fp_context)) = 0; \
         *(_fp_area) = (void *)(_fp_context); \
       }
  #endif
#endif

/*
 *  The following structures define the set of information saved
 *  on the current stack by RTEMS upon receipt of each exc/interrupt.
 *  These are not used by m68k handlers.
 *  The exception frame is for rdbg.
 */

typedef struct {
  uint32_t   vecnum; /* vector number */
} CPU_Interrupt_frame;

typedef struct {
  uint32_t   vecnum; /* vector number */
  uint32_t   sr; /* status register */
  uint32_t   pc; /* program counter */
  uint32_t   d0, d1, d2, d3, d4, d5, d6, d7;
  uint32_t   a0, a1, a2, a3, a4, a5, a6, a7;
} CPU_Exception_frame;

/* variables */

extern void*                     _VBR;

#endif /* ASM */

/* constants */

/*
 *  This defines the number of levels and the mask used to pick those
 *  bits out of a thread mode.
 */

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
 *  This is defined if the port has a special way to report the ISR nesting
 *  level.  Most ports maintain the variable _ISR_Nest_level.
 */

#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE

/*
 *  Minimum size of a thread's stack.
 */

#define CPU_STACK_MINIMUM_SIZE           M68K_CPU_STACK_MINIMUM_SIZE

/*
 *  Maximum priority of a thread. Note based from 0 which is the idle task.
 */
#define CPU_PRIORITY_MAXIMUM             M68K_CPU_PRIORITY_MAXIMUM

#define CPU_SIZEOF_POINTER 4

/*
 *  m68k is pretty tolerant of alignment.  Just put things on 4 byte boundaries.
 */

#define CPU_ALIGNMENT                    4
#define CPU_HEAP_ALIGNMENT               CPU_ALIGNMENT

/*
 *  On m68k thread stacks require no further alignment after allocation
 *  from the Workspace.
 */

#define CPU_STACK_ALIGNMENT        0

#define CPU_INTERRUPT_STACK_ALIGNMENT CPU_CACHE_LINE_BYTES

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

#define _CPU_Initialize_vectors()

#define _CPU_ISR_Disable( _level ) \
  m68k_disable_interrupts( _level )

#define _CPU_ISR_Enable( _level ) \
  m68k_enable_interrupts( _level )

#define _CPU_ISR_Flash( _level ) \
  m68k_flash_interrupts( _level )

RTEMS_INLINE_ROUTINE bool _CPU_ISR_Is_enabled( uint32_t level )
{
  return ( level & 0x0700 ) == 0;
}

#define _CPU_ISR_Set_level( _newlevel ) \
   m68k_set_interrupt_level( _newlevel )

uint32_t   _CPU_ISR_Get_level( void );

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

void _CPU_Context_Initialize(
  Context_Control *the_context,
  void *stack_area_begin,
  size_t stack_area_size,
  uint32_t new_level,
  void (*entry_point)( void ),
  bool is_fp,
  void *tls_area
);

/* end of Context handler macros */

void *_CPU_Thread_Idle_body( uintptr_t ignored );

/*
 *  Fatal Error manager macros
 *
 *  These macros perform the following functions:
 *    + disable interrupts and halt the CPU
 */

#if ( defined(__mcoldfire__) )
#define _CPU_Fatal_halt( _source, _error ) \
  { __asm__ volatile( "move.w %%sr,%%d0\n\t" \
		  "or.l %2,%%d0\n\t" \
		  "move.w %%d0,%%sr\n\t" \
		  "move.l %1,%%d0\n\t" \
		  "move.l #0xDEADBEEF,%%d1\n\t" \
                  "halt" \
		  : "=g" (_error) \
		  : "0" (_error), "d"(0x0700) \
		  : "d0", "d1" ); \
  }
#else
#define _CPU_Fatal_halt( _source, _error ) \
  { __asm__ volatile( "movl  %0,%%d0; " \
                  "orw   #0x0700,%%sr; " \
                  "stop  #0x2700" : "=d" ((_error)) : "0" ((_error)) ); \
  }
#endif

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

#if ( M68K_HAS_BFFFO != 1 )
/*
 *  Lookup table for BFFFO simulation
 */
extern const unsigned char _CPU_m68k_BFFFO_table[256];
#endif

#if ( M68K_HAS_BFFFO == 1 )

#define _CPU_Bitfield_Find_first_bit( _value, _output ) \
  __asm__ volatile( "bfffo (%1),#0,#16,%0" : "=d" (_output) : "a" (&_value));

#elif ( __mcfisaaplus__ )
  /* This is simplified by the fact that RTEMS never calls it with _value=0 */
#define _CPU_Bitfield_Find_first_bit( _value, _output ) \
    __asm__ volatile ( \
       "   swap     %0\n"        \
       "   ff1.l    %0\n"        \
       : "=d" ((_output))        \
       : "0" ((_value))          \
       : "cc" ) ;

#else
/* duplicates BFFFO results for 16 bits (i.e., 15-(_priority) in
   _CPU_Priority_bits_index is not needed), handles the 0 case, and
   does not molest _value -- jsg */
#if ( defined(__mcoldfire__) )

#define _CPU_Bitfield_Find_first_bit( _value, _output ) \
  { \
    int dumby; \
    \
    __asm__ volatile ( \
       "   clr.l   %1\n"         \
       "   move.w  %2,%1\n"      \
       "   lsr.l   #8,%1\n"      \
       "   beq.s   1f\n"         \
       "   move.b  (%3,%1),%0\n" \
       "   bra.s   0f\n"         \
       "1: move.w  %2,%1\n"      \
       "   move.b  (%3,%1),%0\n" \
       "   addq.l  #8,%0\n"      \
       "0: and.l   #0xff,%0\n"   \
       : "=&d" ((_output)), "=&d" ((dumby))    \
       : "d" ((_value)), "ao" ((_CPU_m68k_BFFFO_table)) \
       : "cc" ) ; \
  }
#elif ( M68K_HAS_EXTB_L == 1 )
#define _CPU_Bitfield_Find_first_bit( _value, _output ) \
  { \
    int dumby; \
    \
    __asm__ volatile ( "   move.w  %2,%1\n"        \
       "   lsr.w   #8,%1\n"        \
       "   beq.s   1f\n"           \
       "   move.b  (%3,%1.w),%0\n" \
       "   extb.l  %0\n"           \
       "   bra.s   0f\n"           \
       "1: moveq.l #8,%0\n"        \
       "   add.b   (%3,%2.w),%0\n" \
       "0:\n"                      \
       : "=&d" ((_output)), "=&d" ((dumby)) \
       : "d" ((_value)), "ao" ((_CPU_m68k_BFFFO_table)) \
       : "cc" ) ; \
  }
#else
#define _CPU_Bitfield_Find_first_bit( _value, _output ) \
  { \
    int dumby; \
    \
    __asm__ volatile ( "   move.w  %2,%1\n"        \
       "   lsr.w   #8,%1\n"        \
       "   beq.s   1f\n"           \
       "   move.b  (%3,%1.w),%0\n" \
       "   and.l   #0x000000ff,%0\n"\
       "   bra.s   0f\n"           \
       "1: moveq.l #8,%0\n"        \
       "   add.b   (%3,%2.w),%0\n" \
       "0:\n"                      \
       : "=&d" ((_output)), "=&d" ((dumby)) \
       : "d" ((_value)), "ao" ((_CPU_m68k_BFFFO_table)) \
       : "cc" ) ; \
  }
#endif

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

void _CPU_Initialize(void);

typedef void ( *CPU_ISR_raw_handler )( void );

void _CPU_ISR_install_raw_handler(
  uint32_t             vector,
  CPU_ISR_raw_handler  new_handler,
  CPU_ISR_raw_handler *old_handler
);

typedef void ( *CPU_ISR_handler )( uint32_t );

void _CPU_ISR_install_vector(
  uint32_t         vector,
  CPU_ISR_handler  new_handler,
  CPU_ISR_handler *old_handler
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

void _CPU_Context_Restart_self(
  Context_Control  *the_context
) RTEMS_NO_RETURN;

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

/**
 *  This method prints the CPU exception frame.
 *
 *  @param[in] frame points to the frame to be printed
 */
void _CPU_Exception_frame_print(
  const CPU_Exception_frame *frame
);

typedef uint32_t CPU_Counter_ticks;

uint32_t _CPU_Counter_frequency( void );

CPU_Counter_ticks _CPU_Counter_read( void );

static inline CPU_Counter_ticks _CPU_Counter_difference(
  CPU_Counter_ticks second,
  CPU_Counter_ticks first
)
{
  return second - first;
}

#if (M68K_HAS_FPSP_PACKAGE == 1)
/*
 *  Hooks for the Floating Point Support Package (FPSP) provided by Motorola
 *
 *  NOTES:
 *
 *  Motorola 68k family CPU's before the 68040 used a coprocessor
 *  (68881 or 68882) to handle floating point.  The 68040 has internal
 *  floating point support -- but *not* the complete support provided by
 *  the 68881 or 68882.  The leftover functions are taken care of by the
 *  M68040 Floating Point Support Package.  Quoting from the MC68040
 *  Microprocessors User's Manual, Section 9, Floating-Point Unit (MC68040):
 *
 *    "When used with the M68040FPSP, the MC68040 FPU is fully
 *    compliant with IEEE floating-point standards."
 *
 *  M68KFPSPInstallExceptionHandlers is in libcpu/m68k/MODEL/fpsp and
 *  is invoked early in the application code to ensure that proper FP
 *  behavior is installed.  This is not left to the BSP to call, since
 *  this would force all applications using that BSP to use FPSP which
 *  is not necessarily desirable.
 *
 *  There is a similar package for the 68060 but RTEMS does not yet
 *  support the 68060.
 */

void M68KFPSPInstallExceptionHandlers (void);

extern int (*_FPSP_install_raw_handler)(
  uint32_t   vector,
  CPU_ISR_raw_handler new_handler,
  CPU_ISR_raw_handler *old_handler
);

#endif

/** Type that can store a 32-bit integer or a pointer. */
typedef uintptr_t CPU_Uint32ptr;

#endif

#ifdef __cplusplus
}
#endif

#endif
