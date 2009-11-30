/*
 * Driver for plb inteface of the xilinx temac 3.00a
 *
 * Author: Keith Robertson <kjrobert@alumni.uwaterloo.ca>
 * Copyright (c) 2007 Linn Products Ltd, Scotland.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 */

#ifndef _XILINX_TEMAC_
#define _XILINX_TEMAC_
#include <rtems/irq.h>


#define XILTEMAC_DRIVER_PREFIX   "xiltemac"

#define DRIVER_PREFIX XILTEMAC_DRIVER_PREFIX


/** IPIF interrupt and reset registers
 */
#define XTE_DISR_OFFSET  0x00000000  /**< Device interrupt status */
#define XTE_DIPR_OFFSET  0x00000004  /**< Device interrupt pending */
#define XTE_DIER_OFFSET  0x00000008  /**< Device interrupt enable */
#define XTE_DIIR_OFFSET  0x00000018  /**< Device interrupt ID */
#define XTE_DGIE_OFFSET  0x0000001C  /**< Device global interrupt enable */
#define XTE_IPISR_OFFSET 0x00000020  /**< IP interrupt status */
#define XTE_IPIER_OFFSET 0x00000028  /**< IP interrupt enable */
#define XTE_DSR_OFFSET   0x00000040  /**< Device software reset (write) */

/** IPIF transmit fifo
 */
#define XTE_PFIFO_TX_BASE_OFFSET    0x00002000  /**< Packet FIFO Tx channel */
#define XTE_PFIFO_TX_VACANCY_OFFSET 0x00002004  /**< Packet Fifo Tx Vacancy */
#define XTE_PFIFO_TX_DATA_OFFSET    0x00002100  /**< IPIF Tx packet fifo port */

/** IPIF receive fifo
 */
#define XTE_PFIFO_RX_BASE_OFFSET    0x00002010  /**< Packet FIFO Rx channel */
#define XTE_PFIFO_RX_VACANCY_OFFSET 0x00002014  /**< Packet Fifo Rx Vacancy */
#define XTE_PFIFO_RX_DATA_OFFSET    0x00002200  /**< IPIF Rx packet fifo port */

/** IPIF fifo masks
 */
#define XTE_PFIFO_COUNT_MASK     0x00FFFFFF

/** IPIF transmit and recieve DMA offsets
 */
#define XTE_DMA_SEND_OFFSET      0x00002300  /**< DMA Tx channel */
#define XTE_DMA_RECV_OFFSET      0x00002340  /**< DMA Rx channel */

/** IPIF IPIC_TO_TEMAC Core Registers
 */
#define XTE_CR_OFFSET           0x00001000  /**< Control */
#define XTE_TPLR_OFFSET         0x00001004  /**< Tx packet length (FIFO) */
#define XTE_TSR_OFFSET          0x00001008  /**< Tx status (FIFO) */
#define XTE_RPLR_OFFSET         0x0000100C  /**< Rx packet length (FIFO) */
#define XTE_RSR_OFFSET          0x00001010  /**< Receive status */
#define XTE_IFGP_OFFSET         0x00001014  /**< Interframe gap */
#define XTE_TPPR_OFFSET         0x00001018  /**< Tx pause packet */

/** TEMAC Core Registers
 * These are registers defined within the device's hard core located in the
 * processor block. They are accessed with the host interface. These registers
 * are addressed offset by XTE_HOST_IPIF_OFFSET or by the DCR base address
 * if so configured.
 */
#define XTE_HOST_IPIF_OFFSET    0x00003000  /**< Offset of host registers when
                                                 memory mapped into IPIF */
