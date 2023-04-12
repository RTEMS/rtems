/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
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

#include <bsp.h>
#include <bsp/fatal.h>
#include <bsp/fdt.h>
#include <bsp/irq.h>

#include <chip.h>
#include <dev/i2c/i2c.h>
#include <fsl_clock.h>
#include <fsl_lpi2c.h>
#include <libfdt.h>

#define LPI2C_MTDR_CMD_transmit                           LPI2C_MTDR_CMD(0)
#define LPI2C_MTDR_CMD_receive                            LPI2C_MTDR_CMD(1)
#define LPI2C_MTDR_CMD_stop                               LPI2C_MTDR_CMD(2)
#define LPI2C_MTDR_CMD_receive_and_discard                LPI2C_MTDR_CMD(3)
#define LPI2C_MTDR_CMD_start_and_transmit                 LPI2C_MTDR_CMD(4)
#define LPI2C_MTDR_CMD_start_and_transmit_NACK            LPI2C_MTDR_CMD(5)
#define LPI2C_MTDR_CMD_start_and_transmit_highspeed       LPI2C_MTDR_CMD(6)
#define LPI2C_MTDR_CMD_start_and_transmit_highspeed_NACK  LPI2C_MTDR_CMD(7)

#define LPI2C_INT_ERRORS_SERIOUS ( \
  LPI2C_MSR_FEF_MASK | LPI2C_MSR_ALF_MASK | LPI2C_MSR_PLTF_MASK )

#define LPI2C_INT_ERROR_NO_ACK (LPI2C_MSR_NDF_MASK)

#define LPI2C_INT_ERRORS (LPI2C_INT_ERRORS_SERIOUS | LPI2C_INT_ERROR_NO_ACK)

#define LPI2C_INT_ADDRESSED (LPI2C_INT_ERRORS | LPI2C_MSR_TDF_MASK)

#define LPI2C_INT_STOP_SENT (LPI2C_INT_ERRORS | LPI2C_MSR_SDF_MASK)

#define LPI2C_INT_RECEIVED (LPI2C_INT_ERRORS | LPI2C_MSR_RDF_MASK)

#define LPI2C_INT_TRANSMITTED (LPI2C_INT_ERRORS | LPI2C_MSR_TDF_MASK)

struct imxrt_lpi2c_bus {
  i2c_bus base;
  volatile LPI2C_Type *regs;
  rtems_vector_number irq;
  uint32_t src_clock_hz;
  clock_ip_name_t clock_ip;
  unsigned long clock;

  rtems_binary_semaphore sem;
  int eno;

  uint32_t msg_todo;
  const i2c_msg *msg;

  /* Everything that is necessary for the current message */
  uint32_t chunk_todo;
  uint16_t buf_todo;
  uint8_t *buf;
  bool stop;
  bool read;
};

static void imxrt_lpi2c_sw_reset(volatile LPI2C_Type *regs)
{
  regs->MCR = LPI2C_MCR_RST_MASK | LPI2C_MCR_RRF_MASK | LPI2C_MCR_RTF_MASK;
  regs->SCR = LPI2C_SCR_RST_MASK | LPI2C_SCR_RRF_MASK | LPI2C_SCR_RTF_MASK;
  regs->MCR = 0;
  regs->SCR = 0;
}

static int imxrt_lpi2c_set_clock(i2c_bus *base, unsigned long clock)
{
  struct imxrt_lpi2c_bus *bus;
  volatile LPI2C_Type *regs;

  bus = (struct imxrt_lpi2c_bus *) base;
  regs = bus->regs;

  bus->clock = clock;

  /*
   * Maybe there is a more efficient way than used by that function. But
   * changing clock doesn't happen often. So it should be OK for now.
   */
  LPI2C_MasterSetBaudRate((LPI2C_Type *)regs, bus->src_clock_hz, clock);

  return 0;
}

static void imxrt_lpi2c_do_reinit(
  struct imxrt_lpi2c_bus *bus,
  volatile LPI2C_Type *regs
)
{
  regs->MIER = 0;
  imxrt_lpi2c_sw_reset(regs);

  regs->MCFGR2 = LPI2C_MCFGR2_FILTSDA(0) | LPI2C_MCFGR2_FILTSCL(0) |
    LPI2C_MCFGR2_BUSIDLE(0);
  regs->MCFGR3 = LPI2C_MCFGR3_PINLOW(0);

  regs->MFCR = LPI2C_MFCR_RXWATER(0) | LPI2C_MFCR_TXWATER(1);

  imxrt_lpi2c_set_clock(&bus->base, bus->clock);
}

