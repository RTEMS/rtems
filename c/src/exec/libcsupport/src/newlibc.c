
/*
 *  COPYRIGHT (c) 1994 by Division Incorporated
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of Division Incorporated not be
 *      used in advertising or publicity pertaining to distribution
 *      of the software without specific, written prior permission.
 *      Division Incorporated makes no representations about the
 *      suitability of this software for any purpose.
 *
 *  Description:
 *      Implementation of hooks for the CYGNUS newlib libc
 *      These hooks set things up so that:
 *              '_REENT' is switched at task switch time.
 *
 *
 *  TODO:
 *
 *  NOTE:
 *
 *  $Id$
 *
 */

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#include <rtems.h>

#if defined(RTEMS_NEWLIB)
#include <libcsupport.h>
#include <stdlib.h>             /* for free() */
#include <string.h>             /* for memset() */

#include <sys/reent.h>          /* for extern of _REENT (aka _impure_ptr) */
#include <errno.h>

/*
 *  NOTE: When using RTEMS fake stat, fstat, and isatty, all output
 *        is line buffered so this setvbuf is not necessary.  This
 *        setvbuf insures that we can redirect the output of a test
 *        on the UNIX simulator and it is in the same order as for a
 *        real target.
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

#include "internal.h"

#define LIBC_NOTEPAD RTEMS_NOTEPAD_LAST


int              libc_reentrant;        /* do we think we are reentrant? */
struct _reent    libc_global_reent = _REENT_INIT(libc_global_reent);

/*
 * CYGNUS newlib routine that does atexit() processing and flushes
 *      stdio streams
 *      undocumented
 */

extern void _wrapup_reent(struct _reent *);
extern void _reclaim_reent(struct _reent *);

void
libc_wrapup(void)
{
    _wrapup_reent(0);
    if (_REENT != &libc_global_reent)
    {
        _wrapup_reent(&libc_global_reent);
#if 0
        /* don't reclaim this one, just in case we do printfs */
        /* on our way out to ROM */
        _reclaim_reent(&libc_global_reent);
#endif
        _REENT = &libc_global_reent;
    }
}


rtems_boolean
libc_create_hook(rtems_tcb *current_task,
                 rtems_tcb *creating_task)
{
    MY_task_set_note(creating_task, LIBC_NOTEPAD, 0);
    return TRUE;
}

/*
 * Called for all user TASKS (system tasks are MPCI Receive Server and IDLE)
 */

rtems_extension
libc_start_hook(rtems_tcb *current_task,
                rtems_tcb *starting_task)
{
    struct _reent *ptr;

    /* NOTE: our malloc is reentrant without a reent ptr since
     *  it is based on region manager
     */

    ptr = (struct _reent *) calloc(1, sizeof(struct _reent));

    if (!ptr)
       rtems_fatal_error_occurred(RTEMS_NO_MEMORY);
 
#ifdef __GNUC__
    /* GCC extension: structure constants */
    *ptr = (struct _reent) _REENT_INIT((*ptr));
#else
    /* 
     *  Warning: THIS IS VERY DEPENDENT ON NEWLIB!!! WRITTEN FOR 1.7.0
     */
    ptr->_errno=0;
    ptr->_stdin=&ptr->__sf[0];
    ptr->_stdout=&ptr->__sf[1];
    ptr->_stderr=&ptr->__sf[2];
    ptr->_scanpoint=0;
    ptr->_asctime[0]=0;
    ptr->_next=1;
    ptr->__sdidinit=0;
#endif

    MY_task_set_note(starting_task, LIBC_NOTEPAD, (rtems_unsigned32) ptr);
}

/*
 * Called for all user TASKS (system tasks are MPCI Receive Server and IDLE)
 *
 *  NOTE: When using RTEMS fake stat, fstat, and isatty, all output 
 *        is line buffered so this setvbuf is not necessary.  This
 *        setvbuf insures that we can redirect the output of a test
 *        on the UNIX simulator and it is in the same order as for a 
 *        real target.
 */
 
#ifdef NEED_SETVBUF
rtems_extension
libc_begin_hook(rtems_tcb *current_task)
{
  setvbuf( stdout, NULL, _IOLBF, BUFSIZ );
}
#endif

