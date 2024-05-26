/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file  ctucanfd_txb.h
 *
 * @ingroup CTUCANFD
 *
 * @brief This file is part of CTU CAN FD core driver implementation.
 */

/*
 * Copyright (C) 2024 Michal Lenc <michallenc@seznam.cz> FEE CTU
 * Copyright (C) 2024 Pavel Pisa <pisa@cmp.felk.cvut.cz> FEE CTU
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

#ifndef _DEV_CTUCANFD_CTUCANFD_TXB_H
#define _DEV_CTUCANFD_CTUCANFD_TXB_H

#include "ctucanfd_internal.h"

#undef TXT_DONE
#undef TXT_BF
#undef TXT_MASK
#undef TXT_ANY_DONE
#undef TXB_BF
#undef TXB_M
#undef TXB_ALL
#undef TXB_SH

#define TXT_DONE     0x4
#define TXT_BF       4
#define TXT_MASK     0xf
#define TXT_ANY_DONE ( ( TXT_DONE << ( 0 * TXT_BF ) ) | \
                       ( TXT_DONE << ( 1 * TXT_BF ) ) | \
                       ( TXT_DONE << ( 2 * TXT_BF ) ) | \
                       ( TXT_DONE << ( 3 * TXT_BF ) ) | \
                       ( TXT_DONE << ( 4 * TXT_BF ) ) | \
                       ( TXT_DONE << ( 5 * TXT_BF ) ) | \
                       ( TXT_DONE << ( 6 * TXT_BF ) ) | \
                       ( TXT_DONE << ( 7 * TXT_BF ) ) )
#define TXTB_GET_STATUS( status, id ) ( ( status >> ( id * TXT_BF ) ) & \
                                        TXT_MASK)

#define TXB_BF 4
#define TXB_MASK 0xf
#define TXB_ALL 0xffffffff
#define TXB_SH(idx) (idx * TXB_BF)

static inline uint32_t ctucanfd_txb_slot_demote(
  uint32_t txb_order,
  int from,
  int to
)
{
  uint32_t txb_order_new;
  uint32_t txb_move = ( txb_order >> TXB_SH( from ) ) & TXB_MASK;
  uint32_t mask_from = TXB_ALL << TXB_SH( from );
  uint32_t mask_to = TXB_ALL << TXB_SH( to );
  txb_order_new = txb_move << TXB_SH( to );
  txb_order_new |= txb_order & ( ~mask_from | ( mask_to << TXB_BF ) );
  txb_order_new |= ( ( txb_order >> TXB_BF) & ~mask_to ) & mask_from;
  return txb_order_new;
}

static inline uint32_t ctucanfd_txb_slot_promote(
  uint32_t txb_order,
  int from,
  int to
)
{
  uint32_t txb_order_new;
  uint32_t txb_move = ( txb_order >> TXB_SH( from ) ) & TXB_MASK;
  uint32_t mask_from = TXB_ALL << TXB_SH( from );
  uint32_t mask_to = TXB_ALL << TXB_SH( to );
  txb_order_new = txb_move << TXB_SH( to );
  txb_order_new |= ( txb_order ) & ( ~mask_to | ( mask_from << TXB_BF ) );
  txb_order_new |= ( ( (txb_order & mask_to ) & ~mask_from ) << TXB_BF );
  return txb_order_new;
}

static inline uint32_t ctucanfd_txb_order2prio( uint32_t txb_order )
{
  uint32_t prio = 0;
  int i = RTEMS_CTUCANFD_NTXBUFS_MAX - 1;
  do {
    prio |= i << ( 4 * ( txb_order & TXB_MASK ) );
    txb_order >>= TXB_BF;
  } while ( i-- );

  return prio;
}

static inline unsigned int ctucanfd_txb_from_order(
  uint32_t txb_order,
  unsigned int at
) 
{
  return ( txb_order >> TXB_SH ( at ) ) & TXB_MASK;
}

#endif /* _DEV_CTUCANFD_CTUCANFD_TXB_H */