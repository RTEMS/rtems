/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtems_rfs
 *
 * @brief RTEMS File Systems Directory Hash function
 */

/*
 * Copyright (C) 2010 Chris Johns <chrisj@rtems.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/basedefs.h>
#include <rtems/rfs/rtems-rfs-dir-hash.h>

#ifdef __rtems__
# include <machine/endian.h>    /* attempt to define endianness */
#endif
#ifdef linux
# include <endian.h>    /* attempt to define endianness */
#endif

/*
 * My best guess at if you are big-endian or little-endian.  This may
 * need adjustment.
 */
#if (defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && \
     __BYTE_ORDER == __LITTLE_ENDIAN) || \
  (defined(i386) || defined(__i386__) || defined(__i486__) || \
   defined(__i586__) || defined(__i686__) || defined(vax) || defined(MIPSEL))
# define HASH_LITTLE_ENDIAN 1
# define HASH_BIG_ENDIAN 0
#elif (defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && \
       __BYTE_ORDER == __BIG_ENDIAN) || \
  (defined(sparc) || defined(POWERPC) || defined(mc68000) || defined(sel))
# define HASH_LITTLE_ENDIAN 0
# define HASH_BIG_ENDIAN 1
#else
# define HASH_LITTLE_ENDIAN 0
# define HASH_BIG_ENDIAN 0
#endif

#define hashsize(n) ((uint32_t)1<<(n))
#define hashmask(n) (hashsize(n)-1)
#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

/*
  -------------------------------------------------------------------------------
  mix -- mix 3 32-bit values reversibly.

  This is reversible, so any information in (a,b,c) before mix() is still in
  (a,b,c) after mix().

  If four pairs of (a,b,c) inputs are run through mix(), or through mix() in
  reverse, there are at least 32 bits of the output that are sometimes the same
  for one pair and different for another pair.  This was tested for:

  * pairs that differed by one bit, by two bits, in any combination of top bits
    of (a,b,c), or in any combination of bottom bits of (a,b,c).

  * "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed the
    output delta to a Gray code (a^(a>>1)) so a string of 1's (as is commonly
    produced by subtraction) look like a single 1-bit difference.

  * the base values were pseudorandom, all zero but one bit set, or all zero
    plus a counter that starts at zero.

  Some k values for my "a-=c; a^=rot(c,k); c+=b;" arrangement that satisfy this
  are:

     4  6  8 16 19  4
     9 15  3 18 27 15
    14  9  3  7 17  3

  Well, "9 15 3 18 27 15" didn't quite get 32 bits diffing for "differ" defined
  as + with a one-bit base and a two-bit delta.  I used
  http://burtleburtle.net/bob/hash/avalanche.html to choose the operations,
  constants, and arrangements of the variables.

  This does not achieve avalanche.  There are input bits of (a,b,c) that fail
  to affect some output bits of (a,b,c), especially of a.  The most thoroughly
  mixed value is c, but it doesn't really even achieve avalanche in c.

  This allows some parallelism.  Read-after-writes are good at doubling the
  number of bits affected, so the goal of mixing pulls in the opposite
  direction as the goal of parallelism.  I did what I could.  Rotates seem to
  cost as much as shifts on every machine I could lay my hands on, and rotates
  are much kinder to the top and bottom bits, so I used rotates.
  -------------------------------------------------------------------------------
*/
#define mix(a,b,c) \
  { \
  a -= c;  a ^= rot(c, 4);  c += b; \
  b -= a;  b ^= rot(a, 6);  a += c; \
  c -= b;  c ^= rot(b, 8);  b += a; \
  a -= c;  a ^= rot(c,16);  c += b; \
  b -= a;  b ^= rot(a,19);  a += c; \
  c -= b;  c ^= rot(b, 4);  b += a; \
  }

/*
  -------------------------------------------------------------------------------
  final -- final mixing of 3 32-bit values (a,b,c) into c

  Pairs of (a,b,c) values differing in only a few bits will usually produce
  values of c that look totally different.  This was tested for

  * pairs that differed by one bit, by two bits, in any combination of top bits
    of (a,b,c), or in any combination of bottom bits of (a,b,c).

  * "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed the
    output delta to a Gray code (a^(a>>1)) so a string of 1's (as is commonly
    produced by subtraction) look like a single 1-bit difference.  * the base
    values were pseudorandom, all zero but one bit set, or all zero plus a
    counter that starts at zero.

  These constants passed:
    14 11 25 16 4 14 24
    12 14 25 16 4 14 24
  and these came close:
     4  8 15 26 3 22 24
    10  8 15 26 3 22 24
    11  8 15 26 3 22 24
  -------------------------------------------------------------------------------
*/
#define final(a,b,c)        \
  {                         \
    c ^= b; c -= rot(b,14); \
    a ^= c; a -= rot(c,11); \
    b ^= a; b -= rot(a,25); \
    c ^= b; c -= rot(b,16); \
    a ^= c; a -= rot(c,4);  \
    b ^= a; b -= rot(a,14); \
    c ^= b; c -= rot(b,24); \
  }

