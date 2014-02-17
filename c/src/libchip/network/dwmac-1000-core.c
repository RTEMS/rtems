/**
 * @file
 *
 * @brief DWMAC 1000 on-chip Ethernet controllers Core Handling
 *
 * Functions and data which are specific to the DWMAC 1000 Core Handling.
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
#include "dwmac-core.h"
#include "dwmac-common.h"

#define DWMAC_1000_CORE_DEBUG

//#undef DWMAC_1000_CORE_DEBUG
#ifdef DWMAC_1000_CORE_DEBUG
#define DWMAC_1000_CORE_PRINT_DBG( fmt, args ... )  printk( fmt, ## args )
#else
#define DWMAC_1000_CORE_PRINT_DBG( fmt, args ... )  do { } while ( 0 )
#endif

#define DWMAC_1000_CORE_INIT \
  ( \
    ( MACGRP_MAC_CONFIGURATION_JD \
      | MACGRP_MAC_CONFIGURATION_BE ) \
    & ~MACGRP_MAC_CONFIGURATION_PS \
  )

#define DWMAC_1000_CORE_HASH_TABLE_SIZE 256

static volatile uint32_t *dwmac_1000_core_get_mac_addr_low(
  dwmac_common_context *self,
  const unsigned int    mac_addr_index )
{
  volatile uint32_t *addr = NULL;

  assert( self != NULL );
  assert( mac_addr_index <= 127 );

  if ( mac_addr_index > 15 ) {
    addr = &self->macgrp->mac_addr16_127[mac_addr_index].low;
  } else {
    addr = &self->macgrp->mac_addr0_15[mac_addr_index].low;
  }

  return addr;
}

static volatile uint32_t *dwmac_1000_core_get_mac_addr_high(
  dwmac_common_context *self,
  const unsigned int    mac_addr_index )
{
  volatile uint32_t *addr = NULL;

  assert( self != NULL );
  assert( mac_addr_index <= 127 );

  if ( mac_addr_index > 15 ) {
    addr = &self->macgrp->mac_addr16_127[mac_addr_index].high;
  } else {
    addr = &self->macgrp->mac_addr0_15[mac_addr_index].high;
  }

  return addr;
}

static void dwmac_1000_core_init( dwmac_common_context *self )
{
  uint32_t value = self->macgrp->mac_configuration;

  value |= DWMAC_1000_CORE_INIT;

  if ( ( self->dmagrp->hw_feature & DMAGRP_HW_FEATURE_RXTYP1COE ) != 0
       || ( self->dmagrp->hw_feature & DMAGRP_HW_FEATURE_RXTYP2COE ) != 0 ) {
    /* Enable RX checksum calculation offload to hardware */
    value |= MACGRP_MAC_CONFIGURATION_IPC;
  }

  /* No Jumbo- or Giant frames. The network stack does not support them */
  value                          &= ~MACGRP_MAC_CONFIGURATION_JE;
  value                          &= ~MACGRP_MAC_CONFIGURATION_TWOKPE;
  self->macgrp->mac_configuration = value;

  /* Mask GMAC interrupts */
  self->macgrp->interrupt_mask =
    MACGRP_INTERRUPT_MASK_RGSMIIIM
    | MACGRP_INTERRUPT_MASK_PCSLCHGIM
    | MACGRP_INTERRUPT_MASK_PCSANCIM
    | MACGRP_INTERRUPT_MASK_TSIM;

  /* mask out interrupts because we don't handle them yet */
  self->macgrp->mmc_receive_interrupt_mask     = ( uint32_t ) ~0L;
  self->macgrp->mmc_transmit_interrupt_mask    = ( uint32_t ) ~0L;
  self->macgrp->mmc_ipc_receive_interrupt_mask = ( uint32_t ) ~0L;
}

static void dwmac_1000_core_set_umac_addr(
  dwmac_common_context *self,
  const uint8_t        *addr,
  unsigned int          reg_n )
{
  dwmac_core_set_mac_addr(
    addr,
    dwmac_1000_core_get_mac_addr_high( self, reg_n ),
    dwmac_1000_core_get_mac_addr_low( self, reg_n )
    );
}

static void dwmac_1000_core_set_hash_filter(
  dwmac_common_context *self,
  const bool            add,
  struct ifreq         *ifr )
{
  int            eno = 0;
  struct arpcom *ac  = &self->arpcom;

