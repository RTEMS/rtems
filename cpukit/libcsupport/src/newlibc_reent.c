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
 *  http://www.rtems.org/license/LICENSE.
 *
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>

#if defined(RTEMS_NEWLIB)

#include <sys/reent.h>
#include <stdlib.h>
#include <string.h>

#include <rtems/libcsupport.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/wkspace.h>

bool newlib_create_hook(
  rtems_tcb *current_task __attribute__((unused)),
  rtems_tcb *creating_task
)
{
#if !defined(__DYNAMIC_REENT__)
  if (_Thread_libc_reent == 0)
  {
    _REENT = _GLOBAL_REENT;

    _Thread_Set_libc_reent (&_REENT);
  }
#endif

  _REENT_INIT_PTR((creating_task->libc_reent)); /* GCC extension: structure constants */

  return true;
}

void newlib_terminate_hook(
  rtems_tcb *current_task
)
{
  _reclaim_reent(current_task->libc_reent);
}

#endif
