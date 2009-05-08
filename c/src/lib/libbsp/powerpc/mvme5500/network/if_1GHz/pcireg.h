/*	$NetBSD: pcireg.h,v 1.44 2003/12/02 16:31:06 briggs Exp $	*/

/*
 * Copyright (c) 1995, 1996, 1999, 2000
 *     Christopher G. Demetriou.  All rights reserved.
 * Copyright (c) 1994, 1996 Charles M. Hannum.  All rights reserved.
 * Copyright (C) 2007 Brookhaven National Laboratory, Shuchen Kate Feng
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Charles M. Hannum.
 * 4. The name of the author may not be used to endorse or promote products
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
#include <bsp.h>

/*
 * PCI Class and Revision Register; defines type and revision of device.
 */
#define	PCI_CLASS_REG			0x08

#define	PCI_CLASS_SHIFT				24
#define	PCI_CLASS_MASK				0xff
#define	PCI_CLASS(cr) \
	    (((cr) >> PCI_CLASS_SHIFT) & PCI_CLASS_MASK)

#define	PCI_SUBCLASS_SHIFT			16
#define	PCI_SUBCLASS_MASK			0xff
#define	PCI_SUBCLASS(cr) \
	    (((cr) >> PCI_SUBCLASS_SHIFT) & PCI_SUBCLASS_MASK)

#define	PCI_INTERFACE_SHIFT			8
#define	PCI_INTERFACE_MASK			0xff
#define	PCI_INTERFACE(cr) \
	    (((cr) >> PCI_INTERFACE_SHIFT) & PCI_INTERFACE_MASK)

#define	PCI_REVISION_SHIFT			0
#define	PCI_REVISION_MASK			0xff
#define	PCI_REVISION(cr) \
	    (((cr) >> PCI_REVISION_SHIFT) & PCI_REVISION_MASK)

#define	PCI_CLASS_CODE(mainclass, subclass, interface) \
	    ((((mainclass) & PCI_CLASS_MASK) << PCI_CLASS_SHIFT) | \
	     (((subclass) & PCI_SUBCLASS_MASK) << PCI_SUBCLASS_SHIFT) | \
	     (((interface) & PCI_INTERFACE_MASK) << PCI_INTERFACE_SHIFT))

/* base classes */
#define	PCI_CLASS_PREHISTORIC			0x00
#define	PCI_CLASS_MASS_STORAGE			0x01
#define	PCI_CLASS_NETWORK			0x02
#define	PCI_CLASS_DISPLAY			0x03
#define	PCI_CLASS_MULTIMEDIA			0x04
#define	PCI_CLASS_MEMORY			0x05
#define	PCI_CLASS_BRIDGE			0x06
#define	PCI_CLASS_COMMUNICATIONS		0x07
#define	PCI_CLASS_SYSTEM			0x08
#define	PCI_CLASS_INPUT				0x09
#define	PCI_CLASS_DOCK				0x0a
#define	PCI_CLASS_PROCESSOR			0x0b
#define	PCI_CLASS_SERIALBUS			0x0c
#define	PCI_CLASS_WIRELESS			0x0d
#define	PCI_CLASS_I2O				0x0e
#define	PCI_CLASS_SATCOM			0x0f
#define	PCI_CLASS_CRYPTO			0x10
#define	PCI_CLASS_DASP				0x11
#define	PCI_CLASS_UNDEFINED			0xff

/* 0x00 prehistoric subclasses */
#define	PCI_SUBCLASS_PREHISTORIC_MISC		0x00
#define	PCI_SUBCLASS_PREHISTORIC_VGA		0x01

/* 0x01 mass storage subclasses */
#define	PCI_SUBCLASS_MASS_STORAGE_SCSI		0x00
#define	PCI_SUBCLASS_MASS_STORAGE_IDE		0x01
#define	PCI_SUBCLASS_MASS_STORAGE_FLOPPY	0x02
#define	PCI_SUBCLASS_MASS_STORAGE_IPI		0x03
#define	PCI_SUBCLASS_MASS_STORAGE_RAID		0x04
#define	PCI_SUBCLASS_MASS_STORAGE_ATA		0x05
#define	PCI_SUBCLASS_MASS_STORAGE_SATA		0x06
#define	PCI_SUBCLASS_MASS_STORAGE_MISC		0x80