#define XTE_ERXC0_OFFSET        (XTE_HOST_IPIF_OFFSET + 0x00000200)  /**< Rx configuration word 0 */
#define XTE_ERXC1_OFFSET        (XTE_HOST_IPIF_OFFSET + 0x00000240)  /**< Rx configuration word 1 */
#define XTE_ETXC_OFFSET         (XTE_HOST_IPIF_OFFSET + 0x00000280)  /**< Tx configuration */
#define XTE_EFCC_OFFSET         (XTE_HOST_IPIF_OFFSET + 0x000002C0)  /**< Flow control configuration */
#define XTE_ECFG_OFFSET         (XTE_HOST_IPIF_OFFSET + 0x00000300)  /**< EMAC configuration */
#define XTE_EGMIC_OFFSET        (XTE_HOST_IPIF_OFFSET + 0x00000320)  /**< RGMII/SGMII configuration */
#define XTE_EMC_OFFSET          (XTE_HOST_IPIF_OFFSET + 0x00000340)  /**< Management configuration */
#define XTE_EUAW0_OFFSET        (XTE_HOST_IPIF_OFFSET + 0x00000380)  /**< Unicast address word 0 */
#define XTE_EUAW1_OFFSET        (XTE_HOST_IPIF_OFFSET + 0x00000384)  /**< Unicast address word 1 */
#define XTE_EMAW0_OFFSET        (XTE_HOST_IPIF_OFFSET + 0x00000388)  /**< Multicast address word 0 */
#define XTE_EMAW1_OFFSET        (XTE_HOST_IPIF_OFFSET + 0x0000038C)  /**< Multicast address word 1 */
#define XTE_EAFM_OFFSET         (XTE_HOST_IPIF_OFFSET + 0x00000390)  /**< Promisciuous mode */
#define XTE_EIRS_OFFSET         (XTE_HOST_IPIF_OFFSET + 0x000003A0)  /**< IRstatus */
#define XTE_EIREN_OFFSET        (XTE_HOST_IPIF_OFFSET + 0x000003A4)  /**< IRenable */
#define XTE_EMIID_OFFSET        (XTE_HOST_IPIF_OFFSET + 0x000003B0)  /**< MIIMwrData */
#define XTE_EMIIC_OFFSET        (XTE_HOST_IPIF_OFFSET + 0x000003B4)  /**< MiiMcnt */

/* Register masks. The following constants define bit locations of various
 * control bits in the registers. Constants are not defined for those registers
 * that have a single bit field representing all 32 bits. For further
 * information on the meaning of the various bit masks, refer to the HW spec.
 */

/** Interrupt status bits for top level interrupts
 *  These bits are associated with the XTE_DISR_OFFSET, XTE_DIPR_OFFSET,
 *  and XTE_DIER_OFFSET registers.
 */
#define XTE_DXR_SEND_FIFO_MASK          0x00000040 /**< Send FIFO channel */
#define XTE_DXR_RECV_FIFO_MASK          0x00000020 /**< Receive FIFO channel */
#define XTE_DXR_RECV_DMA_MASK           0x00000010 /**< Receive DMA channel */
#define XTE_DXR_SEND_DMA_MASK           0x00000008 /**< Send DMA channel */
#define XTE_DXR_CORE_MASK               0x00000004 /**< Core */
#define XTE_DXR_DPTO_MASK               0x00000002 /**< Data phase timeout */
#define XTE_DXR_TERR_MASK               0x00000001 /**< Transaction error */

/** Interrupt status bits for MAC interrupts
 *  These bits are associated with XTE_IPISR_OFFSET and XTE_IPIER_OFFSET
 *  registers.
 */
#define XTE_IPXR_XMIT_DONE_MASK         0x00000001 /**< Tx complete */
#define XTE_IPXR_RECV_DONE_MASK         0x00000002 /**< Rx complete */
#define XTE_IPXR_AUTO_NEG_MASK          0x00000004 /**< Auto negotiation complete */
#define XTE_IPXR_RECV_REJECT_MASK       0x00000008 /**< Rx packet rejected */
#define XTE_IPXR_XMIT_SFIFO_EMPTY_MASK  0x00000010 /**< Tx status fifo empty */
#define XTE_IPXR_RECV_LFIFO_EMPTY_MASK  0x00000020 /**< Rx length fifo empty */
#define XTE_IPXR_XMIT_LFIFO_FULL_MASK   0x00000040 /**< Tx length fifo full */
#define XTE_IPXR_RECV_LFIFO_OVER_MASK   0x00000080 /**< Rx length fifo overrun
                                                        Note that this signal is
                                                        no longer asserted by HW
                                                        */