static void imxrt_lpi2c_done(
  struct imxrt_lpi2c_bus *bus,
  volatile LPI2C_Type *regs
)
{
  regs->MIER = 0;
  regs->MCR &= ~LPI2C_MCR_MEN_MASK;
  rtems_binary_semaphore_post(&bus->sem);
}

static void imxrt_lpi2c_next_msg(
  struct imxrt_lpi2c_bus *bus,
  volatile LPI2C_Type *regs
);

static void imxrt_lpi2c_transmit_next(
  struct imxrt_lpi2c_bus *bus,
  volatile LPI2C_Type *regs
)
{
  if (bus->chunk_todo == 0) {
    /* Check whether a stop has to be send */
    if (bus->stop) {
      regs->MTDR = LPI2C_MTDR_CMD_stop;
      bus->stop = false;
      regs->MIER = LPI2C_INT_STOP_SENT;
    } else {
      imxrt_lpi2c_next_msg(bus, regs);
    }
  } else {
    if (bus->read) {
      uint16_t to_read;
      to_read = MIN(bus->chunk_todo, 256);
      bus->chunk_todo -= to_read;

      regs->MTDR = LPI2C_MTDR_CMD_receive | (to_read - 1);
      regs->MIER = LPI2C_INT_RECEIVED;
    } else {
      regs->MTDR = LPI2C_MTDR_CMD_transmit | *bus->buf;
      ++bus->buf;
      --bus->buf_todo;
      --bus->chunk_todo;
      regs->MIER = LPI2C_INT_TRANSMITTED;
    }
  }
}

static void imxrt_lpi2c_next_msg(
  struct imxrt_lpi2c_bus *bus,
  volatile LPI2C_Type *regs
)
{
  if (bus->msg_todo == 0) {
    imxrt_lpi2c_done(bus, regs);
  } else {
    const i2c_msg *msg;
    int flags;
    bool start;
    uint16_t addr;

    msg = bus->msg;
    flags = msg->flags;

    addr = msg->addr;
    start = (flags & I2C_M_NOSTART) == 0;
    bus->read = (flags & I2C_M_RD) != 0;
    bus->chunk_todo = msg->len;
    bus->buf_todo = msg->len;
    bus->buf = msg->buf;
    bus->stop = (flags & I2C_M_STOP) != 0 || bus->msg_todo <= 1;

    ++bus->msg;
    --bus->msg_todo;

    if (start) {
      uint32_t mtdr;
      mtdr = LPI2C_MTDR_CMD_start_and_transmit;
      mtdr |= addr << 1;
      if (bus->read) {
        mtdr |= 1;
      }
      regs->MTDR = mtdr;
      regs->MIER = LPI2C_INT_ADDRESSED;
    } else {
      imxrt_lpi2c_transmit_next(bus, regs);
    }
  }
}

static void imxrt_lpi2c_interrupt(void *arg)
{
  struct imxrt_lpi2c_bus *bus;
  volatile LPI2C_Type *regs;
  uint32_t msr;

  bus = arg;
  regs = bus->regs;

  msr = regs->MSR;
  regs->MSR = msr;

  if ((msr & LPI2C_INT_ERROR_NO_ACK) != 0) {
    /* Just end the transmission */
    bus->eno = EIO;
    imxrt_lpi2c_done(bus, regs);
  } else if ((msr & LPI2C_INT_ERRORS_SERIOUS) != 0) {
    /* Some worse error occurred. Reset hardware. */
    bus->eno = EIO;
    imxrt_lpi2c_do_reinit(bus, regs);
    imxrt_lpi2c_done(bus, regs);
  } else {
    uint32_t mrdr;
    while (((mrdr = regs->MRDR) & LPI2C_MRDR_RXEMPTY_MASK) == 0) {
      if (bus->read && bus->buf_todo > 0) {
        *bus->buf = (mrdr & LPI2C_MRDR_DATA_MASK) >> LPI2C_MRDR_DATA_SHIFT;
        ++bus->buf;
        --bus->buf_todo;
      }
    }

    if (
      ((msr & LPI2C_MSR_TDF_MASK) != 0) &&
      (!bus->read || bus->chunk_todo > 0 || bus->buf_todo == 0)
    ) {
      imxrt_lpi2c_transmit_next(bus, regs);
    }
  }
}