/* 0x02 network subclasses */
#define	PCI_SUBCLASS_NETWORK_ETHERNET		0x00
#define	PCI_SUBCLASS_NETWORK_TOKENRING		0x01
#define	PCI_SUBCLASS_NETWORK_FDDI		0x02
#define	PCI_SUBCLASS_NETWORK_ATM		0x03
#define	PCI_SUBCLASS_NETWORK_ISDN		0x04
#define	PCI_SUBCLASS_NETWORK_WORLDFIP		0x05
#define	PCI_SUBCLASS_NETWORK_PCIMGMULTICOMP	0x06
#define	PCI_SUBCLASS_NETWORK_MISC		0x80

/* 0x03 display subclasses */
#define	PCI_SUBCLASS_DISPLAY_VGA		0x00
#define	PCI_SUBCLASS_DISPLAY_XGA		0x01
#define	PCI_SUBCLASS_DISPLAY_3D			0x02
#define	PCI_SUBCLASS_DISPLAY_MISC		0x80

/* 0x04 multimedia subclasses */
#define	PCI_SUBCLASS_MULTIMEDIA_VIDEO		0x00
#define	PCI_SUBCLASS_MULTIMEDIA_AUDIO		0x01
#define	PCI_SUBCLASS_MULTIMEDIA_TELEPHONY	0x02
#define	PCI_SUBCLASS_MULTIMEDIA_MISC		0x80

/* 0x05 memory subclasses */
#define	PCI_SUBCLASS_MEMORY_RAM			0x00
#define	PCI_SUBCLASS_MEMORY_FLASH		0x01
#define	PCI_SUBCLASS_MEMORY_MISC		0x80

/* 0x06 bridge subclasses */
#define	PCI_SUBCLASS_BRIDGE_HOST		0x00
#define	PCI_SUBCLASS_BRIDGE_ISA			0x01
#define	PCI_SUBCLASS_BRIDGE_EISA		0x02
#define	PCI_SUBCLASS_BRIDGE_MC			0x03	/* XXX _MCA? */
#define	PCI_SUBCLASS_BRIDGE_PCI			0x04
#define	PCI_SUBCLASS_BRIDGE_PCMCIA		0x05
#define	PCI_SUBCLASS_BRIDGE_NUBUS		0x06
#define	PCI_SUBCLASS_BRIDGE_CARDBUS		0x07
#define	PCI_SUBCLASS_BRIDGE_RACEWAY		0x08
#define	PCI_SUBCLASS_BRIDGE_STPCI		0x09
#define	PCI_SUBCLASS_BRIDGE_INFINIBAND		0x0a
#define	PCI_SUBCLASS_BRIDGE_MISC		0x80

/* 0x07 communications subclasses */
#define	PCI_SUBCLASS_COMMUNICATIONS_SERIAL	0x00
#define	PCI_SUBCLASS_COMMUNICATIONS_PARALLEL	0x01
#define	PCI_SUBCLASS_COMMUNICATIONS_MPSERIAL	0x02
#define	PCI_SUBCLASS_COMMUNICATIONS_MODEM	0x03
#define	PCI_SUBCLASS_COMMUNICATIONS_GPIB	0x04
#define	PCI_SUBCLASS_COMMUNICATIONS_SMARTCARD	0x05
#define	PCI_SUBCLASS_COMMUNICATIONS_MISC	0x80

/* 0x08 system subclasses */
#define	PCI_SUBCLASS_SYSTEM_PIC			0x00
#define	PCI_SUBCLASS_SYSTEM_DMA			0x01
#define	PCI_SUBCLASS_SYSTEM_TIMER		0x02
#define	PCI_SUBCLASS_SYSTEM_RTC			0x03
#define	PCI_SUBCLASS_SYSTEM_PCIHOTPLUG		0x04
#define	PCI_SUBCLASS_SYSTEM_MISC		0x80

