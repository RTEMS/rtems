/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2012, 2017 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_POWERPC_T32MPPC_IRQ_H
#define LIBBSP_POWERPC_T32MPPC_IRQ_H

#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>
#include <rtems/score/processormask.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_INTERRUPT_VECTOR_COUNT 1

static inline rtems_status_code bsp_interrupt_set_affinity(
  rtems_vector_number   vector,
  const Processor_mask *affinity
)
{
  (void) vector;
  (void) affinity;
  return RTEMS_SUCCESSFUL;
}

static inline rtems_status_code bsp_interrupt_get_affinity(
  rtems_vector_number  vector,
  Processor_mask      *affinity
)
{
  (void) vector;
  _Processor_mask_From_index( affinity, 0 );
  return RTEMS_SUCCESSFUL;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_POWERPC_T32MPPC_IRQ_H */
