/*
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *
 * THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 * AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 * You are hereby granted permission to use, copy, modify, and distribute
 * this file, provided that this notice, plus the above copyright notice
 * and disclaimer, appears in all copies. Radstone Technology will provide
 * no support for this code.
 *
 */
#ifndef _PCI_H_
#define _PCI_H_

/*
 * PCI Configuration space definitions
 */

#define PCI_CONFIG_ADDR	0xcf8
#define PCI_CONFIG_DATA	0xcfc

#define PCI_MAX_DEVICES		16
#define PCI_MAX_FUNCTIONS	8

#define PCI_CONFIG_VENDOR_LOW		0x00
#define PCI_CONFIG_VENDOR_HIGH		0x01
#define PCI_CONFIG_DEVICE_LOW		0x02
#define PCI_CONFIG_DEVICE_HIGH		0x03
#define PCI_CONFIG_COMMAND		0x04
#define PCI_CONFIG_STATUS		0x06
#define PCI_CONFIG_REVISIONID		0x08
#define PCI_CONFIG_CLASS_CODE_L		0x09
#define PCI_CONFIG_CLASS_CODE_M		0x0a
#define PCI_CONFIG_CLASS_CODE_U		0x0b
#define PCI_CONFIG_CACHE_LINE_SIZE	0x0c
#define PCI_CONFIG_LATENCY_TIMER	0x0d
#define PCI_CONFIG_HEADER_TYPE		0x0e
#define PCI_CONFIG_BIST			0x0f
#define PCI_CONFIG_BAR_0		0x10
#define PCI_CONFIG_BAR_1		0x14
#define PCI_CONFIG_BAR_2		0x18
#define PCI_CONFIG_BAR_3		0x1c
#define PCI_CONFIG_BAR_4		0x20
#define PCI_CONFIG_BAR_5		0x24
#define PCI_CONFIG_SUBVENDOR_LOW	0x2c
#define PCI_CONFIG_SUBVENDOR_HIGH	0x2d
#define PCI_CONFIG_SUBDEVICE_LOW	0x2e
#define PCI_CONFIG_SUBDEVICE_HIGH	0x2f
#define PCI_CONFIG_ROM_BAR		0x30
#define PCI_CONFIG_INTERRUPTLINE	0x3c
#define PCI_CONFIG_INTERRUPTPIN		0x3d
#define PCI_CONFIG_MIN_GNT		0x3e
#define PCI_CONFIG_MAX_LAT		0x3f

/*
 * PCI Status register definitions
 */

#define PCI_STATUS_66MHZ_CAPABLE	0x0020
#define PCI_STATUS_UDF_SUPPORTED	0x0040
#define PCI_STATUS_FAST_BACK_TO_BACK	0x0080
#define PCI_STATUS_DET_DPAR_ERR		0x0100
#define PCI_STATUS_DEVSEL_MSK		0x0600
#define PCI_STATUS_DEVSEL_SLOW		0x0400
#define PCI_STATUS_DEVSEL_MED		0x0200
#define PCI_STATUS_DEVSEL_FAST		0x0000
#define PCI_STATUS_SIG_TARG_ABT		0x0800
#define PCI_STATUS_REC_TARG_ABT		0x1000
#define PCI_STATUS_REC_MAST_ABT		0x2000
#define PCI_STATUS_SIG_SYS_ERR		0x4000
#define PCI_STATUS_DET_PAR_ERR		0x8000

/*
 * PCI Enable register definitions
 */

#define PCI_ENABLE_IO_SPACE             0x0001
#define PCI_ENABLE_MEMORY_SPACE         0x0002
#define PCI_ENABLE_BUS_MASTER           0x0004
#define PCI_ENABLE_SPECIAL_CYCLES       0x0008
#define PCI_ENABLE_WRITE_AND_INVALIDATE 0x0010
#define PCI_ENABLE_VGA_COMPATIBLE_PALETTE 0x0020
#define PCI_ENABLE_PARITY               0x0040
#define PCI_ENABLE_WAIT_CYCLE           0x0080
#define PCI_ENABLE_SERR                 0x0100
#define PCI_ENABLE_FAST_BACK_TO_BACK    0x0200

