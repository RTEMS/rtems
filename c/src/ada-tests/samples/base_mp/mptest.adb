--
--  MPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation for Test 1 of the RTEMS
--  Multiprocessor Test Suite.
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
with TEST_SUPPORT;
with TEXT_IO;
with UNSIGNED32_IO;

package body MPTEST is

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
      TEXT_IO.PUT( "*** SAMPLE MULTIPROCESSOR APPLICATION ***" );
      TEXT_IO.PUT( "Creating and starting an application task" );


      MPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );

      RTEMS.TASK_CREATE( 
         MPTEST.TASK_NAME( 1 ), 
         1, 
         2048, 
         RTEMS.INTERRUPT_LEVEL( 0 ),
         RTEMS.DEFAULT_ATTRIBUTES,
         MPTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA1" );

      RTEMS.TASK_START(
         MPTEST.TASK_ID( 1 ),
         MPTEST.APPLICATION_TASK'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA1" );

      RTEMS.TASK_DELETE( RTEMS.SELF, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_DELETE OF SELF" );

   end INIT;

--PAGE
--
--  APPLICATION_TASK
--

   procedure APPLICATION_TASK (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      TID    : RTEMS.ID;
      STATUS : RTEMS.STATUS_CODES;
   begin

      RTEMS.TASK_IDENT( RTEMS.SELF, RTEMS.SEARCH_ALL_NODES, TID, STATUS );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_IDENT OF SELF" );
   
      TEXT_IO.PUT( "This task was invoked with node argument (" );
      UNSIGNED32_IO.PUT( ARGUMENT );
      TEXT_IO.PUT_LINE( ")" );

      TEXT_IO.PUT( "This task has the id of  0x" );
      UNSIGNED32_IO.PUT( TID, BASE => 16 );
      TEXT_IO.NEW_LINE;
 
      TEXT_IO.PUT_LINE( "*** END OF SAMPLE MULTIPROCESSOR APPLICATION  ***" );

      RTEMS.SHUTDOWN_EXECUTIVE( 0 );

   end APPLICATION_TASK;

end MPTEST;
