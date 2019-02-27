/**
 * @file
 *
 * @ingroup raspberrypi_spi
 *
 * @brief Support for the SPI bus on the Raspberry Pi GPIO P1 header (model A/B)
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
 * - Bi-directional mode untested
 * - Write-only devices not supported
 */

#include <bsp.h>
#include <bsp/raspberrypi.h>
#include <bsp/gpio.h>
#include <bsp/rpi-gpio.h>
#include <bsp/irq.h>
#include <bsp/spi.h>
#include <assert.h>

#define SPI_POLLING(condition)                  \
  while ( condition ) {                         \
    ;                                           \
  }

/**
 * @brief Object containing the SPI bus configuration settings.
 *
 * Encapsulates the current SPI bus configuration.
 */
typedef struct
{
  int initialized;
  uint8_t bytes_per_char;

  /* Shift to be applied on data transfers with
   * least significative bit first (LSB) devices. */
  uint8_t bit_shift;
  uint32_t dummy_char;
  uint32_t current_slave_addr;
  rtems_id task_id;
  int irq_write;
} rpi_spi_softc_t;

/**
 * @brief Object containing the SPI bus description.
 *
 * Encapsulates the current SPI bus description.
 */
typedef struct
{
  rtems_libi2c_bus_t bus_desc;
  rpi_spi_softc_t softc;
} rpi_spi_desc_t;

/* If set to FALSE uses 3-wire SPI, with 2 separate data lines (MOSI and MISO),
 * if set to TRUE uses 2-wire SPI, where the MOSI data line doubles as the
 * slave out (SO) and slave in (SI) data lines. */
static bool bidirectional = false;

/* Calculates a clock divider to be used with the GPU core clock rate
 * to set a SPI clock rate the closest (<=) to a desired frequency. */
static rtems_status_code rpi_spi_calculate_clock_divider(
  uint32_t clock_hz,
  uint16_t *clock_divider
) {
  uint16_t divider;
  uint32_t clock_rate;

  assert( clock_hz > 0 );

  /* Calculates an initial clock divider. */
  divider = GPU_CORE_CLOCK_RATE / clock_hz;

  /* Because the divider must be a power of two (as per the BCM2835 datasheet),
   * calculate the next greater power of two. */
  --divider;

  divider |= (divider >> 1);
  divider |= (divider >> 2);
  divider |= (divider >> 4);
  divider |= (divider >> 8);

  ++divider;

  clock_rate = GPU_CORE_CLOCK_RATE / divider;

  /* If the resulting clock rate is greater than the desired frequency,
   * try the next greater power of two divider. */
  while ( clock_rate > clock_hz ) {
    divider = (divider << 1);

    clock_rate = GPU_CORE_CLOCK_RATE / divider;
  }

  *clock_divider = divider;

  return RTEMS_SUCCESSFUL;
}

/**
 * @brief Set the SPI bus transfer mode.
 *
 * @param[in] bushdl Pointer to the libi2c API bus driver data structure.
 * @param[in] tfr_mode Pointer to a libi2c API transfer mode data structure.
 *
 * @retval RTEMS_SUCCESSFUL Successfully setup the bus transfer mode as desired.
 * @retval RTEMS_INVALID_NUMBER This can have two meanings:
 *                              1. The specified number of bytes per char is not
 *                                 8, 16, 24 or 32;
 *                              2. @see rpi_spi_calculate_clock_divider()
 */
