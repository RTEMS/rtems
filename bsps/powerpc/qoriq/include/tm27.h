/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQ
 *
 * @brief Support file for Timer Test 27.
 */

/*
 * Copyright (C) 2010, 2015 embedded brains GmbH & Co. KG
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

static inline void Install_tm27_vector( rtems_interrupt_handler handler )
{
  rtems_status_code sc;
  rtems_vector_number low = QORIQ_IRQ_IPI_0 + IPI_INDEX_LOW;
  rtems_vector_number high = QORIQ_IRQ_IPI_0 + IPI_INDEX_HIGH;

  sc = rtems_interrupt_handler_install(
    low,
    "tm17 low",
    RTEMS_INTERRUPT_UNIQUE,
    handler,
    NULL
  );
  assert(sc == RTEMS_SUCCESSFUL);

  sc = qoriq_pic_set_priority(low, 1, NULL);
  assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_interrupt_handler_install(
    high,
    "tm17 high",
    RTEMS_INTERRUPT_UNIQUE,
    handler,
    NULL
  );
  assert(sc == RTEMS_SUCCESSFUL);

  sc = qoriq_pic_set_priority(high, 2, NULL);
  assert(sc == RTEMS_SUCCESSFUL);
}

static inline void qoriq_tm27_cause(uint32_t ipi_index)
{
  uint32_t self = ppc_processor_id();

  qoriq.pic.per_cpu[self].ipidr[ipi_index].reg = UINT32_C(1) << self;
}

static inline void Cause_tm27_intr(void)
{
  qoriq_tm27_cause(IPI_INDEX_LOW);
}

static inline void Clear_tm27_intr(void)
{
  /* Nothing to do */
}

static inline inline void Lower_tm27_intr(void)
{
  qoriq_tm27_cause(IPI_INDEX_HIGH);
}

#endif /* TMTESTS_TM27_H */