  if ( add ) {
    eno = ether_addmulti( ifr, ac );
  } else {
    eno = ether_delmulti( ifr, ac );
  }

  if ( eno == ENETRESET ) {
    struct ether_multistep step;
    struct ether_multi    *enm;
    unsigned int           num_multi = 0;
    unsigned int           index;

    ETHER_FIRST_MULTI( step, ac, enm );

    while ( enm != NULL ) {
      /* Find out how many multicast addresses we have to handle */
      uint64_t addrlo = 0;
      uint64_t addrhi = 0;

      memcpy( &addrlo, enm->enm_addrlo, ETHER_ADDR_LEN );
      memcpy( &addrhi, enm->enm_addrhi, ETHER_ADDR_LEN );
      num_multi += 1U + (uint32_t) ( addrhi - addrlo );
    }

    if ( num_multi > DWMAC_1000_CORE_HASH_TABLE_SIZE ) {
      /* Too many addresses to be hashed, Use the
       * pass all multi option instead */

      for ( index = 0; index < 8; ++index ) {
        self->macgrp->hash_table_reg[index] = 0xffffffff;
      }

      self->macgrp->mac_frame_filter |= MACGRP_MAC_FRAME_FILTER_PM;
    } else if ( num_multi > 0 ) {
      uint32_t hash_shadow[8] = {0, 0, 0, 0, 0, 0, 0, 0};
      ETHER_FIRST_MULTI( step, ac, enm );

      while ( enm != NULL ) {
        uint64_t addrlo = 0;
        uint64_t addrhi = 0;

        memcpy( &addrlo, enm->enm_addrlo, ETHER_ADDR_LEN );
        memcpy( &addrhi, enm->enm_addrhi, ETHER_ADDR_LEN );

        while ( addrlo <= addrhi ) {
          /* XXX: ether_crc32_le() does not work, why? */
          uint32_t crc = ether_crc32_be( (uint8_t *) &addrlo, ETHER_ADDR_LEN );

          /* The upper 8 bits of the bit reversed 32 bit CRC are used for hash filtering.
           * The most significant bits determine the register to be used and the
           * least significant five bits determine which bit to be set within the register */
          uint32_t index_reg = ( crc >> 29 ) & 0x7;
          uint32_t index_bit = ( crc >> 24 ) & 0x1f;

          hash_shadow[index_reg] |= 1U << index_bit;
          ++addrlo;
        }

        ETHER_NEXT_MULTI( step, enm );
      }

      for ( index = 0; index < 8; ++index ) {
        self->macgrp->hash_table_reg[index] = hash_shadow[index];
      }

      /* Hash filter for multicast */
      self->macgrp->mac_frame_filter |= MACGRP_MAC_FRAME_FILTER_HMC;
    } else {
      /* Set all hash registers to accect to accept no multicast packets */
      for ( index = 0; index < 8; ++index ) {
        self->macgrp->hash_table_reg[index] = 0x00000000;
      }

      /* Hash filter for multicast */
      self->macgrp->mac_frame_filter |= MACGRP_MAC_FRAME_FILTER_HMC;
    }

    DWMAC_1000_CORE_PRINT_DBG(
      "Frame Filter reg: 0x%08x\n",
      self->macgrp->mac_frame_filter
      );
    DWMAC_1000_CORE_PRINT_DBG(
      "Hash regs:\n"
      "0x%08x\n"
      "0x%08x\n"
      "0x%08x\n"
      "0x%08x\n"
      "0x%08x\n"
      "0x%08x\n"
      "0x%08x\n"
      "0x%08x\n",
      self->macgrp->hash_table_reg[0],
      self->macgrp->hash_table_reg[1],
      self->macgrp->hash_table_reg[2],
      self->macgrp->hash_table_reg[3],
      self->macgrp->hash_table_reg[4],
      self->macgrp->hash_table_reg[5],
      self->macgrp->hash_table_reg[6],
      self->macgrp->hash_table_reg[7]
      );
  }
}

const dwmac_common_core_ops dwmac_core_ops_1000 = {
  .core_init       = dwmac_1000_core_init,
  .set_hash_filter = dwmac_1000_core_set_hash_filter,
  .set_umac_addr   = dwmac_1000_core_set_umac_addr,
};