static rtems_status_code rpi_spi_set_tfr_mode(
  rtems_libi2c_bus_t *bushdl,
  const rtems_libi2c_tfr_mode_t *tfr_mode
) {
  rpi_spi_softc_t *softc_ptr = &(((rpi_spi_desc_t *)(bushdl))->softc);
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  uint16_t clock_divider;

  /* Set the dummy character. */
  softc_ptr->dummy_char = tfr_mode->idle_char;

  /* Calculate the most appropriate clock divider. */
  sc = rpi_spi_calculate_clock_divider(tfr_mode->baudrate, &clock_divider);

  if ( sc != RTEMS_SUCCESSFUL ) {
    return sc;
  }

  /* Set the bus clock divider. */
  BCM2835_REG(BCM2835_SPI_CLK) = clock_divider;

  /* Calculate how many bytes each character has.
   * Only multiples of 8 bits are accepted for the transaction. */
  switch ( tfr_mode->bits_per_char ) {
    case 8:
    case 16:
    case 24:
    case 32:
      softc_ptr->bytes_per_char = tfr_mode->bits_per_char / 8;
      break;

    default:
      return RTEMS_INVALID_NUMBER;
  }

  /* Check the data mode (most or least significant bit first) and calculate
   * the correcting bit shift value to apply on the data before sending. */
  if ( tfr_mode->lsb_first ) {
    softc_ptr->bit_shift = 32 - tfr_mode->bits_per_char;
  }
  /* If MSB first. */
  else {
    softc_ptr->bit_shift = 0;
  }

  /* Set SPI clock polarity.
   * If clock_inv is TRUE, the clock is active high.*/
  if ( tfr_mode->clock_inv ) {
    /* Rest state of clock is low. */
    BCM2835_REG(BCM2835_SPI_CS) &= ~(1 << 3);
  }
  else {
    /* Rest state of clock is high. */
    BCM2835_REG(BCM2835_SPI_CS) |= (1 << 3);
  }

  /* Set SPI clock phase.
   * If clock_phs is true, clock starts toggling
   * at the start of the data transfer. */
  if ( tfr_mode->clock_phs ) {
    /* First SCLK transition at beginning of data bit. */
    BCM2835_REG(BCM2835_SPI_CS) |= (1 << 2);
  }
  else {
    /* First SCLK transition at middle of data bit. */
    BCM2835_REG(BCM2835_SPI_CS) &= ~(1 << 2);
  }

  return sc;
}

/**
 * @brief Reads/writes to/from the SPI bus.
 *
 * @param[in] bushdl Pointer to the libi2c API bus driver data structure.
 * @param[in] rd_buf Read buffer. If not NULL the function will read from
 *                   the bus and store the read on this buffer.
 * @param[in] wr_buf Write buffer. If not NULL the function will write the
 *                   contents of this buffer to the bus.
 * @param[in] buffer_size Size of the non-NULL buffer.
 *
 * @retval -1 Could not send/receive data to/from the bus.
 * @retval >=0 The number of bytes read/written.
 */
