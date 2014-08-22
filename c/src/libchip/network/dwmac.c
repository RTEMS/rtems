/**
 * @file
 *
 * @brief DWMAC 10/100/1000 Network Interface Controller
 *
 * DWMAC 10/100/1000 on-chip Ethernet controllers are a Synopsys IP Core.
 * This is the main mode for the network interface controller.
 */

/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <bsp.h>
#include <bsp/fatal.h>
#include <rtems/endian.h>
#include "dwmac-common.h"
#include "dwmac-core.h"

#ifdef BSP_FEATURE_IRQ_EXTENSION
#include <bsp/irq.h>
#endif

/* PHY events which can be combined to an event set.
 * The PHY can create an event for a corresponding status change */
const dwmac_phy_event PHY_EVENT_JABBER                = 0x80;
const dwmac_phy_event PHY_EVENT_RECEIVE_ERROR         = 0x40;
const dwmac_phy_event PHY_EVENT_PAGE_RECEIVE          = 0x20;
const dwmac_phy_event PHY_EVENT_PARALLEL_DETECT_FAULT = 0x10;
const dwmac_phy_event PHY_EVENT_LINK_PARTNER_ACK      = 0x08;
const dwmac_phy_event PHY_EVENT_LINK_DOWN             = 0x04;
const dwmac_phy_event PHY_EVENT_REMOTE_FAULT          = 0x02;
const dwmac_phy_event PHY_EVENT_LINK_UP               = 0x01;

/* Default values for the number of DMA descriptors and mbufs to be used */
#define DWMAC_CONFIG_RX_UNIT_COUNT_DEFAULT 64
#define DWMAC_CONFIG_RX_UNIT_COUNT_MAX INT_MAX
#define DWMAC_CONFIG_TX_UNIT_COUNT_DEFAULT 64
#define DWMAC_CONFIG_TX_UNIT_COUNT_MAX INT_MAX

/* Default values for the DMA configuration */
#define DWMAC_DMA_BUS_MODE_PBL_DEFAULT DWMAC_DMA_CFG_BUS_MODE_BURST_LENGTH_8
#define DWMAC_DMA_BUS_MODE_FB_DEFAULT \
  DWMAC_DMA_CFG_BUS_MODE_BURST_MODE_SINGLE_OR_INCR
#define DWMAC_DMA_BUS_MODE_MIXED_BURSTS_DEFAULT \
  DWMAC_DMA_CFG_BUS_MODE_BURST_NOT_MIXED
#define DWMAC_DMA_AXI_BURST_LENGTH_4_DEFAULT \
  DWMAC_DMA_CFG_AXI_BURST_LENGTH_4_NOT_SUPPORTED
#define DWMAC_DMA_AXI_BURST_LENGTH_8_DEFAULT \
  DWMAC_DMA_CFG_AXI_BURST_LENGTH_8_NOT_SUPPORTED
#define DWMAC_DMA_AXI_BURST_LENGTH_16_DEFAULT \
  DWMAC_DMA_CFG_AXI_BURST_LENGTH_16_NOT_SUPPORTED
#define DWMAC_DMA_AXI_BURST_BOUNDARY_DEFAULT \
  DWMAC_DMA_CFG_AXI_BURST_BOUNDARY_4_KB

/* CSR Frequency Access Defines*/
#define DWCGNAC3504_CSR_F_35M 35000000
#define DWCGNAC3504_CSR_F_60M 60000000
#define DWCGNAC3504_CSR_F_100M 100000000
#define DWCGNAC3504_CSR_F_150M 150000000
#define DWCGNAC3504_CSR_F_250M 250000000
#define DWCGNAC3504_CSR_F_300M 300000000

/* MDC Clock Selection define*/
#define DWCGNAC3504_CSR_60_100M 0x0 /* MDC = clk_scr_i/42 */
#define DWCGNAC3504_CSR_100_150M 0x1 /* MDC = clk_scr_i/62 */
#define DWCGNAC3504_CSR_20_35M 0x2 /* MDC = clk_scr_i/16 */
#define DWCGNAC3504_CSR_35_60M 0x3 /* MDC = clk_scr_i/26 */
#define DWCGNAC3504_CSR_150_250M 0x4 /* MDC = clk_scr_i/102 */
#define DWCGNAC3504_CSR_250_300M 0x5 /* MDC = clk_scr_i/122 */

#define DWMAC_ALIGN( value, num_bytes_to_align_to ) \
  ( ( ( value ) + ( ( typeof( value ) )( num_bytes_to_align_to ) - 1 ) ) \
    & ~( ( typeof( value ) )( num_bytes_to_align_to ) - 1 ) )

#undef DWMAC_DEBUG
#ifdef DWMAC_DEBUG
#define DWMAC_PRINT_DBG( fmt, args ... )  printk( fmt, ## args )
#else
#define DWMAC_PRINT_DBG( fmt, args ... )  do { } while ( 0 )
#endif

#define DWMAC_DMA_THRESHOLD_CONTROL_DEFAULT 64

#define DWMAC_GLOBAL_MBUF_CNT (rtems_bsdnet_config.mbuf_bytecount / sizeof(struct  mbuf))
#define DWMAG_GLOBAL_MCLUST_CNT (rtems_bsdnet_config.mbuf_cluster_bytecount / MCLBYTES)

static int dwmac_if_mdio_busy_wait( const volatile uint32_t *gmii_address )
{
  rtems_interval timeout = rtems_clock_get_ticks_per_second();
  rtems_interval i       = 0;


  while ( ( *gmii_address & MACGRP_GMII_ADDRESS_GMII_BUSY ) != 0
          && i < timeout ) {
    rtems_task_wake_after( 1 );
    ++i;
  }

  return i < timeout ? 0 : EBUSY;
}

static int dwmac_if_mdio_read(
  int       phy,
  void     *arg,
  unsigned  phy_reg,
  uint32_t *val )
{
  int                   eno         = 0;
  dwmac_common_context *self        = (dwmac_common_context *) arg;
  volatile uint32_t    *mii_address = &self->macgrp->gmii_address;
  volatile uint32_t    *mii_data    = &self->macgrp->gmii_data;
  uint32_t              reg_value   = 0;


  if ( phy == -1 ) {
    reg_value = MACGRP_GMII_ADDRESS_PHYSICAL_LAYER_ADDRESS_SET(
      reg_value,
      self->MDIO_BUS_ADDR
      );
  } else {
    reg_value = MACGRP_GMII_ADDRESS_PHYSICAL_LAYER_ADDRESS_SET(
      reg_value,
      phy
      );
  }

  reg_value = MACGRP_GMII_ADDRESS_GMII_REGISTER_SET(
    reg_value,
    phy_reg
    );
  reg_value &= ~MACGRP_GMII_ADDRESS_GMII_WRITE;

  /* For the 7111 GMAC, we must set BUSY bit  in the MII address register while
   * accessing the PHY registers.
   * Fortunately, it seems this has no drawback for the 7109 MAC. */
  reg_value |= MACGRP_GMII_ADDRESS_GMII_BUSY;
  reg_value  = MACGRP_GMII_ADDRESS_CSR_CLOCK_RANGE_SET(
    reg_value,
    self->csr_clock
    );

  eno = dwmac_if_mdio_busy_wait( mii_address );

  if ( eno == 0 ) {
    *mii_address = reg_value;

    if ( dwmac_if_mdio_busy_wait( mii_address ) ) {
      eno = EBUSY;
    } else {
      /* Read the value from the MII data register */
      *val = MACGRP_GMII_DATA_GMII_DATA_GET( *mii_data );
    }
  }

  return eno;
}

static int dwmac_if_mdio_write(
  int      phy,
  void    *arg,
  unsigned phy_reg,
  uint32_t val )
{
  int                   eno         = 0;
  dwmac_common_context *self        = (dwmac_common_context *) arg;
  volatile uint32_t    *mii_address = &self->macgrp->gmii_address;
  volatile uint32_t    *mii_data    = &self->macgrp->gmii_data;
  uint32_t              reg_value   = *mii_address;


  if ( phy == -1 ) {
    reg_value = MACGRP_GMII_ADDRESS_PHYSICAL_LAYER_ADDRESS_SET(
      reg_value,
      self->MDIO_BUS_ADDR
      );
  } else {
    reg_value = MACGRP_GMII_ADDRESS_PHYSICAL_LAYER_ADDRESS_SET(
      reg_value,
      phy
      );
  }

  reg_value = MACGRP_GMII_ADDRESS_GMII_REGISTER_SET(
    reg_value,
    phy_reg
    );
  reg_value |= MACGRP_GMII_ADDRESS_GMII_WRITE;
  reg_value |= MACGRP_GMII_ADDRESS_GMII_BUSY;
  reg_value  = MACGRP_GMII_ADDRESS_CSR_CLOCK_RANGE_SET(
    reg_value,
    self->csr_clock
    );

  /* Wait until any existing MII operation is complete */
  eno = dwmac_if_mdio_busy_wait( mii_address );

  if ( eno == 0 ) {
    /* Set the MII address register to write */
    *mii_data    = val;
    *mii_address = reg_value;

    /* Wait until any existing MII operation is complete */
    eno = dwmac_if_mdio_busy_wait( mii_address );
  }

  return eno;
}

static int dwmac_get_phy_info(
  dwmac_common_context *self,
  const uint8_t         mdio_bus_addr,
  uint32_t             *oui,
  uint8_t              *model,
  uint8_t              *revision )
{
  int      eno = 0;
  uint32_t read;


  eno = dwmac_if_mdio_read(
    mdio_bus_addr,
    self,
    2,
    &read );

  if ( eno == 0 ) {
    *oui = 0 | ( read << 6 );
    eno  = dwmac_if_mdio_read(
      mdio_bus_addr,
      self,
      3,
      &read );

    if ( eno == 0 ) {
      *oui     |= ( read & 0xFC00 ) >> 10;
      *model    = (uint8_t) ( ( read & 0x03F0 ) >> 4 );
      *revision = read & 0x000F;
    }
  }

  return eno;
}

