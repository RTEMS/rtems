/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMVME2100
 *
 * @brief Polled I2C bus driver for mpc8240-based boards
 */

/*
 * Copyright (C) 2026 UChicago Argonne LLC,
 * as operator of Argonne National Laboratory
 * Author: Vijay Banerjee <vijay@rtems.org>
 *
 * Copyright (C) 2005-2007 Till Straumann <strauman@slac.stanford.edu>
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

/* Note: We maintain base address, IRQ etc. statically and
 *       globally. We don't bother creating driver-specific
 *       data or using the bus handle but simply assume
 *       this is the only 8240/i2c bus in the system.
 *       Proper support for multiple instances would not
 *       be very hard to add but I don't see the point...
 */

#include <bsp.h>
#include <bsp/mpc8240_i2c_busdrv.h>
#include <libcpu/io.h>
#include <libcpu/spr.h>
#include <rtems.h>
#include <rtems/libi2c.h>

/* I2C controller register definitions */
#define I2CADR 0x00
#define I2CFDR 0x04
#define I2CCR 0x08
#define I2CCR_MEN (1 << (7 - 0))
#define I2CCR_MIEN (1 << (7 - 1))
#define I2CCR_MSTA (1 << (7 - 2))
#define I2CCR_MTX (1 << (7 - 3))
#define I2CCR_TXAK (1 << (7 - 4))
#define I2CCR_RSTA (1 << (7 - 5))
#define I2CCR_BCST (1 << (7 - 7))
#define I2CSR 0x0c
#define I2CSR_MCF (1 << (7 - 0))
#define I2CSR_MAAS (1 << (7 - 1))
#define I2CSR_MBB (1 << (7 - 2))
#define I2CSR_MAL (1 << (7 - 3))
#define I2CSR_BCSTM (1 << (7 - 4))
#define I2CSR_SRW (1 << (7 - 5))
#define I2CSR_MIF (1 << (7 - 6))
#define I2CSR_RXAK (1 << (7 - 7))
#define I2CDR 0x10
#define I2CDFSRR 0x14

SPR_RO(TBRL)

/*
 * Wild guess for 0.2 s; this timeout is effective
 * in polling mode; during early init we don't know
 * the system clock rate yet - it's one of the things
 * we have to read from VPD -- via i2c.
 */

static uint32_t poll_timeout = 333333333 / 8 / 5;

static inline uint8_t i2c_rd(unsigned reg) {
  return in_8((volatile uint8_t *)(EUMBBAR + BSP_MPC8240_I2C_OFFSET + reg));
}

static inline void i2c_wr(unsigned reg, uint8_t val) {
  out_8((volatile uint8_t *)(EUMBBAR + BSP_MPC8240_I2C_OFFSET + reg), val);
}

static inline void i2c_set(unsigned reg, uint8_t val) {
  i2c_wr(reg, i2c_rd(reg) | val);
}

static inline void i2c_clr(unsigned reg, uint8_t val) {
  i2c_wr(reg, i2c_rd(reg) & ~val);
}

/* wait until i2c status reg AND mask == cond */
static rtems_status_code i2c_wait(uint8_t msk, uint8_t cond) {
  uint32_t then;

  then = _read_TBRL();
  do {
    /* poll for .2 seconds */
    if ((_read_TBRL() - then) > poll_timeout) {
      return RTEMS_TIMEOUT;
    }
  } while ((msk & i2c_rd(I2CSR)) != cond);

  return RTEMS_SUCCESSFUL;
}

/*
 * multi-byte transfer
 * - set transfer direction (master read or master write)
 * - transfer byte
 * - wait/synchronize
 * - check for ACK
 *
 * RETURNS: number of bytes transferred or negative error code.
 */

static int i2c_xfer(int rw, uint8_t *buf, int len) {
  int i;
  rtems_status_code sc;

  if (rw) {
    i2c_clr(I2CCR, I2CCR_MTX);
  } else {
    i2c_set(I2CCR, I2CCR_MTX);
  }

  for (i = 0; i < len; i++) {
    i2c_clr(I2CSR, I2CSR_MIF);
    if (rw) {
      buf[i] = i2c_rd(I2CDR);
    } else {
      i2c_wr(I2CDR, buf[i]);
    }
    if (RTEMS_SUCCESSFUL != (sc = i2c_wait(I2CSR_MIF, I2CSR_MIF))) {
      return -sc;
    }
    if ((I2CSR_RXAK & i2c_rd(I2CSR))) {
      /* NO ACK */
      return -RTEMS_IO_ERROR;
    }
  }

  return i;
}

/*
 * This bus controller gives us lagging data, i.e.,
 * when we read a byte from the data reg then that
 * issues a read cycle on the bus and gives us the
 * byte from the *previous* read cycle :-(
 *
 * This makes it impossible to properly terminate
 * a read transaction w/o knowing ahead of time
 * how many bytes are going to be read (API decouples
 * 'START'/'STOP' from 'READ') since we would have to
 * set TXAK when reading the next-to-last byte
 * (i.e., when the last byte is read on the i2c bus).
 *
 * Hence, (if we are reading) we must do a dummy
 * read-cycle here -- hopefully
 * that has no side-effects! (i.e., EEPROM drivers should
 * reposition file pointers after issuing STOP)
 *
 */

