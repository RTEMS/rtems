/*
 * Cirrus EP7312 Clock driver
 *
 * Copyright (c) 2002 by Jay Monkman <jtm@smoothsmoothie.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <ep7312.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <assert.h>

#if ON_SKYEYE==1
  #define CLOCK_DRIVER_USE_FAST_IDLE 1
#endif

void Clock_isr(void * arg);

#define Clock_driver_support_at_tick()                \
  do {                                                \
    *EP7312_TC1EOI = 0xffffffff;                      \
  } while(0)

#define Clock_driver_support_install_isr( _new, _old ) \
  do {                                                 \
    rtems_status_code status = RTEMS_SUCCESSFUL;       \
    (_old) = NULL; /* avoid warning */;                \
    status = rtems_interrupt_handler_install(          \
        BSP_TC1OI,                                     \
        "Clock",                                       \
        RTEMS_INTERRUPT_UNIQUE,                        \
        Clock_isr,                                     \
        NULL                                           \
    );                                                 \
    assert(status == RTEMS_SUCCESSFUL);                \
  } while(0)

/*
 * Set up the clock hardware
 */
#if ON_SKYEYE
  #define TCD_VALUE \
    (rtems_configuration_get_microseconds_per_tick() * 2000)/25000
#else
  #define TCD_VALUE \
    (rtems_configuration_get_microseconds_per_tick() * 2000)/1000000
#endif

#define Clock_driver_support_initialize_hardware()  \
  do {                                              \
    *EP7312_SYSCON1 |= EP7312_SYSCON1_TC1_PRESCALE; \
    *EP7312_TC1D = TCD_VALUE;                       \
    *EP7312_TC1EOI = 0xFFFFFFFF;                    \
  } while (0)

#define Clock_driver_support_shutdown_hardware()    \
  do {                                              \
    rtems_status_code status = RTEMS_SUCCESSFUL;    \
    status = rtems_interrupt_handler_remove(        \
        BSP_TC1OI,                                  \
        Clock_isr,                                  \
        NULL                                        \
    );                                              \
    assert(status == RTEMS_SUCCESSFUL);             \
  } while (0)

#include "../../../shared/clockdrv_shell.h"
