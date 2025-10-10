/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2024 Contemporary Software
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS IS
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

#include <rtems.h>
#include <stdint.h>

#include <dev/spi/zynq-qspi-flash-defs.h>

/*
 * The following variables are shared between non-interrupt processing and
 * interrupt processing such that they must be global.
 */
rtems_id transfer_task;

__attribute__((weak)) void zqspi_write_unlock(zqspiflash *driver)
{
  (void) driver;
}

__attribute__((weak)) void zqspi_write_lock(zqspiflash *driver)
{
  (void) driver;
}

static void qspi_flash_rx(void) {
  while (true) {
    if (
      (qspi_reg_read(ZQSPI_QSPI_REG_INTR_STATUS) &
        ZQSPI_QSPI_IXR_RXNEMPTY) == 0
    )
    {
      break;
    }
    qspi_reg_read(ZQSPI_QSPI_REG_RX_DATA);
  }
}

zqspi_error zqspi_transfer(
  zqspi_transfer_buffer* transfer,
  bool *initialised
)
{
  uint32_t tx_reg;
  uint32_t sr;
  rtems_status_code sc;
  transfer_task = rtems_task_self();

  if (*initialised == false)
  {
    qspi_reg_write(ZQSPI_QSPI_REG_EN, 0);
    qspi_reg_write(ZQSPI_QSPI_REG_LSPI_CFG, 0x00a002eb);
    qspi_flash_rx();
    qspi_reg_write(
      ZQSPI_QSPI_REG_CONFIG,
      ZQSPI_QSPI_CR_SSFORCE | ZQSPI_QSPI_CR_MANSTRTEN | ZQSPI_QSPI_CR_HOLDB_DR |
        ZQSPI_QSPI_CR_BAUD_RATE | ZQSPI_QSPI_CR_MODE_SEL
    );
    *initialised = true;
  }

  zqspi_transfer_trace("transfer:TX", transfer);

  /*
   * Set the chip select.
   */
  qspi_reg_write(ZQSPI_QSPI_REG_CONFIG,
           qspi_reg_read(ZQSPI_QSPI_REG_CONFIG) & ~ZQSPI_QSPI_CR_PCS);

  /*
   * Enable SPI.
   */
  qspi_reg_write(ZQSPI_QSPI_REG_EN, ZQSPI_QSPI_EN_SPI_ENABLE);

  /*
   * The RX pointer can never catch up and overtake the TX pointer.
   */
  transfer->tx_data = (uint32_t*) transfer->buffer;
  transfer->rx_data = (uint32_t*) transfer->buffer;
  transfer->tx_length = transfer->length;
  transfer->rx_length = transfer->length;

  /*
   * The buffer to right aligned, that is padding is add to the front of the
   * buffer to get the correct aligment for the instruction size. This means
   * the data in the first "transfer" is not aligned in the correct bits for
   * the keyhole access to the FIFO.
   */
  switch (transfer->padding)
  {
    case 3:
      *(transfer->tx_data) >>= 24;
      tx_reg = ZQSPI_QSPI_REG_TXD1;
      transfer->tx_length -= 1;
      transfer->sending += 1;
      transfer->start = true;
      break;
    case 2:
      *(transfer->tx_data) >>= 16;
      tx_reg = ZQSPI_QSPI_REG_TXD2;
      transfer->tx_length -= 2;
      transfer->sending += 2;
      transfer->start = true;
      break;
    case 1:
      *(transfer->tx_data) >>= 8;
      tx_reg = ZQSPI_QSPI_REG_TXD3;
      transfer->tx_length -= 3;
      transfer->sending += 3;
      transfer->start = true;
      break;
    default:
      tx_reg = ZQSPI_QSPI_REG_TXD0;
      transfer->tx_length -= 4;
      transfer->sending += 4;
      if (transfer->tx_length == 0)
        transfer->start = true;
      break;
  }

  qspi_reg_write (tx_reg, *(transfer->tx_data));
  ++(transfer->tx_data);

  if (transfer->start)
  {
    qspi_reg_write(ZQSPI_QSPI_REG_CONFIG,
             qspi_reg_read(ZQSPI_QSPI_REG_CONFIG) | ZQSPI_QSPI_CR_MANSTRT);

    sr = qspi_reg_read(ZQSPI_QSPI_REG_INTR_STATUS);
    while ((sr & ZQSPI_QSPI_IXR_TXOW) == 0)
    {
      sr = qspi_reg_read(ZQSPI_QSPI_REG_INTR_STATUS);
    }
  }

  /* Enable Interrupts */
  qspi_reg_write(ZQSPI_QSPI_REG_INTR_DISABLE,
    ~(ZQSPI_QSPI_IXR_RXNEMPTY | ZQSPI_QSPI_IXR_TXUF));
  qspi_reg_write(ZQSPI_QSPI_REG_INTR_ENABLE,
    (ZQSPI_QSPI_IXR_RXNEMPTY | ZQSPI_QSPI_IXR_TXUF));

  /* Wait for transfer to complete */
  sc = rtems_event_transient_receive(RTEMS_WAIT, ZQSPI_TIMEOUT_TICKS);
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_event_transient_clear();
    return ZQPSI_FLASH_TRANSFER_FAILED;
  }

  /*
   * skip the command byte.
   */
  zqspi_transfer_buffer_skip(transfer, 1);

  /*
   * Disable the chip select.
   */
  qspi_reg_write(ZQSPI_QSPI_REG_CONFIG,
           qspi_reg_read(ZQSPI_QSPI_REG_CONFIG) | ZQSPI_QSPI_CR_PCS);

  /*
   * Disable SPI.
   */
  qspi_reg_write(ZQSPI_QSPI_REG_EN, 0);

  zqspi_transfer_trace("transfer:RX", transfer);

  return ZQSPI_FLASH_NO_ERROR;
}

