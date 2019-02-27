/**
 * @file
 *
 * @brief x86_64 Dependent Source
 *
 * This include file contains information pertaining to the x86_64 processor.
 */

/*
 * Copyright (c) 2018.
 * Amaan Cheval <amaan.cheval@gmail.com>
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

#ifndef _RTEMS_SCORE_CPU_H
#define _RTEMS_SCORE_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/basedefs.h>
#include <rtems/score/cpu_asm.h>
#include <rtems/score/x86_64.h>

#define CPU_SIMPLE_VECTORED_INTERRUPTS FALSE
#define CPU_ISR_PASSES_FRAME_POINTER FALSE
// XXX: Enable FPU support
#define CPU_HARDWARE_FP     FALSE
#define CPU_SOFTWARE_FP     FALSE
#define CPU_ALL_TASKS_ARE_FP     FALSE
#define CPU_IDLE_TASK_IS_FP      FALSE
#define CPU_USE_DEFERRED_FP_SWITCH       TRUE
#define CPU_ENABLE_ROBUST_THREAD_DISPATCH FALSE
#define CPU_STACK_GROWS_UP               FALSE

#define CPU_STRUCTURE_ALIGNMENT RTEMS_ALIGNED(64)
#define CPU_CACHE_LINE_BYTES 64
#define CPU_MODES_INTERRUPT_MASK   0x00000001
#define CPU_MAXIMUM_PROCESSORS 32

#define CPU_EFLAGS_INTERRUPTS_ON  0x00003202
#define CPU_EFLAGS_INTERRUPTS_OFF 0x00003002

#ifndef ASM

typedef struct {
  uint64_t rflags;

  /**
   * Callee-saved registers as listed in the SysV ABI document:
   * https://github.com/hjl-tools/x86-psABI/wiki/X86-psABI
   */
  uint64_t rbx;
  void    *rsp;
  void    *rbp;
  uint64_t r12;
  uint64_t r13;
  uint64_t r14;
  uint64_t r15;

  // XXX: FS segment descriptor for TLS

#ifdef RTEMS_SMP
    volatile bool is_executing;
#endif
} Context_Control;

#define _CPU_Context_Get_SP( _context ) \
  (_context)->rsp

typedef struct {
  /* XXX: MMX, XMM, others?
   *
   * All x87 registers are caller-saved, so callees that make use of the MMX
   * registers may use the faster femms instruction
   */

  /** FPU registers are listed here */
  double      some_float_register;
} Context_Control_fp;

/*
 * Caller-saved registers for interrupt frames
 */
typedef struct {
  /**
   * @note: rdi is a caller-saved register too, but it's used in function calls
   * and is hence saved separately on the stack;
   *
   * @see DISTINCT_INTERRUPT_ENTRY
   * @see _ISR_Handler
   */

  uint64_t rax;
  uint64_t rcx;
  uint64_t rdx;
  uint64_t rsi;
  uint64_t r8;
  uint64_t r9;
  uint64_t r10;
  uint64_t r11;

  /*
   * This holds the rsp just before _ISR_Handler is called; it's needed because
   * in the handler, we align the stack to make further calls, and we're not
   * sure how alignment may move the stack-pointer around, leaving no way to get
   * back to the stack, and therefore the interrupt frame.
   */
  uint64_t saved_rsp;

  /* XXX:
   * - FS segment selector for TLS
   * - x87 status word?
   * - MMX?
   * - XMM?
   */
} CPU_Interrupt_frame;

#endif /* !ASM */

#define CPU_INTERRUPT_FRAME_SIZE 72

/*
 * When SMP is enabled, percpuasm.c has a similar assert, but since we use the
 * interrupt frame regardless of SMP, we'll confirm it here.
 */
#ifndef ASM
  RTEMS_STATIC_ASSERT(
    sizeof(CPU_Interrupt_frame) == CPU_INTERRUPT_FRAME_SIZE,
    CPU_INTERRUPT_FRAME_SIZE
  );
#endif

#define CPU_CONTEXT_FP_SIZE sizeof( Context_Control_fp )
#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 0
#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE
#define CPU_STACK_MINIMUM_SIZE          (1024*4)
#define CPU_SIZEOF_POINTER         8
#define CPU_ALIGNMENT              8
#define CPU_HEAP_ALIGNMENT         CPU_ALIGNMENT
#define CPU_STACK_ALIGNMENT        16
#define CPU_INTERRUPT_STACK_ALIGNMENT CPU_CACHE_LINE_BYTES

/*
 *  ISR handler macros
 */

#ifndef ASM

#define _CPU_Initialize_vectors()

#define _CPU_ISR_Enable(_level)                             \
{                                                           \
  amd64_enable_interrupts();                                \
  _level = 0;                                               \
  (void) _level; /* Prevent -Wunused-but-set-variable */    \
}

