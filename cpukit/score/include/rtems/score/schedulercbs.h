/**
 *  @file  rtems/score/schedulercbs.h
 *
 *  @brief Thread manipulation for the CBS scheduler
 *
 *  This include file contains all the constants and structures associated
 *  with the manipulation of threads for the CBS scheduler.
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
 *  @defgroup ScoreSchedulerCBS CBS Scheduler
 *
 *  @ingroup ScoreScheduler
 */
/**@{*/

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
    _Scheduler_EDF_Change_priority,  /* change priority entry point */ \
    SCHEDULER_OPERATION_DEFAULT_ASK_FOR_HELP \
    _Scheduler_CBS_Node_initialize,  /* node initialize entry point */ \
    _Scheduler_default_Node_destroy, /* node destroy entry point */ \
    _Scheduler_EDF_Update_priority,  /* update priority entry point */ \
    _Scheduler_EDF_Priority_compare, /* compares two priorities */ \
    _Scheduler_CBS_Release_job,      /* new period of task */ \
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
} Scheduler_CBS_Node;


/**
 * List of servers. The @a Scheduler_CBS_Server is the index to the array
 * of pointers to @a _Scheduler_CBS_Server_list.
 */
extern Scheduler_CBS_Server _Scheduler_CBS_Server_list[];

/**
 *  @brief Unblocks a thread from the queue.
 *
 *  This routine adds @a the_thread to the scheduling decision, that is,
 *  adds it to the ready queue and updates any appropriate scheduling
 *  variables, for example the heir thread. It is checked whether the
 *  remaining budget is sufficient. If not, the thread continues as a
 *  new job in order to protect concurrent threads.
 *
 *  @param[in] scheduler The scheduler instance.
 *  @param[in] the_thread will be unblocked.
 *
 *  @note This has to be asessed as missed deadline of the current job.
 */
Scheduler_Void_or_thread _Scheduler_CBS_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

/**
 *  @brief Called when a new job of task is released.
 *
 *  This routine is called when a new job of task is released.
 *  It is called only from Rate Monotonic manager in the beginning
 *  of new period. Deadline has to be shifted and budget replenished.
 *
 *  @param[in] scheduler The scheduler instance.
 *  @param[in] the_thread is the owner of the job.
 *  @param[in] length of the new job from now. If equal to 0,
 *             the job was cancelled or deleted.
 */

void _Scheduler_CBS_Release_job (
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  uint32_t                 length
);

/**
 *  @brief _Scheduler_CBS_Initialize
 *
 *  Initializes the CBS library.
 *
 *  @retval status code.
 */
int _Scheduler_CBS_Initialize(void);

/**
 *  @brief Attach a task to an already existing server.
 *
 *  Attach a task to an already existing server.
 *
 *  @retval status code.
 */
int _Scheduler_CBS_Attach_thread (
  Scheduler_CBS_Server_id server_id,
  rtems_id                task_id
);

/**
 *  @brief Detach from the CBS Server.
 *
 *  Detach from the CBS Server.
 *
 *  @retval status code.
 */
int _Scheduler_CBS_Detach_thread (
  Scheduler_CBS_Server_id server_id,
  rtems_id                task_id
);

/**
 *  @brief Cleanup resources associated to the CBS Library.
 *
 *  Cleanup resources associated to the CBS Library.
 *
 *  @retval status code.
 */
int _Scheduler_CBS_Cleanup (void);

/**
 *  @brief Create a new server with specified parameters.
 *
 *  Create a new server with specified parameters.
 *
 *  @retval status code.
 */
int _Scheduler_CBS_Create_server (
  Scheduler_CBS_Parameters     *params,
  Scheduler_CBS_Budget_overrun  budget_overrun_callback,
  rtems_id                     *server_id
);

/**
 *  @brief Detach all tasks from a server and destroy it.
 *
 *  Detach all tasks from a server and destroy it.
 *
 *  @param[in] server_id is the ID of the server
 *
 *  @retval status code.
 */
int _Scheduler_CBS_Destroy_server (
  Scheduler_CBS_Server_id server_id
);

/**
 *  @brief Retrieve the approved budget.
 *
 *  Retrieve the budget that has been approved for the subsequent
 *  server instances.
 *
 *  @retval status code.
 */
int _Scheduler_CBS_Get_approved_budget (
  Scheduler_CBS_Server_id  server_id,
  time_t                  *approved_budget
);

/**
 *  @brief Retrieve remaining budget for the current server instance.
 *
 *  Retrieve remaining budget for the current server instance.
 *
 *  @retval status code.
 */
int _Scheduler_CBS_Get_remaining_budget (
  Scheduler_CBS_Server_id  server_id,
  time_t                  *remaining_budget
);

/**
 *  @brief Get relative time info.
 *
 *  Retrieve time info relative to @a server_id. The server status code is returned.
 *
 *  @param[in] server_id is the server to get the status code from.
 *  @param[in] exec_time is the execution time.
 *  @param[in] abs_time is not apparently used.
 *
 *  @retval status code.
 */
int _Scheduler_CBS_Get_execution_time (
  Scheduler_CBS_Server_id   server_id,
  time_t                   *exec_time,
  time_t                   *abs_time
);

/**
 *  @brief Retrieve CBS scheduling parameters.
 *
 *  Retrieve CBS scheduling parameters.
 *
 *  @retval status code.
 */
int _Scheduler_CBS_Get_parameters (
  Scheduler_CBS_Server_id   server_id,
  Scheduler_CBS_Parameters *params
);

/**
 *  @brief Get a thread server id.
 *
 *  Get a thread server id, or SCHEDULER_CBS_ERROR_NOT_FOUND if it is not
 *  attached to any server.
 *
 *  @retval status code.
 */
int _Scheduler_CBS_Get_server_id (
  rtems_id                 task_id,
  Scheduler_CBS_Server_id *server_id
);

/**
 *  @brief Set parameters for CBS scheduling.
 *
 *  Change CBS scheduling parameters.
 *
 *  @param[in] server_id is the ID of the server.
 *  @param[in] parameters are the parameters to set.
 *
 *  @retval status code.
 */
int _Scheduler_CBS_Set_parameters (
  Scheduler_CBS_Server_id   server_id,
  Scheduler_CBS_Parameters *parameters
);

/**
 *  @brief Invoked when a limited time quantum is exceeded.
 *
 *  This routine is invoked when a limited time quantum is exceeded.
 */
void _Scheduler_CBS_Budget_callout(
  Thread_Control *the_thread
);

/**
 *  @brief Initializes a CBS specific scheduler node of @a the_thread.
 */
void _Scheduler_CBS_Node_initialize(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
