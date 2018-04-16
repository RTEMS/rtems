/**
 * @file rtems/score/cpu.h
 */

/*
 *
 * Copyright (c) 2015 University of York.
 * Hesham Almatary <hesham@alumni.york.ac.uk>
 *
 * COPYRIGHT (c) 1989-1999.
 * On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _RISCV_CPU_H
#define _RISCV_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/basedefs.h>
#include <rtems/score/riscv.h> /* pick up machine definitions */
#include <rtems/score/riscv-utility.h>
#ifndef ASM
#include <rtems/bspIo.h>
#include <stdint.h>
#include <stdio.h> /* for printk */
#endif

#define CPU_INLINE_ENABLE_DISPATCH       FALSE
#define CPU_UNROLL_ENQUEUE_PRIORITY      TRUE
#define CPU_HAS_SOFTWARE_INTERRUPT_STACK TRUE
#define CPU_HAS_HARDWARE_INTERRUPT_STACK FALSE
#define CPU_ALLOCATE_INTERRUPT_STACK TRUE
#define CPU_ISR_PASSES_FRAME_POINTER 1
#define CPU_HARDWARE_FP                  FALSE
#define CPU_SOFTWARE_FP                  FALSE
#define CPU_ALL_TASKS_ARE_FP             FALSE
#define CPU_IDLE_TASK_IS_FP              FALSE
#define CPU_USE_DEFERRED_FP_SWITCH       FALSE
#define CPU_PROVIDES_IDLE_THREAD_BODY    TRUE
#define CPU_STACK_GROWS_UP               FALSE

#define CPU_STRUCTURE_ALIGNMENT __attribute__ ((aligned (64)))
#define CPU_HAS_OWN_HOST_TO_NETWORK_ROUTINES     FALSE
#define CPU_BIG_ENDIAN                           FALSE
#define CPU_LITTLE_ENDIAN                        TRUE
#define CPU_MODES_INTERRUPT_MASK   0x0000000000000001

/*
 *  Processor defined structures required for cpukit/score.
 */

#ifndef ASM

typedef struct {
  /* riscv has 32 xlen-bit (where xlen can be 32 or 64) general purpose registers (x0-x31)*/
  unsigned long x[32];

  /* Special purpose registers */
  unsigned long mstatus;
  unsigned long mcause;
  unsigned long mepc;
#ifdef RTEMS_SMP
  /**
   * @brief On SMP configurations the thread context must contain a boolean
   * indicator to signal if this context is executing on a processor.
   *
   * This field must be updated during a context switch.  The context switch
   * to the heir must wait until the heir context indicates that it is no
   * longer executing on a processor.  The context switch must also check if
   * a thread dispatch is necessary to honor updates of the heir thread for
   * this processor.  This indicator must be updated using an atomic test and
   * set operation to ensure that at most one processor uses the heir
   * context at the same time.
   *
   * @code
   * void _CPU_Context_switch(
   *   Context_Control *executing,
   *   Context_Control *heir
   * )
   * {
   *   save( executing );
   *
   *   executing->is_executing = false;
   *   memory_barrier();
   *
   *   if ( test_and_set( &heir->is_executing ) ) {
   *     do {
   *       Per_CPU_Control *cpu_self = _Per_CPU_Get_snapshot();
   *
   *       if ( cpu_self->dispatch_necessary ) {
   *         heir = _Thread_Get_heir_and_make_it_executing( cpu_self );
   *       }
   *     } while ( test_and_set( &heir->is_executing ) );
   *   }
   *
   *   restore( heir );
   * }
   * @endcode
   */
  volatile bool is_executing;
#endif
} Context_Control;

#define _CPU_Context_Get_SP( _context ) \
  (_context)->x[2]

typedef struct {
  /** TODO FPU registers are listed here */
  double  some_float_register;
} Context_Control_fp;

typedef Context_Control CPU_Interrupt_frame;

#define CPU_CONTEXT_FP_SIZE  0
Context_Control_fp  _CPU_Null_fp_context;

#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 0
#if __riscv_xlen == 32
#define CPU_STACK_MINIMUM_SIZE  4096
#else
#define CPU_STACK_MINIMUM_SIZE  4096 * 2
#endif
#define CPU_ALIGNMENT 8
#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE
#define CPU_HEAP_ALIGNMENT         CPU_ALIGNMENT
#define CPU_PARTITION_ALIGNMENT    CPU_ALIGNMENT
#define CPU_STACK_ALIGNMENT        8
#define _CPU_Initialize_vectors()

/*
 *  Disable all interrupts for an RTEMS critical section.  The previous
 *  level is returned in _level.
 *
 */

static inline unsigned long riscv_interrupt_disable( void )
{
  unsigned long status = read_csr(mstatus);
  clear_csr(mstatus, MSTATUS_MIE);
  return status;
}

static inline void riscv_interrupt_enable(unsigned long level)
{
  write_csr(mstatus, level);
}

