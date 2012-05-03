/**
 *  @file  rtems/score/bitfield.h
 *
 *  This include file contains all bit field manipulation routines.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_BITFIELD_H
#define _RTEMS_SCORE_BITFIELD_H

/**
 *  @defgroup ScoreBitfield Bitfield Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality that is used to manipulate the
 *  priority bitfields used to lookup which priority has the highest
 *  priority ready to execute thread.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#if ( CPU_USE_GENERIC_BITFIELD_DATA == TRUE )

/**
 *  This table is used by the generic bitfield routines to perform
 *  a highly optimized bit scan without the use of special CPU
 *  instructions.
 */
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

/**
 *  This routine returns the @a _bit_number of the first bit set
 *  in the specified value.  The correspondence between @a _bit_number
 *  and actual bit position is processor dependent.  The search for
 *  the first bit set may run from most to least significant bit
 *  or vice-versa.
 *
 *  @param[in] _value is the value to bit scan.
 *  @param[in] _bit_number is the position of the first bit set.
 *
 *  @note This routine is used when the executing thread is removed
 *  from the ready state and, as a result, its performance has a
 *  significant impact on the performance of the executive as a whole.
 *
 *  @note This routine must be a macro because if a CPU specific version
 *  is used it will most likely use inline assembly.
 */
#if ( CPU_USE_GENERIC_BITFIELD_CODE == FALSE )
#define _Bitfield_Find_first_bit( _value, _bit_number ) \
        _CPU_Bitfield_Find_first_bit( _value, _bit_number )
#else
#define _Bitfield_Find_first_bit( _value, _bit_number ) \
  { \
    register uint32_t   __value = (uint32_t) (_value); \
    register const unsigned char *__p = __log2table; \
    \
    if ( __value < 0x100 ) \
      (_bit_number) = (Priority_bit_map_Control)( __p[ __value ] + 8 );  \
    else \
      (_bit_number) = (Priority_bit_map_Control)( __p[ __value >> 8 ] ); \
  }
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