#define _CPU_ISR_Disable(_level)                            \
{                                                           \
  amd64_enable_interrupts();                                \
  _level = 1;                                               \
  (void) _level; /* Prevent -Wunused-but-set-variable */    \
}

#define _CPU_ISR_Flash(_level)                              \
{                                                           \
  amd64_enable_interrupts();                                \
  amd64_disable_interrupts();                               \
  _level = 1;                                               \
  (void) _level; /* Prevent -Wunused-but-set-variable */    \
}

RTEMS_INLINE_ROUTINE bool _CPU_ISR_Is_enabled(uint32_t level)
{
  return (level & EFLAGS_INTR_ENABLE) != 0;
}

RTEMS_INLINE_ROUTINE void _CPU_ISR_Set_level(uint32_t new_level)
{
  if ( new_level ) {
    amd64_disable_interrupts();
  }
  else {
    amd64_enable_interrupts();
  }
}

RTEMS_INLINE_ROUTINE uint32_t _CPU_ISR_Get_level(void)
{
  uint64_t rflags;

  __asm__ volatile ( "pushf; \
                      popq %0"
                     : "=rm" (rflags)
  );

  uint32_t level = (rflags & EFLAGS_INTR_ENABLE) ? 0 : 1;
  return level;
}

/* end of ISR handler macros */

/* Context handler macros */
#define _CPU_Context_Destroy( _the_thread, _the_context ) \
  { \
  }

void _CPU_Context_Initialize(
  Context_Control *the_context,
  void *stack_area_begin,
  size_t stack_area_size,
  uint32_t new_level,
  void (*entry_point)( void ),
  bool is_fp,
  void *tls_area
);

#define _CPU_Context_Restart_self( _the_context ) \
   _CPU_Context_restore( (_the_context) );

#define _CPU_Context_Initialize_fp( _destination )  \
  { \
   *(*(_destination)) = _CPU_Null_fp_context; \
  }

/* end of Context handler macros */

/* Fatal Error manager macros */

#define _CPU_Fatal_halt( _source, _error ) \
  { \
  }

/* end of Fatal Error manager macros */

/* Bitfield handler macros */

#define CPU_USE_GENERIC_BITFIELD_CODE TRUE

#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)
#define _CPU_Bitfield_Find_first_bit( _value, _output ) \
  { \
    (_output) = 0;   /* do something to prevent warnings */ \
  }
#endif

/* end of Bitfield handler macros */

#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)
#define _CPU_Priority_Mask( _bit_number ) \
  ( 1 << (_bit_number) )
#endif

#if (CPU_USE_GENERIC_BITFIELD_CODE == FALSE)
#define _CPU_Priority_bits_index( _priority ) \
  (_priority)
#endif

/* end of Priority handler macros */

/* functions */

void _CPU_Initialize(void);

void *_CPU_Thread_Idle_body( uintptr_t ignored );

void _CPU_Context_switch(
  Context_Control  *run,
  Context_Control  *heir
);

void _CPU_Context_restore(
  Context_Control *new_context
) RTEMS_NO_RETURN;

void _CPU_Context_save_fp(
  Context_Control_fp **fp_context_ptr
);

void _CPU_Context_restore_fp(
  Context_Control_fp **fp_context_ptr
);

typedef struct {
  uint32_t processor_state_register;
  uint32_t integer_registers [1];
  double float_registers [1];
} CPU_Exception_frame;

void _CPU_Exception_frame_print( const CPU_Exception_frame *frame );

static inline uint32_t CPU_swap_u32(
  uint32_t value
)
{
  uint32_t byte1, byte2, byte3, byte4, swapped;

  byte4 = (value >> 24) & 0xff;
  byte3 = (value >> 16) & 0xff;
  byte2 = (value >> 8)  & 0xff;
  byte1 =  value        & 0xff;

  swapped = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
  return swapped;
}

#define CPU_swap_u16( value ) \
  (((value&0xff) << 8) | ((value >> 8)&0xff))

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

#ifdef RTEMS_SMP
   *
  uint32_t _CPU_SMP_Initialize( void );

  bool _CPU_SMP_Start_processor( uint32_t cpu_index );

  void _CPU_SMP_Finalize_initialization( uint32_t cpu_count );

  void _CPU_SMP_Prepare_start_multitasking( void );

  static inline uint32_t _CPU_SMP_Get_current_processor( void )
  {
    return 123;
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
    return context->is_executing;
  }

  static inline void _CPU_Context_Set_is_executing(
    Context_Control *context,
    bool is_executing
  )
  {
  }

#endif /* RTEMS_SMP */

typedef uintptr_t CPU_Uint32ptr;

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif /* _RTEMS_SCORE_CPU_H */
