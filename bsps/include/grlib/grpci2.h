/* SPDX-License-Identifier: BSD-2-Clause */

/*  GRLIB GRPCI2 PCI HOST driver.
 * 
 *  COPYRIGHT (c) 2011
 *  Cobham Gaisler AB.
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

#ifndef __GRPCI2_H__
#define __GRPCI2_H__

#ifdef __cplusplus
extern "C" {
#endif

extern void grpci2_register_drv(void);

/* Driver Resources:
 *
 * PCI Interrupts
 * ==============
 * The interrupt settings are normally autodetected from Plyg&Play, however
 * if IRQs are routed using custom GPIO pins in order to reduce the PIN count
 * reserved for PCI, the options below can be used to tell GRPCI2 driver which
 * System IRQ a PCI interrupt is connected to.
 * Name="INTA#", Type=INT, System Interrupt number that PCI INTA is connected to
 * Name="INTB#", Type=INT, System Interrupt number that PCI INTB is connected to
 * Name="INTC#", Type=INT, System Interrupt number that PCI INTC is connected to
 * Name="INTD#", Type=INT, System Interrupt number that PCI INTD is connected to
 *
 * Name="IRQmask", Type=INT, 
 *
 * PCI Bytetwisting (endianess)
 * ============================
 * Name="byteTwisting", Type=INT, Enable/Disable Bytetwisting by hardware
 *
 * PCI Latency timer
 * ============================
 * Name="latencyTimer", Type=INT, Set the latency timer
 *
 * PCI Host's Target BARs setup
 * ============================
 * The Host's BARs are not configured by the configuration routines, by default
 * the BARs are configured disabled (BAR=0) except for BAR0 which is mapped to
 * the Main Memory for the Host.
 * Name="tgtBarCfg", Type=PTR (*grpci2_pcibar_cfg), Target PCI BARs of Host
 */

/* When the Host acts as a target on the PCI bus, the PCI BARs of the host's
 * configuration space determine at which PCI address the Host will be accessed
 * at and when accessing a BAR which AMBA address it will be translated to.
 */
struct grpci2_pcibar_cfg {
	unsigned int pciadr;	/* PCI address of BAR (BAR content) */
	unsigned int ahbadr;	/* 'pciadr' translated to this AHB Address */
	unsigned int barsize;	/* PCI BAR Size, must be a power of 2 */
};

#ifdef __cplusplus
}
#endif

#endif
