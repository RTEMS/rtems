/**
 * @file
 * 
 */

/*
 * Copyright (c) 2018 embedded brains GmbH
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
#include <rtems/score/riscv.h>

#define RISCV_MSTATUS_MIE 0x8

#define CPU_ISR_PASSES_FRAME_POINTER FALSE

#define CPU_HARDWARE_FP                  FALSE
#define CPU_SOFTWARE_FP                  FALSE
#define CPU_ALL_TASKS_ARE_FP             FALSE
#define CPU_IDLE_TASK_IS_FP              FALSE
#define CPU_USE_DEFERRED_FP_SWITCH       FALSE

#define CPU_ENABLE_ROBUST_THREAD_DISPATCH TRUE

#define CPU_STACK_GROWS_UP               FALSE

#define CPU_STRUCTURE_ALIGNMENT __attribute__ ((aligned (64)))
#define CPU_BIG_ENDIAN                           FALSE
#define CPU_LITTLE_ENDIAN                        TRUE
#define CPU_MODES_INTERRUPT_MASK   0x0000000000000001

#define CPU_CACHE_LINE_BYTES 64

#if __riscv_xlen == 32

#define CPU_SIZEOF_POINTER 4

#define CPU_STACK_MINIMUM_SIZE 4096

#elif __riscv_xlen == 64

#define CPU_SIZEOF_POINTER 8

#define CPU_STACK_MINIMUM_SIZE 8192

#endif /* __riscv_xlen */

/* RISC-V ELF psABI specification */
#define CPU_ALIGNMENT 16

#define CPU_HEAP_ALIGNMENT CPU_ALIGNMENT

/* RISC-V ELF psABI specification */
#define CPU_STACK_ALIGNMENT 16

#define CPU_INTERRUPT_STACK_ALIGNMENT CPU_CACHE_LINE_BYTES

/*
 *  Processor defined structures required for cpukit/score.
 */

#ifndef ASM

#if __riscv_flen == 32
typedef float RISCV_Float;
#elif __riscv_flen == 64
typedef double RISCV_Float;
#endif

typedef struct {
#ifdef RTEMS_SMP
  volatile uint32_t is_executing;
#else
  uint32_t reserved;
#endif
  uint32_t isr_dispatch_disable;
  uintptr_t ra;
  uintptr_t sp;
  uintptr_t tp;
  uintptr_t s0;
  uintptr_t s1;
  uintptr_t s2;
  uintptr_t s3;
  uintptr_t s4;
  uintptr_t s5;
  uintptr_t s6;
  uintptr_t s7;
  uintptr_t s8;
  uintptr_t s9;
  uintptr_t s10;
  uintptr_t s11;
#if __riscv_flen > 0
  uint32_t fcsr;
  RISCV_Float fs0;
  RISCV_Float fs1;
  RISCV_Float fs2;
  RISCV_Float fs3;
  RISCV_Float fs4;
  RISCV_Float fs5;
  RISCV_Float fs6;
  RISCV_Float fs7;
  RISCV_Float fs8;
  RISCV_Float fs9;
  RISCV_Float fs10;
  RISCV_Float fs11;
#endif
} Context_Control;

#define _CPU_Context_Get_SP( _context ) \
  (_context)->sp

#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 0

#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE

#define _CPU_Initialize_vectors()

static inline uint32_t riscv_interrupt_disable( void )
{
  unsigned long mstatus;

  __asm__ volatile (
    "csrrc %0, mstatus, " RTEMS_XSTRING( RISCV_MSTATUS_MIE ) :
      "=&r" ( mstatus )
  );

  return mstatus & RISCV_MSTATUS_MIE;
}

static inline void riscv_interrupt_enable( uint32_t level )
{
  __asm__ volatile ( "csrrs zero, mstatus, %0" : : "r" ( level ) );
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
  return ( level & RISCV_MSTATUS_MIE ) != 0;
}

RTEMS_INLINE_ROUTINE void _CPU_ISR_Set_level( uint32_t level )
{
  if ( ( level & CPU_MODES_INTERRUPT_MASK) == 0 ) {
    __asm__ volatile (
      "csrrs zero, mstatus, " RTEMS_XSTRING( RISCV_MSTATUS_MIE )
    );
  } else {
    __asm__ volatile (
      "csrrc zero, mstatus, " RTEMS_XSTRING( RISCV_MSTATUS_MIE )
    );
  }
}

uint32_t _CPU_ISR_Get_level( void );

/* end of ISR handler macros */

void _CPU_Context_Initialize(
  Context_Control *context,
  void            *stack_area_begin,
  size_t           stack_area_size,
  uint32_t         new_level,
  void          ( *entry_point )( void ),
  bool             is_fp,
  void            *tls_area
);

#define _CPU_Context_Restart_self( _the_context ) \
   _CPU_Context_restore( (_the_context) )

extern void _CPU_Fatal_halt(uint32_t source, uint32_t error) RTEMS_NO_RETURN;

#define CPU_USE_GENERIC_BITFIELD_CODE TRUE

#define CPU_USE_GENERIC_BITFIELD_DATA TRUE

#define CPU_MAXIMUM_PROCESSORS 32

typedef uint16_t Priority_bit_map_Word;

/*
 * See The RISC-V Instruction Set Manual, Volume II: RISC-V Privileged
 * Architectures V1.10, Table 3.6: Machine cause register (mcause) values after
 * trap.
 */
