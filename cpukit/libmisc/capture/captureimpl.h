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
#define RTEMS_CAPTURE_INIT           (1u << 0)
#define RTEMS_CAPTURE_ON             (1U << 1)
#define RTEMS_CAPTURE_NO_MEMORY      (1U << 2)
#define RTEMS_CAPTURE_TRIGGERED      (1U << 3)
#define RTEMS_CAPTURE_GLOBAL_WATCH   (1U << 4)
#define RTEMS_CAPTURE_ONLY_MONITOR   (1U << 5)

/*
 * Per-CPU capture flags.
 */
#define RTEMS_CAPTURE_OVERFLOW       (1U << 0)
#define RTEMS_CAPTURE_READER_ACTIVE  (1U << 1)
#define RTEMS_CAPTURE_READER_WAITING (1U << 2)

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
bool rtems_capture_trigger (rtems_tcb* ft,
                            rtems_tcb* tt,
                            uint32_t   events);

/**
 * @brief Capture append to record
 *
 * This function Capture appends data to a capture record.  It should
 * be called between rtems_capture_begin_add_record and
 * rtems_capture_end_add_record.
 *
 * @param[in] rec specifies the next location to write in the record
 * @param[in] data specifies the data to write
 * @param[in] size specifies specifies the size of the data
 *
 * @retval This method returns a pointer which is used as a marker
 * for the next location in the capture record. it should only be
 * used as input into rtems_capture_append_to_record or
 * rtems_capture_end_add_record.
 */
static void *rtems_capture_append_to_record(void*  rec,
                                     void*  data,
                                     size_t size );

/**
 * @brief Capture filter
 *
 * This function this function specifies if the given task
 * and events should be logged.
 *
 * @param[in] task specifies the capture task control block
 * @param[in] events specifies the events
 *
 * @retval This method returns true if this data should be
 * filtered from the log.  It returns false if this data
 * should be logged.
 */
bool rtems_capture_filter( rtems_tcb*            task,
                           uint32_t              events);
/**
 * @brief Capture begin add record.
 *
 * This function opens a record for writing and inserts
 * the header information
 *
 * @param[in] _task specifies the capture task block
 * @param[in] _events specifies the events
 * @param[in] _size specifies the expected size of the capture record
 * @param[out] _rec specifies the next write point in the capture record
 */
#define rtems_capture_begin_add_record( _task, _events, _size, _rec) \
  do { \
    rtems_interrupt_lock_context _lock_context; \
    *_rec = rtems_capture_record_open( _task, _events, _size, &_lock_context );

/**
 * @brief Capture append to record.
 *
 * This function appends data of a specifed size into a capture record.
 *
 * @param[in] rec specifies the next write point in the capture record
 * @param[in] data specifies the data to write
 * @param[in] size specifies the size of the data
 *
 * @retval This method returns the next write point in the capture record.
 */
static inline void *rtems_capture_append_to_record(void*  rec,
                                                   void*  data,
                                                   size_t size )
{
  uint8_t *ptr = rec;
  memcpy( ptr, data, size );
  return (ptr + size);
}

/**
 * @brief Capture end add record.
 *
 * This function completes the add capture record process
 *
 * @param[in] _rec specifies the end of the capture record
 */
#define rtems_capture_end_add_record( _rec ) \
    rtems_capture_record_close( _rec, &_lock_context ); \
  } while (0)

/**
 * @brief Capture returns the current time.
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

/**
 * @brief Capture record open.
 *
 * This function allocates a record and fills in the
 * header information.  It does a lock acquire
 * which will remain in effect until
 * rtems_capture_record_close is called.  This method
 * should only be used by rtems_capture_begin_add_record.
 *
 * @param[in] task specifies the caputre task block
 * @param[in] events specifies the events
 * @param[in] size specifies capture record size
 * @param[out] lock_context specifies the lock context
 *
 * @retval This method returns a pointer to the next location in
 * the capture record to store data.
 */
void* rtems_capture_record_open (rtems_tcb*                    task,
                                 uint32_t                      events,
                                 size_t                        size,
                                 rtems_interrupt_lock_context* lock_context);
/**
 * @brief Capture record close.
 *
 * This function closes writing to capure record and
 * releases the lock that was held on the record. This
 * method should only be used by rtems_capture_end_add_record.
 *
 * @param[in] rec specifies the record
 * @param[out] lock_context specifies the lock context
 */
void rtems_capture_record_close( void *rec, rtems_interrupt_lock_context* lock_context);


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
void rtems_capture_print_record_task(
  uint32_t                cpu,
  rtems_capture_record_t* rec
);

/**
 * @brief Capture print capture record.
 *
 * This function prints a user extension
 * capture record.
 *
 * @param[in] cpu specifies the cpu the cpu the record was logged on.
 * @param[in] rec specifies the record.
 * @param[in] diff specifies the time between this and the last capture record.
 */
void rtems_capture_print_record_capture(
  uint32_t                cpu,
  rtems_capture_record_t* rec,
  uint64_t                diff
);

/**
 * @brief Capture print watch list
 *
 * This function  prints a capture watch list
 */
void rtems_capture_print_watch_list( void );

#ifdef __cplusplus
}
#endif
/**@}*/

#endif
