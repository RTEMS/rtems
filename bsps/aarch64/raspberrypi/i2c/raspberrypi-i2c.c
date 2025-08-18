/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup raspberrypi_4_i2c
 *
 * @brief I2C Driver
 */

/*
 * Copyright (C) 2025 Shaunak Datar
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <bsp/irq.h>
#include <bsp/raspberrypi-i2c.h>
#include <bsp/raspberrypi.h>
#include <bsp/rpi-gpio.h>
#include <dev/i2c/i2c.h>

#define C_REG( bus ) BCM2835_REG( ( bus )->base_address + BCM2711_I2C_CONTROL )
#define C_I2CEN ( 1 << 15 )
#define C_INTR ( 1 << 10 )
#define C_INTT ( 1 << 9 )
#define C_INTD ( 1 << 8 )
#define C_ST ( 1 << 7 )
#define C_CLEAR ( 1 << 5 )
#define C_READ ( 1 << 0 )

#define BSC_CORE_CLK_HZ 150000000

#define S_REG( bus ) BCM2835_REG( ( bus )->base_address + BCM2711_I2C_STATUS )
#define S_CLKT ( 1 << 9 )
#define S_ERR ( 1 << 8 )
#define S_RXF ( 1 << 7 )
#define S_TXE ( 1 << 6 )
#define S_RXD ( 1 << 5 )
#define S_TXD ( 1 << 4 )
#define S_RXR ( 1 << 3 )
#define S_TXW ( 1 << 2 )
#define S_DONE ( 1 << 1 )
#define S_TA ( 1 << 0 )
#define S_ERROR ( S_CLKT | S_ERR | S_DONE )

typedef struct {
  i2c_bus                 base;
  rtems_binary_semaphore  sem;
  uint32_t                input_clock;
  uintptr_t               base_address;
  raspberrypi_bsc_masters device;
  uint32_t                remaining_bytes;
  uint32_t                remaining_transfers;
  uint8_t                *current_buffer;
  uint32_t                current_buffer_size;
  bool                    read_transfer;
} raspberrypi_i2c_bus;

static int rpi_i2c_bus_transfer( raspberrypi_i2c_bus *bus )
{
  while ( bus->remaining_bytes > 0 ) {
    if ( bus->read_transfer ) {
      while ( ( S_REG( bus ) & ( S_RXD | S_CLKT ) ) == 0 ) {
      }
      if ( S_REG( bus ) & S_CLKT ) {
        return -EIO;
      }
      *bus->current_buffer = BCM2835_REG(
                               bus->base_address + BCM2711_I2C_FIFO
                             ) &
                             BCM2711_I2C_FIFO_MASK;

      ++bus->current_buffer;
      if ( ( S_REG( bus ) & S_ERR ) || ( S_REG( bus ) & S_CLKT ) ) {
        return -EIO;
      }
    } else {
#ifdef BSP_I2C_USE_INTERRUPTS
      C_REG( bus ) |= C_INTT;

      if ( rtems_binary_semaphore_wait_timed_ticks(
             &bus->sem,
             bus->base.timeout
           ) != RTEMS_SUCCESSFUL ) {
        rtems_binary_semaphore_try_wait( &bus->sem );
        return -ETIMEDOUT;
      }
#else
      while ( ( S_REG( bus ) & ( S_TXW | S_CLKT ) ) == 0 ) {
      }
      if ( S_REG( bus ) & S_CLKT ) {
        return -EIO;
      }
#endif
      BCM2835_REG(
        bus->base_address + BCM2711_I2C_FIFO
      ) = *bus->current_buffer;

      ++bus->current_buffer;
      if ( ( S_REG( bus ) & S_ERR ) || ( S_REG( bus ) & S_CLKT ) ) {
        return -EIO;
      }
    }
    --bus->remaining_bytes;
  }

  return 0;
}

static void rpi_i2c_destroy( i2c_bus *base )
{
  raspberrypi_i2c_bus *bus = (raspberrypi_i2c_bus *) base;

  i2c_bus_destroy_and_free( &bus->base );
}

static int rpi_i2c_set_clock( i2c_bus *base, unsigned long clock )
{
  raspberrypi_i2c_bus *bus = (raspberrypi_i2c_bus *) base;
  uint32_t             clock_rate;
  uint16_t             divider;

  divider = BSC_CORE_CLK_HZ / clock;

  clock_rate = BSC_CORE_CLK_HZ / divider;

  while ( clock_rate > clock ) {
    ++divider;
    clock_rate = BSC_CORE_CLK_HZ / divider;
  }

  BCM2835_REG( bus->base_address + BCM2711_I2C_DIV ) = divider;

  return 0;
}

static int rpi_i2c_setup_and_transfer( raspberrypi_i2c_bus *bus )
{
  int rv;
  while ( bus->remaining_transfers > 0 ) {
    bus->remaining_bytes = bus->remaining_transfers > 1 ?
                             BCM2711_I2C_DLEN_MASK :
                             ( bus->current_buffer_size & BCM2711_I2C_DLEN_MASK
                             );
    BCM2835_REG( bus->base_address + BCM2711_I2C_DLEN )  = bus->remaining_bytes;
    /* Clear the error bits before starting new transfer */
    S_REG( bus )                                         = S_ERROR;
    C_REG( bus )                                        |= C_ST;

    rv = rpi_i2c_bus_transfer( bus );

    if ( rv < 0 ) {
      return rv;
    }