/* 0x09 input subclasses */
#define	PCI_SUBCLASS_INPUT_KEYBOARD		0x00
#define	PCI_SUBCLASS_INPUT_DIGITIZER		0x01
#define	PCI_SUBCLASS_INPUT_MOUSE		0x02
#define	PCI_SUBCLASS_INPUT_SCANNER		0x03
#define	PCI_SUBCLASS_INPUT_GAMEPORT		0x04
#define	PCI_SUBCLASS_INPUT_MISC			0x80

/* 0x0a dock subclasses */
#define	PCI_SUBCLASS_DOCK_GENERIC		0x00
#define	PCI_SUBCLASS_DOCK_MISC			0x80

/* 0x0b processor subclasses */
#define	PCI_SUBCLASS_PROCESSOR_386		0x00
#define	PCI_SUBCLASS_PROCESSOR_486		0x01
#define	PCI_SUBCLASS_PROCESSOR_PENTIUM		0x02
#define	PCI_SUBCLASS_PROCESSOR_ALPHA		0x10
#define	PCI_SUBCLASS_PROCESSOR_POWERPC		0x20
#define	PCI_SUBCLASS_PROCESSOR_MIPS		0x30
#define	PCI_SUBCLASS_PROCESSOR_COPROC		0x40

/* 0x0c serial bus subclasses */
#define	PCI_SUBCLASS_SERIALBUS_FIREWIRE		0x00
#define	PCI_SUBCLASS_SERIALBUS_ACCESS		0x01
#define	PCI_SUBCLASS_SERIALBUS_SSA		0x02
#define	PCI_SUBCLASS_SERIALBUS_USB		0x03
#define	PCI_SUBCLASS_SERIALBUS_FIBER		0x04	/* XXX _FIBRECHANNEL */
#define	PCI_SUBCLASS_SERIALBUS_SMBUS		0x05
#define	PCI_SUBCLASS_SERIALBUS_INFINIBAND	0x06
#define	PCI_SUBCLASS_SERIALBUS_IPMI		0x07
#define	PCI_SUBCLASS_SERIALBUS_SERCOS		0x08
#define	PCI_SUBCLASS_SERIALBUS_CANBUS		0x09

/* 0x0d wireless subclasses */
#define	PCI_SUBCLASS_WIRELESS_IRDA		0x00
#define	PCI_SUBCLASS_WIRELESS_CONSUMERIR	0x01
#define	PCI_SUBCLASS_WIRELESS_RF		0x10
#define	PCI_SUBCLASS_WIRELESS_BLUETOOTH		0x11
#define	PCI_SUBCLASS_WIRELESS_BROADBAND		0x12
#define	PCI_SUBCLASS_WIRELESS_802_11A		0x20
#define	PCI_SUBCLASS_WIRELESS_802_11B		0x21
#define	PCI_SUBCLASS_WIRELESS_MISC		0x80

/* 0x0e I2O (Intelligent I/O) subclasses */
#define	PCI_SUBCLASS_I2O_STANDARD		0x00

/* 0x0f satellite communication subclasses */
/*	PCI_SUBCLASS_SATCOM_???			0x00	/ * XXX ??? */
#define	PCI_SUBCLASS_SATCOM_TV			0x01
#define	PCI_SUBCLASS_SATCOM_AUDIO		0x02
#define	PCI_SUBCLASS_SATCOM_VOICE		0x03
#define	PCI_SUBCLASS_SATCOM_DATA		0x04

/* 0x10 encryption/decryption subclasses */
#define	PCI_SUBCLASS_CRYPTO_NETCOMP		0x00
#define	PCI_SUBCLASS_CRYPTO_ENTERTAINMENT	0x10
#define	PCI_SUBCLASS_CRYPTO_MISC		0x80

/* 0x11 data acquisition and signal processing subclasses */
#define	PCI_SUBCLASS_DASP_DPIO			0x00
#define	PCI_SUBCLASS_DASP_TIMEFREQ		0x01
#define	PCI_SUBCLASS_DASP_SYNC			0x10
#define	PCI_SUBCLASS_DASP_MGMT			0x20
#define	PCI_SUBCLASS_DASP_MISC			0x80

/*
 * PCI BIST/Header Type/Latency Timer/Cache Line Size Register.
 */
#define	PCI_BHLC_REG			0x0c

