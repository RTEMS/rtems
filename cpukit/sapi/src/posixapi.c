/**
 * @file
 *
 * @brief Initialize POSIX API
 *
 * @ingroup ClassicRTEMS
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#define POSIX_API_INIT

#include <rtems/system.h>    /* include this before checking RTEMS_POSIX_API */
#ifdef RTEMS_POSIX_API

#include <sys/types.h>
#include <mqueue.h>
#include <rtems/config.h>
#include <rtems/posix/barrierimpl.h>
#include <rtems/posix/condimpl.h>
#include <rtems/posix/config.h>
#include <rtems/posix/key.h>
#include <rtems/posix/mqueueimpl.h>
#include <rtems/posix/muteximpl.h>
#include <rtems/posix/posixapi.h>
#include <rtems/posix/priorityimpl.h>
#include <rtems/posix/psignalimpl.h>
#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/rwlockimpl.h>
#include <rtems/posix/timerimpl.h>
#include <rtems/posix/semaphoreimpl.h>
#include <rtems/posix/spinlockimpl.h>
#include <rtems/posix/time.h>

Objects_Information *_POSIX_Objects[ OBJECTS_POSIX_CLASSES_LAST + 1 ];

void _POSIX_API_Initialize(void)
{
  /*
   * If there are any type size assumptions in the POSIX API, this is
   * the appropriate place to place them.
   *
   * Currently, there are no none type size assumptions.
   */

  /*
   * Install our API Object Management Table and initialize the
   * various managers.
   */
  _Objects_Information_table[OBJECTS_POSIX_API] = _POSIX_Objects;

  _POSIX_signals_Manager_Initialization();
  _POSIX_Threads_Manager_initialization();
  _POSIX_Condition_variables_Manager_initialization();
  _POSIX_Key_Manager_initialization();
  _POSIX_Mutex_Manager_initialization();
  _POSIX_Message_queue_Manager_initialization();
  _POSIX_Semaphore_Manager_initialization();
  _POSIX_Timer_Manager_initialization();
  _POSIX_Barrier_Manager_initialization();
  _POSIX_RWLock_Manager_initialization();
  _POSIX_Spinlock_Manager_initialization();
}

#endif