static int imxrt_lpi2c_wait_for_not_busy(volatile LPI2C_Type *regs)
{
  rtems_interval timeout;
  bool before;

  if ((regs->MSR & LPI2C_MSR_BBF_MASK) == 0) {
    return 0;
  }

  timeout = rtems_clock_tick_later_usec(5000);

  do {
    before = rtems_clock_tick_before(timeout);

    if ((regs->MSR & LPI2C_MSR_BBF_MASK) == 0) {
      return 0;
    }
  } while (before);

  return ETIMEDOUT;
}

static void imxrt_lpi2c_first_msg(
  struct imxrt_lpi2c_bus *bus,
  volatile LPI2C_Type *regs
)
{
  if ((regs->MCR & LPI2C_MCR_MEN_MASK) == 0) {
    regs->MCR |= LPI2C_MCR_MEN_MASK;
  }

  imxrt_lpi2c_next_msg(bus, regs);
}

static int imxrt_lpi2c_transfer(i2c_bus *base, i2c_msg *msgs, uint32_t n)
{
  struct imxrt_lpi2c_bus *bus;
  volatile LPI2C_Type *regs;
  int supported_flags;
  int eno;
  uint16_t i;

  bus = (struct imxrt_lpi2c_bus *) base;
  regs = bus->regs;

  supported_flags = I2C_M_RD | I2C_M_STOP;

  for (i = 0; i < n; ++i) {
    if ((msgs[i].flags & ~supported_flags) != 0) {
      return -EINVAL;
    }

    supported_flags |= I2C_M_NOSTART;
  }

  eno = imxrt_lpi2c_wait_for_not_busy(regs);
  if (eno != 0) {
    imxrt_lpi2c_do_reinit(bus, regs);
    return -eno;
  }

  bus->msg_todo = n;
  bus->msg = &msgs[0];
  bus->eno = 0;

  imxrt_lpi2c_first_msg(bus, regs);

  eno = rtems_binary_semaphore_wait_timed_ticks(&bus->sem, bus->base.timeout);
  if (eno != 0) {
    /* Timeout */
    imxrt_lpi2c_do_reinit(bus, regs);
    rtems_binary_semaphore_try_wait(&bus->sem);
    return -eno;
  }

  return -bus->eno;
}

static void imxrt_lpi2c_destroy(i2c_bus *base)
{
  struct imxrt_lpi2c_bus *bus;
  volatile LPI2C_Type *regs;

  bus = (struct imxrt_lpi2c_bus *) base;
  regs = bus->regs;
  imxrt_lpi2c_sw_reset(regs);
  CLOCK_DisableClock(bus->clock_ip);

  rtems_interrupt_handler_remove(bus->irq, imxrt_lpi2c_interrupt, bus);
  i2c_bus_destroy_and_free(&bus->base);
}

static int imxrt_lpi2c_hw_init(struct imxrt_lpi2c_bus *bus)
{
  rtems_status_code sc;
  volatile LPI2C_Type *regs;

  regs = bus->regs;

  CLOCK_EnableClock(bus->clock_ip);

  bus->clock = I2C_BUS_CLOCK_DEFAULT;
  imxrt_lpi2c_do_reinit(bus, regs);

  sc = rtems_interrupt_handler_install(
    bus->irq,
    "LPI2C",
    RTEMS_INTERRUPT_UNIQUE,
    imxrt_lpi2c_interrupt,
    bus
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return EAGAIN;
  }

  return 0;
}

