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
#include <stdio.h>
#include <assert.h>

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



void TestCre();
void TestDel();
void TestPsnd();
void TestRef();
void TestTrcv();
void TestRcv();
void TestPrcv();

void ITRON_Init( void )
{
    printf( "\n\n*** ITRONMBF01 -- ITRON MESSAGE BUFFER TEST ***\n" );

    TestCre();
    TestDel();
    TestPsnd();
    TestRef();
    TestRcv();
    TestRef();
    TestPrcv();
    TestTrcv();

    printf("\n\n*** ITRONMBF01 --- ITRON MESSAGE BUFFER TEST ***\n");
    rtems_test_exit(0);
}

void TestCre()
{
    ER      status;
    T_CMBF  pk_cmbf;
    
    /*
     *  Exercise cre_mbf - this code seems to exercise every path.
     */

    puts( "Init - cre_mbf - NULL pk_cmbf returns E_PAR" );
    status = cre_mbf( 1, NULL );
    assert( status == E_PAR );

    puts( "Init - cre_mbf - negative values for bufsz returns E_PAR" );
    pk_cmbf.bufsz = -1;
    pk_cmbf.maxmsz  = 100;
    pk_cmbf.mbfatr  = 0;
    pk_cmbf.exinf   = NULL;
    status = cre_mbf( 1, &pk_cmbf );
    assert( status == E_PAR );

    puts( "Init - cre_mbf - negative values for maxmsz returns E_PAR" );
    pk_cmbf.bufsz = 100;
    pk_cmbf.maxmsz  = -1;
    status = cre_mbf( 1, &pk_cmbf );
    assert( status == E_PAR );

    puts( "Init - cre_mbf - bufsz < maxmsz returns E_PAR" );
    pk_cmbf.bufsz = 100;
    pk_cmbf.maxmsz  = 101;
    status = cre_mbf( 1, &pk_cmbf );
    assert( status == E_PAR );
  
    /*
     *  Now run through all the bad ID errors
     */

    pk_cmbf.bufsz = 200;
    pk_cmbf.maxmsz = 30;
    puts( "Init - cre_mbf - bad id (less than -4) - E_OACV" );
    status = cre_mbf( -5, &pk_cmbf );
    assert( status == E_OACV );

    puts( "Init - cre_mbf - bad id (between 0 and -4) - E_ID" );
    status = cre_mbf( -4, &pk_cmbf );
    assert( status == E_ID );

    puts( "Init - cre_mbf - bad id (0) - E_ID" );
    status = cre_mbf( 0, &pk_cmbf );
    assert( status == E_ID );

    puts( "Init - cre_mbf - bad id (too great) - E_ID" );
    status = cre_mbf( CONFIGURE_MAXIMUM_ITRON_MESSAGE_BUFFERS + 1, &pk_cmbf );
    assert( status == E_ID );

    puts( "Init - cre_mbf - create mbf 1 TA_TFIFO - E_OK" );
    pk_cmbf.bufsz = 600;
    pk_cmbf.maxmsz  = 200;
    pk_cmbf.mbfatr  = TA_TFIFO;
    status = cre_mbf( 1, &pk_cmbf );
    assert( status == E_OK );
    
    puts( "Init - cre_mbf - create mbf 1 again - E_OBJ" );
    status = cre_mbf( 1, &pk_cmbf );
    assert( status == E_OBJ );

    puts( "Init - cre_mbf - create mbf 2 TA_TPRI - E_OK" );
    pk_cmbf.bufsz = 600;
    pk_cmbf.maxmsz  = 200;
    pk_cmbf.mbfatr  = TA_TPRI;
    status = cre_mbf( 2, &pk_cmbf );
    assert( status == E_OK );
}

