/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file
 *
 * @ingroup DCAN
 *
 * @brief This header file provides AM335x-specific DCAN support.
 */

/*
 * Copyright (C) 2026 Ning Zhang <riverquiet66@gmail.com>
 * Copyright (C) 2026 Michal Lenc <michallenc@seznam.cz>
 * Copyright (C) 2026 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 * Based on DCAN driver code by Prashanth S <fishesprashanth@gmail.com>
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

#ifndef LIBBSP_ARM_BEAGLE_DCAN_AM335X_H
#define LIBBSP_ARM_BEAGLE_DCAN_AM335X_H

struct rtems_can_chip;

/**
 * @brief Initializes the AM335x DCAN1 controller.
 *
 * This function enables the DCAN1 peripheral clock, configures the
 * BeagleBone Black DCAN1 pins.
 */
void rtems_am335x_dcan_initialize( void );

#endif