static uint32_t imxrt_lpi2c_get_src_freq(clock_ip_name_t clock_ip)
{
  uint32_t freq;
#if IMXRT_IS_MIMXRT10xx
  uint32_t mux;
  uint32_t divider;

  (void) clock_ip; /* Not necessary for i.MXRT1050 */

  mux = CLOCK_GetMux(kCLOCK_Lpi2cMux);
  divider = 1;

  switch (mux) {
  case 0: /* pll3_sw_clk */
    freq = CLOCK_GetFreq(kCLOCK_Usb1PllClk);
    divider = 8;
    break;
  case 1: /* OSC */
    freq = CLOCK_GetFreq(kCLOCK_OscClk);
    break;
  default:
    freq = 0;
  }

  divider *= CLOCK_GetDiv(kCLOCK_Lpi2cDiv) + 1;
  freq /= divider;
#elif IMXRT_IS_MIMXRT11xx
  /*
   * FIXME: A future version of the mcux_sdk might provide a better method to
   * get the clock instead of this hack.
   */
  clock_root_t clock_root = clock_ip + kCLOCK_Root_Lpi2c1 - kCLOCK_Lpi2c1;

  freq = CLOCK_GetRootClockFreq(clock_root);
#else
  #error Getting I2C frequency is not implemented for this chip.
#endif

  return freq;
}

static clock_ip_name_t imxrt_lpi2c_clock_ip(volatile LPI2C_Type *regs)
{
  LPI2C_Type *const base_addresses[] = LPI2C_BASE_PTRS;
  static const clock_ip_name_t lpi2c_clocks[] = LPI2C_CLOCKS;
  size_t i;

  for (i = 0; i < RTEMS_ARRAY_SIZE(base_addresses); ++i) {
    if (base_addresses[i] == regs) {
      return lpi2c_clocks[i];
    }
  }

  return kCLOCK_IpInvalid;
}

void imxrt_lpi2c_init(void)
{
  const void *fdt;
  int node;

  fdt = bsp_fdt_get();
  node = -1;

  do {
    node = fdt_node_offset_by_compatible(fdt, node, "nxp,imxrt-lpi2c");

    if (node >= 0 && imxrt_fdt_node_is_enabled(fdt, node)) {
      struct imxrt_lpi2c_bus *bus;
      int eno;
      const char *bus_path;

      bus = (struct imxrt_lpi2c_bus*) i2c_bus_alloc_and_init(sizeof(*bus));
      if (bus == NULL) {
        bsp_fatal(IMXRT_FATAL_LPI2C_ALLOC_FAILED);
      }

      rtems_binary_semaphore_init(&bus->sem, "LPI2C");

      bus->regs = imx_get_reg_of_node(fdt, node);
      if (bus->regs == NULL) {
        (*bus->base.destroy)(&bus->base);
        bsp_fatal(IMXRT_FATAL_LPI2C_INVALID_FDT);
      }

      bus->irq = imx_get_irq_of_node(fdt, node, 0);
      if (bus->irq == BSP_INTERRUPT_VECTOR_INVALID) {
        (*bus->base.destroy)(&bus->base);
        bsp_fatal(IMXRT_FATAL_LPI2C_INVALID_FDT);
      }

      bus_path = fdt_getprop(fdt, node, "rtems,path", NULL);
      if (bus_path == NULL) {
        (*bus->base.destroy)(&bus->base);
        bsp_fatal(IMXRT_FATAL_LPI2C_INVALID_FDT);
      }

      bus->clock_ip = imxrt_lpi2c_clock_ip(bus->regs);
      bus->src_clock_hz = imxrt_lpi2c_get_src_freq(bus->clock_ip);

      eno = imxrt_lpi2c_hw_init(bus);
      if (eno != 0) {
        (*bus->base.destroy)(&bus->base);
        bsp_fatal(IMXRT_FATAL_LPI2C_HW_INIT_FAILED);
      }

      bus->base.transfer = imxrt_lpi2c_transfer;
      bus->base.set_clock = imxrt_lpi2c_set_clock;
      bus->base.destroy = imxrt_lpi2c_destroy;

      /*
       * Need at least three FIFO bytes:
       * 1. One to two data to transmit or receive.
       *    Two is necessary for long receives without NACK.
       * 2. A stop condition.
       */
      if ((1 << ((bus->regs->PARAM & LPI2C_PARAM_MTXFIFO_MASK) >>
          LPI2C_PARAM_MTXFIFO_SHIFT)) < 3) {
        bsp_fatal(IMXRT_FATAL_LPI2C_UNSUPPORTED_HARDWARE);
      }

      eno = i2c_bus_register(&bus->base, bus_path);
      if (eno != 0) {
        bsp_fatal(IMXRT_FATAL_LPI2C_REGISTER_FAILED);
      }
    }
  } while (node >= 0);
}