/*
 * Bit encode for PCI_CONFIG_HEADER_TYPE register
 */

#define PCI_MULTI_FUNCTION	0x80

/*
 * Bit encodes for PCI Config BaseAddressesRegisters (BARs)
 */

#define PCI_ADDRESS_IO_SPACE                0x00000001
#define PCI_ADDRESS_MEMORY_TYPE_MASK        0x00000007
#define PCI_ADDRESS_MEMORY_PREFETCHABLE     0x00000008

#define PCI_TYPE_32BIT      0
#define PCI_TYPE_20BIT      2
#define PCI_TYPE_64BIT      4

/*
 * Bit encodes for PCI Config ROMBaseAddresses
 */

#define PCI_ROMADDRESS_ENABLED              0x00000001

/*
 * PCI Bridge Configuration space definitions
 */

#define PCI_BRIDGE_PRIMARY_BUS		0x18
#define PCI_BRIDGE_SECONDARY_BUS	0x19
#define PCI_BRIDGE_SUBORDINATE_BUS	0x1a
#define PCI_BRIDGE_SECONDARY_LAT	0x1b
#define PCI_BRIDGE_IO_BASE		0x1c
#define PCI_BRIDGE_IO_LIMIT		0x1d
#define PCI_BRIDGE_SECONDARY_STATUS	0x1e
#define PCI_BRIDGE_MEMORY_BASE		0x20
#define PCI_BRIDGE_MEMORY_LIMIT		0x22
#define PCI_BRIDGE_PRE_MEMORY_BASE	0x24
#define PCI_BRIDGE_PRE_MEMORY_LIMIT	0x26
#define PCI_BRIDGE_PRE_BASE_U		0x28
#define PCI_BRIDGE_PRE_LIMIT_U		0x2c
#define PCI_BRIDGE_IO_BASE_U		0x30
#define PCI_BRIDGE_IO_LIMIT_U		0x32
#define PCI_BRIDGE_ROM_BAR		0x38
#define PCI_BRIDGE_CONTROL		0x3e

/*
 * PCI Bridge Control register definitions
 */

#define PCI_BRIDGE_PAR_ERR_RESPONSE	0x01
#define PCI_BRIDGE_S_SERR_L_FWD_EN	0x02
#define PCI_BRIDGE_ENABLE_ISA		0x04
#define PCI_BRIDGE_ENABLE_VGA		0x08
#define PCI_BRIDGE_MASTER_ABORT		0x20
#define PCI_BRIDGE_SECONDARY_RESET	0x40
#define PCI_BRIDGE_BACK_TO_BACK_EN	0x80

/*
 * PCI IO address forwarding capability
 */
#define PCI_BRIDGE_IO_CAPABILITY	0x0f
#define PCI_BRIDGE_IO_16BIT		0x00
#define PCI_BRIDGE_IO_32BIT		0x01

/*
 * Class codes
 */
#define PCI_BASE_CLASS_NULL			0x00
#define PCI_BASE_CLASS_STORAGE		0x01
#define PCI_BASE_CLASS_NETWORK		0x02
#define PCI_BASE_CLASS_DISPLAY		0x03
#define PCI_BASE_CLASS_MULTIMEDIA	0x04
#define PCI_BASE_CLASS_MEMORY		0x05
#define PCI_BASE_CLASS_BRIDGE		0x06
#define PCI_BASE_CLASS_COM_CTRL		0x07
#define PCI_BASE_CLASS_BASEPERIPH	0x08
#define PCI_BASE_CLASS_INPUTDEV		0x09
#define PCI_BASE_CLASS_DOCKING		0x0a
#define PCI_BASE_CLASS_PROC			0x0b
#define PCI_BASE_CLASS_SERBUSCTRL	0x0c
#define PCI_BASE_CLASS_UNDEFINED	0xff

