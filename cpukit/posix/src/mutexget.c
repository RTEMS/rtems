/**
 * @file
 *
 * @brief Convert POSIX Mutex ID to local object pointer
 * @ingroup POSIXAPI
 */

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

#include <rtems/posix/muteximpl.h>
#include <rtems/posix/posixapi.h>

POSIX_Mutex_Control *_POSIX_Mutex_Get(
  pthread_mutex_t      *mutex,
  Thread_queue_Context *queue_context
)
{
  _POSIX_Get_object_body(
    POSIX_Mutex_Control,
    mutex,
    queue_context,
    &_POSIX_Mutex_Information,
    PTHREAD_MUTEX_INITIALIZER,
    pthread_mutex_init
  );
}
