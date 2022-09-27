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

#define MICROBLAZE_EXCEPTION_FRAME_R1    0
#define MICROBLAZE_EXCEPTION_FRAME_R2    4
#define MICROBLAZE_EXCEPTION_FRAME_R3    8
#define MICROBLAZE_EXCEPTION_FRAME_R4   12
#define MICROBLAZE_EXCEPTION_FRAME_R5   16
#define MICROBLAZE_EXCEPTION_FRAME_R6   20
#define MICROBLAZE_EXCEPTION_FRAME_R7   24
#define MICROBLAZE_EXCEPTION_FRAME_R8   28
#define MICROBLAZE_EXCEPTION_FRAME_R9   32
#define MICROBLAZE_EXCEPTION_FRAME_R10  36
#define MICROBLAZE_EXCEPTION_FRAME_R11  40
#define MICROBLAZE_EXCEPTION_FRAME_R12  44
#define MICROBLAZE_EXCEPTION_FRAME_R13  48
#define MICROBLAZE_EXCEPTION_FRAME_R14  52
#define MICROBLAZE_EXCEPTION_FRAME_R15  56
#define MICROBLAZE_EXCEPTION_FRAME_R16  60
#define MICROBLAZE_EXCEPTION_FRAME_R17  64
#define MICROBLAZE_EXCEPTION_FRAME_R18  68
#define MICROBLAZE_EXCEPTION_FRAME_R19  72
#define MICROBLAZE_EXCEPTION_FRAME_R20  76
#define MICROBLAZE_EXCEPTION_FRAME_R21  80
#define MICROBLAZE_EXCEPTION_FRAME_R22  84
#define MICROBLAZE_EXCEPTION_FRAME_R23  88
#define MICROBLAZE_EXCEPTION_FRAME_R24  92
#define MICROBLAZE_EXCEPTION_FRAME_R25  96
#define MICROBLAZE_EXCEPTION_FRAME_R26 100
#define MICROBLAZE_EXCEPTION_FRAME_R27 104
#define MICROBLAZE_EXCEPTION_FRAME_R28 108
#define MICROBLAZE_EXCEPTION_FRAME_R29 112
#define MICROBLAZE_EXCEPTION_FRAME_R30 116
#define MICROBLAZE_EXCEPTION_FRAME_R31 120
#define MICROBLAZE_EXCEPTION_FRAME_MSR 124
#define MICROBLAZE_EXCEPTION_FRAME_EAR 128
#define MICROBLAZE_EXCEPTION_FRAME_ESR 132
#define MICROBLAZE_EXCEPTION_FRAME_BTR 136

#define CPU_EXCEPTION_FRAME_SIZE 140

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
  void *thread_pointer;
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

/*
 * bit definitions in the documentation are reversed for all special registers
 * such that bit 0 is the most significant bit
 */
#define MICROBLAZE_MSR_VM  ( 1 << 13 )
#define MICROBLAZE_MSR_UM  ( 1 << 11 )
#define MICROBLAZE_MSR_PVR ( 1 << 10 )
#define MICROBLAZE_MSR_EIP ( 1 << 9 )
#define MICROBLAZE_MSR_EE  ( 1 << 8 )
#define MICROBLAZE_MSR_DCE ( 1 << 7 )
#define MICROBLAZE_MSR_DZO ( 1 << 6 )
#define MICROBLAZE_MSR_ICE ( 1 << 5 )
#define MICROBLAZE_MSR_FSL ( 1 << 4 )
#define MICROBLAZE_MSR_BIP ( 1 << 3 )
#define MICROBLAZE_MSR_C   ( 1 << 2 )
#define MICROBLAZE_MSR_IE  ( 1 << 1 )

#define MICROBLAZE_ESR_DS  ( 1 << 12 )
#define MICROBLAZE_ESR_EC_MASK   0x1f
#define MICROBLAZE_ESR_ESS_MASK  0x7f
#define MICROBLAZE_ESR_ESS_SHIFT 5

#define _CPU_MSR_GET( _msr_value ) \
  do { \
    (_msr_value) = 0; \
    __asm__ volatile ("mfs %0, rmsr" : "=&r" ((_msr_value)) : "0" ((_msr_value))); \
  } while (0)

#define _CPU_MSR_SET( _msr_value ) \
{ __asm__ volatile ("mts rmsr, %0" : "=&r" ((_msr_value)) : "0" ((_msr_value))); }

#define MICROBLAZE_PVR0_VERSION_GET( _pvr0_value ) \
  ( ( _pvr0_value >> 8 ) & 0xff )

#define _CPU_PVR0_GET( _pvr0_value ) \
  do { \
    ( _pvr0_value ) = 0; \
    __asm__ volatile ( "mfs %0, rpvr0" : "=&r" ( ( _pvr0_value ) ) ); \
  } while ( 0 )

#define MICROBLAZE_PVR3_BP_GET( _pvr3_value ) \
  ( ( _pvr3_value >> 25 ) & 0xf )

#define MICROBLAZE_PVR3_RWP_GET( _pvr3_value ) \
  ( ( _pvr3_value >> 19 ) & 0x7 )

