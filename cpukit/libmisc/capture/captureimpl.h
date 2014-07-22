/**
 * @file rtems/captureimpl.h
 *
 * @brief Capture Implementation file 
 *
 * This file contains an interface between the capture engine and
 * capture user extension methods.
 */

/*
  ------------------------------------------------------------------------

  Copyright Objective Design Systems Pty Ltd, 2002
  All rights reserved Objective Design Systems Pty Ltd, 2002
  Chris Johns (ccj@acm.org)

  COPYRIGHT (c) 1989-2014.
  On-Line Applications Research Corporation (OAR).

  The license and distribution terms for this file may be
  found in the file LICENSE in this distribution.

  This software with is provided ``as is'' and with NO WARRANTY.

  ------------------------------------------------------------------------

  RTEMS Performance Monitoring and Measurement Framework.
  This is the Capture Engine component.

*/

#ifndef __CAPTUREIMPL_H_
#define __CAPTUREIMPL_H_


/**@{*/
#ifdef __cplusplus
extern "C" {
#endif

#include "capture.h"

/*
 * Global capture flags.
 */
#define RTEMS_CAPTURE_ON             (1U << 0)
#define RTEMS_CAPTURE_NO_MEMORY      (1U << 1)
#define RTEMS_CAPTURE_OVERFLOW       (1U << 2)
#define RTEMS_CAPTURE_TRIGGERED      (1U << 3)
#define RTEMS_CAPTURE_READER_ACTIVE  (1U << 4)
#define RTEMS_CAPTURE_READER_WAITING (1U << 5)
#define RTEMS_CAPTURE_GLOBAL_WATCH   (1U << 6)
#define RTEMS_CAPTURE_ONLY_MONITOR   (1U << 7)

/**
 * @brief Capture set extension index.
 *
 * This function is used to set the extension index
 * for the capture engine.  
 *
 * @param[in] index specifies the extension index to be
 * used for capture engine data.
 */
void rtems_capture_set_extension_index(int index);

/**
 * @brief Capture get extension index.
 *
 * This function rturns the extension index for the
 * capture engine.
 *
 * @retval This method returns the extension index.
 */
int  rtems_capture_get_extension_index(void);

/**
 * @brief Capture get flags.
 *
 * This function gets the current flag settings
 * for the capture engine.
 *
 * @retval This method returns the global capture
 * flags.
 * 
 */
uint32_t rtems_capture_get_flags(void);

/**
 * @brief Capture set flags.
 *
 * This function sets a flag in the capture engine
 *
 * @param[in] mask specifies the flag to set
 */
void rtems_capture_set_flags(uint32_t mask);

/**
 * @brief Capture user extension open.
 *
 * This function creates the capture user extensions.
 *
 *
 * @retval This method returns RTEMS_SUCCESSFUL upon successful
 * creation of the user extensions.
 */
rtems_status_code rtems_capture_user_extension_open(void);

/**
 * @brief Capture user extension close.
 *
 * This function closes the capture user extensions.
 *
 * @retval This method returns RTEMS_SUCCESSFUL upon a successful
 * delete of the user extensions.
 */
rtems_status_code rtems_capture_user_extension_close(void);

/**
 * @brief Capture find capture task.
 *
 * This function finds the capture task control block
 *
 * @param[in] ct_id specifies the task_id
 *
 * @retval This method returns the capture task control block associated
 * with the given task id.
 */
rtems_capture_task_t* rtems_capture_find_capture_task( rtems_id ct_id );

/**
 * @brief Capture create capture task control block.
 *
 * This function create the capture task control block
 *
 * @param[in] new_task specifies the rtems thread control block
 *
 * @retval This method returns a capture task control block.
 */
rtems_capture_task_t* rtems_capture_create_capture_task (rtems_tcb* new_task);

/**
 * @brief Capture trigger.
 *
 * This function checks if we have triggered or if this event is a
 * cause of a trigger.
 *
 * @param[in] ft specifies specifices the capture from task
 * @param[in] tt specifies specifices the capture to task
 * @param[in] events specifies the events
 *
 * @retval This method returns true if we have triggered or
 * if the event is a cause of a trigger.
 */
bool rtems_capture_trigger (rtems_capture_task_t* ft,
                       rtems_capture_task_t* tt,
                       uint32_t              events);
/**
 * @brief Capture initialize stack usage
 *
 * This function setups a stack so its usage can be monitored.
 *
 * @param[in] task specifies the capture task block
 */
void rtems_capture_init_stack_usage (rtems_capture_task_t* task);

/**
 * @brief Capture destroy task.
 *
 * This function destroy the task structure if the reference count
 * is 0 and the tcb has been cleared signalling the task has been
 * deleted.
 *
 * @param[in] task specifies the capture task block
 */
void rtems_capture_destroy_capture_task (rtems_capture_task_t* task);

/**
 * @brief .
 *
 * This function returns the current time. If a handler is provided
 * by the user the time is gotten from that.
 *
 * @param[in] time specifies the capture time
 *
 * @retval This method returns a nano-second time if no user handler
 * is provided.  Otherwise, it returns a resolution defined by the handler.
 */
void rtems_capture_get_time (rtems_capture_time_t* time);


#ifdef __cplusplus
}
#endif

#endif
