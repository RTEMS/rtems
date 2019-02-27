/**
 * @file
 *
 * @brief Capture Implementation file
 *
 * This file contains an interface between the capture engine and
 * capture user extension methods.
 */

/*
  ------------------------------------------------------------------------

  Copyright 2002, 2016 Chris Johns <chrisj@rtems.org>.
  All rights reserved.

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

#include "capture.h"

/**@{*/
#ifdef __cplusplus
extern "C" {
#endif

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
 * @brief Capture check trigger.
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
bool rtems_capture_trigger_fired (rtems_tcb* ft,
                                  rtems_tcb* tt,
                                  uint32_t   events);

/**
 * @brief Capture print trace records.
 *
 * This function reads, prints and releases up to
 * total trace records in either a csv format or an
 * ascii table format.
 *
 * @param[in] total specifies the number of records to print
 * @param[in] csv specifies a comma seperated value format
 */
void rtems_capture_print_trace_records ( int total, bool csv );

/**
 * @brief Capture print timestamp.
 *
 * This function prints uptime in a timestamp format.
 *
 * @param[in] uptime specifies the timestamp to print
 */
void rtems_capture_print_timestamp (uint64_t uptime);

/**
 * @brief Capture print record task.
 *
 * This function  prints a capture record task.  This
 * record contains information to identify a task.  It
 * is refrenced in other records by the task id.
 *
 * @param[in] cpu specifies the cpu the cpu the record was logged on.
 * @param[in] rec specifies the task record.
 */
void rtems_capture_print_record_task(int                              cpu,
                                     const rtems_capture_record*      rec,
                                     const rtems_capture_task_record* task_rec);

/**
 * @brief Capture print capture record.
 *
 * This function prints a user extension
 * capture record.
 *
 * @param[in] cpu specifies the cpu the cpu the record was logged on.
 * @param[in] rec specifies the record.
 * @param[in] diff specifies the time between this and the last capture record.
 * @param[in] name specifies the name of the task, NULL if none.
 * @param[in] task_count number of tasks to search for.
 */
void rtems_capture_print_record_capture(int                         cpu,
                                        const rtems_capture_record* rec,
                                        uint64_t                    diff,
                                        const rtems_name*           name);

/**
 * @brief Capture print watch list
 *
 * This function  prints a capture watch list
 */
void rtems_capture_print_watch_list (void);

#ifdef __cplusplus
}
#endif
/**@}*/

#endif