static int rpi_spi_read_write(
  rtems_libi2c_bus_t * bushdl,
  unsigned char *rd_buf,
  const unsigned char *wr_buf,
  int buffer_size
) {
  rpi_spi_softc_t *softc_ptr = &(((rpi_spi_desc_t *)(bushdl))->softc);

  uint8_t bytes_per_char = softc_ptr->bytes_per_char;
  uint8_t bit_shift =  softc_ptr->bit_shift;
  uint32_t dummy_char = softc_ptr->dummy_char;

  uint32_t bytes_sent = buffer_size;
  uint32_t fifo_data;

  /* Clear SPI bus FIFOs. */
  BCM2835_REG(BCM2835_SPI_CS) |= (3 << 4);

  /* Set SPI transfer active. */
  BCM2835_REG(BCM2835_SPI_CS) |= (1 << 7);

  /* If using the SPI bus in interrupt-driven mode. */
#if SPI_IO_MODE == 1
  softc_ptr->irq_write = 1;

  BCM2835_REG(BCM2835_SPI_CS) |= (1 << 9);

  if ( rtems_event_transient_receive(RTEMS_WAIT, 0) != RTEMS_SUCCESSFUL ) {
    rtems_event_transient_clear();

    return -1;
  }

  /* If using the bus in polling mode. */
#else
  /* Poll TXD bit until there is space to write at least one byte
   * on the TX FIFO. */
  SPI_POLLING((BCM2835_REG(BCM2835_SPI_CS) & (1 << 18)) == 0);
#endif

  /* While there is data to be transferred. */
  while ( buffer_size >= bytes_per_char ) {
    /* If reading from the bus, send a dummy character to the device. */
    if ( rd_buf != NULL ) {
      BCM2835_REG(BCM2835_SPI_FIFO) = dummy_char;
    }
    /* If writing to the bus, move the buffer data to the TX FIFO. */
    else {
      switch ( bytes_per_char ) {
        case 1:
          BCM2835_REG(BCM2835_SPI_FIFO) = (((*wr_buf) & 0xFF) << bit_shift);
          break;

        case 2:
          BCM2835_REG(BCM2835_SPI_FIFO) = (((*wr_buf) & 0xFFFF) << bit_shift);
          break;

        case 3:
          BCM2835_REG(BCM2835_SPI_FIFO) = (((*wr_buf) & 0xFFFFFF) << bit_shift);
          break;

        case 4:
          BCM2835_REG(BCM2835_SPI_FIFO) = ((*wr_buf) << bit_shift);
          break;

        default:
          return -1;
      }

      wr_buf += bytes_per_char;

      buffer_size -= bytes_per_char;
    }

    /* If using bi-directional SPI. */
    if ( bidirectional ) {
      /* Change bus direction to read from the slave device. */
      BCM2835_REG(BCM2835_SPI_CS) |= (1 << 12);
    }

    /* If using the SPI bus in interrupt-driven mode. */
#if SPI_IO_MODE == 1
    softc_ptr->irq_write = 0;

    BCM2835_REG(BCM2835_SPI_CS) |= (1 << 9);

    if ( rtems_event_transient_receive(RTEMS_WAIT, 0) != RTEMS_SUCCESSFUL ) {
      rtems_event_transient_clear();

      return -1;
    }

    /* If using the bus in polling mode. */
#else
    /* Poll the Done bit until the data transfer is complete. */
    SPI_POLLING((BCM2835_REG(BCM2835_SPI_CS) & (1 << 16)) == 0);

    /* Poll the RXD bit until there is at least one byte
     * on the RX FIFO to be read. */
    SPI_POLLING((BCM2835_REG(BCM2835_SPI_CS) & (1 << 17)) == 0);
#endif

    /* If writing to the bus, read the dummy char sent by the slave device. */
    if ( rd_buf == NULL ) {
      fifo_data = BCM2835_REG(BCM2835_SPI_FIFO) & 0xFF;
    }

    /* If reading from the bus, retrieve data from the RX FIFO and
     * store it on the buffer. */
    if ( rd_buf != NULL ) {
      switch ( bytes_per_char ) {
        case 1:
          fifo_data = BCM2835_REG(BCM2835_SPI_FIFO) & 0xFF;
          (*rd_buf) = (fifo_data >> bit_shift);
          break;

        case 2:
          fifo_data = BCM2835_REG(BCM2835_SPI_FIFO) & 0xFFFF;
          (*rd_buf) = (fifo_data >> bit_shift);
          break;

        case 3:
          fifo_data = BCM2835_REG(BCM2835_SPI_FIFO) & 0xFFFFFF;
          (*rd_buf) = (fifo_data >> bit_shift);
          break;

        case 4:
          fifo_data = BCM2835_REG(BCM2835_SPI_FIFO);
          (*rd_buf) = (fifo_data >> bit_shift);
          break;

        default:
          return -1;
      }

      rd_buf += bytes_per_char;

      buffer_size -= bytes_per_char;
    }

    /* If using bi-directional SPI. */
    if ( bidirectional ) {
      /* Restore bus direction to write to the slave. */
      BCM2835_REG(BCM2835_SPI_CS) &= ~(1 << 12);
    }
  }

  /* If using the SPI bus in interrupt-driven mode. */
#if SPI_IO_MODE == 1
  softc_ptr->irq_write = 1;

  BCM2835_REG(BCM2835_SPI_CS) |= (1 << 9);

  if ( rtems_event_transient_receive(RTEMS_WAIT, 0) != RTEMS_SUCCESSFUL ) {
    rtems_event_transient_clear();

    return -1;
  }

  /* If using the bus in polling mode. */
#else
  /* Poll the Done bit until the data transfer is complete. */
  SPI_POLLING((BCM2835_REG(BCM2835_SPI_CS) & (1 << 16)) == 0);
#endif

  bytes_sent -= buffer_size;

  return bytes_sent;
}

/**
 * @brief Handler function that is called on any SPI interrupt.
 *
 *        There are 2 situations that can generate an interrupt:
 *
 *        1. Transfer (read/write) complete;
 *        2. RX FIFO full.
 *
 *        Because the 2. situation is not useful to many applications,
 *        the only interrupt that is generated and handled is the
 *        transfer complete interrupt.
 *
 *        The objective of the handler is then, depending on the transfer
 *        context (reading or writing on the bus), to check if there is enough
 *        space available on the TX FIFO to send data over the bus (if writing)
 *        or if the slave device has sent enough data to be fetched from the
 *        RX FIFO (if reading).
 *
 *        When any of these two conditions occur, disables further interrupts
 *        to be generated and sends a waking event to the transfer task
 *        which will allow the following transfer to proceed.
 *
 * @param[in] arg Void pointer to the bus data structure.
 */
