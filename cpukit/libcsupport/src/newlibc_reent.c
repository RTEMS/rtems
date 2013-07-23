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
#include <string.h>

#include <rtems/libcsupport.h>
#include <rtems/score/wkspace.h>

bool newlib_create_hook(
  rtems_tcb *current_task __attribute__((unused)),
  rtems_tcb *creating_task
)
{
  struct _reent *ptr;
  bool ok;

#if !defined(__DYNAMIC_REENT__)
  if (_Thread_libc_reent == 0)
  {
    _REENT = _GLOBAL_REENT;

    _Thread_Set_libc_reent (&_REENT);
  }
#endif

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

void newlib_delete_hook(
  rtems_tcb *current_task,
  rtems_tcb *deleted_task
)
{
  struct _reent *ptr;

  ptr = deleted_task->libc_reent;
  deleted_task->libc_reent = NULL;

  _reclaim_reent(ptr);
  _Workspace_Free(ptr);
}

#endif
