/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file
 *
 * @ingroup CTUCANFD
 *
 * @brief This file is part of CTU CAN FD core driver implementation.
 */

/*
* Copyright (C) 2023-2024 Michal Lenc <michallenc@seznam.cz>
* Implementation is based on Linux CTU CAN FD driver
* Copyright (C) 2015-2018 Ondrej Ille <ondrej.ille@gmail.com> FEE CTU
* Copyright (C) 2018-2021 Ondrej Ille <ondrej.ille@gmail.com> self-funded
* Copyright (C) 2018-2019 Martin Jerabek <martin.jerabek01@gmail.com> FEE CTU
* Copyright (C) 2018-2022 Pavel Pisa <pisa@cmp.felk.cvut.cz> FEE CTU/self-funded
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

#ifndef __CTUCANFD_INTERNAL_H
#define __CTUCANFD_INTERNAL_H

#include <dev/can/can-queue.h>
#include <dev/can/can-helpers.h>

#include <dev/can/ctucanfd.h>

#include "ctucanfd_kregs.h"

/**
 * @brief This represents one HW transmission buffer.
 */
struct ctucanfd_txb_info {
  /**
   * @brief This holds the pointer to the slot the buffer acquired.
   *
   * The stack does not free the slot immediately after it is assigned to
   * the buffer, but waits for succesful transmission (or error/abort). Hence
   * the buffer has to keep the information about the slot it acquired to
   * provide correct error reporting or to schedule it for later processing.
   */
  struct rtems_can_queue_slot *slot;
  /**
   * @brief This holds the pointer to the edge from which the slot was
   *  acquired.
   *
   * This information is useful for future filtering of echo frames back to
   * sending instances.
   */
  struct rtems_can_queue_edge *edge;
};

/**
 * @brief This represents internal CTU CAN FD structure.
 *
 * This structure is CTU CAN FD controller dependent and provides
 * access to non generic parts that do not belong to generic
 * @ref rtems_can_chip structure.
 */
struct ctucanfd_internal {
  /**
   * @brief Base memory address.
   */
  uintptr_t base;
  /**
   * @brief This member holds the enabled interrupts.
   */
  uint32_t int_ena;
  /**
   * @brief This member holds the masked out interrupts.
   */
  uint32_t int_mask;
  /**
   * @brief This member holds the array of @ref ctucanfd_txb_info structures.
   */
  struct ctucanfd_txb_info txb_info[RTEMS_CTUCANFD_NTXBUFS_MAX];
  /**
   * @brief This member holds tails for mapping of priority classes to bufders.
   *
   * These are the positions where buffers with frames from given priority
   * class should be inserted.
   */
  uint8_t txb_prio_tail[RTEMS_CAN_QUEUE_PRIO_NR];
  /**
   * @brief This member holds the order in which the buffers should be
   *  scheduled for transmisison.
   */
  uint32_t txb_order;
  /**
   * @brief This member holds the number of TX buffers for given core.
   */
  unsigned int ntxbufs;
};

static inline void ctucanfd_write32(
  struct ctucanfd_internal *internal,
  enum ctu_can_fd_can_registers reg,
  uint32_t val
)
{
  *( volatile uint32_t * )( internal->base + reg ) = val;
}

static inline uint32_t ctucanfd_read32(
  struct ctucanfd_internal *internal,
  enum ctu_can_fd_can_registers reg
)
{
  return *( volatile uint32_t * )( internal->base + reg );
}

static inline void ctucanfd_write_txt_buf(
  struct ctucanfd_internal *internal,
  enum ctu_can_fd_can_registers buf_base,
  uint32_t offset,
  uint32_t val
)
{
  *( volatile uint32_t * )( internal->base + buf_base + offset ) = val;
}

#define CTU_CAN_FD_ENABLED( internal ) (!!FIELD_GET( \
  REG_MODE_ENA, \
  ctucanfd_read32( internal, CTUCANFD_MODE ) \
) )

#endif /* __CTUCANFD_PRIV_H */
