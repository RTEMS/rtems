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

  build_time( &time, 12, 31, 1988, 9, 0, 0, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );


  /*
   * Set My priority to 8 so that dummy tasks will be
   * forced to run when started.
   */
  
  status = chg_pri( TSK_SELF, 8 );
  fatal_directive_status( status, E_OK, "chg_pri of TSK_SELF");
  status = ref_tsk( &pk_rtsk, TSK_SELF );
  fatal_directive_status( status, E_OK, "ref_tsk of TSK_SELF");
  fatal_directive_status( pk_rtsk.tskpri, 8, "task priority of SELF");
  
  /*
   * Create and verify a DORMANT task.
   */

  pk_ctsk.exinf    = NULL;
  pk_ctsk.tskatr   = TA_HLNG;
  pk_ctsk.itskpri  = 1;
  pk_ctsk.task     = Dormant_task;
  pk_ctsk.stksz    = RTEMS_MINIMUM_STACK_SIZE; 

  puts( "Init - cre_tsk - Dormant Task" );
  status = cre_tsk( DORMANT_TASK_ID, &pk_ctsk );
  fatal_directive_status( status, E_OK, "cre_tsk of DORMANT");
  status = ref_tsk( &pk_rtsk, DORMANT_TASK_ID );
  fatal_directive_status( status, E_OK, "ref_tsk of DORMANT");
  fatal_directive_status( pk_rtsk.tskstat, TTS_DMT, "task state of DORMANT");

  /*
   * Create, Start and verify a not DORMANT task.
   */
  
  pk_ctsk.task     = Non_Dormant_task;
  puts( "Init - cre_tsk - Non-Dormant Task" );
  status = cre_tsk( NON_DORMANT_TASK_ID, &pk_ctsk );
  fatal_directive_status( status, E_OK, "cre_tsk of NON_DORMANT");
  status = sta_tsk( NON_DORMANT_TASK_ID, 1 );
  status = ref_tsk( &pk_rtsk, NON_DORMANT_TASK_ID );
  fatal_directive_status( status, E_OK, "ref_tsk of NON_DORMANT");
  fatal_directive_status( pk_rtsk.tskstat,TTS_WAI,"task state of NON_DORMANT");
    
  
  /*
   *  Bad ID errors
   */

  puts( "\n*** Create Task Errors ***" );

  puts( "Init - cre_tsk - access violation ( id less than -4) - E_OACV" );
  status = cre_tsk( -5, &pk_ctsk );
  fatal_directive_status( status, E_OACV, "cre_tsk of -5");

  puts( "Init - cre_tsk - bad id (between 0 and -4) - E_ID" );
  status = cre_tsk( -2, &pk_ctsk );
  fatal_directive_status( status, E_ID, "cre_tsk of -2");

  puts( "Init - cre_tsk - cannot create TSK_SELF  - E_ID" );
  status = cre_tsk( TSK_SELF, &pk_ctsk );
  fatal_directive_status( status, E_ID, "cre_tsk of TSK_SELF");

  puts( "Init - cre_tsk - invalid id; id already exists  - E_OBJ" );
  status = cre_tsk( 1, &pk_ctsk );
  fatal_directive_status( status, E_OBJ, "cre_tsk of 1");

  /*
   *  Bad task attribute errors
   */

  pk_ctsk.tskatr = -1;
  puts( "Init - cre_tsk - tskatr is invalid - E_RSATR" );
  status = cre_tsk( 5, &pk_ctsk );
  fatal_directive_status( status, E_RSATR, "cre_tsk with tskatr of -1");

  puts( "Init - cre_tsk - pk_ctsk is invalid - E_PAR" );
  status = cre_tsk( 5, NULL );
  fatal_directive_status( status, E_PAR, "cre_tsk with NULL discription");

  pk_ctsk.tskatr = TA_HLNG;
  pk_ctsk.itskpri = 0;
  puts( "Init - cre_tsk - itskpri is 0 - E_PAR" );
  status = cre_tsk( 5, &pk_ctsk );
  fatal_directive_status( status, E_PAR, "cre_tsk with priority of 0");
  pk_ctsk.itskpri = 257;         /* XXX Design parameter not requirement. */
  puts( "Init - cre_tsk - itskpri is 257 - E_PAR" );
  status = cre_tsk( 5, &pk_ctsk );
  fatal_directive_status( status, E_PAR, "cre_tsk with priority of 257");

  pk_ctsk.stksz = -1;
  puts( "Init - cre_tsk - stksz is invalid - E_PAR" );
  status = cre_tsk( 5, &pk_ctsk );
  fatal_directive_status( status, E_PAR, "cre_tsk with size of -1");

  pk_ctsk.task = NULL;
  puts( "Init - cre_tsk - task is invalid - E_PAR" );
  status = cre_tsk( 5, &pk_ctsk );
  fatal_directive_status( status, E_PAR, "cre_tsk with null task identifier");


