/*  Task_1
 *
 *  This routine serves as a test task.  It verifies all combinations of
 *  task mode changes.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include "system.h"

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_mode        previous_mode;
  rtems_status_code status;

/* BEGINNING OF RTEMS_ASR */

  status = rtems_task_mode( RTEMS_ASR, RTEMS_ASR_MASK, &previous_mode );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_ASR                  - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
    RTEMS_NO_ASR,
    RTEMS_ASR_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_NO_ASR               - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
    RTEMS_NO_ASR,
    RTEMS_ASR_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_NO_ASR               - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
    RTEMS_ASR,
    RTEMS_ASR_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_ASR                  - previous mode: ",
    previous_mode
  );

/* END OF RTEMS_ASR */

/* BEGINNING OF RTEMS_TIMESLICE */

  status = rtems_task_mode(
    RTEMS_NO_TIMESLICE,
    RTEMS_TIMESLICE_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_NO_TIMESLICE         - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
    RTEMS_TIMESLICE,
    RTEMS_TIMESLICE_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_TIMESLICE            - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
    RTEMS_TIMESLICE,
    RTEMS_TIMESLICE_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_TIMESLICE            - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
    RTEMS_NO_TIMESLICE,
    RTEMS_TIMESLICE_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_NO_TIMESLICE         - previous mode: ",
    previous_mode
  );

/* END OF RTEMS_TIMESLICE */

/* BEGINNING OF RTEMS_PREEMPT */

  status = rtems_task_mode(
    RTEMS_PREEMPT,
    RTEMS_PREEMPT_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_PREEMPT              - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
    RTEMS_NO_PREEMPT,
    RTEMS_PREEMPT_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_NO_PREEMPT           - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
    RTEMS_NO_PREEMPT,
    RTEMS_PREEMPT_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_NO_PREEMPT           - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
    RTEMS_PREEMPT,
    RTEMS_PREEMPT_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_PREEMPT              - previous mode: ",
    previous_mode
  );

/* END OF RTEMS_PREEMPT */

/* BEGINNING OF RTEMS_INTERRUPT_LEVEL */

  status = rtems_task_mode(
    RTEMS_INTERRUPT_LEVEL(3),
    RTEMS_INTERRUPT_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_INTERRUPT_LEVEL( 3 ) - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
    RTEMS_INTERRUPT_LEVEL(5),
    RTEMS_INTERRUPT_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - RTEMS_INTERRUPT_LEVEL( 5 ) - previous mode: ",
    previous_mode
  );

/* END OF RTEMS_INTERRUPT_LEVEL */

/* BEGINNING OF COMBINATIONS */

  status = rtems_task_mode(
    RTEMS_INTERRUPT_LEVEL(3) | RTEMS_NO_ASR |
      RTEMS_TIMESLICE | RTEMS_NO_PREEMPT,
    RTEMS_INTERRUPT_MASK | RTEMS_ASR_MASK |
      RTEMS_TIMESLICE_MASK | RTEMS_PREEMPT_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - set all modes        - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
    RTEMS_INTERRUPT_LEVEL(3) | RTEMS_NO_ASR |
      RTEMS_TIMESLICE | RTEMS_NO_PREEMPT,
    RTEMS_INTERRUPT_MASK | RTEMS_ASR_MASK |
      RTEMS_TIMESLICE_MASK | RTEMS_PREEMPT_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - set all modes        - previous mode: ",
    previous_mode
  );

  status = rtems_task_mode(
    RTEMS_INTERRUPT_LEVEL(0) | RTEMS_ASR | RTEMS_NO_TIMESLICE | RTEMS_PREEMPT,
    RTEMS_INTERRUPT_MASK | RTEMS_ASR_MASK |
    RTEMS_TIMESLICE_MASK | RTEMS_PREEMPT_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - clear all modes      - previous mode: ",
    previous_mode
  );

/* END OF COMBINATIONS */

/* BEGINNING OF CURRENT MODE */

  status = rtems_task_mode(
    RTEMS_CURRENT_MODE,
    RTEMS_CURRENT_MODE,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode" );
  put_mode(
    "TA1 - rtems_task_mode - get current mode     - previous mode: ",
    previous_mode
  );

/* END OF CURRENT MODE */

  puts( "*** END OF TEST 8 ***" );
  exit( 0 );
}
