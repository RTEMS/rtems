/*
 * Author: Erich Boleyn  <erich@uruk.org>
 *         http://www.uruk.org/~erich/
 *
 * Copyright (c) 1997-2011 Erich Boleyn.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  Header file for Intel Architecture local and I/O APIC definitions.
 *
 *  This file was created from information in the Intel Pentium Pro
 *  Family Developer's Manual, Volume 3: Operating System Writer's
 *  Manual, order number 242692-001, which can be ordered from the
 *  Intel literature center.
 */

#ifndef _APIC_H
#define _APIC_H

/*
 *  APIC Defines.
 */

/*
 * Recommendation:  Don't use this except for MSI interrupt delivery.
 * In general, the "Destination Mode" can be used to control this, since
 * it is DIFFERENT (0xF) for Pentium and P6, but not on the same APIC
 * version for AMD Opteron.
 */
#define APIC_BCAST_ID                           0xFF

/*
 *  APIC register definitions
 */

/*
 *  Shared defines for I/O and local APIC definitions
 */
/* APIC version register */
#define APIC_VERSION(x)                         ((x) & 0xFF)
#define APIC_MAXREDIR(x)                        (((x) >> 16) & 0xFF)
/* APIC id register */
#define APIC_ID(x)                              ((x) >> 24)
#define APIC_VER_NEW                            0x10

#define IOAPIC_REGSEL                           0
#define IOAPIC_RW                               0x10
#define         IOAPIC_ID                       0
#define         IOAPIC_VER                      1
#define         IOAPIC_REDIR                    0x10

#define LAPIC_ID                                0x20
#define LAPIC_VER                               0x30
#define LAPIC_TPR                               0x80
#define LAPIC_APR                               0x90
#define LAPIC_PPR                               0xA0
#define LAPIC_EOI                               0xB0
#define LAPIC_LDR                               0xD0
#define LAPIC_DFR                               0xE0
#define LAPIC_SPIV                              0xF0
#define         LAPIC_SPIV_ENABLE_APIC          0x100
#define LAPIC_ISR                               0x100
#define LAPIC_TMR                               0x180
#define LAPIC_IRR                               0x200
#define LAPIC_ESR                               0x280
#define LAPIC_ICR                               0x300
#define   LAPIC_ICR_DS_SELF                     0x40000
#define   LAPIC_ICR_DS_ALLINC                   0x80000
#define   LAPIC_ICR_DS_ALLEX                    0xC0000
#define   LAPIC_ICR_TM_LEVEL                    0x8000
#define   LAPIC_ICR_LEVELASSERT                 0x4000
#define   LAPIC_ICR_STATUS_PEND                 0x1000
#define   LAPIC_ICR_DM_LOGICAL                  0x800
#define   LAPIC_ICR_DM_LOWPRI                   0x100
#define   LAPIC_ICR_DM_SMI                      0x200
#define   LAPIC_ICR_DM_NMI                      0x400
#define   LAPIC_ICR_DM_INIT                     0x500
#define   LAPIC_ICR_DM_SIPI                     0x600
#define LAPIC_LVTT                              0x320
#define LAPIC_LVTPC                             0x340
#define LAPIC_LVT0                              0x350
#define LAPIC_LVT1                              0x360
#define LAPIC_LVTE                              0x370
#define LAPIC_TICR                              0x380
#define LAPIC_TCCR                              0x390
#define LAPIC_TDCR                              0x3E0

#endif  /* _APIC_H */
