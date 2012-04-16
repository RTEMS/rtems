/**
 * @file qreslib.inl
 *
 *  This include file contains all the constants and structures associated
 *  with the QoS RES library.
 *
 * @note The library is available only together with CBS scheduler.
 */

/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _QRESLIB_H
# error "Never use <qreslib.inl> directly; include <qreslib.h> instead."
#endif

#include <rtems/score/schedulercbs.h>

/**
 *  @brief qres init
 *
 *  Initializes the QoS RES library.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE qos_rv qres_init ( void )
{
  return _Scheduler_CBS_Initialize();
}

/**
 *  @brief qres cleanup
 *
 *  Cleanup resources associated to the QoS RES Library.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE qos_rv qres_cleanup ( void )
{
  return _Scheduler_CBS_Cleanup();
}

/**
 *  @brief qres create server
 *
 *  Create a new server with specified parameters.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE qos_rv qres_create_server (
  qres_params_t *params,
  qres_sid_t    *server_id
)
{
  return _Scheduler_CBS_Create_server(
             (Scheduler_CBS_Parameters *) params,
             NULL,
             server_id
         );
}

/**
 *  @brief qres attach thread
 *
 *  Attach a task to an already existing server.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE qos_rv qres_attach_thread (
  qres_sid_t server_id,
  pid_t      pid,
  tid_t      task_id
)
{
  return _Scheduler_CBS_Attach_thread( server_id, task_id );
}

/**
 *  @brief qres detach thread
 *
 *  Detach from the QoS Server.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE qos_rv qres_detach_thread (
  qres_sid_t server_id,
  pid_t      pid,
  tid_t      task_id
)
{
  return _Scheduler_CBS_Detach_thread( server_id, task_id );
}

/**
 *  @brief qres destroy server
 *
 *  Detach all tasks from a server and destroy it.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE qos_rv qres_destroy_server (
  qres_sid_t server_id
)
{
  return _Scheduler_CBS_Destroy_server( server_id );
}

/**
 *  @brief qres get server id
 *
 *  Get a thread server id, or QOS_E_NOT_FOUND if it is not
 *  attached to any server.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE qos_rv qres_get_sid (
  pid_t       pid,
  tid_t       task_id,
  qres_sid_t *server_id
)
{
  return _Scheduler_CBS_Get_server_id( task_id, server_id );
}

/**
 *  @brief qres get params
 *
 *  Retrieve QoS scheduling parameters.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE qos_rv qres_get_params (
  qres_sid_t     server_id,
  qres_params_t *params
)
{
  return _Scheduler_CBS_Get_parameters(
             server_id,
             (Scheduler_CBS_Parameters *) params
         );
}

/**
 *  @brief qres set params
 *
 *  Change QoS scheduling parameters.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE qos_rv qres_set_params (
  qres_sid_t     server_id,
  qres_params_t *params
)
{
  return _Scheduler_CBS_Set_parameters(
             server_id,
             (Scheduler_CBS_Parameters *) params
         );
}

/**
 *  @brief qres get execution time
 *
 *  Retrieve time info relative to the current server.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE qos_rv qres_get_exec_time (
  qres_sid_t    server_id,
  qres_time_t  *exec_time,
  qres_atime_t *abs_time
)
{
  return _Scheduler_CBS_Get_execution_time( server_id, exec_time, abs_time );
}

/**
 *  @brief qres get current budget
 *
 *  Retrieve remaining budget for the current server instance.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE qos_rv qres_get_curr_budget (
  qres_sid_t   server_id,
  qres_time_t *current_budget
)
{
  return _Scheduler_CBS_Get_remaining_budget( server_id, current_budget );
}

/**
 *  @brief qres get approved budget
 *
 *  Retrieve the budget that has been approved for the subsequent
 *  server instances.
 *
 *  @return status code.
 */
RTEMS_INLINE_ROUTINE qos_rv qres_get_appr_budget (
  qres_sid_t   server_id,
  qres_time_t *appr_budget
)
{
  return _Scheduler_CBS_Get_approved_budget( server_id, appr_budget );
}
