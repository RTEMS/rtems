--
--  DUMMY_RTEMS / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package contains specifications for stub routines
--  which are used to time the invocation overhead incurred
--  with an Ada application program invokes each RTEMS directive.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989-2007.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

with RTEMS;
with RTEMS.CLOCK;
with RTEMS.TASKS;
with RTEMS.TIMER;

package DUMMY_RTEMS is

-- Task Manager

   procedure TASK_CREATE (
      NAME             : in     RTEMS.NAME;
      INITIAL_PRIORITY : in     RTEMS.TASKS.PRIORITY;
      STACK_SIZE       : in     RTEMS.UNSIGNED32;
      INITIAL_MODES    : in     RTEMS.MODE;
      ATTRIBUTE_SET    : in     RTEMS.ATTRIBUTE;
      ID               :    out RTEMS.ID;
      RESULT           :    out RTEMS.STATUS_CODES
   );

   procedure TASK_IDENT (
      NAME   : in     RTEMS.NAME;
      NODE   : in     RTEMS.UNSIGNED32;
      ID     :    out RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure TASK_START (
      ID          : in     RTEMS.ID;
      ENTRY_POINT : in     RTEMS.TASKS.ENTRY_POINT;
      ARGUMENT    : in     RTEMS.UNSIGNED32;
      RESULT      :    out RTEMS.STATUS_CODES
   );

   procedure TASK_RESTART (
      ID       : in     RTEMS.ID;
      ARGUMENT : in     RTEMS.UNSIGNED32;
      RESULT   :    out RTEMS.STATUS_CODES
   );

   procedure TASK_DELETE (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure TASK_SUSPEND (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure TASK_RESUME (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure TASK_SET_PRIORITY (
      ID           : in     RTEMS.ID;
      NEW_PRIORITY : in     RTEMS.TASKS.PRIORITY;
      OLD_PRIORITY :    out RTEMS.TASKS.PRIORITY;
      RESULT       :    out RTEMS.STATUS_CODES
   );

   procedure TASK_MODE (
      MODE_SET          : in     RTEMS.MODE;
      MASK              : in     RTEMS.MODE;
      PREVIOUS_MODE_SET :    out RTEMS.MODE;
      RESULT            :    out RTEMS.STATUS_CODES
   );

   procedure TASK_GET_NOTE (
      ID      : in     RTEMS.ID;
      NOTEPAD : in     RTEMS.NOTEPAD_INDEX;
      NOTE    :    out RTEMS.UNSIGNED32;
      RESULT  :    out RTEMS.STATUS_CODES
   );

   procedure TASK_SET_NOTE (
      ID      : in     RTEMS.ID;
      NOTEPAD : in     RTEMS.NOTEPAD_INDEX;
      NOTE    : in     RTEMS.UNSIGNED32;
      RESULT  :    out RTEMS.STATUS_CODES
   );

   procedure TASK_WAKE_WHEN (
      TIME_BUFFER : in     RTEMS.TIME_OF_DAY;
      RESULT      :    out RTEMS.STATUS_CODES
   );

   procedure TASK_WAKE_AFTER (
      TICKS  : in     RTEMS.INTERVAL;
      RESULT :    out RTEMS.STATUS_CODES
   );

-- Interrupt Manager

--   procedure INTERRUPT_CATCH (
--      NEW_ISR_HANDLER : in     RTEMS.ADDRESS;
--      VECTOR          : in     RTEMS.VECTOR_NUMBER;
--      OLD_ISR_HANDLER :    out RTEMS.ADDRESS;
--      RESULT          :    out RTEMS.STATUS_CODES
--   );

-- Clock Manager

   procedure CLOCK_GET (
      OPTION      : in     RTEMS.CLOCK.GET_OPTIONS;
      TIME_BUFFER : in     RTEMS.ADDRESS;
      RESULT      :    out RTEMS.STATUS_CODES
   );

   procedure CLOCK_SET (
      TIME_BUFFER : in     RTEMS.TIME_OF_DAY;
      RESULT      :    out RTEMS.STATUS_CODES
   );

   procedure CLOCK_TICK (
      RESULT :    out RTEMS.STATUS_CODES
   );

-- Timer Manager

   procedure TIMER_CREATE (
      NAME   : in     RTEMS.NAME;
      ID     :    out RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure TIMER_DELETE (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure TIMER_IDENT (
      NAME   : in     RTEMS.NAME;
      ID     :    out RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure TIMER_FIRE_AFTER (
      ID        : in     RTEMS.ID;
      TICKS     : in     RTEMS.INTERVAL;
      ROUTINE   : in     RTEMS.TIMER.SERVICE_ROUTINE;
      USER_DATA : in     RTEMS.ADDRESS;
      RESULT    :    out RTEMS.STATUS_CODES
   );

   procedure TIMER_FIRE_WHEN (
      ID        : in     RTEMS.ID;
      WALL_TIME : in     RTEMS.TIME_OF_DAY;
      ROUTINE   : in     RTEMS.TIMER.SERVICE_ROUTINE;
      USER_DATA : in     RTEMS.ADDRESS;
      RESULT    :    out RTEMS.STATUS_CODES
   );

   procedure TIMER_RESET (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure TIMER_CANCEL (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

-- Semaphore Manager

   procedure SEMAPHORE_CREATE (
      NAME             : in     RTEMS.NAME;
      COUNT            : in     RTEMS.UNSIGNED32;
      ATTRIBUTE_SET    : in     RTEMS.ATTRIBUTE;
      PRIORITY_CEILING : in     RTEMS.TASKS.PRIORITY;
      ID               :    out RTEMS.ID;
      RESULT           :    out RTEMS.STATUS_CODES
   );

   procedure SEMAPHORE_DELETE (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure SEMAPHORE_IDENT (
      NAME   : in     RTEMS.NAME;
      NODE   : in     RTEMS.UNSIGNED32;
      ID     :    out RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure SEMAPHORE_OBTAIN (
      ID         : in     RTEMS.ID;
      OPTION_SET : in     RTEMS.OPTION;
      TIMEOUT    : in     RTEMS.INTERVAL;
      RESULT     :    out RTEMS.STATUS_CODES
   );

   procedure SEMAPHORE_RELEASE (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

-- Message Queue Manager

   procedure MESSAGE_QUEUE_CREATE (
      Name             : in     RTEMS.Name;
      Count            : in     RTEMS.Unsigned32;
      Max_Message_Size : in     RTEMS.Unsigned32;
      Attribute_Set    : in     RTEMS.Attribute;
      ID               :    out RTEMS.ID;
      Result           :    out RTEMS.Status_Codes
   );

   procedure MESSAGE_QUEUE_IDENT (
      NAME   : in     RTEMS.NAME;
      NODE   : in     RTEMS.UNSIGNED32;
      ID     :    out RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure MESSAGE_QUEUE_DELETE (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure MESSAGE_QUEUE_SEND (
      ID     : in     RTEMS.ID;
      BUFFER : in     RTEMS.ADDRESS;
      SIZE   : in     RTEMS.UNSIGNED32;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure MESSAGE_QUEUE_URGENT (
      ID     : in     RTEMS.ID;
      BUFFER : in     RTEMS.ADDRESS;
      SIZE   : in     RTEMS.UNSIGNED32;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure MESSAGE_QUEUE_BROADCAST (
      ID     : in     RTEMS.ID;
      BUFFER : in     RTEMS.ADDRESS;
      SIZE   : in     RTEMS.UNSIGNED32;
      COUNT  :    out RTEMS.UNSIGNED32;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure MESSAGE_QUEUE_RECEIVE (
      ID         : in     RTEMS.ID;
      BUFFER     : in     RTEMS.ADDRESS;
      OPTION_SET : in     RTEMS.OPTION;
      TIMEOUT    : in     RTEMS.INTERVAL;
      SIZE       :    out RTEMS.UNSIGNED32;
      RESULT     :    out RTEMS.STATUS_CODES
   );

   procedure MESSAGE_QUEUE_FLUSH (
      ID     : in     RTEMS.ID;
      COUNT  :    out RTEMS.UNSIGNED32;
      RESULT :    out RTEMS.STATUS_CODES
   );

-- Event Manager

   procedure EVENT_SEND (
      ID       : in     RTEMS.ID;
      EVENT_IN : in     RTEMS.EVENT_SET;
      RESULT   :    out RTEMS.STATUS_CODES
   );

   procedure EVENT_RECEIVE (
      EVENT_IN   : in     RTEMS.EVENT_SET;
      OPTION_SET : in     RTEMS.OPTION;
      TICKS      : in     RTEMS.INTERVAL;
      EVENT_OUT  :    out RTEMS.EVENT_SET;
      RESULT     :    out RTEMS.STATUS_CODES
   );

-- Signal Manager

   procedure SIGNAL_CATCH (
      ASR_HANDLER : in     RTEMS.ASR_HANDLER;
      MODE_SET    : in     RTEMS.MODE;
      RESULT      :    out RTEMS.STATUS_CODES
   );

   procedure SIGNAL_SEND (
      ID         : in     RTEMS.ID;
      SIGNAL_SET : in     RTEMS.SIGNAL_SET;
      RESULT     :    out RTEMS.STATUS_CODES
   );

-- Partition Manager

   procedure PARTITION_CREATE (
      NAME             : in     RTEMS.NAME;
      STARTING_ADDRESS : in     RTEMS.ADDRESS;
      LENGTH           : in     RTEMS.UNSIGNED32;
      BUFFER_SIZE      : in     RTEMS.UNSIGNED32;
      ATTRIBUTE_SET    : in     RTEMS.ATTRIBUTE;
      ID               :    out RTEMS.ID;
      RESULT           :    out RTEMS.STATUS_CODES
   );

   procedure PARTITION_IDENT (
      NAME   : in     RTEMS.NAME;
      NODE   : in     RTEMS.UNSIGNED32;
      ID     :    out RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure PARTITION_DELETE (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure PARTITION_GET_BUFFER (
      ID     : in     RTEMS.ID;
      BUFFER :    out RTEMS.ADDRESS;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure PARTITION_RETURN_BUFFER (
      ID     : in     RTEMS.ID;
      BUFFER : in     RTEMS.ADDRESS;
      RESULT :    out RTEMS.STATUS_CODES
   );

-- Region Manager

   procedure REGION_CREATE (
      NAME             : in     RTEMS.NAME;
      STARTING_ADDRESS : in     RTEMS.ADDRESS;
      LENGTH           : in     RTEMS.UNSIGNED32;
      PAGE_SIZE        : in     RTEMS.UNSIGNED32;
      ATTRIBUTE_SET    : in     RTEMS.ATTRIBUTE;
      ID               :    out RTEMS.ID;
      RESULT           :    out RTEMS.STATUS_CODES
   );

   procedure REGION_IDENT (
      NAME   : in     RTEMS.NAME;
      ID     :    out RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure REGION_DELETE (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure REGION_GET_SEGMENT (
      ID         : in     RTEMS.ID;
      SIZE       : in     RTEMS.UNSIGNED32;
      OPTION_SET : in     RTEMS.OPTION;
      TIMEOUT    : in     RTEMS.INTERVAL;
      SEGMENT    :    out RTEMS.ADDRESS;
      RESULT     :    out RTEMS.STATUS_CODES
   );

   procedure REGION_RETURN_SEGMENT (
      ID      : in     RTEMS.ID;
      SEGMENT : in     RTEMS.ADDRESS;
      RESULT  :    out RTEMS.STATUS_CODES
   );

-- Dual Ported Memory Manager

   procedure PORT_CREATE (
      NAME           : in     RTEMS.NAME;
      INTERNAL_START : in     RTEMS.ADDRESS;
      EXTERNAL_START : in     RTEMS.ADDRESS;
      LENGTH         : in     RTEMS.UNSIGNED32;
      ID             :    out RTEMS.ID;
      RESULT         :    out RTEMS.STATUS_CODES
   );

   procedure PORT_IDENT (
      NAME   : in     RTEMS.NAME;
      ID     :    out RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure PORT_DELETE (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure PORT_EXTERNAL_TO_INTERNAL (
      ID       : in     RTEMS.ID;
      EXTERNAL : in     RTEMS.ADDRESS;
      INTERNAL :    out RTEMS.ADDRESS;
      RESULT   :    out RTEMS.STATUS_CODES
   );

   procedure PORT_INTERNAL_TO_EXTERNAL (
      ID       : in     RTEMS.ID;
      INTERNAL : in     RTEMS.ADDRESS;
      EXTERNAL :    out RTEMS.ADDRESS;
      RESULT   :    out RTEMS.STATUS_CODES
   );

-- Fatal Error Manager

   procedure FATAL_ERROR_OCCURRED (
      THE_ERROR : in     RTEMS.UNSIGNED32
   );

-- Rate Monotonic Manager

   procedure RATE_MONOTONIC_CREATE (
      NAME   : in     RTEMS.NAME;
      ID     :    out RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure RATE_MONOTONIC_IDENT (
      NAME   : in     RTEMS.NAME;
      ID     :    out RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure RATE_MONOTONIC_DELETE (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure RATE_MONOTONIC_CANCEL (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   );

   procedure RATE_MONOTONIC_PERIOD (
      ID      : in     RTEMS.ID;
      LENGTH  : in     RTEMS.INTERVAL;
      RESULT  :    out RTEMS.STATUS_CODES
   );

-- Multiprocessing Manager

   procedure MULTIPROCESSING_ANNOUNCE;

end DUMMY_RTEMS;