#if (0)
  /* these errors can not be generated for cre_tsk at this time */
  fatal_directive_status( status, E_NOMEM, "");
  fatal_directive_status( status, EN_OBJNO, "");
  fatal_directive_status( status, EN_CTXID, "");
  fatal_directive_status( status, EN_PAR, "");
#endif

  puts( "\n\n*** Delete Task Errors ***" );

  /* 
   *  Reset structure
   */

  pk_ctsk.exinf    = NULL;
  pk_ctsk.tskatr   = TA_HLNG;
  pk_ctsk.itskpri  = 1; 
  pk_ctsk.task     = Dormant_task;
  pk_ctsk.stksz    = RTEMS_MINIMUM_STACK_SIZE; 


  puts( "Init - del_tsk - cannot delete TSK_SELF - E_OBJ" );
  status = del_tsk( TSK_SELF );
  fatal_directive_status( status, E_OBJ, "del_tsk with SELF");

  puts( "Init - del_tsk - task is not DORMANT - E_OBJ" );
  status = del_tsk( NON_DORMANT_TASK_ID );
  fatal_directive_status( status, E_OBJ, "del_tsk NON_DORMANT");

  puts( "Init - del_tsk - task does not exist - E_NOEXS" );
  status = del_tsk( 5 );
  fatal_directive_status( status, E_NOEXS, "del_tsk 5");

  puts( "Init - del_tsk - access violation ( id less than -4) - E_OACV" );
  status =  del_tsk( -5 );
  fatal_directive_status( status, E_OACV, "del_tsk -5");

  puts( "Init - del_tsk - cannot delete TSK_SELF - E_OBJ" );
  status = del_tsk( TSK_SELF );
  fatal_directive_status( status, E_OBJ, "del_tsk self");

  puts( "Init - del_tsk - bad id (between 0 and -4) - E_ID" );
  status = del_tsk( -3 );
  fatal_directive_status( status, E_ID, "del_tsk -3");

#if (0)
  /* these errors can not be generated for del_tsk at this time */
  fatal_directive_status( status, EN_OBJNO, "del_tsk ");
  fatal_directive_status( status, EN_CTXID, "del_tsk ");
#endif


  puts( "\n\n*** Start Task Errors ***" );

  puts( "Init - sta_tsk - access violation ( id less than -4) - E_OACV" );
  status = sta_tsk( -5, 1 );
  fatal_directive_status( status, E_OACV, "sta_tsk of -5");

  puts( "Init - sta_tsk - bad id (between 0 and -4) - E_ID" );
  status = sta_tsk( -2, 1 );
  fatal_directive_status( status, E_ID, "sta_tsk of -2");

  puts( "Init - sta_tsk - cannot start TSK_SELF - E_OBJ" );
  status = sta_tsk( TSK_SELF, 1 );
  fatal_directive_status( status, E_OBJ, "sta_tsk of self");

  puts( "Init - sta_tsk - task is not DORMANT  - E_OBJ" );
  status = sta_tsk( NON_DORMANT_TASK_ID, 1 );
  fatal_directive_status( status, E_OBJ, "sta_tsk NON_DORMANT");

  puts( "Init - sta_tsk - task does not exist  - E_NOEXS" );
  status = sta_tsk( 5, 1 );
  fatal_directive_status( status, E_NOEXS, "5");

#if (0)
  /* these errors can not be generated for sta_tsk at this time */
  fatal_directive_status( status, EN_OBJNO, "sta_tsk");
  fatal_directive_status( status, EN_CTXID, "sta_tsk");
  fatal_directive_status( status, EN_PAR, "sta_tsk");
#endif


#if (0)
  /* these errors can not be tested at this time */
  puts( "\n\n*** Exit Task Errors ***" );
  puts( "Init - ext_tsk - context error - E_CTX" );
  status = ext_tsk(  );
  fatal_directive_status( status, E_CTX, "ext_tsk ");

  puts( "\n\n*** Exit and Delete Task Errors ***" );
  puts( "Init - exd_tsk - context error - E_CTX" );
  status = exd_tsk(  );
  fatal_directive_status( status, E_CTX, "exd_tsk");