/**
 * The follow is the documentation from Bob Jenkin's hash function:
 *
 *  http://burtleburtle.net/bob/hash/doobs.html
 *
 * The function hashlittle() has been renamed.
 *
 *  hashlittle() -- hash a variable-length key into a 32-bit value
 *
 *   k       : the key (the unaligned variable-length array of bytes)
 *   length  : the length of the key, counting by bytes
 *   initval : can be any 4-byte value
 *
 *  Returns a 32-bit value.  Every bit of the key affects every bit of the
 *  return value.  Two keys differing by one or two bits will have totally
 *  different hash values.
 *
 *  The best hash table sizes are powers of 2.  There is no need to do mod a
 *  prime (mod is sooo slow!).  If you need less than 32 bits, use a bitmask.
 *  For example, if you need only 10 bits, do h = (h & hashmask(10)); In which
 *  case, the hash table should have hashsize(10) elements.
 *
 *  If you are hashing n strings (uint8_t **)k, do it like this: for (i=0, h=0;
 *  i<n; ++i) h = hashlittle( k[i], len[i], h);
 *
 *  By Bob Jenkins, 2006.  bob_jenkins@burtleburtle.net.  You may use this
 *  code any way you wish, private, educational, or commercial.  It's free.
 *
 *  Use for hash table lookup, or anything where one collision in 2^^32 is
 *  acceptable.  Do NOT use for cryptographic purposes.
*/

