/*  SPI driver for Blackfin
 *
 *  Copyright (c) 2010 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <stdlib.h>
#include <bsp.h>
#include <rtems/error.h>
#include <rtems/bspIo.h>
#include <errno.h>
#include <rtems/libi2c.h>
#include <libcpu/spiRegs.h>
#include "spi.h"


#ifndef BFIN_REG16
#define BFIN_REG16(base, offset) \
        (*((uint16_t volatile *) ((uint8_t *)(base) + (offset))))
#endif


static bfin_spi_state_t *bfin_spi;


void bfin_spi_isr(int v) {
  bfin_spi_state_t *state;
  uint16_t r;

  state = bfin_spi;
  if (state->len > state->bytes_per_word) {
    if (state->wr_ptr) {
      if (state->bytes_per_word == 2)
        r = *(uint16_t *) state->wr_ptr;
      else
        r = (uint16_t) *state->wr_ptr;
      state->wr_ptr += state->bytes_per_word;
    } else
      r = state->idle_pattern;
    BFIN_REG16(state->base, SPI_TDBR_OFFSET) = r;
  }
  state->len -= state->bytes_per_word;
  if (state->len <= 0) {
    /*
       The transfers are done, so I don't want to kick off another
       transfer or get any more interrupts.  Reading the last word from
       SPI_SHADOW instead of SPI_RDBR should prevent it from triggering
       another transfer, but that doesn't clear the interrupt flag.  I
       could mask the interrupt in the SIC, but that would preclude ever
       using the DMA channel that shares the interrupt independently (and
       they might just share it with something more important in some other
       member of the Blackfin family).  And who knows what problems it
       might cause in this code potentially dealing with that still pended
       interrupt at the beginning of the next transfer.

       So instead I disable the SPI interface, read the data from RDBR
       (thus clearing the interrupt but not triggering another transfer
       since the interface is disabled), then re-eanble the interface.
       This has the problem that the bf537 tri-states the SPI signals
       while the interface is disabled.  Either adding pull-ups on at
       least the chip select signals, or using GPIOs for them so they're
       not controlled by the SPI module, would be correct fixes for that
       (really pull-ups/downs should be added to the SPI CLK and MOSI
       signals as well to insure they cannot float into some region that
       causes input structures to consume excessive power).  Or they can
       all be left alone, assuming that there's enough capacitance on the
       lines to prevent any problems for the short time they're being left
       disabled.

       An alternative approach I attempted involved switching TIMOD
       between RDBR and TDBR when starting and finishing a transfer, but
       I didn't get anywhere with that.  In my limited testing TIMOD TDBR
       wasn't behaving as I expected it to, but maybe with more
       experimentation I'd find some solution there.  However I'm out
       of time for this project, at least for now.
    */

    BFIN_REG16(state->base, SPI_CTL_OFFSET) &= ~SPI_CTL_SPE;
    r = BFIN_REG16(state->base, SPI_RDBR_OFFSET);
    BFIN_REG16(state->base, SPI_CTL_OFFSET) |= SPI_CTL_SPE;
    rtems_semaphore_release(state->sem);
  } else
    r = BFIN_REG16(state->base, SPI_RDBR_OFFSET);

  if (state->rd_ptr) {
    if (state->bytes_per_word == 2)
      *(uint16_t *) state->rd_ptr = r;
    else
      *state->rd_ptr = (uint8_t) r;
    state->rd_ptr += state->bytes_per_word;
  }
}

