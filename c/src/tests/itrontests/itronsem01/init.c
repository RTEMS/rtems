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
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#define TEST_INIT
#include "system.h"
#include <stdio.h>

/*
 *  The following is hopefully temporary.
 */

#include <time.h>

#define put_time(_s) \
  do { \
    time_t a_time_t; \
    a_time_t = time( 0 ); \
    printf( "%s%s", _s, ctime( &a_time_t ) ); \
  } while(0)

void ITRON_Init( void )
{
  ER      status;
  T_CSEM  pk_csem;
  T_RSEM  pk_rsem;

  printf( "\n\n*** ITRONSEM01 -- ITRON SEMAPHORE TEST ***\n" );

  /*
   *  Exercise cre_sem - this code seems to exercise every path.
   */

  puts( "Init - cre_sem - NULL pk_sem returns E_PAR" );
  status = cre_sem( 1, NULL );
  assert( status == E_PAR );

  puts( "Init - cre_sem - isemcnt > maxsem returns E_PAR" );
  pk_csem.sematr = 0;
  pk_csem.isemcnt = 3;
  pk_csem.maxsem  = 2;
  status = cre_sem( 1, &pk_csem );
  assert( status == E_PAR );

  puts( "Init - cre_sem - maxsem of 0 returns E_PAR" );
  pk_csem.isemcnt = 0;
  pk_csem.maxsem  = 0;
  status = cre_sem( 1, &pk_csem );
  assert( status == E_PAR );

  /*
   *  Now run through all the bad ID errors
   */

  pk_csem.isemcnt = 2;
  pk_csem.maxsem  = 3;
  puts( "Init - cre_sem - bad id (less than -4) - E_OACV" );
  status = cre_sem( -5, &pk_csem );
  assert( status == E_OACV );

  puts( "Init - cre_sem - bad id (between 0 and -4) - E_ID" );
  status = cre_sem( -4, &pk_csem );
  assert( status == E_ID );

  puts( "Init - cre_sem - bad id (0) - E_ID" );
  status = cre_sem( 0, &pk_csem );
  assert( status == E_ID );

  puts( "Init - cre_sem - bad id (too great) - E_ID" );
  status = cre_sem( CONFIGURE_MAXIMUM_ITRON_SEMAPHORES + 1, &pk_csem );
  assert( status == E_ID );

  puts( "Init - cre_sem - create semaphore 1 TA_TFIFO - E_OK" );
  pk_csem.isemcnt = 2;
  pk_csem.maxsem  = 3;
  pk_csem.sematr  = TA_TFIFO;
  status = cre_sem( 1, &pk_csem );
  assert( status == E_OK );

  puts( "Init - cre_sem - create semaphore 1 again - E_OBJ" );
  status = cre_sem( 1, &pk_csem );
  assert( status == E_OBJ );

  puts( "Init - cre_sem - create semaphore 2 TA_TPRI - E_OK" );
  pk_csem.isemcnt = 2;
  pk_csem.maxsem  = 3;
  pk_csem.sematr  = TA_TPRI;
  status = cre_sem( 2, &pk_csem );
  assert( status == E_OK );

  /*
   *  Generate all the bad id errors for del_sem
   */

  puts( "Init - del_sem - bad id (less than -4) - E_OACV" );
  status = del_sem( -5 );
  assert( status == E_OACV );

  puts( "Init - del_sem - bad id (between 0 and -4) - E_ID" );
  status = del_sem( -4 );
  assert( status == E_ID );

  puts( "Init - del_sem - bad id (0) - E_ID" );
  status = del_sem( 0 );
  assert( status == E_ID );

  puts( "Init - del_sem - bad id (too great) - E_ID" );
  status = del_sem( CONFIGURE_MAXIMUM_ITRON_SEMAPHORES + 1 );
  assert( status == E_ID );

  /*
   *  Generate all the bad id errors for sig_sem
   */

  puts( "Init - sig_sem - bad id (less than -4) - E_OACV" );
  status = sig_sem( -5 );
  assert( status == E_OACV );

  puts( "Init - sig_sem - bad id (between 0 and -4) - E_ID" );
  status = sig_sem( -4 );
  assert( status == E_ID );

  puts( "Init - sig_sem - bad id (0) - E_ID" );
  status = sig_sem( 0 );
  assert( status == E_ID );

  puts( "Init - sig_sem - bad id (too great) - E_ID" );
  status = wai_sem( CONFIGURE_MAXIMUM_ITRON_SEMAPHORES + 1 );
  assert( status == E_ID );

  /*
   *  Generate all the bad id errors for preq_sem
   */

  puts( "Init - preq_sem - bad id (less than -4) - E_OACV" );
  status = preq_sem( -5 );
  assert( status == E_OACV ); 
  
  puts( "Init - preq_sem - bad id (between 0 and -4) - E_ID" );
  status = preq_sem( -4 );
  assert( status == E_ID );

  puts( "Init - preq_sem - bad id (0) - E_ID" );
  status = preq_sem( 0 );
  assert( status == E_ID );

  puts( "Init - preq_sem - bad id (too great) - E_ID" );
  status = preq_sem( CONFIGURE_MAXIMUM_ITRON_SEMAPHORES + 1 );
  assert( status == E_ID );

  /*
   *  Generate all the bad id errors for wai_sem
   */

  puts( "Init - wai_sem - bad id (less than -4) - E_OACV" );
  status = wai_sem( -5 );
  assert( status == E_OACV );

  puts( "Init - wai_sem - bad id (between 0 and -4) - E_ID" );
  status = wai_sem( -4 );
  assert( status == E_ID );

  puts( "Init - wai_sem - bad id (0) - E_ID" );
  status = wai_sem( 0 );
  assert( status == E_ID );

  puts( "Init - wai_sem - bad id (too great) - E_ID" );
  status = wai_sem( CONFIGURE_MAXIMUM_ITRON_SEMAPHORES + 1 );
  assert( status == E_ID );

  /*
   *  Generate all the bad id errors for twai_sem
   */

  puts( "Init - twai_sem - bad id (less than -4) - E_OACV" );
  status = twai_sem( -5, 1000 );
  assert( status == E_OACV );

  puts( "Init - twai_sem - bad id (between 0 and -4) - E_ID" );
  status = twai_sem( -4, 1000 );
  assert( status == E_ID );

  puts( "Init - twai_sem - bad id (0) - E_ID" );
  status = twai_sem( 0, 1000 );
  assert( status == E_ID );

  puts( "Init - twai_sem - bad id (too great) - E_ID" );
  status = twai_sem( CONFIGURE_MAXIMUM_ITRON_SEMAPHORES + 1, 1000 );
  assert( status == E_ID );

  /*
   *  Generate all the bad id errors for ref_sem
   */

  puts( "Init - ref_sem - bad id (less than -4) - E_OACV" );
  status = ref_sem( -5, &pk_rsem );
  assert( status == E_OACV );

  puts( "Init - ref_sem - bad id (between 0 and -4) - E_ID" );
  status = ref_sem( -4, &pk_rsem );
  assert( status == E_ID );

  puts( "Init - ref_sem - bad id (0) - E_ID" );
  status = ref_sem( 0, &pk_rsem );
  assert( status == E_ID );

  puts( "Init - ref_sem - bad id (too great) - E_ID" );
  status = ref_sem( CONFIGURE_MAXIMUM_ITRON_SEMAPHORES + 1, &pk_rsem );
  assert( status == E_ID );

  /*
   *  Exercise preq_sem (and sig_sem a little)
   */


  puts( "Init - preq_sem - semaphore 1 available (count --> 1) - E_OK" );
  status = preq_sem( 1 );
  assert( status == E_OK );

  puts( "Init - preq_sem - semaphore 1 available (count --> 0) - E_OK" );
  status = preq_sem( 1 );
  assert( status == E_OK );

  puts( "Init - preq_sem - semaphore 1 unavailable - E_TMOUT" );
  status = preq_sem( 1 );
  assert( status == E_TMOUT );

  puts( "Init - sig_sem - semaphore 1 (count --> 1) - E_OK" );
  status = sig_sem( 1 );
  assert( status == E_OK );

  puts("Init - twai_sem(TMO_POL) - semaphore 1 available (count --> 0) - E_OK");
  status = twai_sem( 1, TMO_POL );
  assert( status == E_OK );

  puts( "Init - twai_sem(TMO_POL) - semaphore 1 available - E_TMOUT" );
  status = twai_sem( 1, TMO_POL );
  assert( status == E_TMOUT );

  /*
   *  Exercise twai_sem 
   */

  puts( "Init - twai_sem - semaphore 1 unavailable - 1 second E_TMOUT" );
  put_time( "Init - starting to block at ");
  status = twai_sem( 1, 1000 );
  assert( status == E_TMOUT );
  put_time( "Init - timed out at ");
  puts( "Init - twai_sem - semaphore 1 unavailable - timeout completed" );

  /*
   *  Now let's test exceed the maximum count of semaphore 1
   */

  puts( "Init - sig_sem - semaphore 1 (count --> 1) - E_OK" );
  status = sig_sem( 1 );
  assert( status == E_OK );

  puts( "Init - sig_sem - semaphore 1 (count --> 2) - E_OK" );
  status = sig_sem( 1 );
  assert( status == E_OK );

  puts( "Init - sig_sem - semaphore 1 (count --> 3) - E_OK" );
  status = sig_sem( 1 );
  assert( status == E_OK );

  puts( "Init - sig_sem - semaphore 1 (count > maximum) - E_QOVR" );
  status = sig_sem( 1 );
  assert( status == E_OK );

  /*
   *  Exercise ref_sem
   */

  puts( "Init - ref_sem - NULL pk_sem returns E_PAR" );
  status = ref_sem( 1, NULL );
  assert( status == E_PAR );

  puts( "Init - ref_sem - semaphore 1 - E_OK" );
  status = ref_sem( 1, &pk_rsem );
  assert( status == E_OK );
  printf( "Init - semaphore 1 count = %d\n", pk_rsem.semcnt );
  printf( "Init - semaphore 1 waiting tasks = %d\n", pk_rsem.wtsk );

  puts( "Init - preq_sem - semaphore 1 (count --> 2) - E_OK" );
  status = preq_sem( 1 );
  assert( status == E_OK );

  puts( "Init - ref_sem - semaphore 1 - E_OK" );
  status = ref_sem( 1, &pk_rsem );
  assert( status == E_OK );
  printf( "Init - semaphore 1 count = %d\n", pk_rsem.semcnt );
  printf( "Init - semaphore 1 waiting tasks = %d\n", pk_rsem.wtsk );


  /*
   *  Exercise del_sem
   */

  /* XXX really want another task blocking on the semaphore XXX */

  puts( "Init - del_sem - semaphore 1 deleted - E_OK" );
  status = del_sem( 1 );
  assert( status == E_OK );

  puts( "Init - XXX need other tasks to complete the test XXX" );

  puts( "Init - dly_tsk - 1 second to let other task run" );
  status = dly_tsk( 1000 );
  assert( status == E_OK );

  printf( "*** END OF ITRONSEM01 -- ITRON SEMAPHORE TEST ***\n" );
  rtems_test_exit( 0 );
}
