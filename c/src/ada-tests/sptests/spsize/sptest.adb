--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 1 of the RTEMS
--  Single Processor Test Suite.
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
--  $Id$
--

with INTERFACES; use INTERFACES;
with RTEMS;
with RTEMS_TEST_SUPPORT;
with TEST_SUPPORT;
with TEXT_IO;
with UNSIGNED32_IO;

package body SPTEST is


--PAGE
-- 
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TEST SIZE ***" );

      SPTEST.PUT_SIZE(
         "Tasks",
         TRUE,
         "maximum_tasks",
         RTEMS_TEST_SUPPORT.PER_TASK
      );

      SPTEST.PUT_SIZE(
         "Timers",
         TRUE,
         "maximum_timers",
         RTEMS_TEST_SUPPORT.PER_TIMER
      );

      SPTEST.PUT_SIZE(
         "Semaphores",
         TRUE,
         "maximum_semaphores",
         RTEMS_TEST_SUPPORT.PER_SEMAPHORE
      );

      SPTEST.PUT_SIZE(
         "Message Queues",
         TRUE,
         "maximum_message_queues",
         RTEMS_TEST_SUPPORT.PER_MESSAGE_QUEUE
      );

      SPTEST.PUT_SIZE(
         "Messages",
         TRUE,
         "maximum_messages",
         RTEMS_TEST_SUPPORT.PER_MESSAGE
      );

      SPTEST.PUT_SIZE(
         "Regions",
         TRUE,
         "maximum_regions",
         RTEMS_TEST_SUPPORT.PER_REGION
      );

      SPTEST.PUT_SIZE(
         "Partitions",
         TRUE,
         "maximum_paritions",
         RTEMS_TEST_SUPPORT.PER_PARTITION
      );

      SPTEST.PUT_SIZE(
         "Ports",
         TRUE,
         "maximum_ports",
         RTEMS_TEST_SUPPORT.PER_PORT
      );

      SPTEST.PUT_SIZE(
         "Periods",
         TRUE,
         "maximum_periods",
         RTEMS_TEST_SUPPORT.PER_PERIOD
      );

      SPTEST.PUT_SIZE(
         "Extensions",
         TRUE,
         "maximum_extensions",
         RTEMS_TEST_SUPPORT.PER_EXTENSION
      );

      SPTEST.PUT_SIZE(
         "Device Drivers",
         TRUE,
         "number_of_device_drivers",
         RTEMS_TEST_SUPPORT.PER_DRIVER
      );

      SPTEST.PUT_SIZE(
         "System Requirements",
         FALSE,
         REQUIREMENT => RTEMS_TEST_SUPPORT.SYSTEM_REQUIREMENTS
      );

      SPTEST.PUT_SIZE(
         "Floating Point Tasks",
         TRUE,
         "FP attributes",
         RTEMS_TEST_SUPPORT.PER_FP_TASK
      );

      TEXT_IO.PUT_LINE( "User's Tasks' Stacks - " );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT_LINE( "Interrupt Stacks - " );
      TEXT_IO.NEW_LINE;

      SPTEST.PUT_SIZE(
         "Global object tables",
         TRUE,
         "maximum_nodes",
         RTEMS_TEST_SUPPORT.PER_NODE
      );

      SPTEST.PUT_SIZE(
         "Global objects",
         TRUE,
         "maximum_global_objects",
         RTEMS_TEST_SUPPORT.PER_GLOBAL_OBJECT
      );

      SPTEST.PUT_SIZE(
         "Proxies",
         TRUE,
         "maximum_proxies",
         RTEMS_TEST_SUPPORT.PER_PROXY
      );

      TEXT_IO.PUT_LINE( "*** END OF TEST SIZE ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end INIT;

--PAGE
--
--  PUT_SIZE
--

   procedure PUT_SIZE (
      DESCRIPTION    : in     STRING;
      HAS_FIELD_NAME : in     BOOLEAN;
      FIELD_NAME     : in     STRING             := "";
      REQUIREMENT    : in     RTEMS.UNSIGNED32   := 0
   ) is
      SPACES : constant STRING := "                              ";
   begin

      -- 21 is length of longest description (task's stacks)

      TEXT_IO.PUT( DESCRIPTION );
      TEXT_IO.PUT( SPACES( 1 .. 21 - DESCRIPTION'LENGTH ) );
      TEXT_IO.PUT( " - " );
      if HAS_FIELD_NAME = TRUE then
         TEXT_IO.PUT( FIELD_NAME );
         TEXT_IO.PUT( " * " );
      end if;
      UNSIGNED32_IO.PUT( REQUIREMENT );
      TEXT_IO.NEW_LINE;

  end PUT_SIZE;
    

end SPTEST;
