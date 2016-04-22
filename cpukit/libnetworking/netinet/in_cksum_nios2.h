
/*
 * Altera Nios2 CRC checksum computation
 *
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
 * COPYRIGHT (c) 1989-2012.
 * On-Line Applications Research Corporation (OAR).
 *
 * Copyright (c) 1997 Mark Brinicome
 * Copyright (c) 1997 Causality Limited
 *
 * Copyright (c) 1995 Zubin Dittia.
 * Copyright (c) 1995 Matthew R. Green.
 * Copyright (c) 1994 Charles M. Hannum.
 * Copyright (c) 1992, 1993
 * The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Based on the arm / sparc version, but using instead
 * mostly inline functions in place of naaasty macros.
 *
 * It would be a great idea to somehow detect at runtime
 * that the Nios2 has a user defined instruction that
 * computes the CRC and invoke it here (we could call a
 * function in the BSP).
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_var.h>
#include <machine/in_cksum.h>

/*
 * Checksum routine for Internet Protocol family headers.
 *
 * This routine is very heavily used in the network
 * code and should be modified for each CPU to be as fast as possible.
 */
static inline uint32_t _NIOS2_Add_ones_complement_64
( uint32_t sum, const uint32_t * const pWd )
{
  sum = _NIOS2_Add_ones_complement ( sum, pWd[0] );
  sum = _NIOS2_Add_ones_complement ( sum, pWd[1] );
  sum = _NIOS2_Add_ones_complement ( sum, pWd[2] );
  sum = _NIOS2_Add_ones_complement ( sum, pWd[3] );
  sum = _NIOS2_Add_ones_complement ( sum, pWd[4] );
  sum = _NIOS2_Add_ones_complement ( sum, pWd[5] );
  sum = _NIOS2_Add_ones_complement ( sum, pWd[6] );
  sum = _NIOS2_Add_ones_complement ( sum, pWd[7] );
  sum = _NIOS2_Add_ones_complement ( sum, pWd[8] );
  sum = _NIOS2_Add_ones_complement ( sum, pWd[9] );
  sum = _NIOS2_Add_ones_complement ( sum, pWd[10] );
  sum = _NIOS2_Add_ones_complement ( sum, pWd[11] );
  sum = _NIOS2_Add_ones_complement ( sum, pWd[12] );
  sum = _NIOS2_Add_ones_complement ( sum, pWd[13] );
  sum = _NIOS2_Add_ones_complement ( sum, pWd[14] );
  return _NIOS2_Add_ones_complement ( sum, pWd[15] );
}

static inline uint32_t _NIOS2_Add_ones_complement_32
( uint32_t sum, const uint32_t * const pWd )
{
  sum = _NIOS2_Add_ones_complement ( sum, pWd[0] );
  sum = _NIOS2_Add_ones_complement ( sum, pWd[1] );
  sum = _NIOS2_Add_ones_complement ( sum, pWd[2] );
  sum = _NIOS2_Add_ones_complement ( sum, pWd[3] );
  sum = _NIOS2_Add_ones_complement ( sum, pWd[4] );
  sum = _NIOS2_Add_ones_complement ( sum, pWd[5] );
  sum = _NIOS2_Add_ones_complement ( sum, pWd[6] );
  return _NIOS2_Add_ones_complement ( sum, pWd[7] );
}

static inline uint32_t _NIOS2_Add_ones_complement_16
( uint32_t sum, const uint32_t * const pWd )
{
  sum = _NIOS2_Add_ones_complement ( sum, pWd[0] );
  sum = _NIOS2_Add_ones_complement ( sum, pWd[1] );
  sum = _NIOS2_Add_ones_complement ( sum, pWd[2] );
  return _NIOS2_Add_ones_complement ( sum, pWd[3] );
}

static inline uint32_t _NIOS2_Add_ones_complement_8
( uint32_t sum, const uint32_t * const pWd )
{
  sum = _NIOS2_Add_ones_complement ( sum, pWd[0] );
  return _NIOS2_Add_ones_complement ( sum, pWd[1] );
}

static inline uint32_t _NIOS2_Add_ones_complement_4
( uint32_t sum, const uint32_t * const pWd )
{
  return _NIOS2_Add_ones_complement ( sum, pWd[0] );
}

static inline uint32_t _NIOS2_Reduce_checksum ( uint32_t a )
{
  uint32_t tmp;
  __asm__ __volatile__ (
    "   srli    %1, %0, 16      \n" /* tmp = a >> 16 */
    "   andi    %0, %0, 0xffff  \n" /* a = a & 0xffff */
    "   add     %0, %0, %1      \n" /* a = a + tmp */
  : "+&r"(a), "=&r"(tmp)
  );
  return a;
}

#define combineTokens( A, B ) A ## B

#define ADD_AND_ADVANCE( N ) \
if ( mlen >= N ) { \
  sum = combineTokens ( _NIOS2_Add_ones_complement_,  N ) \
                      ( sum, ( uint32_t * ) w ); \
  mlen -= N; \
  w += N; \
}

static int
in_cksum_internal(struct mbuf *m, int off, int len, u_int sum)
{
  const uint8_t * w;
  int mlen = 0;
  int byte_swapped = 0;

  for (; m && len; m = m->m_next)
  {
    if (m->m_len == 0)
      continue;
    w = mtod(m, u_char *) + off;
    mlen = m->m_len - off;
    off = 0;
    if (len < mlen)
      mlen = len;
    len -= mlen;

    /*
    * Ensure that we're aligned on a word boundary here so
    * that we can do 32 bit operations below.
    */
    if ((3 & (uint32_t)w) != 0)
    {
      sum = _NIOS2_Reduce_checksum ( sum );
      if ((1 & (uint32_t)w) != 0 && mlen >= 1)
      {
        sum <<= 8u;
        sum += *w << 8u;
        byte_swapped ^= 1;
        w += 1;
        mlen -= 1;
      }
      if ((2 & (uint32_t)w) != 0 && mlen >= 2)
      {
        sum += *(uint16_t *)w;
        w += 2;
        mlen -= 2;
      }
    }

    /*
     * instead of using a loop, process in unrolled chunks
     */
    while ( mlen >= 64 )
    {
      sum = _NIOS2_Add_ones_complement_64
            ( sum, ( uint32_t * ) w );
      mlen -= 64;
      w += 64;
    }
    ADD_AND_ADVANCE ( 32 );
    ADD_AND_ADVANCE ( 16 );
    ADD_AND_ADVANCE (  8 );
    ADD_AND_ADVANCE (  4 );

    if ( mlen > 0 )
    {
      sum = _NIOS2_Reduce_checksum ( sum );
      if ( mlen >= 2 )
      {
        sum += *(uint16_t *)w;
        w += 2;
        mlen -= 2;
      }
      if ( mlen == 1 )
      {
        sum <<= 8u;
        sum += *w << 8u;
        byte_swapped ^= 1;
      }
    }
  }
  if ( byte_swapped )
  {
    sum = _NIOS2_Reduce_checksum ( sum );
    sum <<= 8u;
  }
  sum = _NIOS2_Add_ones_complement_word_halves ( sum );
  sum ^= 0xffff;
  return sum;
}

int
in_cksum (
  struct mbuf *m,
  int len )
{
  return in_cksum_internal ( m, 0, len, 0 );
}
