/**
 * @file
 *
 * @ingroup raspberrypi_i2c
 *
 * @brief Support for the I2C bus on the Raspberry Pi GPIO P1 header (model A/B)
 *        and GPIO J8 header on model B+.
 */

/*
 *  Copyright (c) 2014-2015 Andre Marques <andre.lousa.marques at gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

/*
 * STATUS:
 * - 10-bit slave addressing untested
 */

#include <bsp.h>
#include <bsp/raspberrypi.h>
#include <bsp/gpio.h>
#include <bsp/rpi-gpio.h>
#include <bsp/irq.h>
#include <bsp/i2c.h>
#include <assert.h>

#define TRANSFER_COUNT(buffer_size) (buffer_size + 0xFFFE) / 0xFFFF

#define ADJUST_TRANSFER_SIZE(transfer_count, remaining_bytes) \
  transfer_count > 1 ? 0xFFFF : (remaining_bytes & 0xFFFF)

#define I2C_POLLING(condition)                  \
  while ( condition ) {                         \
    ;                                           \
  }

/**
 * @brief Object containing relevant information about an I2C bus.
 *
 * Encapsulates relevant data for a I2C bus transfer.
 */
typedef struct
{
  i2c_bus base;
  uint32_t input_clock;
  rtems_id task_id;

  /* Remaining bytes to read/write on the current bus transfer. */
  uint32_t remaining_bytes;
  /* Each transfer has a limit of 0xFFFF bytes, hence larger transfers
   * have to be divided. Each transfer implies a stop condition, signaled
   * automatically by the BSC controller. */
  uint32_t remaining_transfers;

  uint8_t *current_buffer;
  uint32_t current_buffer_size;

  bool read_transfer;
} rpi_i2c_bus;

static int rpi_i2c_bus_transfer(rpi_i2c_bus *bus)
{
  while ( bus->remaining_bytes >= 1 ) {
    /* If reading. */
    if ( bus->read_transfer ) {
      /* Poll RXD bit until there is data on the RX FIFO to read. */
      I2C_POLLING((BCM2835_REG(BCM2835_I2C_S) & (1 << 5)) == 0);

      /* Read data from the RX FIFO. */
      (*(uint8_t *) bus->current_buffer) = BCM2835_REG(BCM2835_I2C_FIFO) & 0xFF;

      ++bus->current_buffer;

      /* Check for acknowledgment or clock stretching errors. */
      if (
          (BCM2835_REG(BCM2835_I2C_S) & (1 << 8)) ||
          (BCM2835_REG(BCM2835_I2C_S) & (1 << 9))
      ) {
        return -EIO;
      }
    }
    /* If writing. */
    else {
      /* If using the I2C bus in interrupt-driven mode. */
#if I2C_IO_MODE == 1
      /* Generate interrupts on the TXW bit condition. */
      BCM2835_REG(BCM2835_I2C_C) |= (1 << 9);

      /* Sleep until the TX FIFO has free space for a new write. */
      bus->task_id = rtems_task_self();
      if (
          rtems_event_transient_receive(RTEMS_WAIT, bus->base.timeout) !=
          RTEMS_SUCCESSFUL
      ) {
        rtems_event_transient_clear();

        return -ETIMEDOUT;
      }

      /* If using the bus in polling mode. */
#else
      /* Poll TXW bit until there is space available to write. */
      I2C_POLLING((BCM2835_REG(BCM2835_I2C_S) & (1 << 2)) == 0);
#endif

      /* Write data to the TX FIFO. */
      BCM2835_REG(BCM2835_I2C_FIFO) = (*(uint8_t *) bus->current_buffer);

      ++bus->current_buffer;

      /* Check for acknowledgment or clock stretching errors. */
      if (
          (BCM2835_REG(BCM2835_I2C_S) & (1 << 8)) ||
          (BCM2835_REG(BCM2835_I2C_S) & (1 << 9))
      ) {
        return -EIO;
      }
    }

    --bus->remaining_bytes;
    --bus->current_buffer_size;
  }

  return 0;
}

