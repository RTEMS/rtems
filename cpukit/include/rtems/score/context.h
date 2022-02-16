/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreContext
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreContext.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_SCORE_CONTEXT_H
#define _RTEMS_SCORE_CONTEXT_H

/**
 * @defgroup RTEMSScoreContext Context Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the Context Handler implementation.
 *
 * This handler encapsulates functionality which abstracts thread context
 * management in a portable manner.
 *
 * The context switch needed variable is contained in the per cpu
 * data structure.
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/cpu.h>

/**
 *  @brief Size of floating point context area.
 *
 *  This constant defines the number of bytes required
 *  to store a full floating point context.
 */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
  #define CONTEXT_FP_SIZE \
    RTEMS_ALIGN_UP( CPU_CONTEXT_FP_SIZE, CPU_STACK_ALIGNMENT )
#else
  #define CONTEXT_FP_SIZE 0
#endif

/**
 *  @brief Initialize context area.
 *
 *  This routine initializes @a _the_context such that the stack
 *  pointer, interrupt level, and entry point are correct for the
 *  thread's initial state.
 *
 *  @param[in] _the_context will be initialized
 *  @param[in] _stack is the lowest physical address of the thread's
 *         context
 *  @param[in] _size is the size in octets of the thread's context
 *  @param[in] _isr is the ISR enable level for this thread
 *  @param[in] _entry is this thread's entry point
 *  @param[in] _is_fp is set to true if this thread has floating point
 *         enabled
 *  @param[in] _tls_area The thread-local storage (TLS) area begin.
 */
#define _Context_Initialize( _the_context, _stack, _size, _isr, _entry, \
  _is_fp, _tls_area ) \
    _CPU_Context_Initialize( _the_context, _stack, _size, _isr, _entry, \
      _is_fp, _tls_area )

/**
 *  This macro is invoked from _Thread_Handler to do whatever CPU
 *  specific magic is required that must be done in the context of
 *  the thread when it starts.
 *
 *  If the CPU architecture does not require any magic, then this
 *  macro is empty.
 */

#if defined(_CPU_Context_Initialization_at_thread_begin)
  #define _Context_Initialization_at_thread_begin() \
     _CPU_Context_Initialization_at_thread_begin()
#else
  #define _Context_Initialization_at_thread_begin()
#endif

/**
 *  @brief Perform context switch.
 *
 *  This routine saves the current context into the @a _executing
 *  context record and restores the context specified by @a _heir.
 *
 *  @param[in] _executing is the currently executing thread's context
 *  @param[in] _heir is the context of the thread to be switched to
 */
#define _Context_Switch( _executing, _heir ) \
   _CPU_Context_switch( _executing, _heir )

/**
 *  @brief Restart currently executing thread.
 *
 *  This routine restarts the calling thread by restoring its initial
 *  stack pointer and returning to the thread's entry point.
 *
 *  @param[in] _the_context is the context of the thread to restart
 */
#define _Context_Restart_self( _the_context ) \
   _CPU_Context_Restart_self( _the_context )

/**
 *  @brief Initialize floating point context area.
 *
 *  This routine initializes the floating point context save
 *  area to contain an initial known state.
 *
 *  @param[in] _fp_area is the base address of the floating point
 *         context save area to initialize.
 */
#define _Context_Initialize_fp( _fp_area ) \
   _CPU_Context_Initialize_fp( _fp_area )

/**
 *  @brief Restore floating point context area.
 *
 *  This routine restores the floating point context contained
 *  in the @a _fp area.  It is assumed that the current
 *  floating point context has been saved by a previous invocation
 *  of @a _Context_Save_fp.
 *
 *  @param[in] _fp points to the floating point context area to restore.
 */
#define _Context_Restore_fp( _fp ) \
   _CPU_Context_restore_fp( _fp )

/**
 *  @brief Save floating point context area.
 *
 *  This routine saves the current floating point context
 *  in the @a _fp area.
 *
 *  @param[in] _fp points to the floating point context area to restore.
 */
#define _Context_Save_fp( _fp ) \
   _CPU_Context_save_fp( _fp )

#if defined(_CPU_Context_Destroy)
  #define _Context_Destroy( _the_thread, _the_context ) \
    _CPU_Context_Destroy( _the_thread, _the_context )
#else
  #define _Context_Destroy( _the_thread, _the_context )
#endif

#ifdef __cplusplus
}
#endif

/** @} */

#endif
/* end of include file */
