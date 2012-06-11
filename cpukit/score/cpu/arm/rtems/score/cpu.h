/**
 * @file
 *
 * @ingroup ScoreCPU
 *
 * @brief ARM architecture support API.
 */

/*
 *  This include file contains information pertaining to the ARM
 *  processor.
 *
 *  Copyright (c) 2009-2011 embedded brains GmbH.
 *
 *  Copyright (c) 2007 Ray Xu <Rayx.cn@gmail.com>
 *
 *  Copyright (c) 2006 OAR Corporation
 *
 *  Copyright (c) 2002 Advent Networks, Inc.
 *        Jay Monkman <jmonkman@adventnetworks.com>
 *
 *  COPYRIGHT (c) 2000 Canon Research Centre France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#ifndef _RTEMS_SCORE_CPU_H
#define _RTEMS_SCORE_CPU_H

#include <rtems/score/types.h>
#include <rtems/score/arm.h>

#if defined(ARM_MULTILIB_ARCH_V4)

/**
 * @defgroup ScoreCPUARM ARM Specific Support
 *
 * @ingroup ScoreCPU
 *
 * @brief ARM specific support.
 *
 * @{
 */

#ifdef __thumb__
  #define ARM_SWITCH_REGISTERS uint32_t arm_switch_reg
  #define ARM_SWITCH_TO_ARM ".align 2\nbx pc\n.arm\n"
  #define ARM_SWITCH_BACK "add %[arm_switch_reg], pc, #1\nbx %[arm_switch_reg]\n.thumb\n"
  #define ARM_SWITCH_OUTPUT [arm_switch_reg] "=&r" (arm_switch_reg)
  #define ARM_SWITCH_ADDITIONAL_OUTPUT , ARM_SWITCH_OUTPUT
#else
  #define ARM_SWITCH_REGISTERS
  #define ARM_SWITCH_TO_ARM
  #define ARM_SWITCH_BACK
  #define ARM_SWITCH_OUTPUT
  #define ARM_SWITCH_ADDITIONAL_OUTPUT
#endif

/**
 * @name Program Status Register
 *
 * @{
 */

#define ARM_PSR_N (1 << 31)
#define ARM_PSR_Z (1 << 30)
#define ARM_PSR_C (1 << 29)
#define ARM_PSR_V (1 << 28)
#define ARM_PSR_Q (1 << 27)
#define ARM_PSR_J (1 << 24)
#define ARM_PSR_GE_SHIFT 16
#define ARM_PSR_GE_MASK (0xf << ARM_PSR_GE_SHIFT)
#define ARM_PSR_E (1 << 9)
#define ARM_PSR_A (1 << 8)
#define ARM_PSR_I (1 << 7)
#define ARM_PSR_F (1 << 6)
#define ARM_PSR_T (1 << 5)
#define ARM_PSR_M_SHIFT 0
#define ARM_PSR_M_MASK (0x1f << ARM_PSR_M_SHIFT)
#define ARM_PSR_M_USR 0x10
#define ARM_PSR_M_FIQ 0x11
#define ARM_PSR_M_IRQ 0x12
#define ARM_PSR_M_SVC 0x13
#define ARM_PSR_M_ABT 0x17
#define ARM_PSR_M_UND 0x1b
#define ARM_PSR_M_SYS 0x1f

/** @} */

/** @} */

#endif /* defined(ARM_MULTILIB_ARCH_V4) */

/**
 * @addtogroup ScoreCPU
 *
 * @{
 */

/* If someone uses THUMB we assume she wants minimal code size */
#ifdef __thumb__
  #define CPU_INLINE_ENABLE_DISPATCH FALSE
#else
  #define CPU_INLINE_ENABLE_DISPATCH TRUE
#endif

#if defined(__ARMEL__)
  #define CPU_BIG_ENDIAN FALSE
  #define CPU_LITTLE_ENDIAN TRUE
#elif defined(__ARMEB__)
  #define CPU_BIG_ENDIAN TRUE
  #define CPU_LITTLE_ENDIAN FALSE
#else
  #error "unknown endianness"
#endif

#define CPU_UNROLL_ENQUEUE_PRIORITY TRUE

/*
 *  The ARM uses the PIC interrupt model.
 */
#define CPU_SIMPLE_VECTORED_INTERRUPTS FALSE

#define CPU_HAS_SOFTWARE_INTERRUPT_STACK FALSE

