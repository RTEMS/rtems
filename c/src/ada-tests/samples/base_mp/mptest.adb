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
--  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
--  On-Line Applications Research Corporation (OAR).
--  All rights assigned to U.S. Government, 1994.
--
--  This material may be reproduced by or for the U.S. Government pursuant
--  to the copyright license under the clause at DFARS 252.227-7013.  This
--  notice must appear in all copies of this file and its derivatives.
--
--  mptest.adb,v 1.3 1995/07/12 19:40:36 joel Exp
--

with INTERFACES; use INTERFACES;
with RTEMS;
with TEST_SUPPORT;
with TEXT_IO;
with UNSIGNED32_IO;

package body MPTEST is

   package body PER_NODE_CONFIGURATION is separate;

--PAGE
--
--  INIT
--

   procedure INIT (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      C      : RTEMS.CHARACTER;
      TIME   : RTEMS.TIME_OF_DAY;
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
      TIME   : RTEMS.TIME_OF_DAY;
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
