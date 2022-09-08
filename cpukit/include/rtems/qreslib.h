/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Constants and Structures Associated
 * with the QoS RES library in RTEMS
 *
 * This include file contains all the constants and structures
 * associated with the QoS RES library in RTEMS.
 *
 * @note The library is available only together with CBS scheduler.
 */

/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CONFIGURE_SCHEDULER_CBS
  #error "qreslib.h available only with CONFIGURE_SCHEDULER_CBS"
#endif

#ifndef _QRESLIB_H
#define _QRESLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <rtems/score/schedulercbs.h>

/** Return values. */
typedef int qos_rv;

/* Return codes. */
#define QOS_OK                   SCHEDULER_CBS_OK
#define QOS_E_GENERIC            SCHEDULER_CBS_ERROR_GENERIC
#define QOS_E_NO_MEMORY          SCHEDULER_CBS_ERROR_NO_MEMORY
#define QOS_E_INVALID_PARAM      SCHEDULER_CBS_ERROR_INVALID_PARAMETER
#define QOS_E_UNAUTHORIZED       SCHEDULER_CBS_ERROR_UNAUTHORIZED
#define QOS_E_UNIMPLEMENTED      SCHEDULER_CBS_ERROR_UNIMPLEMENTED
#define QOS_E_MISSING_COMPONENT  SCHEDULER_CBS_ERROR_MISSING_COMPONENT
#define QOS_E_INCONSISTENT_STATE SCHEDULER_CBS_ERROR_INCONSISTENT_STATE
#define QOS_E_SYSTEM_OVERLOAD    SCHEDULER_CBS_ERROR_SYSTEM_OVERLOAD
#define QOS_E_INTERNAL_ERROR     SCHEDULER_CBS_ERROR_INTERNAL_ERROR
#define QOS_E_NOT_FOUND          SCHEDULER_CBS_ERROR_NOT_FOUND
#define QOS_E_FULL               SCHEDULER_CBS_ERROR_FULL
#define QOS_E_EMPTY              SCHEDULER_CBS_ERROR_EMPTY
#define QOS_E_NOSERVER           SCHEDULER_CBS_ERROR_NOSERVER

/** Server id. */
typedef Scheduler_CBS_Server_id qres_sid_t;

/** Task id. */
typedef rtems_id tid_t;

/** Time value. */
typedef time_t qres_time_t;

/** Absolute time value */
typedef time_t qres_atime_t;

/** Server parameters. */
typedef struct {
  /** Relative deadline of the server. */
  qres_time_t P;
  /** Budget (computation time) of the server. */
  qres_time_t Q;
} qres_params_t;

/**
 *  @brief qres init
 *
 *  Initializes the QoS RES library.
 *
 *  @return status code.
 */
static inline qos_rv qres_init ( void )
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
static inline qos_rv qres_cleanup ( void )
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
static inline qos_rv qres_create_server (
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
static inline qos_rv qres_attach_thread (
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
static inline qos_rv qres_detach_thread (
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
static inline qos_rv qres_destroy_server (
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
static inline qos_rv qres_get_sid (
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
static inline qos_rv qres_get_params (
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
static inline qos_rv qres_set_params (
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
static inline qos_rv qres_get_exec_time (
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
static inline qos_rv qres_get_curr_budget (
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
static inline qos_rv qres_get_appr_budget (
  qres_sid_t   server_id,
  qres_time_t *appr_budget
)
{
  return _Scheduler_CBS_Get_approved_budget( server_id, appr_budget );
}

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