#ifdef BSP_I2C_USE_INTERRUPTS
    C_REG( bus ) |= C_INTD;
    if ( rtems_binary_semaphore_wait_timed_ticks(
           &bus->sem,
           bus->base.timeout
         ) != 0 ) {
      rtems_binary_semaphore_try_wait( &bus->sem );
      return -ETIMEDOUT;
    }
#else
    while ( ( S_REG( bus ) & ( S_DONE | S_CLKT ) ) == 0 ) {
    }
    if ( S_REG( bus ) & S_CLKT ) {
      return -EIO;
    }
#endif

    --bus->remaining_transfers;
  }
  return 0;
}

#ifdef BSP_I2C_USE_INTERRUPTS
static void i2c_handler( void *args )
{
  raspberrypi_i2c_bus *bus = (raspberrypi_i2c_bus *) args;
  if ( C_REG( bus ) & C_INTT ) {
    C_REG( bus ) &= ~C_INTT;
  } else if ( C_REG( bus ) & C_INTD ) {
    C_REG( bus ) &= ~C_INTD;
  }
  rtems_binary_semaphore_post( &bus->sem );
}
#endif

static int rpi_i2c_transfer( i2c_bus *base, i2c_msg *msgs, uint32_t msg_count )
{
  raspberrypi_i2c_bus *bus = (raspberrypi_i2c_bus *) base;
  int                  rv  = 0;
  uint32_t             i;
  uint8_t              msbs;
  int                  supported_flags = I2C_M_TEN | I2C_M_RD;

  for ( i = 0; i < msg_count; i++ ) {
    if ( msgs[ i ].len == 0 || msgs[ i ].buf == NULL ) {
      return -EINVAL;
    }

    if ( ( msgs[ i ].flags & ~supported_flags ) != 0 ) {
      return -EINVAL;
    }
  }

  for ( i = 0; i < msg_count; i++ ) {
    bus->current_buffer      = msgs[ i ].buf;
    bus->current_buffer_size = msgs[ i ].len;
    bus->remaining_transfers = ( bus->current_buffer_size +
                                 ( BCM2711_I2C_DLEN_MASK - 1 ) ) /
                               BCM2711_I2C_DLEN_MASK;

    /* 10-bit slave address */
    if ( msgs[ i ].flags & I2C_M_TEN ) {
      /* Add the 8 lsbs of the 10-bit slave address to the fifo register */
      BCM2835_REG(
        bus->base_address + BCM2711_I2C_FIFO
      ) = msgs[ i ].addr & BCM2711_I2C_FIFO_MASK;

      msbs = msgs[ i ].addr >> 8;
      BCM2835_REG(
        bus->base_address + BCM2711_I2C_SLAVE_ADDRESS
      ) = BCM2711_10_BIT_ADDR_MASK | msbs;

    } else {
      BCM2835_REG(
        bus->base_address + BCM2711_I2C_SLAVE_ADDRESS
      ) = msgs[ i ].addr;
    }

    if ( msgs[ i ].flags & I2C_M_RD ) {
      C_REG( bus )       |= C_CLEAR | C_READ;
      bus->read_transfer  = true;
    } else {
      C_REG( bus )       |= C_CLEAR;
      C_REG( bus )       &= ~C_READ;
      bus->read_transfer  = false;
    }
    /* Disable clock stretch timeout */
    BCM2835_REG( bus->base_address + BCM2711_I2C_CLKT ) = 0;

    rv = rpi_i2c_setup_and_transfer( bus );
    if ( rv < 0 ) {
      return rv;
    }
  }

  return rv;
}

