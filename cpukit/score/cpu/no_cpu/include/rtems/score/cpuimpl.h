/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief CPU Port Implementation API
 */

/*
 * Copyright (C) 2013, 2016 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_SCORE_CPUIMPL_H
#define _RTEMS_SCORE_CPUIMPL_H

#include <rtems/score/cpu.h>

/**
 * @defgroup RTEMSScoreCPUExample Example
 *
 * @ingroup RTEMSScoreCPU
 *
 * @brief Example CPU Architecture Support.
 *
 * @{
 */

/**
 * @brief The size of the CPU specific per-CPU control.
 *
 * This define must be visible to assember files since it is used to derive
 * structure offsets.
 */
#define CPU_PER_CPU_CONTROL_SIZE 0

/**
 * @brief Defines the thread-local storage (TLS) variant.
 *
 * Use one of the following values:
 *
 * 10: The architecture uses Variant I and the TLS offsets emitted by the
 *     linker neglect the TCB (examples: nios2, m68k, microblaze, powerpc,
 *     riscv).  The thread pointer directly references the thread-local data
 *     area.
 *
 * 11: The architecture uses Variant I and the TLS offsets emitted by the
 *     linker take the TCB into account (examples: arm, aarch64).
 *     The thread pointer references the TCB.
 *
 * 20: The architecture uses Variant II (examples: i386, sparc).
 */
#define CPU_THREAD_LOCAL_STORAGE_VARIANT 10

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The CPU specific per-CPU control.
 *
 * The CPU port can place here all state information that must be available and
 * maintained for each processor in the system.  This structure must contain at
 * least one field for C/C++ compatibility.  In GNU C empty structures have a
 * size of zero.  In C++ structures have a non-zero size.  In case
 * CPU_PER_CPU_CONTROL_SIZE is defined to zero, then this structure is not
 * used.
 */
typedef struct {
  /* CPU specific per-CPU state */
} CPU_Per_CPU_control;

/**
 * @brief Special register pointing to the per-CPU control of the current
 * processor.
 *
 * This is optional.  Not every CPU port needs this.  It is only an optional
 * optimization variant.
 */
register struct Per_CPU_Control *_CPU_Per_CPU_current asm( "rX" );

/**
 * @brief Optional method to obtain the per-CPU control of the current processor.
 *
 * This is optional.  Not every CPU port needs this.  It is only an optional
 * optimization variant.  In case this macro is undefined, the default
 * implementation using the current processor index will be used.
 */
#define _CPU_Get_current_per_CPU_control() ( _CPU_Per_CPU_current )

/**
 * @brief Optional method to get the executing thread.
 *
 * This is optional.  Not every CPU port needs this.  It is only an optional
 * optimization variant.  In case this macro is undefined, the default
 * implementation uses the per-CPU information and the current processor index
 * to get the executing thread.
 */
#define _CPU_Get_thread_executing() ( _CPU_Per_CPU_current->executing )

/**
 * This routine copies _error into a known place -- typically a stack
 * location or a register, optionally disables interrupts, and
 * halts/stops the CPU.
 *
 * Port Specific Information:
 *
 * XXX document implementation including references if appropriate
 */
RTEMS_NO_RETURN void _CPU_Fatal_halt( uint32_t source, CPU_Uint32ptr error );

/* end of Fatal Error manager macros */

/**
 * @addtogroup RTEMSScoreCPUExampleContext
 *
 * @brief Clobbers all volatile registers with values derived from the pattern
 * parameter.
 *
 * This function is used only in test sptests/spcontext01.
 *
 * @param[in] pattern Pattern used to generate distinct register values.
 *
 * @see _CPU_Context_validate().
 */
void _CPU_Context_volatile_clobber( uintptr_t pattern );

/**
 * @addtogroup RTEMSScoreCPUExampleContext
 *
 * @brief Initializes and validates the CPU context with values derived from
 * the pattern parameter.
 *
 * This function will not return if the CPU context remains consistent.  In
 * case this function returns the CPU port is broken.
 *
 * This function is used only in test sptests/spcontext01.
 *
 * @param[in] pattern Pattern used to generate distinct register values.
 *
 * @see _CPU_Context_volatile_clobber().
 */
void _CPU_Context_validate( uintptr_t pattern );

/**
 * @brief Emits an illegal instruction.
 *
 * This function is used only in test sptests/spfatal26.
 */
static inline void _CPU_Instruction_illegal( void )
{
  __asm__ volatile ( ".word 0" );
}

/**
 * @brief Emits a no operation instruction (nop).
 *
 * This function is used only in test sptests/spcache01.
 */
static inline void _CPU_Instruction_no_operation( void )
{
  __asm__ volatile ( "nop" );
}

/**
 * @brief Uses the thread-local storage area of the context.
 *
 * Some architectures may use dedicated registers to reference the thread-local
 * storage area of the associated thread.  This function should set these
 * registers to the values defined by the specified processor context.
 *
 * @param context is the processor context defining the thread-local storage
 *   area to use.
 */
static inline void _CPU_Use_thread_local_storage(
  const Context_Control *context
)
{
  (void) context;
}

/**
 * @brief Gets the thread pointer of the context.
 *
 * The thread pointer is used to get the address of thread-local storage
 * objects associated with a thread.
 *
 * @param context is the processor context containing the thread pointer.
 */
static inline void *_CPU_Get_TLS_thread_pointer(
  const Context_Control *context
)
{
  (void) context;
  return NULL;
}

#ifdef __cplusplus
}
#endif

#endif /* ASM */

/** @} */

#endif /* _RTEMS_SCORE_CPUIMPL_H */
