/**
 * @file
 *
 * @brief DWMAC 10/100/1000 Enhanced DMA Descriptor Handling.
 *
 * DWMAC 10/100/1000 on-chip Ethernet controllers.
 * Functions and data for the handling of enhanced DMA descriptors.
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
 * http://www.rtems.org/license/LICENSE.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "dwmac-common.h"
#include "dwmac-desc-com.h"
#include "dwmac-core.h"
#include <sys/queue.h>

#undef DWMAC_DESC_ENH_DEBUG
#ifdef DWMAC_DESC_ENH_DEBUG
#define DWMAC_DESC_ENH_PRINT_DBG( fmt, args ... )  printk( fmt, ## args )
#else
#define DWMAC_DESC_ENH_PRINT_DBG( fmt, args ... )  do { } while ( 0 )
#endif

typedef enum {
  DWMAC_IP_PAYLOAD_TYPE_UNKNOWN,
  DWMAC_IP_PAYLOAD_TYPE_UDP,
  DWMAC_IP_PAYLOAD_TYPE_TCP,
  DWMAC_IP_PAYLOAD_TYPE_ICMP
} dwmac_ip_payload_type;

static void dwmac_desc_enh_rx_set_on_ring_chain(
  volatile dwmac_desc_ext *p, int end )
{
  /* For simplicity reasons we will not use the second buffer.
   * If we would use it we would have to set the size to MCLBYTES -1 */
  p->erx.des0_3.des1 = DWMAC_DESC_ERX_DES1_RECEIVE_BUFF_2_SIZE_SET(
    p->erx.des0_3.des1, 0
    );
  p->erx.des0_3.des3 = (uint32_t) NULL;

  if ( end )
    p->erx.des0_3.des1 |= DWMAC_DESC_ERX_DES1_RECEIVE_END_OF_RING;
}

static void dwmac_desc_enh_tx_set_on_ring_chain(
  volatile dwmac_desc_ext *p, const bool end )
{
  if ( end )
    p->etx.des0_3.des0 |= DWMAC_DESC_ETX_DES0_TRANSMIT_END_OF_RING;
}

static void dwmac_desc_enh_set_tx_desc_len(
  volatile dwmac_desc_ext *p_enh, size_t len )
{
  p_enh->etx.des0_3.des1 = DWMAC_DESC_ETX_DES1_TRANSMIT_BUFFER_1_SIZE_SET(
    p_enh->etx.des0_3.des1,
    len
    );
}

static bool dwmac_desc_enh_is_giant_frame( const uint32_t des0 )
{
  return (
    ( des0
      & DWMAC_DESC_ERX_DES0_TIMESTAMP_AVAIL_OR_CHECKSUM_ERROR_OR_GIANT_FRAME
    ) != 0
    );
}

static bool dwmac_desc_enh_is_udp_payload( const uint32_t des4 )
{
  return (
    DWMAC_DESC_EXT_ERX_DES4_IP_PAYLOAD_TYPE_GET( des4 )
    == DWMAC_IP_PAYLOAD_TYPE_UDP
    );
}

static bool dwmac_desc_enh_is_tcp_payload( const uint32_t des4 )
{
  return (
    DWMAC_DESC_EXT_ERX_DES4_IP_PAYLOAD_TYPE_GET( des4 )
    == DWMAC_IP_PAYLOAD_TYPE_TCP
    );
}

static bool dwmac_desc_enh_is_icmp_payload( const uint32_t des4 )
{
  return (
    DWMAC_DESC_EXT_ERX_DES4_IP_PAYLOAD_TYPE_GET( des4 )
    == DWMAC_IP_PAYLOAD_TYPE_ICMP
    );
}

