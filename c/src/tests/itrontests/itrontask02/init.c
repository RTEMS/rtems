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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#define TEST_INIT
#include "system.h"
#include <stdio.h>
#include <assert.h>

void ITRON_Init( void )
{

  /*
   *  Status Codes for these errors
   *
   *
   *  E_OK - Normal Completion 
   *
   *  E_NOMEM - Insufficient memory (Memory for control block and/or user
   *            stack cannot be allocated) 
   *
   *  E_ID - Invalid ID Number (tskid was invalid or could not be used) 
   *
   *  E_RSATR - Reserved attribute (tskatr was invalid or could not be used) 
   *
   *  E_OBJ - Invalid object state (a task of the same ID already exists) 
   *
   *  E_OACV - Object access violation (A tskid less than -4 was specified 
   *           from a user task. This is implementation dependent.) 
   *
   *  E_PAR - Parameter error (pk_ctsk, task, itskpri and/or stksz is invalid) 
   *
   *  E_NOEXS - Object does not exist (the task specified by tskid does not
   *            exist) 
   *
   *  E_CTX - Context error (issued from task-independent portions or a task 
   *          in dispatch disabled state) 
   *
   *
   *
   *  Network Specific Errors  (ITRON calls these Connection Function Errors)
   *
   *  EN_OBJNO - An object number which could not be accessed on the target
   *             node is specified. 
   *
   *  EN_CTXID - Specified an object on another node when the system call
   *             was issued from a task in dispatch disabled state or from
   *             a task-independent portion 
   *
   *  EN_PAR - A value outside the range supported by the target node and/or 
   *           transmission packet format was specified as a parameter
   *           (a value outside supported range was specified for exinf,
   *           tskatr, task, itskpri and/or stksz) 
   *
   *  EN_RPAR - A value outside the range supported by the requesting node 
   *            and/or transmission packet format was returned as a return 
   *            parameter (a value outside supported range was returned for
   *            exinf, tskpri and/or tskstat) 
   *
   */


  rtems_time_of_day  time;
  ER                 status;
  T_CTSK             pk_ctsk;
  T_RTSK             pk_rtsk;   /* Reference Task Packet */ 


  puts( "\n\n*** ITRON TASK TEST 2 ***\n" );
  puts( "\n*** Create Task Errors ***" );

  build_time( &time, 12, 31, 1988, 9, 0, 0, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  pk_ctsk.exinf    = NULL;
  pk_ctsk.tskatr   = TA_HLNG;
  pk_ctsk.itskpri  = 1; 
  pk_ctsk.task     = Preempt_task;
  pk_ctsk.stksz    = RTEMS_MINIMUM_STACK_SIZE; 

  puts( "Init - cre_tsk - Preempt Task" );
  status = cre_tsk( PREEMPT_TASK_ID, &pk_ctsk );
  assert( status == E_OK );

  /*
   *  Bad ID errors
   */

  puts( "Init - cre_tsk - access violation ( id less than -4) - E_OACV" );
  status = cre_tsk( -5, &pk_ctsk );
  assert( status == E_OACV );

  puts( "Init - cre_tsk - bad id (between 0 and -4) - E_ID" );
  status = cre_tsk( -2, &pk_ctsk );
  assert( status == E_ID );

  puts( "Init - cre_tsk - cannot create self (0) - E_ID" );
  status = cre_tsk( 0, &pk_ctsk );
  assert( status == E_ID );

  puts( "Init - cre_tsk - invalid id; id already exists  - E_OBJ" );
  status = cre_tsk( 1, &pk_ctsk );
  assert( status == E_OBJ );

  /*
   *  Bad task attribute errors
   */

  pk_ctsk.tskatr = -1;
  puts( "Init - cre_tsk - tskatr is invalid - E_RSATR" );
  status = cre_tsk( 5, &pk_ctsk );
  assert( status == E_RSATR );

  puts( "Init - cre_tsk - pk_ctsk is invalid - E_PAR" );
  status = cre_tsk( 5, NULL );
  assert( status == E_PAR );

  pk_ctsk.tskatr = TA_HLNG;
  pk_ctsk.itskpri = 0;
  puts( "Init - cre_tsk - itskpri is 0 - E_PAR" );
  status = cre_tsk( 5, &pk_ctsk );
  assert( status == E_PAR );
  pk_ctsk.itskpri = 257;         /* XXX Design parameter not requirement. */
  puts( "Init - cre_tsk - itskpri is 257 - E_PAR" );
  status = cre_tsk( 5, &pk_ctsk );
  assert( status == E_PAR );

  pk_ctsk.stksz = -1;
  puts( "Init - cre_tsk - stksz is invalid - E_PAR" );
  status = cre_tsk( 5, &pk_ctsk );
  assert( status == E_PAR );

  pk_ctsk.task = NULL;
  puts( "Init - cre_tsk - task is invalid - E_PAR" );
  status = cre_tsk( 5, &pk_ctsk );
  assert( status == E_PAR );


#if (0)
  /* these errors can not be generated for cre_tsk at this time */
  assert( status == E_NOMEM );
  assert( status == EN_OBJNO );
  assert( status == EN_CTXID );
  assert( status == EN_PAR );
#endif


  puts( "\n\n*** Delete Task Errors ***" );

  /* 
   *  Reset structure
   */

  pk_ctsk.exinf    = NULL;
  pk_ctsk.tskatr   = TA_HLNG;
  pk_ctsk.itskpri  = 1; 
  pk_ctsk.task     = Preempt_task;
  pk_ctsk.stksz    = RTEMS_MINIMUM_STACK_SIZE; 


  puts( "Init - del_tsk - cannot delete self - E_OBJ" );
  status = del_tsk( 1 );
  assert( status == E_OBJ );

  puts( "Init - del_tsk - task is not DORMANT - E_OBJ" );
  status = del_tsk( 1 );
  assert( status == E_OBJ );

  puts( "Init - del_tsk - task does not exist - E_NOEXS" );
  status = del_tsk( 5 );
  assert( status == E_NOEXS );

  puts( "Init - del_tsk - access violation ( id less than -4) - E_OACV" );
  status =  del_tsk( -5 );
  assert( status == E_OACV );

  puts( "Init - del_tsk - cannot delete SELF - E_OBJ" );
  status = del_tsk( 0 );
  assert( status == E_OBJ );

  puts( "Init - del_tsk - bad id (between 0 and -4) - E_ID" );
  status = del_tsk( -3 );
  assert( status == E_ID );

#if (0)
  /* these errors can not be generated for del_tsk at this time */
  assert( status == EN_OBJNO );
  assert( status == EN_CTXID );
#endif


  puts( "\n\n*** Start Task Errors ***" );

  puts( "Init - sta_tsk - access violation ( id less than -4) - E_OACV" );
  status = sta_tsk( -5, 1 );
  assert( status == E_OACV );

  puts( "Init - sta_tsk - bad id (between 0 and -4) - E_ID" );
  status = sta_tsk( -2, 1 );
  assert( status == E_ID );

  puts( "Init - sta_tsk - cannot start SELF - E_OBJ" );
  status = sta_tsk( 0, 1 );
  assert( status == E_OBJ );

  puts( "Init - sta_tsk - task is not DORMANT  - E_OBJ" );
  status = sta_tsk( 1, 1 );
  assert( status == E_OBJ );

  puts( "Init - sta_tsk - task does not exist  - E_NOEXS" );
  status = sta_tsk( 5, 1 );
  assert( status == E_NOEXS );

#if (0)
  /* these errors can not be generated for sta_tsk at this time */
  assert( status == EN_OBJNO );
  assert( status == EN_CTXID );
  assert( status == EN_PAR );
#endif


#if (0)
  /* these errors can not be tested at this time */
  puts( "\n\n*** Exit Task Errors ***" );
  puts( "Init - ext_tsk - context error - E_CTX" );
  status = ext_tsk(  );
  assert( status == E_CTX );

  puts( "\n\n*** Exit and Delete Task Errors ***" );
  puts( "Init - exd_tsk - context error - E_CTX" );
  status = exd_tsk(  );
  assert( status == E_CTX );
#endif


  puts( "\n\n*** Terminate Other Task Errors ***" );

  puts( "Init - ter_tsk - bad id (between 0 and -4) - E_ID" );
  status = ter_tsk( -2 );
  assert( status == E_ID );

  puts( "Init - ter_tsk - cannot terminate SELF (0) - E_OBJ" );
  status = ter_tsk( 0 );
  assert( status == E_OBJ );

  puts( "Init - ter_tsk - task is not DORMANT - E_OBJ" );
  status = ter_tsk( 1 );
  assert( status == E_OBJ );

  puts( "Init - ter_tsk - task does not exist - E_NOEXS" );
  status = ter_tsk( 5 );
  assert( status == E_NOEXS );

  puts( "Init - ter_tsk - access violation ( id less than -4) - E_OACV" );
  status = ter_tsk( -5 );
  assert( status == E_OACV );

#if (0)
  /* these errors can not be generated for ter_tsk at this time */
  assert( status == EN_OBJNO );
  assert( status == EN_CTXID );
#endif


#if (0)
  status = dis_dsp( );
  assert( status == E_CTX );

  status = ena_dsp( );
  assert( status == E_CTX );
#endif


  puts( "\n\n*** Change Priority Task Errors ***" );

  puts( "Init - chg_pri - bad id (between 0 and -4) - E_ID" );
  status = chg_pri( -2, 1 );
  assert( status == E_ID );

  /*  Call from task independent portion to cause E_OBJ
  puts( "Init - chg_pri - change priority of SELF - E_OBJ" );
  status = chg_pri( 0, 1 );
  assert( status == E_OBJ );
  */

  /*  Need a dormant task to call
  puts( "Init - chg_pri - task is not DORMANT - E_OBJ" );
  status = chg_pri( 1, 1 );
  assert( status == E_OBJ );
  */

  puts( "Init - chg_pri - task does not exist - E_NOEXS" );
  status = chg_pri( 5, 1 );
  assert( status == E_NOEXS );

  puts( "Init - chg_pri - access violation ( id less than -4) - E_OACV" );
  status =  chg_pri( -5, 1 );
  assert( status == E_OACV );

  puts( "Init - chg_pri - invalid priority - E_PAR" );
  status =  chg_pri( 1, -1 );
  assert( status == E_PAR );

#if (0)
  /* these errors can not be generated for chg_pri at this time */
  assert( status == EN_OBJNO );
  assert( status == EN_CTXID );
  assert( status == EN_PAR );
#endif

  /*  This gave me a nasty-gram
   *  "core_find_mapping() - access to unmaped address, attach a default map
   *   to handle this - addr=0x80002098 nr_bytes=0x4 processor=0x40134008
   *   cia=0xc744"
   */
  
   puts( "\n\n*** Rotate Ready Queue Errors ***" );
   puts( "Init - rot_rdq - invalid priority - E_PAR" );
   status = rot_rdq( -1 );
   assert( status == E_PAR );

#if (0)
  /* This routine is not coded */
  rel_wai( ID );
  assert( status == E_OK );
  assert( status == E_ID );
  assert( status == E_NOEXS );
  assert( status == E_OACV );
  assert( status == E_OBJ );
  assert( status == EN_OBJNO );
  assert( status == EN_CTXID );
#endif


#if (0)
  get_tid( ID );
  assert( status == E_OK );
#endif

  puts( "\n\n*** Reference Task Status Errors ***" );
  puts( "Init - ref_tsk - bad id (between 0 and -4) - E_ID" );
  status = ref_tsk( &pk_rtsk, -2 );
  assert( status == E_ID );

  /*  Call from task independent portion to cause E_ID 
  puts( "Init - ref_tsk - reference SELF - E_ID" );
  status = ref_tsk( &pk_rtsk, 0 );
  assert( status == E_ID );
  */

  puts( "Init - ref_tsk - task does not exist - E_NOEXS" );
  status = ref_tsk( &pk_rtsk, 5 );
  assert( status == E_NOEXS );

  puts( "Init - ref_tsk - access violation ( id less than -4) - E_OACV" );
  status =  ref_tsk( &pk_rtsk, -5 );
  assert( status == E_OACV );

  puts( "Init - ref_tsk - packet address is bad - E_PAR" );
  status =  ref_tsk( NULL, 1 );
  assert( status == E_PAR );


#if (0)
  /*  these errors can not be generated for ref_tsk at this time */
  assert( status == EN_OBJNO );
  assert( status == EN_CTXID );
  assert( status == EN_RPAR );
#endif


  puts( "\n\n*** Suspend Task Errors ***" );

  puts( "Init - sus_tsk - access violation ( id less than -4) - E_OACV" );
  status = sus_tsk( -5 );
  assert( status == E_OACV );

  puts( "Init - sus_tsk - bad id (between 0 and -4) - E_ID" );
  status = sus_tsk( -2 );
  assert( status == E_ID );

  puts( "Init - sus_tsk - cannot suspend SELF - E_OBJ" );
  status = sus_tsk( 0 );
  assert( status == E_OBJ );

  puts( "Init - sus_tsk - task does not exist - E_NOEXS" );
  status = sus_tsk( 5 );
  assert( status == E_NOEXS );
#if (0) 
  puts( "Init - sus_tsk - no support for nested SUSPENDS - E_QOVR" );
  status = sus_tsk( 1 );
  assert( status == E_QOVR );

  puts( "Init - sus_tsk - exceeded limit for nested SUSPENDS - E_QOVR" );
  status = sus_tsk( 1 );
  assert( status == E_QOVR );
#endif

#if (0)
  /* these errors can not be generated for sus_tsk at this time */
  assert( status == EN_OBJNO );
  assert( status == EN_CTXID );
#endif


  puts( "\n\n*** Resume Task Errors ***" );

  puts( "Init - rsm_tsk - access violation ( id less than -4) - E_OACV" );
  status = rsm_tsk( -5 );
  assert( status == E_OACV );

  puts( "Init - rsm_tsk - bad id (between 0 and -4) - E_ID" );
  status = rsm_tsk( -2 );
  assert( status == E_ID );

  puts( "Init - rsm_tsk - cannot resume SELF - E_OBJ" );
  status = rsm_tsk( 0 );
  assert( status == E_OBJ );

  puts( "Init - rsm_tsk - task is DORMANT - E_OBJ" );
  status = rsm_tsk( 1 );
  assert( status == E_OBJ );

  puts( "Init - rsm_tsk - task does not exist - E_NOEXS" );
  status = rsm_tsk( 5 );
  assert( status == E_NOEXS );

#if (0)
  /* these errors can not be generated for rsm_tsk at this time */
  assert( status == EN_OBJNO );
  assert( status == EN_CTXID );
#endif


  puts( "\n\n*** Forcibly Resume Task Errors ***" );

  puts( "Init - frsm_tsk - access violation ( id less than -4) - E_OACV" );
  status = frsm_tsk( -5 );
  assert( status == E_OACV );

  puts( "Init - frsm_tsk - bad id (between 0 and -4) - E_ID" );
  status = frsm_tsk( -2 );
  assert( status == E_ID );

  puts( "Init - frsm_tsk - cannot forcibly resume SELF - E_OBJ" );
  status = frsm_tsk( 0 );
  assert( status == E_OBJ );

  puts( "Init - frsm_tsk - task is DORMANT - E_OBJ" );
  status = frsm_tsk( 1 );
  assert( status == E_OBJ );

  puts( "Init - frsm_tsk - task does not exist - E_NOEXS" );
  status = frsm_tsk( 5 );
  assert( status == E_NOEXS );

#if (0)
  /* these errors can not be generated for frsm_tsk at this time */
  assert( status == EN_OBJNO );
  assert( status == EN_CTXID );
#endif


#if (0)
  /* these directives are not coded */
  slp_tsk( );
  assert( status == E_OK );
  assert( status == E_PAR );
  assert( status == E_RLWAI );
  assert( status == E_TMOUT );
  assert( status == E_CTX );


  tslp_tsk( TMO );
  assert( status == E_OK );
  assert( status == E_PAR );
  assert( status == E_RLWAI );
  assert( status == E_TMOUT );
  assert( status == E_CTX );


  wup_tsk( ID );
  assert( status == E_OK );
  assert( status == E_ID );
  assert( status == E_NOEXS );
  assert( status == E_OACV );
  assert( status == E_OBJ );
  assert( status == E_QOVR );
  assert( status == EN_OBJNO );
  assert( status == EN_CTXID );


  can_tsk( INT, ID );
  assert( status == E_OK );
  assert( status == E_ID );
  assert( status == E_NOEXS );
  assert( status == E_OACV );
  assert( status == E_OBJ );
  assert( status == EN_OBJNO );
  assert( status == EN_CTXID );
  assert( status == EN_RPAR );
#endif

  puts( "*** ITRON TASK TEST 2 ***" );
  exit( 0 );
}
