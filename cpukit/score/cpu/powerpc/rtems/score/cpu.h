/**
 * @file rtems/score/cpu.h
 */

/*
 * $Id$
 */
 
#ifndef _RTEMS_SCORE_CPU_H
#define _RTEMS_SCORE_CPU_H

#include <rtems/score/powerpc.h>              /* pick up machine definitions */
#ifndef ASM
#include <rtems/score/types.h>
#endif

/* conditional compilation parameters */

/*
 *  Should the calls to _Thread_Enable_dispatch be inlined?
 *
 *  If TRUE, then they are inlined.
 *  If FALSE, then a subroutine call is made.
 *
 *  Basically this is an example of the classic trade-off of size
 *  versus speed.  Inlining the call (TRUE) typically increases the
 *  size of RTEMS while speeding up the enabling of dispatching.
 *  [NOTE: In general, the _Thread_Dispatch_disable_level will
 *  only be 0 or 1 unless you are in an interrupt handler and that
 *  interrupt handler invokes the executive.]  When not inlined
 *  something calls _Thread_Enable_dispatch which in turns calls
 *  _Thread_Dispatch.  If the enable dispatch is inlined, then
 *  one subroutine call is avoided entirely.]
 */

#define CPU_INLINE_ENABLE_DISPATCH       FALSE

/*
 *  Should the body of the search loops in _Thread_queue_Enqueue_priority
 *  be unrolled one time?  In unrolled each iteration of the loop examines
 *  two "nodes" on the chain being searched.  Otherwise, only one node
 *  is examined per iteration.
 *
 *  If TRUE, then the loops are unrolled.
 *  If FALSE, then the loops are not unrolled.
 *
 *  The primary factor in making this decision is the cost of disabling
 *  and enabling interrupts (_ISR_Flash) versus the cost of rest of the
 *  body of the loop.  On some CPUs, the flash is more expensive than
 *  one iteration of the loop body.  In this case, it might be desirable
 *  to unroll the loop.  It is important to note that on some CPUs, this
 *  code is the longest interrupt disable period in RTEMS.  So it is
 *  necessary to strike a balance when setting this parameter.
 */

#define CPU_UNROLL_ENQUEUE_PRIORITY      FALSE

#ifdef _OLD_EXCEPTIONS
#include <rtems/old-exceptions/cpu.h>
#else
#include <rtems/new-exceptions/cpu.h>
#endif

#ifndef ASM
/*  The following routine swaps the endian format of an unsigned int.
 *  It must be static because it is referenced indirectly.
 *
 *  This version will work on any processor, but if there is a better
 *  way for your CPU PLEASE use it.  The most common way to do this is to:
 *
 *     swap least significant two bytes with 16-bit rotate
 *     swap upper and lower 16-bits
 *     swap most significant two bytes with 16-bit rotate
 *
 *  Some CPUs have special instructions which swap a 32-bit quantity in
 *  a single instruction (e.g. i486).  It is probably best to avoid
 *  an "endian swapping control bit" in the CPU.  One good reason is
 *  that interrupts would probably have to be disabled to insure that
 *  an interrupt does not try to access the same "chunk" with the wrong
 *  endian.  Another good reason is that on some CPUs, the endian bit
 *  endianness for ALL fetches -- both code and data -- so the code
 *  will be fetched incorrectly.
 */
 
static inline uint32_t CPU_swap_u32(
  uint32_t value
)
{
  uint32_t   swapped;
 
  asm volatile("rlwimi %0,%1,8,24,31;"
	       "rlwimi %0,%1,24,16,23;"
	       "rlwimi %0,%1,8,8,15;"
	       "rlwimi %0,%1,24,0,7;" :
	       "=&r" ((swapped)) : "r" ((value)));

  return( swapped );
}

#define CPU_swap_u16( value ) \
  (((value&0xff) << 8) | ((value >> 8)&0xff))

#endif /* ASM */

#ifndef ASM
/*
 *  Macros to access PowerPC specific additions to the CPU Table
 */

#define rtems_cpu_configuration_get_clicks_per_usec() \
   (_CPU_Table.clicks_per_usec)

#define rtems_cpu_configuration_get_exceptions_in_ram() \
   (_CPU_Table.exceptions_in_RAM)

#endif /* ASM */

#ifndef ASM
/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

#define CPU_Get_timebase_low( _value ) \
    asm volatile( "mftb  %0" : "=r" (_value) )

#define rtems_bsp_delay( _microseconds ) \
  do { \
    uint32_t   start, ticks, now; \
    CPU_Get_timebase_low( start ) ; \
    ticks = (_microseconds) * rtems_cpu_configuration_get_clicks_per_usec(); \
    do \
      CPU_Get_timebase_low( now ) ; \
    while (now - start < ticks); \
  } while (0)

