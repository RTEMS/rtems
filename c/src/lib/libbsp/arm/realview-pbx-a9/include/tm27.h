/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

#include <assert.h>

#include <bsp.h>
#include <bsp/irq.h>

#define MUST_WAIT_FOR_INTERRUPT 1

#define RVPBXA9_TM27_IRQ ARM_GIC_IRQ_SGI_13

#define RVPBXA9_TM27_PRIO_LOW 0xfe

#define RVPBXA9_TM27_PRIO_HIGH 0x00

static void Install_tm27_vector(void (*handler)(rtems_vector_number))
{
  rtems_status_code sc = rtems_interrupt_handler_install(
    RVPBXA9_TM27_IRQ,
    "TM27",
    RTEMS_INTERRUPT_UNIQUE,
    (rtems_interrupt_handler) handler,
    NULL
  );
  assert(sc == RTEMS_SUCCESSFUL);

  sc = arm_gic_irq_set_priority(
    RVPBXA9_TM27_IRQ,
    RVPBXA9_TM27_PRIO_LOW
  );
  assert(sc == RTEMS_SUCCESSFUL);
}

static void Cause_tm27_intr(void)
{
  rtems_status_code sc = arm_gic_irq_generate_software_irq(
    RVPBXA9_TM27_IRQ,
    ARM_GIC_IRQ_SOFTWARE_IRQ_TO_SELF,
    0
  );
  assert(sc == RTEMS_SUCCESSFUL);
}

static void Clear_tm27_intr(void)
{
  rtems_status_code sc = arm_gic_irq_set_priority(
    RVPBXA9_TM27_IRQ,
    RVPBXA9_TM27_PRIO_LOW
  );
  assert(sc == RTEMS_SUCCESSFUL);
}

static void Lower_tm27_intr(void)
{
  rtems_status_code sc = arm_gic_irq_set_priority(
    RVPBXA9_TM27_IRQ,
    RVPBXA9_TM27_PRIO_HIGH
  );
  assert(sc == RTEMS_SUCCESSFUL);
}

#endif /* __tm27_h */
