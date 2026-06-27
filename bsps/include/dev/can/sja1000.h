/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
* @file
*
* @ingroup SJA1000
*
* @brief This header file contains the implementation of SJA1000 controller.
*/

/*
* Copyright (C) 2025-2026 Michal Lenc <michallenc@seznam.cz> self-funded
* Copyright (C) 2025-2026 Pavel Pisa <pisa@cmp.felk.cvut.cz> self-funded
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

#ifndef _DEV_CAN_SJA1000_H
#define _DEV_CAN_SJA1000_H

struct rtems_can_chip;

/**
 * @brief SJA1000 chip register mapping specification
 */
#define RTEMS_SJA1000_HW_REG_SPAN_MASK ( 3 )
/**
 * @brief SJA1000 chip register mapping one byte each
 */
#define RTEMS_SJA1000_HW_REG_SPAN_1    ( 0 )
/**
 * @brief SJA1000 chip register mapping with span 2 bytes
 */
#define RTEMS_SJA1000_HW_REG_SPAN_2    ( 1 )
/**
 * @brief SJA1000 chip register mapping with span 4 bytes
 */
#define RTEMS_SJA1000_HW_REG_SPAN_4    ( 2 )

/**
 * @brief SJA1000 chip register mapping into CPU I/O space
 */
#define RTEMS_SJA1000_HW_REG_IO_PORT   ( 0x100 )

/**
 * @brief  This function initializes the SJA1000 controller.
 *
 * This is an entry point for SJA1000 controller initialization. This
 * function allocates generic CAN and SJA1000 related structures, sets
 * default values and initializes the resources (interrupt handler, semaphore.
 * worker thread).
 *
 * @param  addr            SJA1000 controller base address.
 * @param  hw_options      SJA1000 controller hardware options for register
 * mapping.
 * @param  irq             Interrupt number.
 * @param  worker_priotiry The priority of TX/RX worker thread.
 * @param  irq_option      RTEMS_INTERRUPT_SHARED or RTEMS_INTERRUPT_UNIQUE.
 * @param  can_clk_rate    CAN clock rate.
 *
 * @return Pointer to CAN chip structure on success, NULL otherwise.
 */
struct rtems_can_chip *rtems_can_sja1000_initialize(
  uintptr_t addr,
  unsigned int hw_options,
  rtems_vector_number irq,
  rtems_task_priority worker_priority,
  rtems_option irq_option,
  unsigned long can_clk_rate
);

#endif /* _DEV_CAN_SJA1000_H */