#define CPU_HAS_HARDWARE_INTERRUPT_STACK FALSE

#define CPU_ALLOCATE_INTERRUPT_STACK FALSE

#define CPU_ISR_PASSES_FRAME_POINTER 0

#if ( ARM_HAS_FPU == 1 )
  #define CPU_HARDWARE_FP TRUE
#else
  #define CPU_HARDWARE_FP FALSE
#endif

#define CPU_SOFTWARE_FP FALSE

#define CPU_ALL_TASKS_ARE_FP FALSE

#define CPU_IDLE_TASK_IS_FP FALSE

#define CPU_USE_DEFERRED_FP_SWITCH FALSE

#define CPU_PROVIDES_IDLE_THREAD_BODY FALSE

#define CPU_STACK_GROWS_UP FALSE

/* XXX Why 32? */
#define CPU_STRUCTURE_ALIGNMENT __attribute__ ((aligned (32)))

#define CPU_TIMESTAMP_USE_INT64_INLINE TRUE

/*
 * The interrupt mask disables only normal interrupts (IRQ).
 *
 * In order to support fast interrupts (FIQ) such that they can do something
 * useful, we have to disable the operating system support for FIQs.  Having
 * operating system support for them would require that FIQs are disabled
 * during critical sections of the operating system and application.  At this
 * level IRQs and FIQs would be equal.  It is true that FIQs could interrupt
 * the non critical sections of IRQs, so here they would have a small
 * advantage.  Without operating system support, the FIQs can execute at any
 * time (of course not during the service of another FIQ). If someone needs
 * operating system support for a FIQ, she can trigger a software interrupt and
 * service the request in a two-step process.
 */
#define CPU_MODES_INTERRUPT_MASK 0x80

#define CPU_CONTEXT_FP_SIZE sizeof( Context_Control_fp )

#define CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK 0

#define CPU_INTERRUPT_NUMBER_OF_VECTORS 8

#define CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER (CPU_INTERRUPT_NUMBER_OF_VECTORS - 1)

#define CPU_PROVIDES_ISR_IS_IN_PROGRESS FALSE

#define CPU_STACK_MINIMUM_SIZE (1024 * 4)

/* AAPCS, section 4.1, Fundamental Data Types */
#define CPU_ALIGNMENT 8

#define CPU_HEAP_ALIGNMENT CPU_ALIGNMENT

/* AAPCS, section 4.3.1, Aggregates */
#define CPU_PARTITION_ALIGNMENT 4

/* AAPCS, section 5.2.1.2, Stack constraints at a public interface */
#define CPU_STACK_ALIGNMENT 8

/*
 * Bitfield handler macros.
 *
 * If we had a particularly fast function for finding the first
 * bit set in a word, it would go here. Since we don't (*), we'll
 * just use the universal macros.
 *
 * (*) On ARM V5 and later, there's a CLZ function which could be
 *     used to implement much quicker than the default macro.
 */

#define CPU_USE_GENERIC_BITFIELD_CODE TRUE

#define CPU_USE_GENERIC_BITFIELD_DATA TRUE

/** @} */

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreCPU
 *
 * @{
 */

typedef struct {
#if defined(ARM_MULTILIB_ARCH_V4)
  uint32_t register_cpsr;
  uint32_t register_r4;
  uint32_t register_r5;
  uint32_t register_r6;
  uint32_t register_r7;
  uint32_t register_r8;
  uint32_t register_r9;
  uint32_t register_r10;
  uint32_t register_fp;
  uint32_t register_sp;
  uint32_t register_lr;
#elif defined(ARM_MULTILIB_ARCH_V7M)
  uint32_t register_r4;
  uint32_t register_r5;
  uint32_t register_r6;
  uint32_t register_r7;
  uint32_t register_r8;
  uint32_t register_r9;
  uint32_t register_r10;
  uint32_t register_r11;
  void *register_lr;
  void *register_sp;
  uint32_t isr_nest_level;
#else
  void *register_sp;
#endif
} Context_Control;

typedef struct {
  /* Not supported */
} Context_Control_fp;

SCORE_EXTERN Context_Control_fp _CPU_Null_fp_context;

extern uint32_t arm_cpu_mode;

static inline uint32_t arm_interrupt_disable( void )
{
  uint32_t level;

#if defined(ARM_MULTILIB_ARCH_V4)
  uint32_t arm_switch_reg;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrs %[level], cpsr\n"
    "orr %[arm_switch_reg], %[level], #0x80\n"
    "msr cpsr, %[arm_switch_reg]\n"
    ARM_SWITCH_BACK
    : [arm_switch_reg] "=&r" (arm_switch_reg), [level] "=&r" (level)
  );
#elif defined(ARM_MULTILIB_ARCH_V7M)
  uint32_t basepri = 0x80;

  __asm__ volatile (
    "mrs %[level], basepri\n"
    "msr basepri_max, %[basepri]\n"
    : [level] "=&r" (level)
    : [basepri] "r" (basepri)
  );
#else
  level = 0;
#endif

  return level;
}

