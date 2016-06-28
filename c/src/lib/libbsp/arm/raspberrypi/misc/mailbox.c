/**
 * @file
 *
 * @ingroup raspberrypi
 *
 * @brief mailbox support.
 */
/*
 * Copyright (c) 2015 Yang Qiao
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.org/license/LICENSE
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <bsp.h>
#include <bsp/raspberrypi.h>
#include <bsp/mailbox.h>

#define BCM2835_MBOX_DATA_MASK( data ) ( data & 0xFFFFFFF0U )
#define BCM2835_MBOX_CHANNEL_MASK( data ) ( data & 0xFU )

static inline bool bcm2835_mailbox_is_empty( void )
{
  return ( BCM2835_REG( BCM2835_MBOX_STATUS ) & BCM2835_MBOX_EMPTY );
}

static inline bool bcm2835_mailbox_is_full( void )
{
  return ( BCM2835_REG( BCM2835_MBOX_STATUS ) & BCM2835_MBOX_FULL );
}

unsigned int raspberrypi_mailbox_read( unsigned int channel )
{
  unsigned int raw;
  unsigned int read_channel;

  while ( 1 ) {
    while ( bcm2835_mailbox_is_empty() ) ;

    raw = BCM2835_REG( BCM2835_MBOX_READ );
    read_channel = BCM2835_MBOX_CHANNEL_MASK( raw );

    if ( read_channel == channel )
      return BCM2835_MBOX_DATA_MASK( raw );
  }
}

void raspberrypi_mailbox_write(
  unsigned int channel,
  unsigned int data
)
{
  while ( bcm2835_mailbox_is_full() ) ;

  BCM2835_REG( BCM2835_MBOX_WRITE ) =
    BCM2835_MBOX_DATA_MASK( data ) |
    BCM2835_MBOX_CHANNEL_MASK( channel );
}
