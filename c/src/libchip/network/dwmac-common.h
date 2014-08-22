/**
 * @file
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems./license/LICENSE.
 */

#ifndef DWMAC_COMMON_H_
#define DWMAC_COMMON_H_

#define __INSIDE_RTEMS_BSD_TCPIP_STACK__ 1
#define __BSD_VISIBLE 1

#include <stdint.h>
#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/rtems_mii_ioctl.h>

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/mbuf.h>

#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>

#include <libchip/dwmac.h>
#include "dwmac-desc.h"
#include "dwmac-regs.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DWMAC_COMMON_DMA_MODE_STORE_AND_FORWARD 1 /* DMA STORE-AND-FORWARD Operation Mode */

/* Events */
/* Common task events */
#define DWMAC_COMMON_EVENT_TASK_INIT RTEMS_EVENT_1
#define DWMAC_COMMON_EVENT_TASK_STOP RTEMS_EVENT_2

/* Events for the transmit task */
#define DWMAC_COMMON_EVENT_TX_TRANSMIT_FRAME RTEMS_EVENT_3
#define DWMAC_COMMON_EVENT_TX_FRAME_TRANSMITTED RTEMS_EVENT_4
#define DWMAC_COMMON_EVENT_TX_BUMP_UP_DMA_THRESHOLD RTEMS_EVENT_5
#define DWMAC_COMMON_EVENT_TX_PHY_STATUS_CHANGE RTEMS_EVENT_6

/* Events for the receive task */
#define DWMAC_COMMON_EVENT_RX_FRAME_RECEIVED RTEMS_EVENT_3

#ifdef __ARM_ARCH_7A__
  #define DWMAC_COMMON_DSB() _ARM_Data_synchronization_barrier()
#else /*  __ARM_ARCH_7A__ */
  #define DWMAC_COMMON_DSB()
#endif /*  __ARM_ARCH_7A__ */

/* Foreward declarations */
typedef struct dwmac_common_core_ops dwmac_common_core_ops;
typedef struct dwmac_common_dma_ops dwmac_common_dma_ops;
typedef struct dwmac_common_desc_ops dwmac_common_desc_ops;

typedef enum { /* IPC status */
  DWMAC_COMMON_RX_FRAME_STATUS_GOOD,
  DWMAC_COMMON_RX_FRAME_STATUS_DISCARD,
  DWMAC_COMMON_RX_FRAME_STATUS_CSUM_NONE,
  DWMAC_COMMON_RX_FRAME_STATUS_LLC_SNAP
} dwmac_common_rx_frame_status;

typedef enum {
  DWMAC_COMMON_STATE_DOWN,
  DWMAC_COMMON_STATE_UP,

  DWMAC_COMMON_STATE_COUNT
} dwmac_common_state;

typedef struct {
  uint32_t link_down;
  uint32_t link_up;
} dwmac_common_phy_status_counts;

typedef struct {
  uint32_t receive;
  uint32_t transmit;
  uint32_t tx_underflow;
  uint32_t tx_jabber;
  uint32_t rx_overflow;
  uint32_t rx_early;
  uint32_t rx_buf_unav;
  uint32_t rx_process_stopped;
  uint32_t rx_watchdog;
  uint32_t tx_early;
  uint32_t tx_buf_unav;
  uint32_t tx_process_stopped;
  uint32_t fatal_bus_error;
  uint32_t unhandled;
} dwmac_common_dma_irq_counts;