static inline void arm_interrupt_enable( uint32_t level )
{
#if defined(ARM_MULTILIB_ARCH_V4)
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "msr cpsr, %[level]\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [level] "r" (level)
  );
#elif defined(ARM_MULTILIB_ARCH_V7M)
  __asm__ volatile (
    "msr basepri, %[level]\n"
    :
    : [level] "r" (level)
  );
#endif
}

static inline void arm_interrupt_flash( uint32_t level )
{
#if defined(ARM_MULTILIB_ARCH_V4)
  uint32_t arm_switch_reg;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrs %[arm_switch_reg], cpsr\n"
    "msr cpsr, %[level]\n"
    "msr cpsr, %[arm_switch_reg]\n"
    ARM_SWITCH_BACK
    : [arm_switch_reg] "=&r" (arm_switch_reg)
    : [level] "r" (level)
  );
#elif defined(ARM_MULTILIB_ARCH_V7M)
  uint32_t basepri;

  __asm__ volatile (
    "mrs %[basepri], basepri\n"
    "msr basepri, %[level]\n"
    "msr basepri, %[basepri]\n"
    : [basepri] "=&r" (basepri)
    : [level] "r" (level)
  );
#endif
}

#define _CPU_ISR_Disable( _isr_cookie ) \
  do { \
    _isr_cookie = arm_interrupt_disable(); \
  } while (0)

#define _CPU_ISR_Enable( _isr_cookie )  \
  arm_interrupt_enable( _isr_cookie )

#define _CPU_ISR_Flash( _isr_cookie ) \
  arm_interrupt_flash( _isr_cookie )

void _CPU_ISR_Set_level( uint32_t level );

uint32_t _CPU_ISR_Get_level( void );

void _CPU_Context_Initialize(
  Context_Control *the_context,
  void *stack_area_begin,
  size_t stack_area_size,
  uint32_t new_level,
  void (*entry_point)( void ),
  bool is_fp
);

#define _CPU_Context_Get_SP( _context ) \
  (_context)->register_sp

#define _CPU_Context_Restart_self( _the_context ) \
   _CPU_Context_restore( (_the_context) );

#define _CPU_Context_Fp_start( _base, _offset ) \
   ( (void *) _Addresses_Add_offset( (_base), (_offset) ) )

#define _CPU_Context_Initialize_fp( _destination ) \
  do { \
    *(*(_destination)) = _CPU_Null_fp_context; \
  } while (0)

#define _CPU_Fatal_halt( _err )             \
   do {                                     \
     uint32_t _level;                       \
     uint32_t _error = _err;                \
     _CPU_ISR_Disable( _level );            \
     __asm__ volatile ("mov r0, %0\n"           \
                   : "=r" (_error)          \
                   : "0" (_error)           \
                   : "r0" );                \
     while (1);                             \
   } while (0);

void _CPU_Initialize( void );

void _CPU_ISR_install_vector(
  uint32_t vector,
  proc_ptr new_handler,
  proc_ptr *old_handler
);

void _CPU_Context_switch( Context_Control *run, Context_Control *heir );

void _CPU_Context_restore( Context_Control *new_context )
  RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

#if defined(ARM_MULTILIB_ARCH_V7M)
  void _ARMV7M_Start_multitasking( Context_Control *bsp, Context_Control *heir );
  void _ARMV7M_Stop_multitasking( Context_Control *bsp )
    RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;
  #define _CPU_Start_multitasking _ARMV7M_Start_multitasking
  #define _CPU_Stop_multitasking _ARMV7M_Stop_multitasking
#endif

void _CPU_Context_save_fp( Context_Control_fp **fp_context_ptr );

