/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPU
 *
 * @brief MicroBlaze architecture support
 */

/*
 * Copyright (c) 2015, Hesham Almatary
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTEMS_SCORE_CPU_H
#define _RTEMS_SCORE_CPU_H

#include <rtems/score/basedefs.h>
#include <rtems/score/microblaze.h>

#define CPU_SIMPLE_VECTORED_INTERRUPTS TRUE

#define CPU_ISR_PASSES_FRAME_POINTER FALSE

#define CPU_HARDWARE_FP FALSE

#define CPU_SOFTWARE_FP FALSE

#define CPU_ALL_TASKS_ARE_FP FALSE

#define CPU_IDLE_TASK_IS_FP FALSE

#define CPU_USE_DEFERRED_FP_SWITCH FALSE

#define CPU_STACK_GROWS_UP FALSE

/**
 * The maximum cache-line size is 16 words.
 */
#define CPU_CACHE_LINE_BYTES 64

#define CPU_STRUCTURE_ALIGNMENT

#define CPU_MODES_INTERRUPT_MASK 0x00000001

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @ingroup CPUContext Management
 *  This defines the minimal set of integer and processor state registers
 *  that must be saved during a voluntary context switch from one thread
 *  to another.
 */
typedef struct {
  uint32_t r1;
  uint32_t r13;
  uint32_t r14;
  uint32_t r15;
  uint32_t r16;
  uint32_t r17;
  uint32_t r18;
  uint32_t r19;
  uint32_t r20;
  uint32_t r21;
  uint32_t r22;
  uint32_t r23;
  uint32_t r24;
  uint32_t r25;
  uint32_t r26;
  uint32_t r27;
  uint32_t r28;
  uint32_t r29;
  uint32_t r30;
  uint32_t r31;
  uint32_t rmsr;
} Context_Control;

/**
 *  @ingroup CPUContext Management
 *
 *  This macro returns the stack pointer associated with @a _context.
 *
 *  @param[in] _context is the thread context area to access
 *
 *  @return This method returns the stack pointer.
 */
#define _CPU_Context_Get_SP( _context ) \
  (_context)->r1

#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 0

#define CPU_INTERRUPT_NUMBER_OF_VECTORS 32

#define CPU_USE_LIBC_INIT_FINI_ARRAY TRUE

#define CPU_MAXIMUM_PROCESSORS 32

/**
 *  @ingroup CPUInterrupt
 *  This defines the highest interrupt vector number for this port.
 */
#define CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER (CPU_INTERRUPT_NUMBER_OF_VECTORS - 1)

#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE

#define CPU_STACK_MINIMUM_SIZE (1024*4)

#define CPU_ALIGNMENT 4

#define CPU_HEAP_ALIGNMENT CPU_ALIGNMENT

#define CPU_STACK_ALIGNMENT        CPU_ALIGNMENT

#define CPU_INTERRUPT_STACK_ALIGNMENT CPU_CACHE_LINE_BYTES

#define MICROBLAZE_MSR_IE (1 << 1)
#define MICROBLAZE_MSR_EE (1 << 8)

#define _CPU_MSR_GET( _msr_value ) \
  do { \
    (_msr_value) = 0; \
    __asm__ volatile ("mfs %0, rmsr" : "=&r" ((_msr_value)) : "0" ((_msr_value))); \
  } while (0)

#define _CPU_MSR_SET( _msr_value ) \
{ __asm__ volatile ("mts rmsr, %0" : "=&r" ((_msr_value)) : "0" ((_msr_value))); }

#define _CPU_ISR_Disable( _isr_cookie ) \
  { \
    unsigned int _new_msr;  \
    _CPU_MSR_GET(_isr_cookie); \
    _new_msr = (_isr_cookie) & ~(MICROBLAZE_MSR_IE | MICROBLAZE_MSR_EE); \
    _CPU_MSR_SET(_new_msr); \
  }

#define _CPU_ISR_Enable( _isr_cookie )  \
  { \
    uint32_t _microblaze_interrupt_enable; \
    uint32_t _microblaze_switch_reg; \
    \
    _microblaze_interrupt_enable = (_isr_cookie) & (MICROBLAZE_MSR_IE | MICROBLAZE_MSR_EE); \
    _CPU_MSR_GET(_microblaze_switch_reg); \
    _microblaze_switch_reg &= ~(MICROBLAZE_MSR_IE | MICROBLAZE_MSR_EE); \
    _microblaze_switch_reg |= _microblaze_interrupt_enable; \
    _CPU_MSR_SET(_microblaze_switch_reg); \
  }

#define _CPU_ISR_Flash( _isr_cookie ) \
  { \
    unsigned int _new_msr;  \
    _CPU_MSR_SET(_isr_cookie); \
    _new_msr = (_isr_cookie) & ~(MICROBLAZE_MSR_IE | MICROBLAZE_MSR_EE); \
    _CPU_MSR_SET(_new_msr); \
  }

void _CPU_ISR_Set_level( uint32_t level );

uint32_t _CPU_ISR_Get_level( void );

RTEMS_INLINE_ROUTINE bool _CPU_ISR_Is_enabled( uint32_t level )
{
  return ( level & (MICROBLAZE_MSR_IE | MICROBLAZE_MSR_EE) ) != 0;
}

void _CPU_Context_Initialize(
  Context_Control *context,
  void *stack_area_begin,
  size_t stack_area_size,
  uint32_t new_level,
  void (*entry_point)( void ),
  bool is_fp,
  void *tls_area
);

#define _CPU_Context_Restart_self( _the_context ) \
   _CPU_Context_restore( (_the_context) );

#define _CPU_Context_Initialize_fp( _destination ) \
  { \
   *(*(_destination)) = _CPU_Null_fp_context; \
  }

/* end of Context handler macros */

/* Fatal Error manager macros */

/* TODO */
#define _CPU_Fatal_halt(_source, _error ) \
        do { \
          __asm__ volatile ( "sleep" ); \
          for(;;) {} \
        } while (0)

/* end of Fatal Error manager macros */

/* Bitfield handler macros */

#define CPU_USE_GENERIC_BITFIELD_CODE TRUE

#define CPU_SIZEOF_POINTER 4

#define CPU_PER_CPU_CONTROL_SIZE 0

typedef struct {
  /* TODO: enumerate registers */
  uint32_t r[32];
} CPU_Exception_frame;

/* end of Priority handler macros */

/* functions */

void _CPU_Initialize( void );

typedef void ( *CPU_ISR_handler )( uint32_t );

void _CPU_ISR_install_vector(
  uint32_t         vector,
  CPU_ISR_handler  new_handler,
  CPU_ISR_handler *old_handler
);

void _CPU_Context_switch(
  Context_Control  *run,
  Context_Control  *heir
);

RTEMS_NO_RETURN void _CPU_Context_restore(
  Context_Control *new_context
);

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

void _CPU_Exception_frame_print( const CPU_Exception_frame *frame );

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

void *_CPU_Thread_Idle_body( uintptr_t ignored );

void bsp_interrupt_dispatch( uint32_t source );

/** Type that can store a 32-bit integer or a pointer. */
typedef uintptr_t CPU_Uint32ptr;

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif /* _RTEMS_SCORE_CPU_H */
