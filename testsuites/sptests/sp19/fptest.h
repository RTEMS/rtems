/*  fptest.h
 *
 *  This include file contains the CPU dependent implementation
 *  of the following routines needed to test RTEMS floating
 *  point support:
 *           FP_load( &context )
 *           FP_check( &context )
 *
 *  FP_load   - loads the specified floating point context
 *  FP_check  - checks the specified floating point context
 *
 *  NOTE:  These routines are VERY CPU dependent and are thus
 *         located in in the CPU dependent include file
 *         fptest.h.  These routines form the core of the
 *         floating point context switch test.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */


#ifndef __FPTEST_h
#define __FPTEST_h

#include <stdio.h>

#ifndef RTEMS_HAS_HARDWARE_FP
#error "This CPU does not have RTEMS_HAS_HARDWARE_FP defined"
#endif

#if ( RTEMS_HAS_HARDWARE_FP == FALSE )

/*
 *  The following is useless except to avoid some warnings.
 */

#define FP_DECLARE unsigned int fp01 = 0;
#define FP_LOAD( _factor ) fp01 = 2;
#define FP_CHECK( _factor ) \
   if ( fp01 != 2 ) \
          printf("%" PRIu32 ": single integer is wrong -- (%d != 2) \n", \
             task_index, fp01 );  \

#else

#define FP_DECLARE \
    double  fp01 = 1.0; \
    double  fp02 = 2.0; \
    double  fp03 = 3.0; \
    double  fp04 = 4.0; \
    double  fp05 = 5.0; \
    double  fp06 = 6.0; \
    double  fp07 = 7.0; \
    double  fp08 = 8.0; \
    double  fp09 = 9.0; \
    double  fp10 = 10.0; \
    double  fp11 = 11.0; \
    double  fp12 = 12.0; \
    double  fp13 = 13.0; \
    double  fp14 = 14.0; \
    double  fp15 = 15.0; \
    double  fp16 = 16.0; \
    double  fp17 = 17.0; \
    double  fp18 = 18.0; \
    double  fp19 = 19.0; \
    double  fp20 = 20.0; \
    double  fp21 = 21.0; \
    double  fp22 = 22.0; \
    double  fp23 = 23.0; \
    double  fp24 = 24.0; \
    double  fp25 = 25.0; \
    double  fp26 = 26.0; \
    double  fp27 = 27.0; \
    double  fp28 = 28.0; \
    double  fp29 = 29.0; \
    double  fp30 = 30.0; \
    double  fp31 = 31.0; \
    double  fp32 = 32.0

#define FP_LOAD( _factor ) \
  do {                \
    fp01  += _factor; \
    fp02  += _factor; \
    fp03  += _factor; \
    fp04  += _factor; \
    fp05  += _factor; \
    fp06  += _factor; \
    fp07  += _factor; \
    fp08  += _factor; \
    fp09  += _factor; \
    fp10  += _factor; \
    fp11  += _factor; \
    fp12  += _factor; \
    fp13  += _factor; \
    fp14  += _factor; \
    fp15  += _factor; \
    fp16  += _factor; \
    fp17  += _factor; \
    fp18  += _factor; \
    fp19  += _factor; \
    fp20  += _factor; \
    fp21  += _factor; \
    fp22  += _factor; \
    fp23  += _factor; \
    fp24  += _factor; \
    fp25  += _factor; \
    fp26  += _factor; \
    fp27  += _factor; \
    fp28  += _factor; \
    fp29  += _factor; \
    fp30  += _factor; \
    fp31  += _factor; \
    fp32  += _factor; \
  } while (0)

#define EPSILON    (0.0005)
#define FPABS(d)   (((d) < 0.0) ? -(d) : (d))
#define FPNEQ(a,b) (FPABS((a)-(b)) > EPSILON)

#define FP_CHECK_ONE( _v, _base, _factor ) \
      if ( FPNEQ( (_v), ((_base) + (_factor)) ) )  { \
          printf("%" PRIu32 ": " #_v " wrong -- (%g not %g)\n", \
             task_index, (_v), (_base + _factor));  \
      }


#define FP_CHECK( _factor ) \
    do { \
      FP_CHECK_ONE( fp01,  1.0, (_factor) ); \
      FP_CHECK_ONE( fp02,  2.0, (_factor) ); \
      FP_CHECK_ONE( fp03,  3.0, (_factor) ); \
      FP_CHECK_ONE( fp04,  4.0, (_factor) ); \
      FP_CHECK_ONE( fp05,  5.0, (_factor) ); \
      FP_CHECK_ONE( fp06,  6.0, (_factor) ); \
      FP_CHECK_ONE( fp07,  7.0, (_factor) ); \
      FP_CHECK_ONE( fp08,  8.0, (_factor) ); \
      FP_CHECK_ONE( fp09,  9.0, (_factor) ); \
      FP_CHECK_ONE( fp10, 10.0, (_factor) ); \
      FP_CHECK_ONE( fp11, 11.0, (_factor) ); \
      FP_CHECK_ONE( fp12, 12.0, (_factor) ); \
      FP_CHECK_ONE( fp13, 13.0, (_factor) ); \
      FP_CHECK_ONE( fp14, 14.0, (_factor) ); \
      FP_CHECK_ONE( fp15, 15.0, (_factor) ); \
      FP_CHECK_ONE( fp16, 16.0, (_factor) ); \
      FP_CHECK_ONE( fp17, 17.0, (_factor) ); \
      FP_CHECK_ONE( fp18, 18.0, (_factor) ); \
      FP_CHECK_ONE( fp19, 19.0, (_factor) ); \
      FP_CHECK_ONE( fp20, 20.0, (_factor) ); \
      FP_CHECK_ONE( fp21, 21.0, (_factor) ); \
      FP_CHECK_ONE( fp22, 22.0, (_factor) ); \
      FP_CHECK_ONE( fp23, 23.0, (_factor) ); \
      FP_CHECK_ONE( fp24, 24.0, (_factor) ); \
      FP_CHECK_ONE( fp25, 25.0, (_factor) ); \
      FP_CHECK_ONE( fp26, 26.0, (_factor) ); \
      FP_CHECK_ONE( fp27, 27.0, (_factor) ); \
      FP_CHECK_ONE( fp28, 28.0, (_factor) ); \
      FP_CHECK_ONE( fp29, 29.0, (_factor) ); \
      FP_CHECK_ONE( fp30, 30.0, (_factor) ); \
      FP_CHECK_ONE( fp31, 31.0, (_factor) ); \
      FP_CHECK_ONE( fp32, 32.0, (_factor) ); \
    } while (0)

#endif

#endif