void zqspi_transfer_intr(zqspiflash *driver)
{
  uint32_t sr;
  zqspi_transfer_buffer* transfer = &(driver->buf);

  /* Disable and clear interrupts */
  qspi_reg_write(ZQSPI_QSPI_REG_INTR_DISABLE, 0xFFFFFFFF);
  qspi_reg_write(ZQSPI_QSPI_REG_INTR_STATUS, 0xFFFFFFFF);
  sr = qspi_reg_read(ZQSPI_QSPI_REG_INTR_STATUS);

  if (transfer->rx_length)
  {
    while (transfer->start && transfer->sending)
    {
      if ((sr & ZQSPI_QSPI_IXR_RXNEMPTY) != 0)
      {
        *(transfer->rx_data) = qspi_reg_read(ZQSPI_QSPI_REG_RX_DATA);
        ++(transfer->rx_data);
        if (transfer->rx_length > sizeof(uint32_t)) {
          transfer->rx_length -= sizeof(uint32_t);
        } else {
          transfer->rx_length = 0;
        }
        if (transfer->sending > sizeof(uint32_t)) {
          transfer->sending -= sizeof(uint32_t);
        } else {
          transfer->sending = 0;
        }
      }

      sr = qspi_reg_read(ZQSPI_QSPI_REG_INTR_STATUS);
    }
  }

  if (transfer->tx_length)
  {
    transfer->start = false;
    while (transfer->tx_length && ((sr & ZQSPI_QSPI_IXR_TXFULL) == 0))
    {
      qspi_reg_write (ZQSPI_QSPI_REG_TXD0, *(transfer->tx_data));
      ++(transfer->tx_data);
      if (transfer->tx_length > sizeof(uint32_t)) {
        transfer->tx_length -= sizeof(uint32_t);
      } else {
        transfer->tx_length = 0;
      }
      transfer->sending += sizeof(uint32_t);
      transfer->start = true;

      sr = qspi_reg_read(ZQSPI_QSPI_REG_INTR_STATUS);
    }

    if (transfer->start)
    {
      qspi_reg_write(ZQSPI_QSPI_REG_CONFIG,
               qspi_reg_read(ZQSPI_QSPI_REG_CONFIG) | ZQSPI_QSPI_CR_MANSTRT);
    }
  }

  if (transfer->tx_length) {
    qspi_reg_write(ZQSPI_QSPI_REG_INTR_ENABLE,
      qspi_reg_read(ZQSPI_QSPI_REG_INTR_ENABLE) | ZQSPI_QSPI_IXR_TXUF);
  }
  if (transfer->rx_length) {
    qspi_reg_write(ZQSPI_QSPI_REG_INTR_ENABLE,
      qspi_reg_read(ZQSPI_QSPI_REG_INTR_ENABLE) | ZQSPI_QSPI_IXR_RXNEMPTY);
  }
  if (transfer->tx_length == 0 && transfer->rx_length == 0) {
    (void) rtems_event_transient_send(transfer_task);
  }
}
