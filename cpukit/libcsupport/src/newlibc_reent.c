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

#include <sys/reent.h>
#include <stdlib.h>
#include <stdio.h>

#include <rtems/libcsupport.h>

int _fwalk(struct _reent *ptr, int (*function) (FILE *) );

bool newlib_create_hook(
  rtems_tcb *current_task __attribute__((unused)),
  rtems_tcb *creating_task
)
{
  struct _reent *ptr;
  bool ok;

  if (_Thread_libc_reent == 0)
  {
    _REENT = _GLOBAL_REENT;

    _Thread_Set_libc_reent (&_REENT);
  }

  /* It is OK to allocate from the workspace because these
   * hooks run with thread dispatching disabled.
   */
  ptr = (struct _reent *) _Workspace_Allocate(sizeof(*ptr));
  creating_task->libc_reent = ptr;
  ok = ptr != NULL;

  if (ok) {
    _REENT_INIT_PTR((ptr)); /* GCC extension: structure constants */
  }

  return ok;
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

  ptr = deleted_task->libc_reent;
  deleted_task->libc_reent = NULL;

  /*
   *  Just in case there are some buffers lying around.
   */
  _fwalk(ptr, newlib_free_buffers);

  _Workspace_Free(ptr);
}

#endif
