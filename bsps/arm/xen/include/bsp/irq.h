/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2019 DornerWorks
 * Written by Jeff Kubascik <jeff.kubascik@dornerworks.com>
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

#ifndef LIBBSP_ARM_XEN_IRQ_H
#define LIBBSP_ARM_XEN_IRQ_H

#ifndef ASM

#include <rtems/irq.h>
#include <rtems/irq-extension.h>

#include <bsp/arm-gic-irq.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BSP_INTERRUPT_VECTOR_MIN 0
#define BSP_INTERRUPT_VECTOR_MAX 1023

/* Xen guest interrupts */
#define GUEST_TIMER_VIRT_PPI 27
#define GUEST_TIMER_PHYS_S_PPI 29
#define GUEST_TIMER_PHYS_NS_PPI 30
#define GUEST_EVTCHN_PPI 31

#define GUEST_VPL011_SPI 32

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASM */

#endif /* LIBBSP_ARM_XEN_IRQ_H */
