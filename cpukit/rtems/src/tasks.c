/**
 *  @file
 *
 *  @brief RTEMS Task API Extensions
 *  @ingroup ClassicTasks
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/config.h>
#include <rtems/sysinit.h>
#include <rtems/rtems/eventimpl.h>
#include <rtems/rtems/tasksimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/userextimpl.h>

Thread_Information _RTEMS_tasks_Information;

static void _RTEMS_tasks_Start_extension(
  Thread_Control *executing,
  Thread_Control *started
)
{
  RTEMS_API_Control *api;

  api = started->API_Extensions[ THREAD_API_RTEMS ];

  _Event_Initialize( &api->Event );
  _Event_Initialize( &api->System_event );
}

#if defined(RTEMS_MULTIPROCESSING)
static void _RTEMS_tasks_Terminate_extension( Thread_Control *executing )
{
  if ( executing->is_global ) {
    _Objects_MP_Close(
      &_RTEMS_tasks_Information.Objects,
      executing->Object.id
    );
    _RTEMS_tasks_MP_Send_process_packet(
      RTEMS_TASKS_MP_ANNOUNCE_DELETE,
      executing->Object.id,
      0                                /* Not used */
    );
  }
}
#endif

User_extensions_Control _RTEMS_tasks_User_extensions = {
  .Callouts = {
#if defined(RTEMS_MULTIPROCESSING)
    .thread_terminate = _RTEMS_tasks_Terminate_extension,
#endif
    .thread_start     = _RTEMS_tasks_Start_extension,
    .thread_restart   = _RTEMS_tasks_Start_extension
  }
};

static void _RTEMS_tasks_Manager_initialization(void)
{
  _Thread_Initialize_information(
    &_RTEMS_tasks_Information, /* object information table */
    OBJECTS_CLASSIC_API,       /* object API */
    OBJECTS_RTEMS_TASKS,       /* object class */
    Configuration_RTEMS_API.maximum_tasks
                               /* maximum objects of this class */
  );

  /*
   *  Add all the extensions for this API
   */

  _User_extensions_Add_API_set( &_RTEMS_tasks_User_extensions );

  /*
   *  Register the MP Process Packet routine.
   */

#if defined(RTEMS_MULTIPROCESSING)
  _MPCI_Register_packet_processor(
    MP_PACKET_TASKS,
    _RTEMS_tasks_MP_Process_packet
  );
#endif

}

RTEMS_SYSINIT_ITEM(
  _RTEMS_tasks_Manager_initialization,
  RTEMS_SYSINIT_CLASSIC_TASKS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
