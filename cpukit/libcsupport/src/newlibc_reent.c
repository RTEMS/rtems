/**
 *  @file
 *
 *  @brief Newlib Support
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1994 by Division Incorporated
 *
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

  /* It is OK to allocate from the workspace because these
   * hooks run with thread dispatching disabled.
   */
  ptr = (struct _reent *) _Workspace_Allocate(sizeof(struct _reent));

  if (ptr) {
    _REENT_INIT_PTR((ptr)); /* GCC extension: structure constants */
    creating_task->libc_reent = ptr;
    return TRUE;
  }

  return FALSE;
}

static int newlib_free_buffers(
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

    _Workspace_Free(ptr);
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