#define XTE_IPXR_RECV_LFIFO_UNDER_MASK  0x00000100 /**< Rx length fifo underrun */
#define XTE_IPXR_XMIT_SFIFO_OVER_MASK   0x00000200 /**< Tx status fifo overrun */
#define XTE_IPXR_XMIT_SFIFO_UNDER_MASK  0x00000400 /**< Tx status fifo underrun */
#define XTE_IPXR_XMIT_LFIFO_OVER_MASK   0x00000800 /**< Tx length fifo overrun */
#define XTE_IPXR_XMIT_LFIFO_UNDER_MASK  0x00001000 /**< Tx length fifo underrun */
#define XTE_IPXR_RECV_PFIFO_ABORT_MASK  0x00002000 /**< Rx packet rejected due to
                                                        full packet FIFO */
#define XTE_IPXR_RECV_LFIFO_ABORT_MASK  0x00004000 /**< Rx packet rejected due to
                                                        full length FIFO */

#define XTE_IPXR_RECV_DROPPED_MASK                                      \
    (XTE_IPXR_RECV_REJECT_MASK |                                        \
     XTE_IPXR_RECV_PFIFO_ABORT_MASK |                                   \
     XTE_IPXR_RECV_LFIFO_ABORT_MASK)    /**< IPXR bits that indicate a dropped
                                             receive frame */
#define XTE_IPXR_XMIT_ERROR_MASK                                        \
    (XTE_IPXR_XMIT_SFIFO_OVER_MASK |                                    \
     XTE_IPXR_XMIT_SFIFO_UNDER_MASK |                                   \
     XTE_IPXR_XMIT_LFIFO_OVER_MASK |                                    \
     XTE_IPXR_XMIT_LFIFO_UNDER_MASK)    /**< IPXR bits that indicate transmit
                                             errors */

#define XTE_IPXR_RECV_ERROR_MASK                                        \
    (XTE_IPXR_RECV_DROPPED_MASK |                                       \
     XTE_IPXR_RECV_LFIFO_UNDER_MASK)    /**< IPXR bits that indicate receive
                                             errors */

#define XTE_IPXR_FIFO_FATAL_ERROR_MASK          \
    (XTE_IPXR_XMIT_SFIFO_OVER_MASK |            \
     XTE_IPXR_XMIT_SFIFO_UNDER_MASK |           \
     XTE_IPXR_XMIT_LFIFO_OVER_MASK |            \
     XTE_IPXR_XMIT_LFIFO_UNDER_MASK |           \
     XTE_IPXR_RECV_LFIFO_UNDER_MASK)    /**< IPXR bits that indicate errors with
                                             one of the length or status FIFOs
                                             that is fatal in nature. These bits
                                             can only be cleared by a device
                                             reset */

/** Software reset register (DSR)
 */
#define XTE_DSR_RESET_MASK      0x0000000A  /**< Write this value to DSR to
                                                 reset entire core */


/** Global interrupt enable register (DGIE)
 */
#define XTE_DGIE_ENABLE_MASK    0x80000000  /**< Write this value to DGIE to
                                                 enable interrupts from this
                                                 device */

/** Control Register (CR)
 */
#define XTE_CR_HTRST_MASK       0x00000008   /**< Reset hard temac */
#define XTE_CR_BCREJ_MASK       0x00000004   /**< Disable broadcast address
                                                  filtering */
#define XTE_CR_MCREJ_MASK       0x00000002   /**< Disable multicast address
                                                  filtering */
#define XTE_CR_HDUPLEX_MASK     0x00000001   /**< Enable half duplex operation */


/** Transmit Packet Length Register (TPLR)
 */
#define XTE_TPLR_TXPL_MASK      0x00003FFF   /**< Tx packet length in bytes */


/** Transmit Status Register (TSR)
 */
#define XTE_TSR_TXED_MASK       0x80000000   /**< Excess deferral error */
#define XTE_TSR_PFIFOU_MASK     0x40000000   /**< Packet FIFO underrun */
#define XTE_TSR_TXA_MASK        0x3E000000   /**< Transmission attempts */
#define XTE_TSR_TXLC_MASK       0x01000000   /**< Late collision error */
#define XTE_TSR_TPCF_MASK       0x00000001   /**< Transmit packet complete
                                                  flag */

