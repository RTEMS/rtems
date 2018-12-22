/*  GRLIB GRPCI2 PCI HOST driver.
 * 
 *  COPYRIGHT (c) 2011
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
