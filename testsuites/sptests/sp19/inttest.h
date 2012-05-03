/*
 *  This include file contains the CPU dependent implementation
 *  of the following routines needed to test RTEMS floating
 *  point support:
 *           INTEGER_load( &context )
 *           INTEGER_check( &context )
 *
 *  INTEGER_load   - loads the specified floating point context
 *  INTEGER_check  - checks the specified floating point context
 *
 *  NOTE:  These routines are VERY CPU dependent and are thus
 *         located in in the CPU dependent include file
 *         inttest.h.  These routines form the core of the
 *         floating point context switch test.
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */


#ifndef __INTEGER_TEST_h
#define __INTEGER_TEST_h

#include <stdio.h>
#define REG_VARIABLE uint32_t

#define INTEGER_DECLARE \
    REG_VARIABLE int01 = 1; \
    REG_VARIABLE int02 = 2; \
    REG_VARIABLE int03 = 3; \
    REG_VARIABLE int04 = 4; \
    REG_VARIABLE int05 = 5; \
    REG_VARIABLE int06 = 6; \
    REG_VARIABLE int07 = 7; \
    REG_VARIABLE int08 = 8; \
    REG_VARIABLE int09 = 9; \
    REG_VARIABLE int10 = 10; \
    REG_VARIABLE int11 = 11; \
    REG_VARIABLE int12 = 12; \
    REG_VARIABLE int13 = 13; \
    REG_VARIABLE int14 = 14; \
    REG_VARIABLE int15 = 15; \
    REG_VARIABLE int16 = 16; \
    REG_VARIABLE int17 = 17; \
    REG_VARIABLE int18 = 18; \
    REG_VARIABLE int19 = 19; \
    REG_VARIABLE int20 = 20; \
    REG_VARIABLE int21 = 21; \
    REG_VARIABLE int22 = 22; \
    REG_VARIABLE int23 = 23; \
    REG_VARIABLE int24 = 24; \
    REG_VARIABLE int25 = 25; \
    REG_VARIABLE int26 = 26; \
    REG_VARIABLE int27 = 27; \
    REG_VARIABLE int28 = 28; \
    REG_VARIABLE int29 = 29; \
    REG_VARIABLE int30 = 30; \
    REG_VARIABLE int31 = 31; \
    REG_VARIABLE int32 = 32

#define INTEGER_LOAD( _factor ) \
  do {                \
    int01  += _factor; \
    int02  += _factor; \
    int03  += _factor; \
    int04  += _factor; \
    int05  += _factor; \
    int06  += _factor; \
    int07  += _factor; \
    int08  += _factor; \
    int09  += _factor; \
    int10  += _factor; \
    int11  += _factor; \
    int12  += _factor; \
    int13  += _factor; \
    int14  += _factor; \
    int15  += _factor; \
    int16  += _factor; \
    int17  += _factor; \
    int18  += _factor; \
    int19  += _factor; \
    int20  += _factor; \
    int21  += _factor; \
    int22  += _factor; \
    int23  += _factor; \
    int24  += _factor; \
    int25  += _factor; \
    int26  += _factor; \
    int27  += _factor; \
    int28  += _factor; \
    int29  += _factor; \
    int30  += _factor; \
    int31  += _factor; \
    int32  += _factor; \
  } while (0)

#define INTEGER_CHECK_ONE( _v, _base, _factor ) \
  if ( (_v) != ((_base) + (_factor)) )  { \
    printf( \
      "%" PRIu32 ": " #_v " wrong -- (0x%" PRIx32 " not 0x%" PRIx32 ")\n", \
      task_index, \
      (_v), \
      (_base + _factor) \
    ); \
  }


#define INTEGER_CHECK( _factor ) \
    do { \
      INTEGER_CHECK_ONE( int01,  1, (_factor) ); \
      INTEGER_CHECK_ONE( int02,  2, (_factor) ); \
      INTEGER_CHECK_ONE( int03,  3, (_factor) ); \
      INTEGER_CHECK_ONE( int04,  4, (_factor) ); \
      INTEGER_CHECK_ONE( int05,  5, (_factor) ); \
      INTEGER_CHECK_ONE( int06,  6, (_factor) ); \
      INTEGER_CHECK_ONE( int07,  7, (_factor) ); \
      INTEGER_CHECK_ONE( int08,  8, (_factor) ); \
      INTEGER_CHECK_ONE( int09,  9, (_factor) ); \
      INTEGER_CHECK_ONE( int10, 10, (_factor) ); \
      INTEGER_CHECK_ONE( int11, 11, (_factor) ); \
      INTEGER_CHECK_ONE( int12, 12, (_factor) ); \
      INTEGER_CHECK_ONE( int13, 13, (_factor) ); \
      INTEGER_CHECK_ONE( int14, 14, (_factor) ); \
      INTEGER_CHECK_ONE( int15, 15, (_factor) ); \
      INTEGER_CHECK_ONE( int16, 16, (_factor) ); \
      INTEGER_CHECK_ONE( int17, 17, (_factor) ); \
      INTEGER_CHECK_ONE( int18, 18, (_factor) ); \
      INTEGER_CHECK_ONE( int19, 19, (_factor) ); \
      INTEGER_CHECK_ONE( int20, 20, (_factor) ); \
      INTEGER_CHECK_ONE( int21, 21, (_factor) ); \
      INTEGER_CHECK_ONE( int22, 22, (_factor) ); \
      INTEGER_CHECK_ONE( int23, 23, (_factor) ); \
      INTEGER_CHECK_ONE( int24, 24, (_factor) ); \
      INTEGER_CHECK_ONE( int25, 25, (_factor) ); \
      INTEGER_CHECK_ONE( int26, 26, (_factor) ); \
      INTEGER_CHECK_ONE( int27, 27, (_factor) ); \
      INTEGER_CHECK_ONE( int28, 28, (_factor) ); \
      INTEGER_CHECK_ONE( int29, 29, (_factor) ); \
      INTEGER_CHECK_ONE( int30, 30, (_factor) ); \
      INTEGER_CHECK_ONE( int31, 31, (_factor) ); \
      INTEGER_CHECK_ONE( int32, 32, (_factor) ); \
    } while (0)

#endif