static inline void dwmac_enable_irq_rx( dwmac_common_context *self )
{
  dwmac_core_enable_dma_irq_rx( self );
}

static inline void dwmac_enable_irq_tx_default( dwmac_common_context *self )
{
  dwmac_core_enable_dma_irq_tx_default( self );
}

static inline void dwmac_enable_irq_tx_transmitted( dwmac_common_context *self )
{
  dwmac_core_enable_dma_irq_tx_transmitted( self );
}

static inline void dwmac_disable_irq_rx( dwmac_common_context *self )
{
  dwmac_core_disable_dma_irq_rx( self );
}

static inline void dwmac_disable_irq_tx_all( dwmac_common_context *self )
{
  dwmac_core_disable_dma_irq_tx_all( self );
}

static inline void dwmac_disable_irq_tx_transmitted ( dwmac_common_context *self )
{
  dwmac_core_disable_dma_irq_tx_transmitted( self );
}

static void dwmac_control_request_complete( const dwmac_common_context *self )
{
  rtems_status_code sc = rtems_event_transient_send( self->task_id_control );

  assert( sc == RTEMS_SUCCESSFUL );
}

static int dwmac_control_request(
  dwmac_common_context *self,
  rtems_id              task,
  rtems_event_set       event )
{
  int               eno        = 0;
  rtems_status_code sc         = RTEMS_SUCCESSFUL;
  uint32_t          nest_count = 0;


  self->task_id_control = rtems_task_self();

  sc                    = rtems_bsdnet_event_send( task, event );

  eno                   = rtems_status_code_to_errno( sc );

  if ( eno == 0 ) {
    nest_count = rtems_bsdnet_semaphore_release_recursive();
    sc         = rtems_event_transient_receive( RTEMS_WAIT, RTEMS_NO_TIMEOUT );
    eno        = rtems_status_code_to_errno( sc );
    rtems_bsdnet_semaphore_obtain_recursive( nest_count );
    self->task_id_control = 0;
  }

  return eno;
}

static bool dwmac_if_media_status(
  dwmac_common_context *self,
  int                  *media,
  uint8_t               phy_address )
{
  struct ifnet *ifp = &self->arpcom.ac_if;


  *media = (int) IFM_MAKEWORD( 0, 0, 0, phy_address );

  return ( *ifp->if_ioctl )( ifp, SIOCGIFMEDIA, (caddr_t) media ) == 0;
}

