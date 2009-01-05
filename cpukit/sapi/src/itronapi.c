/*
 *  ITRON API Initialization Support
 *
 *  NOTE:
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>

/*
 *  ITRON_API_INIT is defined so all of the ITRON API
 *  data will be included in this object file.
 */

#define ITRON_API_INIT

#include <rtems/system.h>    /* include this before checking RTEMS_ITRON_API */
#ifdef RTEMS_ITRON_API

#include <rtems/itron.h>

#include <sys/types.h>
#include <rtems/config.h>
#include <rtems/score/object.h>

#include <rtems/itron/eventflags.h>
#include <rtems/itron/fmempool.h>
#include <rtems/itron/itronapi.h>
#include <rtems/itron/mbox.h>
#include <rtems/itron/msgbuffer.h>
#include <rtems/itron/port.h>
#include <rtems/itron/semaphore.h>
#include <rtems/itron/task.h>
#include <rtems/itron/vmempool.h>

/*PAGE
 *
 *  _ITRON_API_Initialize
 *
 *  XXX
 */

Objects_Information *_ITRON_Objects[ OBJECTS_ITRON_CLASSES_LAST + 1 ];

void _ITRON_API_Initialize(void)
{
  /*
   * If there are any type size assumptions in the ITRON API, this is
   * the appropriate place to place them.
   *
   * Currently, there are no none type size assumptions.
   */

  /*
   * Install our API Object Management Table and initialize the
   * various managers.
   */
  _Objects_Information_table[OBJECTS_ITRON_API] = _ITRON_Objects;

  _ITRON_Task_Manager_initialization();
  _ITRON_Semaphore_Manager_initialization();
  _ITRON_Eventflags_Manager_initialization();
  _ITRON_Fixed_memory_pool_Manager_initialization();
  _ITRON_Mailbox_Manager_initialization();
  _ITRON_Message_buffer_Manager_initialization();
  _ITRON_Port_Manager_initialization();
  _ITRON_Variable_memory_pool_Manager_initialization();
}

#endif
/* end of file */