#define	PCI_BIST_SHIFT				24
#define	PCI_BIST_MASK				0xff
#define	PCI_BIST(bhlcr) \
	    (((bhlcr) >> PCI_BIST_SHIFT) & PCI_BIST_MASK)

#define	PCI_HDRTYPE_SHIFT			16
#define	PCI_HDRTYPE_MASK			0xff
#define	PCI_HDRTYPE(bhlcr) \
	    (((bhlcr) >> PCI_HDRTYPE_SHIFT) & PCI_HDRTYPE_MASK)

#define	PCI_HDRTYPE_TYPE(bhlcr) \
	    (PCI_HDRTYPE(bhlcr) & 0x7f)
#define	PCI_HDRTYPE_MULTIFN(bhlcr) \
	    ((PCI_HDRTYPE(bhlcr) & 0x80) != 0)

#define	PCI_LATTIMER_SHIFT			8
#define	PCI_LATTIMER_MASK			0xff
#define	PCI_LATTIMER(bhlcr) \
	    (((bhlcr) >> PCI_LATTIMER_SHIFT) & PCI_LATTIMER_MASK)

#define	PCI_CACHELINE_SHIFT			0
#define	PCI_CACHELINE_MASK			0xff
#define	PCI_CACHELINE(bhlcr) \
	    (((bhlcr) >> PCI_CACHELINE_SHIFT) & PCI_CACHELINE_MASK)

#define PCI_BHLC_CODE(bist,type,multi,latency,cacheline)		\
	    ((((bist) & PCI_BIST_MASK) << PCI_BIST_SHIFT) |		\
	     (((type) & PCI_HDRTYPE_MASK) << PCI_HDRTYPE_SHIFT) |	\
	     (((multi)?0x80:0) << PCI_HDRTYPE_SHIFT) |			\
	     (((latency) & PCI_LATTIMER_MASK) << PCI_LATTIMER_SHIFT) |	\
	     (((cacheline) & PCI_CACHELINE_MASK) << PCI_CACHELINE_SHIFT))

/*
 * PCI header type
 */
#define PCI_HDRTYPE_DEVICE	0
#define PCI_HDRTYPE_PPB		1
#define PCI_HDRTYPE_PCB		2

/*
 * Mapping registers
 */
#define	PCI_MAPREG_START		0x10
#define	PCI_MAPREG_END			0x28
#define	PCI_MAPREG_ROM			0x30
#define	PCI_MAPREG_PPB_END		0x18
#define	PCI_MAPREG_PCB_END		0x14

#define	PCI_MAPREG_TYPE(mr)						\
	    ((mr) & PCI_MAPREG_TYPE_MASK)
#define	PCI_MAPREG_TYPE_MASK			0x00000001

#define	PCI_MAPREG_TYPE_MEM			0x00000000
#define	PCI_MAPREG_TYPE_IO			0x00000001
#define	PCI_MAPREG_ROM_ENABLE			0x00000001

#define	PCI_MAPREG_MEM_TYPE(mr)						\
	    ((mr) & PCI_MAPREG_MEM_TYPE_MASK)
#define	PCI_MAPREG_MEM_TYPE_MASK		0x00000006

#define	PCI_MAPREG_MEM_TYPE_32BIT		0x00000000
#define	PCI_MAPREG_MEM_TYPE_32BIT_1M		0x00000002
#define	PCI_MAPREG_MEM_TYPE_64BIT		0x00000004

#define	PCI_MAPREG_MEM_PREFETCHABLE(mr)				\
	    (((mr) & PCI_MAPREG_MEM_PREFETCHABLE_MASK) != 0)
#define	PCI_MAPREG_MEM_PREFETCHABLE_MASK	0x00000008

#define	PCI_MAPREG_MEM_ADDR(mr)						\
	    ((mr) & PCI_MAPREG_MEM_ADDR_MASK)
#define	PCI_MAPREG_MEM_SIZE(mr)						\
	    (PCI_MAPREG_MEM_ADDR(mr) & -PCI_MAPREG_MEM_ADDR(mr))
#define	PCI_MAPREG_MEM_ADDR_MASK		0xfffffff0