void TestDel()
{
    ER    status;
    
    /*
     *  Generate all the bad id errors for del_mbf
     */

    puts( "Init - del_mbf - bad id (less than -4) - E_OACV" );
    status = del_mbf( -5 );
    assert( status == E_OACV );

    puts( "Init - del_mbf - bad id (between 0 and -4) - E_ID" );
    status = del_mbf( -4 );
    assert( status == E_ID );

    puts( "Init - del_mbf - bad id (0) - E_ID" );
    status = del_mbf( 0 );
    assert( status == E_ID );

    puts( "Init - del_mbf - bad id (too great) - E_ID" );
    status = del_mbf( CONFIGURE_MAXIMUM_ITRON_MESSAGE_BUFFERS + 1 );
    assert( status == E_ID );

    
    puts("Init - del_mbf   - E_OK" );
    status = del_mbf(2);
    assert(status == E_OK);
}

void TestPsnd()
{
    ER   status;
    char msg[100] = "This is test message 1";
    
    /*
     *  Generate all the bad id errors for psnd_mbf
     */

    puts( "Init - psnd_mbf - bad id (less than -4) - E_OACV" );
    status = psnd_mbf(-5, msg, sizeof(msg));
    assert( status == E_OACV );

    puts( "Init - psnd_mbf - bad id (between 0 and -4) - E_ID" );
    status = psnd_mbf(-4, msg, sizeof(msg));
    assert( status == E_ID );

    puts( "Init - psnd_mbf - bad id (0) - E_ID" );
    status = psnd_mbf(0, msg, sizeof(msg));
    assert( status == E_ID );

    puts( "Init - psnd_mbf - bad id (too great) - E_ID" );
    status = psnd_mbf(CONFIGURE_MAXIMUM_ITRON_MESSAGE_BUFFERS + 1, msg,
                      sizeof(msg)); 
    assert( status == E_ID );

    puts( "Init - psnd_mbf - msg == 0, E_PAR" );
    status = psnd_mbf(1, 0, sizeof(msg)); 
    assert( status == E_PAR );

    puts( "Init - psnd_mbf - msgsz too big - E_PAR" );
    status = psnd_mbf(1, msg, 300); 
    assert( status == E_PAR );

    puts( "Init - psnd_mbf - msgsz <0 - E_PAR" );
    status = psnd_mbf(1, msg, -10); 
    assert( status == E_PAR );

    puts( "Init - psnd_mbf - E_OK" );
    status = psnd_mbf(1, msg, sizeof(msg)); 
    assert( status == E_OK );
}

void TestRef()
{
    ER       status;
    T_RMBF   pk_rmbf;
    
    puts( "Init - ref_mbf - bad id (less than -4) - E_OACV" );
    status = ref_mbf(&pk_rmbf, -5);
    assert( status == E_OACV );

    puts( "Init - ref_mbf - bad id (between 0 and -4) - E_ID" );
    status = ref_mbf(&pk_rmbf, -4);
    assert( status == E_ID );

    puts( "Init - ref_mbf - bad id (0) - E_ID" );
    status = ref_mbf(&pk_rmbf, 0);
    assert( status == E_ID );

    puts( "Init - ref_mbf - bad id (too great) - E_ID" );
    status = ref_mbf(&pk_rmbf,
                     CONFIGURE_MAXIMUM_ITRON_MESSAGE_BUFFERS + 1);
    assert( status == E_ID );

    puts( "Init - ref_mbf - NULL pk_mbf returns E_PAR" );
    status = ref_mbf( NULL, 1 );
    assert( status == E_PAR );

    puts( "Init - ref_mbf - mbf 1 - E_OK" );
    status = ref_mbf( &pk_rmbf, 1 );
    assert( status == E_OK );
    
    printf( "Init - mbf 1 msgsz = %d\n", pk_rmbf.msgsz );
    printf( "Init - mbf 1 frbufsz = %d\n", pk_rmbf.frbufsz );
    printf( "Init - mbf 1 waiting tasks = %d\n", pk_rmbf.wtsk );
    printf( "Init - mbf 1 sending tasks = %d\n", pk_rmbf.stsk);
}

