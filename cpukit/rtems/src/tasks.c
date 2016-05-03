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
#include <rtems/rtems/asrimpl.h>
#include <rtems/rtems/eventimpl.h>
#include <rtems/rtems/tasksimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/userextimpl.h>

Thread_Information _RTEMS_tasks_Information;

/*
 *  _RTEMS_tasks_Create_extension
 *
 *  This routine is an extension routine that is invoked as part
 *  of creating any type of task or thread in the system.  If the
 *  task is created via another API, then this routine is invoked
 *  and this API given the opportunity to initialize its extension
 *  area.
 */

static bool _RTEMS_tasks_Create_extension(
  Thread_Control *executing,
  Thread_Control *created
)
{
  RTEMS_API_Control *api;

  api = created->API_Extensions[ THREAD_API_RTEMS ];

  _ASR_Create( &api->Signal );

  return true;
}

/*
 *  _RTEMS_tasks_Start_extension
 *
 *  This extension routine is invoked when a task is started for the
 *  first time.
 */

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

static void _RTEMS_tasks_Delete_extension(
  Thread_Control *executing,
  Thread_Control *deleted
)
{
  RTEMS_API_Control *api;

  api = deleted->API_Extensions[ THREAD_API_RTEMS ];

  _ASR_Destroy( &api->Signal );
}

User_extensions_Control _RTEMS_tasks_User_extensions = {
  .Callouts = {
    .thread_create  = _RTEMS_tasks_Create_extension,
    .thread_start   = _RTEMS_tasks_Start_extension,
    .thread_restart = _RTEMS_tasks_Start_extension,
    .thread_delete  = _RTEMS_tasks_Delete_extension
  }
};

static void _RTEMS_tasks_Manager_initialization(void)
{
  _Thread_Initialize_information(
    &_RTEMS_tasks_Information, /* object information table */
    OBJECTS_CLASSIC_API,       /* object API */
    OBJECTS_RTEMS_TASKS,       /* object class */
    Configuration_RTEMS_API.maximum_tasks,
                               /* maximum objects of this class */
    false,                     /* true if the name is a string */
    RTEMS_MAXIMUM_NAME_LENGTH  /* maximum length of an object name */
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