#define _CPU_ISR_Disable( _level ) \
    _level = riscv_interrupt_disable()

#define _CPU_ISR_Enable( _level )  \
  riscv_interrupt_enable( _level )

#define _CPU_ISR_Flash( _level ) \
  do{ \
      _CPU_ISR_Enable( _level ); \
      riscv_interrupt_disable(); \
    } while(0)

RTEMS_INLINE_ROUTINE bool _CPU_ISR_Is_enabled( unsigned long level )
{
  return ( level & MSTATUS_MIE ) != 0;
}

void _CPU_ISR_Set_level( unsigned long level );

unsigned long _CPU_ISR_Get_level( void );

/* end of ISR handler macros */

/* Context handler macros */
#define RISCV_GCC_RED_ZONE_SIZE 128

void _CPU_Context_Initialize(
  Context_Control *context,
  void *stack_area_begin,
  size_t stack_area_size,
  unsigned long new_level,
  void (*entry_point)( void ),
  bool is_fp,
  void *tls_area
);

#define _CPU_Context_Restart_self( _the_context ) \
   _CPU_Context_restore( (_the_context) )


#define _CPU_Context_Fp_start( _base, _offset ) \
   ( (void *) _Addresses_Add_offset( (_base), (_offset) ) )

#define _CPU_Context_Initialize_fp( _destination ) \
  { \
   *(*(_destination)) = _CPU_Null_fp_context; \
  }

extern void _CPU_Fatal_halt(uint32_t source, uint32_t error) RTEMS_NO_RETURN;

/* end of Fatal Error manager macros */

#define CPU_USE_GENERIC_BITFIELD_CODE TRUE
#define CPU_USE_GENERIC_BITFIELD_DATA TRUE

#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)

#define _CPU_Bitfield_Find_first_bit( _value, _output ) \
  { \
    (_output) = 0;   /* do something to prevent warnings */ \
  }
#endif

/* end of Bitfield handler macros */

/*
 *  This routine builds the mask which corresponds to the bit fields
 *  as searched by _CPU_Bitfield_Find_first_bit().  See the discussion
 *  for that routine.
 *
 */

#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)

#define _CPU_Priority_Mask( _bit_number ) \
    (1 << _bit_number)

#endif

#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)

#define _CPU_Priority_bits_index( _priority ) \
  (_priority)

#endif

#define CPU_MAXIMUM_PROCESSORS 32

#define CPU_TIMESTAMP_USE_STRUCT_TIMESPEC FALSE
#define CPU_TIMESTAMP_USE_INT64 TRUE
#define CPU_TIMESTAMP_USE_INT64_INLINE FALSE

typedef struct {
  /* There is no CPU specific per-CPU state */
} CPU_Per_CPU_control;
#endif /* ASM */

#if __riscv_xlen == 32
#define CPU_SIZEOF_POINTER 4

/* 32-bit load/store instructions */
#define LREG lw
#define SREG sw

#define CPU_EXCEPTION_FRAME_SIZE 128
#else /* xlen = 64 */
#define CPU_SIZEOF_POINTER 8

/* 64-bit load/store instructions */
#define LREG ld
#define SREG sd

#define CPU_EXCEPTION_FRAME_SIZE 256
#endif

#define CPU_PER_CPU_CONTROL_SIZE 0

#ifndef ASM
typedef uint16_t Priority_bit_map_Word;

typedef struct {
  unsigned long x[32];;
} CPU_Exception_frame;

/**
 * @brief Prints the exception frame via printk().
 *
 * @see rtems_fatal() and RTEMS_FATAL_SOURCE_EXCEPTION.
 */
void _CPU_Exception_frame_print( const CPU_Exception_frame *frame );


/* end of Priority handler macros */

/* functions */

/*
 *  _CPU_Initialize
 *
 *  This routine performs CPU dependent initialization.
 *
 */

void _CPU_Initialize(
  void
);

/*
 *  _CPU_ISR_install_raw_handler
 *
 *  This routine installs a "raw" interrupt handler directly into the
 *  processor's vector table.
 *
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
 *
 *  NO_CPU Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */

void _CPU_ISR_install_vector(
  unsigned long    vector,
  proc_ptr   new_handler,
  proc_ptr   *old_handler
);

/*
 *  _CPU_Install_interrupt_stack
 *
 *  This routine installs the hardware interrupt stack pointer.
 *
 *  NOTE:  It need only be provided if CPU_HAS_HARDWARE_INTERRUPT_STACK
 *         is TRUE.
 *
 */

void _CPU_Install_interrupt_stack( void );

/*
 *  _CPU_Thread_Idle_body
 *
 *  This routine is the CPU dependent IDLE thread body.
 *
 *  NOTE:  It need only be provided if CPU_PROVIDES_IDLE_THREAD_BODY
 *         is TRUE.
 *
 */

void *_CPU_Thread_Idle_body( uintptr_t ignored );

