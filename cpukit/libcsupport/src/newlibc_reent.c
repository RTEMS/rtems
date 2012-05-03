/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#include <rtems.h>

#if defined(RTEMS_NEWLIB)
#include <rtems/libcsupport.h>

/* Since we compile with strict ANSI we need to undef it to get
 * prototypes for extensions
 */
#undef __STRICT_ANSI__

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

#include <stdio.h>

int _fwalk(struct _reent *ptr, int (*function) (FILE *) );

extern struct _reent * const _global_impure_ptr __ATTRIBUTE_IMPURE_PTR__;
/*
 * reent struct allocation moved here from libc_start_hook() to avoid
 * mutual exclusion problems when memory is allocated from the start hook.
 *
 * Memory is also now allocated from the workspace rather than the heap.
 *  -- ptorre 9/30/03
 */
bool newlib_create_hook(
  rtems_tcb *current_task __attribute__((unused)),
  rtems_tcb *creating_task
)
{
  struct _reent *ptr;

  if (_Thread_libc_reent == 0)
  {
    _REENT = _global_impure_ptr;

    _Thread_Set_libc_reent (&_REENT);
  }

  /*  NOTE: The RTEMS malloc is reentrant without a reent ptr since
   *        it is based on the Classic API Region Manager.
   */

  #define REENT_MALLOCED 0
  #if REENT_MALLOCED
    ptr = (struct _reent *) calloc(1, sizeof(struct _reent));
  #else
    /* It is OK to allocate from the workspace because these
     * hooks run with thread dispatching disabled.
     */
    ptr = (struct _reent *) _Workspace_Allocate(sizeof(struct _reent));
  #endif

  if (ptr) {
    _REENT_INIT_PTR((ptr)); /* GCC extension: structure constants */
    creating_task->libc_reent = ptr;
    return TRUE;
  }

  return FALSE;
}

/*
 * Called for all user TASKS (system tasks are MPCI Receive Server and IDLE)
 */

#ifdef NEED_SETVBUF
void newlib_begin_hook(rtems_tcb *current_task)
{
  setvbuf( stdout, NULL, _IOLBF, BUFSIZ );
}
#endif

/*
 *  Called when a task is deleted.
 *  Must restore the new lib reentrancy state for the new current
 *  task.
 *
 */

int newlib_free_buffers(
  FILE *fp
)
{
  switch ( fileno(fp) ) {
    case 0:
    case 1:
    case 2:
      if (fp->_flags & __SMBF) {
        free( fp->_bf._base );
        fp->_flags &= ~__SMBF;
        fp->_bf._base = fp->_p = (unsigned char *) NULL;
      }
      break;
    default:
     fclose(fp);
  }
  return 0;
}

void newlib_delete_hook(
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
    ptr = deleted_task->libc_reent;
  }

  if (ptr && ptr != _global_impure_ptr) {
/*
    _wrapup_reent(ptr);
    _reclaim_reent(ptr);
*/
    /*
     *  Just in case there are some buffers lying around.
     */
    _fwalk(ptr, newlib_free_buffers);
#if REENT_MALLOCED
    free(ptr);
#else
    _Workspace_Free(ptr);
#endif
  }

  deleted_task->libc_reent = NULL;

  /*
   * Require the switch back to another task to install its own
   */

  if ( current_task == deleted_task ) {
    _REENT = 0;
  }
}

#endif
