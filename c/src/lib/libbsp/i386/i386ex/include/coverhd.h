/* coverhd.h
 *
 *  This include file has defines to represent the overhead associated
 *  with calling a particular directive from C on this target.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __COVERHD_h
#define __COVERHD_h

#ifdef __cplusplus
extern "C" {
#endif

#define CALLING_OVERHEAD_INITIALIZE_EXECUTIVE      3
#define CALLING_OVERHEAD_SHUTDOWN_EXECUTIVE        3
#define CALLING_OVERHEAD_TASK_CREATE               4
#define CALLING_OVERHEAD_TASK_IDENT                4
#define CALLING_OVERHEAD_TASK_START                4
#define CALLING_OVERHEAD_TASK_RESTART              3
#define CALLING_OVERHEAD_TASK_DELETE               3
#define CALLING_OVERHEAD_TASK_SUSPEND              3
#define CALLING_OVERHEAD_TASK_RESUME               3
#define CALLING_OVERHEAD_TASK_SET_PRIORITY         4
#define CALLING_OVERHEAD_TASK_MODE                 4
#define CALLING_OVERHEAD_TASK_GET_NOTE             4
#define CALLING_OVERHEAD_TASK_SET_NOTE             4
#define CALLING_OVERHEAD_TASK_WAKE_WHEN            7
#define CALLING_OVERHEAD_TASK_WAKE_AFTER           3
#define CALLING_OVERHEAD_INTERRUPT_CATCH           4
#define CALLING_OVERHEAD_CLOCK_GET                 7
#define CALLING_OVERHEAD_CLOCK_SET                 7
#define CALLING_OVERHEAD_CLOCK_TICK                2

#define CALLING_OVERHEAD_TIMER_CREATE              3
#define CALLING_OVERHEAD_TIMER_IDENT               3
#define CALLING_OVERHEAD_TIMER_DELETE              3
#define CALLING_OVERHEAD_TIMER_FIRE_AFTER          4
#define CALLING_OVERHEAD_TIMER_FIRE_WHEN           8
#define CALLING_OVERHEAD_TIMER_RESET               3
#define CALLING_OVERHEAD_TIMER_CANCEL              3
#define CALLING_OVERHEAD_SEMAPHORE_CREATE          4
#define CALLING_OVERHEAD_SEMAPHORE_DELETE          3
#define CALLING_OVERHEAD_SEMAPHORE_IDENT           4
#define CALLING_OVERHEAD_SEMAPHORE_OBTAIN          4
#define CALLING_OVERHEAD_SEMAPHORE_RELEASE         3
#define CALLING_OVERHEAD_MESSAGE_QUEUE_CREATE      4
#define CALLING_OVERHEAD_MESSAGE_QUEUE_IDENT       4
#define CALLING_OVERHEAD_MESSAGE_QUEUE_DELETE      3
#define CALLING_OVERHEAD_MESSAGE_QUEUE_SEND        3
#define CALLING_OVERHEAD_MESSAGE_QUEUE_URGENT      3
#define CALLING_OVERHEAD_MESSAGE_QUEUE_BROADCAST   4
#define CALLING_OVERHEAD_MESSAGE_QUEUE_RECEIVE     4
#define CALLING_OVERHEAD_MESSAGE_QUEUE_FLUSH       3

#define CALLING_OVERHEAD_EVENT_SEND                4
#define CALLING_OVERHEAD_EVENT_RECEIVE             4
#define CALLING_OVERHEAD_SIGNAL_CATCH              3
#define CALLING_OVERHEAD_SIGNAL_SEND               3
#define CALLING_OVERHEAD_PARTITION_CREATE          4
#define CALLING_OVERHEAD_PARTITION_IDENT           4
#define CALLING_OVERHEAD_PARTITION_DELETE          3
#define CALLING_OVERHEAD_PARTITION_GET_BUFFER      4
#define CALLING_OVERHEAD_PARTITION_RETURN_BUFFER   4
#define CALLING_OVERHEAD_REGION_CREATE             4
#define CALLING_OVERHEAD_REGION_IDENT              3
#define CALLING_OVERHEAD_REGION_DELETE             3
#define CALLING_OVERHEAD_REGION_GET_SEGMENT        4
#define CALLING_OVERHEAD_REGION_RETURN_SEGMENT     4
#define CALLING_OVERHEAD_PORT_CREATE               4
#define CALLING_OVERHEAD_PORT_IDENT                3
#define CALLING_OVERHEAD_PORT_DELETE               3
#define CALLING_OVERHEAD_PORT_EXTERNAL_TO_INTERNAL 4
#define CALLING_OVERHEAD_PORT_INTERNAL_TO_EXTERNAL 4

#define CALLING_OVERHEAD_IO_INITIALIZE             4
#define CALLING_OVERHEAD_IO_OPEN                   4
#define CALLING_OVERHEAD_IO_CLOSE                  4
#define CALLING_OVERHEAD_IO_READ                   4
#define CALLING_OVERHEAD_IO_WRITE                  4
#define CALLING_OVERHEAD_IO_CONTROL                4
#define CALLING_OVERHEAD_FATAL_ERROR_OCCURRED      3
#define CALLING_OVERHEAD_RATE_MONOTONIC_CREATE     3
#define CALLING_OVERHEAD_RATE_MONOTONIC_IDENT      3
#define CALLING_OVERHEAD_RATE_MONOTONIC_DELETE     3
#define CALLING_OVERHEAD_RATE_MONOTONIC_CANCEL     3
#define CALLING_OVERHEAD_RATE_MONOTONIC_PERIOD     3
#define CALLING_OVERHEAD_MULTIPROCESSING_ANNOUNCE  2

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