#define XTE_TSR_ERROR_MASK                 \
    (XTE_TSR_TXED_MASK |                   \
     XTE_TSR_PFIFOU_MASK |                 \
     XTE_TSR_TXLC_MASK)                      /**< TSR bits that indicate an
                                                  error */


/** Receive Packet Length Register (RPLR)
 */
#define XTE_RPLR_RXPL_MASK      0x00003FFF   /**< Rx packet length in bytes */


/** Receive Status Register (RSR)
 */
#define XTE_RSR_RPCF_MASK       0x00000001   /**< Receive packet complete
                                                  flag */

/** Interframe Gap Register (IFG)
 */
#define XTE_IFG_IFGD_MASK       0x000000FF   /**< IFG delay */


/** Transmit Pause Packet Register (TPPR)
 */
#define XTE_TPPR_TPPD_MASK      0x0000FFFF   /**< Tx pause packet data */


/** Receiver Configuration Word 1 (ERXC1)
 */
#define XTE_ERXC1_RXRST_MASK    0x80000000   /**< Receiver reset */
#define XTE_ERXC1_RXJMBO_MASK   0x40000000   /**< Jumbo frame enable */
#define XTE_ERXC1_RXFCS_MASK    0x20000000   /**< FCS not stripped */
#define XTE_ERXC1_RXEN_MASK     0x10000000   /**< Receiver enable */
#define XTE_ERXC1_RXVLAN_MASK   0x08000000   /**< VLAN enable */
#define XTE_ERXC1_RXHD_MASK     0x04000000   /**< Half duplex */
#define XTE_ERXC1_RXLT_MASK     0x02000000   /**< Length/type check disable */
#define XTE_ERXC1_ERXC1_MASK    0x0000FFFF   /**< Pause frame source address
                                                  bits [47:32]. Bits [31:0]
                                                  are stored in register
                                                  ERXC0 */


/** Transmitter Configuration (ETXC)
 */
#define XTE_ETXC_TXRST_MASK     0x80000000   /**< Transmitter reset */
#define XTE_ETXC_TXJMBO_MASK    0x40000000   /**< Jumbo frame enable */
#define XTE_ETXC_TXFCS_MASK     0x20000000   /**< Generate FCS */
#define XTE_ETXC_TXEN_MASK      0x10000000   /**< Transmitter enable */
#define XTE_ETXC_TXVLAN_MASK    0x08000000   /**< VLAN enable */
#define XTE_ETXC_TXHD_MASK      0x04000000   /**< Half duplex */
#define XTE_ETXC_TXIFG_MASK     0x02000000   /**< IFG adjust enable */


/** Flow Control Configuration (EFCC)
 */
#define XTE_EFCC_TXFLO_MASK     0x40000000   /**< Tx flow control enable */
#define XTE_EFCC_RXFLO_MASK     0x20000000   /**< Rx flow control enable */


/** EMAC Configuration (ECFG)
 */
#define XTE_ECFG_LINKSPD_MASK   0xC0000000   /**< Link speed */
#define XTE_ECFG_RGMII_MASK     0x20000000   /**< RGMII mode enable */
#define XTE_ECFG_SGMII_MASK     0x10000000   /**< SGMII mode enable */
#define XTE_ECFG_1000BASEX_MASK 0x08000000   /**< 1000BaseX mode enable */
#define XTE_ECFG_HOSTEN_MASK    0x04000000   /**< Host interface enable */
#define XTE_ECFG_TX16BIT        0x02000000   /**< 16 bit Tx client enable */
#define XTE_ECFG_RX16BIT        0x01000000   /**< 16 bit Rx client enable */

#define XTE_ECFG_LINKSPD_10     0x00000000   /**< XTE_ECFG_LINKSPD_MASK for
                                                  10 Mbit */
#define XTE_ECFG_LINKSPD_100    0x40000000   /**< XTE_ECFG_LINKSPD_MASK for
                                                  100 Mbit */
#define XTE_ECFG_LINKSPD_1000   0x80000000   /**< XTE_ECFG_LINKSPD_MASK for
                                                  1000 Mbit */

/** EMAC RGMII/SGMII Configuration (EGMIC)
 */
