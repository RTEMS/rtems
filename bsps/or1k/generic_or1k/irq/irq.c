/**
 * @file
 *
 * @ingroup or1k_interrupt
 *
 * @brief Interrupt support.
 */

/*
 * Copyright (c) 2014 Hesham ALMatary
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <rtems/inttypes.h>

/* Almost all of the jobs that the following functions should
 * do are implemented in cpukit
 */

void bsp_interrupt_handler_default(rtems_vector_number vector)
{
    printk("spurious interrupt: %" PRIdrtems_vector_number "\n", vector);
}

rtems_status_code bsp_interrupt_facility_initialize()
{
  return 0;
}

void bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
}

void bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
}