static dwmac_common_rx_frame_status dwmac_desc_enh_coe_status(
  volatile dwmac_desc_ext *p_enh )
{
  dwmac_common_rx_frame_status ret  = DWMAC_COMMON_RX_FRAME_STATUS_GOOD;
  const uint32_t               DES0 = p_enh->erx.des0_3.des0;
  const uint32_t               DES4 = p_enh->erx.des4;


  if ( ( DES0 & DWMAC_DESC_ERX_DES0_EXT_STATUS_AVAIL_OR_RX_MAC_ADDR_STATUS )
       != 0 ) {
    if ( !dwmac_desc_enh_is_giant_frame( DES0 )
         && ( DES0 & DWMAC_DESC_ERX_DES0_FREAME_TYPE ) == 0
         && ( DES4 & DWMAC_DESC_EXT_ERX_DES4_IPV6_PACKET_RECEIVED ) == 0
         && ( DES4 & DWMAC_DESC_EXT_ERX_DES4_IPV4_PACKET_RECEIVED ) == 0
         && ( DES4 & DWMAC_DESC_EXT_ERX_DES4_IP_PAYLOAD_ERROR ) == 0
         && ( DES4 & DWMAC_DESC_EXT_ERX_DES4_IP_HEADER_ERROR ) == 0 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "RX Des0 status: IEEE 802.3 Type frame.\n" );
      ret = DWMAC_COMMON_RX_FRAME_STATUS_LLC_SNAP;
    } else if ( ( ( DES4 & DWMAC_DESC_EXT_ERX_DES4_IPV6_PACKET_RECEIVED ) != 0
                  || ( DES4 & DWMAC_DESC_EXT_ERX_DES4_IPV4_PACKET_RECEIVED )
                  != 0 )
                && dwmac_desc_enh_is_giant_frame( DES0 ) ) {
      DWMAC_DESC_ENH_PRINT_DBG( "RX Des0 status: IPv4/6 No CSUM Error.\n" );
      ret = DWMAC_COMMON_RX_FRAME_STATUS_GOOD;
    } else if ( ( ( DES4 & DWMAC_DESC_EXT_ERX_DES4_IPV6_PACKET_RECEIVED ) != 0
                  || ( DES4 & DWMAC_DESC_EXT_ERX_DES4_IPV4_PACKET_RECEIVED )
                  != 0 )
                && ( DES4 & DWMAC_DESC_EXT_ERX_DES4_IP_PAYLOAD_ERROR ) != 0 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "RX Des0 status: IPv4/6 Payload Error.\n" );
      ret = DWMAC_COMMON_RX_FRAME_STATUS_CSUM_NONE;
    } else if ( ( ( DES4 & DWMAC_DESC_EXT_ERX_DES4_IPV6_PACKET_RECEIVED ) != 0
                  || ( DES4 & DWMAC_DESC_EXT_ERX_DES4_IPV4_PACKET_RECEIVED )
                  != 0 )
                && ( DES4 & DWMAC_DESC_EXT_ERX_DES4_IP_HEADER_ERROR ) != 0 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "RX Des0 status: IPv4/6 Header Error.\n" );
      ret = DWMAC_COMMON_RX_FRAME_STATUS_CSUM_NONE;
    } else if ( ( ( DES4 & DWMAC_DESC_EXT_ERX_DES4_IPV6_PACKET_RECEIVED ) != 0
                  || ( DES4 & DWMAC_DESC_EXT_ERX_DES4_IPV4_PACKET_RECEIVED )
                  != 0 )
                && ( DES4 & DWMAC_DESC_EXT_ERX_DES4_IP_PAYLOAD_ERROR ) != 0
                && ( DES4 & DWMAC_DESC_EXT_ERX_DES4_IP_HEADER_ERROR ) != 0 ) {
      DWMAC_DESC_ENH_PRINT_DBG(
        "RX Des0 status: IPv4/6 Header and Payload Error.\n" );
      ret = DWMAC_COMMON_RX_FRAME_STATUS_CSUM_NONE;
    } else if ( ( ( DES4 & DWMAC_DESC_EXT_ERX_DES4_IPV6_PACKET_RECEIVED ) != 0
                  || ( DES4 & DWMAC_DESC_EXT_ERX_DES4_IPV4_PACKET_RECEIVED )
                  != 0 )
                && ( !dwmac_desc_enh_is_udp_payload( DES4 ) )
                && ( !dwmac_desc_enh_is_tcp_payload( DES4 ) )
                && ( !dwmac_desc_enh_is_icmp_payload( DES4 ) ) ) {
      DWMAC_DESC_ENH_PRINT_DBG(
        "RX Des0 status: IPv4/6 unsupported IP PAYLOAD.\n" );
      ret = DWMAC_COMMON_RX_FRAME_STATUS_DISCARD;
    } else if ( ( DES4 & DWMAC_DESC_EXT_ERX_DES4_IPV6_PACKET_RECEIVED ) == 0
                && ( DES4 & DWMAC_DESC_EXT_ERX_DES4_IPV4_PACKET_RECEIVED )
                == 0 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "RX Des0 status: No IPv4, IPv6 frame.\n" );
      ret = DWMAC_COMMON_RX_FRAME_STATUS_DISCARD;
    }
  } else {
    uint32_t status = (
      (uint32_t) ( dwmac_desc_enh_is_giant_frame( DES0 ) << 2U )
      | (uint32_t) ( ( ( DES0 & DWMAC_DESC_ERX_DES0_FREAME_TYPE )
                       != 0 ) << 1U ) )
                      & 0x7U;

    /* bits 5 7 0 | Frame status
     * ----------------------------------------------------------
     *      0 0 0 | IEEE 802.3 Type frame (length < 1536 octects)
     *      1 0 0 | IPv4/6 No CSUM errorS.
     *      1 0 1 | IPv4/6 CSUM PAYLOAD error
     *      1 1 0 | IPv4/6 CSUM IP HR error
     *      1 1 1 | IPv4/6 IP PAYLOAD AND HEADER errorS
     *      0 0 1 | IPv4/6 unsupported IP PAYLOAD
     *      0 1 1 | COE bypassed.. no IPv4/6 frame
     *      0 1 0 | Reserved.
     */
    if ( status == 0x0 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "RX Des0 status: IEEE 802.3 Type frame.\n" );
      ret = DWMAC_COMMON_RX_FRAME_STATUS_LLC_SNAP;
    } else if ( status == 0x4 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "RX Des0 status: IPv4/6 No CSUM errorS.\n" );
      ret = DWMAC_COMMON_RX_FRAME_STATUS_GOOD;
    } else if ( status == 0x5 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "RX Des0 status: IPv4/6 Payload Error.\n" );
      ret = DWMAC_COMMON_RX_FRAME_STATUS_CSUM_NONE;
    } else if ( status == 0x6 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "RX Des0 status: IPv4/6 Header Error.\n" );
      ret = DWMAC_COMMON_RX_FRAME_STATUS_CSUM_NONE;
    } else if ( status == 0x7 ) {
      DWMAC_DESC_ENH_PRINT_DBG(
        "RX Des0 status: IPv4/6 Header and Payload Error.\n" );
      ret = DWMAC_COMMON_RX_FRAME_STATUS_CSUM_NONE;
    } else if ( status == 0x1 ) {
      DWMAC_DESC_ENH_PRINT_DBG(
        "RX Des0 status: IPv4/6 unsupported IP PAYLOAD.\n" );
      ret = DWMAC_COMMON_RX_FRAME_STATUS_DISCARD;
    } else if ( status == 0x3 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "RX Des0 status: No IPv4, IPv6 frame.\n" );
      ret = DWMAC_COMMON_RX_FRAME_STATUS_DISCARD;
    }
  }

  return ret;
}

