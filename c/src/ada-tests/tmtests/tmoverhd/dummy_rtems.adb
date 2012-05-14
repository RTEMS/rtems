--
--  DUMMY_RTEMS / SPECIFICATION
--
--  DESCRIPTION:
--
--  This package contains implementation of stub routines
--  which are used to time the invocation overhead incurred
--  with an Ada application program invokes each RTEMS directive.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989-2009.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

package body DUMMY_RTEMS is

-- Task Manager

   procedure TASK_CREATE (
      NAME             : in     RTEMS.NAME;
      INITIAL_PRIORITY : in     RTEMS.TASKS.PRIORITY;
      STACK_SIZE       : in     RTEMS.UNSIGNED32;
      INITIAL_MODES    : in     RTEMS.MODE;
      ATTRIBUTE_SET    : in     RTEMS.ATTRIBUTE;
      ID               :    out RTEMS.ID;
      RESULT           :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(NAME);
      pragma Unreferenced(INITIAL_PRIORITY);
      pragma Unreferenced(STACK_SIZE);
      pragma Unreferenced(INITIAL_MODES);
      pragma Unreferenced(ATTRIBUTE_SET);
   begin

      ID := 0;
      RESULT := RTEMS.SUCCESSFUL;

   end TASK_CREATE;

   procedure TASK_IDENT (
      NAME   : in     RTEMS.NAME;
      NODE   : in     RTEMS.UNSIGNED32;
      ID     :    out RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(NAME);
      pragma Unreferenced(NODE);
   begin

      ID := 0;
      RESULT := RTEMS.SUCCESSFUL;

   end TASK_IDENT;

   procedure TASK_START (
      ID          : in     RTEMS.ID;
      ENTRY_POINT : in     RTEMS.TASKS.ENTRY_POINT;
      ARGUMENT    : in     RTEMS.UNSIGNED32;
      RESULT      :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
      pragma Unreferenced(ENTRY_POINT);
      pragma Unreferenced(ARGUMENT);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end TASK_START;

   procedure TASK_RESTART (
      ID       : in     RTEMS.ID;
      ARGUMENT : in     RTEMS.UNSIGNED32;
      RESULT   :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
      pragma Unreferenced(ARGUMENT);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end TASK_RESTART;

   procedure TASK_DELETE (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end TASK_DELETE;

   procedure TASK_SUSPEND (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end TASK_SUSPEND;

   procedure TASK_RESUME (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end TASK_RESUME;

   procedure TASK_SET_PRIORITY (
      ID           : in     RTEMS.ID;
      NEW_PRIORITY : in     RTEMS.TASKS.PRIORITY;
      OLD_PRIORITY :    out RTEMS.TASKS.PRIORITY;
      RESULT       :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
      pragma Unreferenced(NEW_PRIORITY);
   begin

      OLD_PRIORITY := 0;
      RESULT := RTEMS.SUCCESSFUL;

   end TASK_SET_PRIORITY;

   procedure TASK_MODE (
      MODE_SET          : in     RTEMS.MODE;
      MASK              : in     RTEMS.MODE;
      PREVIOUS_MODE_SET :    out RTEMS.MODE;
      RESULT            :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(MODE_SET);
      pragma Unreferenced(MASK);
   begin

      PREVIOUS_MODE_SET := 0;
      RESULT := RTEMS.SUCCESSFUL;

   end TASK_MODE;

   procedure TASK_GET_NOTE (
      ID      : in     RTEMS.ID;
      NOTEPAD : in     RTEMS.NOTEPAD_INDEX;
      NOTE    :    out RTEMS.UNSIGNED32;
      RESULT  :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
      pragma Unreferenced(NOTEPAD);
   begin

      NOTE := 0;
      RESULT := RTEMS.SUCCESSFUL;

   end TASK_GET_NOTE;

   procedure TASK_SET_NOTE (
      ID      : in     RTEMS.ID;
      NOTEPAD : in     RTEMS.NOTEPAD_INDEX;
      NOTE    : in     RTEMS.UNSIGNED32;
      RESULT  :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
      pragma Unreferenced(NOTEPAD);
      pragma Unreferenced(NOTE);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end TASK_SET_NOTE;

   procedure TASK_WAKE_WHEN (
      TIME_BUFFER : in     RTEMS.TIME_OF_DAY;
      RESULT      :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(TIME_BUFFER);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end TASK_WAKE_WHEN;

   procedure TASK_WAKE_AFTER (
      TICKS  : in     RTEMS.INTERVAL;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(TICKS);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end TASK_WAKE_AFTER;

-- Interrupt Manager

--   procedure INTERRUPT_CATCH (
--      NEW_ISR_HANDLER : in     RTEMS.ADDRESS;
--      VECTOR          : in     RTEMS.VECTOR_NUMBER;
--      OLD_ISR_HANDLER :    out RTEMS.ADDRESS;
--      RESULT          :    out RTEMS.STATUS_CODES
--   ) is
--   begin
--
--      OLD_ISR_HANDLER := RTEMS.Null_Address;
--      RESULT := RTEMS.SUCCESSFUL;
--
--   end INTERRUPT_CATCH;

-- Clock Manager

   procedure CLOCK_GET (
      OPTION      : in     RTEMS.CLOCK.GET_OPTIONS;
      TIME_BUFFER : in     RTEMS.ADDRESS;
      RESULT      :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(OPTION);
      pragma Unreferenced(TIME_BUFFER);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end CLOCK_GET;

   procedure CLOCK_SET (
      TIME_BUFFER : in     RTEMS.TIME_OF_DAY;
      RESULT      :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(TIME_BUFFER);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end CLOCK_SET;

   procedure CLOCK_TICK (
      RESULT :    out RTEMS.STATUS_CODES
   ) is
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end CLOCK_TICK;

-- Timer Manager

   procedure TIMER_CREATE (
      NAME   : in     RTEMS.NAME;
      ID     :    out RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(NAME);
   begin

      ID := 0;
      RESULT := RTEMS.SUCCESSFUL;

   end TIMER_CREATE;

   procedure TIMER_DELETE (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end TIMER_DELETE;

   procedure TIMER_IDENT (
      NAME   : in     RTEMS.NAME;
      ID     :    out RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(NAME);
   begin

      ID := 0;
      RESULT := RTEMS.SUCCESSFUL;

   end TIMER_IDENT;

   procedure TIMER_FIRE_AFTER (
      ID        : in     RTEMS.ID;
      TICKS     : in     RTEMS.INTERVAL;
      ROUTINE   : in     RTEMS.TIMER.SERVICE_ROUTINE;
      USER_DATA : in     RTEMS.ADDRESS;
      RESULT    :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
      pragma Unreferenced(TICKS);
      pragma Unreferenced(ROUTINE);
      pragma Unreferenced(USER_DATA);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end TIMER_FIRE_AFTER;

   procedure TIMER_FIRE_WHEN (
      ID        : in     RTEMS.ID;
      WALL_TIME : in     RTEMS.TIME_OF_DAY;
      ROUTINE   : in     RTEMS.TIMER.SERVICE_ROUTINE;
      USER_DATA : in     RTEMS.ADDRESS;
      RESULT    :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
      pragma Unreferenced(WALL_TIME);
      pragma Unreferenced(ROUTINE);
      pragma Unreferenced(USER_DATA);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end TIMER_FIRE_WHEN;

   procedure TIMER_RESET (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end TIMER_RESET;

   procedure TIMER_CANCEL (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end TIMER_CANCEL;

-- Semaphore Manager

   procedure SEMAPHORE_CREATE (
      NAME             : in     RTEMS.NAME;
      COUNT            : in     RTEMS.UNSIGNED32;
      ATTRIBUTE_SET    : in     RTEMS.ATTRIBUTE;
      PRIORITY_CEILING : in     RTEMS.TASKS.PRIORITY;
      ID               :    out RTEMS.ID;
      RESULT           :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(NAME);
      pragma Unreferenced(COUNT);
      pragma Unreferenced(ATTRIBUTE_SET);
      pragma Unreferenced(PRIORITY_CEILING);
   begin

      ID := 0;
      RESULT := RTEMS.SUCCESSFUL;

   end SEMAPHORE_CREATE;

   procedure SEMAPHORE_DELETE (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end SEMAPHORE_DELETE;

   procedure SEMAPHORE_IDENT (
      NAME   : in     RTEMS.NAME;
      NODE   : in     RTEMS.UNSIGNED32;
      ID     :    out RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(NAME);
      pragma Unreferenced(NODE);
   begin

      ID := 0;
      RESULT := RTEMS.SUCCESSFUL;

   end SEMAPHORE_IDENT;

   procedure SEMAPHORE_OBTAIN (
      ID         : in     RTEMS.ID;
      OPTION_SET : in     RTEMS.OPTION;
      TIMEOUT    : in     RTEMS.INTERVAL;
      RESULT     :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
      pragma Unreferenced(OPTION_SET);
      pragma Unreferenced(TIMEOUT);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end SEMAPHORE_OBTAIN;

   procedure SEMAPHORE_RELEASE (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end SEMAPHORE_RELEASE;

-- Message Queue Manager

   procedure MESSAGE_QUEUE_CREATE (
      Name             : in     RTEMS.Name;
      Count            : in     RTEMS.Unsigned32;
      Max_Message_Size : in     RTEMS.Unsigned32;
      Attribute_Set    : in     RTEMS.Attribute;
      ID               :    out RTEMS.ID;
      Result           :    out RTEMS.Status_Codes
   ) is
      pragma Unreferenced(Name);
      pragma Unreferenced(Count);
      pragma Unreferenced(Max_Message_Size);
      pragma Unreferenced(Attribute_Set);
   begin

      ID := 0;
      RESULT := RTEMS.SUCCESSFUL;

   end MESSAGE_QUEUE_CREATE;

   procedure MESSAGE_QUEUE_IDENT (
      NAME   : in     RTEMS.NAME;
      NODE   : in     RTEMS.UNSIGNED32;
      ID     :    out RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(NAME);
      pragma Unreferenced(NODE);
   begin

      ID := 0;
      RESULT := RTEMS.SUCCESSFUL;

   end MESSAGE_QUEUE_IDENT;

   procedure MESSAGE_QUEUE_DELETE (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end MESSAGE_QUEUE_DELETE;

   procedure MESSAGE_QUEUE_SEND (
      ID     : in     RTEMS.ID;
      BUFFER : in     RTEMS.ADDRESS;
      Size   : in     RTEMS.Unsigned32;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
      pragma Unreferenced(BUFFER);
      pragma Unreferenced(Size);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end MESSAGE_QUEUE_SEND;

   procedure MESSAGE_QUEUE_URGENT (
      ID     : in     RTEMS.ID;
      BUFFER : in     RTEMS.ADDRESS;
      Size   : in     RTEMS.Unsigned32;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
      pragma Unreferenced(BUFFER);
      pragma Unreferenced(Size);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end MESSAGE_QUEUE_URGENT;

   procedure MESSAGE_QUEUE_BROADCAST (
      ID     : in     RTEMS.ID;
      BUFFER : in     RTEMS.ADDRESS;
      Size   : in     RTEMS.Unsigned32;
      COUNT  :    out RTEMS.UNSIGNED32;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
      pragma Unreferenced(BUFFER);
      pragma Unreferenced(Size);
   begin

      COUNT := 0;
      RESULT := RTEMS.SUCCESSFUL;

   end MESSAGE_QUEUE_BROADCAST;

   procedure MESSAGE_QUEUE_RECEIVE (
      ID         : in     RTEMS.ID;
      BUFFER     : in     RTEMS.ADDRESS;
      OPTION_SET : in     RTEMS.OPTION;
      TIMEOUT    : in     RTEMS.INTERVAL;
      Size       :    out RTEMS.Unsigned32;
      RESULT     :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
      pragma Unreferenced(BUFFER);
      pragma Unreferenced(OPTION_SET);
      pragma Unreferenced(TIMEOUT);
   begin

      Size := 0;
      RESULT := RTEMS.SUCCESSFUL;

   end MESSAGE_QUEUE_RECEIVE;

   procedure MESSAGE_QUEUE_FLUSH (
      ID     : in     RTEMS.ID;
      COUNT  :    out RTEMS.UNSIGNED32;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
   begin

      COUNT := 0;
      RESULT := RTEMS.SUCCESSFUL;

   end MESSAGE_QUEUE_FLUSH;

-- Event Manager

   procedure EVENT_SEND (
      ID       : in     RTEMS.ID;
      EVENT_IN : in     RTEMS.EVENT_SET;
      RESULT   :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
      pragma Unreferenced(EVENT_IN);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end EVENT_SEND;

   procedure EVENT_RECEIVE (
      EVENT_IN   : in     RTEMS.EVENT_SET;
      OPTION_SET : in     RTEMS.OPTION;
      TICKS      : in     RTEMS.INTERVAL;
      EVENT_OUT  :    out RTEMS.EVENT_SET;
      RESULT     :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(EVENT_IN);
      pragma Unreferenced(OPTION_SET);
      pragma Unreferenced(TICKS);
   begin

      EVENT_OUT := 0;
      RESULT := RTEMS.SUCCESSFUL;

   end EVENT_RECEIVE;

-- Signal Manager

   procedure SIGNAL_CATCH (
      ASR_HANDLER : in     RTEMS.ASR_HANDLER;
      MODE_SET    : in     RTEMS.MODE;
      RESULT      :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ASR_HANDLER);
      pragma Unreferenced(MODE_SET);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end SIGNAL_CATCH;

   procedure SIGNAL_SEND (
      ID         : in     RTEMS.ID;
      SIGNAL_SET : in     RTEMS.SIGNAL_SET;
      RESULT     :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
      pragma Unreferenced(SIGNAL_SET);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end SIGNAL_SEND;

-- Partition Manager

   procedure PARTITION_CREATE (
      NAME             : in     RTEMS.NAME;
      STARTING_ADDRESS : in     RTEMS.ADDRESS;
      LENGTH           : in     RTEMS.UNSIGNED32;
      BUFFER_SIZE      : in     RTEMS.UNSIGNED32;
      ATTRIBUTE_SET    : in     RTEMS.ATTRIBUTE;
      ID               :    out RTEMS.ID;
      RESULT           :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(NAME);
      pragma Unreferenced(STARTING_ADDRESS);
      pragma Unreferenced(LENGTH);
      pragma Unreferenced(BUFFER_SIZE);
      pragma Unreferenced(ATTRIBUTE_SET);
   begin

      ID := 0;
      RESULT := RTEMS.SUCCESSFUL;

   end PARTITION_CREATE;

   procedure PARTITION_IDENT (
      NAME   : in     RTEMS.NAME;
      NODE   : in     RTEMS.UNSIGNED32;
      ID     :    out RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(NAME);
      pragma Unreferenced(NODE);
   begin

      ID := 0;
      RESULT := RTEMS.SUCCESSFUL;

   end PARTITION_IDENT;

   procedure PARTITION_DELETE (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end PARTITION_DELETE;

   procedure PARTITION_GET_BUFFER (
      ID     : in     RTEMS.ID;
      BUFFER :    out RTEMS.ADDRESS;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
   begin

      BUFFER := RTEMS.Null_Address;
      RESULT := RTEMS.SUCCESSFUL;

   end PARTITION_GET_BUFFER;

   procedure PARTITION_RETURN_BUFFER (
      ID     : in     RTEMS.ID;
      BUFFER : in     RTEMS.ADDRESS;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
      pragma Unreferenced(BUFFER);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end PARTITION_RETURN_BUFFER;

-- Region Manager

   procedure REGION_CREATE (
      NAME             : in     RTEMS.NAME;
      STARTING_ADDRESS : in     RTEMS.ADDRESS;
      LENGTH           : in     RTEMS.UNSIGNED32;
      PAGE_SIZE        : in     RTEMS.UNSIGNED32;
      ATTRIBUTE_SET    : in     RTEMS.ATTRIBUTE;
      ID               :    out RTEMS.ID;
      RESULT           :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(NAME);
      pragma Unreferenced(STARTING_ADDRESS);
      pragma Unreferenced(LENGTH);
      pragma Unreferenced(PAGE_SIZE);
      pragma Unreferenced(ATTRIBUTE_SET);
   begin

      ID := 0;
      RESULT := RTEMS.SUCCESSFUL;

   end REGION_CREATE;

   procedure REGION_IDENT (
      NAME   : in     RTEMS.NAME;
      ID     :    out RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(NAME);
   begin

      ID := 0;
      RESULT := RTEMS.SUCCESSFUL;

   end REGION_IDENT;

   procedure REGION_DELETE (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end REGION_DELETE;

   procedure REGION_GET_SEGMENT (
      ID         : in     RTEMS.ID;
      SIZE       : in     RTEMS.UNSIGNED32;
      OPTION_SET : in     RTEMS.OPTION;
      TIMEOUT    : in     RTEMS.INTERVAL;
      SEGMENT    :    out RTEMS.ADDRESS;
      RESULT     :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
      pragma Unreferenced(SIZE);
      pragma Unreferenced(OPTION_SET);
      pragma Unreferenced(TIMEOUT);
   begin

      SEGMENT := RTEMS.Null_Address;
      RESULT := RTEMS.SUCCESSFUL;

   end REGION_GET_SEGMENT;

   procedure REGION_RETURN_SEGMENT (
      ID      : in     RTEMS.ID;
      SEGMENT : in     RTEMS.ADDRESS;
      RESULT  :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
      pragma Unreferenced(SEGMENT);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end REGION_RETURN_SEGMENT;

-- Dual Ported Memory Manager

   procedure PORT_CREATE (
      NAME           : in     RTEMS.NAME;
      INTERNAL_START : in     RTEMS.ADDRESS;
      EXTERNAL_START : in     RTEMS.ADDRESS;
      LENGTH         : in     RTEMS.UNSIGNED32;
      ID             :    out RTEMS.ID;
      RESULT         :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(NAME);
      pragma Unreferenced(INTERNAL_START);
      pragma Unreferenced(EXTERNAL_START);
      pragma Unreferenced(LENGTH);
   begin

      ID := 0;
      RESULT := RTEMS.SUCCESSFUL;

   end PORT_CREATE;

   procedure PORT_IDENT (
      NAME   : in     RTEMS.NAME;
      ID     :    out RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(NAME);
   begin

      ID := 0;
      RESULT := RTEMS.SUCCESSFUL;

   end PORT_IDENT;

   procedure PORT_DELETE (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end PORT_DELETE;

   procedure PORT_EXTERNAL_TO_INTERNAL (
      ID       : in     RTEMS.ID;
      EXTERNAL : in     RTEMS.ADDRESS;
      INTERNAL :    out RTEMS.ADDRESS;
      RESULT   :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
      pragma Unreferenced(EXTERNAL);
   begin

      INTERNAL := RTEMS.Null_Address;
      RESULT := RTEMS.SUCCESSFUL;

   end PORT_EXTERNAL_TO_INTERNAL;

   procedure PORT_INTERNAL_TO_EXTERNAL (
      ID       : in     RTEMS.ID;
      INTERNAL : in     RTEMS.ADDRESS;
      EXTERNAL :    out RTEMS.ADDRESS;
      RESULT   :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
      pragma Unreferenced(INTERNAL);
   begin

      EXTERNAL := RTEMS.Null_Address;
      RESULT := RTEMS.SUCCESSFUL;

   end PORT_INTERNAL_TO_EXTERNAL;

-- Fatal Error Manager

   procedure FATAL_ERROR_OCCURRED (
      THE_ERROR : in     RTEMS.UNSIGNED32
   ) is
      pragma Unreferenced(THE_ERROR);
   begin

      NULL;

   end FATAL_ERROR_OCCURRED;

-- Rate Monotonic Manager

   procedure RATE_MONOTONIC_CREATE (
      NAME   : in     RTEMS.NAME;
      ID     :    out RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(NAME);
   begin

      ID := 0;
      RESULT := RTEMS.SUCCESSFUL;

   end RATE_MONOTONIC_CREATE;

   procedure RATE_MONOTONIC_IDENT (
      NAME   : in     RTEMS.NAME;
      ID     :    out RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(NAME);
   begin

      ID := 0;
      RESULT := RTEMS.SUCCESSFUL;

   end RATE_MONOTONIC_IDENT;

   procedure RATE_MONOTONIC_DELETE (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end RATE_MONOTONIC_DELETE;

   procedure RATE_MONOTONIC_CANCEL (
      ID     : in     RTEMS.ID;
      RESULT :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end RATE_MONOTONIC_CANCEL;

   procedure RATE_MONOTONIC_PERIOD (
      ID      : in     RTEMS.ID;
      LENGTH  : in     RTEMS.INTERVAL;
      RESULT  :    out RTEMS.STATUS_CODES
   ) is
      pragma Unreferenced(ID);
      pragma Unreferenced(LENGTH);
   begin

      RESULT := RTEMS.SUCCESSFUL;

   end RATE_MONOTONIC_PERIOD;

-- Multiprocessing Manager

   procedure MULTIPROCESSING_ANNOUNCE
   is
   begin

      NULL;

   end MULTIPROCESSING_ANNOUNCE;

end DUMMY_RTEMS;
