/**
 * @file cbs.inl
 *
 *  This include file contains all the constants and structures associated
 *  with the CBS library.
 *
 */

/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_CBS_H
# error "Never use <rtems/cbs.inl> directly; include <rtems/cbs.h> instead."
#endif

#include <rtems/score/schedulercbs.h>

/**
 *  @brief rtems cbs init
 *
 *  Initializes the CBS library.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE int rtems_cbs_initialize ( void )
{
  return _Scheduler_CBS_Initialize();
}

/**
 *  @brief rtems cbs cleanup
 *
 *  Cleanup resources associated to the CBS Library.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE int rtems_cbs_cleanup ( void )
{
  return _Scheduler_CBS_Cleanup();
}

/**
 *  @brief rtems cbs create server
 *
 *  Create a new server with specified parameters.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE int rtems_cbs_create_server (
  rtems_cbs_parameters     *params,
  rtems_cbs_budget_overrun  budget_overrun_callback,
  rtems_cbs_server_id      *server_id
)
{
  return _Scheduler_CBS_Create_server(
             params,
             budget_overrun_callback,
             server_id
         );
}

/**
 *  @brief rtems cbs attach thread
 *
 *  Attach a task to an already existing server.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE int rtems_cbs_attach_thread (
  rtems_cbs_server_id server_id,
  rtems_id            task_id
)
{
  return _Scheduler_CBS_Attach_thread( server_id, task_id );
}

/**
 *  @brief rtems cbs detach thread
 *
 *  Detach from the CBS Server.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE int rtems_cbs_detach_thread (
  rtems_cbs_server_id server_id,
  rtems_id            task_id
)
{
  return _Scheduler_CBS_Detach_thread( server_id, task_id );
}

/**
 *  @brief rtems cbs destroy server
 *
 *  Detach all tasks from a server and destroy it.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE int rtems_cbs_destroy_server (
  rtems_cbs_server_id server_id
)
{
  return _Scheduler_CBS_Destroy_server( server_id );
}

/**
 *  @brief rtems cbs get server id
 *
 *  Get a thread server id, or RTEMS_CBS_E_NOT_FOUND if it is not
 *  attached to any server.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE int rtems_cbs_get_server_id (
  rtems_id             task_id,
  rtems_cbs_server_id *server_id
)
{
  return _Scheduler_CBS_Get_server_id( task_id, server_id );
}

/**
 *  @brief rtems cbs get parameters
 *
 *  Retrieve CBS scheduling parameters.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE int rtems_cbs_get_parameters (
  rtems_cbs_server_id   server_id,
  rtems_cbs_parameters *params
)
{
  return _Scheduler_CBS_Get_parameters( server_id, params );
}

/**
 *  @brief rtems cbs set parameters
 *
 *  Change CBS scheduling parameters.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE int rtems_cbs_set_parameters (
  rtems_cbs_server_id   server_id,
  rtems_cbs_parameters *params
)
{
  return _Scheduler_CBS_Set_parameters( server_id, params );
}

/**
 *  @brief rtems cbs get execution time
 *
 *  Retrieve time info relative to the current server.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE int rtems_cbs_get_execution_time (
  rtems_cbs_server_id    server_id,
  time_t                *exec_time,
  time_t                *abs_time
)
{
  return _Scheduler_CBS_Get_execution_time( server_id, exec_time, abs_time );
}

/**
 *  @brief rtems cbs get remaining budget
 *
 *  Retrieve remaining budget for the current server instance.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE int rtems_cbs_get_remaining_budget (
  rtems_cbs_server_id  server_id,
  time_t              *remaining_budget
)
{
  return _Scheduler_CBS_Get_remaining_budget( server_id, remaining_budget );
}

/**
 *  @brief rtems cbs get approved budget
 *
 *  Retrieve the budget that has been approved for the subsequent
 *  server instances.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE int rtems_cbs_get_approved_budget (
  rtems_cbs_server_id  server_id,
  time_t              *appr_budget
)
{
  return _Scheduler_CBS_Get_approved_budget( server_id, appr_budget );
}