#define XTE_EGMIC_RGLINKSPD_MASK    0xC0000000   /**< RGMII link speed */
#define XTE_EGMIC_SGLINKSPD_MASK    0x0000000C   /**< SGMII link speed */
#define XTE_EGMIC_RGSTATUS_MASK     0x00000002   /**< RGMII link status */
#define XTE_EGMIC_RGHALFDUPLEX_MASK 0x00000001   /**< RGMII half duplex */

#define XTE_EGMIC_RGLINKSPD_10      0x00000000   /**< XTE_EGMIC_RGLINKSPD_MASK
                                                      for 10 Mbit */
#define XTE_EGMIC_RGLINKSPD_100     0x40000000   /**< XTE_EGMIC_RGLINKSPD_MASK
                                                      for 100 Mbit */
#define XTE_EGMIC_RGLINKSPD_1000    0x80000000   /**< XTE_EGMIC_RGLINKSPD_MASK
                                                      for 1000 Mbit */
#define XTE_EGMIC_SGLINKSPD_10      0x00000000   /**< XTE_SGMIC_RGLINKSPD_MASK
                                                      for 10 Mbit */
#define XTE_EGMIC_SGLINKSPD_100     0x00000004   /**< XTE_SGMIC_RGLINKSPD_MASK
                                                      for 100 Mbit */
#define XTE_EGMIC_SGLINKSPD_1000    0x00000008   /**< XTE_SGMIC_RGLINKSPD_MASK
                                                      for 1000 Mbit */

/** EMAC Management Configuration (EMC)
 */
#define XTE_EMC_MDIO_MASK       0x00000040   /**< MII management enable */
#define XTE_EMC_CLK_DVD_MAX     0x3F         /**< Maximum MDIO divisor */


/** EMAC Unicast Address Register Word 1 (EUAW1)
 */
#define XTE_EUAW1_MASK          0x0000FFFF   /**< Station address bits [47:32]
                                                  Station address bits [31:0]
                                                  are stored in register
                                                  EUAW0 */


/** EMAC Multicast Address Register Word 1 (EMAW1)
 */
#define XTE_EMAW1_CAMRNW_MASK   0x00800000   /**< CAM read/write control */
#define XTE_EMAW1_CAMADDR_MASK  0x00030000   /**< CAM address mask */
#define XTE_EUAW1_MASK          0x0000FFFF   /**< Multicast address bits [47:32]
                                                  Multicast address bits [31:0]
                                                  are stored in register
                                                  EMAW0 */
#define XTE_EMAW1_CAMMADDR_SHIFT_MASK 16     /**< Number of bits to shift right
                                                  to align with
                                                  XTE_EMAW1_CAMADDR_MASK */


/** EMAC Address Filter Mode (EAFM)
 */
#define XTE_EAFM_EPPRM_MASK         0x80000000   /**< Promiscuous mode enable */


/** EMAC MII Management Write Data (EMIID)
 */
#define XTE_EMIID_MIIMWRDATA_MASK   0x0000FFFF   /**< Data port */


/** EMAC MII Management Control (EMIIC)
 */
#define XTE_EMIID_MIIMDECADDR_MASK  0x0000FFFF   /**< Address port */


struct XilTemacStats
{
  volatile uint32_t iInterrupts;

  volatile uint32_t iRxInterrupts;
  volatile uint32_t iRxRejectedInterrupts;
  volatile uint32_t iRxRejectedInvalidFrame;
  volatile uint32_t iRxRejectedDataFifoFull;
  volatile uint32_t iRxRejectedLengthFifoFull;
  volatile uint32_t iRxMaxDrained;
  volatile uint32_t iRxStrayEvents;

  volatile uint32_t iTxInterrupts;
  volatile uint32_t iTxMaxDrained;
};

#define MAX_UNIT_BYTES 50

struct XilTemac
{
  struct arpcom         iArpcom;
  struct XilTemacStats  iStats;
  struct ifnet*         iIfp;

  char                  iUnitName[MAX_UNIT_BYTES];

  uint32_t              iAddr;
  rtems_event_set       iIoEvent;

  int                   iIsrVector;

#if PPC_HAS_CLASSIC_EXCEPTIONS
  rtems_isr_entry       iOldHandler;
#else
  rtems_irq_connect_data iOldHandler;
#endif
  int                   iIsPresent;
};


#endif /* _XILINX_EMAC_*/