#endif


  puts( "\n\n*** Terminate Other Task Errors ***" );

  puts( "Init - ter_tsk - bad id (between 0 and -4) - E_ID" );
  status = ter_tsk( -2 );
  fatal_directive_status( status, E_ID, "ter_tsk of -2");

  puts( "Init - ter_tsk - cannot terminate TSK_SELF (0) - E_OBJ" );
  status = ter_tsk( TSK_SELF );
  fatal_directive_status( status, E_OBJ, "ter_tsk of self");

  puts( "Init - ter_tsk - task is not DORMANT - E_OBJ" );
  status = ter_tsk( DORMANT_TASK_ID );
  fatal_directive_status( status, E_OBJ, "ter_tsk DORMANT");

  puts( "Init - ter_tsk - task does not exist - E_NOEXS" );
  status = ter_tsk( 5 );
  fatal_directive_status( status, E_NOEXS, "ter_tsk of 5");

  puts( "Init - ter_tsk - access violation ( id less than -4) - E_OACV" );
  status = ter_tsk( -5 );
  fatal_directive_status( status, E_OACV, "ter_tsk of -5");

#if (0)
  /* these errors can not be generated for ter_tsk at this time */
  fatal_directive_status( status, EN_OBJNO, "ter_tsk");
  fatal_directive_status( status, EN_CTXID, "ter_tsk");
#endif


#if (0)
  status = dis_dsp( );
  fatal_directive_status( status, E_CTX, "dis_dsp");

  status = ena_dsp( );
  fatal_directive_status( status, E_CTX, "ena_dsp");
#endif

  puts( "\n\n*** Change Priority Task Errors ***" );

  puts( "Init - chg_pri - bad id (between 0 and -4) - E_ID" );
  status = chg_pri( -2, 1 );
  fatal_directive_status( status, E_ID, "chg_pri of -2");

  /*  Call from task independent portion to cause E_OBJ
  puts( "Init - chg_pri - change priority of TSK_SELF - E_OBJ" );
  status = chg_pri( XXX - INTERRUPT, 1 );
  assert( status == E_OBJ );
  */

  puts( "Init - chg_pri - task is DORMANT - E_OBJ" );
  status = chg_pri( DORMANT_TASK_ID, 1 );
  fatal_directive_status( status, E_OBJ, "chg_pri of DORMANT");

  puts( "Init - chg_pri - task does not exist - E_NOEXS" );
  status = chg_pri( 5, 1 );
  fatal_directive_status( status, E_NOEXS, "chg_pri of 5");

  puts( "Init - chg_pri - access violation ( id less than -4) - E_OACV" );
  status =  chg_pri( -5, 1 );
  fatal_directive_status( status, E_OACV, "chg_pri of -5");

  puts( "Init - chg_pri - invalid priority - E_PAR" );
  status =  chg_pri( 1, -1 );
  fatal_directive_status( status, E_PAR, "chg_pri with priority of -1");

#if (0)
  /* these errors can not be generated for chg_pri at this time */
  fatal_directive_status( status, EN_OBJNO, "chg_pri");
  fatal_directive_status( status, EN_CTXID, "chg_pri");
  fatal_directive_status( status, EN_PAR, "chg_pri");