/*
 *  _CPU_Context_switch
 *
 *  This routine switches from the run context to the heir context.
 *
 *  RISCV Specific Information:
 *
 *  Please see the comments in the .c file for a description of how
 *  this function works. There are several things to be aware of.
 */

void _CPU_Context_switch(
  Context_Control  *run,
  Context_Control  *heir
);

/*
 *  _CPU_Context_restore
 *
 *  This routine is generally used only to restart self in an
 *  efficient manner.  It may simply be a label in _CPU_Context_switch.
 *
 *  NOTE: May be unnecessary to reload some registers.
 *
 */

void _CPU_Context_restore(
  Context_Control *new_context
) RTEMS_NO_RETURN;

/*
 *  _CPU_Context_save_fp
 *
 *  This routine saves the floating point context passed to it.
 *
 */

void _CPU_Context_save_fp(
  void **fp_context_ptr
);

/*
 *  _CPU_Context_restore_fp
 *
 *  This routine restores the floating point context passed to it.
 *
 */

void _CPU_Context_restore_fp(
  void **fp_context_ptr
);

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
 *
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
  return ( swapped );
}

#define CPU_swap_u16( value ) \
  (((value&0xff) << 8) | ((value >> 8)&0xff))

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

typedef uint32_t CPU_Counter_ticks;

CPU_Counter_ticks _CPU_Counter_read( void );

#ifdef RTEMS_SMP
/**
 * @brief Performs CPU specific SMP initialization in the context of the boot
 * processor.
 *
 * This function is invoked on the boot processor during system
 * initialization.  All interrupt stacks are allocated at this point in case
 * the CPU port allocates the interrupt stacks.  This function is called
 * before _CPU_SMP_Start_processor() or _CPU_SMP_Finalize_initialization() is
 * used.
 *
 * @return The count of physically or virtually available processors.
 * Depending on the configuration the application may use not all processors.
 */
uint32_t _CPU_SMP_Initialize( void );

/**
 * @brief Starts a processor specified by its index.
 *
 * This function is invoked on the boot processor during system
 * initialization.
 *
 * This function will be called after _CPU_SMP_Initialize().
 *
 * @param[in] cpu_index The processor index.
 *
 * @retval true Successful operation.
 * @retval false Unable to start this processor.
 */
bool _CPU_SMP_Start_processor( uint32_t cpu_index );

/**
 * @brief Performs final steps of CPU specific SMP initialization in the
 * context of the boot processor.
 *
 * This function is invoked on the boot processor during system
 * initialization.
 *
 * This function will be called after all processors requested by the
 * application have been started.
 *
 * @param[in] cpu_count The minimum value of the count of processors
 * requested by the application configuration and the count of physically or
 * virtually available processors.
 */
void _CPU_SMP_Finalize_initialization( uint32_t cpu_count );

/**
 * @brief Returns the index of the current processor.
 *
 * An architecture specific method must be used to obtain the index of the
 * current processor in the system.  The set of processor indices is the
 * range of integers starting with zero up to the processor count minus one.
 */
uint32_t _CPU_SMP_Get_current_processor( void );

/**
 * @brief Sends an inter-processor interrupt to the specified target
 * processor.
 *
 * This operation is undefined for target processor indices out of range.
 *
 * @param[in] target_processor_index The target processor index.
 */
void _CPU_SMP_Send_interrupt( uint32_t target_processor_index );

/**
 * @brief Broadcasts a processor event.
 *
 * Some architectures provide a low-level synchronization primitive for
 * processors in a multi-processor environment.  Processors waiting for this
 * event may go into a low-power state and stop generating system bus
 * transactions.  This function must ensure that preceding store operations
 * can be observed by other processors.
 *
 * @see _CPU_SMP_Processor_event_receive().
 */
void _CPU_SMP_Processor_event_broadcast( void );

/**
 * @brief Receives a processor event.
 *
 * This function will wait for the processor event and may wait forever if no
 * such event arrives.
 *
 * @see _CPU_SMP_Processor_event_broadcast().
 */
static inline void _CPU_SMP_Processor_event_receive( void )
{
  __asm__ volatile ( "" : : : "memory" );
}

/**
 * @brief Gets the is executing indicator of the thread context.
 *
 * @param[in] context The context.
 */
static inline bool _CPU_Context_Get_is_executing(
  const Context_Control *context
)
{
  return context->is_executing;
}

/**
 * @brief Sets the is executing indicator of the thread context.
 *
 * @param[in] context The context.
 * @param[in] is_executing The new value for the is executing indicator.
 */
static inline void _CPU_Context_Set_is_executing(
  Context_Control *context,
  bool is_executing
)
{
  context->is_executing = is_executing;
}
#endif /* RTEMS_SMP */

/** Type that can store a 32-bit integer or a pointer. */
typedef uintptr_t CPU_Uint32ptr;

#endif /* ASM */

#ifdef __cplusplus
}
#endif

#endif
