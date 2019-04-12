/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerCBS
 *
 * @brief Thread manipulation for the CBS scheduler
 *
 * This include file contains all the constants and structures associated
 * with the manipulation of threads for the CBS scheduler.
 */

/*
 *  Copryight (c) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULERCBS_H
#define _RTEMS_SCORE_SCHEDULERCBS_H

#include <rtems/score/chain.h>
#include <rtems/score/priority.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/rbtree.h>
#include <rtems/score/scheduleredf.h>
#include <rtems/rtems/signal.h>
#include <rtems/rtems/timer.h>
#include <rtems/score/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScoreSchedulerCBS CBS Scheduler
 *
 * @ingroup RTEMSScoreScheduler
 *
 * @brief CBS Scheduler.
 *
 * @{
 */

#define SCHEDULER_CBS_MAXIMUM_PRIORITY SCHEDULER_EDF_MAXIMUM_PRIORITY

/**
 *  Entry points for the Constant Bandwidth Server Scheduler.
 *
 *  @note: The CBS scheduler is an enhancement of EDF scheduler,
 *         therefor some routines are similar.
 */
#define SCHEDULER_CBS_ENTRY_POINTS \
  { \
    _Scheduler_EDF_Initialize,       /* initialize entry point */ \
    _Scheduler_EDF_Schedule,         /* schedule entry point */ \
    _Scheduler_EDF_Yield,            /* yield entry point */ \
    _Scheduler_EDF_Block,            /* block entry point */ \
    _Scheduler_CBS_Unblock,          /* unblock entry point */ \
    _Scheduler_EDF_Update_priority,  /* update priority entry point */ \
    _Scheduler_EDF_Map_priority,     /* map priority entry point */ \
    _Scheduler_EDF_Unmap_priority,   /* unmap priority entry point */ \
    SCHEDULER_OPERATION_DEFAULT_ASK_FOR_HELP \
    _Scheduler_CBS_Node_initialize,  /* node initialize entry point */ \
    _Scheduler_default_Node_destroy, /* node destroy entry point */ \
    _Scheduler_CBS_Release_job,      /* new period of task */ \
    _Scheduler_CBS_Cancel_job,       /* cancel period of task */ \
    _Scheduler_default_Tick,         /* tick entry point */ \
    _Scheduler_default_Start_idle    /* start idle entry point */ \
    SCHEDULER_OPERATION_DEFAULT_GET_SET_AFFINITY \
  }

/* Return values for CBS server. */
#define SCHEDULER_CBS_OK                        0
#define SCHEDULER_CBS_ERROR_GENERIC            -16
#define SCHEDULER_CBS_ERROR_NO_MEMORY          -17
#define SCHEDULER_CBS_ERROR_INVALID_PARAMETER  -18
#define SCHEDULER_CBS_ERROR_UNAUTHORIZED       -19
#define SCHEDULER_CBS_ERROR_UNIMPLEMENTED      -20
#define SCHEDULER_CBS_ERROR_MISSING_COMPONENT  -21
#define SCHEDULER_CBS_ERROR_INCONSISTENT_STATE -22
#define SCHEDULER_CBS_ERROR_SYSTEM_OVERLOAD    -23
#define SCHEDULER_CBS_ERROR_INTERNAL_ERROR     -24
#define SCHEDULER_CBS_ERROR_NOT_FOUND          -25
#define SCHEDULER_CBS_ERROR_FULL               -26
#define SCHEDULER_CBS_ERROR_EMPTY              -27
#define SCHEDULER_CBS_ERROR_NOSERVER           SCHEDULER_CBS_ERROR_NOT_FOUND

/** Maximum number of simultaneous servers. */
extern const uint32_t _Scheduler_CBS_Maximum_servers;

/** Server id. */
typedef uint32_t Scheduler_CBS_Server_id;

/** Callback function invoked when a budget overrun of a task occurs. */
typedef void (*Scheduler_CBS_Budget_overrun)(
    Scheduler_CBS_Server_id server_id
);

/**
 * This structure handles server parameters.
 */
typedef struct {
  /** Relative deadline of the server. */
  time_t deadline;
  /** Budget (computation time) of the server. */
  time_t budget;
} Scheduler_CBS_Parameters;

/**
 * This structure represents a time server.
 */
typedef struct {
  /**
   * Task id.
   *
   * @note: The current implementation of CBS handles only one task per server.
   */
  rtems_id                 task_id;
  /** Server paramenters. */
  Scheduler_CBS_Parameters parameters;
  /** Callback function invoked when a budget overrun occurs. */
  Scheduler_CBS_Budget_overrun  cbs_budget_overrun;

  /**
   * @brief Indicates if this CBS server is initialized.
   *
   * @see _Scheduler_CBS_Create_server() and _Scheduler_CBS_Destroy_server().
   */
  bool initialized;
} Scheduler_CBS_Server;