#define DWMAC_PRINT_COUNTER( fmt, counter, args ... ) \
  if ( counter != 0 ) { \
    printf( fmt, counter, ## args ); \
  }

/* Print statistics. Get called via ioctl. */
static int dwmac_if_interface_stats( void *arg )
{
  int                   eno      = 0;
  dwmac_common_context *self     = arg;
  volatile macgrp      *macgrp   = self->macgrp;
  int                   media    = 0;
  bool                  media_ok = dwmac_if_media_status(
    self, &media, self->MDIO_BUS_ADDR );
  uint32_t              oui;
  uint8_t               model;
  uint8_t               revision;
  uint16_t              giant_frame_size = 1518;


  if ( ( MACGRP_MAC_CONFIGURATION_JE & macgrp->mac_configuration ) != 0 ) {
    /* Jumbo Frames are enabled */
    giant_frame_size = 9018;
  }

  if ( media_ok ) {
    rtems_ifmedia2str( media, NULL, 0 );
    printf( "\n" );
    eno = dwmac_get_phy_info(
      self,
      self->MDIO_BUS_ADDR,
      &oui,
      &model,
      &revision );

    if ( eno == 0 ) {
      printf( "PHY 0x%02x: OUI = 0x%04" PRIX32 ", Model = 0x%02" PRIX8 ", Rev = "
              "0x%02" PRIX8 "\n",
              self->MDIO_BUS_ADDR,
              oui,
              model,
              revision );
      printf( "PHY status counters:\n" );
      DWMAC_PRINT_COUNTER(
        "%" PRIu32 " link down\n",
        self->stats.phy_status_counts.link_down
        );
      DWMAC_PRINT_COUNTER(
        "%" PRIu32 " link up\n",
        self->stats.phy_status_counts.link_up
        );
    }
  } else {
    printf( "PHY %d communication error\n", self->MDIO_BUS_ADDR );
  }

  printf( "\nHardware counters:\n" );

  if ( ( self->dmagrp->hw_feature & DMAGRP_HW_FEATURE_MMCSEL ) != 0 ) {
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " bytes transmitted, exclusive of preamble and retried bytes, "
      "in good and bad frames\n",
      macgrp->txoctetcount_gb
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good and bad frames transmitted, exclusive of retried "
      "frames\n",
      macgrp->txframecount_gb
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good broadcast frames transmitted\n",
      macgrp->txbroadcastframes_g
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good multicast frames transmitted\n",
      macgrp->txmulticastframes_g
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good and bad frames transmitted with length 64 bytes, "
      "exclusive of preamble and retried frames\n",
      macgrp->tx64octets_gb
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good and bad frames transmitted with length between 65 and "
      "127 (inclusive) bytes, exclusive of preamble and retried frames\n",
      macgrp->tx65to127octets_gb
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good and bad frames transmitted with length between 128 and "
      "255 (inclusive) bytes, exclusive of preamble and retried frames\n",
      macgrp->tx128to255octets_gb
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good and bad frames transmitted with length between 256 and "
      "511 (inclusive) bytes, exclusive of preamble and retried frames\n",
      macgrp->tx256to511octets_gb
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good and bad frames transmitted with length between 512 and "
      "1,023 (inclusive) bytes, exclusive of preamble and retried frames\n",
      macgrp->tx512to1023octets_gb
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good and bad frames transmitted with length between 1,024 and"
      " maxsize (inclusive) bytes, exclusive of preamble and retried frames\n",
      macgrp->tx1024tomaxoctets_gb
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good and bad unicast frames transmitted\n",
      macgrp->txunicastframes_gb
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good and bad multicast frames transmitted\n",
      macgrp->txmulticastframes_gb
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good and bad broadcast frames transmitted\n",
      macgrp->txbroadcastframes_gb
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " frames aborted due to frame underflow error\n",
      macgrp->txunderflowerror
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " successfully transmitted frames after a single collision in "
      "Half-duplex mode\n",
      macgrp->txsinglecol_g
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " successfully transmitted frames after more than a single"
      " collision in Half-duplex mode\n",
      macgrp->txmulticol_g
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " successfully transmitted frames after a deferral in "
      "Halfduplex mode\n",
      macgrp->txdeferred
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " frames aborted due to late collision error\n",
      macgrp->txlatecol
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " frames aborted due to excessive (16) collision errors\n",
      macgrp->txexesscol
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " frames aborted due to carrier sense error (no carrier or loss"
      " of carrier)\n",
      macgrp->txcarriererr
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " bytes transmitted, exclusive of preamble, in good frames "
      "only\n",
      macgrp->txoctetcnt
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good frames transmitted\n",
      macgrp->txframecount_g
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " frames aborted due to excessive deferral error (deferred for"
      " more than two max-sized frame times)\n",
      macgrp->txexcessdef
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good PAUSE frames transmitted\n",
      macgrp->txpauseframes
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good VLAN frames transmitted, exclusive of retried frames\n",
      macgrp->txvlanframes_g
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good and bad frames received\n",
      macgrp->txoversize_g
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good and bad frames received\n",
      macgrp->rxframecount_gb
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " bytes received, exclusive of preamble, in good and bad "
      "frames\n",
      macgrp->rxoctetcount_gb
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " bytes received, exclusive of preamble, only in good frames\n",
      macgrp->rxoctetcount_g
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good broadcast frames received\n",
      macgrp->rxbroadcastframes_g
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good multicast frames received\n",
      macgrp->rxmulticastframes_g
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " frames received with CRC error\n",
      macgrp->rxcrcerror
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " frames received with alignment (dribble) error. Valid only"
      " in 10/100 mode\n",
      macgrp->rxalignmenterror
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " frames received with runt (<64 bytes and CRC error) error\n",
      macgrp->rxrunterror
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " giant frames received with length (including CRC) greater "
      "than %" PRIu16 " bytes (%" PRIu16 " bytes for VLAN tagged) and with CRC"
      " error\n",
      macgrp->rxjabbererror, giant_frame_size, giant_frame_size + 4
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " frames received with length less than 64 bytes, without any"
      " errors\n",
      macgrp->rxundersize_g
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " frames received with length greater than the maxsize (1,518"
      " or 1,522 for VLAN tagged frames), without errors\n",
      macgrp->rxoversize_g
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good and bad frames received with length 64 bytes, exclusive"
      " of preamble\n",
      macgrp->rx64octets_gb
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good and bad frames received with length between 65 and 127"
      " (inclusive) bytes, exclusive of preamble\n",
      macgrp->rx65to127octets_gb
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good and bad frames received with length between 128 and 255"
      " (inclusive) bytes, exclusive of preamble\n",
      macgrp->rx128to255octets_gb
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good and bad frames received with length between 256 and 511"
      " (inclusive) bytes, exclusive of preamble\n",
      macgrp->rx256to511octets_gb
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good and bad frames received with length between 512 and "
      "1,023 (inclusive) bytes, exclusive of preamble\n",
      macgrp->rx512to1023octets_gb
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good and bad frames received with length between 1,024 and"
      " maxsize (inclusive) bytes, exclusive of preamble and retried frames\n",
      macgrp->rx1024tomaxoctets_gb
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good unicast frames received\n",
      macgrp->rxunicastframes_g
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " frames received with length error (length type field not "
      "equal to frame size), for all frames with valid length field\n",
      macgrp->rxlengtherror
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " frames received with length field not equal to the valid "
      "frame size (greater than 1,500 but less than 1,536)\n",
      macgrp->rxoutofrangetype
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good and valid PAUSE frames received\n",
      macgrp->rxpauseframes
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " missed received frames due to FIFO overflow\n",
      macgrp->rxfifooverflow
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good and bad VLAN frames received\n",
      macgrp->rxvlanframes_gb
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " frames received with error due to watchdog timeout error "
      "(frames with a data load larger than 2,048 bytes)\n",
      macgrp->rxwatchdogerror
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " frames received with Receive error or Frame Extension error"
      " on the GMII or MII interface\n",
      macgrp->rxrcverror
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " received good control frames\n",
      macgrp->rxctrlframes_g
      );

    printf( "\n" );

    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good IPv4 datagrams received with the TCP, UDP, or ICMP "
      "payload\n",
      macgrp->rxipv4_gd_frms
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " IPv4 datagrams received with header (checksum, length, or "
      "version mismatch) errors\n",
      macgrp->rxipv4_hdrerr_frms
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " IPv4 datagram frames received that did not have a TCP, UDP, "
      "or ICMP payload processed by the Checksum engine\n",
      macgrp->rxipv4_nopay_frms
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good IPv4 datagrams with fragmentation\n",
      macgrp->rxipv4_frag_frms
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good IPv4 datagrams received that had a UDP payload with "
      "checksum disabled\n",
      macgrp->rxipv4_udsbl_frms
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good IPv6 datagrams received with TCP, UDP, or ICMP "
      "payloads\n",
      macgrp->rxipv6_gd_frms
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " IPv6 datagrams received with header errors (length or "
      "version mismatch)\n",
      macgrp->rxipv6_hdrerr_frms
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " IPv6 datagram frames received that did not have a TCP, UDP,"
      " or ICMP payload. This includes all IPv6 datagrams with fragmentation"
      " or security extension headers\n",
      macgrp->rxipv6_nopay_frms
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good IP datagrams with a good UDP payload. This counter is "
      "not updated when the counter is incremented\n",
      macgrp->rxudp_gd_frms
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good IP datagrams whose UDP payload has a checksum error\n",
      macgrp->rxudp_err_frms
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good IP datagrams with a good TCP payload\n",
      macgrp->rxtcp_gd_frms
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good IP datagrams whose TCP payload has a checksum error\n",
      macgrp->rxtcp_err_frms
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good IP datagrams with a good ICMP payload\n",
      macgrp->rxicmp_gd_frms
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " good IP datagrams whose ICMP payload has a checksum error\n",
      macgrp->rxicmp_err_frms
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " bytes received in good IPv4 datagrams encapsulating TCP, UDP,"
      " or ICMP data\n",
      macgrp->rxipv4_gd_octets
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " bytes received in IPv4 datagrams with header errors (checksum,"
      " length, version mismatch). The value in the Length field of IPv4 "
      "header is used to update this counter\n",
      macgrp->rxipv4_hdrerr_octets
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " bytes received in IPv4 datagrams that did not have a TCP, UDP"
      ", or ICMP payload. The value in the IPv4 headers Length field is used"
      " to update this counter\n",
      macgrp->rxipv4_nopay_octets
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " bytes received in fragmented IPv4 datagrams. The value in the"
      " IPv4 headers Length field is used to update this counter\n",
      macgrp->rxipv4_frag_octets
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " bytes received in a UDP segment that had the UDP checksum "
      "disabled. This counter does not count IP Header bytes\n",
      macgrp->rxipv4_udsbl_octets
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " bytes received in good IPv6 datagrams encapsulating TCP, UDP "
      "or ICMPv6 data\n",
      macgrp->rxipv6_gd_octets
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " bytes received in IPv6 datagrams with header errors (length, "
      "version mismatch). The value in the IPv6 headers Length field is used "
      "to update this counter\n",
      macgrp->rxipv6_hdrerr_octets
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " bytes received in IPv6 datagrams that did not have a TCP, UDP"
      ", or ICMP payload. The value in the IPv6 headers Length field is used "
      "to update this counter\n",
      macgrp->rxipv6_nopay_octets
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " bytes received in a good UDP segment. This counter does not "
      "count IP header bytes\n",
      macgrp->rxudp_gd_octets
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " bytes received in a UDP segment that had checksum errors\n",
      macgrp->rxudp_err_octets
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " bytes received in a good TCP segment\n",
      macgrp->rxtcp_gd_octets
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " bytes received in a TCP segment with checksum errors\n",
      macgrp->rxtcperroctets
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " bytes received in a good ICMP segment\n",
      macgrp->rxicmp_gd_octets
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " bytes received in an ICMP segment with checksum errors\n",
      macgrp->rxicmp_err_octets
      );
  }

  if ( eno == 0 ) {
    printf( "\nInterrupt counts:\n" );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " receive interrputs\n",
                         self->stats.dma_irq_counts.receive
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " transmit interrupts\n",
                         self->stats.dma_irq_counts.transmit
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " tx jabber interrupts\n",
                         self->stats.dma_irq_counts.tx_jabber
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " rx overflow interrupts\n",
                         self->stats.dma_irq_counts.rx_overflow
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " rx early interrupts\n",
                         self->stats.dma_irq_counts.rx_early
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " rx buffer unavailable interrupts\n",
                         self->stats.dma_irq_counts.rx_buf_unav
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " rx process stopped interrupts\n",
                         self->stats.dma_irq_counts.rx_process_stopped
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " rx watchdog interrupts\n",
                         self->stats.dma_irq_counts.rx_watchdog
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " tx early interrupts\n",
                         self->stats.dma_irq_counts.tx_early
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " tx buffer unavailable interrupts\n",
                         self->stats.dma_irq_counts.tx_buf_unav
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " tx process stopped interrupts\n",
                         self->stats.dma_irq_counts.tx_process_stopped
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " tx underflow interrupts\n",
                         self->stats.dma_irq_counts.tx_underflow
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " fatal bus error interrupts\n",
                         self->stats.dma_irq_counts.fatal_bus_error
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " unhandled interrupts\n",
                         self->stats.dma_irq_counts.unhandled
                         );

    printf( "\nRX DMA status counts:\n" );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " CRC errors\n",
                         self->stats.desc_status_counts_rx.crc_error
                         );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " late collision when receing in half duplex mode\n",
      self->stats.desc_status_counts_rx.late_collision
      );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " giant frame or timestamp or checksum error\n",
      self->stats.desc_status_counts_rx.giant_frame
      );

    if ( self->stats.desc_status_counts_rx.watchdog_timeout != 0
         || self->stats.desc_status_counts_rx.receive_error != 0 ) {
      printf( "  IP Header or IP Payload:\n" );
      DWMAC_PRINT_COUNTER( "%" PRIu32 " receive watchdog timeout\n",
                           self->stats.desc_status_counts_rx.watchdog_timeout
                           );
      DWMAC_PRINT_COUNTER( "%" PRIu32 " receive error\n",
                           self->stats.desc_status_counts_rx.receive_error
                           );
    }

    DWMAC_PRINT_COUNTER( "%" PRIu32 " buffer overflows in MTL\n",
                         self->stats.desc_status_counts_rx.overflow_error
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " descriptor buffers too small\n",
                         self->stats.desc_status_counts_rx.descriptor_error
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " length errors\n",
                         self->stats.desc_status_counts_rx.length_error
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " dribble bit errors\n",
                         self->stats.desc_status_counts_rx.dribble_bit_error
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " vlan tags\n",
                         self->stats.desc_status_counts_rx.vlan_tag
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " ethernet frames\n",
                         self->stats.desc_status_counts_rx.ethernet_frames
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " destination address filter failures\n",
                         self->stats.desc_status_counts_rx.dest_addr_fail
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " source addresss filter failures\n",
                         self->stats.desc_status_counts_rx.source_addr_fail
                         );

    printf( "\nTX DMA status counts:\n" );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " jabber time-outs\n",
                         self->stats.desc_status_counts_tx.jabber
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " frame flushes\n",
                         self->stats.desc_status_counts_tx.frame_flushed
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " losses of carrier\n",
                         self->stats.desc_status_counts_tx.losscarrier
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " no carriers\n",
                         self->stats.desc_status_counts_tx.no_carrier
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " excessive collisions\n",
                         self->stats.desc_status_counts_tx.excessive_collisions
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " excessive deferrals\n",
                         self->stats.desc_status_counts_tx.excessive_deferral
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " underfolw errors\n",
                         self->stats.desc_status_counts_tx.underflow
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " IP header error\n",
                         self->stats.desc_status_counts_tx.ip_header_error
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " payload error\n",
                         self->stats.desc_status_counts_rx.source_addr_fail
                         );
    DWMAC_PRINT_COUNTER(
      "%" PRIu32 " MAC defers before transmission because of the presence of carrier\n",
      self->stats.desc_status_counts_tx.deferred
      );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " VLAN frames\n",
                         self->stats.desc_status_counts_tx.vlan
                         );

    printf( "\nRX frame counts:\n" );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " frames with errors\n",
                         self->stats.frame_counts_rx.errors
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " frames dropped\n",
                         self->stats.frame_counts_rx.dropped
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " frames good\n",
                         self->stats.frame_counts_rx.frames_good
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " bytes in good frames\n",
                         self->stats.frame_counts_rx.bytes_good
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " frames good or bad\n",
                         self->stats.frame_counts_rx.frames
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " DMA suspended\n",
                         self->stats.frame_counts_rx.dma_suspended
                         );

    printf( "\nTX frame counts:\n" );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " frames received from network stack\n",
                         self->stats.frame_counts_tx.frames_from_stack
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " frames tranmitted to DMA\n",
                         self->stats.frame_counts_tx.frames_to_dma
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " packets tranmitted to DMA\n",
                         self->stats.frame_counts_tx.packets_to_dma
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " bytes tranmitted to DMA\n",
                         self->stats.frame_counts_tx.bytes_to_dma
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " packet buffers regained from DMA\n",
                         self->stats.frame_counts_tx.packets_tranmitted_by_DMA
                         );
    DWMAC_PRINT_COUNTER( "%" PRIu32 " packet errors\n",
                         self->stats.frame_counts_tx.packet_errors
                         );

    printf( "\n" );

