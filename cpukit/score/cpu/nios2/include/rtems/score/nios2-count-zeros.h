/*
 * Author: Jeffrey O. Hill
 *
 * Copyright 2012. Los Alamos National Security, LLC.
 * This material was produced under U.S. Government contract
 * DE-AC52-06NA25396 for Los Alamos National Laboratory (LANL),
 * which is operated by Los Alamos National Security, LLC for
 * the U.S. Department of Energy. The U.S. Government has rights
 * to use, reproduce, and distribute this software.  NEITHER THE
 * GOVERNMENT NOR LOS ALAMOS NATIONAL SECURITY, LLC MAKES ANY
 * WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY FOR
 * THE USE OF THIS SOFTWARE.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _NIOS2_COUNT_ZEROS_H
#define _NIOS2_COUNT_ZEROS_H

#include <stdint.h>

#include <rtems/score/bitfield.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * This implementation is currently much more efficient than
 * the GCC provided __builtin_clz
 */
static inline unsigned _Nios2_Count_leading_zeros( uint32_t p )
{
  unsigned bitIdx;

  if ( p <= 0xffffu ) {
    if ( p < 0x100u ) {
      bitIdx = _Bitfield_Leading_zeros[ p ] + 24u;
    } else {
      bitIdx = _Bitfield_Leading_zeros[ p >> 8u ] + 16u;
    }
  } else {
    p >>= 16u;

    if ( p < 0x100u ) {
      bitIdx = _Bitfield_Leading_zeros[ p ] + 8u;
    } else {
      bitIdx = _Bitfield_Leading_zeros[ p >> 8u ];
    }
  }

  return bitIdx;
}

/*
 * This implementation is currently much more efficient than
 * the GCC provided __builtin_ctz
 */
static inline unsigned _Nios2_Count_trailing_zeros( uint32_t p )
{
  return 31u - _Nios2_Count_leading_zeros( p & ( -p ) );
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _NIOS2_COUNT_ZEROS_H */