#define PCI_SUB_CLASS_NULL_NVGA		0x00
#define PCI_IF_CLASS_DISPLAY_VGA		0x00
#define PCI_IF_CLASS_DISPLAY_VGA8514	0x01
#define PCI_SUB_CLASS_NULL_VGA		0x01

#define PCI_SUB_CLASS_STORAGE_SCSI		0x00
#define PCI_SUB_CLASS_STORAGE_IDE		0x01
#define PCI_SUB_CLASS_STORAGE_FLOPPY	0x02
#define PCI_SUB_CLASS_STORAGE_IPI		0x03
#define PCI_SUB_CLASS_STORAGE_RAID		0x04
#define PCI_SUB_CLASS_STORAGE_OTHER		0x80

#define PCI_SUB_CLASS_NETWORK_ETH		0x00
#define PCI_SUB_CLASS_NETWORK_TOKEN		0x01
#define PCI_SUB_CLASS_NETWORK_FDDI		0x02
#define PCI_SUB_CLASS_NETWORK_ATM		0x03
#define PCI_SUB_CLASS_NETWORK_OTHER		0x80

#define PCI_SUB_CLASS_DISPLAY_VGA		0x00
#define PCI_SUB_CLASS_DISPLAY_XGA		0x01
#define PCI_SUB_CLASS_DISPLAY_OTHER		0x80

#define PCI_SUB_CLASS_MULTIMEDIA_VIDEO	0x00
#define PCI_SUB_CLASS_MULTIMEDIA_AUDIO	0x01
#define PCI_SUB_CLASS_MULTIMEDIA_OTHER	0x80

#define PCI_SUB_CLASS_MEMORY_RAM		0x00
#define PCI_SUB_CLASS_MEMORY_FLASH		0x01
#define PCI_SUB_CLASS_MEMORY_OTHER		0x80

#define PCI_SUB_CLASS_BRIDGE_HOST		0x00
#define PCI_SUB_CLASS_BRIDGE_ISA		0x01
#define PCI_SUB_CLASS_BRIDGE_EISA		0x02
#define PCI_SUB_CLASS_BRIDGE_MC			0x03
#define PCI_SUB_CLASS_BRIDGE_PCI		0x04
#define PCI_SUB_CLASS_BRIDGE_PCMCIA		0x05
#define PCI_SUB_CLASS_BRIDGE_NUBUS		0x06
#define PCI_SUB_CLASS_BRIDGE_CARDBUS	0x07
#define PCI_SUB_CLASS_BRIDGE_OTHER		0x80

#define PCI_SUB_CLASS_COM_CTRL_SERIAL	0x00
#define PCI_IF_CLASS_COM_SER_XT			0x00
#define PCI_IF_CLASS_COM_SER_16450		0x01
#define PCI_IF_CLASS_COM_SER_16550		0x02
#define PCI_SUB_CLASS_COM_CTRL_PARALLEL	0x01
#define PCI_IF_CLASS_COM_PAR			0x00
#define PCI_IF_CLASS_COM_PAR_BI			0x01
#define PCI_IF_CLASS_COM_PAR_ECP		0x02
#define PCI_SUB_CLASS_COM_CTRL_OTHER	0x80

