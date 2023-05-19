/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Header file for timer functions.
 */

/*
 * Copyright (c) 2008 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_POWERPC_MPC83XX_TIMER_H
#define LIBBSP_POWERPC_MPC83XX_TIMER_H

#include <stdint.h>
#include <stdbool.h>

#include <rtems.h>

#define MPC83XX_GTM_NUMBER 8

#define MPC83XX_GTM_CLOCK_CASCADED 0x0000
#define MPC83XX_GTM_CLOCK_SYSTEM 0x0002
#define MPC83XX_GTM_CLOCK_SYSTEM_SLOW 0x0004
#define MPC83XX_GTM_CLOCK_EXTERN 0x0006

rtems_status_code mpc83xx_gtm_initialize( int timer, int clock);

rtems_status_code mpc83xx_gtm_enable_restart( int timer, bool enable);

rtems_status_code mpc83xx_gtm_set_clock( int timer, int clock);

rtems_status_code mpc83xx_gtm_get_clock( int timer, int *clock);

rtems_status_code mpc83xx_gtm_start( int timer);

rtems_status_code mpc83xx_gtm_stop( int timer);

rtems_status_code mpc83xx_gtm_set_value( int timer, uint16_t value);

rtems_status_code mpc83xx_gtm_get_value( int timer, uint16_t *value);

rtems_status_code mpc83xx_gtm_set_reference( int timer, uint16_t reference);

rtems_status_code mpc83xx_gtm_get_reference( int timer, uint16_t *reference);

rtems_status_code mpc83xx_gtm_set_prescale( int timer, uint8_t prescale);

rtems_status_code mpc83xx_gtm_get_prescale( int timer, uint8_t *prescale);

rtems_status_code mpc83xx_gtm_interrupt_get_vector( int timer, rtems_vector_number *vector);

rtems_status_code mpc83xx_gtm_interrupt_enable( int timer);

rtems_status_code mpc83xx_gtm_interrupt_disable( int timer);

rtems_status_code mpc83xx_gtm_interrupt_clear( int timer);

#endif /* LIBBSP_POWERPC_MPC83XX_TIMER_H */
