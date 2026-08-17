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
#include <rtems/score/interr.h>
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
 * Exception vectors of the processor
 * @see EXCEPTION_ENTRY
 */
static const uintptr_t amd64_exceptions[BSP_VECTOR_APIC_FIRST] = {
  (uintptr_t) amd64_exception_prologue_0,
  (uintptr_t) amd64_exception_prologue_1,
  (uintptr_t) amd64_exception_prologue_2,
  (uintptr_t) amd64_exception_prologue_3,
  (uintptr_t) amd64_exception_prologue_4,
  (uintptr_t) amd64_exception_prologue_5,
  (uintptr_t) amd64_exception_prologue_6,
  (uintptr_t) amd64_exception_prologue_7,
  (uintptr_t) amd64_exception_prologue_8,
  (uintptr_t) amd64_exception_prologue_9,
  (uintptr_t) amd64_exception_prologue_10,
  (uintptr_t) amd64_exception_prologue_11,
  (uintptr_t) amd64_exception_prologue_12,
  (uintptr_t) amd64_exception_prologue_13,
  (uintptr_t) amd64_exception_prologue_14,
  (uintptr_t) amd64_exception_prologue_15,
  (uintptr_t) amd64_exception_prologue_16,
  (uintptr_t) amd64_exception_prologue_17,
  (uintptr_t) amd64_exception_prologue_18,
  (uintptr_t) amd64_exception_prologue_19,
  (uintptr_t) amd64_exception_prologue_20,
  (uintptr_t) amd64_exception_prologue_21,
  (uintptr_t) amd64_exception_prologue_22,
  (uintptr_t) amd64_exception_prologue_23,
  (uintptr_t) amd64_exception_prologue_24,
  (uintptr_t) amd64_exception_prologue_25,
  (uintptr_t) amd64_exception_prologue_26,
  (uintptr_t) amd64_exception_prologue_27,
  (uintptr_t) amd64_exception_prologue_28,
  (uintptr_t) amd64_exception_prologue_29,
  (uintptr_t) amd64_exception_prologue_30,
  (uintptr_t) amd64_exception_prologue_31
};

/**
 * IRQs that the RTEMS Interrupt Manager will manage
 * @see DISTINCT_INTERRUPT_ENTRY
 */
static const uintptr_t rtemsIRQs[
  BSP_IRQ_VECTOR_NUMBER - BSP_VECTOR_APIC_FIRST
] = {
  (uintptr_t) rtems_irq_prologue_32,
  (uintptr_t) rtems_irq_prologue_33,
  (uintptr_t) rtems_irq_prologue_34
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
  /*
   * Acknowledge before the handlers run.  All interrupt sources of this BSP
   * are edge triggered, so the request is not raised again by the
   * acknowledge.  It clears the in service bit and thus the processor
   * priority, which is what allows a handler to enable interrupts and take a
   * nested interrupt of the same vector.
   */
  lapic_eoi();

  bsp_interrupt_handler_dispatch(vector);
}

void amd64_exception_handler(CPU_Exception_frame *frame)
{
  _Terminate(RTEMS_FATAL_SOURCE_EXCEPTION, (rtems_fatal_code) frame);
}

bool bsp_interrupt_is_valid_vector(rtems_vector_number vector)
{
  /*
   * The vectors below the first Local APIC vector are the exception vectors of
   * the processor.  They terminate the system and are not vectors of the
   * interrupt manager.
   */
  return vector >= BSP_VECTOR_APIC_FIRST &&
    vector < (rtems_vector_number) BSP_INTERRUPT_VECTOR_COUNT;
}

void bsp_interrupt_facility_initialize(void)
{
  uintptr_t old;

  for (uint32_t i = 0; i < BSP_VECTOR_APIC_FIRST; i++) {
    amd64_install_raw_interrupt(i, amd64_exceptions[i], &old);
  }

  for (uint32_t i = BSP_VECTOR_APIC_FIRST; i < BSP_IRQ_VECTOR_NUMBER; i++) {
    amd64_install_raw_interrupt(
      i,
      rtemsIRQs[i - BSP_VECTOR_APIC_FIRST],
      &old
    );
  }

  lidt(&amd64_idtr);

  if (lapic_initialize() == false) {
    bsp_fatal(BSP_FATAL_INTERRUPT_INITIALIZATION);
  }
}

/*
 * Only the local vector table entry of the Local APIC timer has a mask bit.
 * The interprocessor and software vectors are delivered by a write to the
 * interrupt command register and there is nothing which masks them.
 */
rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (vector != BSP_VECTOR_APIC_TIMER) {
    return RTEMS_UNSATISFIED;
  }

  lapic_timer_set_masked(true);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_set_priority(
  rtems_vector_number vector,
  uint32_t priority
)
{
  (void) vector;
  (void) priority;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_get_priority(
  rtems_vector_number vector,
  uint32_t *priority
)
{
  (void) vector;
  (void) priority;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(priority != NULL);
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  attributes->is_maskable = true;
  attributes->can_raise = true;
  attributes->cleared_by_acknowledge = true;
#ifdef RTEMS_SMP
  attributes->can_raise_on = true;
#endif

  if (vector == BSP_VECTOR_APIC_TIMER) {
    attributes->can_enable = true;
    attributes->maybe_enable = true;
    attributes->can_disable = true;
    attributes->maybe_disable = true;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(pending != NULL);

  *pending = lapic_is_pending(vector);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_raise(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  lapic_raise_self(vector);
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_clear(rtems_vector_number vector)
{
  (void) vector;

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

  if (vector == BSP_VECTOR_APIC_TIMER) {
    *enabled = !lapic_timer_is_masked();
  } else {
    *enabled = true;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  if (vector != BSP_VECTOR_APIC_TIMER) {
    return RTEMS_UNSATISFIED;
  }

  lapic_timer_set_masked(false);
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
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  lapic_send_ipi(cpu_index, (uint8_t) vector);
  return RTEMS_SUCCESSFUL;
}
#endif