#define	PCI_MAPREG_MEM64_ADDR(mr)					\
	    ((mr) & PCI_MAPREG_MEM64_ADDR_MASK)
#define	PCI_MAPREG_MEM64_SIZE(mr)					\
	    (PCI_MAPREG_MEM64_ADDR(mr) & -PCI_MAPREG_MEM64_ADDR(mr))
#define	PCI_MAPREG_MEM64_ADDR_MASK		0xfffffffffffffff0ULL

#define	PCI_MAPREG_IO_ADDR(mr)						\
	    ((mr+PCI0_IO_BASE) & PCI_MAPREG_IO_ADDR_MASK)
#define	PCI_MAPREG_IO_SIZE(mr)						\
	    (PCI_MAPREG_IO_ADDR(mr) & -PCI_MAPREG_IO_ADDR(mr))
#define	PCI_MAPREG_IO_ADDR_MASK			0xfffffffc

#define PCI_MAPREG_SIZE_TO_MASK(size)					\
	    (-(size))

#define PCI_MAPREG_NUM(offset)						\
	    (((unsigned)(offset)-PCI_MAPREG_START)/4)


/*
 * Cardbus CIS pointer (PCI rev. 2.1)
 */
#define PCI_CARDBUS_CIS_REG 0x28

/*
 * Subsystem identification register; contains a vendor ID and a device ID.
 * Types/macros for PCI_ID_REG apply.
 * (PCI rev. 2.1)
 */
#define PCI_SUBSYS_ID_REG 0x2c

/*
 * capabilities link list (PCI rev. 2.2)
 */
#define	PCI_CAPLISTPTR_REG		0x34	/* header type 0 */
#define	PCI_CARDBUS_CAPLISTPTR_REG	0x14	/* header type 2 */
#define	PCI_CAPLIST_PTR(cpr)	((cpr) & 0xff)
#define	PCI_CAPLIST_NEXT(cr)	(((cr) >> 8) & 0xff)
#define	PCI_CAPLIST_CAP(cr)	((cr) & 0xff)

#define	PCI_CAP_RESERVED0	0x00
#define	PCI_CAP_PWRMGMT		0x01
#define	PCI_CAP_AGP		0x02
#define	PCI_CAP_VPD		0x03
#define	PCI_CAP_SLOTID		0x04
#define	PCI_CAP_MSI		0x05
#define	PCI_CAP_CPCI_HOTSWAP	0x06
#define	PCI_CAP_PCIX		0x07
#define	PCI_CAP_LDT		0x08
#define	PCI_CAP_VENDSPEC	0x09
#define	PCI_CAP_DEBUGPORT	0x0a
#define	PCI_CAP_CPCI_RSRCCTL	0x0b
#define	PCI_CAP_HOTPLUG		0x0c
#define	PCI_CAP_AGP8		0x0e
#define	PCI_CAP_SECURE		0x0f
#define	PCI_CAP_PCIEXPRESS     	0x10
#define	PCI_CAP_MSIX		0x11

/*
 * Vital Product Data; access via capability pointer (PCI rev 2.2).
 */
#define	PCI_VPD_ADDRESS_MASK	0x7fff
#define	PCI_VPD_ADDRESS_SHIFT	16
#define	PCI_VPD_ADDRESS(ofs)	\
	(((ofs) & PCI_VPD_ADDRESS_MASK) << PCI_VPD_ADDRESS_SHIFT)
#define	PCI_VPD_DATAREG(ofs)	((ofs) + 4)
#define	PCI_VPD_OPFLAG		0x80000000

/*
 * Power Management Capability; access via capability pointer.
 */

/* Power Management Capability Register */
#define PCI_PMCR		0x02
#define PCI_PMCR_D1SUPP		0x0200
#define PCI_PMCR_D2SUPP		0x0400
/* Power Management Control Status Register */
#define PCI_PMCSR		0x04
#define PCI_PMCSR_STATE_MASK	0x03
#define PCI_PMCSR_STATE_D0      0x00
#define PCI_PMCSR_STATE_D1      0x01
#define PCI_PMCSR_STATE_D2      0x02
#define PCI_PMCSR_STATE_D3      0x03