#define PCI_SUB_CLASS_BASEPERIPH_PIC		0x00
#define PCI_IF_CLASS_BASEPERIPH_PIC			0x00
#define PCI_IF_CLASS_BASEPERIPH_PIC_ISA		0x01
#define PCI_IF_CLASS_BASEPERIPH_PIC_EISA	0x02
#define PCI_SUB_CLASS_BASEPERIPH_DMA		0x01
#define PCI_IF_CLASS_BASEPERIPH_DMA			0x00
#define PCI_IF_CLASS_BASEPERIPH_DMA_ISA		0x01
#define PCI_IF_CLASS_BASEPERIPH_DMA_EISA	0x02
#define PCI_SUB_CLASS_BASEPERIPH_TIMER		0x02
#define PCI_IF_CLASS_BASEPERIPH_TIMER		0x00
#define PCI_IF_CLASS_BASEPERIPH_TIMER_ISA	0x01
#define PCI_IF_CLASS_BASEPERIPH_TIMER_EISA	0x02
#define PCI_SUB_CLASS_BASEPERIPH_RTC		0x03
#define PCI_IF_CLASS_BASEPERIPH_RTC			0x00
#define PCI_IF_CLASS_BASEPERIPH_RTC_ISA		0x01
#define PCI_SUB_CLASS_BASEPERIPH_OTHER		0x80

#define PCI_SUB_CLASS_INPUTDEV_KEYBOARD	0x00
#define PCI_SUB_CLASS_INPUTDEV_PEN		0x01
#define PCI_SUB_CLASS_INPUTDEV_MOUSE	0x02
#define PCI_SUB_CLASS_INPUTDEV_OTHER	0x80

#define PCI_SUB_CLASS_DOCKING_GENERIC	0x00
#define PCI_SUB_CLASS_DOCKING_OTHER		0x80

#define PCI_SUB_CLASS_PROC_386			0x00
#define PCI_SUB_CLASS_PROC_486			0x01
#define PCI_SUB_CLASS_PROC_PENTIUM		0x02
#define PCI_SUB_CLASS_PROC_ALPHA		0x10
#define PCI_SUB_CLASS_PROC_POWERPC		0x20
#define PCI_SUB_CLASS_PROC_COPROC		0x40

#define PCI_SUB_CLASS_SERBUSCTRL_FIREWIRE	0x00
#define PCI_SUB_CLASS_SERBUSCTRL_ACCESS		0x01
#define PCI_SUB_CLASS_SERBUSCTRL_SSA		0x02
#define PCI_SUB_CLASS_SERBUSCTRL_USB		0x03
#define PCI_SUB_CLASS_SERBUSCTRL_FIBRECHAN	0x04

#define PCI_INVALID_VENDORDEVICEID	0xffffffff
#define PCI_ID(v, d) ((d << 16) | v)

/*
 * PCI access functions
 */
extern rtems_status_code PCIConfigWrite8(
    unsigned8 ucBusNumber,
    unsigned8 ucSlotNumber,
    unsigned8 ucFunctionNumber,
    unsigned8 ucOffset,
    unsigned8 ucValue
);

extern rtems_status_code PCIConfigWrite16(
    unsigned8 ucBusNumber,
    unsigned8 ucSlotNumber,
    unsigned8 ucFunctionNumber,
    unsigned8 ucOffset,
    unsigned16 usValue
);

extern rtems_status_code PCIConfigWrite32(
    unsigned8 ucBusNumber,
    unsigned8 ucSlotNumber,
    unsigned8 ucFunctionNumber,
    unsigned8 ucOffset,
    unsigned32 ulValue
);

extern rtems_status_code PCIConfigRead8(
    unsigned8 ucBusNumber,
    unsigned8 ucSlotNumber,
    unsigned8 ucFunctionNumber,
    unsigned8 ucOffset,
    unsigned8 *pucValue
);

extern rtems_status_code PCIConfigRead16(
    unsigned8 ucBusNumber,
    unsigned8 ucSlotNumber,
    unsigned8 ucFunctionNumber,
    unsigned8 ucOffset,
    unsigned16 *pusValue
);

extern rtems_status_code PCIConfigRead32(
    unsigned8 ucBusNumber,
    unsigned8 ucSlotNumber,
    unsigned8 ucFunctionNumber,
    unsigned8 ucOffset,
    unsigned32 *pulValue
);

/*
 * Return the number of PCI busses in the system
 */
extern unsigned8 BusCountPCI();

#endif /* _PCI_H_ */
