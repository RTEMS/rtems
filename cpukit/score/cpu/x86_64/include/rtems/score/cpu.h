/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief x86_64 Dependent Source
 *
 * This include file contains information pertaining to the x86_64 processor.
 */

/*
 * Copyright (C) 2024 Matheus Pecoraro
 * Copyright (c) 2018 Amaan Cheval <amaan.cheval@gmail.com>
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
#define CPU_HARDWARE_FP TRUE
#define CPU_SOFTWARE_FP FALSE
#define CPU_ALL_TASKS_ARE_FP TRUE
#define CPU_IDLE_TASK_IS_FP FALSE
#define CPU_USE_DEFERRED_FP_SWITCH FALSE
#define CPU_ENABLE_ROBUST_THREAD_DISPATCH FALSE
#define CPU_STACK_GROWS_UP               FALSE

#define CPU_STRUCTURE_ALIGNMENT RTEMS_ALIGNED(64)
#define CPU_CACHE_LINE_BYTES 64
#define CPU_MODES_INTERRUPT_MASK   0x00000001
#define CPU_MAXIMUM_PROCESSORS 32

#define CPU_EFLAGS_INTERRUPTS_ON  0x00003202
#define CPU_EFLAGS_INTERRUPTS_OFF 0x00003002

#define CPU_CONTEXT_CONTROL_EFLAGS 0

#define CPU_CONTEXT_CONTROL_RBX CPU_CONTEXT_CONTROL_EFLAGS + 8
#define CPU_CONTEXT_CONTROL_RSP CPU_CONTEXT_CONTROL_RBX + 8
#define CPU_CONTEXT_CONTROL_RBP CPU_CONTEXT_CONTROL_RSP + 8
#define CPU_CONTEXT_CONTROL_R12 CPU_CONTEXT_CONTROL_RBP + 8
#define CPU_CONTEXT_CONTROL_R13 CPU_CONTEXT_CONTROL_R12 + 8
#define CPU_CONTEXT_CONTROL_R14 CPU_CONTEXT_CONTROL_R13 + 8
#define CPU_CONTEXT_CONTROL_R15 CPU_CONTEXT_CONTROL_R14 + 8

#define CPU_CONTEXT_CONTROL_FS CPU_CONTEXT_CONTROL_R15 + 8

#define CPU_CONTEXT_CONTROL_ISR_DISPATCH_DISABLE CPU_CONTEXT_CONTROL_FS + 8

#define CPU_CONTEXT_CONTROL_IS_EXECUTING CPU_CONTEXT_CONTROL_ISR_DISPATCH_DISABLE + 4

#ifndef ASM

typedef struct {
  uint64_t rflags;

  /**
   * Callee-saved registers as listed in the SysV ABI document:
   * https://gitlab.com/x86-psABIs/x86-64-ABI
   */
  uint64_t rbx;
  void    *rsp;
  void    *rbp;
  uint64_t r12;
  uint64_t r13;
  uint64_t r14;
  uint64_t r15;

  /* Thread pointer */
  uint64_t fs;

  uint32_t isr_dispatch_disable;

#ifdef RTEMS_SMP
  volatile uint16_t is_executing;
#endif
} Context_Control;

typedef struct {
  /**
   * Callee-saved FP registers as listed in the SysV ABI document:
   * https://gitlab.com/x86-psABIs/x86-64-ABI
   */
  uint32_t  mxcsr;
  uint16_t  fpucw;
} Context_Control_fp;

#define _CPU_Context_Get_SP( _context ) \
  (uintptr_t)(_context)->rsp

#endif /* !ASM */

#define CPU_INTERRUPT_FRAME_SSE_STATE 0

#define CPU_INTERRUPT_FRAME_RAX CPU_INTERRUPT_FRAME_SSE_STATE + 512
#define CPU_INTERRUPT_FRAME_RCX CPU_INTERRUPT_FRAME_RAX + 8
#define CPU_INTERRUPT_FRAME_RDX CPU_INTERRUPT_FRAME_RCX + 8
#define CPU_INTERRUPT_FRAME_RSI CPU_INTERRUPT_FRAME_RDX + 8
#define CPU_INTERRUPT_FRAME_R8  CPU_INTERRUPT_FRAME_RSI + 8
#define CPU_INTERRUPT_FRAME_R9  CPU_INTERRUPT_FRAME_R8 + 8
#define CPU_INTERRUPT_FRAME_R10 CPU_INTERRUPT_FRAME_R9 + 8
#define CPU_INTERRUPT_FRAME_R11 CPU_INTERRUPT_FRAME_R10 + 8
#define CPU_INTERRUPT_FRAME_RSP CPU_INTERRUPT_FRAME_R11 + 8

#ifndef ASM

/*
 * Caller-saved registers for interrupt frames
 */
typedef struct {
  /* Registers saved by CPU */
  uint64_t error_code; /* only in some exceptions */
  uint64_t rip;
  uint64_t cs;
  uint64_t rflags;
  uint64_t rsp;
  uint64_t ss;

  /* Saved in rtems_irq_prologue_* */
  uint64_t rdi;
  uint64_t rbp;
  uint64_t rbx;

  /* SSE state saved by the FXSAVE instruction */
  uint8_t sse_state[512];

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

  /**
   * The caller-saved registers needs to start in a 16-byte aligned position
   * on the stack for the FXSAVE instruction. Therefore, we have 8 extra bytes
   * in case the interrupt handler needs to align it.
   */
  uint8_t padding[8];
} CPU_Interrupt_frame;

