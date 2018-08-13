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

#ifndef _AMD64_APIC_H
#define _AMD64_APIC_H

#ifdef __cplusplus
extern "C" {
#endif

/* The address of the MSR pointing to the APIC base physical address */
#define APIC_BASE_MSR             0x1B
/* Value to hardware-enable the APIC through the APIC_BASE_MSR */
#define APIC_BASE_MSR_ENABLE      0x800

/*
 * Since amd64_apic_base is an array of 32-bit elements, these byte-offsets
 * need to be divided by 4 to index the array.
 */
#define APIC_OFFSET(val) (val >> 2)

#define APIC_REGISTER_APICID        APIC_OFFSET(0x20)
#define APIC_REGISTER_EOI           APIC_OFFSET(0x0B0)
#define APIC_REGISTER_SPURIOUS      APIC_OFFSET(0x0F0)
#define APIC_REGISTER_LVT_TIMER     APIC_OFFSET(0x320)
#define APIC_REGISTER_TIMER_INITCNT APIC_OFFSET(0x380)
#define APIC_REGISTER_TIMER_CURRCNT APIC_OFFSET(0x390)
#define APIC_REGISTER_TIMER_DIV     APIC_OFFSET(0x3E0)

#define APIC_DISABLE                0x10000
#define APIC_EOI_ACK                0
#define APIC_SELECT_TMR_PERIODIC    0x20000
#define APIC_SPURIOUS_ENABLE        0x100

#ifdef __cplusplus
}
#endif

#endif /* _AMD64_APIC_H */