static rtems_status_code setTFRMode(rtems_libi2c_bus_t *bus,
                                    const rtems_libi2c_tfr_mode_t *tfrMode) {
  rtems_status_code result;
  bfin_spi_state_t *state;
  uint32_t divisor;
  uint16_t ctrl;

  result = RTEMS_SUCCESSFUL;
  state = &((bfin_spi_bus_t *) bus)->p;

  if (result == RTEMS_SUCCESSFUL) {
    if (tfrMode->bits_per_char != 8 &&
        tfrMode->bits_per_char != 16)
      result = RTEMS_INVALID_NUMBER;
    if (tfrMode->baudrate <= 0)
      result = RTEMS_INVALID_NUMBER;
  }
  if (result == RTEMS_SUCCESSFUL) {
    divisor = (SCLK / 2 + tfrMode->baudrate - 1) /
              tfrMode->baudrate;
    if (divisor < 2)
      divisor = 2;
    else if (divisor > 65535)
      result = RTEMS_INVALID_NUMBER;
  }
  if (result == RTEMS_SUCCESSFUL) {
    state->idle_pattern = (uint16_t) tfrMode->idle_char;
    state->bytes_per_word = (tfrMode->bits_per_char > 8) ? 2 : 1;
    BFIN_REG16(state->base, SPI_BAUD_OFFSET) = divisor;
    ctrl = BFIN_REG16(state->base, SPI_CTL_OFFSET);
    if (tfrMode->lsb_first)
      ctrl |= SPI_CTL_LSBF;
    else
      ctrl &= ~SPI_CTL_LSBF;
    if (tfrMode->bits_per_char > 8)
      ctrl |= SPI_CTL_SIZE;
    else
      ctrl &= ~SPI_CTL_SIZE;
    if (tfrMode->clock_inv)
      ctrl |= SPI_CTL_CPOL;
    else
      ctrl &= ~SPI_CTL_CPOL;
    if (tfrMode->clock_phs)
      ctrl |= SPI_CTL_CPHA;
    else
      ctrl &= ~SPI_CTL_CPHA;
    BFIN_REG16(state->base, SPI_CTL_OFFSET) = ctrl;
  }

  return result;
}

static int readWrite(rtems_libi2c_bus_t *bus, uint8_t *rdBuf,
                     const uint8_t *wrBuf, int len) {
  rtems_status_code result;
  bfin_spi_state_t *state;
  uint16_t r;

  result = RTEMS_SUCCESSFUL;
  state = &((bfin_spi_bus_t *) bus)->p;

  if (len) {
    state->rd_ptr = rdBuf;
    state->wr_ptr = wrBuf;
    state->len = len;
    if (state->wr_ptr) {
      if (state->bytes_per_word == 2)
        r = *(uint16_t *) state->wr_ptr;
      else
        r = (uint16_t) *state->wr_ptr;
      state->wr_ptr += state->bytes_per_word;
    } else
      r = state->idle_pattern;
    BFIN_REG16(state->base, SPI_TDBR_OFFSET) = r;
    BFIN_REG16(state->base, SPI_RDBR_OFFSET); /* trigger */
    /* wait until done */
    do {
      result = rtems_semaphore_obtain(state->sem, RTEMS_WAIT, 100);
    } while (result == RTEMS_SUCCESSFUL && state->len > 0);
  }

  return (result == RTEMS_SUCCESSFUL) ? len : -result;
}


rtems_status_code bfin_spi_init(rtems_libi2c_bus_t *bus) {
  rtems_status_code result;
  bfin_spi_state_t *state;

  state = &((bfin_spi_bus_t *) bus)->p;

  BFIN_REG16(state->base, SPI_CTL_OFFSET) = SPI_CTL_SPE |
                                            SPI_CTL_MSTR |
                                            SPI_CTL_CPHA |
                                            SPI_CTL_TIMOD_RDBR;

  result = rtems_semaphore_create(rtems_build_name('s','p','i','s'),
                                  0,
                                  RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE,
                                  0,
                                  &state->sem);
  if (result == RTEMS_SUCCESSFUL)
    bfin_spi = state; /* for isr */

  return result;
}

rtems_status_code bfin_spi_send_start(rtems_libi2c_bus_t *bus) {

  return RTEMS_SUCCESSFUL;
}

int bfin_spi_read_bytes(rtems_libi2c_bus_t *bus, unsigned char *buf, int len) {

  return readWrite(bus, buf, NULL, len);
}

int bfin_spi_write_bytes(rtems_libi2c_bus_t *bus, unsigned char *buf, int len) {

  return readWrite(bus, NULL, buf, len);
}

int bfin_spi_ioctl(rtems_libi2c_bus_t *bus, int cmd, void *arg) {
  int result;

  result = -RTEMS_NOT_DEFINED;
  switch(cmd) {
  case RTEMS_LIBI2C_IOCTL_SET_TFRMODE:
    result = -setTFRMode(bus, (const rtems_libi2c_tfr_mode_t *) arg);
    break;
  case RTEMS_LIBI2C_IOCTL_READ_WRITE:
    result = readWrite(bus,
                       ((rtems_libi2c_read_write_t *) arg)->rd_buf,
                       ((rtems_libi2c_read_write_t *) arg)->wr_buf,
                       ((rtems_libi2c_read_write_t *) arg)->byte_cnt);
    break;
  default:
    break;
  }

  return result;
}