rtems_extension
libc_switch_hook(rtems_tcb *current_task,
                 rtems_tcb *heir_task)
{
    rtems_unsigned32 impure_value;

    /* XXX We can't use rtems_task_set_note() here since SYSI task has a
     * tid of 0, which is treated specially (optimized, actually)
     * by rtems_task_set_note
     *
     * NOTE:  The above comment is no longer true and we need to use
     *        the extension data areas added about the same time.
     */

    /*
     *  Don't touch the outgoing task if it has been deleted.
     */

    if ( !_States_Is_transient( current_task->current_state ) ) {
      impure_value = (rtems_unsigned32) _REENT;
      MY_task_set_note(current_task, LIBC_NOTEPAD, impure_value);
    }

    _REENT = (struct _reent *) MY_task_get_note(heir_task, LIBC_NOTEPAD);

}

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
rtems_extension
libc_delete_hook(rtems_tcb *current_task,
                 rtems_tcb *deleted_task)
{
    struct _reent *ptr;

    /*
     * The reentrancy structure was allocated by newlib using malloc()
     */

    if (current_task == deleted_task)
    {
      ptr = _REENT;
    }
    else
    {
      ptr = (struct _reent *) MY_task_get_note(deleted_task, LIBC_NOTEPAD);
    }

    /* if (ptr) */
    if (ptr && ptr != &libc_global_reent)
    {
      _wrapup_reent(ptr);
      _reclaim_reent(ptr);
      free(ptr);
    }

    MY_task_set_note(deleted_task, LIBC_NOTEPAD, 0);

    /*
     * Require the switch back to another task to install its own
     */

    if (current_task == deleted_task)
    {
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
    rtems_id                extension_id;
    rtems_status_code       rc;

    _REENT = &libc_global_reent;

    if (reentrant)
    {
        memset(&libc_extension, 0, sizeof(libc_extension));

        libc_extension.thread_create  = libc_create_hook;
        libc_extension.thread_start   = libc_start_hook;
#ifdef NEED_SETVBUF
        libc_extension.thread_begin   = libc_begin_hook;
#endif
        libc_extension.thread_switch  = libc_switch_hook;
        libc_extension.thread_delete  = libc_delete_hook;

        rc = rtems_extension_create(rtems_build_name('L', 'I', 'B', 'C'),
                              &libc_extension, &extension_id);
        if (rc != RTEMS_SUCCESSFUL)
            rtems_fatal_error_occurred(rc);

        libc_reentrant = reentrant;
    }
}

#if 0
/*
 *  Routines required by the gnat runtime.
 */

int get_errno()
{
  return errno;
}
#endif

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

#include <stdio.h>

/* #if !defined(RTEMS_UNIX) && !defined(__GO32__) && !defined(_AM29K) */
#if !defined(RTEMS_UNIX) && !defined(_AM29K)
void _exit(int status)
{
    rtems_shutdown_executive(status);
}

#else

void exit(int status)
{
    libc_wrapup();
    rtems_shutdown_executive(status);
}
#endif


/*
 *  These are directly supported (and completely correct) in the posix api.
 */

#if !defined(RTEMS_POSIX_API)
pid_t __getpid(void)
{
  return 0;
}
#endif

/* #if !defined(RTEMS_POSIX_API) || defined(__GO32__) */
#if !defined(RTEMS_POSIX_API)
pid_t getpid(void)
{
  return __getpid();
}
#endif

/* #if !defined(RTEMS_POSIX_API) || defined(__GO32__) */
#if !defined(RTEMS_POSIX_API)
int kill( pid_t pid, int sig )
{
  return 0;
}
#endif

int __kill( pid_t pid, int sig )
{
  return 0;
}

#if !defined(RTEMS_POSIX_API)
unsigned int sleep(
  unsigned int seconds
)
{
  rtems_status_code status;
  rtems_interval    ticks_per_second;
  rtems_interval    ticks;
 
  status = rtems_clock_get(
    RTEMS_CLOCK_GET_TICKS_PER_SECOND,
    &ticks_per_second
  );
 
  ticks = seconds * ticks_per_second;
 
  status = rtems_task_wake_after( ticks );
 
  /*
   *  Returns the "unslept" amount of time.  In RTEMS signals are not
   *  interruptable, so tasks really sleep all of the requested time.
   */
 
  return 0;
}
#endif


#endif