#if SPI_IO_MODE == 1
static void spi_handler(void* arg)
{
  rpi_spi_softc_t *softc_ptr = (rpi_spi_softc_t *) arg;

  /* If waiting to write to the bus, expect the TXD bit to be set, or
   * if waiting to read from the bus, expect the RXD bit to be set
   * before sending a waking event to the transfer task. */
  if (
      ( softc_ptr->irq_write == 1 &&
        (BCM2835_REG(BCM2835_SPI_CS) & (1 << 18)) != 0
      ) ||
      ( softc_ptr->irq_write == 0 &&
        (BCM2835_REG(BCM2835_SPI_CS) & (1 << 17)) != 0
      )
  ) {
    /* Disable the SPI interrupt generation when a transfer is complete. */
    BCM2835_REG(BCM2835_SPI_CS) &= ~(1 << 9);

    /* Allow the transfer process to continue. */
    rtems_event_transient_send(softc_ptr->task_id);
  }
}
#endif

/**
 * @brief Low level function to initialize the SPI bus.
 *        This function is used by the libi2c API.
 *
 * @param[in] bushdl Pointer to the libi2c API bus driver data structure.
 *
 * @retval RTEMS_SUCCESSFUL SPI bus successfully initialized.
 * @retval Any other status code @see rtems_interrupt_handler_install().
 */
static rtems_status_code rpi_libi2c_spi_init(rtems_libi2c_bus_t * bushdl)
{
  rpi_spi_softc_t *softc_ptr = &(((rpi_spi_desc_t *)(bushdl))->softc);
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if ( softc_ptr->initialized == 1 ) {
    return sc;
  }

  softc_ptr->initialized = 1;

  /* If using the SPI bus in interrupt-driven mode. */
#if SPI_IO_MODE == 1
  softc_ptr->task_id = rtems_task_self();

  sc = rtems_interrupt_handler_install(
         BCM2835_IRQ_ID_SPI,
         NULL,
         RTEMS_INTERRUPT_UNIQUE,
         (rtems_interrupt_handler) spi_handler,
         softc_ptr
       );
#endif

  return sc;
}

/**
 * @brief Low level function that would send a start condition over an I2C bus.
 *        As it is not required to access a SPI bus it is here just to satisfy
 *        the libi2c API, which requires this function.
 *
 * @param[in] bushdl Pointer to the libi2c API bus driver data structure.
 *
 * @retval RTEMS_SUCCESSFUL
 */
static rtems_status_code rpi_libi2c_spi_send_start(rtems_libi2c_bus_t * bushdl)
{
  return RTEMS_SUCCESSFUL;
}

/**
 * @brief Low level function that terminates a SPI transfer.
 *        It stops the SPI transfer and unselects the current SPI slave device.
 *        This function is used by the libi2c API.
 *
 * @param[in] bushdl Pointer to the libi2c API bus driver data structure.
 *
 * @retval RTEMS_SUCCESSFUL The slave device has been successfully unselected.
 * @retval RTEMS_INVALID_ADDRESS The stored slave address is neither 0 or 1.
 */
static rtems_status_code rpi_libi2c_spi_stop(rtems_libi2c_bus_t * bushdl)
{
  rpi_spi_softc_t *softc_ptr = &(((rpi_spi_desc_t *)(bushdl))->softc);

  uint32_t addr = softc_ptr->current_slave_addr;
  uint32_t chip_select_bit = 21 + addr;

  /* Set SPI transfer as not active. */
  BCM2835_REG(BCM2835_SPI_CS) &= ~(1 << 7);

  /* Unselect the active SPI slave. */
  switch ( addr ) {
    case 0:
    case 1:
      BCM2835_REG(BCM2835_SPI_CS) |= (1 << chip_select_bit);

      break;

    default:
      return RTEMS_INVALID_ADDRESS;
  }

  return RTEMS_SUCCESSFUL;
}

/**
 * @brief Low level function which addresses a SPI slave device.
 *        This function is used by the libi2c API.
 *
 * @param[in] bushdl Pointer to the libi2c API bus driver data structure.
 * @param[in] addr SPI slave select line address (0 for CE0 or 1 for CE1).
 * @param[in] rw This values is unnecessary to address a SPI device and its
 *               presence here is only to fulfill a libi2c requirement.
 *
 * @retval RTEMS_SUCCESSFUL The slave device has been successfully addressed.
 * @retval RTEMS_INVALID_ADDRESS The received address is neither 0 or 1.
 */