void _CPU_Context_restore_fp( Context_Control_fp **fp_context_ptr );

static inline uint32_t CPU_swap_u32( uint32_t value )
{
#if defined(__thumb2__)
  __asm__ volatile (
    "rev %0, %0"
    : "=r" (value)
    : "0" (value)
  );
  return value;
#elif defined(__thumb__)
  uint32_t byte1, byte2, byte3, byte4, swapped;

  byte4 = (value >> 24) & 0xff;
  byte3 = (value >> 16) & 0xff;
  byte2 = (value >> 8)  & 0xff;
  byte1 =  value & 0xff;

  swapped = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
  return swapped;
#else
  uint32_t tmp = value; /* make compiler warnings go away */
  __asm__ volatile ("EOR %1, %0, %0, ROR #16\n"
                "BIC %1, %1, #0xff0000\n"
                "MOV %0, %0, ROR #8\n"
                "EOR %0, %0, %1, LSR #8\n"
                : "=r" (value), "=r" (tmp)
                : "0" (value), "1" (tmp));
  return value;
#endif
}

static inline uint16_t CPU_swap_u16( uint16_t value )
{
#if defined(__thumb2__)
  __asm__ volatile (
    "rev16 %0, %0"
    : "=r" (value)
    : "0" (value)
  );
  return value;
#else
  return (uint16_t) (((value & 0xffU) << 8) | ((value >> 8) & 0xffU));
#endif
}

/** @} */

#if defined(ARM_MULTILIB_ARCH_V4)

/**
 * @addtogroup ScoreCPUARM
 *
 * @{
 */

typedef struct {
  uint32_t r0;
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
  uint32_t sp;
  uint32_t lr;
  uint32_t pc;
  uint32_t cpsr;
} arm_cpu_context;

typedef void arm_exc_abort_handler( arm_cpu_context *context );

typedef enum {
  ARM_EXCEPTION_RESET = 0,
  ARM_EXCEPTION_UNDEF = 1,
  ARM_EXCEPTION_SWI = 2,
  ARM_EXCEPTION_PREF_ABORT = 3,
  ARM_EXCEPTION_DATA_ABORT = 4,
  ARM_EXCEPTION_RESERVED = 5,
  ARM_EXCEPTION_IRQ = 6,
  ARM_EXCEPTION_FIQ = 7,
  MAX_EXCEPTIONS = 8
} Arm_symbolic_exception_name;

static inline uint32_t arm_status_irq_enable( void )
{
  uint32_t arm_switch_reg;
  uint32_t psr;

  RTEMS_COMPILER_MEMORY_BARRIER();

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "mrs %[psr], cpsr\n"
    "bic %[arm_switch_reg], %[psr], #0x80\n"
    "msr cpsr, %[arm_switch_reg]\n"
    ARM_SWITCH_BACK
    : [arm_switch_reg] "=&r" (arm_switch_reg), [psr] "=&r" (psr)
  );

  return psr;
}

static inline void arm_status_restore( uint32_t psr )
{
  ARM_SWITCH_REGISTERS;

  __asm__ volatile (
    ARM_SWITCH_TO_ARM
    "msr cpsr, %[psr]\n"
    ARM_SWITCH_BACK
    : ARM_SWITCH_OUTPUT
    : [psr] "r" (psr)
  );

  RTEMS_COMPILER_MEMORY_BARRIER();
}

void arm_exc_data_abort_set_handler( arm_exc_abort_handler handler );

void arm_exc_data_abort( void );

void arm_exc_prefetch_abort_set_handler( arm_exc_abort_handler handler );

void arm_exc_prefetch_abort( void );

void bsp_interrupt_dispatch( void );

void arm_exc_interrupt( void );

void arm_exc_undefined( void );

/** @} */

/* XXX This is out of date */
typedef struct {
  uint32_t register_r0;
  uint32_t register_r1;
  uint32_t register_r2;
  uint32_t register_r3;
  uint32_t register_ip;
  uint32_t register_lr;
} CPU_Exception_frame;

typedef CPU_Exception_frame CPU_Interrupt_frame;

#else /* !defined(ARM_MULTILIB_ARCH_V4) */

typedef void CPU_Interrupt_frame;

#endif /* !defined(ARM_MULTILIB_ARCH_V4) */

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif /* _RTEMS_SCORE_CPU_H */