static int dwmac_desc_enh_get_tx_status(
  dwmac_common_context *self,
  const unsigned int    idx_tx )
{
  int                                 ret    = 0;
  volatile dwmac_desc_ext            *dma_tx =
    (volatile dwmac_desc_ext *) self->dma_tx;
  volatile dwmac_desc_ext            *p_desc = &dma_tx[idx_tx];
  dwmac_common_desc_status_counts_tx *counts =
    &self->stats.desc_status_counts_tx;


  if ( ( p_desc->etx.des0_3.des0 & DWMAC_DESC_ETX_DES0_ERROR_SUMMARY ) != 0 ) {
    DWMAC_DESC_ENH_PRINT_DBG( "DWMAC TX error... 0x%08x\n", p->des01.etx );

    if ( ( p_desc->etx.des0_3.des0 & DWMAC_DESC_ETX_DES0_JABBER_TIMEOUT )
         != 0 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "\tjabber_timeout error\n" );
      ++counts->jabber;
    }

    if ( ( p_desc->etx.des0_3.des0 & DWMAC_DESC_ETX_DES0_FRAME_FLUSHED )
         != 0 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "\tframe_flushed error\n" );
      ++counts->frame_flushed;
      dwmac_core_dma_flush_tx_fifo( self );
    }

    if ( ( p_desc->etx.des0_3.des0 & DWMAC_DESC_ETX_DES0_LOSS_OF_CARRIER )
         != 0 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "\tloss_carrier error\n" );
      ++counts->losscarrier;
    }

    if ( ( p_desc->etx.des0_3.des0 & DWMAC_DESC_ETX_DES0_NO_CARRIER )
         != 0 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "\tno_carrier error\n" );
      ++counts->no_carrier;
    }

    if ( ( p_desc->etx.des0_3.des0 & DWMAC_DESC_ETX_DES0_EXCESSIVE_COLLISION )
         != 0 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "\texcessive_collisions\n" );
      ++counts->excessive_collisions;
    }

    if ( ( p_desc->etx.des0_3.des0 & DWMAC_DESC_ETX_DES0_EXCESSIVE_DEFERAL )
         != 0 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "\texcessive tx_deferral\n" );
      ++counts->excessive_deferral;
    }

    if ( ( p_desc->etx.des0_3.des0 & DWMAC_DESC_ETX_DES0_UNDERFLOW_ERROR )
         != 0 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "\tunderflow error\n" );
      dwmac_core_dma_flush_tx_fifo( self );
      ++counts->underflow;
    }

    if ( ( p_desc->etx.des0_3.des0 & DWMAC_DESC_ETX_DES0_IP_HEADER_ERROR )
         != 0 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "\tTX IP header csum error\n" );
      ++counts->ip_header_error;
    }

    if ( ( p_desc->etx.des0_3.des0 & DWMAC_DESC_ETX_DES0_IP_PAYLOAD_ERROR )
         != 0 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "\tAddr/Payload csum error\n" );
      ++counts->payload_error;
      dwmac_core_dma_flush_tx_fifo( self );
    }

    ret = -1;
  }

  if ( ( p_desc->etx.des0_3.des0 & DWMAC_DESC_ETX_DES0_DEFERRED_BIT ) != 0 ) {
    DWMAC_DESC_ENH_PRINT_DBG( "GMAC TX status: tx deferred\n" );
    ++counts->deferred;
  }

  if ( ( p_desc->etx.des0_3.des0 & DWMAC_DESC_ETX_DES0_VLAN_FRAME ) != 0 ) {
    DWMAC_DESC_ENH_PRINT_DBG( "GMAC TX status: VLAN frame\n" );
    ++counts->vlan;
  }

  return ret;
}

static dwmac_common_rx_frame_status dwmac_desc_enh_get_rx_status(
  dwmac_common_context *self,
  const unsigned int    desc_idx )
{
  dwmac_common_desc_status_counts_rx *counts =
    &self->stats.desc_status_counts_rx;
  dwmac_common_rx_frame_status        ret    =
    DWMAC_COMMON_RX_FRAME_STATUS_GOOD;
  volatile dwmac_desc_ext            *dma_rx =
    (volatile dwmac_desc_ext *) self->dma_rx;
  const uint32_t                      DES0   = dma_rx[desc_idx].erx.des0_3.des0;


  if ( ( DES0 & DWMAC_DESC_ERX_DES0_ERROR_SUMMARY ) != 0 ) {
    DWMAC_DESC_ENH_PRINT_DBG( "GMAC RX Error Summary 0x%08x\n",
                              DES0 );

    if ( ( DES0 & DWMAC_DESC_ERX_DES0_DESCRIPTOR_ERROR ) != 0 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "\tdescriptor error\n" );
      ++counts->descriptor_error;
    }

    if ( ( DES0 & DWMAC_DESC_ERX_DES0_OVERFLOW_ERROR ) != 0 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "\toverflow error\n" );
      ++counts->overflow_error;
    }

    if ( dwmac_desc_enh_is_giant_frame( DES0 ) ) {
      DWMAC_DESC_ENH_PRINT_DBG( "\tIPC Csum Error/Giant frame\n" );
      ++counts->giant_frame;
    }

    if ( ( DES0 & DWMAC_DESC_ERX_DES0_LATE_COLLISION ) != 0 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "\tlate_collision error\n" );
      ++counts->late_collision;
    }

    if ( ( DES0 & DWMAC_DESC_ERX_DES0_RECEIVE_WATCHDOG_TIMEOUT )
         != 0 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "\treceive_watchdog error\n" );
      ++counts->watchdog_timeout;
    }

    if ( ( DES0 & DWMAC_DESC_ERX_DES0_RECEIVE_ERROR ) != 0 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "\tReceive Error\n" );
      ++counts->receive_error;
    }

    if ( ( DES0 & DWMAC_DESC_ERX_DES0_CRC_ERROR ) != 0 ) {
      DWMAC_DESC_ENH_PRINT_DBG( "\tCRC error\n" );
      ++counts->crc_error;
    }

    ret = DWMAC_COMMON_RX_FRAME_STATUS_DISCARD;
  }

  if ( ret == DWMAC_COMMON_RX_FRAME_STATUS_GOOD ) {
    /* After a payload csum error, the ES bit is set.
     * It doesn't match with the information reported into the databook.
     * At any rate, we need to understand if the CSUM hw computation is ok
     * and report this info to the upper layers. */
    ret = dwmac_desc_enh_coe_status( &dma_rx[desc_idx] );
  }

  if ( ( DES0 & DWMAC_DESC_ERX_DES0_DRIBBLE_BIT_ERROR ) != 0 ) {
    DWMAC_DESC_ENH_PRINT_DBG( "GMAC RX: dribbling error\n" );
    ++counts->dribble_bit_error;
  }

  if ( ( DES0 & DWMAC_DESC_ERX_DES0_SRC_ADDR_FILTER_FAIL ) != 0 ) {
    DWMAC_DESC_ENH_PRINT_DBG( "GMAC RX : Source Address filter fail\n" );
    ++counts->source_addr_fail;
    ret = DWMAC_COMMON_RX_FRAME_STATUS_DISCARD;
  }

  if ( ( DES0 & DWMAC_DESC_ERX_DES0_DEST_ADDR_FILTER_FAIL ) != 0 ) {
    DWMAC_DESC_ENH_PRINT_DBG( "GMAC RX : Dest Address filter fail\n" );
    ++counts->dest_addr_fail;
    ret = DWMAC_COMMON_RX_FRAME_STATUS_DISCARD;
  }

  if ( ( DES0 & DWMAC_DESC_ERX_DES0_LENGTH_ERROR ) != 0 ) {
    DWMAC_DESC_ENH_PRINT_DBG( "GMAC RX: length_error error\n" );
    ++counts->length_error;
    ret = DWMAC_COMMON_RX_FRAME_STATUS_DISCARD;
  }

  if ( ( DES0 & DWMAC_DESC_ERX_DES0_VLAN_TAG ) != 0 ) {
    DWMAC_DESC_ENH_PRINT_DBG( "GMAC RX: VLAN frame tagged\n" );
    ++counts->vlan_tag;
  }

  return ret;
}

