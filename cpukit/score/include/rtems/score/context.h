/*  context.h
 *
 *  This include file contains all information about a context.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __RTEMS_CONTEXT_h
#define __RTEMS_CONTEXT_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/cpu.h>

/*
 *  The following constant defines the number of bytes required
 *  to store a full floating point context.
 */

#define CONTEXT_FP_SIZE CPU_CONTEXT_FP_SIZE

/*
 *  The following variable is set to TRUE when a reschedule operation
 *  has determined that the processor should be taken away from the
 *  currently executing thread and given to the heir thread.
 */

SCORE_EXTERN volatile boolean _Context_Switch_necessary;

/*
 *  _Context_Initialize
 *
 *  DESCRIPTION:
 *
 *  This routine initializes THE_CONTEXT such that the stack
 *  pointer, interrupt level, and entry point are correct for the
 *  thread's initial state.
 */

#define \
   _Context_Initialize( _the_context, _stack, _size, _isr, _entry, _is_fp ) \
   _CPU_Context_Initialize( _the_context, _stack, _size, _isr, _entry, _is_fp )

/*
 *  _Context_Switch
 *
 *  DESCRIPTION:
 *
 *  This routine saves the current context into the EXECUTING
 *  context record and restores the context specified by HEIR.
 */

#define _Context_Switch( _executing, _heir ) \
   _CPU_Context_switch( _executing, _heir )

/*
 *  _Context_Restart_self
 *
 *  DESCRIPTION:
 *
 *  This routine restarts the calling thread by restoring its initial
 *  stack pointer and returning to the thread's entry point.
 */

#define _Context_Restart_self( _the_context ) \
   _CPU_Context_Restart_self( _the_context )

/*
 *  _Context_Fp_start
 *
 *  DESCRIPTION:
 *
 *  This function returns the starting address of the floating
 *  point context save area.  It is assumed that the are reserved
 *  for the floating point save area is large enough.
 */

#define _Context_Fp_start( _base, _offset ) \
   _CPU_Context_Fp_start( (_base), (_offset) )

/*
 *  _Context_Initialize_fp
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the floating point context save
 *  area to contain an initial known state.
 */

#define _Context_Initialize_fp( _fp_area ) \
   _CPU_Context_Initialize_fp( _fp_area )

/*
 *  _Context_Restore_fp
 *
 *  DESCRIPTION:
 *
 *  This routine restores the floating point context contained
 *  in the FP_CONTEXT area.  It is assumed that the current
 *  floating point context has been saved by a previous invocation
 *  of SAVE_FP.
 */

#define _Context_Restore_fp( _fp ) \
   _CPU_Context_restore_fp( _fp )

/*
 *  _Context_Save_fp
 *
 *  DESCRIPTION:
 *
 *  This routine saves the current floating point context
 *  in the FP_CONTEXT area.
 */

#define _Context_Save_fp( _fp ) \
   _CPU_Context_save_fp( _fp )

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
