/*
 *  ITRON API Initialization Support
 *
 *  NOTE:
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <assert.h>

/*
 *  ITRON_API_INIT is defined so all of the ITRON API
 *  data will be included in this object file.
 */

#define ITRON_API_INIT

#include <rtems/system.h>    /* include this before checking RTEMS_ITRON_API */
#ifdef RTEMS_ITRON_API

#include <itron.h>

#include <sys/types.h>
#include <rtems/config.h>
#include <rtems/score/object.h>

#include <rtems/itron/eventflags.h>
#include <rtems/itron/fmempool.h>
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

itron_api_configuration_table _ITRON_Default_configuration = {
  0,                             /* maximum_tasks */
  0,                             /* maximum_semaphores */
  0,                             /* maximum_eventflags */
  0,                             /* maximum_mailboxes */
  0,                             /* maximum_message_buffers */
  0,                             /* maximum_ports */
  0,                             /* maximum_memory_pools */
  0,                             /* maximum_fixed_memory_pools */
  0,                             /* number_of_initialization_tasks */
  NULL                           /* User_initialization_tasks_table */
};

Objects_Information *_ITRON_Objects[ OBJECTS_ITRON_CLASSES_LAST + 1 ];

void _ITRON_API_Initialize(
  rtems_configuration_table *configuration_table
)
{
  itron_api_configuration_table *api_configuration;

  /* XXX need to assert here based on size assumptions */

  assert( sizeof(ID) == sizeof(Objects_Id) );

  api_configuration = configuration_table->ITRON_api_configuration;
  if ( !api_configuration ) 
    api_configuration = &_ITRON_Default_configuration;

  _Objects_Information_table[OBJECTS_ITRON_API] = _ITRON_Objects;
    
  _ITRON_Task_Manager_initialization(
    api_configuration->maximum_tasks,
    api_configuration->number_of_initialization_tasks,
    api_configuration->User_initialization_tasks_table
  );
 
  _ITRON_Semaphore_Manager_initialization( 
    api_configuration->maximum_semaphores
  );

  _ITRON_Eventflags_Manager_initialization(
    api_configuration->maximum_eventflags
  );

  _ITRON_Fixed_memory_pool_Manager_initialization(
    api_configuration->maximum_fixed_memory_pools
  );

  _ITRON_Mailbox_Manager_initialization(
    api_configuration->maximum_mailboxes
  );

  _ITRON_Message_buffer_Manager_initialization(
    api_configuration->maximum_message_buffers
  );

  _ITRON_Port_Manager_initialization(
    api_configuration->maximum_ports
  );

  _ITRON_Variable_memory_pool_Manager_initialization(
    api_configuration->maximum_memory_pools
  );


}

#endif
/* end of file */
