--
--  SPTEST / BODY
--
--  DESCRIPTION:
--
--  This package is the implementation of Test 8 of the RTEMS
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
--  http://www.OARcorp.com/rtems/license.html.
--
--  $Id$
--

with INTERFACES; use INTERFACES;
with RTEMS;
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
      TEXT_IO.PUT_LINE( "*** TEST 8 ***" );

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
--  PUT_MODE
--

   procedure PUT_MODE(
      COMMENT     : in    STRING;
      OUTPUT_MODE : in    RTEMS.MODE
   ) is
   begin

      TEXT_IO.PUT( COMMENT );
      UNSIGNED32_IO.PUT( OUTPUT_MODE, BASE => 16, WIDTH => 8 );
      TEXT_IO.NEW_LINE;

   end PUT_MODE;

--PAGE
-- 
--  TASK_1
--

   procedure TASK_1 (
      ARGUMENT : in     RTEMS.TASK_ARGUMENT
   ) is
      PREVIOUS_MODE : RTEMS.MODE;
      STATUS : RTEMS.STATUS_CODES;
   begin

-- BEGINNING OF ASR

      RTEMS.TASK_MODE( 
         RTEMS.ASR, 
         RTEMS.ASR_MASK, 
         PREVIOUS_MODE, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );
      SPTEST.PUT_MODE(
         "TA1 - task_mode - ASR                  - previous mode: ", 
         PREVIOUS_MODE
      );
   
      RTEMS.TASK_MODE( 
         RTEMS.NO_ASR, 
         RTEMS.ASR_MASK, 
         PREVIOUS_MODE, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );
      SPTEST.PUT_MODE(
         "TA1 - task_mode - NO_ASR               - previous mode: ", 
         PREVIOUS_MODE
      );
   
      RTEMS.TASK_MODE( 
         RTEMS.NO_ASR, 
         RTEMS.ASR_MASK, 
         PREVIOUS_MODE, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );
      SPTEST.PUT_MODE(
         "TA1 - task_mode - NO_ASR               - previous mode: ", 
         PREVIOUS_MODE
      );
   
      RTEMS.TASK_MODE( 
         RTEMS.ASR, 
         RTEMS.ASR_MASK, 
         PREVIOUS_MODE, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );
      SPTEST.PUT_MODE(
         "TA1 - task_mode - ASR                  - previous mode: ", 
         PREVIOUS_MODE
      );
   
-- END OF ASR

-- BEGINNING OF TIMESLICE

      RTEMS.TASK_MODE( 
         RTEMS.NO_TIMESLICE, 
         RTEMS.TIMESLICE_MASK, 
         PREVIOUS_MODE, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );
      SPTEST.PUT_MODE(
         "TA1 - task_mode - NO_TIMESLICE         - previous mode: ", 
         PREVIOUS_MODE
      );
   
      RTEMS.TASK_MODE( 
         RTEMS.TIMESLICE, 
         RTEMS.TIMESLICE_MASK, 
         PREVIOUS_MODE, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );
      SPTEST.PUT_MODE(
         "TA1 - task_mode - TIMESLICE            - previous mode: ", 
         PREVIOUS_MODE
      );
   
      RTEMS.TASK_MODE( 
         RTEMS.TIMESLICE, 
         RTEMS.TIMESLICE_MASK, 
         PREVIOUS_MODE, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );
      SPTEST.PUT_MODE(
         "TA1 - task_mode - TIMESLICE            - previous mode: ", 
         PREVIOUS_MODE
      );
   
      RTEMS.TASK_MODE( 
         RTEMS.NO_TIMESLICE, 
         RTEMS.TIMESLICE_MASK, 
         PREVIOUS_MODE, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );
      SPTEST.PUT_MODE(
         "TA1 - task_mode - NO_TIMESLICE         - previous mode: ", 
         PREVIOUS_MODE
      );
   
-- END OF TIMESLICE

