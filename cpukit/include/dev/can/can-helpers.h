/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file
 *
 * @ingroup CANFDStack
 *
 * @brief This header file is part of CAN/CAN FD bus common support. It
 *        implements functions and defines used to simplify controller's
 *        operations.
 *
 * Implementation is based on original LinCAN - Linux CAN bus driver
 * Part of OrtCAN project https://ortcan.sourceforge.net/
 */

/*
 * Copyright (C) 2023-2024 Michal Lenc <michallenc@seznam.cz>
 * Copyright (C) 2002-2009 DCE FEE CTU Prague
 * Copyright (C) 2002-2024 Pavel Pisa <pisa@cmp.felk.cvut.cz>
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

#ifndef _DEV_CAN_CAN_HELPERS_H
#define _DEV_CAN_CAN_HELPERS_H

#include <rtems.h>
#include <stdatomic.h>

/**
 * @brief This define provides user magic value. It is used to check whether
 *  @ref rtems_can_user structure was correctly initialized.
 */
#define RTEMS_CAN_USER_MAGIC 0x05402033

#ifndef BIT
#define BIT(nr)			(1UL << (nr))
#endif

#define GENMASK(h, l) (((~0UL) << (l)) & (~0UL >> (sizeof(long) * 8 - 1 - (h))))

#ifndef __bf_shf
#define __bf_shf(x) (__builtin_ffsll(x) - 1)
#endif

#ifndef FIELD_PREP
#define FIELD_PREP(_mask, _val)						\
	({								\
		((typeof(_mask))(_val) << __bf_shf(_mask)) & (_mask);	\
	})
#endif

#ifndef FIELD_GET
#define FIELD_GET(_mask, _reg)						\
	({								\
		(typeof(_mask))(((_reg) & (_mask)) >> __bf_shf(_mask));	\
	})
#endif

static const uint8_t rtems_can_len2dlc[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8,  /* 0 - 8 */
                                             9, 9, 9, 9, /* 9 - 12 */
                                             10, 10, 10, 10, /* 13 - 16 */
                                             11, 11, 11, 11, /* 17 - 20 */
                                             12, 12, 12, 12, /* 21 - 24 */
                                             13, 13, 13, 13, 13, 13, 13, 13, /* 25 - 32 */
                                             14, 14, 14, 14, 14, 14, 14, 14, /* 33 - 40 */
                                             14, 14, 14, 14, 14, 14, 14, 14, /* 41 - 48 */
                                             15, 15, 15, 15, 15, 15, 15, 15, /* 49 - 56 */
                                             15, 15, 15, 15, 15, 15, 15, 15 /* 57 - 64 */
};

/**
 * @brief This function calculates CAN DLC from given length in bytes.
 *
 * @param len Length of the data in bytes.
 *
 * @return CAN data length code.
 */
static inline uint8_t rtems_canfd_len2dlc( uint8_t len )
{
  if ( len > 64 ) {
    return 0xF;
  }

  return rtems_can_len2dlc[len];
}

/**
 * @brief This function atomically sets nth bit on given address.
 *
 *  This function is a convinient wrapper around atomic_fetch_or().
 *
 * @param nr   Bit shift value.
 * @param addr Adress where the @ref nr bit should be set.
 *
 * @return None
 */
static inline void rtems_can_set_bit( int nr, atomic_uint *addr )
{
  atomic_fetch_or( addr, 1 << nr );
}

/**
 * @brief This function atomically clears nth bit on given address.
 *
 *  This function is a convinient wrapper around atomic_fetch_and().
 *
 * @param nr   Bit shift value.
 * @param addr Adress where the @ref nr bit should be clear.
 *
 * @return None
 */
static inline void rtems_can_clear_bit( int nr, atomic_uint *addr )
{
  atomic_fetch_and( addr, ~( 1 << nr ) );
}

/**
 * @brief This function atomically tests whether nth bit on given address
 *  is set.
 *
 *  This function is a convinient wrapper around atomic_load().
 *
 * @param nr   Bit shift value.
 * @param addr Adress where the @ref nr bit should be tested.
 *
 * @return 1 if set, 0 otherwise.
 */
static inline int rtems_can_test_bit( int nr, atomic_uint *addr )
{
  return ( atomic_load( addr ) ) & ( 1 << nr ) ? 1 : 0;
}

/**
 * @brief This function atomically tests and sets nth bit on given address.
 *
 *  This function is a convinient wrapper around atomic_fetch_or().
 *
 * @param nr   Bit shift value.
 * @param addr Adress where the @ref nr bit should be tested and set.
 *
 * @return 1 if set, 0 otherwise.
 */
static inline int rtems_can_test_and_set_bit( int nr, atomic_uint *addr )
{
  return ( atomic_fetch_or( addr, 1 << nr ) & ( 1 << nr ) ) ? 1 : 0;
}

#endif /* _DEV_CAN_CAN_HELPERS_H */
