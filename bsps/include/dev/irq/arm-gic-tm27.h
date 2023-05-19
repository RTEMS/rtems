/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @ingroup arm_gic
 *
 *  @brief ARM GIC TM27 Support
 */

/*
 * Copyright (C) 2013, 2014 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef LIBBSP_ARM_SHARED_ARM_GIC_TM27_H
#define LIBBSP_ARM_SHARED_ARM_GIC_TM27_H

#include <assert.h>

#include <bsp.h>
#include <bsp/irq.h>

#define MUST_WAIT_FOR_INTERRUPT 1

#ifndef ARM_GIC_TM27_IRQ_LOW
#define ARM_GIC_TM27_IRQ_LOW ARM_GIC_IRQ_SGI_12
#endif

#ifndef ARM_GIC_TM27_IRQ_HIGH
#define ARM_GIC_TM27_IRQ_HIGH ARM_GIC_IRQ_SGI_13
#endif

#define ARM_GIC_TM27_PRIO_LOW 0x80

#define ARM_GIC_TM27_PRIO_HIGH 0x00

static inline void Install_tm27_vector( rtems_interrupt_handler handler )
{
  rtems_status_code sc = rtems_interrupt_handler_install(
    ARM_GIC_TM27_IRQ_LOW,
    "tm27 low",
    RTEMS_INTERRUPT_UNIQUE,
    handler,
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
    1U << _SMP_Get_current_processor()
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
    1U << _SMP_Get_current_processor()
  );
  assert(sc == RTEMS_SUCCESSFUL);
}

#endif /* LIBBSP_ARM_SHARED_ARM_GIC_TM27_H */
