/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsSharedAPIC
 *
 * @brief APIC definitions
 */

/*
 * Copyright (C) 2024 Matheus Pecoraro
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
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _BSP_APIC_H
#define _BSP_APIC_H

#include <rtems/score/basedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The address of the MSR pointing to the APIC base physical address */
#define APIC_BASE_MSR        0x1B
/* Value to hardware-enable the APIC through the APIC_BASE_MSR */
#define APIC_BASE_MSR_ENABLE 0x800

#define xAPIC_MAX_APIC_ID    0xFE

/*
 * APIC register definitions.
 */

#define LAPIC_ID                0x20
#define LAPIC_VER               0x30
#define LAPIC_TPR               0x80
#define LAPIC_APR               0x90
#define LAPIC_PPR               0xA0
#define LAPIC_EOI               0xB0
#define LAPIC_LDR               0xD0
#define LAPIC_DFR               0xE0
#define LAPIC_SPIV              0xF0
#define LAPIC_SPIV_ENABLE_APIC  0x100
#define LAPIC_ISR               0x100
#define LAPIC_TMR               0x180
#define LAPIC_IRR               0x200
#define LAPIC_ESR               0x280
#define LAPIC_ICR_LOW           0x300
#define LAPIC_ICR_HIGH          0x310

#define LAPIC_ICR_DS_SELF       0x40000
#define LAPIC_ICR_DS_ALLINC     0x80000
#define LAPIC_ICR_DS_ALLEX      0xC0000
#define LAPIC_ICR_TM_LEVEL      0x8000
#define LAPIC_ICR_LEVELASSERT   0x4000
#define LAPIC_ICR_STATUS_PEND   0x1000
#define LAPIC_ICR_DM_LOGICAL    0x800
#define LAPIC_ICR_DM_LOWPRI     0x100
#define LAPIC_ICR_DM_SMI        0x200
#define LAPIC_ICR_DM_NMI        0x400
#define LAPIC_ICR_DM_INIT       0x500
#define LAPIC_ICR_DM_SIPI       0x600

#define LAPIC_LVTT              0x320
#define LAPIC_LVTPC             0x340
#define LAPIC_LVT0              0x350
#define LAPIC_LVT1              0x360
#define LAPIC_LVTE              0x370
#define LAPIC_TICR              0x380
#define LAPIC_TCCR              0x390
#define LAPIC_TDCR              0x3E0

/*
 * Since the LAPIC registers are contained in an array of 32-bit elements
 * these byte-offsets need to be divided by 4 to index the array.
 */
#define LAPIC_OFFSET(val)             ((val) >> 2)

#define LAPIC_REGISTER_ID              LAPIC_OFFSET(LAPIC_ID)
#define LAPIC_REGISTER_EOI             LAPIC_OFFSET(LAPIC_EOI)
#define LAPIC_REGISTER_SPURIOUS       LAPIC_OFFSET(LAPIC_SPIV)
#define LAPIC_REGISTER_ESR             LAPIC_OFFSET(LAPIC_ESR)
#define LAPIC_REGISTER_ICR_LOW         LAPIC_OFFSET(LAPIC_ICR_LOW)
#define LAPIC_REGISTER_ICR_HIGH        LAPIC_OFFSET(LAPIC_ICR_HIGH)

#define LAPIC_EOI_ACK                 0
#define LAPIC_SPURIOUS_ENABLE         0x100

#ifdef __cplusplus
}
#endif

#endif /* _BSP_APIC_H */