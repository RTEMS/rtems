/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is called from init_exec and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define TEST_INIT
#include "system.h"
#include <timesys.h>
#include <stdio.h>

void ITRON_Init( void )
{
  T_CSEM  pk_csem;
  T_RSEM  pk_rsem;
  int     index;

  pk_csem.isemcnt = 2;
  pk_csem.maxsem  = 3;
  pk_csem.sematr  = TA_TFIFO;

  printf( "\n\n*** TMITRON01 -- ITRON TIMING TEST ***\n" );

  /*
   *  time one invocation of cre_sem
   */

  Timer_initialize();
    (void) cre_sem( 1, &pk_csem );
  end_time = Read_timer();
  put_time(
    "cre_sem",
    end_time,
    1,
    0,  /* no loop overhead to take out */
    0   /* no calling overhead */
  );

  /*
   *  time one invocation of del_sem
   */

  Timer_initialize(); 
    (void) del_sem( 1 );
  end_time = Read_timer();
  put_time(
    "del_sem",
    end_time,
    1,
    0,  /* no loop overhead to take out */
    0   /* no calling overhead */
  );

  pk_csem.isemcnt = 100;
  pk_csem.maxsem  = 100;
  pk_csem.sematr  = TA_TFIFO;
  (void) cre_sem( 1, &pk_csem );

  /*
   *  time preq_sem (available)
   */

  Timer_initialize();
    for ( index = 1 ; index<=OPERATION_COUNT ; index++ )
      (void) preq_sem( 1 );
  end_time = Read_timer();
  put_time(
    "preq_sem (available)",
    end_time,
    OPERATION_COUNT,
    0,  /* don't take the loop overhead out */
    0   /* no calling overhead */
  );

  /*
   *  time preq_sem (not available)
   */

  Timer_initialize();
    for ( index = 1 ; index<=OPERATION_COUNT ; index++ )
      (void) preq_sem( 1 );
  end_time = Read_timer();
  put_time(
    "preq_sem (not available)",
    end_time,
    OPERATION_COUNT,
    0,  /* don't take the loop overhead out */
    0   /* no calling overhead */
  );

  /*
   *  time sig_sem (no waiting tasks)
   */
  
  Timer_initialize(); 
    for ( index = 1 ; index<=OPERATION_COUNT ; index++ )
      (void) sig_sem( 1 );
  end_time = Read_timer();
  put_time(
    "sig_sem (no waiting tasks)",
    end_time,
    OPERATION_COUNT,
    0,  /* don't take the loop overhead out */
    0   /* no calling overhead */
  );

  /*
   *  time ref_sem
   */

  Timer_initialize();
    for ( index = 1 ; index<=OPERATION_COUNT ; index++ )
      (void) ref_sem( 1, &pk_rsem );
  end_time = Read_timer();
  put_time(
    "ref_sem",
    end_time,
    OPERATION_COUNT,
    0,  /* don't take the loop overhead out */
    0   /* no calling overhead */
  );

  printf( "\n\nXXX ADD BLOCKING AND PREEMPTING TIME TESTS\n\n" );


  printf( "*** END OF TMITRON01 -- ITRON TIMING TEST ***\n" );
  rtems_test_exit( 0 );
}
