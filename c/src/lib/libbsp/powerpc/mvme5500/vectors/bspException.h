#ifndef BSP_EXCEPTION_HANDLER_H
#define BSP_EXCEPTION_HANDLER_H

/* A slightly improved exception 'default' exception handler for RTEMS / SVGM */

/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 5/2002,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * This software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */

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
