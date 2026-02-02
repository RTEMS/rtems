/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsm68kuC5282
 *
 * @brief Core IRQ definitions
 */

/*
 * Copyright (c) 2025 Jeremy Lorelli <lorelli@slac.stanford.edu>
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

#ifndef LIBBSP_M68K_UC5282_IRQ_H
#define LIBBSP_M68K_UC5282_IRQ_H

#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>

#define BSP_INTERRUPT_CUSTOM_VALID_VECTOR

#define BSP_INTERRUPT_VECTOR_COUNT 192

#define BSP_VME_INTERRUPT_VECTOR_COUNT (255 - BSP_INTERRUPT_VECTOR_COUNT)

#define BSP_FIRST_VME_INTERRUPT_VECTOR BSP_INTERRUPT_VECTOR_COUNT

extern void uC5282_interrupt_vector_install(rtems_vector_number vector);
extern void uC5282_interrupt_vector_remove(rtems_vector_number vector);

#define bsp_interrupt_vector_install(v) uC5282_interrupt_vector_install(v)
#define bsp_interrupt_vector_remove(v) uC5282_interrupt_vector_remove(v)

#endif // LIBBSP_M68K_UC5282_IRQ_H
