/**
 * @file
 *
 * @brief Header file for timer functions.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
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