static rtems_status_code rpi_i2c_gpio_init(
  raspberrypi_bsc_masters device,
  raspberrypi_i2c_bus    *bus
)
{
  switch ( device ) {
    case raspberrypi_bscm0:
      raspberrypi_gpio_set_function( 0, GPIO_AF0 );
      raspberrypi_gpio_set_function( 1, GPIO_AF0 );
      bus->base_address = BCM2711_I2C0_BASE;
      break;
    case raspberrypi_bscm1:
      raspberrypi_gpio_set_function( 2, GPIO_AF0 );
      raspberrypi_gpio_set_function( 3, GPIO_AF0 );
      bus->base_address = BCM2711_I2C1_BASE;
      break;
    case raspberrypi_bscm3:
      raspberrypi_gpio_set_function( 4, GPIO_AF5 );
      raspberrypi_gpio_set_function( 5, GPIO_AF5 );
      bus->base_address = BCM2711_I2C3_BASE;
      break;
    case raspberrypi_bscm4:
      raspberrypi_gpio_set_function( 6, GPIO_AF5 );
      raspberrypi_gpio_set_function( 7, GPIO_AF5 );
      bus->base_address = BCM2711_I2C4_BASE;
      break;
    case raspberrypi_bscm5:
      raspberrypi_gpio_set_function( 10, GPIO_AF5 );
      raspberrypi_gpio_set_function( 11, GPIO_AF5 );
      bus->base_address = BCM2711_I2C5_BASE;
      break;
    case raspberrypi_bscm6:
      raspberrypi_gpio_set_function( 22, GPIO_AF5 );
      raspberrypi_gpio_set_function( 23, GPIO_AF5 );
      bus->base_address = BCM2711_I2C6_BASE;
      break;
    default:
      return RTEMS_INVALID_ADDRESS;
  }
  return RTEMS_SUCCESSFUL;
}

static char *rpi_select_bus( raspberrypi_bsc_masters device )
{
  switch ( device ) {
    case raspberrypi_bscm0:
      return "/dev/i2c-0";
    case raspberrypi_bscm1:
      return "/dev/i2c-1";
    case raspberrypi_bscm3:
      return "/dev/i2c-3";
    case raspberrypi_bscm4:
      return "/dev/i2c-4";
    case raspberrypi_bscm5:
      return "/dev/i2c-5";
    case raspberrypi_bscm6:
      return "/dev/i2c-6";
    default:
      return NULL;
  }
}

rtems_status_code rpi_i2c_init(
  raspberrypi_bsc_masters device,
  uint32_t                bus_clock
)
{
  raspberrypi_i2c_bus *bus;
  rtems_status_code    sc;
  const char          *bus_path;

  if ( device != raspberrypi_bscm0 && device != raspberrypi_bscm1 &&
       device != raspberrypi_bscm3 && device != raspberrypi_bscm4 &&
       device != raspberrypi_bscm5 && device != raspberrypi_bscm6 ) {
    return RTEMS_INVALID_NUMBER;
  }

  bus_path = rpi_select_bus( device );
  if ( bus_path == NULL ) {
    return RTEMS_INVALID_NUMBER;
  }

  bus = (raspberrypi_i2c_bus *) i2c_bus_alloc_and_init( sizeof( *bus ) );
  if ( bus == NULL ) {
    return RTEMS_NO_MEMORY;
  }

  sc = rpi_i2c_gpio_init( device, bus );
  if ( sc != RTEMS_SUCCESSFUL ) {
    i2c_bus_destroy_and_free( &bus->base );
    return sc;
  }

  /* Enable I2C */
  C_REG( bus ) = C_CLEAR;
  C_REG( bus ) = C_I2CEN;

#ifdef BSP_I2C_USE_INTERRUPTS
  sc = rtems_interrupt_handler_install(
    BCM2711_IRQ_I2C,
    "I2C",
    RTEMS_INTERRUPT_SHARED,
    (rtems_interrupt_handler) i2c_handler,
    bus
  );
  rtems_binary_semaphore_init( &bus->sem, "RPII2C" );
  if ( sc != RTEMS_SUCCESSFUL ) {
    return -EIO;
  }
#endif

  sc = rpi_i2c_set_clock( &bus->base, bus_clock );
  if ( sc != RTEMS_SUCCESSFUL ) {
    i2c_bus_destroy_and_free( &bus->base );
    return sc;
  }

  bus->base.transfer      = rpi_i2c_transfer;
  bus->base.set_clock     = rpi_i2c_set_clock;
  bus->base.destroy       = rpi_i2c_destroy;
  bus->base.functionality = I2C_FUNC_I2C | I2C_FUNC_10BIT_ADDR;

  return i2c_bus_register( &bus->base, bus_path );
}