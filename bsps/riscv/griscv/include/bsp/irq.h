/**
 * @file
 *
 * @ingroup RISCV_IRQ
 *
 * @brief Interrupt definitions.
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

#ifndef LIBBSP_RISCV_GRISCV_IRQ_H
#define LIBBSP_RISCV_GRISCV_IRQ_H

#ifndef ASM

#include <bsp.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>
#include <rtems/score/processormask.h>

#define RISCV_INTERRUPT_VECTOR_SOFTWARE 0

#define RISCV_INTERRUPT_VECTOR_TIMER 1

#define RISCV_INTERRUPT_VECTOR_EXTERNAL(x) ((x) + 2)

#define RISCV_INTERRUPT_VECTOR_IS_EXTERNAL(x) ((x) >= 2)

#define RISCV_INTERRUPT_VECTOR_EXTERNAL_TO_INDEX(x) ((x) - 2)

#define BSP_INTERRUPT_VECTOR_MIN 0

#define BSP_INTERRUPT_VECTOR_MAX RISCV_INTERRUPT_VECTOR_EXTERNAL(RISCV_MAXIMUM_EXTERNAL_INTERRUPTS - 1)

void bsp_interrupt_set_affinity(
  rtems_vector_number vector,
  const Processor_mask *affinity
);

void bsp_interrupt_get_affinity(
  rtems_vector_number vector,
  Processor_mask *affinity
);

#endif /* ASM */

#endif /* LIBBSP_RISCV_GRISCV_IRQ_H */
