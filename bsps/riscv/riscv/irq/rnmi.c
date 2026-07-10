/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsRISCVGeneric
 *
 * @brief Basic RNMI implementation: standard dispatch, deferred
 *        processing infrastructure, and default handler.
 */

/*
 * Copyright (C) 2025-2026 Intel Corporation
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

#include <bsp/fatal.h>
#include <bsp/irq-generic.h>
#include <bsp/rnmi.h>

#include <inttypes.h>
#include <rtems/bspIo.h>
#include <rtems/score/cpu.h>
#include <rtems/score/percpu.h>
#include <rtems/score/riscv-rnmi.h>
#include <rtems/score/riscv-utility.h>
#include <rtems/score/smpimpl.h>
#include <rtems/score/threaddispatch.h>

static void _RISCV_RNMI_deferred_enable_action(void *arg)
{
  (void)arg;

  /* The deferred path uses the software interrupt, already enabled on SMP */
  set_csr(mie, MIP_MSIP);

  _Per_CPU_Get()->cpu_per_cpu.rnmi.deferred = 1;
}

void riscv_rnmi_deferred_enable(void)
{
  Per_CPU_Control *cpu_self;

  cpu_self = _Thread_Dispatch_disable();
  _RISCV_RNMI_deferred_enable_action(NULL);
#ifdef RTEMS_SMP
  _SMP_Othercast_action(_RISCV_RNMI_deferred_enable_action, NULL);
#endif
  _Thread_Dispatch_enable(cpu_self);
}

void __attribute__((weak))
riscv_rnmi_deferred_work_dispatch(
  uintptr_t mncause,
  uintptr_t mnepc,
  uintptr_t dropped
)
{
  printk("RNMI: mncause=0x%016" PRIxPTR " mnepc=0x%016" PRIxPTR
         " dropped=%" PRIuPTR "\n", mncause, mnepc, dropped);
}

void _RISCV_RNMI_deferred(void)
{
  RISCV_RNMI_context *ctx;
  uintptr_t mncause;
  uintptr_t dropped;

  ctx = &_Per_CPU_Get()->cpu_per_cpu.rnmi;
  mncause = ctx->mncause;

  if (!mncause)
    return;

  /* Only the RNMI handler writes dropped, so no atomic exchange is needed */
  dropped = ctx->dropped;
  riscv_rnmi_deferred_work_dispatch(mncause, ctx->mnepc,
                                    dropped - ctx->dropped_reported);
  ctx->dropped_reported = dropped;

  /* A cleared mncause lets the RNMI handler defer the next RNMI */
  ctx->mnepc = 0;
  RTEMS_COMPILER_MEMORY_BARRIER();
  ctx->mncause = 0;
}

void _RISCV_RNMI_dispatch(uintptr_t mncause, Per_CPU_Control *cpu_self)
{
  uintptr_t line;

  (void) cpu_self;

  line = mncause & ~MNCAUSE_INTERRUPT;

  if ((mncause & MNCAUSE_INTERRUPT) == 0 ||
      line >= RISCV_MAXIMUM_RNMI_INTERRUPTS) {
    bsp_fatal(RISCV_FATAL_UNEXPECTED_RNMI_EXCEPTION);
  }

  bsp_interrupt_handler_dispatch_unchecked(RISCV_INTERRUPT_VECTOR_RNMI(line));
}
