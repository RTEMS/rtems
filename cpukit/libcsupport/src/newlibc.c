/*
 *  Implementation of hooks for the CYGNUS newlib libc
 *  These hooks set things up so that:
 *       + '_REENT' is switched at task switch time.
 *
 *  COPYRIGHT (c) 1994 by Division Incorporated
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 *
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#include <rtems.h>

#if defined(RTEMS_NEWLIB)
#include <rtems/libcsupport.h>
#include <stdlib.h>             /* for free() */
#include <string.h>             /* for memset() */

#include <sys/reent.h>          /* for extern of _REENT (aka _impure_ptr) */
#include <errno.h>

/*
 *  NOTE:
 *        There is some problem with doing this on the hpux version
 *        of the UNIX simulator (symptom is printf core dumps), so
 *        we just don't for now.
 *        Not sure if this is a problem with hpux, newlib, or something else.
 */
 
#if defined(RTEMS_UNIX) && !defined(hpux)
#define NEED_SETVBUF
#endif
 
#ifdef NEED_SETVBUF
#include <stdio.h>
#endif

int              libc_reentrant;        /* do we think we are reentrant? */
struct _reent    libc_global_reent;

/*
 * CYGNUS newlib routine that does atexit() processing and flushes
 *      stdio streams
 *      undocumented
 */

extern void _wrapup_reent(struct _reent *);
extern void _reclaim_reent(struct _reent *);

#include <stdio.h>

void libc_wrapup(void)
{
  /*
   *  In case RTEMS is already down, don't do this.  It could be 
   *  dangerous.
   */

  if (!_System_state_Is_up(_System_state_Get()))
     return;

  /*
   *  This was already done if the user called exit() directly .
  _wrapup_reent(0);
   */

  if (_REENT != &libc_global_reent) {
      _wrapup_reent(&libc_global_reent);
#if 0
      /*  Don't reclaim this one, just in case we do printfs
       *  on the way out to ROM.
       */
      _reclaim_reent(&libc_global_reent);
#endif
      _REENT = &libc_global_reent;
  }
  
  /*
   * Try to drain output buffers.
   *
   * Should this be changed to do *all* file streams?
   *    _fwalk (_REENT, fclose);
   */

  fclose (stdin);
  fclose (stdout);
  fclose (stderr);
}


rtems_boolean libc_create_hook(
  rtems_tcb *current_task,
  rtems_tcb *creating_task
)
{
  creating_task->libc_reent = NULL;
  return TRUE;
}

/*
 * Called for all user TASKS (system tasks are MPCI Receive Server and IDLE)
 */

rtems_extension libc_start_hook(
  rtems_tcb *current_task,
  rtems_tcb *starting_task
)
{
  struct _reent *ptr;

  /*  NOTE: The RTEMS malloc is reentrant without a reent ptr since
   *        it is based on the Classic API Region Manager.
   */

  ptr = (struct _reent *) calloc(1, sizeof(struct _reent));

  if (!ptr)
     rtems_fatal_error_occurred(RTEMS_NO_MEMORY);
 
#ifdef __GNUC__
  /* GCC extension: structure constants */
  *ptr = (struct _reent) _REENT_INIT((*ptr));
#else
  /* 
   *  WARNING: THIS IS VERY DEPENDENT ON NEWLIB!!! 
   *           Last visual check was against newlib 1.8.2 but last known
   *           use was against 1.7.0.  This is basically an exansion of
   *           REENT_INIT() in <sys/reent.h>.
   *  NOTE:    calloc() takes care of zeroing fields.
   */
  ptr->_stdin = &ptr->__sf[0];
  ptr->_stdout = &ptr->__sf[1];
  ptr->_stderr = &ptr->__sf[2];
  ptr->_current_locale = "C";
  ptr->_new._reent._rand_next = 1;
#endif

  starting_task->libc_reent = ptr;
}

/*
 * Called for all user TASKS (system tasks are MPCI Receive Server and IDLE)
 */
 
#ifdef NEED_SETVBUF
rtems_extension libc_begin_hook(rtems_tcb *current_task)
{
  setvbuf( stdout, NULL, _IOLBF, BUFSIZ );
}
#endif

