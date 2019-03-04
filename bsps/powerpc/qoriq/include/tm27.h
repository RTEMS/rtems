/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQ
 *
 * @brief Support file for Timer Test 27.
 */

/*
 * Copyright (c) 2010-2015 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_TMTEST27
  #error "This is an RTEMS internal file you must not include directly."
#endif /* _RTEMS_TMTEST27 */

#ifndef TMTESTS_TM27_H
#define TMTESTS_TM27_H

#include <assert.h>

#include <libcpu/powerpc-utility.h>

#include <bsp/irq.h>
#include <bsp/qoriq.h>

#define MUST_WAIT_FOR_INTERRUPT 1

#define IPI_INDEX_LOW 1

#define IPI_INDEX_HIGH 2

RTEMS_INLINE_ROUTINE void Install_tm27_vector(void (*handler)(rtems_vector_number))
{
  rtems_status_code sc;
  rtems_vector_number low = QORIQ_IRQ_IPI_0 + IPI_INDEX_LOW;
  rtems_vector_number high = QORIQ_IRQ_IPI_0 + IPI_INDEX_HIGH;

  sc = rtems_interrupt_handler_install(
    low,
    "tm17 low",
    RTEMS_INTERRUPT_UNIQUE,
    (rtems_interrupt_handler) handler,
    NULL
  );
  assert(sc == RTEMS_SUCCESSFUL);

  sc = qoriq_pic_set_priority(low, 1, NULL);
  assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_interrupt_handler_install(
    high,
    "tm17 high",
    RTEMS_INTERRUPT_UNIQUE,
    (rtems_interrupt_handler) handler,
    NULL
  );
  assert(sc == RTEMS_SUCCESSFUL);

  sc = qoriq_pic_set_priority(high, 2, NULL);
  assert(sc == RTEMS_SUCCESSFUL);
}

RTEMS_INLINE_ROUTINE void qoriq_tm27_cause(uint32_t ipi_index)
{
  uint32_t self = ppc_processor_id();

  qoriq.pic.per_cpu[self].ipidr[ipi_index].reg = UINT32_C(1) << self;
}

RTEMS_INLINE_ROUTINE void Cause_tm27_intr(void)
{
  qoriq_tm27_cause(IPI_INDEX_LOW);
}

RTEMS_INLINE_ROUTINE void Clear_tm27_intr(void)
{
  /* Nothing to do */
}

RTEMS_INLINE_ROUTINE inline void Lower_tm27_intr(void)
{
  qoriq_tm27_cause(IPI_INDEX_HIGH);
}

#endif /* TMTESTS_TM27_H */
