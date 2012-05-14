/*  coverhd.h
 *
 *  This include file has defines to represent the overhead associated
 *  with calling a particular directive from C.  These are used in the
 *  Timing Test Suite to ignore the overhead required to pass arguments
 *  to directives.  On some CPUs and/or target boards, this overhead
 *  is significant and makes it difficult to distinguish internal
 *  RTEMS execution time from that used to call the directive.
 *  This file should be updated after running the C overhead timing
 *  test.  Once this update has been performed, the RTEMS Time Test
 *  Suite should be rebuilt to account for these overhead times in the
 *  timing results.
 *
 *  NOTE:  If these are all zero, then the times reported include
 *         all calling overhead including passing of arguments.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __COVERHD_h
#define __COVERHD_h

#ifdef __cplusplus
extern "C" {
#endif

#define CALLING_OVERHEAD_INITIALIZE_EXECUTIVE      5
#define CALLING_OVERHEAD_SHUTDOWN_EXECUTIVE        4
#define CALLING_OVERHEAD_TASK_CREATE               6
#define CALLING_OVERHEAD_TASK_IDENT                4
#define CALLING_OVERHEAD_TASK_START                5
#define CALLING_OVERHEAD_TASK_RESTART              4
#define CALLING_OVERHEAD_TASK_DELETE               4
#define CALLING_OVERHEAD_TASK_SUSPEND              4
#define CALLING_OVERHEAD_TASK_RESUME               4
#define CALLING_OVERHEAD_TASK_SET_PRIORITY         5
#define CALLING_OVERHEAD_TASK_MODE                 4
#define CALLING_OVERHEAD_TASK_GET_NOTE             5
#define CALLING_OVERHEAD_TASK_SET_NOTE             5
#define CALLING_OVERHEAD_TASK_WAKE_WHEN            9
#define CALLING_OVERHEAD_TASK_WAKE_AFTER           4
#define CALLING_OVERHEAD_INTERRUPT_CATCH           5
#define CALLING_OVERHEAD_CLOCK_GET                 9
#define CALLING_OVERHEAD_CLOCK_SET                 8
#define CALLING_OVERHEAD_CLOCK_TICK                3

#define CALLING_OVERHEAD_TIMER_CREATE              4
#define CALLING_OVERHEAD_TIMER_IDENT               4
#define CALLING_OVERHEAD_TIMER_DELETE              4
#define CALLING_OVERHEAD_TIMER_FIRE_AFTER          6
#define CALLING_OVERHEAD_TIMER_FIRE_WHEN           10
#define CALLING_OVERHEAD_TIMER_RESET               4
#define CALLING_OVERHEAD_TIMER_CANCEL              4
#define CALLING_OVERHEAD_SEMAPHORE_CREATE          5
#define CALLING_OVERHEAD_SEMAPHORE_IDENT           4
#define CALLING_OVERHEAD_SEMAPHORE_DELETE          4
#define CALLING_OVERHEAD_SEMAPHORE_OBTAIN          5
#define CALLING_OVERHEAD_SEMAPHORE_RELEASE         4
#define CALLING_OVERHEAD_MESSAGE_QUEUE_CREATE      5
#define CALLING_OVERHEAD_MESSAGE_QUEUE_IDENT       4
#define CALLING_OVERHEAD_MESSAGE_QUEUE_DELETE      4
#define CALLING_OVERHEAD_MESSAGE_QUEUE_SEND        4
#define CALLING_OVERHEAD_MESSAGE_QUEUE_URGENT      4
#define CALLING_OVERHEAD_MESSAGE_QUEUE_BROADCAST   5
#define CALLING_OVERHEAD_MESSAGE_QUEUE_RECEIVE     5
#define CALLING_OVERHEAD_MESSAGE_QUEUE_FLUSH       4

#define CALLING_OVERHEAD_EVENT_SEND                4
#define CALLING_OVERHEAD_EVENT_RECEIVE             5
#define CALLING_OVERHEAD_SIGNAL_CATCH              5
#define CALLING_OVERHEAD_SIGNAL_SEND               4
#define CALLING_OVERHEAD_PARTITION_CREATE          6
#define CALLING_OVERHEAD_PARTITION_IDENT           4
#define CALLING_OVERHEAD_PARTITION_DELETE          4
#define CALLING_OVERHEAD_PARTITION_GET_BUFFER      4
#define CALLING_OVERHEAD_PARTITION_RETURN_BUFFER   4
#define CALLING_OVERHEAD_REGION_CREATE             6
#define CALLING_OVERHEAD_REGION_IDENT              4
#define CALLING_OVERHEAD_REGION_DELETE             4
#define CALLING_OVERHEAD_REGION_GET_SEGMENT        5
#define CALLING_OVERHEAD_REGION_RETURN_SEGMENT     4
#define CALLING_OVERHEAD_PORT_CREATE               6
#define CALLING_OVERHEAD_PORT_IDENT                4
#define CALLING_OVERHEAD_PORT_DELETE               4
#define CALLING_OVERHEAD_PORT_EXTERNAL_TO_INTERNAL 4
#define CALLING_OVERHEAD_PORT_INTERNAL_TO_EXTERNAL 4

#define CALLING_OVERHEAD_IO_INITIALIZE             5
#define CALLING_OVERHEAD_IO_OPEN                   5
#define CALLING_OVERHEAD_IO_CLOSE                  5
#define CALLING_OVERHEAD_IO_READ                   5
#define CALLING_OVERHEAD_IO_WRITE                  5
#define CALLING_OVERHEAD_IO_CONTROL                5
#define CALLING_OVERHEAD_FATAL_ERROR_OCCURRED      4
#define CALLING_OVERHEAD_RATE_MONOTONIC_CREATE     4
#define CALLING_OVERHEAD_RATE_MONOTONIC_IDENT      4
#define CALLING_OVERHEAD_RATE_MONOTONIC_DELETE     4
#define CALLING_OVERHEAD_RATE_MONOTONIC_CANCEL     4
#define CALLING_OVERHEAD_RATE_MONOTONIC_PERIOD     4
#define CALLING_OVERHEAD_MULTIPROCESSING_ANNOUNCE  3

#ifdef __cplusplus
}
#endif

#endif
