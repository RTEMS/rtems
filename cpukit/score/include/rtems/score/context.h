/**
 *  @file  rtems/score/context.h
 *
 *  This include file contains all information about each thread's context.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CONTEXT_H
#define _RTEMS_SCORE_CONTEXT_H

/**
 *  @defgroup ScoreContext Context Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality which abstracts thread context
 *  management in a portable manner.
 *
 *  The context switch needed variable is contained in the per cpu
 *  data structure.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/cpu.h>

/**
 *  @brief Size of Floating Point Context Area
 *
 *  This constant defines the number of bytes required
 *  to store a full floating point context.
 */
#define CONTEXT_FP_SIZE CPU_CONTEXT_FP_SIZE

/**
 *  @brief Initialize Context Area
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
 */
#define _Context_Initialize(_the_context, _stack, _size, _isr, _entry, _is_fp) \
   _CPU_Context_Initialize( _the_context, _stack, _size, _isr, _entry, _is_fp )

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
 *  @brief Perform Context Switch
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
 *  @brief Restart Currently Executing Thread
 *
 *  This routine restarts the calling thread by restoring its initial
 *  stack pointer and returning to the thread's entry point.
 *
 *  @param[in] _the_context is the context of the thread to restart
 */
#define _Context_Restart_self( _the_context ) \
   _CPU_Context_Restart_self( _the_context )

#if defined(RTEMS_SMP)
/*
 *  @brief Switch to First Task on Secondary Core
 *
 *  This routine is only used to switch to the first task on a
 *  secondary core in an SMP configuration.  Since the switch
 *  to the first task is done from an interrupt handler, this
 *  may be different from simply restarting the currently running
 *  task.
 *
 *  @param[in] _the_context is the context of the first thread to
 *             run on this core
 */
#define _Context_Switch_to_first_task_smp( _the_context ) \
   _CPU_Context_switch_to_first_task_smp( _the_context )
#endif

/**
 *  @brief Return Starting Address of Floating Point Context
 *
 *  This function returns the starting address of the floating
 *  point context save area.  It is assumed that the are reserved
 *  for the floating point save area is large enough.
 *
 *  @param[in] _base is lowest physical address of the floating point
 *         context save area.
 *  @param[in] _offset is the offset into the floating point area
 *
 *  @return the initial FP context pointer
 */
#define _Context_Fp_start( _base, _offset ) \
   _CPU_Context_Fp_start( (_base), (_offset) )

/**
 *  @brief Initialize Floating Point Context Area
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
 *  @brief Restore Floating Point Context Area
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
 *  @brief Save Floating Point Context Area
 *
 *  This routine saves the current floating point context
 *  in the @a _fp area.
 *
 *  @param[in] _fp points to the floating point context area to restore.
 */
#define _Context_Save_fp( _fp ) \
   _CPU_Context_save_fp( _fp )

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
