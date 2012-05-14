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
 *  NOTE:  If these are all zero, then the times reported include all
 *         all calling overhead including passing of arguments.
 *
 *
 * These are the figures tmoverhd.exe reported with gcc-2.95.1 -O4
 * on a Hitachi SH7045F Evaluation Board with SH7045F at 29 MHz
 *
 * These results are assumed to be applicable to most SH7045/29MHz boards
 *
 *  Author: John M.Mills (jmills@tga.com)
 *
 *  COPYRIGHT (c) 1999. TGA Technologies, Inc., Norcross, GA, USA
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  This file may be copied and distributed in accordance
 *  the above-referenced license. It is provided for critique and
 *  developmental purposes without any warranty nor representation
 *  by the authors or by TGA Technologies.
 */

#ifndef __COVERHD_h
#define __COVERHD_h

#ifdef __cplusplus
extern "C" {
#endif

#define CALLING_OVERHEAD_INITIALIZE_EXECUTIVE      1
#define CALLING_OVERHEAD_SHUTDOWN_EXECUTIVE        1
#define CALLING_OVERHEAD_TASK_CREATE               2
#define CALLING_OVERHEAD_TASK_IDENT                1
#define CALLING_OVERHEAD_TASK_START                1
#define CALLING_OVERHEAD_TASK_RESTART              1
#define CALLING_OVERHEAD_TASK_DELETE               1
#define CALLING_OVERHEAD_TASK_SUSPEND              1
#define CALLING_OVERHEAD_TASK_RESUME               1
#define CALLING_OVERHEAD_TASK_SET_PRIORITY         1
#define CALLING_OVERHEAD_TASK_MODE                 1
#define CALLING_OVERHEAD_TASK_GET_NOTE             1
#define CALLING_OVERHEAD_TASK_SET_NOTE             1
#define CALLING_OVERHEAD_TASK_WAKE_WHEN            2
#define CALLING_OVERHEAD_TASK_WAKE_AFTER           1
#define CALLING_OVERHEAD_INTERRUPT_CATCH           1
#define CALLING_OVERHEAD_CLOCK_GET                 3
#define CALLING_OVERHEAD_CLOCK_SET                 2
#define CALLING_OVERHEAD_CLOCK_TICK                1

#define CALLING_OVERHEAD_TIMER_CREATE              1
#define CALLING_OVERHEAD_TIMER_IDENT               1
#define CALLING_OVERHEAD_TIMER_DELETE              1
#define CALLING_OVERHEAD_TIMER_FIRE_AFTER          1
#define CALLING_OVERHEAD_TIMER_FIRE_WHEN           3
#define CALLING_OVERHEAD_TIMER_RESET               1
#define CALLING_OVERHEAD_TIMER_CANCEL              1
#define CALLING_OVERHEAD_SEMAPHORE_CREATE          1
#define CALLING_OVERHEAD_SEMAPHORE_IDENT           1
#define CALLING_OVERHEAD_SEMAPHORE_DELETE          1
#define CALLING_OVERHEAD_SEMAPHORE_OBTAIN          1
#define CALLING_OVERHEAD_SEMAPHORE_RELEASE         1
#define CALLING_OVERHEAD_MESSAGE_QUEUE_CREATE      1
#define CALLING_OVERHEAD_MESSAGE_QUEUE_IDENT       1
#define CALLING_OVERHEAD_MESSAGE_QUEUE_DELETE      1
#define CALLING_OVERHEAD_MESSAGE_QUEUE_SEND        1
#define CALLING_OVERHEAD_MESSAGE_QUEUE_URGENT      1
#define CALLING_OVERHEAD_MESSAGE_QUEUE_BROADCAST   1
#define CALLING_OVERHEAD_MESSAGE_QUEUE_RECEIVE     1
#define CALLING_OVERHEAD_MESSAGE_QUEUE_FLUSH       1

#define CALLING_OVERHEAD_EVENT_SEND                1
#define CALLING_OVERHEAD_EVENT_RECEIVE             1
#define CALLING_OVERHEAD_SIGNAL_CATCH              1
#define CALLING_OVERHEAD_SIGNAL_SEND               1
#define CALLING_OVERHEAD_PARTITION_CREATE          2
#define CALLING_OVERHEAD_PARTITION_IDENT           1
#define CALLING_OVERHEAD_PARTITION_DELETE          1
#define CALLING_OVERHEAD_PARTITION_GET_BUFFER      1
#define CALLING_OVERHEAD_PARTITION_RETURN_BUFFER   1
#define CALLING_OVERHEAD_REGION_CREATE             2
#define CALLING_OVERHEAD_REGION_IDENT              1
#define CALLING_OVERHEAD_REGION_DELETE             1
#define CALLING_OVERHEAD_REGION_GET_SEGMENT        2
#define CALLING_OVERHEAD_REGION_RETURN_SEGMENT     1
#define CALLING_OVERHEAD_PORT_CREATE               2
#define CALLING_OVERHEAD_PORT_IDENT                1
#define CALLING_OVERHEAD_PORT_DELETE               1
#define CALLING_OVERHEAD_PORT_EXTERNAL_TO_INTERNAL 1
#define CALLING_OVERHEAD_PORT_INTERNAL_TO_EXTERNAL 1

#define CALLING_OVERHEAD_IO_INITIALIZE             1
#define CALLING_OVERHEAD_IO_OPEN                   1
#define CALLING_OVERHEAD_IO_CLOSE                  1
#define CALLING_OVERHEAD_IO_READ                   1
#define CALLING_OVERHEAD_IO_WRITE                  1
#define CALLING_OVERHEAD_IO_CONTROL                1
#define CALLING_OVERHEAD_FATAL_ERROR_OCCURRED      1
#define CALLING_OVERHEAD_RATE_MONOTONIC_CREATE     1
#define CALLING_OVERHEAD_RATE_MONOTONIC_IDENT      1
#define CALLING_OVERHEAD_RATE_MONOTONIC_DELETE     1
#define CALLING_OVERHEAD_RATE_MONOTONIC_CANCEL     1
#define CALLING_OVERHEAD_RATE_MONOTONIC_PERIOD     1
#define CALLING_OVERHEAD_MULTIPROCESSING_ANNOUNCE  1

#ifdef __cplusplus
}
#endif

#endif
