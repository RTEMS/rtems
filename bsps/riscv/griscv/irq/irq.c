/**
 * @file
 *
 * @ingroup riscv_interrupt
 *
 * @brief Interrupt support.
 */

/*
 * Copyright (c) 2018 embedded brains GmbH
 *
 * Copyright (c) 2015 University of York.
 * Hesham Almatary <hesham@alumni.york.ac.uk>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <bsp/irq.h>
#include <bsp/fatal.h>
#include <bsp/irq-generic.h>
#include <amba.h>

#include <rtems/score/percpu.h>
#include <rtems/score/riscv-utility.h>
#include <rtems/score/smpimpl.h>

#if defined(RTEMS_SMP)
/* Interrupt to CPU map. Default to CPU0 since in BSS. */
const unsigned char GRLIB_irq_to_cpu[32] __attribute__((weak));

/* On SMP use map table above relative to SMP Boot CPU (normally CPU0) */
static inline int bsp_irq_cpu(int irq)
{
  /* protect from bad user configuration, default to boot cpu */
  if (rtems_configuration_get_maximum_processors() <= GRLIB_irq_to_cpu[irq])
    return GRLIB_Cpu_Index;
  else
    return GRLIB_Cpu_Index + GRLIB_irq_to_cpu[irq];
}
#else
/* when not SMP the local CPU is returned */
static inline int bsp_irq_cpu(int irq)
{
  return read_csr(mhartid);
}
#endif

void _RISCV_Interrupt_dispatch(uintptr_t mcause, Per_CPU_Control *cpu_self)
{
  if (mcause & 0x80000000) {

    bsp_interrupt_handler_dispatch(mcause & 0xf);

  } else {
    bsp_fatal(RISCV_FATAL_UNEXPECTED_INTERRUPT_EXCEPTION);
  }
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{

  /*
   * External M-mode interrupts on secondary processors are enabled in
   * bsp_start_on_secondary_processor().
   */
  set_csr(mie, MIP_MEIP);

  return RTEMS_SUCCESSFUL;
}

void bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  int irq = (int)vector;
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  GRLIB_Cpu_Unmask_interrupt(irq, bsp_irq_cpu(irq));
}

void bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  int irq = (int)vector;
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  GRLIB_Cpu_Mask_interrupt(irq, bsp_irq_cpu(irq));
}

void bsp_interrupt_get_affinity(
  rtems_vector_number vector,
  Processor_mask *affinity
)
{
  uint32_t cpu_count = rtems_scheduler_get_processor_maximum();
  uint32_t cpu_index;

  _Processor_mask_Zero(affinity);

  for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
    if (!BSP_Cpu_Is_interrupt_masked(vector, cpu_index)) {
      _Processor_mask_Set(affinity, cpu_index);
    }
  }
}

void bsp_interrupt_set_affinity(
  rtems_vector_number vector,
  const Processor_mask *affinity
)
{
  uint32_t unmasked = 0;
  uint32_t cpu_count = rtems_scheduler_get_processor_maximum();
  uint32_t cpu_index;

  for (cpu_index = 0; cpu_index < cpu_count; ++cpu_index) {
    if (_Processor_mask_Is_set(affinity, cpu_index)) {
      GRLIB_Cpu_Unmask_interrupt(vector, cpu_index);
      ++unmasked;
    }
  }

  if (unmasked > 1) {
    GRLIB_Enable_interrupt_broadcast(vector);
  } else {
    GRLIB_Disable_interrupt_broadcast(vector);
  }
}