static int rpi_i2c_setup_transfer(rpi_i2c_bus *bus)
{
  int rv;

  while ( bus->remaining_transfers > 0 ) {
    /* Setup the byte size of the current transfer. */
    bus->remaining_bytes = ADJUST_TRANSFER_SIZE(
                             bus->remaining_transfers,
                             bus->current_buffer_size
                           );

    /* Set the DLEN register, which specifies how many data packets
     * will be transferred. */
    BCM2835_REG(BCM2835_I2C_DLEN) = bus->remaining_bytes;

    /* Clear the acknowledgment and clock stretching error status. */
    BCM2835_REG(BCM2835_I2C_S) |= (3 << 8);

    /* Send start bit. */
    BCM2835_REG(BCM2835_I2C_C) |= (1 << 7);

    /* Check for an acknowledgment error. */
    if ( (BCM2835_REG(BCM2835_I2C_S) & (1 << 8)) != 0 ) {
      return -EIO;
    }

    rv = rpi_i2c_bus_transfer(bus);

    if ( rv < 0 ) {
      return rv;
    }

    /* Wait for the current transfer to finish. */

    /* If using the I2C bus in interrupt-driven mode. */
#if I2C_IO_MODE == 1
    /* Generate interrupts on the DONE bit condition. */
    BCM2835_REG(BCM2835_I2C_C) |= (1 << 8);

    if (
        rtems_event_transient_receive(RTEMS_WAIT, bus->base.timeout) !=
        RTEMS_SUCCESSFUL
    ) {
      rtems_event_transient_clear();

      return -ETIMEDOUT;
    }
    /* If using the bus in polling mode. */
#else
    /* Poll DONE bit until all data has been sent. */
    I2C_POLLING((BCM2835_REG(BCM2835_I2C_S) & (1 << 1)) == 0);
#endif

    --bus->remaining_transfers;
  }

  return 0;
}

/* Handler function that is called on any I2C interrupt.
 *
 * There are 3 situations that can generate an interrupt:
 *
 * 1. Transfer (read/write) complete;
 * 2. The TX FIFO has space for more data (during a write transfer);
 * 3. The RX FIFO is full.
 *
 * Because the I2C FIFO has a 16 byte size, the 3. situation is not
 * as useful to many applications as knowing that at least 1 byte can
 * be read from the RX FIFO. For that reason this information is
 * got through polling the RXD bit even in interrupt-driven mode.
 *
 * This leaves only 2 interrupts to be caught. At any given time
 * when no I2C bus transfer is taking place no I2C interrupts are
 * generated, and they do they are only enabled one at a time:
 *
 * - When trying to write, the 2. interrupt is enabled to signal that
 *   data can be written on the TX FIFO, avoiding data loss in case
 *   it is full. When caught the handler disables that interrupt from
 *   being generated and sends a waking event to the transfer task,
 *   which will allow the transfer process to continue
 *   (by writing to the TX FIFO);
 *
 * - When the transfer is done on the Raspberry side, the 1. interrupt is
 *   enabled for the device to signal it has finished the transfer as
 *   well. When caught the handler disables that interrupt from being
 *   generated and sends a waking event to the transfer task, marking
 *   the end of the transfer.
 */
#if I2C_IO_MODE == 1
static void i2c_handler(void *arg)
{
  rpi_i2c_bus *bus = (rpi_i2c_bus *) arg;

  /* If the current enabled interrupt is on the TXW condition, disable it. */
  if ( (BCM2835_REG(BCM2835_I2C_C) & (1 << 9)) ) {
    BCM2835_REG(BCM2835_I2C_C) &= ~(1 << 9);
  }
  /* If the current enabled interrupt is on the DONE condition, disable it. */
  else if ( (BCM2835_REG(BCM2835_I2C_C) & (1 << 8)) ) {
    BCM2835_REG(BCM2835_I2C_C) &= ~(1 << 8);
  }

  /* Allow the transfer process to continue. */
  rtems_event_transient_send(bus->task_id);
}
#endif

