--
--  RTEMS_CALLING_OVERHEAD / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package contains the invocation overhead for each 
--  of the RTEMS directives on the MC68020 Timing Platform.
--  This time is then subtracted from the execution time
--  of each directive as measured by the Timing Suite.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989-1997.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

with RTEMS;

package RTEMS_CALLING_OVERHEAD is

   INITIALIZE_EXECUTIVE      : constant RTEMS.UNSIGNED32 := 0;
   SHUTDOWN_EXECUTIVE        : constant RTEMS.UNSIGNED32 := 0;
   TASK_CREATE               : constant RTEMS.UNSIGNED32 := 0;
   TASK_IDENT                : constant RTEMS.UNSIGNED32 := 0;
   TASK_START                : constant RTEMS.UNSIGNED32 := 0;
   TASK_RESTART              : constant RTEMS.UNSIGNED32 := 0;
   TASK_DELETE               : constant RTEMS.UNSIGNED32 := 0;
   TASK_SUSPEND              : constant RTEMS.UNSIGNED32 := 0;
   TASK_RESUME               : constant RTEMS.UNSIGNED32 := 0;
   TASK_SET_PRIORITY         : constant RTEMS.UNSIGNED32 := 0;
   TASK_MODE                 : constant RTEMS.UNSIGNED32 := 0;
   TASK_GET_NOTE             : constant RTEMS.UNSIGNED32 := 0;
   TASK_SET_NOTE             : constant RTEMS.UNSIGNED32 := 0;
   TASK_WAKE_WHEN            : constant RTEMS.UNSIGNED32 := 0;
   TASK_WAKE_AFTER           : constant RTEMS.UNSIGNED32 := 0;
   INTERRUPT_CATCH           : constant RTEMS.UNSIGNED32 := 0;
   CLOCK_GET                 : constant RTEMS.UNSIGNED32 := 0;
   CLOCK_SET                 : constant RTEMS.UNSIGNED32 := 0;
   CLOCK_TICK                : constant RTEMS.UNSIGNED32 := 0;

   TIMER_CREATE              : constant RTEMS.UNSIGNED32 := 0;
   TIMER_DELETE              : constant RTEMS.UNSIGNED32 := 0;
   TIMER_IDENT               : constant RTEMS.UNSIGNED32 := 0;
   TIMER_FIRE_AFTER          : constant RTEMS.UNSIGNED32 := 0;
   TIMER_FIRE_WHEN           : constant RTEMS.UNSIGNED32 := 0;
   TIMER_RESET               : constant RTEMS.UNSIGNED32 := 0;
   TIMER_CANCEL              : constant RTEMS.UNSIGNED32 := 0;
   SEMAPHORE_CREATE          : constant RTEMS.UNSIGNED32 := 0;
   SEMAPHORE_DELETE          : constant RTEMS.UNSIGNED32 := 0;
   SEMAPHORE_IDENT           : constant RTEMS.UNSIGNED32 := 0;
   SEMAPHORE_OBTAIN          : constant RTEMS.UNSIGNED32 := 0;
   SEMAPHORE_RELEASE         : constant RTEMS.UNSIGNED32 := 0;
   MESSAGE_QUEUE_CREATE      : constant RTEMS.UNSIGNED32 := 0;
   MESSAGE_QUEUE_IDENT       : constant RTEMS.UNSIGNED32 := 0;
   MESSAGE_QUEUE_DELETE      : constant RTEMS.UNSIGNED32 := 0;
   MESSAGE_QUEUE_SEND        : constant RTEMS.UNSIGNED32 := 0;
   MESSAGE_QUEUE_URGENT      : constant RTEMS.UNSIGNED32 := 0;
   MESSAGE_QUEUE_BROADCAST   : constant RTEMS.UNSIGNED32 := 0;
   MESSAGE_QUEUE_RECEIVE     : constant RTEMS.UNSIGNED32 := 0;
   MESSAGE_QUEUE_FLUSH       : constant RTEMS.UNSIGNED32 := 0;

   EVENT_SEND                : constant RTEMS.UNSIGNED32 := 0;
   EVENT_RECEIVE             : constant RTEMS.UNSIGNED32 := 0;
   SIGNAL_CATCH              : constant RTEMS.UNSIGNED32 := 0;
   SIGNAL_SEND               : constant RTEMS.UNSIGNED32 := 0;
   PARTITION_CREATE          : constant RTEMS.UNSIGNED32 := 0;
   PARTITION_IDENT           : constant RTEMS.UNSIGNED32 := 0;
   PARTITION_DELETE          : constant RTEMS.UNSIGNED32 := 0;
   PARTITION_GET_BUFFER      : constant RTEMS.UNSIGNED32 := 0;
   PARTITION_RETURN_BUFFER   : constant RTEMS.UNSIGNED32 := 0;
   REGION_CREATE             : constant RTEMS.UNSIGNED32 := 0;
   REGION_IDENT              : constant RTEMS.UNSIGNED32 := 0;
   REGION_DELETE             : constant RTEMS.UNSIGNED32 := 0;
   REGION_GET_SEGMENT        : constant RTEMS.UNSIGNED32 := 0;
   REGION_RETURN_SEGMENT     : constant RTEMS.UNSIGNED32 := 0;
   PORT_CREATE               : constant RTEMS.UNSIGNED32 := 0;
   PORT_IDENT                : constant RTEMS.UNSIGNED32 := 0;
   PORT_DELETE               : constant RTEMS.UNSIGNED32 := 0;
   PORT_EXTERNAL_TO_INTERNAL : constant RTEMS.UNSIGNED32 := 0;
   PORT_INTERNAL_TO_EXTERNAL : constant RTEMS.UNSIGNED32 := 0;

   IO_INITIALIZE             : constant RTEMS.UNSIGNED32 := 0;
   IO_OPEN                   : constant RTEMS.UNSIGNED32 := 0;
   IO_CLOSE                  : constant RTEMS.UNSIGNED32 := 0;
   IO_READ                   : constant RTEMS.UNSIGNED32 := 0;
   IO_WRITE                  : constant RTEMS.UNSIGNED32 := 0;
   IO_CONTROL                : constant RTEMS.UNSIGNED32 := 0;
   FATAL_ERROR_OCCURRED      : constant RTEMS.UNSIGNED32 := 0;
   RATE_MONOTONIC_CREATE     : constant RTEMS.UNSIGNED32 := 0;
   RATE_MONOTONIC_IDENT      : constant RTEMS.UNSIGNED32 := 0;
   RATE_MONOTONIC_DELETE     : constant RTEMS.UNSIGNED32 := 0;
   RATE_MONOTONIC_CANCEL     : constant RTEMS.UNSIGNED32 := 0;
   RATE_MONOTONIC_PERIOD     : constant RTEMS.UNSIGNED32 := 0;
   MULTIPROCESSING_ANNOUNCE  : constant RTEMS.UNSIGNED32 := 0;

end RTEMS_CALLING_OVERHEAD;