#ifdef RTEMS_DEBUG
    {
      const dwmac_common_desc_ops *DESC_OPS =
        (const dwmac_common_desc_ops *) self->CFG->DESC_OPS->ops;

      ( DESC_OPS->print_rx_desc )(
        self->dma_rx,
        (unsigned int) self->bsd_config->xbuf_count
        );

      ( DESC_OPS->print_tx_desc )(
        self->dma_tx,
        (unsigned int) self->bsd_config->xbuf_count
        );
    }
#endif /* RTEMS_DEBUG */
  }

  return eno;
}

static inline unsigned int dwmac_increment(
  const unsigned int value,
  const unsigned int cycle )
{
  if ( value < cycle ) {
    return value + 1;
  } else {
    return 0;
  }
}

/* Receive task
 * It handles receiving frames */
static void dwmac_task_rx( void *arg )
{
  dwmac_common_context         *self           = arg;
  dwmac_common_rx_frame_counts *counts         = &self->stats.frame_counts_rx;
  const unsigned int            INDEX_MAX      =
    (unsigned int) self->bsd_config->rbuf_count - 1U;
  size_t                        frame_len_last = 0;
  const dwmac_common_desc_ops  *DESC_OPS       =
    (const dwmac_common_desc_ops *) self->CFG->DESC_OPS->ops;
  size_t                        segment_size;


  while ( true ) {
    rtems_event_set   events;
    rtems_status_code sc = rtems_bsdnet_event_receive(
      DWMAC_COMMON_EVENT_TASK_INIT
      | DWMAC_COMMON_EVENT_TASK_STOP
      | DWMAC_COMMON_EVENT_RX_FRAME_RECEIVED,
      RTEMS_EVENT_ANY | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &events
      );
    assert( sc == RTEMS_SUCCESSFUL );

    /* Stop the task */
    if ( ( events & DWMAC_COMMON_EVENT_TASK_STOP ) != 0 ) {
      dwmac_core_dma_stop_rx( self );
      dwmac_disable_irq_rx( self );

      /* Release all tx mbufs at the risk of data loss */
      ( DESC_OPS->release_rx_bufs )( self );

      dwmac_control_request_complete( self );

      /* Return to events reception without re-enabling the interrupts
       * The task needs a re-initialization to to resume work */
      continue;
    }

    /* Ininitialize / Re-initialize reception handling */
    if ( ( events & DWMAC_COMMON_EVENT_TASK_INIT ) != 0 ) {
      unsigned int index;
      dwmac_core_dma_stop_rx( self );
      ( DESC_OPS->release_rx_bufs )( self );

      for ( index = 0; index <= INDEX_MAX; ++index ) {
        self->mbuf_addr_rx[index] = ( DESC_OPS->alloc_data_buf )( self );
        assert( self->mbuf_addr_rx[index] != NULL );
        ( DESC_OPS->init_rx_desc )( self, index );
      }

      self->idx_rx   = 0;
      frame_len_last = 0;

      /* Clear our interrupt statuses */
      dwmac_core_reset_dma_irq_status_rx( self );

      dwmac_core_dma_start_rx( self );

      dwmac_control_request_complete( self );
      events = events
               & (rtems_event_set) ( ~DWMAC_COMMON_EVENT_RX_FRAME_RECEIVED );
    }

    /* Handle the reception of one or more frames */
    if ( ( events & DWMAC_COMMON_EVENT_RX_FRAME_RECEIVED ) != 0 ) {
      unsigned int idx = self->idx_rx;

      /* Only handle frames for which we own the DMA descriptor */
      while ( ( DESC_OPS->am_i_rx_owner )( self, idx ) ) {
        struct mbuf                 *p_m_new;
        dwmac_common_rx_frame_status status;
        unsigned int                 idx_next = dwmac_increment(
          self->idx_rx, INDEX_MAX );

        /* Assign the next index (with wrap around) */
        self->idx_rx = idx_next;

        /* read the status of the incoming frame */
        status = ( DESC_OPS->rx_status )( self, idx );

        if ( status == DWMAC_COMMON_RX_FRAME_STATUS_DISCARD
             || status == DWMAC_COMMON_RX_FRAME_STATUS_CSUM_NONE ) {
          DWMAC_PRINT_DBG(
            "rx discarded: %02u\n",
            idx
            );
          ++counts->errors;

          /* simply re-initialize the DMA descriptor */
          ( DESC_OPS->init_rx_desc )( self, idx );
        } else if ( status == DWMAC_COMMON_RX_FRAME_STATUS_LLC_SNAP ) {
          DWMAC_PRINT_DBG(
            "rx dropped llc snap: %02u\n",
            idx
            );
          ++counts->dropped;

          /* simply re-initialize the DMA descriptor */
          ( DESC_OPS->init_rx_desc )( self, idx );
        } else {
          /* Before actually handling the valid frame, make sure we get a new
           * mbuf */
          p_m_new = ( DESC_OPS->alloc_data_buf )( self );

          if ( p_m_new != NULL ) {
            bool         is_first_seg;
            bool         is_last_seg;
            struct mbuf *p_m;
            uint32_t     frame_len = ( DESC_OPS->get_rx_frame_len )(
              self, idx );

            /* frame_len is the cummulated size of all segments of a frame.
             * But what we need is the size of the single segment */
            is_first_seg = ( DESC_OPS->is_first_rx_segment )( self, idx );
            is_last_seg  = ( DESC_OPS->is_last_rx_segment )( self, idx );

            if ( is_first_seg ) {
              segment_size = frame_len;
            } else {
              segment_size = frame_len - frame_len_last;
            }

            frame_len_last          = frame_len;

            p_m                     = self->mbuf_addr_rx[idx];
            self->mbuf_addr_rx[idx] = p_m_new;

            /* Initialize the DMA descriptor with the new mbuf */
            ( DESC_OPS->init_rx_desc )( self, idx );

            if ( p_m != NULL ) {
              /* Ethernet header */
              struct ether_header *eh = mtod( p_m, struct ether_header * );
              int                  sz = (int) segment_size;

              rtems_cache_invalidate_multiple_data_lines(
                eh, segment_size );

              /* Discard Ethernet header and CRC */
              if ( is_last_seg ) {
                sz -= ( ETHER_HDR_LEN + ETHER_CRC_LEN );
              } else {
                sz -= ETHER_HDR_LEN;
              }

              /* Update mbuf */
              p_m->m_len        = sz;
              p_m->m_pkthdr.len = sz;
              p_m->m_data       = mtod( p_m, char * ) + ETHER_HDR_LEN;

              DWMAC_COMMON_DSB();

              DWMAC_PRINT_DBG(
                "rx: %02u: %u %s%s\n",
                idx,
                segment_size,
                ( is_first_seg ) ? ( "F" ) : ( "" ),
                ( is_last_seg ) ? ( "L" ) : ( "" )
                );

              ether_input( &self->arpcom.ac_if, eh, p_m );

              ++counts->frames_good;
              counts->bytes_good += (uint32_t) p_m->m_len;
            } else {
              DWMAC_PRINT_DBG( "rx: %s: Inconsistent Rx descriptor chain\n",
                               self->arpcom.ac_if.if_name );
              ++counts->dropped;
            }
          } else {
            /* We have to discard the received frame because we did not get a new
             * mbuf to replace the frame's mbuf. */
            ++counts->dropped;

            /* simply re-initialize the DMA descriptor with the existing mbuf */
            ( DESC_OPS->init_rx_desc )( self, idx );
            DWMAC_PRINT_DBG(
              "rx dropped: %02u\n",
              idx
              );
          }
        }

        if ( ( self->dmagrp->status & DMAGRP_STATUS_RU ) != 0 ) {
          /* The receive DMA is suspended and needs to get restarted */
          dwmac_core_dma_restart_rx( self );
          DWMAC_PRINT_DBG(
            "rx DMA restart: %02u\n",
            idx
            );
          ++counts->dma_suspended;
          self->dmagrp->status = DMAGRP_STATUS_RU;
        }

        idx = idx_next;
        ++counts->frames;
      }

      /* Clear interrupt */
      self->dmagrp->status = DMAGRP_STATUS_RI;
    }

    /* Re-enable the IRQs */
    dwmac_enable_irq_rx( self );
  }
}

static struct mbuf *dwmac_next_fragment(
  struct ifnet *ifp,
  struct mbuf  *m,
  bool         *is_first,
  bool         *is_last,
  size_t       *size ) {
  struct mbuf *n  = NULL;
  int          sz = 0;


  while ( true ) {
    if ( m == NULL ) {
      /* Dequeue first fragment of the next frame */
      IF_DEQUEUE( &ifp->if_snd, m );

      /* Empty queue? */
      if ( m == NULL ) {
        return m;
      }

      /* The sum of all fragments lengths must fit into one
       * ethernet transfer unit */
      assert( ETHER_MAX_LEN >= m->m_pkthdr.len );
      *is_first = true;
    }

    /* Get fragment size */
    sz = m->m_len;

    if ( sz > 0 ) {
      /* Now we have a not empty fragment */
      break;
    } else {
      /* Discard empty fragments */
      m = m_free( m );
    }
  }

  /* Set fragment size */
  *size = (size_t) sz;

  /* Discard empty successive fragments */
  n = m->m_next;

  while ( n != NULL && n->m_len <= 0 ) {
    n = m_free( n );
  }

  m->m_next = n;

  /* Is our fragment the last in the frame? */
  if ( n == NULL ) {
    *is_last = true;
  } else {
    *is_last = false;
  }

  return m;
}