static int rpi_i2c_transfer(i2c_bus *base, i2c_msg *msgs, uint32_t msg_count)
{
  rpi_i2c_bus *bus = (rpi_i2c_bus *) base;
  int rv = 0;
  uint32_t i;

  /* Perform an initial parse through the messages for the I2C_M_RECV_LEN flag,
   * which the Pi seems to not support and the I2C framework expects the bus
   * to provide as part of the I2C_FUNC_I2C functionality.
   *
   * It states that the slave device sends an initial byte containing the size
   * of the transfer, and for this to work the Pi will likely require two
   * transfers, with a stop-start condition in-between. */
  for ( i = 0; i < msg_count; ++i ) {
    if ( msgs[i].flags & I2C_M_RECV_LEN ) {
      return -EINVAL;
    }
  }

  for ( i = 0; i < msg_count; ++i ) {
    /* Clear FIFOs. */
    BCM2835_REG(BCM2835_I2C_C) |= (3 << 4);

    /* Setup transfer. */
    bus->current_buffer = msgs[i].buf;
    bus->current_buffer_size = msgs[i].len;
    bus->remaining_transfers = TRANSFER_COUNT(bus->current_buffer_size);

    /* If the slave uses 10-bit addressing. */
    if ( msgs[i].flags & I2C_M_TEN ) {
      /* Write the 8 least-significative bits of the slave address
       * to the bus FIFO. */
      BCM2835_REG(BCM2835_I2C_FIFO) = msgs[i].addr & 0xFF;

      /* Address slave device, with the 2 most-significative bits at the end. */
      BCM2835_REG(BCM2835_I2C_A) = (0x1E << 2) | (msgs[i].addr >> 8);
    }
    /* If using the regular 7-bit slave addressing. */
    else {
      /* Address slave device. */
      BCM2835_REG(BCM2835_I2C_A) = msgs[i].addr;
    }

    if ( msgs[i].flags & I2C_M_RD ) {
      /* If the slave uses 10-bit addressing. */
      if ( msgs[i].flags & I2C_M_TEN ) {
        /* 10-bit addressing setup for a read transfer. */
        BCM2835_REG(BCM2835_I2C_DLEN) = 1;

        /* Set write bit. */
        BCM2835_REG(BCM2835_I2C_C) &= ~(1 << 0);

        /* Send start bit. */
        BCM2835_REG(BCM2835_I2C_C) |= (1 << 7);

        /* Poll the TA bit until the transfer has started. */
        I2C_POLLING((BCM2835_REG(BCM2835_I2C_S) & (1 << 0)) == 0);
      }

      /* Set read bit. */
      BCM2835_REG(BCM2835_I2C_C) |= (1 << 0);

      bus->read_transfer = true;
    }
    else if ( msgs[i].flags == 0 || msgs[i].flags == I2C_M_TEN ) {
      /* If the slave uses 10-bit addressing. */
      if ( msgs[i].flags & I2C_M_TEN ) {
        /* 10-bit addressing setup for a write transfer. */
        bus->current_buffer_size += 1;

        bus->remaining_transfers = TRANSFER_COUNT(bus->current_buffer_size);
      }

      /* Set write bit. */
      BCM2835_REG(BCM2835_I2C_C) &= ~(1 << 0);

      bus->read_transfer = false;
    }

    rv = rpi_i2c_setup_transfer(bus);

    if ( rv < 0 ) {
      return rv;
    }
  }

  return rv;
}

/* Calculates a clock divider to be used with the BSC core clock rate
 * to set a I2C clock rate the closest (<=) to a desired frequency. */
static int rpi_i2c_set_clock(i2c_bus *base, unsigned long clock)
{
  rpi_i2c_bus *bus = (rpi_i2c_bus *) base;
  uint32_t clock_rate;
  uint16_t divider;

  /* Calculates an initial clock divider. */
  divider = BSC_CORE_CLK_HZ / clock;

  clock_rate = BSC_CORE_CLK_HZ / divider;

  /* If the resulting clock rate is greater than desired, try the next greater
   * divider. */
  while ( clock_rate > clock ) {
    ++divider;

    clock_rate = BSC_CORE_CLK_HZ / divider;
  }

  /* Set clock divider. */
  BCM2835_REG(BCM2835_I2C_DIV) = divider;

  bus->input_clock = clock_rate;

  return 0;
}

static void rpi_i2c_destroy(i2c_bus *base)
{
  rpi_i2c_bus *bus = (rpi_i2c_bus *) base;

  i2c_bus_destroy_and_free(&bus->base);
}

int rpi_i2c_register_bus(
  const char *bus_path,
  uint32_t bus_clock
) {
#if I2C_IO_MODE == 1
  rtems_status_code sc;
#endif
  rpi_i2c_bus *bus;
  int rv;

  bus = (rpi_i2c_bus *) i2c_bus_alloc_and_init(sizeof(*bus));

  if ( bus == NULL ) {
    return -1;
  }

  /* Enable the I2C BSC interface. */
  BCM2835_REG(BCM2835_I2C_C) |= (1 << 15);

  /* If the access to the bus is configured to be interrupt-driven. */
#if I2C_IO_MODE == 1
  bus->task_id = rtems_task_self();

  sc = rtems_interrupt_handler_install(
         BCM2835_IRQ_ID_I2C,
         NULL,
         RTEMS_INTERRUPT_UNIQUE,
         (rtems_interrupt_handler) i2c_handler,
         bus
       );

  if ( sc != RTEMS_SUCCESSFUL ) {
    return -EIO;
  }
#endif

  rv = rpi_i2c_set_clock(&bus->base, bus_clock);

  if ( rv < 0 ) {
    (*bus->base.destroy)(&bus->base);

    return -1;
  }

  bus->base.transfer = rpi_i2c_transfer;
  bus->base.set_clock = rpi_i2c_set_clock;
  bus->base.destroy = rpi_i2c_destroy;
  bus->base.functionality = I2C_FUNC_I2C | I2C_FUNC_10BIT_ADDR;

  return i2c_bus_register(&bus->base, bus_path);
}

void rpi_i2c_init(void)
{
  /* Enable the I2C interface on the Raspberry Pi. */
  rtems_gpio_initialize();

  assert ( rpi_gpio_select_i2c() == RTEMS_SUCCESSFUL );
}
