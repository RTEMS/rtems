/*  bitfield.h
 *
 *  This include file contains all bit field manipulation routines.
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

#ifndef __RTEMS_BITFIELD_h
#define __RTEMS_BITFIELD_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  _Bitfield_Find_first_bit
 *
 *  DESCRIPTION:
 *
 *  This routine returns the bit_number of the first bit set
 *  in the specified value.  The correspondence between bit_number
 *  and actual bit position is processor dependent.  The search for
 *  the first bit set may run from most to least significant bit
 *  or vice-versa.
 *
 *  NOTE:
 *
 *  This routine is used when the executing thread is removed
 *  from the ready state and, as a result, its performance has a
 *  significant impact on the performance of the executive as a whole.
 */

#if ( CPU_USE_GENERIC_BITFIELD_DATA == TRUE )

#ifndef SCORE_INIT
extern const unsigned char __log2table[256];
#else
const unsigned char __log2table[256] = {
    7, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
#endif
 
#endif

#if ( CPU_USE_GENERIC_BITFIELD_CODE == FALSE )

#define _Bitfield_Find_first_bit( _value, _bit_number ) \
        _CPU_Bitfield_Find_first_bit( _value, _bit_number )

#else

/*
 *  The following must be a macro because if a CPU specific version
 *  is used it will most likely use inline assembly.
 */

#define _Bitfield_Find_first_bit( _value, _bit_number ) \
  { \
    register unsigned32 __value = (unsigned32) (_value); \
    register const unsigned char *__p = __log2table; \
    \
    if ( __value < 0x100 ) \
      (_bit_number) = __p[ __value ] + 8; \
    else \
      (_bit_number) = __p[ __value >> 8 ]; \
  }

#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
