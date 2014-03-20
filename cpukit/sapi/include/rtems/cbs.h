/**
 * @file
 *
 * @brief Constants and Structures Associated
 * with the CBS library in RTEMS
 *
 * This include file contains all the constants and structures associated
 * with the CBS library in RTEMS.
 */

/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef CONFIGURE_SCHEDULER_CBS
  #error "cbs.h available only with CONFIGURE_SCHEDULER_CBS"
#endif

#ifndef _RTEMS_CBS_H
#define _RTEMS_CBS_H

#include <rtems/score/schedulercbs.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Return codes. */
#define RTEMS_CBS_OK                       SCHEDULER_CBS_OK
#define RTEMS_CBS_ERROR_GENERIC            SCHEDULER_CBS_ERROR_GENERIC
#define RTEMS_CBS_ERROR_NO_MEMORY          SCHEDULER_CBS_ERROR_NO_MEMORY
#define RTEMS_CBS_ERROR_INVALID_PARAMETER  SCHEDULER_CBS_ERROR_INVALID_PARAM
#define RTEMS_CBS_ERROR_UNAUTHORIZED       SCHEDULER_CBS_ERROR_UNAUTHORIZED
#define RTEMS_CBS_ERROR_UNIMPLEMENTED      SCHEDULER_CBS_ERROR_UNIMPLEMENTED
#define RTEMS_CBS_ERROR_MISSING_COMPONENT  SCHEDULER_CBS_ERROR_MISSING_COMPONENT
#define RTEMS_CBS_ERROR_INCONSISTENT_STATE SCHEDULER_CBS_ERROR_INCONSISTENT_STATE
#define RTEMS_CBS_ERROR_SYSTEM_OVERLOAD    SCHEDULER_CBS_ERROR_SYSTEM_OVERLOAD
#define RTEMS_CBS_ERROR_INTERNAL_ERROR     SCHEDULER_CBS_ERROR_INTERNAL_ERROR
#define RTEMS_CBS_ERROR_NOT_FOUND          SCHEDULER_CBS_ERROR_NOT_FOUND
#define RTEMS_CBS_ERROR_FULL               SCHEDULER_CBS_ERROR_FULL
#define RTEMS_CBS_ERROR_EMPTY              SCHEDULER_CBS_ERROR_EMPTY
#define RTEMS_CBS_ERROR_NOSERVER           SCHEDULER_CBS_ERROR_NOSERVER

/** Callback function invoked when a budget overrun of a task occurs. */
typedef Scheduler_CBS_Budget_overrun rtems_cbs_budget_overrun;

/** Server id. */
typedef Scheduler_CBS_Server_id rtems_cbs_server_id;

/** Server parameters. */
typedef Scheduler_CBS_Parameters rtems_cbs_parameters;

/**
 * @brief Initialize the CBS library.
 *
 * Initializes the CBS library.
 *
 * @return status code.
 */
RTEMS_INLINE_ROUTINE int rtems_cbs_initialize ( void )
{
  return _Scheduler_CBS_Initialize();
}

/**
 * @brief Cleanup resources associated to the CBS Library
 *
 * Cleanup resources associated to the CBS Library.
 *
 * @return status code.
 */
RTEMS_INLINE_ROUTINE int rtems_cbs_cleanup ( void )
{
  return _Scheduler_CBS_Cleanup();
}

/**
 * @brief Create a new server with specified parameters.
 *
 * Create a new server with specified parameters.
 *
 * @return status code.
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
 * @brief Attach a task to an already existing server.
 *
 * Attach a task to an already existing server.
 *
 * @return status code.
 */
RTEMS_INLINE_ROUTINE int rtems_cbs_attach_thread (
  rtems_cbs_server_id server_id,
  rtems_id            task_id
)
{
  return _Scheduler_CBS_Attach_thread( server_id, task_id );
}

/**
 * @brief Detach from the CBS server.
 *
 * Detach from the CBS Server.
 *
 * @return status code.
 */
RTEMS_INLINE_ROUTINE int rtems_cbs_detach_thread (
  rtems_cbs_server_id server_id,
  rtems_id            task_id
)
{
  return _Scheduler_CBS_Detach_thread( server_id, task_id );
}

/**
 * @brief Detach all tasks from a server and destroy it.
 *
 * Detach all tasks from a server and destroy it.
 *
 * @return status code.
 */
RTEMS_INLINE_ROUTINE int rtems_cbs_destroy_server (
  rtems_cbs_server_id server_id
)
{
  return _Scheduler_CBS_Destroy_server( server_id );
}

/**
 * @brief Get CBS server id.
 *
 * Get a thread server id, or RTEMS_CBS_E_NOT_FOUND if it is not
 * attached to any server.
 *
 * @return status code.
 */
RTEMS_INLINE_ROUTINE int rtems_cbs_get_server_id (
  rtems_id             task_id,
  rtems_cbs_server_id *server_id
)
{
  return _Scheduler_CBS_Get_server_id( task_id, server_id );
}

/**
 * @brief Get CBS parameters.
 *
 * Retrieve CBS scheduling parameters.
 *
 * @return status code.
 */
RTEMS_INLINE_ROUTINE int rtems_cbs_get_parameters (
  rtems_cbs_server_id   server_id,
  rtems_cbs_parameters *params
)
{
  return _Scheduler_CBS_Get_parameters( server_id, params );
}

/**
 * @brief Set CBS parameters.
 *
 * Change CBS scheduling parameters.
 *
 * @return status code.
 */
RTEMS_INLINE_ROUTINE int rtems_cbs_set_parameters (
  rtems_cbs_server_id   server_id,
  rtems_cbs_parameters *params
)
{
  return _Scheduler_CBS_Set_parameters( server_id, params );
}

/**
 * @brief Get the CBS get execution time.
 *
 * Retrieve time info relative to the current server.
 *
 * @return status code.
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
 * @brief Get the remaining CBS budget.
 *
 * Retrieve remaining budget for the current server instance.
 *
 * @return status code.
 */
RTEMS_INLINE_ROUTINE int rtems_cbs_get_remaining_budget (
  rtems_cbs_server_id  server_id,
  time_t              *remaining_budget
)
{
  return _Scheduler_CBS_Get_remaining_budget( server_id, remaining_budget );
}

/**
 * @brief Get the approved CBS budget.
 *
 * Retrieve the budget that has been approved for the subsequent
 * server instances.
 *
 * @return status code.
 */
RTEMS_INLINE_ROUTINE int rtems_cbs_get_approved_budget (
  rtems_cbs_server_id  server_id,
  time_t              *appr_budget
)
{
  return _Scheduler_CBS_Get_approved_budget( server_id, appr_budget );
}

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
