--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 18 of the RTEMS
--  Timing Test Suite.
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
--  http://www.OARcorp.com/rtems/license.html.
--
--  $Id$
--

with INTERFACES; use INTERFACES;
with RTEMS;
with RTEMS_CALLING_OVERHEAD;
with TEST_SUPPORT;
with TEXT_IO;
with TIME_TEST_SUPPORT;
with UNSIGNED32_IO;

package body TMTEST is

--PAGE
-- 
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      STATUS     : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TIME TEST 18 ***" );

      TMTEST.TEST_INIT;

      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--PAGE
-- 
--  TEST_INIT
--

   procedure TEST_INIT
   is
      ID         : RTEMS.ID;
      TASK_ENTRY : RTEMS.TASK_ENTRY;
      INDEX      : RTEMS.UNSIGNED32;
      STATUS     : RTEMS.STATUS_CODES;
   begin

      for INDEX in 0 .. TIME_TEST_SUPPORT.OPERATION_COUNT
      loop

         RTEMS.TASK_CREATE( 
            RTEMS.BUILD_NAME( 'T', 'I', 'M', 'E' ),
            128,
            1024, 
            RTEMS.DEFAULT_MODES,
            RTEMS.DEFAULT_ATTRIBUTES,
            ID,
            STATUS
         );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE LOOP" );


         if INDEX = TIME_TEST_SUPPORT.OPERATION_COUNT then
            TASK_ENTRY := TMTEST.LAST_TASK'ACCESS;
         elsif INDEX = 0 then
            TASK_ENTRY := TMTEST.FIRST_TASK'ACCESS;
         else
            TASK_ENTRY := TMTEST.MIDDLE_TASKS'ACCESS;
         end if;

         RTEMS.TASK_START( ID, TASK_ENTRY, 0, STATUS );
         TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START LOOP" );

      end loop;

   end TEST_INIT;

--PAGE
-- 
--  FIRST_TASK
--

   procedure FIRST_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      STATUS : RTEMS.STATUS_CODES;
   begin

      TIMER_DRIVER.INITIALIZE;

      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );

   end FIRST_TASK;

--PAGE
-- 
--  MIDDLE_TASKS
--

   procedure MIDDLE_TASKS (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );

   end MIDDLE_TASKS;

--PAGE
-- 
--  LAST_TASK
--

   procedure LAST_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
   begin

      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "TASK_DELETE (self)", 
         TMTEST.END_TIME, 
         TIME_TEST_SUPPORT.OPERATION_COUNT, 
         0,
         RTEMS_CALLING_OVERHEAD.TASK_DELETE 
      );

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end LAST_TASK;

end TMTEST;
