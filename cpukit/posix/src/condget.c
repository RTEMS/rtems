/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/condimpl.h>
#include <rtems/posix/posixapi.h>

POSIX_Condition_variables_Control *_POSIX_Condition_variables_Get(
  pthread_cond_t       *cond,
  Thread_queue_Context *queue_context
)
{
  _POSIX_Get_object_body(
    POSIX_Condition_variables_Control,
    cond,
    queue_context,
    &_POSIX_Condition_variables_Information,
    PTHREAD_COND_INITIALIZER,
    pthread_cond_init
  );
}