static int dwmac_update_autonegotiation_params( dwmac_common_context *self )
{
  int      eno      = 0;
  uint32_t value    = self->macgrp->mac_configuration;
  int      media    = 0;
  bool     media_ok = dwmac_if_media_status(
    self, &media, self->MDIO_BUS_ADDR );


  if ( media_ok ) {
    /* only ethernet supported, so far */
    if ( IFM_ETHER == IFM_TYPE( media ) ) {
      if ( IFM_NONE != IFM_SUBTYPE( media ) ) {
        if ( IFM_FDX & media ) {
          /* Enable duplex mode */
          value |= MACGRP_MAC_CONFIGURATION_DM;
        } else {
          /* Disable duplex mode */
          value &= ~MACGRP_MAC_CONFIGURATION_DM;
        }

        switch ( IFM_SUBTYPE( media ) ) {
          case IFM_10_T:

            /* Set RMII/RGMII speed to 10Mbps */
            value &= ~MACGRP_MAC_CONFIGURATION_FES;

            /* MII 10/100 Mbps */
            value |= MACGRP_MAC_CONFIGURATION_PS;
            break;
          case IFM_100_TX:

            /* Set RMII/RGMII speed to 100Mbps */
            value |= MACGRP_MAC_CONFIGURATION_FES;

            /* MII 10/100 Mbps */
            value |= MACGRP_MAC_CONFIGURATION_PS;
            break;
          case IFM_1000_T:

            /* RMII/RGMII speed irrelevant for 1000baseT */
            value &= ~MACGRP_MAC_CONFIGURATION_FES;

            /* GMII 1000 Mbps */
            value &= ~MACGRP_MAC_CONFIGURATION_PS;
            break;
          default:
            eno = ENOTSUP;
            break;
        }
      } else {
        eno = ENOTSUP;
      }
    } else {
      eno = ENOTSUP;
    }
  } else {
    eno = ENOTSUP;
  }

  if ( eno == 0 ) {
    self->macgrp->mac_configuration = value;
  }

  return eno;
}

/* Transmit task
 * It handles transmitting frames */
static void dwmac_task_tx( void *arg )
{
  dwmac_common_context        *self               = arg;
  unsigned int                 idx_transmit       = 0;
  unsigned int                 idx_transmit_first = 0;
  unsigned int                 idx_transmitted    = 0;
  unsigned int                 idx_release        = 0;
  struct mbuf                 *p_m                = NULL;
  bool                         is_first           = false;
  bool                         is_last            = false;
  size_t                       size               = 0;
  const unsigned int           INDEX_MAX          =
    (unsigned int) self->bsd_config->xbuf_count - 1U;
  const dwmac_common_dma_ops  *DMA_OPS            =
    (const dwmac_common_dma_ops *) self->CFG->MAC_OPS->dma;
  const dwmac_common_desc_ops *DESC_OPS           =
    (const dwmac_common_desc_ops *) self->CFG->DESC_OPS->ops;
  const dwmac_callback_cfg    *CALLBACK           = &self->CFG->CALLBACK;


  while ( true ) {
    rtems_event_set   events = 0;
    rtems_status_code sc     = rtems_bsdnet_event_receive(
      DWMAC_COMMON_EVENT_TASK_INIT
      | DWMAC_COMMON_EVENT_TASK_STOP
      | DWMAC_COMMON_EVENT_TX_TRANSMIT_FRAME
      | DWMAC_COMMON_EVENT_TX_FRAME_TRANSMITTED
      | DWMAC_COMMON_EVENT_TX_BUMP_UP_DMA_THRESHOLD
      | DWMAC_COMMON_EVENT_TX_PHY_STATUS_CHANGE,
      RTEMS_EVENT_ANY | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &events
      );
    assert( sc == RTEMS_SUCCESSFUL );

    while( events != 0 ) {
      /* Handle a status change of the ethernet PHY */
      if ( ( events & DWMAC_COMMON_EVENT_TX_PHY_STATUS_CHANGE ) != 0 ) {
        events &= ~DWMAC_COMMON_EVENT_TX_PHY_STATUS_CHANGE;
        dwmac_common_phy_status_counts *counts     =
          &self->stats.phy_status_counts;
        dwmac_phy_event                 phy_events = 0;
        int                             eno;

        /* Get tripped PHY events */
        eno = CALLBACK->phy_events_get(
          self->arg,
          &phy_events
          );

        if ( eno == 0 ) {
          /* Clear the PHY events */
          eno = CALLBACK->phy_event_clear( self->arg );
        }

        if ( eno == 0 ) {
          if ( ( phy_events & PHY_EVENT_LINK_DOWN ) != 0 ) {
            ++counts->link_down;
          }

          if ( ( phy_events & PHY_EVENT_LINK_UP ) != 0 ) {
            ++counts->link_up;

            /* A link up events means that we have a new connection.
            * Thus the autonegotiation paremeters must get updated */
            (void) dwmac_update_autonegotiation_params( self );
          }
        }

        assert( eno == 0 );
      }

      /* Stop the task */
      if ( ( events & DWMAC_COMMON_EVENT_TASK_STOP ) != 0 ) {
        dwmac_core_dma_stop_tx( self );
        dwmac_disable_irq_tx_all( self );

        /* Release all tx mbufs at the risk of data loss */
        ( DESC_OPS->release_tx_bufs )( self );

        dwmac_control_request_complete( self );

        /* Return to events reception without re-enabling the interrupts
        * The task needs a re-initialization to to resume work */
        events = 0;
        continue;
      }

      /* Ininitialize / Re-initialize transmission handling */
      if ( ( events & DWMAC_COMMON_EVENT_TASK_INIT ) != 0 ) {
        events &= ~DWMAC_COMMON_EVENT_TASK_INIT;
        (void) dwmac_update_autonegotiation_params( self );
        dwmac_core_dma_stop_tx( self );
        ( DESC_OPS->release_tx_bufs )( self );
        idx_transmit       = 0;
        idx_transmit_first = 0;
        idx_transmitted    = 0;
        idx_release        = 0;
        p_m                = NULL;
        is_first           = false;
        is_last            = false;
        size               = 0;
        ( DESC_OPS->init_tx_desc )( self );
        dwmac_core_dma_start_tx( self );
        dwmac_core_dma_restart_tx( self );

        /* Clear our interrupt statuses */
        dwmac_core_reset_dma_irq_status_tx( self );
        dwmac_enable_irq_tx_default( self );

        dwmac_control_request_complete( self );
      }

      /* Try to bump up the dma threshold due to a failure */
      if ( ( events & DWMAC_COMMON_EVENT_TX_BUMP_UP_DMA_THRESHOLD ) != 0 ) {
        events &= ~DWMAC_COMMON_EVENT_TX_BUMP_UP_DMA_THRESHOLD;
        if ( self->dma_threshold_control
            != DWMAC_COMMON_DMA_MODE_STORE_AND_FORWARD
            && self->dma_threshold_control <= 256 ) {
          self->dma_threshold_control += 64;
          ( DMA_OPS->dma_mode )(
            self,
            self->dma_threshold_control,
            DWMAC_COMMON_DMA_MODE_STORE_AND_FORWARD
            );
        }
      }

      /* Handle one or more transmitted frames */
      if ( ( events & DWMAC_COMMON_EVENT_TX_FRAME_TRANSMITTED ) != 0 ) {
        events &= ~DWMAC_COMMON_EVENT_TX_FRAME_TRANSMITTED;
        dwmac_common_tx_frame_counts *counts = &self->stats.frame_counts_tx;
        dwmac_disable_irq_tx_transmitted( self );

        /* Next index to be transmitted */
        unsigned int idx_transmitted_next = dwmac_increment(
          idx_transmitted, INDEX_MAX );

        /* Free consumed fragments */
        if( idx_release != idx_transmitted_next
            && ( DESC_OPS->am_i_tx_owner )( self, idx_release ) ) {
          while ( idx_release != idx_transmitted_next
                  && ( DESC_OPS->am_i_tx_owner )( self, idx_release ) ) {
            /* Status handling per packet */
            if ( ( DESC_OPS->get_tx_ls )( self, idx_release ) ) {
              int status = ( DESC_OPS->tx_status )(
                self, idx_release
                );

              if ( status == 0 ) {
                ++counts->packets_tranmitted_by_DMA;
              } else {
                ++counts->packet_errors;
              }
            }

            DWMAC_PRINT_DBG(
              "tx: release %u\n",
              idx_release
              );

            /* Release the DMA descriptor */
            ( DESC_OPS->release_tx_desc )( self, idx_release );

            /* Release mbuf */
            m_free( self->mbuf_addr_tx[idx_release] );
            self->mbuf_addr_tx[idx_release] = NULL;

            /* Next release index */
            idx_release = dwmac_increment(
              idx_release, INDEX_MAX );
          }
          if ( ( self->arpcom.ac_if.if_flags & IFF_OACTIVE ) != 0 ) {
            /* The last tranmission has been incomplete
            * (for example due to lack of DMA descriptors).
            * Continue it now! */
            events |= DWMAC_COMMON_EVENT_TX_TRANSMIT_FRAME;
          }
        } else {
          /* Clear transmit interrupt status */
          self->dmagrp->status = DMAGRP_STATUS_TI;
          /* Get re-activated by the next interrupt */
          dwmac_enable_irq_tx_transmitted( self );
        }
      }

      /* There are one or more frames to be transmitted. */
      if ( ( events & DWMAC_COMMON_EVENT_TX_TRANSMIT_FRAME ) != 0 ) {
        events &= ~DWMAC_COMMON_EVENT_TX_TRANSMIT_FRAME;
        dwmac_common_tx_frame_counts *counts = &self->stats.frame_counts_tx;

        if ( p_m != NULL ) {
          /* This frame will get re-counted */
          --counts->frames_from_stack;
        }

        while ( true ) {
          unsigned int idx = dwmac_increment(
            idx_transmit, INDEX_MAX );

          p_m = dwmac_next_fragment(
            &self->arpcom.ac_if,
            p_m,
            &is_first,
            &is_last,
            &size
            );

          /* New fragment? */
          if ( p_m != NULL ) {
            ++counts->frames_from_stack;

            /* Queue full? */
            if ( idx == idx_release ) {
              DWMAC_PRINT_DBG( "tx: full queue: 0x%08x\n", p_m );

              /* The queue is full, wait for transmitted interrupt */
              break;
            }

            /* Set the transfer data */
            rtems_cache_flush_multiple_data_lines(
              mtod( p_m, const void * ),
              size
              );

            ( DESC_OPS->prepare_tx_desc )(
              self,
              idx_transmit,
              is_first,
              size,
              mtod( p_m, const void * )
              );
            self->mbuf_addr_tx[idx_transmit] = p_m;

            ++counts->frames_to_dma;
            counts->bytes_to_dma += size;
            DWMAC_PRINT_DBG(
              "tx: %02" PRIu32 ": %u %s%s\n",
              idx_transmit, size,
              ( is_first != false ) ? ( "F" ) : ( "" ),
              ( is_last != false ) ? ( "L" ) : ( "" )

              );

            if ( is_first ) {
              idx_transmit_first = idx_transmit;
              is_first           = false;
            } else {
              /* To avoid race condition */
              ( DESC_OPS->release_tx_ownership )( self, idx_transmit );
            }

            if ( is_last ) {
              /* Interrupt on completition only for the latest fragment */
              ( DESC_OPS->close_tx_desc )( self, idx_transmit );

              /* To avoid race condition */
              ( DESC_OPS->release_tx_ownership )( self, idx_transmit_first );
              idx_transmitted = idx_transmit;

              if ( ( self->dmagrp->status & DMAGRP_STATUS_TU ) != 0 ) {
                /* Re-enable the tranmit DMA */
                dwmac_core_dma_restart_tx( self );
                DWMAC_PRINT_DBG(
                  "tx DMA restart: %02u\n",
                  idx_transmit_first
                  );
              }
            }

            /* Next transmit index */
            idx_transmit = idx;

            if ( is_last ) {
              ++counts->packets_to_dma;
            }

            /* Next fragment of the frame */
            p_m = p_m->m_next;
          } else {
            /* Nothing to transmit */
            break;
          }
        }

        /* No more packets and fragments? */
        if ( p_m == NULL ) {
          /* Interface is now inactive */
          self->arpcom.ac_if.if_flags &= ~IFF_OACTIVE;
        } else {
          /* There are more packets pending to be sent,
          * but we have run out of DMA descriptors.
          * We will continue sending once descriptors
          * have been freed due to a transmitted interupt */
          DWMAC_PRINT_DBG( "tx: transmission incomplete\n" );
          events |= DWMAC_COMMON_EVENT_TX_FRAME_TRANSMITTED;
        }

        /* TODO: Add handling */
      }

      DWMAC_PRINT_DBG( "tx: enable transmit interrupts\n" );
    }
  }
}