/*
 *  Function:   libc_delete_hook
 *  Created:    94/12/10
 *
 *  Description:
 *      Called when a task is deleted.
 *      Must restore the new lib reentrancy state for the new current
 *      task.
 *
 *  Parameters:
 *
 *
 *  Returns:
 *
 *
 *  Side Effects:
 *
 *  Notes:
 *
 *
 *  Deficiencies/ToDo:
 *
 *
 */

rtems_extension libc_delete_hook(
  rtems_tcb *current_task,
  rtems_tcb *deleted_task
)
{
  struct _reent *ptr;

  /*
   * The reentrancy structure was allocated by newlib using malloc()
   */

  if (current_task == deleted_task) {
    ptr = _REENT;
  } else {
    ptr = (struct _reent *) deleted_task->libc_reent;
  }

  /* if (ptr) */
  if (ptr && ptr != &libc_global_reent) {
    _wrapup_reent(ptr);
    _reclaim_reent(ptr);
    free(ptr);
  }

  deleted_task->libc_reent = NULL;

  /*
   * Require the switch back to another task to install its own
   */

  if ( current_task == deleted_task ) {
    _REENT = 0;
  }
}

/*
 *  Function:   libc_init
 *  Created:    94/12/10
 *
 *  Description:
 *      Init libc for CYGNUS newlib
 *      Set up _REENT to use our global libc_global_reent.
 *      (newlib provides a global of its own, but we prefer our
 *      own name for it)
 *
 *      If reentrancy is desired (which it should be), then
 *      we install the task extension hooks to maintain the
 *      newlib reentrancy global variable _REENT on task
 *      create, delete, switch, exit, etc.
 *
 *  Parameters:
 *      reentrant               non-zero if reentrant library desired.
 *
 *  Returns:
 *
 *  Side Effects:
 *      installs libc extensions if reentrant.
 *
 *  Notes:
 *
 *
 *  Deficiencies/ToDo:
 *
 */

void
libc_init(int reentrant)
{
  rtems_extensions_table  libc_extension;
  rtems_status_code       rc;
  rtems_id                extension_id;

  libc_global_reent = (struct _reent) _REENT_INIT((libc_global_reent));
  _REENT = &libc_global_reent;

  if (reentrant) {
    memset(&libc_extension, 0, sizeof(libc_extension));

    libc_extension.thread_create  = libc_create_hook;
    libc_extension.thread_start   = libc_start_hook;
#ifdef NEED_SETVBUF
    libc_extension.thread_begin   = libc_begin_hook;
#endif
    libc_extension.thread_delete  = libc_delete_hook;

    _Thread_Set_libc_reent ((void**) &_REENT);

    rc = rtems_extension_create(rtems_build_name('L', 'I', 'B', 'C'),
                          &libc_extension, &extension_id);
    if (rc != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred( rc );

    libc_reentrant = reentrant;
  }
}

/*
 *  Function:   _exit
 *  Created:    94/12/10
 *
 *  Description:
 *      Called from exit() after it does atexit() processing and stdio fflush's
 *
 *      called from bottom of exit() to really delete the task.
 *      If we are using reentrant libc, then let the delete extension
 *      do all the work, otherwise if a shutdown is in progress,
 *      then just do it.
 *
 *  Parameters:
 *      exit status
 *
 *  Returns:
 *      does not return
 *
 *  Side Effects:
 *
 *  Notes:
 *
 *
 *  Deficiencies/ToDo:
 *
 *
 */

#include <unistd.h>

#if !defined(RTEMS_UNIX)
void _exit(int status)
{
  /*
   *  We need to do the exit processing on the global reentrancy structure.
   *  This has already been done on the per task reentrancy structure
   *  associated with this task.
   */

  libc_wrapup();
  rtems_shutdown_executive(status);
  for (;;) ; /* to avoid warnings */
}

#else

void exit(int status)
{
  libc_wrapup();
  rtems_shutdown_executive(status);
  for (;;) ; /* to avoid warnings */
}
#endif

#endif