#endif

  /*  This gave me a nasty-gram
   *  "core_find_mapping() - access to unmaped address, attach a default map
   *   to handle this - addr=0x80002098 nr_bytes=0x4 processor=0x40134008
   *   cia=0xc744"
   */
  
   puts( "\n\n*** Rotate Ready Queue Errors ***" );
   puts( "Init - rot_rdq - priority  -1 - E_PAR" );
   status = rot_rdq( -1 );
   fatal_directive_status( status, E_PAR, "rot_rdq -1");
   puts( "Init - rot_rdq - priority  257 - E_PAR" );
   status = rot_rdq( 257 );
   fatal_directive_status( status, E_PAR, "rot_rdq 256");

  /* XXX - This routine is not coded */

  puts( "Init - rel_rdq - XXX Add when rel_wai coded - E_OK" );
  status = rel_wai( 1 );
  fatal_directive_status( status, E_OK, "rel_wai");

  puts( "Init - rel_rdq - XXX Add when rel_wai coded - E_ID" );
  status = E_ID;
  fatal_directive_status( status, E_ID, "rel_wai");

  puts( "Init - rel_rdq - XXX Add when rel_wai coded - E_NOEXS" );
  status = E_NOEXS;
  fatal_directive_status( status, E_NOEXS, "rel_wai");

  puts( "Init - rel_rdq - XXX Add when rel_wai coded - E_OACV" );
  status = E_OACV;
  fatal_directive_status( status, E_OACV, "rel_wai");

  puts( "Init - rel_rdq - XXX Add when rel_wai coded - E_OBJ" );
  status = E_OBJ;
  fatal_directive_status( status, E_OBJ, "rel_wai");

  puts( "Init - rel_rdq - XXX Add when rel_wai coded - EN_OBJNO" );
  status = EN_OBJNO;
  fatal_directive_status( status, EN_OBJNO, "rel_wai");

  puts( "Init - rel_rdq - XXX Add when rel_wai coded - EN_CTXID" );
  status = EN_CTXID;
  fatal_directive_status( status, EN_CTXID, "rel_wai");


  puts( "\n\n*** Reference Task Status Errors ***" );
  puts( "Init - ref_tsk - bad id (between 0 and -4) - E_ID" );
  status = ref_tsk( &pk_rtsk, -2 );
  fatal_directive_status( status, E_ID, "ref_tsk -2");

  /*  XXX Call from task independent portion to cause E_ID 
  puts( "Init - ref_tsk - reference INTERRUPT - E_ID" );
  status = ref_tsk( &pk_rtsk, TSK_SELF );
  assert( status == E_ID );
  */

  puts( "Init - ref_tsk - task does not exist - E_NOEXS" );
  status = ref_tsk( &pk_rtsk, 5 );
  fatal_directive_status( status, E_NOEXS, "ref_tsk 5");

  puts( "Init - ref_tsk - access violation ( id less than -4) - E_OACV" );
  status =  ref_tsk( &pk_rtsk, -5 );
  fatal_directive_status( status, E_OACV, "ref_tsk -5");

  puts( "Init - ref_tsk - packet address is bad - E_PAR" );
  status =  ref_tsk( NULL, 1 );
  fatal_directive_status( status, E_PAR, "ref_tsk SELF with NULL descriptor");

#if (0)
  /*  these errors can not be generated for ref_tsk at this time */
  fatal_directive_status( status, EN_OBJNO, "ref_tsk");
  fatal_directive_status( status, EN_CTXID, "ref_tsk");
  fatal_directive_status( status, EN_RPAR, "ref_tsk");
#endif

  puts( "\n\n*** Suspend Task Errors ***" );

  puts( "Init - sus_tsk - access violation ( id less than -4) - E_OACV" );
  status = sus_tsk( -5 );
  fatal_directive_status( status, E_OACV, "sus_tsk of -5");

  puts( "Init - sus_tsk - bad id (between 0 and -4) - E_ID" );
  status = sus_tsk( -2 );
  fatal_directive_status( status, E_ID, "sus_tsk of -2");

  puts( "Init - sus_tsk - cannot suspend SELF - E_OBJ" );
  status = sus_tsk( TSK_SELF );
  fatal_directive_status( status, E_OBJ, "sus_tsk of self");

  puts( "Init - sus_tsk - task does not exist - E_NOEXS" );
  status = sus_tsk( 5 );
  fatal_directive_status( status, E_NOEXS, "sus_tsk of 5");

  /* XXX - We support nested suspends and will never return this error.
  puts( "Init - sus_tsk - no support for nested SUSPENDS - E_QOVR" );
  status = sus_tsk( 1 );
  fatal_directive_status( status, E_QOVR, "sus_tsk");
  */

  /* XXX - Can not test this.
  puts( "Init - sus_tsk - exceeded limit for nested SUSPENDS - E_QOVR" );
  status = sus_tsk( 1 );
  fatal_directive_status( status, E_QOVR, "sus_tsk");
  */

#if (0)
  /* these errors can not be generated for sus_tsk at this time */
  fatal_directive_status( status, EN_OBJNO, "sus_tsk");
  fatal_directive_status( status, EN_CTXID, "sus_tsk");
