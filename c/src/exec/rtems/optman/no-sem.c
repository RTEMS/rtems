/*
 *  Semaphore Manager
 *
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/attr.h>
#include <rtems/config.h>
#include <rtems/core/isr.h>
#include <rtems/core/object.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/sem.h>
#include <rtems/core/states.h>
#include <rtems/core/thread.h>
#include <rtems/core/threadq.h>

void _Semaphore_Manager_initialization(
  unsigned32 maximum_semaphores
)
{
}

rtems_status_code rtems_semaphore_create(
  rtems_name            name,
  unsigned32            count,
  rtems_attribute       attribute_set,
  rtems_task_priority   priority_ceiling,
  Objects_Id           *id
)
{
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_semaphore_ident(
  rtems_name    name,
  unsigned32    node,
  Objects_Id   *id
)
{
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_semaphore_delete(
  Objects_Id id
)
{
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_semaphore_obtain(
  Objects_Id        id,
  unsigned32        option_set,
  rtems_interval timeout
)
{
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_semaphore_release(
  Objects_Id id
)
{
  return RTEMS_NOT_CONFIGURED;
}

boolean _Semaphore_Seize(
  Semaphore_Control *the_semaphore,
  rtems_option    option_set
)
{
  _Thread_Executing->Wait.return_code = RTEMS_UNSATISFIED;
  return( TRUE );
}