/* Major driver initialization method. Gets called
 * by the network stack automatically */
static void dwmac_if_interface_init( void *arg )
{
  (void) arg;
}

static void dwmac_if_set_promiscous_mode(
  dwmac_common_context *self,
  const bool            enabled )
{
  if ( enabled ) {
    /* Enable promiscuos mode */
    self->macgrp->mac_frame_filter |= MACGRP_MAC_FRAME_FILTER_PR;
  } else {
    /* Hash filter or perfect match */
    self->macgrp->mac_frame_filter &= ~MACGRP_MAC_FRAME_FILTER_PR;
    self->macgrp->mac_frame_filter &= ~MACGRP_MAC_FRAME_FILTER_HUC;
    self->macgrp->mac_frame_filter |= MACGRP_MAC_FRAME_FILTER_HPF;
  }
}

static int dwmac_create_dma_desc_rings( dwmac_common_context *self )
{
  int                          eno      = 0;
  const dwmac_common_desc_ops *DESC_OPS =
    (const dwmac_common_desc_ops *) self->CFG->DESC_OPS->ops;


  /* create and initialize the Rx/Tx descriptors */
  eno = ( DESC_OPS->create_rx_desc )( self );

  if ( eno == 0 ) {
    eno = ( DESC_OPS->create_tx_desc )( self );
  }

  return eno;
}

static int dwmac_destroy_dma_desc_rings( dwmac_common_context *self )
{
  int                          eno      = 0;
  const dwmac_common_desc_ops *DESC_OPS =
    (const dwmac_common_desc_ops *) self->CFG->DESC_OPS->ops;


  /* Free the Rx/Tx descriptors and the data buffers */
  eno = ( DESC_OPS->destroy_rx_desc )( self );

  if ( eno == 0 ) {
    eno = ( DESC_OPS->destroy_tx_desc )( self );
  }

  return eno;
}

static int dwmac_init_dma_engine( dwmac_common_context *self )
{
  uint32_t                     pbl                  =
    DWMAC_DMA_BUS_MODE_PBL_DEFAULT;
  uint32_t                     fixed_burst          =
    DWMAC_DMA_BUS_MODE_FB_DEFAULT;
  uint32_t                     mixed_burst          =
    DWMAC_DMA_BUS_MODE_MIXED_BURSTS_DEFAULT;
  uint32_t                     burst_len_4_support  =
    DWMAC_DMA_AXI_BURST_LENGTH_4_DEFAULT;
  uint32_t                     burst_len_8_support  =
    DWMAC_DMA_AXI_BURST_LENGTH_8_DEFAULT;
  uint32_t                     burst_len_16_support =
    DWMAC_DMA_AXI_BURST_LENGTH_16_DEFAULT;
  uint32_t                     burst_boundary       =
    DWMAC_DMA_AXI_BURST_BOUNDARY_DEFAULT;
  const dwmac_common_dma_ops  *DMA_OPS              =
    (const dwmac_common_dma_ops *) self->CFG->MAC_OPS->dma;
  const dwmac_common_desc_ops *DESC_OPS             =
    (const dwmac_common_desc_ops *) self->CFG->DESC_OPS->ops;


  /* The DMA configuration is optional */
  if ( self->CFG->DMA_CFG != NULL ) {
    const dwmac_dma_cfg *DMA_CFG = self->CFG->DMA_CFG;
    pbl                  = DMA_CFG->bus_mode_burst_length;
    fixed_burst          = DMA_CFG->bus_mode_burst_mode;
    mixed_burst          = DMA_CFG->bus_mode_burst_mixed;
    burst_len_4_support  = DMA_CFG->axi_burst_length_4_support;
    burst_len_8_support  = DMA_CFG->axi_burst_length_8_support;
    burst_len_16_support = DMA_CFG->axi_burst_length_16_support;
    burst_boundary       = DMA_CFG->axi_burst_boundary;
  }

  return ( DMA_OPS->init )(
    self,
    pbl,
    fixed_burst,
    mixed_burst,
    ( DESC_OPS->use_enhanced_descs )( self ),
    burst_len_4_support,
    burst_len_8_support,
    burst_len_16_support,
    burst_boundary,
    &self->dma_tx[0],
    &self->dma_rx[0]
    );
}

static void dwmac_dma_operation_mode( dwmac_common_context *self )
{
  const dwmac_common_dma_ops *DMA_OPS =
    (const dwmac_common_dma_ops *) self->CFG->MAC_OPS->dma;


  if ( ( self->dmagrp->hw_feature & DMAGRP_HW_FEATURE_TXOESEL ) != 0 ) {
    ( DMA_OPS->dma_mode )(
      self,
      DWMAC_COMMON_DMA_MODE_STORE_AND_FORWARD,
      DWMAC_COMMON_DMA_MODE_STORE_AND_FORWARD
      );
    self->dma_threshold_control = DWMAC_COMMON_DMA_MODE_STORE_AND_FORWARD;
  } else {
    ( DMA_OPS->dma_mode )(
      self,
      self->dma_threshold_control,
      DWMAC_COMMON_DMA_MODE_STORE_AND_FORWARD
      );
  }
}

static void dwmac_mmc_setup( dwmac_common_context *self )
{
  /* Mask MMC irq, counters are managed in HW and registers
   * are not cleared on each READ eventually. */

  /* No MAC Management Counters available */
  assert( ( self->dmagrp->hw_feature & DMAGRP_HW_FEATURE_MMCSEL ) != 0 );

  if ( ( self->dmagrp->hw_feature & DMAGRP_HW_FEATURE_MMCSEL ) != 0 ) {
    self->macgrp->mmc_control = MACGRP_MMC_CONTROL_CNTRST;
  }
}