static void dwmac_desc_enh_print_tx_desc(
  volatile dwmac_desc *p,
  const unsigned int   count )
{
  volatile dwmac_desc_ext *p_enh = (volatile dwmac_desc_ext *) p;
  unsigned int             index;


  if ( p_enh != NULL ) {
    for ( index = 0; index < count; ++index ) {
      printf( "Transmit DMA Descriptor %d\n", index );
      printf( "des0\n" );
      printf(
        " %u own bit\n"
        " %u IRQ on Completion\n"
        " %u Last Segment\n"
        " %u First Segment\n"
        " %u Disable CRC\n"
        " %u Disable Pad\n"
        " %u Transmit Timestamp Enable\n"
        " %lu Checksum Insertion Control\n"
        " %u Transmit End of Ring\n"
        " %u Second Address Chained\n"
        " %u Transmit Timestamp Status\n"
        " %u IP Header Error\n"
        " %u VLAN Frame\n"
        " %lu Collision Count\n"
        " %u Deferred Bit\n",
        ( p_enh[index].etx.des0_3.des0 & DWMAC_DESC_ETX_DES0_OWN_BIT ) != 0,
        ( p_enh[index].etx.des0_3.des0 & DWMAC_DESC_ETX_DES0_IRQ_ON_COMPLETION ) != 0,
        ( p_enh[index].etx.des0_3.des0 & DWMAC_DESC_ETX_DES0_LAST_SEGMENT ) != 0,
        ( p_enh[index].etx.des0_3.des0 & DWMAC_DESC_ETX_DES0_FIRST_SEGMENT ) != 0,
        ( p_enh[index].etx.des0_3.des0 & DWMAC_DESC_ETX_DES0_DISABLE_CRC ) != 0,
        ( p_enh[index].etx.des0_3.des0 & DWMAC_DESC_ETX_DES0_DISABLE_PAD ) != 0,
        ( p_enh[index].etx.des0_3.des0
          & DWMAC_DESC_ETX_DES0_TRANSMIT_TIMESTAMP_ENABLE ) != 0,
        DWMAC_DESC_ETX_DES0_CHECKSUM_INSERTION_CONTROL_GET( p_enh[index].etx.
                                                            des0_3.des0 ),
        ( p_enh[index].etx.des0_3.des0
          & DWMAC_DESC_ETX_DES0_TRANSMIT_END_OF_RING ) != 0,
        ( p_enh[index].etx.des0_3.des0 & DWMAC_DESC_ETX_DES0_SECOND_ADDR_CHAINED ) != 0,
        ( p_enh[index].etx.des0_3.des0
          & DWMAC_DESC_ETX_DES0_TRANSMIT_TIMESTAMP_STATUS ) != 0,
        ( p_enh[index].etx.des0_3.des0 & DWMAC_DESC_ETX_DES0_IP_HEADER_ERROR ) != 0,
        ( p_enh[index].etx.des0_3.des0 & DWMAC_DESC_ETX_DES0_VLAN_FRAME ) != 0,
        DWMAC_DESC_ETX_DES0_COLLISION_COUNT_GET( p_enh[index].etx.des0_3.des0 ),
        ( p_enh[index].etx.des0_3.des0 & DWMAC_DESC_ETX_DES0_DEFERRED_BIT ) != 0
        );

      if ( ( p_enh[index].etx.des0_3.des0
             & DWMAC_DESC_ETX_DES0_ERROR_SUMMARY ) != 0 ) {
        printf( " Error Summary:\n" );

        if ( p_enh[index].etx.des0_3.des0
             & DWMAC_DESC_ETX_DES0_JABBER_TIMEOUT ) {
          printf( "  Jabber Timeout\n" );
        }

        if ( ( p_enh[index].etx.des0_3.des0
               & DWMAC_DESC_ETX_DES0_FRAME_FLUSHED ) != 0 ) {
          printf( "  Frame Flush\n" );
        }

        if ( ( p_enh[index].etx.des0_3.des0
               & DWMAC_DESC_ETX_DES0_IP_PAYLOAD_ERROR ) != 0 ) {
          printf( "  Payload Error\n" );
        }

        if ( ( p_enh[index].etx.des0_3.des0
               & DWMAC_DESC_ETX_DES0_LOSS_OF_CARRIER ) != 0 ) {
          printf( "  Loss of Carrier\n" );
        }

        if ( ( p_enh[index].etx.des0_3.des0
               & DWMAC_DESC_ETX_DES0_NO_CARRIER ) != 0 ) {
          printf( "  No Carrier\n" );
        }

        if ( ( p_enh[index].etx.des0_3.des0
               & DWMAC_DESC_ETX_DES0_EXCESSIVE_COLLISION ) != 0 ) {
          printf( "  Excessive Collision\n" );
        }

        if ( ( p_enh[index].etx.des0_3.des0
               & DWMAC_DESC_ETX_DES0_EXCESSIVE_COLLISION ) != 0 ) {
          printf( "  Ecessive Deferral\n" );
        }

        if ( ( p_enh[index].etx.des0_3.des0
               & DWMAC_DESC_ETX_DES0_UNDERFLOW_ERROR ) != 0 ) {
          printf( "  Undeflow Error\n" );
        }
      }

      printf( "des1\n" );
      printf(
        " %lu Transmit Buffer 2 Size\n"
        " %lu Transmit Buffer 1 Size\n",
        DWMAC_DESC_ETX_DES1_TRANSMIT_BUFFER_2_SIZE_GET( p_enh[index].etx.des0_3.
                                                        des1 ),
        DWMAC_DESC_ETX_DES1_TRANSMIT_BUFFER_1_SIZE_GET( p_enh[index].etx.des0_3.
                                                        des1 )
        );
      printf( "des2\n" );
      printf( " %p Buffer 1 Address\n", (void *) p_enh[index].etx.des0_3.des2 );
      printf( "des3\n" );
      printf( " %p Buffer 2 Address\n", (void *) p_enh[index].etx.des0_3.des3 );
    }
  }
}