typedef struct {
  uint32_t dest_addr_fail; /* When set, this bit indicates a frame that failed in the DA Filter in the MAC. */
  uint32_t crc_error; /* When set, this bit indicates that a CRC error occurred on the received frame. This field is valid only when the Last
                         Descriptor (RDES0[8]) is set. */
  uint32_t receive_error; /* When set, this bit indicates that the gmii_rxer_i signal is asserted while gmii_rxdv_i is asserted during frame
                             reception. Error can be of less or no extension, or error (rxd !=0xf) during extension. */
  uint32_t watchdog_timeout; /* When set, this bit indicates that the receive Watchdog Timer has expired while receiving the current frame and the
                                current frame is truncated after the Watchdog Timeout. */
  uint32_t late_collision; /* When set, this bit indicates that a late collision has occurred while receiving the frame in the half-duplex mode. */
  uint32_t giant_frame; /* When advanced timestamp feature is present, when set, this bit indicates that a snapshot of the Timestamp is
                           written in descriptor words 6 (RDES6) and 7 (RDES7). This is valid only when the Last Descriptor bit (RDES0[8]) is
                           set.
                           When IP Checksum Engine (Type 1) is selected, this bit, when set, indicates that the 16-bit IPv4 Header checksum
                           calculated by the EMAC did not match the received checksum bytes.
                           Otherwise, this bit, when set, indicates the Giant frame Status. Giant frames are larger than 1,518-byte (or
                           1,522-byte for VLAN or 2,000-byte when Bit 27 (2KPE) of MAC Configuration register is set) normal frames and
                           larger than 9,018-byte (9,022-byte for VLAN) frame when Jumbo frame processing is enabled. */
  uint32_t overflow_error; /* When set, this bit indicates that the received frame was damaged because of buffer overflow in MTL.
                              Note: This bit is set only when the DMA transfers a partial frame to the application. This happens only when the RX
                              FIFO buffer is operating in the threshold mode. In the store-and-forward mode, all partial frames are dropped
                              completely in RX FIFO buffer. */
  uint32_t descriptor_error; /* When set, this bit indicates a frame truncation caused by a frame that does not fit within the current descriptor
                                buffers, and that the DMA does not own the Next descriptor. The frame is truncated. This field is valid only when the
                                Last Descriptor (RDES0[8]) is set. */
  uint32_t source_addr_fail; /* When set, this bit indicates that the SA field of frame failed the SA Filter in the MAC. */
  uint32_t length_error; /* When set, this bit indicates that the actual length of the frame received and that the Length/ Type field does not
                            match. This bit is valid only when the Frame Type (RDES0[5]) bit is reset. */
  uint32_t vlan_tag; /* When set, this bit indicates that the frame to which this descriptor is pointing is a VLAN frame tagged by the MAC.
                        The VLAN tagging depends on checking the VLAN fields of received frame based on the Register 7 (VLAN Tag
                        Register) setting. */
  uint32_t ethernet_frames; /* When set, this bit indicates that the receive frame is an Ethernet-type frame (the LT field is greater than or equal to
                               0x0600). When this bit is reset, it indicates that the received frame is an IEEE802.3 frame. This bit is not valid for
                               Runt frames less than 14 bytes. */
  uint32_t dribble_bit_error; /* When set, this bit indicates that the received frame has a non-integer multiple of bytes (odd nibbles). This bit is valid
                                 only in the MII Mode. */
} dwmac_common_desc_status_counts_rx;

typedef struct {
  uint32_t jabber; /* When set, this bit indicates the MAC transmitter has experienced a jabber time-out. This bit is only set when Bit 22
                      (Jabber Disable) of Register 0 (MAC Configuration Register) is not set. */
  uint32_t frame_flushed; /* When set, this bit indicates that the DMA or MTL flushed the frame because of a software Flush command given by
                             the CPU. */
  uint32_t losscarrier; /* When set, this bit indicates that a loss of carrier occurred during frame transmission (that is, the gmii_crs_i
                           signal was inactive for one or more transmit clock periods during frame transmission). This is valid only for the
                           frames transmitted without collision when the MAC operates in the half-duplex mode. */
  uint32_t no_carrier; /* When set, this bit indicates that the Carrier Sense signal form the PHY was not asserted during transmission. */
  uint32_t excessive_collisions; /* When set, this bit indicates that the transmission was aborted after 16 successive collisions while attempting to
                                    transmit the current frame. If Bit 9 (Disable Retry) bit in the Register 0 (MAC Configuration Register) is set, this bit
                                    is set after the first collision, and the transmission of the frame is aborted. */
  uint32_t excessive_deferral; /* When set, this bit indicates that the transmission has ended because of excessive deferral of over 24,288 bit times
                                  (155,680 bits times in 1,000-Mbps mode or if Jumbo frame is enabled) if Bit 4 (Deferral Check) bit in Register 0
                                  (MAC Configuration Register) is set high. */
  uint32_t underflow; /* When set, this bit indicates that the MAC aborted the frame because the data arrived late from the Host memory.
                         Underflow Error indicates that the DMA encountered an empty transmit buffer while transmitting the frame. The
                         transmission process enters the Suspended state and sets both Transmit Underflow (Register 5[5]) and Transmit
                         Interrupt (Register 5[0]). */
  uint32_t ip_header_error; /* When set, this bit indicates that the MAC transmitter detected an error in the IP datagram header. The transmitter
                               checks the header length in the IPv4 packet against the number of header bytes received from the application and
                               indicates an error status if there is a mismatch. For IPv6 frames, a header error is reported if the main header length
                               is not 40 bytes. Furthermore, the Ethernet Length/Type field value for an IPv4 or IPv6 frame must match the IP
                               header version received with the packet. For IPv4 frames, an error status is also indicated if the Header Length field
                               has a value less than 0x5. */
  uint32_t payload_error; /* When set, this bit indicates that MAC transmitter detected an error in the TCP, UDP, or ICMP IP datagram payload.
                             The transmitter checks the payload length received in the IPv4 or IPv6 header against the actual number of TCP,
                             UDP, or ICMP packet bytes received from the application and issues an error status in case of a mismatch. */
  uint32_t deferred; /* When set, this bit indicates that the MAC defers before transmission because of the presence of carrier. This bit is
                        valid only in the half-duplex mode. */
  uint32_t vlan; /* When set, this bit indicates that the transmitted frame is a VLAN-type frame. */
} dwmac_common_desc_status_counts_tx;

