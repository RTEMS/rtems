--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 21 of the RTEMS
--  Single Processor Test Suite.
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
--  sptest.adb,v 1.3 1995/07/12 19:42:28 joel Exp
--

with INTERFACES; use INTERFACES;
with RTEMS;
with TEST_SUPPORT;
with TEXT_IO;

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
      TEXT_IO.PUT_LINE( "*** TEST 21 ***" );

      SPTEST.TASK_NAME( 1 ) := RTEMS.BUILD_NAME(  'T', 'A', '1', ' ' );

      RTEMS.TASK_CREATE( 
         SPTEST.TASK_NAME( 1 ), 
         1, 
         2048, 
         RTEMS.DEFAULT_MODES,
         RTEMS.DEFAULT_ATTRIBUTES,
         SPTEST.TASK_ID( 1 ),
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_CREATE OF TA1" );

      RTEMS.TASK_START(
         SPTEST.TASK_ID( 1 ),
         SPTEST.TASK_1'ACCESS,
         0,
         STATUS
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_START OF TA1" );

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
      RETURN_VALUE : RTEMS.UNSIGNED32;
      STATUS       : RTEMS.STATUS_CODES;
   begin

      TEXT_IO.PUT_LINE( "----- TESTING THE NULL DRIVER CHECKS -----" );

      RTEMS.IO_INITIALIZE( 
         SPTEST.NO_DRIVER_MAJOR,
         0,
         RTEMS.NULL_ADDRESS,
         RETURN_VALUE,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "IO_INITIALIZE" );
      TEXT_IO.PUT_LINE(
         "TA1 - io_initialize - NULL DRIVER SUCCESSFUL"
      );
   
      RTEMS.IO_OPEN( 
         SPTEST.NO_DRIVER_MAJOR,
         0,
         RTEMS.NULL_ADDRESS,
         RETURN_VALUE,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "IO_OPEN" );
      TEXT_IO.PUT_LINE(
         "TA1 - io_open       - NULL DRIVER SUCCESSFUL"
      );
   
      RTEMS.IO_CLOSE( 
         SPTEST.NO_DRIVER_MAJOR,
         0,
         RTEMS.NULL_ADDRESS,
         RETURN_VALUE,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "IO_CLOSE" );
      TEXT_IO.PUT_LINE(
         "TA1 - io_close      - NULL DRIVER SUCCESSFUL"
      );
   
      RTEMS.IO_READ( 
         SPTEST.NO_DRIVER_MAJOR,
         0,
         RTEMS.NULL_ADDRESS,
         RETURN_VALUE,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "IO_READ" );
      TEXT_IO.PUT_LINE(
         "TA1 - io_read       - NULL DRIVER SUCCESSFUL"
      );
   
      RTEMS.IO_WRITE( 
         SPTEST.NO_DRIVER_MAJOR,
         0,
         RTEMS.NULL_ADDRESS,
         RETURN_VALUE,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "IO_WRITE" );
      TEXT_IO.PUT_LINE(
         "TA1 - io_write      - NULL DRIVER SUCCESSFUL"
      );
   
      RTEMS.IO_CONTROL( 
         SPTEST.NO_DRIVER_MAJOR,
         0,
         RTEMS.NULL_ADDRESS,
         RETURN_VALUE,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "IO_CONTROL" );
      TEXT_IO.PUT_LINE(
         "TA1 - io_control    - NULL DRIVER SUCCESSFUL"
      );
   
      TEXT_IO.PUT_LINE( "----- TESTING THE I/O MANAGER DIRECTIVES -----" );

      RTEMS.IO_INITIALIZE( 
         SPTEST.STUB_DRIVER_MAJOR,
         0,
         RTEMS.NULL_ADDRESS,
         RETURN_VALUE,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "IO_INITIALIZE" );
      TEXT_IO.PUT_LINE(
         "TA1 - io_initialize - STUB DRIVER SUCCESSFUL"
      );
   
      RTEMS.IO_OPEN( 
         SPTEST.STUB_DRIVER_MAJOR,
         0,
         RTEMS.NULL_ADDRESS,
         RETURN_VALUE,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "IO_OPEN" );
      TEXT_IO.PUT_LINE(
         "TA1 - io_open       - STUB DRIVER SUCCESSFUL"
      );
   
      RTEMS.IO_CLOSE( 
         SPTEST.STUB_DRIVER_MAJOR,
         0,
         RTEMS.NULL_ADDRESS,
         RETURN_VALUE,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "IO_CLOSE" );
      TEXT_IO.PUT_LINE(
         "TA1 - io_close      - STUB DRIVER SUCCESSFUL"
      );
   
      RTEMS.IO_READ( 
         SPTEST.STUB_DRIVER_MAJOR,
         0,
         RTEMS.NULL_ADDRESS,
         RETURN_VALUE,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "IO_READ" );
      TEXT_IO.PUT_LINE(
         "TA1 - io_read       - STUB DRIVER SUCCESSFUL"
      );
   
      RTEMS.IO_WRITE( 
         SPTEST.STUB_DRIVER_MAJOR,
         0,
         RTEMS.NULL_ADDRESS,
         RETURN_VALUE,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "IO_WRITE" );
      TEXT_IO.PUT_LINE(
         "TA1 - io_write      - STUB DRIVER SUCCESSFUL"
      );
   
      RTEMS.IO_CONTROL( 
         SPTEST.STUB_DRIVER_MAJOR,
         0,
         RTEMS.NULL_ADDRESS,
         RETURN_VALUE,
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "IO_CONTROL" );
      TEXT_IO.PUT_LINE(
         "TA1 - io_control    - STUB DRIVER SUCCESSFUL"
      );

      TEXT_IO.PUT_LINE( "----- RETURNING INVALID MAJOR NUMBER -----" );

      RTEMS.IO_INITIALIZE( 
         SPTEST.INVALID_DRIVER_MAJOR,
         0,
         RTEMS.NULL_ADDRESS,
         RETURN_VALUE,
         STATUS 
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS( 
         STATUS,
         RTEMS.INVALID_NUMBER, 
         "IO_INITIALIZE" 
      );
      TEXT_IO.PUT_LINE(
         "TA1 - io_initialize - INVALID_NUMBER"
      );
   
      RTEMS.IO_OPEN( 
         SPTEST.INVALID_DRIVER_MAJOR,
         0,
         RTEMS.NULL_ADDRESS,
         RETURN_VALUE,
         STATUS 
      );
      TEST_SUPPORT.FATAL_DIRECTIVE_STATUS( 
         STATUS,
         RTEMS.INVALID_NUMBER, 
         "IO_OPEN" 
      );
      TEXT_IO.PUT_LINE(
         "TA1 - io_open       - INVALID_NUMBER"
      );
   
      TEXT_IO.PUT_LINE( "*** END OF TEST 21 ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );
   
   end TASK_1;

end SPTEST;