static void dwmac_desc_enh_print_rx_desc(
  volatile dwmac_desc *p,
  const unsigned int   count )
{
  volatile dwmac_desc_ext *p_enh = (volatile dwmac_desc_ext *) p;
  unsigned int             index;


  if ( p_enh != NULL ) {
    for ( index = 0; index < count; ++index ) {
      printf( "Receive DMA Descriptor %d\n", index );
      printf( "des0\n" );
      printf(
        " %u Own Bit\n"
        " %u Dest. Addr. Filter Fail\n"
        " %lu Frame Length\n"
        " %u Source Addr. Filter Fail\n"
        " %u Length Error\n"
        " %u VLAN Tag\n"
        " %u First Descriptor\n"
        " %u Last Descriptor\n"
        " %u Frame Type\n"
        " %u Dribble Bit Error\n"
        " %u Extended Status Available\n",
        ( p_enh[index].erx.des0_3.des0 & DWMAC_DESC_ERX_DES0_OWN_BIT ) != 0,
        ( p_enh[index].erx.des0_3.des0
          & DWMAC_DESC_ERX_DES0_DEST_ADDR_FILTER_FAIL ) != 0,
        DWMAC_DESC_ERX_DES0_FRAME_LENGTH_GET(
          p_enh[index].erx.des0_3.des0 ),
        ( p_enh[index].erx.des0_3.des0
          & DWMAC_DESC_ERX_DES0_SRC_ADDR_FILTER_FAIL ) != 0,
        ( p_enh[index].erx.des0_3.des0 & DWMAC_DESC_ERX_DES0_LENGTH_ERROR ) != 0,
        ( p_enh[index].erx.des0_3.des0 & DWMAC_DESC_ERX_DES0_VLAN_TAG ) != 0,
        ( p_enh[index].erx.des0_3.des0 & DWMAC_DESC_ERX_DES0_FIRST_DESCRIPTOR ) != 0,
        ( p_enh[index].erx.des0_3.des0 & DWMAC_DESC_ERX_DES0_LAST_DESCRIPTOR ) != 0,
        ( p_enh[index].erx.des0_3.des0 & DWMAC_DESC_ERX_DES0_FREAME_TYPE ) != 0,
        ( p_enh[index].erx.des0_3.des0 & DWMAC_DESC_ERX_DES0_DRIBBLE_BIT_ERROR ) != 0,
        ( p_enh[index].erx.des0_3.des0
          & DWMAC_DESC_ERX_DES0_EXT_STATUS_AVAIL_OR_RX_MAC_ADDR_STATUS ) != 0
        );

      if ( ( p_enh[index].erx.des0_3.des0
             & DWMAC_DESC_ERX_DES0_ERROR_SUMMARY ) != 0 ) {
        printf( " Error Summary:\n" );

        if ( ( p_enh[index].erx.des0_3.des0
               & DWMAC_DESC_ERX_DES0_DESCRIPTOR_ERROR ) != 0 ) {
          printf( "  Descriptor Error\n" );
        }

        if ( ( p_enh[index].erx.des0_3.des0
               & DWMAC_DESC_ERX_DES0_OVERFLOW_ERROR ) != 0 ) {
          printf( "  Overflow Error\n" );
        }

        if ( ( p_enh[index].erx.des0_3.des0
               &
               DWMAC_DESC_ERX_DES0_TIMESTAMP_AVAIL_OR_CHECKSUM_ERROR_OR_GIANT_FRAME )
             != 0 ) {
          printf( "  Giant Frame\n" );
        }

        if ( ( p_enh[index].erx.des0_3.des0
               & DWMAC_DESC_ERX_DES0_LATE_COLLISION ) != 0 ) {
          printf( "  Late Collision\n" );
        }

        if ( ( p_enh[index].erx.des0_3.des0
               & DWMAC_DESC_ERX_DES0_RECEIVE_WATCHDOG_TIMEOUT ) != 0
             || ( p_enh[index].erx.des0_3.des0
                  & DWMAC_DESC_ERX_DES0_RECEIVE_ERROR ) != 0 ) {
          printf( "  IP Header or IP Payload:\n" );

          if ( ( p_enh[index].erx.des0_3.des0
                 & DWMAC_DESC_ERX_DES0_RECEIVE_WATCHDOG_TIMEOUT ) != 0 ) {
            printf( "   Watchdog Timeout\n" );
          }

          if ( ( p_enh[index].erx.des0_3.des0
                 & DWMAC_DESC_ERX_DES0_RECEIVE_ERROR ) != 0 ) {
            printf( "   Receive Error\n" );
          }
        }

        if ( ( p_enh[index].erx.des0_3.des0 & DWMAC_DESC_ERX_DES0_CRC_ERROR )
             != 0 ) {
          printf( "  CRC Error\n" );
        }
      }

      printf( "des1\n" );
      printf(
        " %u Disable Interrupt on Completion\n"
        " %lu Receive Buffer 2 Size\n"
        " %u Receive End of Ring\n"
        " %u Second Addr. Chained\n"
        " %lu Receive Buffer 1 Size\n",
        ( p_enh[index].erx.des0_3.des1
          & DWMAC_DESC_ERX_DES1_DISABLE_IRQ_ON_COMPLETION ) != 0,
        DWMAC_DESC_ERX_DES1_RECEIVE_BUFF_2_SIZE_GET( p_enh[index].erx.des0_3.
                                                     des1 ),
        ( p_enh[index].erx.des0_3.des1 & DWMAC_DESC_ERX_DES1_RECEIVE_END_OF_RING ) != 0,
        ( p_enh[index].erx.des0_3.des1 & DWMAC_DESC_ERX_DES1_SECOND_ADDR_CHAINED ) != 0,
        DWMAC_DESC_ERX_DES1_RECEIVE_BUFF_1_SIZE_GET( p_enh[index].erx.des0_3.
                                                     des1 )
        );
      printf( "des2\n" );
      printf( " %p Buffer 1 Address\n", (void *) p_enh[index].erx.des0_3.des2 );
      printf( "des3\n" );
      printf( " %p Buffer 2 Address\n", (void *) p_enh[index].erx.des0_3.des3 );
    }
  }
}

