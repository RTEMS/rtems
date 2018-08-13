/*
 * Copyright (c) 2018.
 * Amaan Cheval <amaan.cheval@gmail.com>
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

#ifndef _RTEMS_SCORE_IDT_H
#define _RTEMS_SCORE_IDT_H

#include <rtems/score/basedefs.h>
#include <rtems/rtems/intr.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IDT_INTERRUPT_GATE     (0b1110)
#define IDT_PRESENT            (0b10000000)

/*
 * XXX: The IDT size should be smaller given that we likely won't map all 256
 * vectors, but for simplicity, this works better.
 */
#define IDT_SIZE               256

/* Target vector number for spurious IRQs */
#define BSP_VECTOR_SPURIOUS    0xFF
/* Target vector number for the APIC timer */
#define BSP_VECTOR_APIC_TIMER  32

typedef struct _interrupt_descriptor {
  uint16_t offset_0;              // bits 0-15
  uint16_t segment_selector;      // a segment selector in the GDT or LDT
  /* bits 0-2 are the offset into the IST, stored in the TSS */
  uint8_t  interrupt_stack_table;
  uint8_t  type_and_attributes;
  uint16_t offset_1;              // bits 16-31
  uint32_t offset_2;              // bits 32-63
  uint32_t reserved_zero;
} interrupt_descriptor;

extern interrupt_descriptor amd64_idt[IDT_SIZE];

struct idt_record {
  uint16_t  limit;      /* Size of IDT array - 1 */
  uintptr_t base;       /* Pointer to IDT array  */
} RTEMS_PACKED;

RTEMS_STATIC_ASSERT(
  sizeof(struct idt_record) == 10,
  "IDT pointer must be exactly 10 bytes"
);

void lidt(struct idt_record *idtr);

interrupt_descriptor amd64_create_interrupt_descriptor(
  uintptr_t handler, uint8_t types_and_attributes
);

uintptr_t amd64_get_handler_from_idt(uint32_t vector);

void amd64_install_raw_interrupt(
  uint32_t vector, uintptr_t new_handler, uintptr_t *old_handler
);

/*
 * Called by _ISR_Handler to dispatch "RTEMS interrupts", i.e. call the
 * registered RTEMS ISR.
 */
void amd64_dispatch_isr(rtems_vector_number vector);

/* Defined in isr_handler.S */
extern void rtems_irq_prologue_0(void);
extern void rtems_irq_prologue_1(void);
extern void rtems_irq_prologue_2(void);
extern void rtems_irq_prologue_3(void);
extern void rtems_irq_prologue_4(void);
extern void rtems_irq_prologue_5(void);
extern void rtems_irq_prologue_6(void);
extern void rtems_irq_prologue_7(void);
extern void rtems_irq_prologue_8(void);
extern void rtems_irq_prologue_9(void);
extern void rtems_irq_prologue_10(void);
extern void rtems_irq_prologue_11(void);
extern void rtems_irq_prologue_12(void);
extern void rtems_irq_prologue_13(void);
extern void rtems_irq_prologue_14(void);
extern void rtems_irq_prologue_15(void);
extern void rtems_irq_prologue_16(void);
extern void rtems_irq_prologue_17(void);
extern void rtems_irq_prologue_18(void);
extern void rtems_irq_prologue_19(void);
extern void rtems_irq_prologue_20(void);
extern void rtems_irq_prologue_21(void);
extern void rtems_irq_prologue_22(void);
extern void rtems_irq_prologue_23(void);
extern void rtems_irq_prologue_24(void);
extern void rtems_irq_prologue_25(void);
extern void rtems_irq_prologue_26(void);
extern void rtems_irq_prologue_27(void);
extern void rtems_irq_prologue_28(void);
extern void rtems_irq_prologue_29(void);
extern void rtems_irq_prologue_30(void);
extern void rtems_irq_prologue_31(void);
extern void rtems_irq_prologue_32(void);

#ifdef __cplusplus
}
#endif

#endif