/**
 * This structure handles CBS specific data of a thread.
 */
typedef struct {
  /** EDF scheduler specific data of a task. */
  Scheduler_EDF_Node            Base;
  /** CBS server specific data of a task. */
  Scheduler_CBS_Server         *cbs_server;

  Priority_Node                *deadline_node;
} Scheduler_CBS_Node;


/**
 * List of servers. The @a Scheduler_CBS_Server is the index to the array
 * of pointers to @a _Scheduler_CBS_Server_list.
 */
extern Scheduler_CBS_Server _Scheduler_CBS_Server_list[];

/**
 * @brief Unblocks a thread.
 *
 * @param scheduler The scheduler control.
 * @param the_thread The thread to unblock.
 * @param node The scheduler node.
 */
void _Scheduler_CBS_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Releases a job.
 *
 * @param scheduler The scheduler for the operation.
 * @param the_thread The corresponding thread.
 * @param priority_node The priority node for the operation.
 * @param deadline The deadline for the job.
 * @param queue_context The thread queue context.
 */
void _Scheduler_CBS_Release_job(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Priority_Node           *priority_node,
  uint64_t                 deadline,
  Thread_queue_Context    *queue_context
);

/**
 * @brief Cancels a job.
 *
 * @param scheduler The scheduler for the operation.
 * @param the_thread The corresponding thread.
 * @param priority_node The priority node for the operation.
 * @param queue_context The thread queue context.
 */
void _Scheduler_CBS_Cancel_job(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Priority_Node           *priority_node,
  Thread_queue_Context    *queue_context
);

/**
 * @brief _Scheduler_CBS_Initialize
 *
 * Initializes the CBS library.
 *
 * @return SCHEDULER_CBS_OK This method always returns this status.
 */
int _Scheduler_CBS_Initialize(void);

/**
 * @brief Attaches a task to an already existing server.
 *
 * Attach a task to an already existing server.
 *
 * @param server_id The id of the existing server.
 * @param task_id The id of the task to attach.
 *
 * @retval SCHEDULER_CBS_OK The operation was successful.
 * @retval SCHEDULER_CBS_ERROR_INVALID_PARAMETER The server id is so big
 *      or there is no thread for this task id.
 * @retval SCHEDULER_CBS_ERROR_NOSERVER The server is not yet initialized.
 * @retval SCHEDULER_CBS_ERROR_FULL The server already has a task.
 */
int _Scheduler_CBS_Attach_thread (
  Scheduler_CBS_Server_id server_id,
  rtems_id                task_id
);

/**
 * @brief Detaches from the CBS Server.
 *
 * Detach from the CBS Server.
 *
 * @param server_id The id of the existing server.
 * @param task_id The id of the task to attach.
 *
 * @retval SCHEDULER_CBS_OK The operation was successful.
 * @retval SCHEDULER_CBS_ERROR_INVALID_PARAMETER The server id is to big,
 *      or the task with this id is not attached to this server or there is
 *      no thread with this task.
 * @retval SCHEDULER_CBS_ERROR_NOSERVER The server is not yet initialized.
 */
int _Scheduler_CBS_Detach_thread (
  Scheduler_CBS_Server_id server_id,
  rtems_id                task_id
);

/**
 * @brief Cleans up resources associated to the CBS Library.
 *
 * Cleanup resources associated to the CBS Library.
 *
 * @return This method always returns SCHEDULER_CBS_OK.
 */
int _Scheduler_CBS_Cleanup (void);

/**
 * @brief Creates a new server with specified parameters.
 *
 * Create a new server with specified parameters.
 *
 * @param params The parameters for the server.
 * @param budget_overrun_callback The budget overrun for the new server.
 * @param[out] server_id In the case of success, this parameter contains the
 *      id of the newly created server.
 *
 * @retval SCHEDULER_CBS_OK The operation succeeded.
 * @retval SCHEDULER_CBS_ERROR_INVALID_PARAMETER The given parameters are invalid.
 * @retval SCHEDULER_CBS_ERROR_FULL The maximum number of servers was already
 *      created, a new server cannot be created.
 */
int _Scheduler_CBS_Create_server (
  Scheduler_CBS_Parameters     *params,
  Scheduler_CBS_Budget_overrun  budget_overrun_callback,
  rtems_id                     *server_id
);

/**
 * @brief Detaches all tasks from a server and destroys it.
 *
 * Detach all tasks from a server and destroy it.
 *
 * @param server_id The id of the server to destroy.
 *
 * @retval SCHEDULER_CBS_OK The operation was successful.
 * @retval SCHEDULER_CBS_ERROR_INVALID_PARAMETER The server id is too big.
 * @retval SCHEDULER_CBS_ERROR_NOSERVER There is no initialized server with this id.
 */
int _Scheduler_CBS_Destroy_server (
  Scheduler_CBS_Server_id server_id
);