static int dwmac_desc_enh_create_rx_desc( dwmac_common_context *self )
{
  int          eno        = 0;
  const size_t NUM_DESCS  = (size_t) self->bsd_config->rbuf_count;
  const size_t SIZE_DESCS = NUM_DESCS * sizeof( dwmac_desc_ext );
  void        *desc_mem   = NULL;


  assert( NULL == self->dma_rx );

  /* Allocate an array of mbuf pointers */
  self->mbuf_addr_rx = calloc( NUM_DESCS, sizeof( struct mbuf * ) );

  if ( self->mbuf_addr_rx == NULL ) {
    eno = ENOMEM;
  }

  /* Allocate an array of dma descriptors */
  if ( eno == 0 ) {
    eno = ( self->CFG->CALLBACK.mem_alloc_nocache )(
      self->arg,
      &desc_mem,
      SIZE_DESCS
      );
  }

  if ( eno == 0 ) {
    if ( desc_mem != NULL ) {
      memset( desc_mem, 0, SIZE_DESCS );
      DWMAC_COMMON_DSB();
    } else {
      eno = ENOMEM;
    }
  }

  if ( eno == 0 ) {
    self->dma_rx = (volatile dwmac_desc *) desc_mem;
    DWMAC_COMMON_DSB();
  }

  return eno;
}

static void dwmac_desc_enh_init_rx_desc(
  dwmac_common_context *self,
  const unsigned int    index )
{
  volatile dwmac_desc_ext *p_enh       =
    (volatile dwmac_desc_ext *) self->dma_rx;
  const size_t             NUM_DESCS   = (size_t) self->bsd_config->rbuf_count;
  char                    *clust_start =
    mtod( self->mbuf_addr_rx[index], char * );


  assert( NULL != p_enh );

  DWMAC_COMMON_DSB();

  rtems_cache_invalidate_multiple_data_lines(
    clust_start,
    DWMAC_DESC_COM_BUF_SIZE + ETHER_ALIGN
    );

  if ( self->mbuf_addr_rx[index] != NULL ) {
    p_enh[index].erx.des0_3.des1 = DWMAC_DESC_ERX_DES1_RECEIVE_BUFF_1_SIZE_SET(
      p_enh->erx.des0_3.des1,
      DWMAC_DESC_COM_BUF_SIZE );
  } else {
    p_enh[index].erx.des0_3.des1 = DWMAC_DESC_ERX_DES1_RECEIVE_BUFF_1_SIZE_SET(
      p_enh->erx.des0_3.des1,
      0 );
  }

  p_enh[index].erx.des0_3.des2 = (uint32_t) clust_start;

  /* The network controller supports adding a second data buffer to
   * p_enh->erx.des0_3.des3. For simplicity reasons we will not do this */
  dwmac_desc_enh_rx_set_on_ring_chain( &p_enh[index],
                                       ( index == NUM_DESCS - 1 ) );
  DWMAC_COMMON_DSB();
  p_enh[index].erx.des0_3.des0 = DWMAC_DESC_ERX_DES0_OWN_BIT;
}

static int dwmac_desc_enh_destroy_rx_desc( dwmac_common_context *self )
{
  int                  eno    = 0;
  volatile dwmac_desc *dma_rx = self->dma_rx;


  if ( self->mbuf_addr_rx != NULL ) {
    free( self->mbuf_addr_rx, 0 );
    self->mbuf_addr_rx = NULL;
  }

  if ( dma_rx != NULL ) {
    eno = self->CFG->CALLBACK.mem_free_nocache(
      self->arg,
      RTEMS_DEVOLATILE( void *, dma_rx )
    );
    self->dma_rx = NULL;
  }

  DWMAC_COMMON_DSB();

  return eno;
}

static void dwmac_desc_enh_release_rx_bufs( dwmac_common_context *self )
{
  volatile dwmac_desc_ext *p_enh     = (volatile dwmac_desc_ext *) self->dma_rx;
  const size_t             NUM_DESCS = (size_t) self->bsd_config->rbuf_count;
  unsigned int             i;


  assert( p_enh != NULL );

  for ( i = 0; i < NUM_DESCS; ++i ) {
    if ( p_enh[i].erx.des0_3.des2 != 0 ) {
      struct mbuf *dummy;

      assert( self->mbuf_addr_rx[i] != NULL );

      MFREE( self->mbuf_addr_rx[i], dummy );
      (void) dummy;
      memset(
        RTEMS_DEVOLATILE( void *, &p_enh[i].erx ),
        0,
        sizeof( dwmac_desc_ext )
      );
    }
  }

  self->dma_rx = (volatile dwmac_desc *) p_enh;
  DWMAC_COMMON_DSB();
}