void TestRcv()
{
    ER    status;
    char  buffer[200];
    int   s;

    puts( "Init - rcv_mbf - bad id (less than -4) - E_OACV" );
    status = rcv_mbf(buffer, &s, -5);
    assert( status == E_OACV );

    puts( "Init - rcv_mbf - bad id (between 0 and -4) - E_ID" );
    status = rcv_mbf(buffer, &s, -4);
    assert( status == E_ID );

    puts( "Init - rcv_mbf - bad id (0) - E_ID" );
    status = rcv_mbf(buffer, &s, 0);
    assert( status == E_ID );

    puts( "Init - rcv_mbf - bad id (too great) - E_ID" );
    status = rcv_mbf(buffer, &s, CONFIGURE_MAXIMUM_ITRON_MESSAGE_BUFFERS + 1);
    assert( status == E_ID );

    puts( "Init - rcv_mbf - NULL msg returns E_PAR" );
    status = rcv_mbf(NULL, &s, 1);
    assert( status == E_PAR );

    puts( "Init - rcv_mbf - NULL p_msgsz returns E_PAR" );
    status = rcv_mbf(buffer, 0, 1);
    assert( status == E_PAR );

    puts("Init - rcv_mbf -- OK");
    status = rcv_mbf(buffer, &s, 1);
    assert(status == E_OK);
    printf("Message:[%s], length:%d\n", buffer, s);
}


void TestPrcv()
{
    ER    status;
    char  buffer[200];
    int   s;

    puts( "Init - prcv_mbf - bad id (less than -4) - E_OACV" );
    status = prcv_mbf(buffer, &s, -5);
    assert( status == E_OACV );

    puts( "Init - prcv_mbf - bad id (between 0 and -4) - E_ID" );
    status = prcv_mbf(buffer, &s, -4);
    assert( status == E_ID );

    puts( "Init - prcv_mbf - bad id (0) - E_ID" );
    status = prcv_mbf(buffer, &s, 0);
    assert( status == E_ID );

    puts( "Init - prcv_mbf - bad id (too great) - E_ID" );
    status = prcv_mbf(buffer, &s, CONFIGURE_MAXIMUM_ITRON_MESSAGE_BUFFERS + 1);
    assert( status == E_ID );

    puts( "Init - prcv_mbf - NULL msg returns E_PAR" );
    status = prcv_mbf(NULL, &s, 1);
    assert( status == E_PAR );

    puts( "Init - prcv_mbf - NULL p_msgsz returns E_PAR" );
    status = prcv_mbf(buffer, 0, 1);
    assert( status == E_PAR );

    puts("Init - prcv_mbf -- E_TMOUT");
    status = prcv_mbf(buffer, &s, 1);
    assert(status == E_TMOUT);
}


void TestTrcv()
{
    ER    status;
    char  buffer[200];
    int   s;

    puts( "Init - trcv_mbf - bad id (less than -4) - E_OACV" );
    status = trcv_mbf(buffer, &s, -5, 5000);
    assert( status == E_OACV );

    puts( "Init - trcv_mbf - bad id (between 0 and -4) - E_ID" );
    status = trcv_mbf(buffer, &s, -4, 5000);
    assert( status == E_ID );

    puts( "Init - trcv_mbf - bad id (0) - E_ID" );
    status = trcv_mbf(buffer, &s, 0, 5000);
    assert( status == E_ID );

    puts( "Init - trcv_mbf - bad id (too great) - E_ID" );
    status = trcv_mbf(buffer, &s,
                      CONFIGURE_MAXIMUM_ITRON_MESSAGE_BUFFERS + 1,
                      5000); 
    assert( status == E_ID );

    puts( "Init - trcv_mbf - NULL msg returns E_PAR" );
    status = trcv_mbf(NULL, &s, 1, 5000);
    assert( status == E_PAR );

    puts( "Init - trcv_mbf - NULL p_msgsz returns E_PAR" );
    status = trcv_mbf(buffer, 0, 1, 5000);
    assert( status == E_PAR );

    puts( "Init - trcv_mbf - tmout <-2  returns E_PAR" );
    status = trcv_mbf(buffer, &s, 1, -2);
    assert( status == E_PAR );

    puts("\nInit - trcv_mbf -- E_TMOUT");
    put_time( "Init - starting to block at ");
    status = trcv_mbf(buffer, &s, 1, 1000);
    assert(status == E_TMOUT);
    put_time( "Init - time out at ");
}


