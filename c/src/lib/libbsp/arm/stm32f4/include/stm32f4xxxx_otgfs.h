/*
 * Copyright (c) 2013 Chris Nott.  All rights reserved.
 *
 *  Virtual Logic
 *  21-25 King St.
 *  Rockdale NSW 2216
 *  Australia
 *  <rtems@vl.com.au>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_STM32F4_STM32F4XXXX_OTGFS_H
#define LIBBSP_ARM_STM32F4_STM32F4XXXX_OTGFS_H

#include <bsp/utility.h>

#define USB_OTG_NUM_EPS 4
#define USB_OTG_MAX_TX_FIFOS 4

#define USB_FIFO_BASE 0x1000
#define USB_FIFO_OFFS 0x1000

struct stm32f4_otgfs_s {
  uint32_t gotgctl; // 0x00: Control and status register
#define STM32F4_OTGFS_GOTGCTL_BSVLD     BSP_BIT32(19) // B-session valid
#define STM32F4_OTGFS_GOTGCTL_ASVLD     BSP_BIT32(18) // A-session valid
#define STM32F4_OTGFS_GOTGCTL_DBCT      BSP_BIT32(17) // Debounce time
#define STM32F4_OTGFS_GOTGCTL_CIDSTS    BSP_BIT32(16) // Connector ID status
#define STM32F4_OTGFS_GOTGCTL_DHNPEN    BSP_BIT32(11) // Device HNP enable
#define STM32F4_OTGFS_GOTGCTL_HSHNPEN   BSP_BIT32(10) // Host set HNP enable
#define STM32F4_OTGFS_GOTGCTL_HNPRQ     BSP_BIT32(9)  // HNP request
#define STM32F4_OTGFS_GOTGCTL_HNGSCS    BSP_BIT32(8)  // Host negotiation status
#define STM32F4_OTGFS_GOTGCTL_SRQ       BSP_BIT32(1)  // Session request
#define STM32F4_OTGFS_GOTGCTL_SRQSCS    BSP_BIT32(0)  // Session request success

  uint32_t gotgint; // 0x04: Interrupt register
#define STM32F4_OTGFS_GOTGINT_DBCDNE    BSP_BIT32(19) // Debounce done
#define STM32F4_OTGFS_GOTGINT_ADTOCHG   BSP_BIT32(18) // A-device timeout change
#define STM32F4_OTGFS_GOTGINT_HNGDET    BSP_BIT32(17) // Host negotiation detected
#define STM32F4_OTGFS_GOTGINT_HNSSCHG   BSP_BIT32(9)  // Host negotiation success status change
#define STM32F4_OTGFS_GOTGINT_SRSSCHG   BSP_BIT32(8)  // Session request status change
#define STM32F4_OTGFS_GOTGINT_SEDET     BSP_BIT32(2)  // Session end detected

  uint32_t gahbcfg; // 0x08: AHB configuration register
#define STM32F4_OTGFS_GAHBCFG_PTXFELVL  BSP_BIT32(8)  // Periodic txfifo empty level
#define STM32F4_OTGFS_GAHBCFG_TXFELVL   BSP_BIT32(7)  // Txfifo empty level
#define STM32F4_OTGFS_GAHBCFG_GINTMSK   BSP_BIT32(0)  // Global interrupt mask

  uint32_t gusbcfg; // 0x0C: USB configuration register
#define STM32F4_OTGFS_GUSBCFG_CTXPKT    BSP_BIT32(31) // Corrupt TX packet
#define STM32F4_OTGFS_GUSBCFG_FDMOD     BSP_BIT32(30) // Force device mode
#define STM32F4_OTGFS_GUSBCFG_FHMOD     BSP_BIT32(29) // Force host mode
#define STM32F4_OTGFS_GUSBCFG_TRDT(val) BSP_FLD32(val, 10, 13)  // USB turnaround time
#define STM32F4_OTGFS_GUSBCFG_TRDT_GET(reg) BSP_FLD32GET(reg, 10, 13)
#define STM32F4_OTGFS_GUSBCFG_TRDT_SET(reg, val)  BSP_FLD32SET(reg, val, 10, 13)
#define STM32F4_OTGFS_GUSBCFG_HNPCAP    BSP_BIT32(9)  // HNP-capable
#define STM32F4_OTGFS_GUSBCFG_SRPCAP    BSP_BIT32(8)  // SRP-capable
#define STM32F4_OTGFS_GUSBCFG_PHYSEL    BSP_BIT32(6)  // Full speed serial transceiver select
#define STM32F4_OTGFS_GUSBCFG_TOCAL(val)  BSP_FLD32(val, 0, 2)  // FS timeout calibration
#define STM32F4_OTGFS_GUSBCFG_TOCAL_GET(reg)  BSP_FLD32GET(reg, 0, 2)
#define STM32F4_OTGFS_GUSBCFG_TOCAL_SET(reg, val) BSP_FLD32SET(reg, val, 0, 2)

  uint32_t grstctl; // 0x10: Reset register
#define STM32F4_OTGFS_GRSTCTL_AHBIDL    BSP_BIT32(31) // AHB master idle
#define STM32F4_OTGFS_GRSTCTL_TXFNUM(val) BSP_FLD32(val, 6, 10) // Tx fifo number
#define STM32F4_OTGFS_GRSTCTL_TXFNUM_GET(reg) BSP_FLD32GET(reg, 6, 10)
#define STM32F4_OTGFS_GRSTCTL_TXFNUM_SET(reg, val)  BSP_FLD32SET(reg, val, 6, 10)
#define STM32F4_OTGFS_GRSTCTL_TXFNUM_ALL  STM32F4_OTGFS_GRSTCTL_TXFNUM(0x10)
#define STM32F4_OTGFS_GRSTCTL_TXFFLSH   BSP_BIT32(5)  // TX fifo flush
#define STM32F4_OTGFS_GRSTCTL_RXFFLSH   BSP_BIT32(4)  // RX fifo flush
#define STM32F4_OTGFS_GRSTCTL_FCRST     BSP_BIT32(2)  // Host frame counter reset
#define STM32F4_OTGFS_GRSTCTL_HSRST     BSP_BIT32(1)  // HCLK soft reset
#define STM32F4_OTGFS_GRSTCTL_CSRST     BSP_BIT32(0)  // Core soft reset

  uint32_t gintsts; // 0x14: Core interrupt register
#define STM32F4_OTGFS_GINTSTS_WKUPINT   BSP_BIT32(31) // Resume / remote wakeup detected interrupt
#define STM32F4_OTGFS_GINTSTS_SRQINT    BSP_BIT32(30) // Session request / new session detected interrupt
#define STM32F4_OTGFS_GINTSTS_DISCINT   BSP_BIT32(29) // Disconnect detected interrupt
#define STM32F4_OTGFS_GINTSTS_CIDSCHG   BSP_BIT32(28) // Connector ID status change
#define STM32F4_OTGFS_GINTSTS_PTXFE     BSP_BIT32(26) // Periodic TX fifo empty
#define STM32F4_OTGFS_GINTSTS_HCINT     BSP_BIT32(25) // Host channels interrupt
#define STM32F4_OTGFS_GINTSTS_HPRTINT   BSP_BIT32(24) // Host port interrupt
#define STM32F4_OTGFS_GINTSTS_IPXFR     BSP_BIT32(21) // Incomplete periodic transfer
#define STM32F4_OTGFS_GINTSTS_IISOOXFR  BSP_BIT32(21) // Incomplete isochronous OUT transfer
#define STM32F4_OTGFS_GINTSTS_IISOIXFR  BSP_BIT32(20) // Incomplete isochronous IN transfer
#define STM32F4_OTGFS_GINTSTS_OEPINT    BSP_BIT32(19) // OUT endpoint interrupt
#define STM32F4_OTGFS_GINTSTS_IEPINT    BSP_BIT32(18) // IN endpoint interrupt
#define STM32F4_OTGFS_GINTSTS_EOPF      BSP_BIT32(15) // End of periodic frame interrupt
#define STM32F4_OTGFS_GINTSTS_ISOODRP   BSP_BIT32(14) // Isochronous OUT packet dropped interrupt
#define STM32F4_OTGFS_GINTSTS_ENUMDNE   BSP_BIT32(13) // Enumeration done
#define STM32F4_OTGFS_GINTSTS_USBRST    BSP_BIT32(12) // USB reset
#define STM32F4_OTGFS_GINTSTS_USBSUSP   BSP_BIT32(11) // USB suspend
#define STM32F4_OTGFS_GINTSTS_ESUSP     BSP_BIT32(10) // Early suspend
#define STM32F4_OTGFS_GINTSTS_GONAKEFF  BSP_BIT32(7)  // Global OUT NAK effective
#define STM32F4_OTGFS_GINTSTS_GINAKEFF  BSP_BIT32(6)  // Global IN non-periodic NAK effective
#define STM32F4_OTGFS_GINTSTS_NPTXFE    BSP_BIT32(5)  // Non-periodic TX fifo empty
#define STM32F4_OTGFS_GINTSTS_RXFLVL    BSP_BIT32(4)  // RX fifo non-empty
#define STM32F4_OTGFS_GINTSTS_SOF       BSP_BIT32(3)  // Start of frame
#define STM32F4_OTGFS_GINTSTS_OTGINT    BSP_BIT32(2)  // OTG interrupt
#define STM32F4_OTGFS_GINTSTS_MMIS      BSP_BIT32(1)  // Mode mismatch interrupt
#define STM32F4_OTGFS_GINTSTS_CMOD      BSP_BIT32(0)  // Current mode of operation

  uint32_t gintmsk; // 0x18: Interrupt mask register

  uint32_t grxstsr; // 0x1C: Receive status debug read

  uint32_t grxstsp; // 0x20: OTG status read and pop
#define STM32F4_OTGFS_GRXSTSP_FRMNUM(val) BSP_FLD32(val, 21, 24)  // Frame number
#define STM32F4_OTGFS_GRXSTSP_FRMNUM_GET(reg) BSP_FLD32GET(reg, 21, 24)
#define STM32F4_OTGFS_GRXSTSP_FRMNUM_SET(reg, val)  BSP_FLD32SET(reg, val, 21, 24)
#define STM32F4_OTGFS_GRXSTSP_PKTSTS(val) BSP_FLD32(val, 17, 20)  // Packet status
#define STM32F4_OTGFS_GRXSTSP_PKTSTS_GET(reg) BSP_FLD32GET(reg, 17, 20)
#define STM32F4_OTGFS_GRXSTSP_PKTSTS_SET(reg, val)  BSP_FLD32SET(reg, val, 17, 20)
#define PKTSTS_IN_DATA  (0x2)
#define PKTSTS_IN_COMPLETE  (0x3)
#define PKTSTS_TOGGLE_ERR (0x5)
#define PKTSTS_HALTED (0x7)
#define PKTSTS_OUTNAK (0x1)
#define PKTSTS_OUT_DATA (0x2)
#define PKTSTS_OUT_COMPLETE (0x3)
#define PKTSTS_SETUP_COMPLETE (0x4)
#define PKTSTS_SETUP_DATA (0x6)
#define STM32F4_OTGFS_GRXSTSP_DPIG(val) BSP_FLD32(val, 15, 16)  // Data PID
#define STM32F4_OTGFS_GRXSTSP_DPID_GET(reg) BSP_FLD32GET(reg, 15, 16)
#define STM32F4_OTGFS_GRXSTSP_DPID_SET(reg, val)  BSP_FLD32SET(reg, val, 15, 16)
#define STM32F4_OTGFS_GRXSTSP_DPID_DATA0  STM32F4_OTGFS_GRXSTSP_PKTSTS(0x0)
#define STM32F4_OTGFS_GRXSTSP_DPID_DATA1  STM32F4_OTGFS_GRXSTSP_PKTSTS(0x1)
#define STM32F4_OTGFS_GRXSTSP_DPID_DATA2  STM32F4_OTGFS_GRXSTSP_PKTSTS(0x2)
#define STM32F4_OTGFS_GRXSTSP_DPID_MDATA0 STM32F4_OTGFS_GRXSTSP_PKTSTS(0x3)
#define STM32F4_OTGFS_GRXSTSP_BCNT(val) BSP_FLD32(val, 4, 14) // Byte count
#define STM32F4_OTGFS_GRXSTSP_BCNT_GET(reg) BSP_FLD32GET(reg, 4, 14)
#define STM32F4_OTGFS_GRXSTSP_BCNT_SET(reg, val)  BSP_FLD32SET(reg, val, 4, 14)
#define STM32F4_OTGFS_GRXSTSP_CHNUM(val)  BSP_FLD32(val, 0, 3)  // Channel number
#define STM32F4_OTGFS_GRXSTSP_CHNUM_GET(reg)  BSP_FLD32GET(reg, 0, 3)
#define STM32F4_OTGFS_GRXSTSP_CHNUM_SET(reg, val) BSP_FLD32SET(reg, val, 0, 3)
#define STM32F4_OTGFS_GRXSTSP_EPNUM(val)  BSP_FLD32(val, 0, 3)  // Endpoint number
#define STM32F4_OTGFS_GRXSTSP_EPNUM_GET(reg)  BSP_FLD32GET(reg, 0, 3)
#define STM32F4_OTGFS_GRXSTSP_EPNUM_SET(reg, val) BSP_FLD32SET(reg, val, 0, 3)

  uint32_t grxfsiz; // 0x24: Receive FIFO size register
#define STM32F4_OTGFS_GRXFSIZ_RXFD(val) BSP_FLD32(val, 0, 15)
#define STM32F4_OTGFS_GRXFSIZ_RXFD_GET(reg) BSP_FLD32GET(reg, 0, 15)
#define STM32F4_OTGFS_GRXFSIZ_RXFD_SET(reg, val)  BSP_FLD32SET(reg, val, 0, 15)
#define STM32F4_OTGFS_GRXFSIZ_RXFD_MIN 16
#define STM32F4_OTGFS_GRXFSIZ_RXFD_MAX 256

  uint32_t dieptxf0; // 0x28: EP 0 transmit fifo size
#define STM32F4_OTGFS_DIEPTXF_DEPTH(val)  BSP_FLD32(val, 16, 31)
#define STM32F4_OTGFS_DIEPTXF_DEPTH_GET(reg)  BSP_FLD32GET(reg, 16, 31)
#define STM32F4_OTGFS_DIEPTXF_DEPTH_SET(reg, val) BSP_FLD32SET(reg, val, 16, 31)
#define STM32F4_OTGFS_DIEPTXF_DEPTH_MIN 16
#define STM32F4_OTGFS_DIEPTXF_DEPTH_MAX 256
#define STM32F4_OTGFS_DIEPTXF_SADDR(val)  BSP_FLD32(val, 0, 15)
#define STM32F4_OTGFS_DIEPTXF_SADDR_GET(reg)  BSP_FLD32GET(reg, 0, 15)
#define STM32F4_OTGFS_DIEPTXF_SADDR_SET(reg, val) BSP_FLD32SET(reg, val, 0, 15)

  uint32_t resv2C;

  uint32_t gi2cctl; // 0x30
  uint32_t resv34;  // 0x34

  uint32_t gccfg; // 0x38: General core configuration register
#define STM32F4_OTGFS_GCCFG_NOVBUSSENS  BSP_BIT32(21) // Vbus sensing disable
#define STM32F4_OTGFS_GCCFG_SOFOUTEN  BSP_BIT32(20) // SOF output enable
#define STM32F4_OTGFS_GCCFG_VBUSBSEN  BSP_BIT32(19) // Vbus sensing "B" device
#define STM32F4_OTGFS_GCCFG_VBUSASEN  BSP_BIT32(18) // Vbus sensing "A" device
#define STM32F4_OTGFS_GCCFG_PWRDWN    BSP_BIT32(16) // Power down

  uint32_t cid; // 0x3C: Product ID

  uint32_t resv40[48];  // 0x40 - 0x9C

  uint32_t hptxfsiz;  // 0x100

  uint32_t dieptxf[USB_OTG_MAX_TX_FIFOS]; // 0x104

} __attribute__ ((packed));
typedef struct stm32f4_otgfs_s stm32f4_otgfs;

struct stm32f4_otgfs_dregs_s {
  uint32_t dcfg;  // 0x800
#define STM32F4_OTGFS_DCFG_PFIVL(val) BSP_FLD32(val, 11, 12)  // Periodic frame interval
#define STM32F4_OTGFS_DCFG_PFIVL_GET(reg) BSP_FLD32GET(reg, 11, 12)
#define STM32F4_OTGFS_DCFG_PFIVL_SET(reg, val)  BSP_FLD32SET(reg, val, 11, 12)
#define PFIVL_80 0
#define PFIVL_85 1
#define PFIVL_90 2
#define PFIVL_95 3
#define STM32F4_OTGFS_DCFG_DAD(val) BSP_FLD32(val, 4, 10) // Device address
#define STM32F4_OTGFS_DCFG_DAD_GET(reg) BSP_FLD32GET(reg, 4, 10)
#define STM32F4_OTGFS_DCFG_DAD_SET(reg, val)  BSP_FLD32SET(reg, val, 4, 10)
#define STM32F4_OTGFS_DCFG_NZLSOHSK BSP_BIT32(2)  // Non-zero-length status OUT handshake
#define STM32F4_OTGFS_DCFG_DSPD(val)  BSP_FLD32(val, 0, 1)  // Device speed
#define STM32F4_OTGFS_DCFG_DSPD_GET(reg)  BSP_FLD32GET(reg, 0, 1)
#define STM32F4_OTGFS_DCFG_DSPD_SET(reg, val) BSP_FLD32SET(reg, val, 0, 1)
#define STM32F4_OTGFS_DCFG_DSPD_FULL STM32F4_OTGFS_DCFG_DSPD(0x3)

  uint32_t dctl;  // 0x804
#define STM32F4_OTGFS_DCTL_POPRGDNE   BSP_BIT32(11) // Power-on programming done
#define STM32F4_OTGFS_DCTL_CGONAK     BSP_BIT32(10) // Clear global OUT NAK
#define STM32F4_OTGFS_DCTL_SGONAK     BSP_BIT32(9)  // Set global OUT NAK
#define STM32F4_OTGFS_DCTL_CGINAK     BSP_BIT32(8)  // Clear global IN NAK
#define STM32F4_OTGFS_DCTL_SGINAK     BSP_BIT32(7)  // Set global IN NAK
#define STM32F4_OTGFS_DCTL_TCTL(val)  BSP_FLD32(val, 4, 6)  // Test control
#define STM32F4_OTGFS_DCTL_TCTL_GET(reg)  BSP_FLD32GET(reg, 4, 6)
#define STM32F4_OTGFS_DCTL_TCTL_SET(reg, val) BSP_FLD32SET(reg, val, 4, 6)
#define STM32F4_OTGFS_DCTL_GONSTS     BSP_BIT32(3)  // Global OUT NAK status
#define STM32F4_OTGFS_DCTL_GINSTS     BSP_BIT32(2)  // Global IN NAK status
#define STM32F4_OTGFS_DCTL_SDIS       BSP_BIT32(1)  // Soft disconnect
#define STM32F4_OTGFS_DCTL_RWUSIG     BSP_BIT32(0)  // Remote wakeup signalling

  uint32_t dsts;  // 0x808
#define STM32F4_OTGFS_DSTS_FNSOF(val) BSP_FLD32(val, 8, 21) // Frame number of received SOF
#define STM32F4_OTGFS_DSTS_FNSOF_GET(reg) BSP_FLD32GET(reg, 8, 21)
#define STM32F4_OTGFS_DSTS_EERR       BSP_BIT32(3)  // Erratic error
#define STM32F4_OTGFS_DSTS_ENUMSPD(val) BSP_FLD32(val, 1, 2)  // Enumerated speed
#define STM32F4_OTGFS_DSTS_ENUMSPD_GET(reg) BSP_FLD32GET(reg, 1, 2)
#define STM32F4_OTGFS_DSTS_ENUMSPD_FULL STM32F4_OTGFS_DSTS_ENUMSPD(0x3)
#define STM32F4_OTGFS_DSTS_SUSPSTS    BSP_BIT32(0)  // Suspend status

  uint32_t unused4; // 0x80C

  uint32_t diepmsk; // 0x810

  uint32_t doepmsk; // 0x814

  uint32_t daint; // 0x818
#define STM32F4_OTGFS_DAINT_OEPINT15    BSP_BIT32(31) // OUT endpoint 15 interrupt
#define STM32F4_OTGFS_DAINT_OEPINT14    BSP_BIT32(30) // OUT endpoint 14 interrupt
#define STM32F4_OTGFS_DAINT_OEPINT13    BSP_BIT32(29) // OUT endpoint 13 interrupt
#define STM32F4_OTGFS_DAINT_OEPINT12    BSP_BIT32(28) // OUT endpoint 12 interrupt
#define STM32F4_OTGFS_DAINT_OEPINT11    BSP_BIT32(27) // OUT endpoint 11 interrupt
#define STM32F4_OTGFS_DAINT_OEPINT10    BSP_BIT32(26) // OUT endpoint 10 interrupt
#define STM32F4_OTGFS_DAINT_OEPINT9     BSP_BIT32(25) // OUT endpoint 9 interrupt
#define STM32F4_OTGFS_DAINT_OEPINT8     BSP_BIT32(24) // OUT endpoint 8 interrupt
#define STM32F4_OTGFS_DAINT_OEPINT7     BSP_BIT32(23) // OUT endpoint 7 interrupt
#define STM32F4_OTGFS_DAINT_OEPINT6     BSP_BIT32(22) // OUT endpoint 6 interrupt
#define STM32F4_OTGFS_DAINT_OEPINT5     BSP_BIT32(21) // OUT endpoint 5 interrupt
#define STM32F4_OTGFS_DAINT_OEPINT4     BSP_BIT32(20) // OUT endpoint 4 interrupt
#define STM32F4_OTGFS_DAINT_OEPINT3     BSP_BIT32(19) // OUT endpoint 3 interrupt
#define STM32F4_OTGFS_DAINT_OEPINT2     BSP_BIT32(18) // OUT endpoint 2 interrupt
#define STM32F4_OTGFS_DAINT_OEPINT1     BSP_BIT32(17) // OUT endpoint 1 interrupt
#define STM32F4_OTGFS_DAINT_OEPINT0     BSP_BIT32(16) // OUT endpoint 0 interrupt
#define STM32F4_OTGFS_DAINT_IEPINT15    BSP_BIT32(15) // IN endpoint 15 interrupt
#define STM32F4_OTGFS_DAINT_IEPINT14    BSP_BIT32(14) // IN endpoint 14 interrupt
#define STM32F4_OTGFS_DAINT_IEPINT13    BSP_BIT32(13) // IN endpoint 13 interrupt
#define STM32F4_OTGFS_DAINT_IEPINT12    BSP_BIT32(12) // IN endpoint 12 interrupt
#define STM32F4_OTGFS_DAINT_IEPINT11    BSP_BIT32(11) // IN endpoint 11 interrupt
#define STM32F4_OTGFS_DAINT_IEPINT10    BSP_BIT32(10) // IN endpoint 10 interrupt
#define STM32F4_OTGFS_DAINT_IEPINT9     BSP_BIT32(9)  // IN endpoint 9 interrupt
#define STM32F4_OTGFS_DAINT_IEPINT8     BSP_BIT32(8)  // IN endpoint 8 interrupt
#define STM32F4_OTGFS_DAINT_IEPINT7     BSP_BIT32(7)  // IN endpoint 7 interrupt
#define STM32F4_OTGFS_DAINT_IEPINT6     BSP_BIT32(6)  // IN endpoint 6 interrupt
#define STM32F4_OTGFS_DAINT_IEPINT5     BSP_BIT32(5)  // IN endpoint 5 interrupt
#define STM32F4_OTGFS_DAINT_IEPINT4     BSP_BIT32(4)  // IN endpoint 4 interrupt
#define STM32F4_OTGFS_DAINT_IEPINT3     BSP_BIT32(3)  // IN endpoint 3 interrupt
#define STM32F4_OTGFS_DAINT_IEPINT2     BSP_BIT32(2)  // IN endpoint 2 interrupt
#define STM32F4_OTGFS_DAINT_IEPINT1     BSP_BIT32(1)  // IN endpoint 1 interrupt
#define STM32F4_OTGFS_DAINT_IEPINT0     BSP_BIT32(0)  // IN endpoint 0 interrupt

  uint32_t daintmsk;  // 0x81C
#define STM32F4_OTGFS_DAINTMSK_OEPM(val)  BSP_FLD32(val, 16, 31)  // OUT endpoint interrupt mask
#define STM32F4_OTGFS_DAINTMSK_OEPM_GET(reg)  BSP_FLD32GET(reg, 16, 31)
#define STM32F4_OTGFS_DAINTMSK_OEPM_SET(reg, val) BSP_FLD32SET(reg, val, 16, 31)
#define STM32F4_OTGFS_DAINTMSK_IEPM(val)  BSP_FLD32(val, 0, 15) // IN endpoint interrupt mask
#define STM32F4_OTGFS_DAINTMSK_IEPM_GET(reg)  BSP_FLD32GET(reg, 0, 15)
#define STM32F4_OTGFS_DAINTMSK_IEPM_SET(reg, val) BSP_FLD32SET(reg, val, 0, 15)

  uint32_t unused5[2];  // 0x820 - 0x824

  uint32_t dvbusdis;  // 0x828
#define STM32F4_OTGFS_DVBUSDIS_VBUSDT(val)  BSP_FLD32(val, 0, 15) // Device Vbus discharge time
#define STM32F4_OTGFS_DVBUSDIS_VBUSDT_GET(reg)  BSP_FLD32GET(reg, 0, 15)
#define STM32F4_OTGFS_DVBUSDIS_VBUSDT_SET(reg, val) BSP_FLD32SET(reg, val, 0, 15)

  uint32_t dvbuspulse;  // 0x82C
#define STM32F4_OTGFS_DVBUSPULSE_DVBUSP(val)  BSP_FLD32(val, 0, 15) // Device Vbus pulsing time
#define STM32F4_OTGFS_DVBUSPULSE_DVBUSP_GET(reg)  BSP_FLD32GET(reg, 0, 15)
#define STM32F4_OTGFS_DVBUSPULSE_DVBUSP_SET(reg, val) BSP_FLD32SET(reg, val, 0, 15)

  uint32_t unused6; // 0x830

  uint32_t diepempmsk;  // 0x834
#define STM32F4_OTGFS_DIEPEMPMSK_INEPTXFEM15  BSP_BIT32(15) // IN endpoint 15 TxFIFO empty interrupt mask
#define STM32F4_OTGFS_DIEPEMPMSK_INEPTXFEM14  BSP_BIT32(14) // IN endpoint 14 TxFIFO empty interrupt mask
#define STM32F4_OTGFS_DIEPEMPMSK_INEPTXFEM13  BSP_BIT32(13) // IN endpoint 13 TxFIFO empty interrupt mask
#define STM32F4_OTGFS_DIEPEMPMSK_INEPTXFEM12  BSP_BIT32(12) // IN endpoint 12 TxFIFO empty interrupt mask
#define STM32F4_OTGFS_DIEPEMPMSK_INEPTXFEM11  BSP_BIT32(11) // IN endpoint 11 TxFIFO empty interrupt mask
#define STM32F4_OTGFS_DIEPEMPMSK_INEPTXFEM10  BSP_BIT32(10) // IN endpoint 10 TxFIFO empty interrupt mask
#define STM32F4_OTGFS_DIEPEMPMSK_INEPTXFEM9   BSP_BIT32(9)  // IN endpoint 9 TxFIFO empty interrupt mask
#define STM32F4_OTGFS_DIEPEMPMSK_INEPTXFEM8   BSP_BIT32(8)  // IN endpoint 8 TxFIFO empty interrupt mask
#define STM32F4_OTGFS_DIEPEMPMSK_INEPTXFEM7   BSP_BIT32(7)  // IN endpoint 7 TxFIFO empty interrupt mask
#define STM32F4_OTGFS_DIEPEMPMSK_INEPTXFEM6   BSP_BIT32(6)  // IN endpoint 6 TxFIFO empty interrupt mask
#define STM32F4_OTGFS_DIEPEMPMSK_INEPTXFEM5   BSP_BIT32(5)  // IN endpoint 5 TxFIFO empty interrupt mask
#define STM32F4_OTGFS_DIEPEMPMSK_INEPTXFEM4   BSP_BIT32(4)  // IN endpoint 4 TxFIFO empty interrupt mask
#define STM32F4_OTGFS_DIEPEMPMSK_INEPTXFEM3   BSP_BIT32(3)  // IN endpoint 3 TxFIFO empty interrupt mask
#define STM32F4_OTGFS_DIEPEMPMSK_INEPTXFEM2   BSP_BIT32(2)  // IN endpoint 2 TxFIFO empty interrupt mask
#define STM32F4_OTGFS_DIEPEMPMSK_INEPTXFEM1   BSP_BIT32(1)  // IN endpoint 1 TxFIFO empty interrupt mask
#define STM32F4_OTGFS_DIEPEMPMSK_INEPTXFEM0   BSP_BIT32(0)  // IN endpoint 0 TxFIFO empty interrupt mask

} __attribute__ ((packed));
typedef struct stm32f4_otgfs_dregs_s stm32f4_otgfs_dregs;

struct stm32f4_otgfs_inepregs_s {
  uint32_t diepctl;   // 0x900
#define STM32F4_OTGFS_DIEPCTL_EPENA     BSP_BIT32(31) // Endpoint enable
#define STM32F4_OTGFS_DIEPCTL_EPDIS     BSP_BIT32(30) // Endpoint disable
#define STM32F4_OTGFS_DIEPCTL_SODDFRM   BSP_BIT32(29) // Set odd frame
#define STM32F4_OTGFS_DIEPCTL_SD0PID    BSP_BIT32(28) // Set DATA0 PID / Set even frame
#define STM32F4_OTGFS_DIEPCTL_SEVNFRM   BSP_BIT32(28) // Set DATA0 PID / Set even frame
#define STM32F4_OTGFS_DIEPCTL_SNAK      BSP_BIT32(27) // Set NAK
#define STM32F4_OTGFS_DIEPCTL_CNAK      BSP_BIT32(26) // Clear NAK
#define STM32F4_OTGFS_DIEPCTL_TXFNUM(val) BSP_FLD32(val, 22, 25)  // TxFIFO number
#define STM32F4_OTGFS_DIEPCTL_TXFNUM_GET(reg) BSP_FLD32GET(reg, 22, 25)
#define STM32F4_OTGFS_DIEPCTL_TXFNUM_SET(reg, val)  BSP_FLD32SET(reg, val, 22, 25)
#define STM32F4_OTGFS_DIEPCTL_STALL     BSP_BIT32(21) // Stall handshake
#define STM32F4_OTGFS_DIEPCTL_EPTYP(val)  BSP_FLD32(val, 18, 19)  // Endpoint type - 00 = Control, 01 = Isoch, 10 = Bulk, 11 = Interrupt
#define STM32F4_OTGFS_DIEPCTL_EPTYP_GET(reg)  BSP_FLD32GET(reg, 18, 19)
#define STM32F4_OTGFS_DIEPCTL_EPTYP_SET(reg, val) BSP_FLD32SET(reg, val, 18, 19)
#define EPTYPE_CTRL 0
#define EPTYPE_ISOC 1
#define EPTYPE_BULK 2
#define EPTYPE_INTR 3
#define STM32F4_OTGFS_DIEPCTL_NAKSTS    BSP_BIT32(17) // NAK status
#define STM32F4_OTGFS_DIEPCTL_EONUM_DPID  BSP_BIT32(16) // Data PID / Even/odd frame
#define STM32F4_OTGFS_DIEPCTL_USBAEP    BSP_BIT32(15) // USB active endpoint
#define STM32F4_OTGFS_DIEPCTL_EP0_MPSIZ(val)  BSP_FLD32(val, 0, 1)  // Maximum packet size (bytes)
#define STM32F4_OTGFS_DIEPCTL_EP0_MPSIZ_GET(reg)  BSP_FLD32GET(reg, 0, 1)
#define STM32F4_OTGFS_DIEPCTL_EP0_MPSIZ_SET(reg, val) BSP_FLD32SET(reg, val, 0, 1)
#define EP0_MPSIZ_8   3
#define EP0_MPSIZ_16  2
#define EP0_MPSIZ_32  1
#define EP0_MPSIZ_64  0
#define STM32F4_OTGFS_DIEPCTL_EP0_MPSIZ_8 STM32F4_OTGFS_DIEPCTL_MPSIZ(EP0_MPSIZ_8)
#define STM32F4_OTGFS_DIEPCTL_EP0_MPSIZ_16  STM32F4_OTGFS_DIEPCTL_MPSIZ(EP0_MPSIZ_16)
#define STM32F4_OTGFS_DIEPCTL_EP0_MPSIZ_32  STM32F4_OTGFS_DIEPCTL_MPSIZ(EP0_MPSIZ_32)
#define STM32F4_OTGFS_DIEPCTL_EP0_MPSIZ_64  STM32F4_OTGFS_DIEPCTL_MPSIZ(EP0_MPSIZ_64)
#define STM32F4_OTGFS_DIEPCTL_MPSIZ(val)  BSP_FLD32(val, 0, 10) // Maximum packet size (bytes)
#define STM32F4_OTGFS_DIEPCTL_MPSIZ_GET(reg)  BSP_FLD32GET(reg, 0, 10)
#define STM32F4_OTGFS_DIEPCTL_MPSIZ_SET(reg, val) BSP_FLD32SET(reg, val, 0, 10)

  uint32_t reserved_04;

  uint32_t diepint;   // 0x908
#define STM32F4_OTGFS_DIEPINT_TXFE      BSP_BIT32(7)  // Transmit FIFO empty
#define STM32F4_OTGFS_DIEPINT_INEPNE    BSP_BIT32(6)  // IN endpoint NAK effective
#define STM32F4_OTGFS_DIEPINT_ITTXFE    BSP_BIT32(4)  // IN token received, TxFIFO empty
#define STM32F4_OTGFS_DIEPINT_TOC       BSP_BIT32(3)  // Timeout condition
#define STM32F4_OTGFS_DIEPINT_EPDISD    BSP_BIT32(1)  // Endpoint disabled
#define STM32F4_OTGFS_DIEPINT_XFRC      BSP_BIT32(0)  // Transfer complete

  uint32_t reserved_0C;

  uint32_t dieptsiz;  // 0x910
#define STM32F4_OTGFS_DIEPTSIZ_EP0_PKTCNT(val)  BSP_FLD32(val, 19, 20)  // EP0 packet count
#define STM32F4_OTGFS_DIEPTSIZ_EP0_PKTCNT_GET(reg)  BSP_FLD32GET(reg, 19, 20)
#define STM32F4_OTGFS_DIEPTSIZ_EP0_PKTCNT_SET(reg, val) BSP_FLD32SET(reg, val, 19, 20)
#define STM32F4_OTGFS_DIEPTSIZ_EP0_XFRSIZ(val)  BSP_FLD32(val, 0, 6)  // EP0 transfer size
#define STM32F4_OTGFS_DIEPTSIZ_EP0_XFRSIZ_GET(reg)  BSP_FLD32GET(reg, 0, 6)
#define STM32F4_OTGFS_DIEPTSIZ_EP0_XFRSIZ_SET(reg, val) BSP_FLD32SET(reg, val, 0, 6)
#define STM32F4_OTGFS_DIEPTSIZ_MCNT(val)  BSP_FLD32(val, 29, 30)  // Multi count
#define STM32F4_OTGFS_DIEPTSIZ_MCNT_GET(reg)  BSP_FLD32GET(reg, 29, 30)
#define STM32F4_OTGFS_DIEPTSIZ_MCNT_SET(reg, val) BSP_FLD32SET(reg, val, 29, 30)
#define STM32F4_OTGFS_DIEPTSIZ_PKTCNT(val)  BSP_FLD32(val, 19, 28)  // Packet count
#define STM32F4_OTGFS_DIEPTSIZ_PKTCNT_GET(reg)  BSP_FLD32GET(reg, 19, 28)
#define STM32F4_OTGFS_DIEPTSIZ_PKTCNT_SET(reg, val) BSP_FLD32SET(reg, val, 19, 28)
#define STM32F4_OTGFS_DIEPTSIZ_XFRSIZ(val)  BSP_FLD32(val, 0, 18) // Transfer size
#define STM32F4_OTGFS_DIEPTSIZ_XFRSIZ_GET(reg)  BSP_FLD32GET(reg, 0, 18)
#define STM32F4_OTGFS_DIEPTSIZ_XFRSIZ_SET(reg, val) BSP_FLD32SET(reg, val, 0, 18)

  uint32_t reserved_14;

  uint32_t dtxfsts;   // 0x918
#define STM32F4_OTGFS_DTXFSTS_INEPTFSAV(val)  BSP_FLD32(val, 0, 15) // IN endpoint TxFIFO space available
#define STM32F4_OTGFS_DTXFSTS_INEPTFSAV_GET(reg)  BSP_FLD32(reg, 0, 15)
#define STM32F4_OTGFS_DTXFSTS_INEPTFSAV_SET(reg, val) BSP_FLD32SET(reg, val, 0, 15)

  uint32_t reserved_1C;

} __attribute__ ((packed));
typedef struct stm32f4_otgfs_inepregs_s stm32f4_otgfs_inepregs;

struct stm32f4_otgfs_outepregs_s {
  uint32_t doepctl; // 0xBx0: Endpoint control register
#define STM32F4_OTGFS_DOEPCTL_EPENA     BSP_BIT32(31) // Endpoint enable
#define STM32F4_OTGFS_DOEPCTL_EPDIS     BSP_BIT32(30) // Endpoint disable
#define STM32F4_OTGFS_DOEPCTL_SD1PID    BSP_BIT32(29) // Set DATA1 PID / Set odd frame
#define STM32F4_OTGFS_DOEPCTL_SD0PID    BSP_BIT32(28) // Set DATA0 PID / Set even frame
#define STM32F4_OTGFS_DOEPCTL_SNAK      BSP_BIT32(27) // Set NAK
#define STM32F4_OTGFS_DOEPCTL_CNAK      BSP_BIT32(26) // Clear NAK
#define STM32F4_OTGFS_DOEPCTL_STALL     BSP_BIT32(21) // Stall handshake
#define STM32F4_OTGFS_DOEPCTL_SNPM      BSP_BIT32(20) // Snoop mode
#define STM32F4_OTGFS_DOEPCTL_EPTYP(val)  BSP_FLD32(val, 18, 19)  // Endpoint type - 00 = Control, 01 = Isoch, 10 = Bulk, 11 = Interrupt
#define STM32F4_OTGFS_DOEPCTL_EPTYP_GET(reg)  BSP_FLD32GET(reg, 18, 19)
#define STM32F4_OTGFS_DOEPCTL_EPTYP_SET(reg, val) BSP_FLD32SET(reg, val, 18, 19)
#define STM32F4_OTGFS_DOEPCTL_NAKSTS    BSP_BIT32(17) // NAK status
#define STM32F4_OTGFS_DOEPCTL_EONUM_DPID  BSP_BIT32(16) // Data PID / Even/odd frame
#define STM32F4_OTGFS_DOEPCTL_USBAEP    BSP_BIT32(15) // USB active endpoint
#define STM32F4_OTGFS_DOEPCTL_MPSIZ(val)  BSP_FLD32(val, 0, 10) // Maximum packet size (bytes)
#define STM32F4_OTGFS_DOEPCTL_MPSIZ_GET(reg)  BSP_FLD32GET(reg, 0, 10)
#define STM32F4_OTGFS_DOEPCTL_MPSIZ_SET(reg, val) BSP_FLD32SET(reg, val, 0, 10)

  uint32_t resv04;

  uint32_t doepint; // 0xBx8: Endpoint interrupt register
#define STM32F4_OTGFS_DOEPINT_B2BSTUP   BSP_BIT32(6)  // Back-to-back SETUP packets received
#define STM32F4_OTGFS_DOEPINT_OTEPDIS   BSP_BIT32(4)  // OUT token received when endpoint disabled
#define STM32F4_OTGFS_DOEPINT_STUP      BSP_BIT32(3)  // SETUP phase done
#define STM32F4_OTGFS_DOEPINT_EPDISD    BSP_BIT32(1)  // Endpoint disabled interrupt
#define STM32F4_OTGFS_DOEPINT_XFRC      BSP_BIT32(0)  // Transfer complete

  uint32_t doeptsiz;  // 0xBy0
#define STM32F4_OTGFS_DOEPTSIZ_EP0_STUPCNT(val) BSP_FLD32(val, 29, 30)  // EP0 SETUP packet count
#define STM32F4_OTGFS_DOEPTSIZ_EP0_STUPCNT_GET(reg) BSP_FLD32GET(reg, 29, 30)
#define STM32F4_OTGFS_DOEPTSIZ_EP0_STUPCNT_SET(reg, val)  BSP_FLD32SET(reg, val, 29, 30)
#define STM32F4_OTGFS_DOEPTSIZ_EP0_PKTCNT   BSP_BIT32(19) // EP0 packet count
#define STM32F4_OTGFS_DOEPTSIZ_EP0_XFRSIZ(val)  BSP_FLD32(val, 0, 6)  // EP0 transfer size
#define STM32F4_OTGFS_DOEPTSIZ_EP0_XFRSIZ_GET(reg)  BSP_FLD32GET(reg, 0, 6)
#define STM32F4_OTGFS_DOEPTSIZ_EP0_XFRSIZ_SET(reg, val) BSP_FLD32SET(reg, val, 0, 6)
#define STM32F4_OTGFS_DOEPTSIZ_RXDPID(val)  BSP_FLD32(val, 29, 30)  // Received data PID
#define STM32F4_OTGFS_DOEPTSIZ_RXDPID_GET(reg)  BSP_FLD32GET(reg, 29, 30)
#define STM32F4_OTGFS_DOEPTSIZ_RXDPID_SET(reg, val) BSP_FLD32SET(reg, val, 29, 30)
#define STM32F4_OTGFS_DOEPTSIZ_PKTCNT(val)  BSP_FLD32(val, 19, 28)  // Packet count
#define STM32F4_OTGFS_DOEPTSIZ_PKTCNT_GET(reg)  BSP_FLD32GET(reg, 19, 28)
#define STM32F4_OTGFS_DOEPTSIZ_PKTCNT_SET(reg, val) BSP_FLD32SET(reg, val, 19, 28)
#define STM32F4_OTGFS_DOEPTSIZ_XFRSIZ(val)  BSP_FLD32(val, 0, 18) // Transfer size
#define STM32F4_OTGFS_DOEPTSIZ_XFRSIZ_GET(reg)  BSP_FLD32GET(reg, 0, 18)
#define STM32F4_OTGFS_DOEPTSIZ_XFRSIZ_SET(reg, val) BSP_FLD32SET(reg, val, 0, 18)

  uint32_t resv14[3];
} __attribute__ ((packed));
typedef struct stm32f4_otgfs_outepregs_s stm32f4_otgfs_outepregs;

struct stm32f4_otgfs_pwrctlregs_s {
  uint32_t pcgcctl;   // 0xE00: Power and clock gating control register
#define STM32F4_OTGFS_PCGCCTL_PHYSUSP   BSP_BIT32(4)  // PHY suspend
#define STM32F4_OTGFS_PCGCCTL_GATEHCLK  BSP_BIT32(1)  // Gate HCLK
#define STM32F4_OTGFS_PCGCCTL_STPPCLK   BSP_BIT32(0)  // Stop PHY clk
} __attribute__ ((packed));
typedef struct stm32f4_otgfs_pwrctlregs_s stm32f4_otgfs_pwrctlregs;

#endif /* LIBBSP_ARM_STM32F4_STM32F4XXXX_OTGFS_H */
