--
--  MPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation for Test 11 of the RTEMS
--  Multiprocessor Test Suite.
--
--  DEPENDENCIES:
--
--
--
--  COPYRIGHT (c) 1989-2011.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

with INTERFACES; use INTERFACES;
with RTEMS;
with RTEMS.MESSAGE_QUEUE;
with RTEMS.PARTITION;
with RTEMS.SEMAPHORE;
with RTEMS.TASKS;
with TEST_SUPPORT;
with TEXT_IO;
with UNSIGNED32_IO;

package body MPTEST is

--
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT( "*** TEST 11 -- NODE " );
      UNSIGNED32_IO.PUT(
         TEST_SUPPORT.NODE,
         WIDTH => 1
      );
      TEXT_IO.PUT_LINE( " ***" );

      MPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  '1', '1', '1', ' ' );
      MPTEST.TASK_NAME( 2 ) := RTEMS.BUILD_NAME(  '2', '2', '2', ' ' );

      MPTEST.QUEUE_NAME( 1 ) := RTEMS.BUILD_NAME(  'M', 'S', 'G', ' ' );

      MPTEST.SEMAPHORE_NAME( 1 ) := RTEMS.BUILD_NAME(  'S', 'E', 'M', ' ' );

      MPTEST.PARTITION_NAME( 1 ) := RTEMS.BUILD_NAME(  'P', 'A', 'R', ' ' );

      if TEST_SUPPORT.NODE = 1 then

         TEXT_IO.PUT_LINE( "Attempting to create Test_task (Global)" );
         RTEMS.TASKS.CREATE(
            MPTEST.TASK_NAME( 1 ),
            1,
            2048,
            RTEMS.DEFAULT_MODES,
            RTEMS.GLOBAL,
            MPTEST.TASK_ID( 1 ),
            STATUS
         );
         TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
            STATUS,
            RTEMS.TOO_MANY,
            "TASK_CREATE"
         );
         TEXT_IO.PUT_LINE( "task_create correctly returned TOO_MANY" );

         TEXT_IO.PUT_LINE( "Attempting to create Message Queue (Global)" );
         RTEMS.MESSAGE_QUEUE.CREATE(
            MPTEST.QUEUE_NAME( 1 ),
            3,
            RTEMS.GLOBAL + RTEMS.LIMIT,
            MPTEST.QUEUE_ID( 1 ),
            STATUS
         );
         TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
            STATUS,
            RTEMS.TOO_MANY,
            "MESSAGE_QUEUE_CREATE"
         );
         TEXT_IO.PUT_LINE( 
            "message_queue_create correctly returned TOO_MANY"
         );

         TEXT_IO.PUT_LINE( "Creating Semaphore (Global)" );
         RTEMS.SEMAPHORE.CREATE(
            MPTEST.SEMAPHORE_NAME( 1 ),
            1,
            RTEMS.GLOBAL,
            MPTEST.SEMAPHORE_ID( 1 ),
            STATUS
         );
         TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
            STATUS, 
            RTEMS.TOO_MANY, 
            "SEMAPHORE_CREATE"
         );
         TEXT_IO.PUT_LINE( "semaphore_create correctly returned TOO_MANY" );

         TEXT_IO.PUT_LINE( "Creating Partition (Global)" );
         RTEMS.PARTITION.CREATE(
            MPTEST.PARTITION_NAME( 1 ),
            MPTEST.PARTITION_AREA( 0 )'ADDRESS,
            128,
            64,
            RTEMS.GLOBAL,
            MPTEST.PARTITION_ID( 1 ),
            STATUS
         );
         TEST_SUPPORT.FATAL_DIRECTIVE_STATUS(
            STATUS,
            RTEMS.TOO_MANY,
            "PARTITION_CREATE"
         );
         TEXT_IO.PUT_LINE( "partition_create correctly returned TOO_MANY" );

      end if;

      TEXT_IO.PUT_LINE( "*** END OF TEST 11 ***" );

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end INIT; 
 
end MPTEST;
