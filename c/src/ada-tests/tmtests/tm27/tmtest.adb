--
--  TMTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 27 of the RTEMS
--  Timing Test Suite.
--
--  DEPENDENCIES: 
--
--  
--
--  COPYRIGHT (c) 1989-1997.
--  On-Line Applications Research Corporation (OAR).
--  Copyright assigned to U.S. Government, 1994.
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.OARcorp.com/rtems/license.html.
--
--  $Id$
--

with INTERRUPT_TIME_TEST_SUPPORT;
with INTERFACES; use INTERFACES;
with RTEMS_CALLING_OVERHEAD;
with RTEMS;
with RTEMS_TEST_SUPPORT;
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
      STATUS  : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** TIME TEST 27 ***" );

      RTEMS.TASK_CREATE( 
         RTEMS.BUILD_NAME( 'T', 'A', '1', ' ' ),
         254, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         TMTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE TASK_1" );

      RTEMS.TASK_START( 
         TMTEST.TASK_ID( 1 ),
         TMTEST.TASK_1'ACCESS, 
         0, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START TASK_1" );

      RTEMS.TASK_CREATE( 
         RTEMS.BUILD_NAME( 'T', 'A', '2', ' ' ),
         254, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         TMTEST.TASK_ID( 2 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE TASK_2" );

      RTEMS.TASK_START( 
         TMTEST.TASK_ID( 2 ),
         TMTEST.TASK_2'ACCESS, 
         0, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START TASK_2" );

      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--PAGE
-- 
--  TASK_1
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
   begin

      RTEMS_TEST_SUPPORT.THREAD_DISPATCH_DISABLE_LEVEL := 1;

      TMTEST.INTERRUPT_NEST := 1;

      INTERRUPT_TIME_TEST_SUPPORT.INSTALL_HANDLER( 
         TMTEST.ISR_HANDLER'ADDRESS 
      );

      TIMER_DRIVER.INITIALIZE;
         INTERRUPT_TIME_TEST_SUPPORT.CAUSE_INTERRUPT; 
      -- goes to ISR_HANDLER
         
      TMTEST.INTERRUPT_RETURN_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "INTERRUPT_ENTER (nested interrupt)",
         TMTEST.INTERRUPT_ENTER_NESTED_TIME, 
         1, 
         0,
         0
      );
 
      TIME_TEST_SUPPORT.PUT_TIME( 
         "INTERRUPT_RETURN (nested interrupt)",
         TMTEST.INTERRUPT_RETURN_NESTED_TIME, 
         1, 
         0,
         0
      );
 
      TMTEST.INTERRUPT_NEST := 0;
      
      RTEMS_TEST_SUPPORT.THREAD_DISPATCH_DISABLE_LEVEL := 0;
      
      TIMER_DRIVER.INITIALIZE;
         INTERRUPT_TIME_TEST_SUPPORT.CAUSE_INTERRUPT; 
      -- goes to ISR_HANDLER
         
      TMTEST.INTERRUPT_RETURN_TIME := TIMER_DRIVER.READ_TIMER;

      TIME_TEST_SUPPORT.PUT_TIME( 
         "INTERRUPT_ENTER (no preempt)",
         TMTEST.INTERRUPT_ENTER_TIME, 
         1, 
         0,
         0
      );
 
      TIME_TEST_SUPPORT.PUT_TIME( 
         "INTERRUPT_RETURN (no preempt)",
         TMTEST.INTERRUPT_RETURN_TIME, 
         1, 
         0,
         0
      );
 
      RTEMS_TEST_SUPPORT.THREAD_DISPATCH_DISABLE_LEVEL := 0;
      
      RTEMS_TEST_SUPPORT.THREAD_HEIR :=
          RTEMS_TEST_SUPPORT.THREAD_TO_CONTROL_POINTER(
             RTEMS_TEST_SUPPORT.CHAIN_NODE_POINTER_TO_ADDRESS(
                RTEMS_TEST_SUPPORT.THREAD_READY_CHAIN( 254 ).LAST
             )
          );

      RTEMS_TEST_SUPPORT.CONTEXT_SWITCH_NECESSARY := TRUE;

      TIMER_DRIVER.INITIALIZE;
         INTERRUPT_TIME_TEST_SUPPORT.CAUSE_INTERRUPT; 
      -- goes to ISR_HANDLER

   end TASK_1;

--PAGE
-- 
--  TASK_2
--

   procedure TASK_2 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
   begin
 
      TMTEST.INTERRUPT_RETURN_TIME := TIMER_DRIVER.READ_TIMER; 
      TIME_TEST_SUPPORT.PUT_TIME( 
         "INTERRUPT_ENTER (preempt)", 
         TMTEST.INTERRUPT_ENTER_TIME, 
         1, 
         0,
         0
      );
 
      TIME_TEST_SUPPORT.PUT_TIME( 
         "INTERRUPT_RETURN (preempt)",
         TMTEST.INTERRUPT_RETURN_TIME, 
         1, 
         0,
         0
      );

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end TASK_2;

--PAGE
-- 
--  ISR_HANDLER
--

--
--  The ISR_HANDLER and ISR_HANDLER_INNER subprograms are structured
--  so that there will be as little entry overhead as possible included
--  in the interrupt entry time.
--

   procedure ISR_HANDLER (
      VECTOR : in     RTEMS.VECTOR_NUMBER
   ) is
   begin

      TMTEST.END_TIME := TIMER_DRIVER.READ_TIMER; 

      TMTEST.ISR_HANDLER_INNER;

   end ISR_HANDLER;

-- 
--  ISR_HANDLER_INNER
--

   procedure ISR_HANDLER_INNER
   is
   begin

      -- enable tracing here is necessary

      INTERRUPT_TIME_TEST_SUPPORT.CLEAR_INTERRUPT;

      case TMTEST.INTERRUPT_NEST is
         when 0 =>
            TMTEST.INTERRUPT_ENTER_TIME := TMTEST.END_TIME;

         when 1 =>
            TMTEST.INTERRUPT_ENTER_TIME := TMTEST.END_TIME;
            TMTEST.INTERRUPT_NEST := 2;
            TIMER_DRIVER.INITIALIZE;
               INTERRUPT_TIME_TEST_SUPPORT.CAUSE_INTERRUPT; 
            -- goes to a nested copy of ISR_HANDLER
 
            TMTEST.INTERRUPT_RETURN_NESTED_TIME := TIMER_DRIVER.READ_TIMER; 

         when 2 =>
            TMTEST.INTERRUPT_ENTER_NESTED_TIME := TMTEST.END_TIME;

         when others =>
             NULL;

      end case;

      -- Start the timer so interrupt return times can be measured

      TIMER_DRIVER.INITIALIZE;
    
   end ISR_HANDLER_INNER;

end TMTEST;
