/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2024 Kevin Kirspel
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

#ifndef LIBBSP_NIOSV_IRQ_H
#define LIBBSP_NIOSV_IRQ_H

#ifndef ASM

#include <bsp.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>
#include <rtems/score/processormask.h>
#include <rtems/score/riscv-utility.h>

#define NIOSV_INTERRUPT_VECTOR_SOFTWARE 0

#define NIOSV_INTERRUPT_VECTOR_TIMER 1

#define NIOSV_INTERRUPT_VECTOR_EXTERNAL(x) ((x) + 2)

#define NIOSV_INTERRUPT_VECTOR_IS_EXTERNAL(x) ((x) >= 2)

#define NIOSV_INTERRUPT_VECTOR_EXTERNAL_TO_INDEX(x) ((x) - 2)

#define BSP_INTERRUPT_VECTOR_COUNT \
  NIOSV_INTERRUPT_VECTOR_EXTERNAL(NIOSV_MAXIMUM_EXTERNAL_INTERRUPTS)

#define BSP_INTERRUPT_CUSTOM_VALID_VECTOR

/*
 * The following enumeration describes the value in the mcause CSR.
 */
enum alt_exception_cause_e {
  NIOSV_UNDEFINED_CAUSE                = -1,
  NIOSV_INSTRUCTION_ADDRESS_MISALIGNED = 0,
  NIOSV_INSTRUCTION_ACCESS_FAULT       = 1,
  NIOSV_ILLEGAL_INSTRUCTION            = 2,
  NIOSV_BREAKPOINT                     = 3,
  NIOSV_LOAD_ADDRESS_MISALIGNED        = 4,
  NIOSV_LOAD_ACCESS_FAULT              = 5,
  NIOSV_STORE_AMO_ADDRESS_MISALIGNED   = 6,
  NIOSV_STORE_AMO_ACCESS_FAULT         = 7,
  NIOSV_ENVIRONMENT_CALL_FROM_U_MODE   = 8,
  NIOSV_ENVIRONMENT_CALL_FROM_S_MODE   = 9,
  NIOSV_RESERVED_BIT_10                = 10,
  NIOSV_ENVIRONMENT_CALL_FROM_M_MODE   = 11,
  NIOSV_INSTRUCTION_PAGE_FAULT         = 12,
  NIOSV_LOAD_PAGE_FAULT                = 13,
  NIOSV_RESERVED_BIT_14                = 14,
  NIOSV_STORE_AMO_PAGE_FAULT           = 15
};
typedef enum alt_exception_cause_e alt_exception_cause;

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

static inline uint32_t alt_irq_pending(void)
{
  uint32_t active, enabled;

  active = read_csr(mip);
  enabled = read_csr(mie);

  /*
   * we want to only process the upper 16-bits, the interrupt lines connected
   * via Platform Designer.
   */
  return (active & enabled) >> 16;
}

static inline uint32_t alt_irq_index(uint32_t active)
{
  uint32_t mask, i;

  i = 0;
  mask = 1;
  /*
   * Test each bit in turn looking for an active interrupt. Once one is
   * found, the interrupt handler asigned by a call to alt_irq_register() is
   * called to clear the interrupt condition.
   */
  do {
    if (active & mask) {
      return i;
    }
    mask <<= 1;
    i++;
  } while (1);
  /* should not happen */
  return 0;
}

static inline bool alt_irq_is_pending(uint32_t index)
{
  uint32_t mask, active;

  active = alt_irq_pending();

  if(active != 0) {
    mask = 1 << index;
    if (active & mask) {
      return true;
    }
  }
  return false;
}

static inline bool alt_irq_is_enabled(uint32_t index)
{
  uint32_t mask, enabled;

  enabled = read_csr(mie) >> 16;

  if(enabled != 0) {
    mask = 1 << index;
    if (enabled & mask) {
      return true;
    }
  }
  return false;
}

static inline void alt_irq_enable(uint32_t index)
{
  uint32_t mask;

  mask = 1 << (index + 16);
  set_csr(mie, mask);
}

static inline void alt_irq_disable(uint32_t index)
{
  uint32_t mask;

  mask = 1 << (index + 16);
  clear_csr(mie, mask);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* LIBBSP_NIOSV_IRQ_H */