typedef struct {
  uint32_t errors; /* Frames with errors */
  uint32_t dropped; /* Frames dropped */
  uint32_t frames_good; /* Frames passed to the network stack */
  uint32_t bytes_good; /* Sum of bytes passed to the network stack */
  uint32_t frames; /* Frames handled (good or bad) */
  uint32_t dma_suspended; /* The receive DMA was supended due to lack of descriptors */
} dwmac_common_rx_frame_counts;

typedef struct {
  uint32_t frames_from_stack; /* Frames received from the network stack fro tranmission */
  uint32_t frames_to_dma; /* Number of frames transmitted to DMA */
  uint32_t packets_to_dma; /* Number of packets transmitted to DMA*/
  uint32_t bytes_to_dma; /* Number of bytes transmitted */
  uint32_t packet_errors; /* Packets with errors */
  uint32_t packets_tranmitted_by_DMA; /* Packets tranmitted by the DMA */
} dwmac_common_tx_frame_counts;

typedef struct {
  dwmac_common_phy_status_counts phy_status_counts;
  dwmac_common_dma_irq_counts dma_irq_counts;
  dwmac_common_desc_status_counts_rx desc_status_counts_rx;
  dwmac_common_desc_status_counts_tx desc_status_counts_tx;
  dwmac_common_rx_frame_counts frame_counts_rx;
  dwmac_common_tx_frame_counts frame_counts_tx;
} dwmac_common_stats;

typedef struct {
  struct arpcom arpcom;
  struct rtems_bsdnet_ifconfig *bsd_config;
  struct rtems_mdio_info mdio;
  rtems_id task_id_rx;
  rtems_id task_id_tx;
  rtems_id task_id_control;
  void *arg;
  volatile macgrp *macgrp;
  volatile dmagrp *dmagrp;
  unsigned int csr_clock;
  dwmac_common_state state;
  dwmac_common_stats stats;
  unsigned int dma_threshold_control;
  volatile dwmac_desc *dma_tx;
  volatile dwmac_desc *dma_rx;
  unsigned int idx_rx;
  struct mbuf **mbuf_addr_rx;
  struct mbuf **mbuf_addr_tx;
  const dwmac_cfg *CFG;
  int MDIO_BUS_ADDR;
} dwmac_common_context;

struct dwmac_common_core_ops {
  /* MAC core initialization */
  void (*core_init) ( dwmac_common_context *self );

  /* Multicast filter setting */
  void (*set_hash_filter) (
    dwmac_common_context *self,
    const bool            add,
    struct ifreq         *ifr );

  /* Set/Get Unicast MAC addresses */
  void (*set_umac_addr) (
    dwmac_common_context *ioaddr,
    const unsigned char  *addr,
    const unsigned int    reg_n );
};

