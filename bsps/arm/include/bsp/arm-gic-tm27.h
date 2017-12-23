/**
 *  @file
 *
 *  @ingroup arm_gic
 *
 *  @brief ARM GIC TM27 Support
 */

/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef LIBBSP_ARM_SHARED_ARM_GIC_TM27_H
#define LIBBSP_ARM_SHARED_ARM_GIC_TM27_H

#include <assert.h>

#include <bsp.h>
#include <bsp/irq.h>

#define MUST_WAIT_FOR_INTERRUPT 1

#define ARM_GIC_TM27_IRQ_LOW ARM_GIC_IRQ_SGI_12

#define ARM_GIC_TM27_IRQ_HIGH ARM_GIC_IRQ_SGI_13

#define ARM_GIC_TM27_PRIO_LOW 0x80

#define ARM_GIC_TM27_PRIO_HIGH 0x00

static inline void Install_tm27_vector(void (*handler)(rtems_vector_number))
{
  rtems_status_code sc = rtems_interrupt_handler_install(
    ARM_GIC_TM27_IRQ_LOW,
    "tm27 low",
    RTEMS_INTERRUPT_UNIQUE,
    (rtems_interrupt_handler) handler,
    NULL
  );
  assert(sc == RTEMS_SUCCESSFUL);

  sc = arm_gic_irq_set_priority(
    ARM_GIC_TM27_IRQ_LOW,
    ARM_GIC_TM27_PRIO_LOW
  );
  assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_interrupt_handler_install(
    ARM_GIC_TM27_IRQ_HIGH,
    "tm27 high",
    RTEMS_INTERRUPT_UNIQUE,
    (rtems_interrupt_handler) handler,
    NULL
  );
  assert(sc == RTEMS_SUCCESSFUL);

  sc = arm_gic_irq_set_priority(
    ARM_GIC_TM27_IRQ_HIGH,
    ARM_GIC_TM27_PRIO_HIGH
  );
  assert(sc == RTEMS_SUCCESSFUL);
}

static inline void Cause_tm27_intr(void)
{
  rtems_status_code sc = arm_gic_irq_generate_software_irq(
    ARM_GIC_TM27_IRQ_LOW,
    ARM_GIC_IRQ_SOFTWARE_IRQ_TO_SELF,
    0
  );
  assert(sc == RTEMS_SUCCESSFUL);
}

static inline void Clear_tm27_intr(void)
{
  /* Nothing to do */
}

static inline void Lower_tm27_intr(void)
{
  rtems_status_code sc = arm_gic_irq_generate_software_irq(
    ARM_GIC_TM27_IRQ_HIGH,
    ARM_GIC_IRQ_SOFTWARE_IRQ_TO_SELF,
    0
  );
  assert(sc == RTEMS_SUCCESSFUL);
}

#endif /* LIBBSP_ARM_SHARED_ARM_GIC_TM27_H */
