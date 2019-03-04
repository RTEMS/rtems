/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief Timer benchmark functions for the lpc176x bsp.
 */

/*
 * Copyright (c) 2014 Taller Technologies.
 *
 * @author  Boretto Martin    (martin.boretto@tallertechnologies.com)
 * @author  Diaz Marcos (marcos.diaz@tallertechnologies.com)
 * @author  Lenarduzzi Federico  (federico.lenarduzzi@tallertechnologies.com)
 * @author  Daniel Chicco  (daniel.chicco@tallertechnologies.com)
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/btimer.h>

#include <bsp/timer.h>

static uint32_t benchmark_timer_base;

void benchmark_timer_initialize( void )
{
  benchmark_timer_base = lpc176x_timer_get_timer_value( LPC176X_TIMER_1 );
}

benchmark_timer_t benchmark_timer_read( void )
{
  uint32_t delta = lpc176x_timer_get_timer_value( LPC176X_TIMER_1 ) -
                   benchmark_timer_base;

  return delta;
}

void benchmark_timer_disable_subtracting_average_overhead( bool find_avg_ovhead )
{
}