#define MICROBLAZE_PVR3_WWP_GET( _pvr3_value ) \
  ( ( _pvr3_value >> 13 ) & 0x7 )

#define _CPU_PVR3_GET( _pvr3_value ) \
  do { \
    ( _pvr3_value ) = 0; \
    __asm__ volatile ( "mfs %0, rpvr3" : "=&r" ( ( _pvr3_value ) ) ); \
  } while ( 0 )

#define _CPU_ISR_Disable( _isr_cookie ) \
  { \
    unsigned int _new_msr;  \
    _CPU_MSR_GET(_isr_cookie); \
    _new_msr = (_isr_cookie) & ~(MICROBLAZE_MSR_IE); \
    _CPU_MSR_SET(_new_msr); \
  }

#define _CPU_ISR_Enable( _isr_cookie )  \
  { \
    uint32_t _microblaze_interrupt_enable; \
    uint32_t _microblaze_switch_reg; \
    \
    _microblaze_interrupt_enable = (_isr_cookie) & (MICROBLAZE_MSR_IE); \
    _CPU_MSR_GET(_microblaze_switch_reg); \
    _microblaze_switch_reg &= ~(MICROBLAZE_MSR_IE); \
    _microblaze_switch_reg |= _microblaze_interrupt_enable; \
    _CPU_MSR_SET(_microblaze_switch_reg); \
  }

#define _CPU_ISR_Flash( _isr_cookie ) \
  { \
    unsigned int _new_msr;  \
    _CPU_MSR_SET(_isr_cookie); \
    _new_msr = (_isr_cookie) & ~(MICROBLAZE_MSR_IE); \
    _CPU_MSR_SET(_new_msr); \
  }

void _CPU_ISR_Set_level( uint32_t level );

uint32_t _CPU_ISR_Get_level( void );

static inline bool _CPU_ISR_Is_enabled( uint32_t level )
{
  return ( level & MICROBLAZE_MSR_IE ) != 0;
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
  /* r0 is unnecessary since it is always 0 */
  uint32_t r1;
  uint32_t r2;
  uint32_t r3; /* return 1/scratch */
  uint32_t r4; /* return 2/scratch */
  uint32_t r5; /* param 1/scratch */
  uint32_t r6; /* param 2/scratch */
  uint32_t r7; /* param 3/scratch */
  uint32_t r8; /* param 4/scratch */
  uint32_t r9; /* param 5/scratch */
  uint32_t r10; /* param 6/scratch */
  uint32_t r11; /* scratch */
  uint32_t r12; /* scratch */
  uint32_t r13;
  uint32_t *r14; /* Interrupt Link Register */
  uint32_t *r15; /* Link Register */
  uint32_t *r16; /* Trap/Debug Link Register */
  uint32_t *r17; /* Exception Link Register */
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
  uint32_t msr; /* Machine Status Register */
  uint32_t *ear; /* Exception Address Register */
  uint32_t esr; /* Exception Status Register */
  uint32_t *btr; /* Branch Target Register */
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

typedef void ( *MicroBlaze_Exception_handler )( CPU_Exception_frame *ef );

void _MicroBlaze_Exception_install_handler(
  MicroBlaze_Exception_handler  new_handler,
  MicroBlaze_Exception_handler *old_handler
);

void _MicroBlaze_Exception_handle(
  CPU_Exception_frame *ef
);

void _MicroBlaze_Debug_install_handler(
  MicroBlaze_Exception_handler  new_handler,
  MicroBlaze_Exception_handler *old_handler
);

void _MicroBlaze_Debug_handle(
  CPU_Exception_frame *ef
);

void _CPU_Context_switch(
  Context_Control  *run,
  Context_Control  *heir
);

/* Selects the appropriate resume function based on CEF state */
RTEMS_NO_RETURN void _CPU_Exception_resume( CPU_Exception_frame *frame );

RTEMS_NO_RETURN void _MicroBlaze_Exception_resume_from_exception(
  CPU_Exception_frame *frame
);

RTEMS_NO_RETURN void _MicroBlaze_Exception_resume_from_break(
  CPU_Exception_frame *frame
);

/*
 * Only functions for exception cases since debug exception frames will never
 * need dispatch
 */
RTEMS_NO_RETURN void _CPU_Exception_dispatch_and_resume(
  CPU_Exception_frame *frame
);

void _CPU_Exception_disable_thread_dispatch( void );

int _CPU_Exception_frame_get_signal( CPU_Exception_frame *frame );

void _CPU_Exception_frame_set_resume(
  CPU_Exception_frame *frame,
  void                *address
);

void _CPU_Exception_frame_make_resume_next_instruction(
  CPU_Exception_frame *frame
);

uint32_t *_MicroBlaze_Get_return_address( CPU_Exception_frame *ef );

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

void *_CPU_Thread_Idle_body( uintptr_t ignored );

void bsp_interrupt_dispatch( uint32_t source );

/** Type that can store a 32-bit integer or a pointer. */
typedef uintptr_t CPU_Uint32ptr;

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif /* _RTEMS_SCORE_CPU_H */