#define initval (20010928)
uint32_t
rtems_rfs_dir_hash (const void *key, size_t length)
{
  uint32_t a,b,c;                             /* internal state */
  union { const void *ptr; size_t i; } u;     /* needed for Mac Powerbook G4 */

  /* Set up the internal state */
  a = b = c = 0xdeadbeef + ((uint32_t)length) + initval;

  u.ptr = key;
  if (HASH_LITTLE_ENDIAN && ((u.i & 0x3) == 0)) {
    const uint32_t *k = (const uint32_t *)key;         /* read 32-bit chunks */
    /*const uint8_t  *k8;*/

    /*------ all but last block: aligned reads and affect 32 bits of (a,b,c) */
    while (length > 12)
    {
      a += k[0];
      b += k[1];
      c += k[2];
      mix(a,b,c);
      length -= 12;
      k += 3;
    }

    /*----------------------------- handle the last (probably partial) block */
    /*
     * "k[2]&0xffffff" actually reads beyond the end of the string, but
     * then masks off the part it's not allowed to read.  Because the
     * string is aligned, the masked-off tail is in the same word as the
     * rest of the string.  Every machine with memory protection I've seen
     * does it on word boundaries, so is OK with this.  But VALGRIND will
     * still catch it and complain.  The masking trick does make the hash
     * noticably faster for short strings (like English words).
     */
#ifndef VALGRIND

    switch(length)
    {
      case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
      case 11: c+=k[2]&0xffffff; b+=k[1]; a+=k[0]; break;
      case 10: c+=k[2]&0xffff; b+=k[1]; a+=k[0]; break;
      case 9 : c+=k[2]&0xff; b+=k[1]; a+=k[0]; break;
      case 8 : b+=k[1]; a+=k[0]; break;
      case 7 : b+=k[1]&0xffffff; a+=k[0]; break;
      case 6 : b+=k[1]&0xffff; a+=k[0]; break;
      case 5 : b+=k[1]&0xff; a+=k[0]; break;
      case 4 : a+=k[0]; break;
      case 3 : a+=k[0]&0xffffff; break;
      case 2 : a+=k[0]&0xffff; break;
      case 1 : a+=k[0]&0xff; break;
      case 0 : return c;              /* zero length strings require no mixing */
    }

#else /* make valgrind happy */

    k8 = (const uint8_t *)k;
    switch(length)
    {
      case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
      case 11: c+=((uint32_t)k8[10])<<16;  /* fall through */
      case 10: c+=((uint32_t)k8[9])<<8;    /* fall through */
      case 9 : c+=k8[8];                   /* fall through */
      case 8 : b+=k[1]; a+=k[0]; break;
      case 7 : b+=((uint32_t)k8[6])<<16;   /* fall through */
      case 6 : b+=((uint32_t)k8[5])<<8;    /* fall through */
      case 5 : b+=k8[4];                   /* fall through */
      case 4 : a+=k[0]; break;
      case 3 : a+=((uint32_t)k8[2])<<16;   /* fall through */
      case 2 : a+=((uint32_t)k8[1])<<8;    /* fall through */
      case 1 : a+=k8[0]; break;
      case 0 : return c;
    }

#endif /* !valgrind */

  } else if (HASH_LITTLE_ENDIAN && ((u.i & 0x1) == 0)) {
    const uint16_t *k = (const uint16_t *)key;         /* read 16-bit chunks */
    const uint8_t  *k8;

    /*--------------- all but last block: aligned reads and different mixing */
    while (length > 12)
    {
      a += k[0] + (((uint32_t)k[1])<<16);
      b += k[2] + (((uint32_t)k[3])<<16);
      c += k[4] + (((uint32_t)k[5])<<16);
      mix(a,b,c);
      length -= 12;
      k += 6;
    }

    /*----------------------------- handle the last (probably partial) block */
    k8 = (const uint8_t *)k;
    switch(length)
    {
      case 12: c+=k[4]+(((uint32_t)k[5])<<16);
        b+=k[2]+(((uint32_t)k[3])<<16);
        a+=k[0]+(((uint32_t)k[1])<<16);
        break;
      case 11: c+=((uint32_t)k8[10])<<16;     /* fall through */
      case 10: c+=k[4];
        b+=k[2]+(((uint32_t)k[3])<<16);
        a+=k[0]+(((uint32_t)k[1])<<16);
        break;
      case 9 : c+=k8[8];                      /* fall through */
      case 8 : b+=k[2]+(((uint32_t)k[3])<<16);
        a+=k[0]+(((uint32_t)k[1])<<16);
        break;
      case 7 : b+=((uint32_t)k8[6])<<16;      /* fall through */
      case 6 : b+=k[2];
        a+=k[0]+(((uint32_t)k[1])<<16);
        break;
      case 5 : b+=k8[4];                      /* fall through */
      case 4 : a+=k[0]+(((uint32_t)k[1])<<16);
        break;
      case 3 : a+=((uint32_t)k8[2])<<16;      /* fall through */
      case 2 : a+=k[0];
        break;
      case 1 : a+=k8[0];
        break;
      case 0 : return c;                     /* zero length requires no mixing */
    }

  } else {                        /* need to read the key one byte at a time */
    const uint8_t *k = (const uint8_t *)key;

    /*--------------- all but the last block: affect some 32 bits of (a,b,c) */
    while (length > 12)
    {
      a += k[0];
      a += ((uint32_t)k[1])<<8;
      a += ((uint32_t)k[2])<<16;
      a += ((uint32_t)k[3])<<24;
      b += k[4];
      b += ((uint32_t)k[5])<<8;
      b += ((uint32_t)k[6])<<16;
      b += ((uint32_t)k[7])<<24;
      c += k[8];
      c += ((uint32_t)k[9])<<8;
      c += ((uint32_t)k[10])<<16;
      c += ((uint32_t)k[11])<<24;
      mix(a,b,c);
      length -= 12;
      k += 12;
    }

    /*-------------------------------- last block: affect all 32 bits of (c) */
    switch(length)                   /* all the case statements fall through */
    {
      case 12: c+=((uint32_t)k[11])<<24; RTEMS_FALL_THROUGH();
      case 11: c+=((uint32_t)k[10])<<16; RTEMS_FALL_THROUGH();
      case 10: c+=((uint32_t)k[9])<<8; RTEMS_FALL_THROUGH();
      case 9 : c+=k[8]; RTEMS_FALL_THROUGH();
      case 8 : b+=((uint32_t)k[7])<<24; RTEMS_FALL_THROUGH();
      case 7 : b+=((uint32_t)k[6])<<16; RTEMS_FALL_THROUGH();
      case 6 : b+=((uint32_t)k[5])<<8; RTEMS_FALL_THROUGH();
      case 5 : b+=k[4]; RTEMS_FALL_THROUGH();
      case 4 : a+=((uint32_t)k[3])<<24; RTEMS_FALL_THROUGH();
      case 3 : a+=((uint32_t)k[2])<<16; RTEMS_FALL_THROUGH();
      case 2 : a+=((uint32_t)k[1])<<8; RTEMS_FALL_THROUGH();
      case 1 : a+=k[0];
        break;
      case 0 : return c;
    }
  }

  final(a,b,c);
  return c;
}