typedef enum {
  RISCV_INTERRUPT_SOFTWARE_USER = 0,
  RISCV_INTERRUPT_SOFTWARE_SUPERVISOR = 1,
  RISCV_INTERRUPT_SOFTWARE_MACHINE = 3,
  RISCV_INTERRUPT_TIMER_USER = 4,
  RISCV_INTERRUPT_TIMER_SUPERVISOR = 5,
  RISCV_INTERRUPT_TIMER_MACHINE = 7,
  RISCV_INTERRUPT_EXTERNAL_USER = 8,
  RISCV_INTERRUPT_EXTERNAL_SUPERVISOR = 9,
  RISCV_INTERRUPT_EXTERNAL_MACHINE = 11
} RISCV_Interrupt_code;

/*
 * See The RISC-V Instruction Set Manual, Volume II: RISC-V Privileged
 * Architectures V1.10, Table 3.6: Machine cause register (mcause) values after
 * trap.
 */
typedef enum {
  RISCV_EXCEPTION_INSTRUCTION_ADDRESS_MISALIGNED = 0,
  RISCV_EXCEPTION_INSTRUCTION_ACCESS_FAULT = 1,
  RISCV_EXCEPTION_ILLEGAL_INSTRUCTION = 2,
  RISCV_EXCEPTION_BREAKPOINT = 3,
  RISCV_EXCEPTION_LOAD_ADDRESS_MISALIGNED = 4,
  RISCV_EXCEPTION_LOAD_ACCESS_FAULT = 5,
  RISCV_EXCEPTION_STORE_OR_AMO_ADDRESS_MISALIGNED = 6,
  RISCV_EXCEPTION_STORE_OR_AMO_ACCESS_FAULT = 7,
  RISCV_EXCEPTION_ENVIRONMENT_CALL_FROM_U_MODE = 8,
  RISCV_EXCEPTION_ENVIRONMENT_CALL_FROM_S_MODE = 9,
  RISCV_EXCEPTION_ENVIRONMENT_CALL_FROM_M_MODE = 11,
  RISCV_EXCEPTION_INSTRUCTION_PAGE_FAULT = 12,
  RISCV_EXCEPTION_LOAD_PAGE_FAULT = 13,
  RISCV_EXCEPTION_STORE_OR_AMO_PAGE_FAULT = 15
} RISCV_Exception_code;

typedef struct {
  uintptr_t mstatus;
  uintptr_t mepc;
  uintptr_t a2;
  uintptr_t s0;
  uintptr_t s1;
  uintptr_t ra;
  uintptr_t a3;
  uintptr_t a4;
  uintptr_t a5;
  uintptr_t a6;
  uintptr_t a7;
  uintptr_t t0;
  uintptr_t t1;
  uintptr_t t2;
  uintptr_t t3;
  uintptr_t t4;
  uintptr_t t5;
  uintptr_t t6;
#if __riscv_flen > 0
  uint32_t fcsr;
  RISCV_Float ft0;
  RISCV_Float ft1;
  RISCV_Float ft2;
  RISCV_Float ft3;
  RISCV_Float ft4;
  RISCV_Float ft5;
  RISCV_Float ft6;
  RISCV_Float ft7;
  RISCV_Float ft8;
  RISCV_Float ft9;
  RISCV_Float ft10;
  RISCV_Float ft11;
  RISCV_Float fa0;
  RISCV_Float fa1;
  RISCV_Float fa2;
  RISCV_Float fa3;
  RISCV_Float fa4;
  RISCV_Float fa5;
  RISCV_Float fa6;
  RISCV_Float fa7;
#endif
  uintptr_t a0;
  uintptr_t a1;
} RTEMS_ALIGNED( CPU_STACK_ALIGNMENT ) CPU_Interrupt_frame;

typedef struct {
  CPU_Interrupt_frame Interrupt_frame;
  uintptr_t mcause;
  uintptr_t sp;
  uintptr_t gp;
  uintptr_t tp;
  uintptr_t s2;
  uintptr_t s3;
  uintptr_t s4;
  uintptr_t s5;
  uintptr_t s6;
  uintptr_t s7;
  uintptr_t s8;
  uintptr_t s9;
  uintptr_t s10;
  uintptr_t s11;
#if __riscv_flen > 0
  RISCV_Float fs0;
  RISCV_Float fs1;
  RISCV_Float fs2;
  RISCV_Float fs3;
  RISCV_Float fs4;
  RISCV_Float fs5;
  RISCV_Float fs6;
  RISCV_Float fs7;
  RISCV_Float fs8;
  RISCV_Float fs9;
  RISCV_Float fs10;
  RISCV_Float fs11;
#endif
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

typedef uint32_t CPU_Counter_ticks;

uint32_t _CPU_Counter_frequency( void );

extern volatile uint32_t * const _RISCV_Counter;

CPU_Counter_ticks _CPU_Counter_read( void );

static inline CPU_Counter_ticks _CPU_Counter_difference(
  CPU_Counter_ticks second,
  CPU_Counter_ticks first
)
{
  return second - first;
}

#ifdef RTEMS_SMP

uint32_t _CPU_SMP_Initialize( void );

bool _CPU_SMP_Start_processor( uint32_t cpu_index );

void _CPU_SMP_Finalize_initialization( uint32_t cpu_count );

void _CPU_SMP_Prepare_start_multitasking( void );

static inline uint32_t _CPU_SMP_Get_current_processor( void )
{
  unsigned long mhartid;

  __asm__ volatile ( "csrr %0, mhartid" : "=&r" ( mhartid ) );

  return (uint32_t) mhartid;
}

void _CPU_SMP_Send_interrupt( uint32_t target_processor_index );

static inline void _CPU_SMP_Processor_event_broadcast( void )
{
  __asm__ volatile ( "" : : : "memory" );
}

static inline void _CPU_SMP_Processor_event_receive( void )
{
  __asm__ volatile ( "" : : : "memory" );
}

static inline bool _CPU_Context_Get_is_executing(
  const Context_Control *context
)
{
  return context->is_executing;
}

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