static int dwmac_if_up_or_down(
  dwmac_common_context *self,
  const bool            up )
{
  int                          eno      = 0;
  rtems_status_code            sc       = RTEMS_SUCCESSFUL;
  struct ifnet                *ifp      = &self->arpcom.ac_if;
  const dwmac_callback_cfg    *CALLBACK = &self->CFG->CALLBACK;
  const dwmac_common_core_ops *CORE_OPS =
    (const dwmac_common_core_ops *) self->CFG->MAC_OPS->core;


  if ( up && self->state == DWMAC_COMMON_STATE_DOWN ) {
    eno = CALLBACK->nic_enable( self->arg );

    if ( eno == 0 ) {
      eno = CALLBACK->phy_enable( self->arg );
    }

    if ( eno == 0 ) {
      eno = CALLBACK->phy_event_enable(
        self->arg,
        PHY_EVENT_LINK_DOWN
        | PHY_EVENT_LINK_UP
        );
    }

    if ( eno == 0 ) {
      eno = dwmac_create_dma_desc_rings( self );
    }

    if ( eno == 0 ) {
      eno = dwmac_init_dma_engine( self );
    }

    if ( eno == 0 ) {
      /* Copy the MAC addr into the HW  */
      ( CORE_OPS->set_umac_addr )( self, self->arpcom.ac_enaddr, 0 );

      eno = ( CALLBACK->bus_setup )( self->arg );
    }

    if ( eno == 0 ) {
      /* Initialize the MAC Core */
      ( CORE_OPS->core_init )( self );

      /* Enable the MAC Rx/Tx */
      dwmac_core_set_mac( self, true );

      /* Set the HW DMA mode and the COE */
      dwmac_dma_operation_mode( self );

      /* Set up mmc counters */
      dwmac_mmc_setup( self );

#ifdef BSP_FEATURE_IRQ_EXTENSION
      /* Install interrupt handler */
      sc = rtems_interrupt_handler_install(
        self->CFG->IRQ_EMAC,
        "Ethernet",
        RTEMS_INTERRUPT_UNIQUE,
        dwmac_core_dma_interrupt,
        self
        );
#else
      sc = RTEMS_NOT_IMPLEMENTED;
#endif
      eno = rtems_status_code_to_errno( sc );
    }

    if ( eno == 0 ) {
      /* Start the ball rolling ... */
      eno = dwmac_control_request(
        self, self->task_id_tx, DWMAC_COMMON_EVENT_TASK_INIT );
    }

    if ( eno == 0 ) {
      eno = dwmac_control_request(
        self, self->task_id_rx, DWMAC_COMMON_EVENT_TASK_INIT );
    }

    if ( eno == 0 ) {
      dwmac_core_dma_start_rx( self );

      /* Start the phy */
      eno = ( CALLBACK->phy_start )( self->arg );
    }

    if ( eno == 0 ) {
      self->state = DWMAC_COMMON_STATE_UP;
    } else {
      ifp->if_flags &= ~IFF_UP;
      (void) dwmac_control_request(
        self, self->task_id_tx, DWMAC_COMMON_EVENT_TASK_STOP );
      (void) dwmac_control_request(
        self, self->task_id_rx, DWMAC_COMMON_EVENT_TASK_STOP );
#ifdef BSP_FEATURE_IRQ_EXTENSION
      (void) rtems_interrupt_handler_remove(
        self->CFG->IRQ_EMAC,
        dwmac_core_dma_interrupt,
        self
        );
#endif
      (void) ( CALLBACK->phy_stop )( self->arg );
      (void) ( CALLBACK->phy_disable )( self->arg );

      /* Disable the MAC Rx/Tx */
      dwmac_core_set_mac( self, false );
      (void) ( CALLBACK->nic_disable )( self->arg );
      (void) dwmac_destroy_dma_desc_rings( self );
    }
  } else if ( !up && self->state == DWMAC_COMMON_STATE_UP ) {
    if ( eno == 0 ) {
      eno = dwmac_control_request(
        self, self->task_id_tx, DWMAC_COMMON_EVENT_TASK_STOP );
    }

    if ( eno == 0 ) {
      eno = dwmac_control_request(
        self, self->task_id_rx, DWMAC_COMMON_EVENT_TASK_STOP );
    }

    if ( eno == 0 ) {
#ifdef BSP_FEATURE_IRQ_EXTENSION
      sc = rtems_interrupt_handler_remove(
        self->CFG->IRQ_EMAC,
        dwmac_core_dma_interrupt,
        self
        );
#else
      sc = RTEMS_NOT_IMPLEMENTED;
#endif
      eno = rtems_status_code_to_errno( sc );
    }

    if ( eno == 0 ) {
      eno = ( CALLBACK->phy_stop )( self->arg );
    }

    if ( eno == 0 ) {
      eno = ( CALLBACK->phy_disable )( self->arg );
    }

    if ( eno == 0 ) {
      /* Disable the MAC Rx/Tx */
      dwmac_core_set_mac( self, false );

      eno = CALLBACK->nic_disable( self->arg );
    }

    if ( eno == 0 ) {
      eno = dwmac_destroy_dma_desc_rings( self );
    }

    if ( eno == 0 ) {
      /* Reset counters to be printed */
      memset( &self->stats, 0, sizeof( dwmac_common_stats ) );

      /* Change state */
      self->state = DWMAC_COMMON_STATE_DOWN;
    }
  }

  return eno;
}

/* Get commands executed by the driver. This method gets called
 * automatically from the network stack */
static int dwmac_if_interface_ioctl(
  struct ifnet   *ifp,
  ioctl_command_t cmd,
  caddr_t         data )
{
  dwmac_common_context *self = ifp->if_softc;
  int                   eno  = 0;


  switch ( cmd ) {
    case SIOCGIFMEDIA:
    case SIOCSIFMEDIA:
      rtems_mii_ioctl( &self->mdio, self, cmd, (int *) data );
      break;
    case SIOCGIFADDR:
    case SIOCSIFADDR:
      ether_ioctl( ifp, cmd, data );
      break;
    case SIOCSIFFLAGS:
      eno = dwmac_if_up_or_down( self, ( ifp->if_flags & IFF_UP ) != 0 );

      if ( eno == 0 && ( ifp->if_flags & IFF_UP ) != 0 ) {
        dwmac_if_set_promiscous_mode( self, ( ifp->if_flags & IFF_PROMISC )
                                      != 0 );
      } else {
        assert( eno == 0 );
      }

      break;
    case SIOCADDMULTI:
    case SIOCDELMULTI:
#ifndef COMMENTED_OUT
      eno = ENOTSUP;
#else /* COMMENTED_OUT */
      {
        struct ifreq                *ifr      = (struct ifreq *) data;
        const dwmac_common_core_ops *CORE_OPS =
          (const dwmac_common_core_ops *) self->CFG->MAC_OPS->core;
        ( CORE_OPS->set_hash_filter )( self, cmd == SIOCADDMULTI, ifr );
      }
#endif /* COMMENTED_OUT */
      break;
    case SIO_RTEMS_SHOW_STATS:
      eno = dwmac_if_interface_stats( self );
      break;
    default:
      eno = EINVAL;
      break;
  }

  return eno;
}

/* Method called by the network stack if there are frames to be transmitted */
static void dwmac_if_interface_start( struct ifnet *ifp )
{
  rtems_status_code     sc;
  dwmac_common_context *self = ifp->if_softc;


  /* Mark that we are transmitting */
  ifp->if_flags |= IFF_OACTIVE;

  if ( self->state == DWMAC_COMMON_STATE_UP ) {
    /* Wake up our task */
    sc = rtems_bsdnet_event_send( self->task_id_tx,
                                  DWMAC_COMMON_EVENT_TX_TRANSMIT_FRAME );
    assert( sc == RTEMS_SUCCESSFUL );
  }
}

static int dwmac_if_clk_csr_set(
  const uint32_t        gmii_clk_rate,
  dwmac_common_context *self )
{
  int eno = 0;


  if ( gmii_clk_rate < DWCGNAC3504_CSR_F_35M ) {
    self->csr_clock = DWCGNAC3504_CSR_20_35M;
  } else if ( ( gmii_clk_rate >= DWCGNAC3504_CSR_F_35M )
              && ( gmii_clk_rate < DWCGNAC3504_CSR_F_60M ) ) {
    self->csr_clock = DWCGNAC3504_CSR_35_60M;
  } else if ( ( gmii_clk_rate >= DWCGNAC3504_CSR_F_60M )
              && ( gmii_clk_rate < DWCGNAC3504_CSR_F_100M ) ) {
    self->csr_clock = DWCGNAC3504_CSR_60_100M;
  } else if ( ( gmii_clk_rate >= DWCGNAC3504_CSR_F_100M )
              && ( gmii_clk_rate < DWCGNAC3504_CSR_F_150M ) ) {
    self->csr_clock = DWCGNAC3504_CSR_100_150M;
  } else if ( ( gmii_clk_rate >= DWCGNAC3504_CSR_F_150M )
              && ( gmii_clk_rate < DWCGNAC3504_CSR_F_250M ) ) {
    self->csr_clock = DWCGNAC3504_CSR_150_250M;
  } else if ( ( gmii_clk_rate >= DWCGNAC3504_CSR_F_250M )
              && ( gmii_clk_rate < DWCGNAC3504_CSR_F_300M ) ) {
    self->csr_clock = DWCGNAC3504_CSR_250_300M;
  } else {
    assert( gmii_clk_rate < DWCGNAC3504_CSR_F_300M );
    eno = EINVAL;
  }

  return eno;
}

static int dwmac_fixup_unit_count(
  const dwmac_common_context *self,
  const int                   count,
  const int                   default_value,
  const int                   max )
{
  int ret = count;


  if ( ret <= 0 ) {
    ret = DWMAC_ALIGN( default_value, self->CFG->L1_CACHE_LINE_SIZE );
  } else if ( ret > max ) {
    ret = DWMAC_ALIGN( max, self->CFG->L1_CACHE_LINE_SIZE );
  }

  return ret;
}