#endif


  puts( "\n\n*** Resume Task Errors ***" );

  puts( "Init - rsm_tsk - access violation ( id less than -4) - E_OACV" );
  status = rsm_tsk( -5 );
  fatal_directive_status( status, E_OACV, "rsm_tsk -5");

  puts( "Init - rsm_tsk - bad id (between 0 and -4) - E_ID" );
  status = rsm_tsk( -2 );
  fatal_directive_status( status, E_ID, "rsm_tsk -2");

  puts( "Init - rsm_tsk - cannot resume SELF - E_OBJ" );
  status = rsm_tsk( TSK_SELF );
  fatal_directive_status( status, E_OBJ, "rsm_tsk self");

  puts( "Init - rsm_tsk - task is DORMANT - E_OBJ" );
  status = rsm_tsk( DORMANT_TASK_ID );
  fatal_directive_status( status, E_OBJ, "rsm_tsk DORMANT");

  puts( "Init - rsm_tsk - task is NON_DORMANT not suspended - E_OK" );
  status = rsm_tsk( NON_DORMANT_TASK_ID );
  fatal_directive_status( status, E_OK, "rsm_tsk NON_DORMANT");

  puts( "Init - rsm_tsk - task does not exist - E_NOEXS" );
  status = rsm_tsk( 5 );
  fatal_directive_status( status, E_NOEXS, "rms_tsk 5");

#if (0)
  /* these errors can not be generated for rsm_tsk at this time */
  fatal_directive_status( status, EN_OBJNO, "rsm_tsk");
  fatal_directive_status( status, EN_CTXID, "rsm_tsk");
#endif


  puts( "\n\n*** Forcibly Resume Task Errors ***" );

  puts( "Init - frsm_tsk - access violation ( id less than -4) - E_OACV" );
  status = frsm_tsk( -5 );
  fatal_directive_status( status, E_OACV, "frsm_tsk -5");

  puts( "Init - frsm_tsk - bad id (between 0 and -4) - E_ID" );
  status = frsm_tsk( -2 );
  fatal_directive_status( status, E_ID, "frsm_tsk -2");

  puts( "Init - frsm_tsk - cannot forcibly resume SELF - E_OBJ" );
  status = frsm_tsk( TSK_SELF );
  fatal_directive_status( status, E_OBJ, "frsm_tsk self");

  puts( "Init - frsm_tsk - task is DORMANT - E_OBJ" );
  status = frsm_tsk( DORMANT_TASK_ID );
  fatal_directive_status( status, E_OBJ, "frsm_tsk DORMANT");

  puts( "Init - frsm_tsk - task does not exist - E_NOEXS" );
  status = frsm_tsk( 5 );
  fatal_directive_status( status, E_NOEXS, "frsm_tsk 5");

  puts( "Init - frsm_tsk - task is NON_DORMANT not suspended - E_OK" );
  status = frsm_tsk( NON_DORMANT_TASK_ID );
  fatal_directive_status( status, E_OK, "frsm_tsk NON_DORMANT");

#if (0)
  /* these errors can not be generated for frsm_tsk at this time */
  fatal_directive_status( status, EN_OBJNO, "frsm_tsk");
  fatal_directive_status( status, EN_CTXID, "frsm_tsk");
#endif


#if (0)
XXXXX - FIX ME
  /* these directives are not coded */
  slp_tsk( );
  fatal_directive_status( status, E_OK, "");
  fatal_directive_status( status, E_PAR, "");
  fatal_directive_status( status, E_RLWAI, "");
  fatal_directive_status( status, E_TMOUT, "");
  fatal_directive_status( status, E_CTX, "");


  tslp_tsk( TMO );
  fatal_directive_status( status, E_OK, "");
  fatal_directive_status( status, E_PAR, "");
  fatal_directive_status( status, E_RLWAI, "");
  fatal_directive_status( status, E_TMOUT, "");
  fatal_directive_status( status, E_CTX, "");

  wup_tsk( ID );
  fatal_directive_status( status, E_OK, "");
  fatal_directive_status( status, E_ID, "");
  fatal_directive_status( status, E_NOEXS, "");
  fatal_directive_status( status, E_OACV, "");
  fatal_directive_status( status, E_OBJ, "");
  fatal_directive_status( status, E_QOVR, "");
  fatal_directive_status( status, EN_OBJNO, "");
  fatal_directive_status( status, EN_CTXID, "");


  can_tsk( INT, ID );
  fatal_directive_status( status, E_OK, "");
  fatal_directive_status( status, E_ID, "");
  fatal_directive_status( status, E_NOEXS, "");
  fatal_directive_status( status, E_OACV, "");
  fatal_directive_status( status, E_OBJ, "");
  fatal_directive_status( status, EN_OBJNO, "");
  fatal_directive_status( status, EN_CTXID, "");
  fatal_directive_status( status, EN_RPAR, "");
#endif

  puts( "*** END OF ITRON TASK TEST 2 ***" );
  rtems_test_exit( 0 );
}
