/*  bitfield.h
 *
 *  This include file contains all bit field manipulation routines.
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

#define _Bitfield_Find_first_bit( _value, _bit_number ) \
        _CPU_Bitfield_Find_first_bit( _value, _bit_number )

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