/* Called during initial setup */
static int dwmac_if_attach(
  dwmac_common_context         *self,
  struct rtems_bsdnet_ifconfig *bsd_config,
  const dwmac_cfg              *driver_config,
  void                         *arg )
{
  int           eno                     = 0;
  struct ifnet *ifp                     = &self->arpcom.ac_if;
  char         *unit_name               = NULL;
  int           unit_number             = rtems_bsdnet_parse_driver_name(
    bsd_config,
    &unit_name );
  const dwmac_callback_cfg    *CALLBACK = &driver_config->CALLBACK;
  const dwmac_common_desc_ops *DESC_OPS =
    (const dwmac_common_desc_ops *) driver_config->DESC_OPS->ops;
  const dwmac_ifconfig_drv_ctrl *drv_ctrl =
    (const dwmac_ifconfig_drv_ctrl *) bsd_config->drv_ctrl;

  assert( self != NULL );
  assert( bsd_config != NULL );
  assert( driver_config != NULL );
  assert( CALLBACK != NULL );

  if ( self == NULL
       || bsd_config == NULL
       || driver_config == NULL
       || CALLBACK == NULL
       ) {
    eno = EINVAL;
  } else {
    assert( CALLBACK->nic_enable != NULL );
    assert( CALLBACK->nic_disable != NULL );
    assert( CALLBACK->phy_enable != NULL );
    assert( CALLBACK->phy_disable != NULL );
    assert( CALLBACK->phy_event_enable != NULL );
    assert( CALLBACK->phy_event_clear != NULL );
    assert( CALLBACK->phy_events_get != NULL );
    assert( CALLBACK->phy_start != NULL );
    assert( CALLBACK->phy_stop != NULL );
    assert( CALLBACK->mem_alloc_nocache != NULL );
    assert( CALLBACK->mem_free_nocache != NULL );
    assert( CALLBACK->bus_setup != NULL );

    if ( CALLBACK->nic_enable == NULL
         || CALLBACK->nic_disable == NULL
         || CALLBACK->phy_enable == NULL
         || CALLBACK->phy_disable == NULL
         || CALLBACK->phy_event_enable == NULL
         || CALLBACK->phy_event_clear == NULL
         || CALLBACK->phy_events_get == NULL
         || CALLBACK->phy_start == NULL
         || CALLBACK->phy_stop == NULL
         || CALLBACK->mem_alloc_nocache == NULL
         || CALLBACK->mem_free_nocache == NULL
         || CALLBACK->bus_setup == NULL
         ) {
      eno = EINVAL;
    }
  }

  if ( eno == 0 ) {
    self->dma_threshold_control = DWMAC_DMA_THRESHOLD_CONTROL_DEFAULT;

    assert( driver_config->addr_gmac_regs != NULL );
    if ( driver_config->addr_gmac_regs != NULL ) {
      self->macgrp = driver_config->addr_gmac_regs;

      assert( driver_config->addr_dma_regs != NULL );
      if ( driver_config->addr_dma_regs != NULL ) {
        self->dmagrp = driver_config->addr_dma_regs;
      } else {
        eno = EINVAL;
      }
    } else {
      eno = EINVAL;
    }
  }

  if ( eno == 0 ) {
    eno = dwmac_if_clk_csr_set(
      driver_config->GMII_CLK_RATE,
      self
      );
  }

  if ( eno == 0 ) {
    if ( drv_ctrl == NULL ) {
      self->MDIO_BUS_ADDR = driver_config->MDIO_BUS_ADDR;
    } else {
      self->MDIO_BUS_ADDR = drv_ctrl->phy_addr;
    }

    assert( 32 >= self->MDIO_BUS_ADDR );

    if ( 32 < self->MDIO_BUS_ADDR ) {
      eno = EINVAL;
    }
  }

  if ( eno == 0 ) {
    self->arg        = arg;        /* Optional argument from upper layer */
    self->state      = DWMAC_COMMON_STATE_DOWN;
    self->bsd_config = bsd_config; /* BSD-Net configuration.
                                      Exists for ll BSD based network divers */

    /* Device control */
    bsd_config->drv_ctrl = NULL;

    /* MDIO */
    self->mdio.mdio_r = dwmac_if_mdio_read; /* read from phy
                                               registers */
    self->mdio.mdio_w = dwmac_if_mdio_write; /* write to phy
                                                registers */

    if ( driver_config->MAC_OPS == &DWMAC_1000_ETHERNET_MAC_OPS ) {
      self->mdio.has_gmii = 1;
    } else {
      self->mdio.has_gmii = 0;

      /* dwmac_dma_ops_100 and dwmac_core_ops_100 is not yet implemented! */
      assert( driver_config->MAC_OPS == &DWMAC_1000_ETHERNET_MAC_OPS );
      eno = ENOTSUP;
    }

    if ( eno == 0 ) {
      self->CFG     = driver_config;
      ifp->if_ioctl = dwmac_if_interface_ioctl; /* Execute commands
                                                   at runtime */
      ifp->if_init  = dwmac_if_interface_init;        /* Initialization
                                                         (called once) */

      /* Receive unit count */
      bsd_config->rbuf_count = dwmac_fixup_unit_count(
        self,
        bsd_config->rbuf_count,
        DWMAC_CONFIG_RX_UNIT_COUNT_DEFAULT,
        DWMAC_CONFIG_RX_UNIT_COUNT_MAX
        );

      /* Transmit unit count */
      bsd_config->xbuf_count = dwmac_fixup_unit_count(
        self,
        bsd_config->xbuf_count,
        DWMAC_CONFIG_TX_UNIT_COUNT_DEFAULT,
        DWMAC_CONFIG_TX_UNIT_COUNT_MAX
        );

      if (
        DWMAC_GLOBAL_MBUF_CNT / 4 < bsd_config->rbuf_count
          || DWMAG_GLOBAL_MCLUST_CNT / 4 < bsd_config->rbuf_count
      ) {
        bsp_fatal( DWMAC_FATAL_TOO_MANY_RBUFS_CONFIGURED );
      }

      /* Copy MAC address */
      memcpy(
        self->arpcom.ac_enaddr,
        bsd_config->hardware_address,
        ETHER_ADDR_LEN
        );

      /* Set interface data */
      ifp->if_softc          = self;
      ifp->if_unit           = (short) unit_number;
      ifp->if_name           = unit_name;

      ifp->if_start          = dwmac_if_interface_start;  /* Start transmitting
                                                             frames */
      ifp->if_output         = ether_output;
      ifp->if_watchdog       = NULL;
      ifp->if_flags          = IFF_BROADCAST | IFF_SIMPLEX;
      ifp->if_snd.ifq_maxlen = ifqmaxlen;
      ifp->if_timer          = 0;
      ifp->if_mtu            =
        bsd_config->mtu > 0 ? (u_long) bsd_config->mtu : ETHERMTU;

      eno = DESC_OPS->validate( self );
      assert( eno == 0 );
    }

    if ( eno == 0 ) {
      /* Create and start the receive task for our driver */
      assert( self->task_id_rx == RTEMS_ID_NONE );
      self->task_id_rx = rtems_bsdnet_newproc(
        "ntrx",
        4096,
        dwmac_task_rx,
        self
        );
      assert( self->task_id_rx != RTEMS_ID_NONE );

      if ( self->task_id_rx == RTEMS_ID_NONE ) {
        eno = ENOMEM;
      }
    }

    if ( eno == 0 ) {
      /* Create and start the transmit task for our driver */
      assert( self->task_id_tx == RTEMS_ID_NONE );
      self->task_id_tx = rtems_bsdnet_newproc(
        "nttx",
        4096,
        dwmac_task_tx,
        self
        );
      assert( self->task_id_tx != RTEMS_ID_NONE );

      if ( self->task_id_tx == RTEMS_ID_NONE ) {
        eno = ENOMEM;
      }
    }
  }

  if ( eno == 0 ) {
    /* Change status */
    ifp->if_flags |= IFF_RUNNING;

    /* Attach the interface */
    if_attach( ifp );
    ether_ifattach( ifp );
  }

  return eno;
}

/* Initial setup method. Part of the driver API
 * Returns the address of the driver context on success and NULL on failure */
void *dwmac_network_if_attach_detach(
  struct rtems_bsdnet_ifconfig *bsd_config,
  const dwmac_cfg              *driver_config,
  void                         *arg,
  int                           attaching )
{
  int                   eno     = 0;
  dwmac_common_context *context = NULL;


  if ( attaching ) {
    context = calloc( 1, sizeof( dwmac_common_context ) );
    assert( context != NULL );

    if ( context != NULL ) {
      eno = dwmac_if_attach(
        context,
        bsd_config,
        driver_config,
        arg
        );

      if ( eno != 0 ) {
        free( context, 0 );
        context = NULL;
      }
    }
  } else {
    /* Detaching is not supported */
    assert( attaching != false );
  }

  return context;
}

int dwmac_if_read_from_phy(
  void          *arg,
  const unsigned phy_reg,
  uint16_t      *val )
{
  int                   eno  = EINVAL;
  dwmac_common_context *self = (dwmac_common_context *) arg;
  uint32_t              value;


  if ( arg != NULL ) {
    eno = dwmac_if_mdio_read(
      self->MDIO_BUS_ADDR,
      self,
      phy_reg,
      &value );
  }

  if ( eno == 0 ) {
    *val = (uint16_t) value;
  }

  return eno;
}

int dwmac_if_write_to_phy(
  void          *arg,
  const unsigned phy_reg,
  const uint16_t val )
{
  int                   eno  = EINVAL;
  dwmac_common_context *self = (dwmac_common_context *) arg;


  if ( arg != NULL ) {
    eno = dwmac_if_mdio_write(
      self->MDIO_BUS_ADDR,
      self,
      phy_reg,
      val );
  }

  return eno;
}

int dwmac_if_handle_phy_event( void *arg )
{
  int                   eno;
  rtems_status_code     sc;
  dwmac_common_context *self = (dwmac_common_context *) arg;


  sc = rtems_bsdnet_event_send(
    self->task_id_tx,
    DWMAC_COMMON_EVENT_TX_PHY_STATUS_CHANGE
    );
  eno = rtems_status_code_to_errno( sc );

  return eno;
}
