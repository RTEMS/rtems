/*
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

#define TEST_INIT
#include "system.h"

#include <bsp.h>

rtems_task Task_1(
  rtems_task_argument argument
);

rtems_task Task_2(
  rtems_task_argument argument
);

volatile rtems_unsigned32 Interrupt_occurred;
volatile rtems_unsigned32 Interrupt_enter_time, Interrupt_enter_nested_time;
volatile rtems_unsigned32 Interrupt_return_time, Interrupt_return_nested_time;
rtems_unsigned32 Interrupt_nest;

rtems_isr Isr_handler(
  rtems_vector_number vector
);

/*
 *  INTERNAL RTEMS VARIABLES!!!
 */

extern rtems_unsigned32 _Thread_Dispatch_disable_level;
extern rtems_unsigned32 _Context_Switch_necessary;
extern Chain_Control *_Thread_Ready_chain;
extern rtems_tcb     *_Thread_Heir;

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 27 ***" );

  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '1', ' ' ),
    254,
    1024,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create Task_1" );

  status = rtems_task_start( Task_id[ 1 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start Task_1" );

  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '2', ' ' ),
    254,
    1024,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of Task_2" );

  status = rtems_task_start( Task_id[ 2 ], Task_2, 0 );
  directive_failed( status, "rtems_task_start of Task_2" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

rtems_task Task_1(
  rtems_task_argument argument
)
{
  _Thread_Dispatch_disable_level = 1;

  Interrupt_nest = 1;

  Install_tm27_vector( Isr_handler );

  Interrupt_occurred = 0;
  Timer_initialize();
    Cause_tm27_intr();
  /* goes to Isr_handler */

#if (MUST_WAIT_FOR_INTERRUPT == 1)
  while ( Interrupt_occurred == 0 );
#endif
  Interrupt_return_time = Read_timer();

  put_time(
    "INTERRUPT_ENTER (nested interrupt)",
    Interrupt_enter_nested_time,
    1,
    0,
    0
  );

  put_time(
    "INTERRUPT_RETURN (nested interrupt)",
    Interrupt_return_nested_time,
    1,
    0,
    0
  );

  Interrupt_nest = 0;

  _Thread_Dispatch_disable_level = 0;

  Interrupt_occurred = 0;
  Timer_initialize();
    Cause_tm27_intr();
  /* goes to Isr_handler */

#if (MUST_WAIT_FOR_INTERRUPT == 1)
  while ( Interrupt_occurred == 0 );
#endif
  Interrupt_return_time = Read_timer();

  put_time(
    "INTERRUPT_ENTER (no preempt)",
    Interrupt_enter_time,
    1,
    0,
    0
  );

  put_time(
    "INTERRUPT_RETURN (no preempt)",
    Interrupt_return_time,
    1,
    0,
    0
  );

  _Thread_Dispatch_disable_level = 0;

  _Thread_Heir = (rtems_tcb *) _Thread_Ready_chain[254].last;

  _Context_Switch_necessary = 1;

  Interrupt_occurred = 0;
  Timer_initialize();
    Cause_tm27_intr();
  /* goes to Isr_handler */
}

rtems_task Task_2(
  rtems_task_argument argument
)
{
#if (MUST_WAIT_FOR_INTERRUPT == 1)
  while ( Interrupt_occurred == 0 );
#endif
  end_time = Read_timer();

  put_time(
    "INTERRUPT_ENTER (preempt)",
    Interrupt_enter_time,
    1,
    0,
    0
  );

  put_time(
    "INTERRUPT_RETURN (preempt)",
    end_time,
    1,
    0,
    0
  );

  puts( "*** END OF TEST 27 ***" );
  exit( 0 );
}

/*  The Isr_handler() and Isr_handler_inner() routines are structured
 *  so that there will be as little entry overhead as possible included
 *  in the interrupt entry time.
 */

void Isr_handler_inner( void );

rtems_isr Isr_handler(
  rtems_vector_number vector
)
{
  end_time = Read_timer();

  Interrupt_occurred = 1;
  Isr_handler_inner();
}

void Isr_handler_inner( void )
{

  /*enable_tracing();*/
  Clear_tm27_intr();
  switch ( Interrupt_nest ) {
    case 0:
      Interrupt_enter_time = end_time;
      break;
    case 1:
      Interrupt_enter_time = end_time;
      Interrupt_nest = 2;
      Interrupt_occurred = 0;
      Lower_tm27_intr();
      Timer_initialize();
        Cause_tm27_intr();
      /* goes to a nested copy of Isr_handler */
#if (MUST_WAIT_FOR_INTERRUPT == 1)
       while ( Interrupt_occurred == 0 );
#endif
      Interrupt_return_nested_time = Read_timer();
      break;
    case 2:
      Interrupt_enter_nested_time = end_time;
      break;
  }

  Timer_initialize();
}
