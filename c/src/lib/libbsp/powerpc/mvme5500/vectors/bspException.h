#ifndef BSP_EXCEPTION_HANDLER_H
#define BSP_EXCEPTION_HANDLER_H
/* $Id$ */

/* A slightly improved exception 'default' exception handler for RTEMS / SVGM */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002/5 */

#include <bsp/vectors.h>

/* Two types of exception intercepting / catching is supported:
 *
 *  - lowlevel handling (runs at IRQ level, before restoring any
 *    task context).
 *  - highlevel handling.
 *
 *  A lowlevel user hook is invoked twice, before and after processing
 *  (printing) the exception.
 *  If the user hook returns a nonzero value, normal processing
 *  is skipped (including the second call to the hook)
 *
 *  If the hook returns nonzero to the second call, no default
 *  'panic' occurs.
 *
 *  Default 'panic':
 *   - if a task context is available:
 *     - if a highlevel handler is installed, pass control
 *       to the highlevel handler when returning from the
 *       exception (the highlevel handler should probably
 *       do a longjmp()). Otherwise:
 * 	   - try to suspend interrupted task.
 *   - hang if no task context is available.
 *
 */

typedef struct BSP_ExceptionExtensionRec_ *BSP_ExceptionExtension;

typedef int (*BSP_ExceptionHookProc)(BSP_Exception_frame *frame, BSP_ExceptionExtension ext, int after);

typedef struct BSP_ExceptionExtensionRec_ {
	BSP_ExceptionHookProc	lowlevelHook;
	int						quiet;			/* silence the exception handler */
	void					(*highlevelHook)(BSP_ExceptionExtension);
	/* user fields may be added after this */
} BSP_ExceptionExtensionRec;

#define SRR1_TEA_EXC	(1<<(31-13))
#define SRR1_MCP_EXC	(1<<(31-12))

void
BSP_exceptionHandler(BSP_Exception_frame* excPtr);

/* install an exception handler to the current task context */
BSP_ExceptionExtension
BSP_exceptionHandlerInstall(BSP_ExceptionExtension e);

#endif
