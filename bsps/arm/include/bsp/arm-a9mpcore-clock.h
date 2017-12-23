/*
 * Copyright (c) 2013 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_SHARED_ARM_A9MPCORE_CLOCK_H
#define LIBBSP_ARM_SHARED_ARM_A9MPCORE_CLOCK_H

#include <rtems/counter.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Return the peripheral clock. For systems such as the zynq this
 * is controlled by the PL logic generation and can vary. Provide this
 * function in your application to override the BSP default.
 */
uint32_t a9mpcore_clock_periphclk(void);

/**
 * @brief Do early clock initialization so that the CPU counter conversion
 * works.
 */
static inline void a9mpcore_clock_initialize_early(void)
{
  rtems_counter_initialize_converter(a9mpcore_clock_periphclk());
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_ARM_A9MPCORE_CLOCK_H */
