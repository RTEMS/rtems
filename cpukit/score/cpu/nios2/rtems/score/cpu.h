/*
 *  Copyright (c) 2011 embedded brains GmbH
 *
 *  Copyright (c) 2006 Kolja Waschk (rtemsdev/ixo.de)
 *
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_CPU_H
#define _RTEMS_SCORE_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/types.h>
#include <rtems/score/nios2.h>

/*
 * TODO: Run the timing tests and figure out what is better.
 */
#define CPU_INLINE_ENABLE_DISPATCH FALSE

/*
 * TODO: Run the timing tests and figure out what is better.
 */
#define CPU_UNROLL_ENQUEUE_PRIORITY TRUE

#define CPU_HAS_SOFTWARE_INTERRUPT_STACK TRUE

#define CPU_SIMPLE_VECTORED_INTERRUPTS TRUE

#define CPU_INTERRUPT_NUMBER_OF_VECTORS 32

#define CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER (CPU_INTERRUPT_NUMBER_OF_VECTORS - 1)

#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE

#define CPU_HAS_HARDWARE_INTERRUPT_STACK FALSE

#define CPU_ALLOCATE_INTERRUPT_STACK TRUE

#define CPU_ISR_PASSES_FRAME_POINTER 1

#define CPU_HARDWARE_FP FALSE

#define CPU_SOFTWARE_FP FALSE

#define CPU_CONTEXT_FP_SIZE 0

#define CPU_ALL_TASKS_ARE_FP FALSE

#define CPU_IDLE_TASK_IS_FP FALSE

#define CPU_USE_DEFERRED_FP_SWITCH FALSE

#define CPU_PROVIDES_IDLE_THREAD_BODY FALSE

#define CPU_STACK_GROWS_UP FALSE

/*
 * TODO: Run the timing tests and figure out if we profit from cache alignment.
 */
#define CPU_STRUCTURE_ALIGNMENT

#define CPU_BIG_ENDIAN FALSE

#define CPU_LITTLE_ENDIAN TRUE

#define CPU_STACK_MINIMUM_SIZE (4 * 1024)

/*
 * Alignment value according to "Nios II Processor Reference" chapter 7
 * "Application Binary Interface" section "Memory Alignment".
 */
#define CPU_ALIGNMENT 4

#define CPU_HEAP_ALIGNMENT CPU_ALIGNMENT

#define CPU_PARTITION_ALIGNMENT CPU_ALIGNMENT

#define CPU_STACK_ALIGNMENT CPU_ALIGNMENT

#define CPU_MODES_INTERRUPT_MASK 0x1

#define CPU_USE_GENERIC_BITFIELD_CODE TRUE

#define CPU_USE_GENERIC_BITFIELD_DATA TRUE

#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 0

#ifndef ASM

/**
 * @brief Thread register context.
 *
 * The thread register context covers the non-volatile registers, the thread
 * stack pointer, the return address, and the processor status.
 *
 * There is no need to save the global pointer (gp) since it is a system wide
 * constant and set-up with the C runtime environment.
 */
typedef struct {
  uint32_t r16;
  uint32_t r17;
  uint32_t r18;
  uint32_t r19;
  uint32_t r20;
  uint32_t r21;
  uint32_t r22;
  uint32_t r23;
  uint32_t fp;
  uint32_t sp;
  uint32_t ra;
  uint32_t status;
} Context_Control;

#define _CPU_Context_Get_SP( _context ) \
  (_context)->sp

typedef struct {
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t r4;
  uint32_t r5;
  uint32_t r6;
  uint32_t r7;
  uint32_t r8;
  uint32_t r9;
  uint32_t r10;
  uint32_t r11;
  uint32_t r12;
  uint32_t r13;
  uint32_t r14;
  uint32_t r15;
  uint32_t ra;
  uint32_t gp;
  uint32_t et;
  uint32_t ea;
} CPU_Interrupt_frame;

typedef struct {
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t r4;
  uint32_t r5;
  uint32_t r6;
  uint32_t r7;
  uint32_t r8;
  uint32_t r9;
  uint32_t r10;
  uint32_t r11;
  uint32_t r12;
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
  uint32_t gp;
  uint32_t fp;
  uint32_t sp;
  uint32_t ra;
  uint32_t et;
  uint32_t ea;
  uint32_t status;
  uint32_t ienable;
  uint32_t ipending;
} CPU_Exception_frame;

#define _CPU_Initialize_vectors()

#define _CPU_ISR_Disable( _isr_cookie ) \
  do { \
    _isr_cookie = __builtin_rdctl( 0 ); \
    __builtin_wrctl( 0, 0 ); \
  } while ( 0 )

#define _CPU_ISR_Enable( _isr_cookie ) \
  do { \
    __builtin_wrctl( 0, (int) _isr_cookie ); \
  } while ( 0 )

#define _CPU_ISR_Flash( _isr_cookie ) \
  do { \
    __builtin_wrctl( 0, (int) _isr_cookie ); \
    __builtin_wrctl( 0, 0 ); \
  } while ( 0 )

#define _CPU_ISR_Set_level( new_level ) \
  _CPU_ISR_Enable( new_level == 0 ? 1 : 0 );

uint32_t _CPU_ISR_Get_level( void );

/*
 * FIXME: Evaluate interrupt level.
 */
#define _CPU_Context_Initialize( _the_context, _stack_base, _size, \
                                 _isr, _entry_point, _is_fp ) \
  do { \
    uint32_t _stack = (uint32_t)(_stack_base) + (_size) - 4; \
    (_the_context)->fp = (void *)_stack; \
    (_the_context)->sp = (void *)_stack; \
    (_the_context)->ra = (void *)(_entry_point); \
    (_the_context)->status  = 0x1; /* IRQs enabled */ \
  } while ( 0 )

#define _CPU_Context_Restart_self( _the_context ) \
  _CPU_Context_restore( (_the_context) );

#define _CPU_Fatal_halt( _error ) \
  do { \
    __builtin_wrctl(0, 0); /* write 0 to status register (disable interrupts) */ \
    __asm volatile ("mov et, %z0" : : "rM" (_error)); /* write error code to ET register */ \
    for (;;); \
  } while ( 0 )

void _CPU_Initialize( void );

void _CPU_ISR_install_raw_handler(
  uint32_t vector,
  proc_ptr new_handler,
  proc_ptr *old_handler
);

void _CPU_ISR_install_vector(
  uint32_t vector,
  proc_ptr new_handler,
  proc_ptr *old_handler
);

void _CPU_Context_switch( Context_Control *run, Context_Control *heir );

void _CPU_Context_restore(
  Context_Control *new_context
) RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

static inline uint32_t CPU_swap_u32( uint32_t value )
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

#endif /* ASM */

#ifdef __cplusplus
}
#endif

#endif