-- BEGINNING OF PREEMPT

      RTEMS.TASK_MODE( 
         RTEMS.PREEMPT, 
         RTEMS.PREEMPT_MASK, 
         PREVIOUS_MODE, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );
      SPTEST.PUT_MODE(
         "TA1 - task_mode - PREEMPT              - previous mode: ", 
         PREVIOUS_MODE
      );
   
      RTEMS.TASK_MODE( 
         RTEMS.NO_PREEMPT, 
         RTEMS.PREEMPT_MASK, 
         PREVIOUS_MODE, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );
      SPTEST.PUT_MODE(
         "TA1 - task_mode - NO_PREEMPT           - previous mode: ", 
         PREVIOUS_MODE
      );
   
      RTEMS.TASK_MODE( 
         RTEMS.NO_PREEMPT, 
         RTEMS.PREEMPT_MASK, 
         PREVIOUS_MODE, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );
      SPTEST.PUT_MODE(
         "TA1 - task_mode - NO_PREEMPT           - previous mode: ", 
         PREVIOUS_MODE
      );
   
      RTEMS.TASK_MODE( 
         RTEMS.PREEMPT, 
         RTEMS.PREEMPT_MASK, 
         PREVIOUS_MODE, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );
      SPTEST.PUT_MODE(
         "TA1 - task_mode - PREEMPT              - previous mode: ", 
         PREVIOUS_MODE
      );
   
-- END OF PREEMPT

-- BEGINNING OF INTERRUPT LEVEL

      RTEMS.TASK_MODE( 
         RTEMS.INTERRUPT_LEVEL( 3 ), 
         RTEMS.INTERRUPT_MASK, 
         PREVIOUS_MODE, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );
      SPTEST.PUT_MODE(
         "TA1 - task_mode - INTERRUPT_LEVEL( 3 ) - previous mode: ", 
         PREVIOUS_MODE
      );
   
      RTEMS.TASK_MODE( 
         RTEMS.INTERRUPT_LEVEL( 5 ), 
         RTEMS.INTERRUPT_MASK, 
         PREVIOUS_MODE, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );
      SPTEST.PUT_MODE(
         "TA1 - task_mode - INTERRUPT_LEVEL( 5 ) - previous mode: ", 
         PREVIOUS_MODE
      );
   
-- END OF INTERRUPT LEVEL

-- BEGINNING OF COMBINATIONS

      RTEMS.TASK_MODE( 
         RTEMS.INTERRUPT_LEVEL( 3 ) + RTEMS.NO_ASR + 
            RTEMS.TIMESLICE + RTEMS.NO_PREEMPT, 
         RTEMS.INTERRUPT_MASK + RTEMS.ASR_MASK + 
            RTEMS.TIMESLICE_MASK + RTEMS.PREEMPT_MASK, 
         PREVIOUS_MODE, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );
      SPTEST.PUT_MODE(
         "TA1 - task_mode - set all modes        - previous mode: ", 
         PREVIOUS_MODE
      );
   
      RTEMS.TASK_MODE( 
         RTEMS.INTERRUPT_LEVEL( 3 ) + RTEMS.NO_ASR + 
            RTEMS.TIMESLICE + RTEMS.NO_PREEMPT, 
         RTEMS.INTERRUPT_MASK + RTEMS.ASR_MASK + 
            RTEMS.TIMESLICE_MASK + RTEMS.PREEMPT_MASK, 
         PREVIOUS_MODE, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );
      SPTEST.PUT_MODE(
         "TA1 - task_mode - set all modes        - previous mode: ", 
         PREVIOUS_MODE
      );
   
      RTEMS.TASK_MODE( 
         RTEMS.INTERRUPT_LEVEL( 0 ) + RTEMS.ASR + 
            RTEMS.NO_TIMESLICE + RTEMS.PREEMPT, 
         RTEMS.INTERRUPT_MASK + RTEMS.ASR_MASK + 
            RTEMS.TIMESLICE_MASK + RTEMS.PREEMPT_MASK, 
         PREVIOUS_MODE, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );
      SPTEST.PUT_MODE(
         "TA1 - task_mode - clear all modes      - previous mode: ", 
         PREVIOUS_MODE
      );
   
-- END OF COMBINATIONS

-- BEGINNING OF CURRENT MODE

      RTEMS.TASK_MODE( 
         RTEMS.CURRENT_MODE,
         RTEMS.CURRENT_MODE,
         PREVIOUS_MODE, 
         STATUS 
      );
      TEST_SUPPORT.DIRECTIVE_FAILED( STATUS, "TASK_MODE" );
      SPTEST.PUT_MODE(
         "TA1 - task_mode - get current mode     - previous mode: ", 
         PREVIOUS_MODE
      );
   
-- END OF CURRENT MODE

      TEXT_IO.PUT_LINE( "*** END OF TEST 8 ***" );
      RTEMS.SHUTDOWN_EXECUTIVE( 0 );
   end TASK_1;

end SPTEST;