struct dwmac_common_dma_ops {
  /* DMA core initialization */
  int (*init) (
    dwmac_common_context *self,
    const uint32_t        pbl,
    const uint32_t        fb,
    const uint32_t        mb,
    const bool            use_enhanced_desc,
    const uint32_t        burst_len_4_support,
    const uint32_t        burst_len_8_support,
    const uint32_t        burst_len_16_support,
    const uint32_t        burst_boundary,
    volatile dwmac_desc  *dma_tx,
    volatile dwmac_desc  *dma_rx );

  /* Set tx/rx threshold in the csr6 register
   * An invalid value enables the store-and-forward mode */
  void (*dma_mode) (
    dwmac_common_context *self,
    const unsigned int    txmode,
    const unsigned int    rxmode );
};

struct dwmac_common_desc_ops {
  /* Verify that it is OK to use the selected descriptor operations */
  int (*validate) ( dwmac_common_context *self );
  bool (*use_enhanced_descs) ( dwmac_common_context *self );

  /* DMA RX descriptor ring allocation */
  int (*create_rx_desc) ( dwmac_common_context *self );

  /* DMA TX descriptor ring allocation */
  int (*create_tx_desc) ( dwmac_common_context *self );

  /* Free DMA RX descriptor ring */
  int (*destroy_rx_desc) ( dwmac_common_context *self );

  /* Free DMA TX descriptor ring */
  int (*destroy_tx_desc) ( dwmac_common_context *self );

  /* DMA RX descriptor initialization */
  void (*init_rx_desc) (
    dwmac_common_context *self,
    const unsigned int    index );

  /* DMA TX descriptor ring initialization */
  void (*init_tx_desc) ( dwmac_common_context *self );

  /* Free rx data buffers */
  void (*release_rx_bufs) ( dwmac_common_context *self );

  /* Allocate a data buffer */
  struct mbuf *(*alloc_data_buf)( dwmac_common_context *self );

  /* Free tx data buffers */
  void (*release_tx_bufs) ( dwmac_common_context *self );

  /* Invoked by the xmit function to prepare the tx descriptor */
  void (*prepare_tx_desc) (
    dwmac_common_context *self,
    const unsigned int    idx,
    const bool            is_first,
    const size_t          len,
    const void           *pdata );

  /* Set/get the owner of the descriptor */
  void (*release_tx_ownership) (
    dwmac_common_context *self,
    const unsigned int    idx_tx );
  bool (*am_i_tx_owner) (
    dwmac_common_context *self,
    const unsigned int    idx_tx );

  /* Invoked by the xmit function to close the tx descriptor */
  void (*close_tx_desc) (
    dwmac_common_context *self,
    const unsigned int    idx_tx );

  /* Clean the tx descriptor as soon as the tx irq is received */
  void (*release_tx_desc) (
    dwmac_common_context *self,
    const unsigned int    idx_tx );

  /* Last tx segment reports the transmit status */
  int (*get_tx_ls) (
    dwmac_common_context *self,
    const unsigned int    idx_tx );

  /* Return the transmit status looking at the TDES1 */
  int (*tx_status) (
    dwmac_common_context *self,
    const unsigned int    idx_tx );

  /* Handle extra events on specific interrupts hw dependent */
  bool (*am_i_rx_owner) (
    dwmac_common_context *self,
    const unsigned int    desc_idx );

  /* Get the receive frame size */
  size_t (*get_rx_frame_len) (
    dwmac_common_context *self,
    const unsigned int    desc_idx );

  /* Return the reception status looking at the RDES1 */
  dwmac_common_rx_frame_status (*rx_status) (
    dwmac_common_context *self,
    const unsigned int    desc_idx );
  bool (*is_first_rx_segment) (
    dwmac_common_context *self,
    const unsigned int    descriptor_index );
  bool (*is_last_rx_segment) (
    dwmac_common_context *self,
    const unsigned int    descriptor_index );
  void (*print_tx_desc) (
    volatile dwmac_desc *p,
    const unsigned int   count );
  void (*print_rx_desc) (
    volatile dwmac_desc *p,
    const unsigned int   count );
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DWMAC_COMMON_H_ */