static int dwmac_desc_enh_create_tx_desc( dwmac_common_context *self )
{
  int          eno        = 0;
  void        *mem_desc   = NULL;
  const size_t NUM_DESCS  = (size_t) self->bsd_config->xbuf_count;
  const size_t SIZE_DESCS = NUM_DESCS * sizeof( dwmac_desc_ext );


  assert( self->dma_tx == NULL );

  /* Allocate an array of mbuf pointers */
  self->mbuf_addr_tx = calloc( NUM_DESCS, sizeof( struct mbuf * ) );

  if ( self->mbuf_addr_tx == NULL ) {
    eno = ENOMEM;
  }

  if ( eno == 0 ) {
    eno = ( self->CFG->CALLBACK.mem_alloc_nocache )(
      self->arg,
      &mem_desc,
      SIZE_DESCS
      );
  }

  if ( eno == 0 ) {
    if ( mem_desc != NULL ) {
      memset( mem_desc, 0, SIZE_DESCS );
      DWMAC_COMMON_DSB();
    } else {
      eno = ENOMEM;
    }
  }

  if ( eno == 0 ) {
    self->dma_tx = mem_desc;
    DWMAC_COMMON_DSB();
  }

  return eno;
}

static void dwmac_desc_enh_init_tx_desc( dwmac_common_context *self )
{
  volatile dwmac_desc_ext *p_enh     = (volatile dwmac_desc_ext *) self->dma_tx;
  const size_t             NUM_DESCS = (size_t) self->bsd_config->xbuf_count;
  unsigned int             i;


  assert( p_enh != NULL );

  for ( i = 0; i < NUM_DESCS; ++i ) {
    dwmac_desc_enh_tx_set_on_ring_chain( &p_enh[i], ( i == NUM_DESCS - 1 ) );
  }

  self->dma_tx = (volatile dwmac_desc *) &p_enh[0];
  DWMAC_COMMON_DSB();
}

static int dwmac_desc_enh_destroy_tx_desc( dwmac_common_context *self )
{
  int   eno      = 0;
  void *mem_desc = RTEMS_DEVOLATILE( void *, self->dma_tx );


  if ( self->mbuf_addr_tx != NULL ) {
    free( self->mbuf_addr_tx, 0 );
    self->mbuf_addr_tx = NULL;
  }

  if ( mem_desc != NULL ) {
    eno          = self->CFG->CALLBACK.mem_free_nocache( self->arg, mem_desc );
    mem_desc     = NULL;
    self->dma_tx = (volatile dwmac_desc *) mem_desc;
  }

  DWMAC_COMMON_DSB();

  return eno;
}

static void dwmac_desc_enh_release_tx_bufs( dwmac_common_context *self )
{
  volatile dwmac_desc_ext *p_enh     = (volatile dwmac_desc_ext *) self->dma_tx;
  const size_t             NUM_DESCS = (size_t) self->bsd_config->xbuf_count;
  unsigned int             i;


  assert( p_enh != NULL );

  for ( i = 0; i < NUM_DESCS; ++i ) {
    if ( p_enh[i].etx.des0_3.des1 != 0 ) {
      struct mbuf *dummy;

      assert( self->mbuf_addr_tx[i] != NULL );

      MFREE( self->mbuf_addr_tx[i], dummy );
      (void) dummy;
      memset( RTEMS_DEVOLATILE( void *,
                            &p_enh[i].etx ), 0, sizeof( dwmac_desc_ext ) );
    }
  }

  self->dma_tx = (volatile dwmac_desc *) p_enh;
  DWMAC_COMMON_DSB();
}

static inline size_t dwmac_desc_enh_get_rx_frame_len(
  dwmac_common_context *self,
  const unsigned int    desc_idx )
{
  volatile dwmac_desc_ext *p_enh = (volatile dwmac_desc_ext *) self->dma_rx;


  /* The type-1 checksum offload engines append the checksum at
   * the end of frame and the two bytes of checksum are added in
   * the length.
   * Adjust for that in the framelen for type-1 checksum offload
   * engines. */
  if ( self->dmagrp->hw_feature & DMAGRP_HW_FEATURE_RXTYP1COE ) {
    return DWMAC_DESC_ERX_DES0_FRAME_LENGTH_GET( p_enh[desc_idx].erx.des0_3.des0 )
           - 2U;
  } else {
    return DWMAC_DESC_ERX_DES0_FRAME_LENGTH_GET( p_enh[desc_idx].erx.des0_3.des0 );
  }
}

static bool dwmac_desc_enh_am_i_rx_owner(
  dwmac_common_context *self,
  const unsigned int    desc_idx )
{
  volatile dwmac_desc_ext *p_enh = (volatile dwmac_desc_ext *) self->dma_rx;
  bool                     am_i_owner;


  DWMAC_COMMON_DSB();
  am_i_owner =
    ( p_enh[desc_idx].erx.des0_3.des0 & DWMAC_DESC_ERX_DES0_OWN_BIT ) == 0;

  return am_i_owner;
}

static bool dwmac_desc_enh_am_i_tx_owner(
  dwmac_common_context *self,
  const unsigned int    idx_tx )
{
  volatile dwmac_desc_ext *p_enh = (volatile dwmac_desc_ext *) self->dma_tx;
  bool                     am_i_owner;


  DWMAC_COMMON_DSB();
  am_i_owner =
    ( p_enh[idx_tx].etx.des0_3.des0 & DWMAC_DESC_ETX_DES0_OWN_BIT ) == 0;

  return am_i_owner;
}

static void dwmac_desc_enh_release_tx_ownership(
  dwmac_common_context *self,
  const unsigned int    idx_tx )
{
  volatile dwmac_desc_ext *p_enh = (volatile dwmac_desc_ext *) self->dma_tx;


  DWMAC_COMMON_DSB();
  p_enh[idx_tx].erx.des0_3.des0 |= DWMAC_DESC_ETX_DES0_OWN_BIT;
}