/**
 * @brief Retrieves the approved budget.
 *
 * Retrieve the budget that has been approved for the subsequent
 * server instances.
 *
 * @param server_id The id of the server instance of which the budget is wanted.
 * @param[out] approved_budget Contains the approved budget after a successful method call.
 *
 * @retval SCHEDULER_CBS_OK The operation was successful.
 * @retval SCHEDULER_CBS_ERROR_INVALID_PARAMETER The server id is too big.
 * @retval SCHEDULER_CBS_ERROR_NOSERVER There is no initialized server with this id.
 */
int _Scheduler_CBS_Get_approved_budget (
  Scheduler_CBS_Server_id  server_id,
  time_t                  *approved_budget
);

/**
 * @brief Retrieves remaining budget for the current server instance.
 *
 * Retrieve remaining budget for the current server instance.
 *
 * @param server_id The id of the server instance of which the remaining budget is wanted.
 * @param[out] remaining_budget Contains the remaining budget after a successful method call.
 *
 * @retval SCHEDULER_CBS_OK The operation was successful.
 * @retval SCHEDULER_CBS_ERROR_INVALID_PARAMETER The server id is too big.
 * @retval SCHEDULER_CBS_ERROR_NOSERVER There is no initialized server with this id.
 */
int _Scheduler_CBS_Get_remaining_budget (
  Scheduler_CBS_Server_id  server_id,
  time_t                  *remaining_budget
);

/**
 * @brief Gets relative time info.
 *
 * Retrieve time info relative to @a server_id. The server status code is returned.
 *
 * @param server_id is the server to get the status code from.
 * @param[out] exec_time Contains the execution time after a successful method call.
 * @param abs_time Not apparently used.
 *
 * @retval SCHEDULER_CBS_OK The operation was successful.
 * @retval SCHEDULER_CBS_ERROR_INVALID_PARAMETER The server id is too big.
 * @retval SCHEDULER_CBS_ERROR_NOSERVER There is no initialized server with this id.
 */
int _Scheduler_CBS_Get_execution_time (
  Scheduler_CBS_Server_id   server_id,
  time_t                   *exec_time,
  time_t                   *abs_time
);

/**
 * @brief Retrieves CBS scheduling parameters.
 *
 * Retrieve CBS scheduling parameters.
 *
 * @param server_id The id of the server to get the scheduling parameters from.
 * @param[out] params Will contain the scheduling parameters after successful method call.
 *
 * @retval SCHEDULER_CBS_OK The operation was successful.
 * @retval SCHEDULER_CBS_ERROR_INVALID_PARAMETER The server id is too big.
 * @retval SCHEDULER_CBS_ERROR_NOSERVER There is no initialized server with this id.
 */
int _Scheduler_CBS_Get_parameters (
  Scheduler_CBS_Server_id   server_id,
  Scheduler_CBS_Parameters *params
);

/**
 * @brief Gets a thread server id.
 *
 * Get a thread server id, or SCHEDULER_CBS_ERROR_NOT_FOUND if it is not
 * attached to any server.
 *
 * @param task_id The id of the task to get the corresponding server.
 * @param[out] server_id Will contain the server id after successful method call.
 * @retval SCHEDULER_CBS_OK The operation was successful
 * @retval SCHEDULER_CBS_ERROR_NOSERVER There is no server with this task attached.
 */
int _Scheduler_CBS_Get_server_id (
  rtems_id                 task_id,
  Scheduler_CBS_Server_id *server_id
);

/**
 * @brief Sets parameters for CBS scheduling.
 *
 * Change CBS scheduling parameters.
 *
 * @param server_id The id of the server.
 * @param parameters The parameters to set.
 *
 * @retval SCHEDULER_CBS_OK The operation was successful.
 * @retval SCHEDULER_CBS_ERROR_INVALID_PARAMETER The server id is too big or the
 *      given parameters are invalid.
 * @retval SCHEDULER_CBS_ERROR_NOSERVER There is no server with this id.
 */
int _Scheduler_CBS_Set_parameters (
  Scheduler_CBS_Server_id   server_id,
  Scheduler_CBS_Parameters *parameters
);

/**
 * @brief Invoked when a limited time quantum is exceeded.
 *
 * This routine is invoked when a limited time quantum is exceeded.
 *
 * @param the_thread The thread that exceeded a limited time quantum.
 */
void _Scheduler_CBS_Budget_callout(
  Thread_Control *the_thread
);

/**
 * @brief Initializes a CBS specific scheduler node of @a the_thread.
 *
 * @param scheduler The scheduler control for the operation.
 * @param[out] node The scheduler node to initalize.
 * @param the_thread The thread to initialize a scheduler node for.
 * @param priority The priority for the node.
 */
void _Scheduler_CBS_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node,
  Thread_Control          *the_thread,
  Priority_Control         priority
);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
/* end of include file */
