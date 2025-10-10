/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsX8664AMD64
 *
 * @brief Interrupts support
 */

/*
 * Copyright (c) 2018 Amaan Cheval <amaan.cheval@gmail.com>
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

#include <apic.h>
#include <bsp/fatal.h>
#include <bsp/irq-generic.h>
#include <rtems.h>
#include <rtems/score/idt.h>
#include <rtems/score/basedefs.h>
#include <rtems/score/x86_64.h>
#include <rtems/score/cpuimpl.h>
#include <rtems/score/processormaskimpl.h>

#include <stdint.h>

/*
 * The IDT maps every interrupt vector to an interrupt_descriptor based on the
 * vector number.
 */
interrupt_descriptor amd64_idt[IDT_SIZE] RTEMS_ALIGNED(8) = { { 0 } };

struct idt_record amd64_idtr = {
  .limit = (IDT_SIZE * 16) - 1,
  .base = (uintptr_t) amd64_idt
};

/**
 * IRQs that the RTEMS Interrupt Manager will manage
 * @see DISTINCT_INTERRUPT_ENTRY
 */
static uintptr_t rtemsIRQs[BSP_IRQ_VECTOR_NUMBER] = {
  (uintptr_t) rtems_irq_prologue_0,
  (uintptr_t) rtems_irq_prologue_1,
  (uintptr_t) rtems_irq_prologue_2,
  (uintptr_t) rtems_irq_prologue_3,
  (uintptr_t) rtems_irq_prologue_4,
  (uintptr_t) rtems_irq_prologue_5,
  (uintptr_t) rtems_irq_prologue_6,
  (uintptr_t) rtems_irq_prologue_7,
  (uintptr_t) rtems_irq_prologue_8,
  (uintptr_t) rtems_irq_prologue_9,
  (uintptr_t) rtems_irq_prologue_10,
  (uintptr_t) rtems_irq_prologue_11,
  (uintptr_t) rtems_irq_prologue_12,
  (uintptr_t) rtems_irq_prologue_13,
  (uintptr_t) rtems_irq_prologue_14,
  (uintptr_t) rtems_irq_prologue_15,
  (uintptr_t) rtems_irq_prologue_16,
  (uintptr_t) rtems_irq_prologue_17,
  (uintptr_t) rtems_irq_prologue_18,
  (uintptr_t) rtems_irq_prologue_19,
  (uintptr_t) rtems_irq_prologue_20,
  (uintptr_t) rtems_irq_prologue_21,
  (uintptr_t) rtems_irq_prologue_22,
  (uintptr_t) rtems_irq_prologue_23,
  (uintptr_t) rtems_irq_prologue_24,
  (uintptr_t) rtems_irq_prologue_25,
  (uintptr_t) rtems_irq_prologue_26,
  (uintptr_t) rtems_irq_prologue_27,
  (uintptr_t) rtems_irq_prologue_28,
  (uintptr_t) rtems_irq_prologue_29,
  (uintptr_t) rtems_irq_prologue_30,
  (uintptr_t) rtems_irq_prologue_31,
  (uintptr_t) rtems_irq_prologue_32,
  (uintptr_t) rtems_irq_prologue_33
};

void lidt(struct idt_record *ptr)
{
  __asm__ volatile ("lidt %0" :: "m"(*ptr));
}

interrupt_descriptor amd64_create_interrupt_descriptor(
  uintptr_t handler, uint8_t types_and_attributes
)
{
  interrupt_descriptor entry = {
    .offset_0 = handler & 0xffff,
    .segment_selector = amd64_get_cs(),
    .interrupt_stack_table = 0,
    .type_and_attributes = types_and_attributes,
    .offset_1 = (handler >> 16) & 0xffff,
    .offset_2 = handler >> 32,
    .reserved_zero = 0,
  };
  return entry;
}

uintptr_t amd64_get_handler_from_idt(uint32_t vector)
{
  interrupt_descriptor entry = amd64_idt[vector];
  uintptr_t handler = entry.offset_0 | (entry.offset_1 << 16) |
    ((uint64_t) entry.offset_2 << 32);
  return handler;
}

void amd64_install_raw_interrupt(
  uint32_t vector, uintptr_t new_handler, uintptr_t *old_handler
)
{
  *old_handler = amd64_get_handler_from_idt(vector);
  interrupt_descriptor new_desc = amd64_create_interrupt_descriptor(
    new_handler,
    IDT_INTERRUPT_GATE | IDT_PRESENT
  );
  amd64_idt[vector] = new_desc;
}

void amd64_dispatch_isr(rtems_vector_number vector)
{
  bsp_interrupt_handler_dispatch(vector);
}

void bsp_interrupt_facility_initialize(void)
{
  uintptr_t old;
  for (uint32_t i = 0; i < BSP_IRQ_VECTOR_NUMBER; i++) {
    amd64_install_raw_interrupt(i, rtemsIRQs[i], &old);
  }

  lidt(&amd64_idtr);

  if (lapic_initialize() == false) {
    bsp_fatal(BSP_FATAL_INTERRUPT_INITIALIZATION);
  }
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  (void) vector;

  /* XXX: Should be implemented once I/O APIC support is added */
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_set_priority(
  rtems_vector_number vector,
  uint32_t priority
)
{
  (void) priority;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_get_priority(
  rtems_vector_number vector,
  uint32_t *priority
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(priority != NULL);
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  (void) vector;
  (void) attributes;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(pending != NULL);
  *pending = false;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_raise(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_clear(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_is_enabled(
  rtems_vector_number vector,
  bool               *enabled
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(enabled != NULL);
  *enabled = false;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  (void) vector;

  /* XXX: Should be implemented once I/O APIC support is added */
  return RTEMS_SUCCESSFUL;
}

#ifdef RTEMS_SMP
/* TODO: See #5121 */
rtems_status_code bsp_interrupt_get_affinity(
  rtems_vector_number  vector,
  Processor_mask      *affinity
)
{
  (void) vector;
  _Processor_mask_From_index( affinity, 0 );
  return RTEMS_UNSATISFIED;
}

/* TODO: See #5121 */
rtems_status_code bsp_interrupt_set_affinity(
  rtems_vector_number   vector,
  const Processor_mask *affinity
)
{
  (void) vector;
  (void) affinity;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_raise_on(
  rtems_vector_number vector,
  uint32_t            cpu_index
)
{
  (void) vector;
  (void) cpu_index;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}
#endif