static int dwmac_desc_enh_get_tx_ls(
  dwmac_common_context *self,
  const unsigned int    idx_tx )
{
  volatile dwmac_desc_ext *p_enh = (volatile dwmac_desc_ext *) self->dma_tx;


  return ( ( p_enh[idx_tx].etx.des0_3.des0
             & DWMAC_DESC_ETX_DES0_LAST_SEGMENT ) != 0 );
}

static void dwmac_desc_enh_release_tx_desc(
  dwmac_common_context *self,
  const unsigned int    idx_tx )
{
  volatile dwmac_desc_ext *p_enh = (volatile dwmac_desc_ext *) self->dma_tx;


  p_enh[idx_tx].etx.des0_3.des0 =
    p_enh[idx_tx].etx.des0_3.des0
    & ( DWMAC_DESC_ETX_DES0_TRANSMIT_END_OF_RING
        | DWMAC_DESC_ETX_DES0_SECOND_ADDR_CHAINED );

  p_enh[idx_tx].etx.des0_3.des1 = 0;
}

static void dwmac_desc_enh_prepare_tx_desc(
  dwmac_common_context *self,
  const unsigned int    idx,
  const bool            is_first,
  const size_t          len,
  const void           *pdata )
{
  volatile dwmac_desc_ext *p_enh = (volatile dwmac_desc_ext *) self->dma_tx;


  if ( is_first ) {
    p_enh[idx].etx.des0_3.des0 |= DWMAC_DESC_ETX_DES0_FIRST_SEGMENT;
  }

  dwmac_desc_enh_set_tx_desc_len( &p_enh[idx], len );

  p_enh[idx].etx.des0_3.des2 = (uintptr_t) pdata;
}

static void dwmac_desc_enh_close_tx_desc(
  dwmac_common_context *self,
  const unsigned int    idx_tx
)
{
  volatile dwmac_desc_ext *p_enh = (volatile dwmac_desc_ext *) self->dma_tx;

  p_enh[idx_tx].etx.des0_3.des0 |= DWMAC_DESC_ETX_DES0_LAST_SEGMENT;
  p_enh[idx_tx].etx.des0_3.des0 |= DWMAC_DESC_ETX_DES0_IRQ_ON_COMPLETION;
}

static bool dwmac_desc_enh_is_first_rx_segment(
  dwmac_common_context *self,
  const unsigned int    descriptor_index )
{
  volatile dwmac_desc_ext *p_descs = (volatile dwmac_desc_ext *) self->dma_rx;


  return ( ( p_descs[descriptor_index].erx.des0_3.des0
             & DWMAC_DESC_ERX_DES0_FIRST_DESCRIPTOR ) != 0 );
}

static bool dwmac_desc_enh_is_last_rx_segment(
  dwmac_common_context *self,
  const unsigned int    descriptor_index )
{
  volatile dwmac_desc_ext *p_descs = (volatile dwmac_desc_ext *) self->dma_rx;


  return ( ( p_descs[descriptor_index].erx.des0_3.des0
             & DWMAC_DESC_ERX_DES0_LAST_DESCRIPTOR ) != 0 );
}

static int dwmac_desc_enh_validate( dwmac_common_context *self )
{
  /* Does the hardware support enhanced descriptors? */
  if ( ( self->dmagrp->hw_feature & DMAGRP_HW_FEATURE_ENHDESSEL ) != 0 ) {
    return 0;
  } else {
    return EINVAL;
  }
}

static bool dwmac_desc_enh_use_enhanced_descs( dwmac_common_context *self )
{
  (void) self;

  /* Yes, we use enhanced descriptors */
  return true;
}

const dwmac_common_desc_ops dwmac_desc_ops_enhanced = {
  .validate             = dwmac_desc_enh_validate,
  .use_enhanced_descs   = dwmac_desc_enh_use_enhanced_descs,
  .tx_status            = dwmac_desc_enh_get_tx_status,
  .rx_status            = dwmac_desc_enh_get_rx_status,
  .create_rx_desc       = dwmac_desc_enh_create_rx_desc,
  .create_tx_desc       = dwmac_desc_enh_create_tx_desc,
  .destroy_rx_desc      = dwmac_desc_enh_destroy_rx_desc,
  .destroy_tx_desc      = dwmac_desc_enh_destroy_tx_desc,
  .init_rx_desc         = dwmac_desc_enh_init_rx_desc,
  .init_tx_desc         = dwmac_desc_enh_init_tx_desc,
  .release_rx_bufs      = dwmac_desc_enh_release_rx_bufs,
  .release_tx_bufs      = dwmac_desc_enh_release_tx_bufs,
  .alloc_data_buf       = dwmac_desc_com_new_mbuf,
  .am_i_tx_owner        = dwmac_desc_enh_am_i_tx_owner,
  .am_i_rx_owner        = dwmac_desc_enh_am_i_rx_owner,
  .release_tx_desc      = dwmac_desc_enh_release_tx_desc,
  .prepare_tx_desc      = dwmac_desc_enh_prepare_tx_desc,
  .close_tx_desc        = dwmac_desc_enh_close_tx_desc,
  .get_tx_ls            = dwmac_desc_enh_get_tx_ls,
  .release_tx_ownership = dwmac_desc_enh_release_tx_ownership,
  .get_rx_frame_len     = dwmac_desc_enh_get_rx_frame_len,
  .is_first_rx_segment  = dwmac_desc_enh_is_first_rx_segment,
  .is_last_rx_segment   = dwmac_desc_enh_is_last_rx_segment,
  .print_tx_desc        = dwmac_desc_enh_print_tx_desc,
  .print_rx_desc        = dwmac_desc_enh_print_rx_desc,
};

/* This wrapped function pointer struct can be passed into the
 * configuration initializer for the driver */
const dwmac_descriptor_ops DWMAC_DESCRIPTOR_OPS_ENHANCED =
  DWMAC_DESCRIPTOR_OPS_INITIALIZER(
    &dwmac_desc_ops_enhanced
    );