static rtems_status_code rpi_libi2c_spi_send_addr(
  rtems_libi2c_bus_t * bushdl,
  uint32_t addr,
  int rw
) {
  rpi_spi_softc_t *softc_ptr = &(((rpi_spi_desc_t *)(bushdl))->softc);

  /* Calculates the bit corresponding to the received address
   * on the SPI control register. */
  uint32_t chip_select_bit = 21 + addr;

  /* Save which slave will be currently addressed,
   * so it can be unselected later. */
  softc_ptr->current_slave_addr = addr;

  /* Select one of the two available SPI slave address lines. */
  switch ( addr ) {
    case 0:
    case 1:
      BCM2835_REG(BCM2835_SPI_CS) &= ~(1 << chip_select_bit);
      break;

    default:
      return RTEMS_INVALID_ADDRESS;
  }

  return RTEMS_SUCCESSFUL;
}

/**
 * @brief Low level function that reads a number of bytes from the SPI bus
 *        on to a buffer.
 *        This function is used by the libi2c API.
 *
 * @param[in] bushdl Pointer to the libi2c API bus driver data structure.
 * @param[in] bytes Buffer where the data read from the bus will be stored.
 * @param[in] nbytes Number of bytes to be read from the bus to the bytes buffer.
 *
 * @retval @see rpi_spi_read_write().
 */
static int rpi_libi2c_spi_read_bytes(
  rtems_libi2c_bus_t * bushdl,
  unsigned char *bytes,
  int nbytes
) {
  return rpi_spi_read_write(bushdl, bytes, NULL, nbytes);
}

/**
 * @brief Low level function that writes a number of bytes from a buffer
 *        to the SPI bus.
 *        This function is used by the libi2c API.
 *
 * @param[in] bushdl Pointer to the libi2c API bus driver data structure.
 * @param[in] bytes Buffer with data to send over the SPI bus.
 * @param[in] nbytes Number of bytes to be written from the bytes buffer
                     to the bus.
 *
 * @retval @see rpi_spi_read_write().
 */
static int rpi_libi2c_spi_write_bytes(
  rtems_libi2c_bus_t * bushdl,
  unsigned char *bytes,
  int nbytes
) {
  return rpi_spi_read_write(bushdl, NULL, bytes, nbytes);
}

/**
 * @brief Low level function that is used to perform ioctl
 *        operations on the bus. Currently only setups
 *        the bus transfer mode.
 *        This function is used by the libi2c API.
 *
 * @param[in] bushdl Pointer to the libi2c API bus driver data structure.
 * @param[in] cmd IOCTL request command.
 * @param[in] arg Arguments needed to fulfill the requested IOCTL command.
 *
 * @retval -1 Unknown request command.
 * @retval >=0 @see rpi_spi_set_tfr_mode().
 */
static int rpi_libi2c_spi_ioctl(rtems_libi2c_bus_t * bushdl, int cmd, void *arg)
{
  switch ( cmd ) {
    case RTEMS_LIBI2C_IOCTL_SET_TFRMODE:
      return rpi_spi_set_tfr_mode(
               bushdl,
               (const rtems_libi2c_tfr_mode_t *)arg
             );
    default:
      return -1;
  }

  return 0;
}

static rtems_libi2c_bus_ops_t rpi_spi_ops = {
  .init = rpi_libi2c_spi_init,
  .send_start = rpi_libi2c_spi_send_start,
  .send_stop = rpi_libi2c_spi_stop,
  .send_addr = rpi_libi2c_spi_send_addr,
  .read_bytes = rpi_libi2c_spi_read_bytes,
  .write_bytes = rpi_libi2c_spi_write_bytes,
  .ioctl = rpi_libi2c_spi_ioctl
};

static rpi_spi_desc_t rpi_spi_bus_desc = {
  {
    .ops = &rpi_spi_ops,
    .size = sizeof(rpi_spi_bus_desc)
  },
  {
    .initialized = 0
  }
};

int rpi_spi_init(bool bidirectional_mode)
{
  /* Initialize the libi2c API. */
  rtems_libi2c_initialize();

  /* Enable the SPI interface on the Raspberry Pi. */
  rtems_gpio_initialize();

  assert ( rpi_gpio_select_spi() == RTEMS_SUCCESSFUL );

  bidirectional = bidirectional_mode;

  /* Clear SPI control register and clear SPI FIFOs. */
  BCM2835_REG(BCM2835_SPI_CS) = (3 << 4);

  /* Register the SPI bus. */
  return rtems_libi2c_register_bus("/dev/spi", &(rpi_spi_bus_desc.bus_desc));
}