static void rd1byte_noack(void) {
  uint8_t dum;
  uint8_t ccr;

  /* If we are in reading state then read one more
   * byte w/o acknowledge
   */
  ccr = i2c_rd(I2CCR);

  if (!(I2CCR_MTX & ccr)) {
    i2c_wr(I2CCR, ccr | I2CCR_TXAK);
    i2c_xfer(1, &dum, 1);
    /* restore original TXAK bit setting */
    i2c_clr(I2CCR, (I2CCR_TXAK & ccr));
  }
}

static rtems_status_code mpc8240_i2c_init(rtems_libi2c_bus_t *bh) {
  (void)bh;

  /* compute more accurate timeout */
  if (BSP_bus_frequency && BSP_time_base_divisor) {
    poll_timeout = BSP_bus_frequency / BSP_time_base_divisor * 1000 / 5;
  }

  i2c_clr(I2CCR, I2CCR_MEN);
  i2c_set(I2CCR, I2CCR_MEN);

  i2c_wr(I2CADR, 0);

  /* leave firmware settings for divisor and filter registers */

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code mpc8240_i2c_start(rtems_libi2c_bus_t *bh) {
  uint8_t v;

  (void)bh;

  v = i2c_rd(I2CCR);
  if (I2CCR_MSTA & v) {
    /* RESTART */
    rd1byte_noack();
    v |= I2CCR_RSTA;
  } else {
    v |= I2CCR_MSTA;
  }
  i2c_wr(I2CCR, v);

  /* On MBB we can only poll-wait (no IRQ is generated)
   * and this is also much faster than reading a byte
   * (1/2-bit time) so the overhead of an IRQ may not
   * be justified.
   * OTOH, we can put this off into the 'send_addr' routine
   */

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code mpc8240_i2c_stop(rtems_libi2c_bus_t *bh) {
  (void)bh;

  rd1byte_noack();

  /* STOP */
  i2c_clr(I2CCR, I2CCR_TXAK | I2CCR_MSTA);

  return i2c_wait(I2CSR_MBB, 0);
}

static rtems_status_code mpc8240_i2c_send_addr(rtems_libi2c_bus_t *bh,
                                               uint32_t addr, int rw) {
  uint8_t buf[2];
  int l = 0;
  uint8_t read_mask = rw ? 1 : 0;
  rtems_status_code sc;
  int xfer;

  (void)bh;

  /* Make sure we are started; (i2c_start() didn't bother to wait
   * so we do it here - some time already has expired.
   */
  sc = i2c_wait(I2CSR_MBB, I2CSR_MBB);

  if (RTEMS_SUCCESSFUL != sc) {
    return sc;
  }

  if (addr > 0x7f) {
    /* 10-bit request; 1st address byte is 0b11110<b9><b8><r/w> */
    buf[l] = 0xf0 | ((addr >> 7) & 0x06) | read_mask;
    read_mask = 0;
    l++;
    buf[l] = addr & 0xff;
  } else {
    buf[l] = (addr << 1) | read_mask;
    l++;
  }

  /*
   * After sending a an address for reading we must
   * read a dummy byte (this actually clocks the first real
   * byte on the i2c bus and makes it available in the
   * data register so that the first 'read_bytes' operation
   * obtains the byte we clock in here [and starts clocking
   * the second byte]) to overcome the pipeline
   * delay in the hardware (I don't like this design) :-(.
   */
  xfer = i2c_xfer(0, buf, l);
  if (rw && l == xfer) {
    sc = i2c_xfer(1, buf, 1);
  }
  return sc >= 0 ? RTEMS_SUCCESSFUL : -sc;
}

static int mpc8240_i2c_read_bytes(rtems_libi2c_bus_t *bh, unsigned char *buf,
                                  int len) {
  (void)bh;

  return i2c_xfer(1, buf, len);
}

static int mpc8240_i2c_write_bytes(rtems_libi2c_bus_t *bh, unsigned char *buf,
                                   int len) {
  (void)bh;

  return i2c_xfer(0, buf, len);
}

static rtems_libi2c_bus_ops_t mpc8240_i2c_ops = {
    .init = mpc8240_i2c_init,
    .send_start = mpc8240_i2c_start,
    .send_stop = mpc8240_i2c_stop,
    .send_addr = mpc8240_i2c_send_addr,
    .read_bytes = mpc8240_i2c_read_bytes,
    .write_bytes = mpc8240_i2c_write_bytes,
};

static rtems_libi2c_bus_t mpc8240_i2c_bus = {
    .ops = &mpc8240_i2c_ops,
    .size = sizeof(mpc8240_i2c_bus),
};

rtems_libi2c_bus_t *mpc8240_i2c_bus_descriptor = &mpc8240_i2c_bus;
