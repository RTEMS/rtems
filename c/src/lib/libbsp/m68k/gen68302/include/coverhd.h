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

#define CALLING_OVERHEAD_INITIALIZE_EXECUTIVE      14
#define CALLING_OVERHEAD_SHUTDOWN_EXECUTIVE        11
#define CALLING_OVERHEAD_TASK_CREATE               22
#define CALLING_OVERHEAD_TASK_IDENT                17
#define CALLING_OVERHEAD_TASK_START                18
#define CALLING_OVERHEAD_TASK_RESTART              15
#define CALLING_OVERHEAD_TASK_DELETE               12
#define CALLING_OVERHEAD_TASK_SUSPEND              12
#define CALLING_OVERHEAD_TASK_RESUME               12
#define CALLING_OVERHEAD_TASK_SET_PRIORITY         16
#define CALLING_OVERHEAD_TASK_MODE                 15
#define CALLING_OVERHEAD_TASK_GET_NOTE             16
#define CALLING_OVERHEAD_TASK_SET_NOTE             16
#define CALLING_OVERHEAD_TASK_WAKE_WHEN            31
#define CALLING_OVERHEAD_TASK_WAKE_AFTER           11
#define CALLING_OVERHEAD_INTERRUPT_CATCH           17
#define CALLING_OVERHEAD_CLOCK_GET                 32
#define CALLING_OVERHEAD_CLOCK_SET                 31
#define CALLING_OVERHEAD_CLOCK_TICK                8

#define CALLING_OVERHEAD_TIMER_CREATE              13
#define CALLING_OVERHEAD_TIMER_IDENT               12
#define CALLING_OVERHEAD_TIMER_DELETE              14
#define CALLING_OVERHEAD_TIMER_FIRE_AFTER          19
#define CALLING_OVERHEAD_TIMER_FIRE_WHEN           39
#define CALLING_OVERHEAD_TIMER_RESET               12
#define CALLING_OVERHEAD_TIMER_CANCEL              12
#define CALLING_OVERHEAD_SEMAPHORE_CREATE          18
#define CALLING_OVERHEAD_SEMAPHORE_IDENT           12
#define CALLING_OVERHEAD_SEMAPHORE_DELETE          17
#define CALLING_OVERHEAD_SEMAPHORE_OBTAIN          17
#define CALLING_OVERHEAD_SEMAPHORE_RELEASE         12
#define CALLING_OVERHEAD_MESSAGE_QUEUE_CREATE      18
#define CALLING_OVERHEAD_MESSAGE_QUEUE_IDENT       17
#define CALLING_OVERHEAD_MESSAGE_QUEUE_DELETE      12
#define CALLING_OVERHEAD_MESSAGE_QUEUE_SEND        14
#define CALLING_OVERHEAD_MESSAGE_QUEUE_URGENT      14
#define CALLING_OVERHEAD_MESSAGE_QUEUE_BROADCAST   17
#define CALLING_OVERHEAD_MESSAGE_QUEUE_RECEIVE     19
#define CALLING_OVERHEAD_MESSAGE_QUEUE_FLUSH       14

#define CALLING_OVERHEAD_EVENT_SEND                15
#define CALLING_OVERHEAD_EVENT_RECEIVE             18
#define CALLING_OVERHEAD_SIGNAL_CATCH              14
#define CALLING_OVERHEAD_SIGNAL_SEND               14
#define CALLING_OVERHEAD_PARTITION_CREATE          23
#define CALLING_OVERHEAD_PARTITION_IDENT           17
#define CALLING_OVERHEAD_PARTITION_DELETE          12
#define CALLING_OVERHEAD_PARTITION_GET_BUFFER      15
#define CALLING_OVERHEAD_PARTITION_RETURN_BUFFER   15
#define CALLING_OVERHEAD_REGION_CREATE             23
#define CALLING_OVERHEAD_REGION_IDENT              14
#define CALLING_OVERHEAD_REGION_DELETE             12
#define CALLING_OVERHEAD_REGION_GET_SEGMENT        21
#define CALLING_OVERHEAD_REGION_RETURN_SEGMENT     15
#define CALLING_OVERHEAD_PORT_CREATE               20
#define CALLING_OVERHEAD_PORT_IDENT                14
#define CALLING_OVERHEAD_PORT_DELETE               12
#define CALLING_OVERHEAD_PORT_EXTERNAL_TO_INTERNAL 18
#define CALLING_OVERHEAD_PORT_INTERNAL_TO_EXTERNAL 18

#define CALLING_OVERHEAD_IO_INITIALIZE             18
#define CALLING_OVERHEAD_IO_OPEN                   18
#define CALLING_OVERHEAD_IO_CLOSE                  18
#define CALLING_OVERHEAD_IO_READ                   18
#define CALLING_OVERHEAD_IO_WRITE                  18
#define CALLING_OVERHEAD_IO_CONTROL                18
#define CALLING_OVERHEAD_FATAL_ERROR_OCCURRED      11
#define CALLING_OVERHEAD_RATE_MONOTONIC_CREATE     13
#define CALLING_OVERHEAD_RATE_MONOTONIC_IDENT      14
#define CALLING_OVERHEAD_RATE_MONOTONIC_DELETE     12
#define CALLING_OVERHEAD_RATE_MONOTONIC_CANCEL     12
#define CALLING_OVERHEAD_RATE_MONOTONIC_PERIOD     14
#define CALLING_OVERHEAD_MULTIPROCESSING_ANNOUNCE  8

#ifdef __cplusplus
}
#endif

#endif