extern Context_Control_fp _CPU_Null_fp_context;

#define CPU_CONTEXT_FP_SIZE sizeof( Context_Control_fp )

#endif /* !ASM */

#define CPU_INTERRUPT_FRAME_X86_64_SIZE       48
#define CPU_INTERRUPT_FRAME_PROLOGUE_SIZE     24
#define CPU_INTERRUPT_FRAME_CALLER_SAVED_SIZE (512 + 72)
#define CPU_INTERRUPT_FRAME_PADDING_SIZE      8

#define CPU_INTERRUPT_FRAME_SIZE           \
  (CPU_INTERRUPT_FRAME_X86_64_SIZE       + \
   CPU_INTERRUPT_FRAME_PROLOGUE_SIZE     + \
   CPU_INTERRUPT_FRAME_CALLER_SAVED_SIZE + \
   CPU_INTERRUPT_FRAME_PADDING_SIZE)

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

#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 0
#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE
#define CPU_STACK_MINIMUM_SIZE          (1024*8)
#define CPU_SIZEOF_POINTER         8
#define CPU_ALIGNMENT              16
#define CPU_HEAP_ALIGNMENT         CPU_ALIGNMENT
#define CPU_STACK_ALIGNMENT        16
#define CPU_INTERRUPT_STACK_ALIGNMENT CPU_CACHE_LINE_BYTES

/*
 *  ISR handler macros
 */

/* ISR_Level is an uint32_t meaning we can't use RFLAGS to represent it */
#define CPU_ISR_LEVEL_ENABLED 0

#ifndef ASM

#define _CPU_ISR_Enable(_level)                             \
{                                                           \
  if (_level == 0) {                                        \
    amd64_enable_interrupts();                              \
  }                                                         \
}

#define _CPU_ISR_Disable(_level)                            \
{                                                           \
  _level = _CPU_ISR_Get_level();                            \
  amd64_disable_interrupts();                               \
}

#define _CPU_ISR_Flash(_level)                              \
{                                                           \
  if (_level == 0) {                                        \
    amd64_enable_interrupts();                              \
  }                                                         \
  amd64_disable_interrupts();                               \
}

static inline bool _CPU_ISR_Is_enabled(uint32_t level)
{
  return level == 0;
}

static inline void _CPU_ISR_Set_level(uint32_t new_level)
{
  if ( new_level ) {
    amd64_disable_interrupts();
  }
  else {
    amd64_enable_interrupts();
  }
}

static inline uint32_t _CPU_ISR_Get_level(void)
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

#define CPU_USE_LIBC_INIT_FINI_ARRAY TRUE

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

RTEMS_NO_RETURN void *_CPU_Thread_Idle_body( uintptr_t ignored );

void _CPU_Context_switch(
  Context_Control  *run,
  Context_Control  *heir
);

RTEMS_NO_RETURN void _CPU_Context_switch_no_return(
  Context_Control *executing,
  Context_Control *heir
);

RTEMS_NO_RETURN void _CPU_Context_restore( Context_Control *new_context );

typedef struct {
  uint32_t processor_state_register;
  uint32_t integer_registers [1];
  double float_registers [1];
} CPU_Exception_frame;

void _CPU_Context_save_fp(
  Context_Control_fp **fp_context_ptr
);

void _CPU_Context_restore_fp(
  Context_Control_fp **fp_context_ptr
);

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

#define _CPU_Context_save_fp(fp_context_pp) \
  do {                                      \
    __asm__ __volatile__(                   \
      "fstcw %0"                            \
      :"=m"((*(fp_context_pp))->fpucw)      \
    );                                      \
	__asm__ __volatile__(                     \
      "stmxcsr %0"                          \
      :"=m"((*(fp_context_pp))->mxcsr)      \
    );                                      \
  } while (0)

#define _CPU_Context_restore_fp(fp_context_pp) \
  do {                                         \
    __asm__ __volatile__(                      \
      "fldcw %0"                               \
      :"=m"((*(fp_context_pp))->fpucw)         \
    );                                         \
  __asm__ __volatile__(                        \
      "ldmxcsr %0"                             \
      :"=m"((*(fp_context_pp))->mxcsr)         \
    );                                         \
  } while (0)

#define CPU_swap_u16( value ) \
  (((value&0xff) << 8) | ((value >> 8)&0xff))

typedef uint32_t CPU_Counter_ticks;

uint32_t _CPU_Counter_frequency( void );

CPU_Counter_ticks _CPU_Counter_read( void );

#ifdef RTEMS_SMP

  uint32_t _CPU_SMP_Initialize( void );

  bool _CPU_SMP_Start_processor( uint32_t cpu_index );

  void _CPU_SMP_Finalize_initialization( uint32_t cpu_count );

  void _CPU_SMP_Prepare_start_multitasking( void );

  uint32_t _CPU_SMP_Get_current_processor( void );

  void _CPU_SMP_Send_interrupt( uint32_t target_processor_index );

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

typedef uintptr_t CPU_Uint32ptr;

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif /* _RTEMS_SCORE_CPU_H */
