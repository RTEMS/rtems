/** 
 *  @file  rtems/score/context.h
 *
 *  This include file contains all information about each thread's context.
 */

/*
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_CONTEXT_H
#define _RTEMS_SCORE_CONTEXT_H

/**
 *  @defgroup ScoreContext Context Handler
 *
 *  This group contains functionality which abstracts thread context
 *  management in a portable manner.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/cpu.h>

/**
 *  @ingroup ScoreContext
 *  This constant defines the number of bytes required
 *  to store a full floating point context.
 */
#define CONTEXT_FP_SIZE CPU_CONTEXT_FP_SIZE

/**
 *  @ingroup ScoreContext
 *  This variable is set to TRUE when a reschedule operation
 *  has determined that the processor should be taken away from the
 *  currently executing thread and given to the heir thread.
 */

SCORE_EXTERN volatile boolean _Context_Switch_necessary;

/**
 *  @ingroup ScoreContext
 *  This routine initializes @a _the_context such that the stack
 *  pointer, interrupt level, and entry point are correct for the
 *  thread's initial state.
 *
 *  @param _the_context (in) will be initialized
 *  @param _stack (in) is the lowest physical address of the thread's 
 *         context
 *  @param _size (in) is the size in octets of the thread's context
 *  @param _isr (in) is the ISR enable level for this thread
 *  @param _entry (in) is this thread's entry point
 *  @param _is_fp (in) is set to TRUE if this thread has floating point
 *         enabled
 */
#define \
   _Context_Initialize( _the_context, _stack, _size, _isr, _entry, _is_fp ) \
   _CPU_Context_Initialize( _the_context, _stack, _size, _isr, _entry, _is_fp )

/**
 *  @ingroup ScoreContext
 *  This routine saves the current context into the @a _executing
 *  context record and restores the context specified by @a _heir.
 *
 *  @param _executing (in) is the currently executing thread's context
 *  @param _heir (in) is the context of the thread to be switched to
 */
#define _Context_Switch( _executing, _heir ) \
   _CPU_Context_switch( _executing, _heir )

/**
 *  @ingroup ScoreContext
 *  This routine restarts the calling thread by restoring its initial
 *  stack pointer and returning to the thread's entry point.
 *
 *  @param _the_context (in) is the context of the thread to restart
 */
#define _Context_Restart_self( _the_context ) \
   _CPU_Context_Restart_self( _the_context )

/**
 *  @ingroup ScoreContext
 *  This function returns the starting address of the floating
 *  point context save area.  It is assumed that the are reserved
 *  for the floating point save area is large enough.
 *
 *  @param _base (in) is lowest physical address of the floating point
 *         context save area.
 *  @param _offset (in) is XXX
 *
 *  @return the initial FP context pointer
 */
#define _Context_Fp_start( _base, _offset ) \
   _CPU_Context_Fp_start( (_base), (_offset) )

/**
 *  @ingroup ScoreContext
 *  This routine initializes the floating point context save
 *  area to contain an initial known state.
 *
 *  @param _fp_area (in) is the base address of the floating point
 *         context save area to initialize.
 */
#define _Context_Initialize_fp( _fp_area ) \
   _CPU_Context_Initialize_fp( _fp_area )

/**
 *  @ingroup ScoreContext
 *  This routine restores the floating point context contained
 *  in the @a _fp area.  It is assumed that the current
 *  floating point context has been saved by a previous invocation
 *  of @a _Context_Save_fp.
 *
 *  @param _fp (in) points to the floating point context area to restore.
 */
#define _Context_Restore_fp( _fp ) \
   _CPU_Context_restore_fp( _fp )

/**
 *  @ingroup ScoreContext
 *  This routine saves the current floating point context
 *  in the @a _fp area.
 *
 *  @param _fp (in) points to the floating point context area to restore.
 */
#define _Context_Save_fp( _fp ) \
   _CPU_Context_save_fp( _fp )

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
