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
--  COPYRIGHT (c) 1989-2011.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

with TEST_SUPPORT;
with TEXT_IO;
with UNSIGNED32_IO;

package body SPTEST is

-- 
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      pragma Unreferenced(ARGUMENT);
      STATUS : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.NEW_LINE( 2 );
      TEXT_IO.PUT_LINE( "*** SAMPLE SINGLE PROCESSOR APPLICATION ***" );
      TEXT_IO.PUT_LINE( "Creating and starting an application task" );

      SPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );

      RTEMS.TASKS.CREATE( 
         SPTEST.TASK_NAME( 1 ), 
         1, 
         2048, 
         RTEMS.INTERRUPT_LEVEL( 0 ),
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA1" );

      RTEMS.TASKS.START(
         SPTEST.TASK_ID( 1 ),
         SPTEST.APPLICATION_TASK'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA1" );

      RTEMS.TASKS.DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

-- 
--  APPLICATION_TASK
--

   procedure APPLICATION_TASK (
      ARGUMENT : in     RTEMS.TASKS.ARGUMENT
   ) is
      TID    : RTEMS.ID;
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.TASKS.IDENT( RTEMS.SELF, RTEMS.SEARCH_ALL_NODES, TID, STATUS );
   
      TEXT_IO.PUT( "Application task was invoked with argument (" );
      UNSIGNED32_IO.PUT( ARGUMENT );
      TEXT_IO.PUT( ") and has id of 0x" );
      UNSIGNED32_IO.PUT( TID, BASE => 16 );
      TEXT_IO.NEW_LINE;

      TEXT_IO.PUT_LINE( "*** END OF SAMPLE SINGLE PROCESSOR APPLICATION ***" );

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end APPLICATION_TASK;

end SPTEST;