#define rtems_bsp_delay_in_bus_cycles( _cycles ) \
  do { \
    uint32_t   start, now; \
    CPU_Get_timebase_low( start ); \
    do \
      CPU_Get_timebase_low( now ); \
    while (now - start < (_cycles)); \
  } while (0)

#endif /* ASM */

#ifndef ASM
/*
 *  Routines to access the decrementer register
 */

#define PPC_Set_decrementer( _clicks ) \
  do { \
    asm volatile( "mtdec %0" : : "r" ((_clicks)) ); \
  } while (0)

#define PPC_Get_decrementer( _clicks ) \
    asm volatile( "mfdec  %0" : "=r" (_clicks) )

#endif /* ASM */

#ifndef ASM
/*
 *  Routines to access the time base register
 */

static inline uint64_t PPC_Get_timebase_register( void )
{
  uint32_t tbr_low;
  uint32_t tbr_high;
  uint32_t tbr_high_old;
  uint64_t tbr;

  do {
    asm volatile( "mftbu %0" : "=r" (tbr_high_old));
    asm volatile( "mftb  %0" : "=r" (tbr_low));
    asm volatile( "mftbu %0" : "=r" (tbr_high));
  } while ( tbr_high_old != tbr_high );

  tbr = tbr_high;
  tbr <<= 32;
  tbr |= tbr_low;
  return tbr;
}

static inline  void PPC_Set_timebase_register (uint64_t tbr)
{
  uint32_t tbr_low;
  uint32_t tbr_high;

  tbr_low = (tbr & 0xffffffff) ;
  tbr_high = (tbr >> 32) & 0xffffffff;
  asm volatile( "mtspr 284, %0" : : "r" (tbr_low));
  asm volatile( "mtspr 285, %0" : : "r" (tbr_high));
  
}
#endif /* ASM */

#ifndef ASM
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
 *  This routine generally does not set any unnecessary register
 *  in the context.  The state of the "general data" registers is
 *  undefined at task start time.
 */

void _CPU_Context_Initialize(
  Context_Control  *the_context,
  uint32_t         *stack_base,
  uint32_t          size,
  uint32_t          new_level,
  void             *entry_point,
  boolean           is_fp
);

/*
 *  This routine is responsible for somehow restarting the currently
 *  executing task.  If you are lucky, then all that is necessary
 *  is restoring the context.  Otherwise, there will need to be
 *  a special assembly routine which does something special in this
 *  case.  Context_Restore should work most of the time.  It will
 *  not work if restarting self conflicts with the stack frame
 *  assumptions of restoring a context.
 */

#define _CPU_Context_Restart_self( _the_context ) \
   _CPU_Context_restore( (_the_context) );

/*
 *  The purpose of this macro is to allow the initial pointer into
 *  a floating point context area (used to save the floating point
 *  context) to be at an arbitrary place in the floating point
 *  context area.
 *
 *  This is necessary because some FP units are designed to have
 *  their context saved as a stack which grows into lower addresses.
 *  Other FP units can be saved by simply moving registers into offsets
 *  from the base of the context area.  Finally some FP units provide
 *  a "dump context" instruction which could fill in from high to low
 *  or low to high based on the whim of the CPU designers.
 */

#define _CPU_Context_Fp_start( _base, _offset ) \
   ( (void *) _Addresses_Add_offset( (_base), (_offset) ) )

/*
 *  This routine initializes the FP context area passed to it to.
 *  There are a few standard ways in which to initialize the
 *  floating point context.  The code included for this macro assumes
 *  that this is a CPU in which a "initial" FP context was saved into
 *  _CPU_Null_fp_context and it simply copies it to the destination
 *  context passed to it.
 *
 *  Other models include (1) not doing anything, and (2) putting
 *  a "null FP status word" in the correct place in the FP context.
 */

#define _CPU_Context_Initialize_fp( _destination ) \
  { \
   ((Context_Control_fp *) *((void **) _destination))->fpscr = PPC_INIT_FPSCR; \
  }

/* end of Context handler macros */
#endif /* ASM */

/* functions */

#ifndef ASM

/*
 *  _CPU_Initialize
 *
 *  This routine performs CPU dependent initialization.
 */

void _CPU_Initialize(
  rtems_cpu_table  *cpu_table,
  void            (*thread_dispatch)
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
 *  _CPU_Install_interrupt_stack
 *
 *  This routine installs the hardware interrupt stack pointer.
 *
 *  NOTE:  It need only be provided if CPU_HAS_HARDWARE_INTERRUPT_STACK
 *         is TRUE.
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
 *  This routine is generallu used only to restart self in an
 *  efficient manner.  It may simply be a label in _CPU_Context_switch.
 *
 *  NOTE: May be unnecessary to reload some registers.
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

void _CPU_Fatal_error(
  uint32_t   _error
);

#endif /* ASM */

#endif /* _RTEMS_SCORE_CPU_H */
