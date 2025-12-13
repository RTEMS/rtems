/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
* @file
*
* @ingroup CTUCANFD
*
* @brief This header file contains the implementation of CTU CAN FD
 *   controller.
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

#ifndef _DEV_CAN_CTUCANFD_H
#define _DEV_CAN_CTUCANFD_H

/**
 * @brief This defines the maximum number of TX buffers.
 *
 * This is the maximum number of HW buffers that can be implemented to any
 * CTU CAN FD core. The user can use this as an argument to initialization
 * function @ref rtems_ctucanfd_initialize and the function automatically
 * limits the used buffers based on the used core.
 */
#define RTEMS_CTUCANFD_NTXBUFS_MAX ( 8 )

struct rtems_can_chip;

/**
 * @brief  This function initializes the CTU CAN FD controller.
 *
 * This is an entry point for CTU CAN FD controller initialization. This
 * function allocates generic CAN and CTU CAN FD related structures, sets
 * default values and initializes the resources (interrupt handler, semaphore.
 * worker thread).
 *
 * @param  addr            CTU CAN FD code base address.
 * @param  irq             Interrupt number
 * @param  worker_priotiry The priority of TX/RX worker thread
 * @param  ntxbufs         Number of TX hardware buffers to be used.
 * The function automatically limits the maximum HW buffers based on the
 * used core. You can use @ref RTEMS_CTUCANFD_NTXBUFS_MAX to setup maximum
 * available buffers.
 * @param  irq_option      RTEMS_INTERRUPT_SHARED or RTEMS_INTERRUPT_UNIQUE
 * @param  can_clk_rate    CAN clock rate.
 *
 * @return Pointer to CAN chip structure on success, NULL otherwise.
 */
struct rtems_can_chip *rtems_ctucanfd_initialize(
  uintptr_t addr,
  rtems_vector_number irq,
  rtems_task_priority worker_priority,
  unsigned int ntxbufs,
  rtems_option irq_option,
  unsigned long can_clk_rate
);

#endif /* _DEV_CAN_CTUCANFD_H */
