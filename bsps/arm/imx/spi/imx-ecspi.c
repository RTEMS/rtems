/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/fdt.h>
#include <libfdt.h>
#include <arm/freescale/imx/imx_ccmvar.h>
#include <arm/freescale/imx/imx_ecspireg.h>
#include <dev/spi/spi.h>
#include <rtems/irq-extension.h>
#include <sys/param.h>
#include <sys/endian.h>

#define IMX_ECSPI_FIFO_SIZE 64

typedef struct imx_ecspi_bus imx_ecspi_bus;

struct imx_ecspi_bus {
  spi_bus base;
  volatile imx_ecspi *regs;
  uint32_t conreg;
  uint32_t speed_hz;
  uint32_t mode;
  uint8_t bits_per_word;
  uint8_t cs;
  uint32_t msg_todo;
  const spi_ioc_transfer *msg;
  uint32_t todo;
  uint32_t in_transfer;
  uint8_t *rx_buf;
  const uint8_t *tx_buf;
  void (*push)(imx_ecspi_bus *, volatile imx_ecspi *);
  void (*pop)(imx_ecspi_bus *, volatile imx_ecspi *);
  rtems_id task_id;
  rtems_vector_number irq;
};

static bool imx_ecspi_is_rx_fifo_not_empty(volatile imx_ecspi *regs)
{
  return (regs->statreg & IMX_ECSPI_RR) != 0;
}

static void imx_ecspi_reset(volatile imx_ecspi *regs)
{
  while (imx_ecspi_is_rx_fifo_not_empty(regs)) {
    regs->rxdata;
  }
}

static void imx_ecspi_done(imx_ecspi_bus *bus)
{
  rtems_event_transient_send(bus->task_id);
}

#define IMC_ECSPI_PUSH(type) \
static void imx_ecspi_push_##type(imx_ecspi_bus *bus, volatile imx_ecspi *regs) \
{ \
  type val = 0; \
  if (bus->tx_buf != NULL) { \
    val = *(type *)bus->tx_buf; \
    bus->tx_buf += sizeof(type); \
  } \
  bus->todo -= sizeof(type); \
  regs->txdata = val; \
}

#define IMX_ECSPI_POP(type) \
static void imx_ecspi_pop_##type(imx_ecspi_bus *bus, volatile imx_ecspi *regs) \
{ \
  uint32_t val = regs->rxdata; \
  if (bus->rx_buf != NULL) { \
    *(type *)bus->rx_buf = val; \
    bus->rx_buf += sizeof(type); \
  } \
}

IMC_ECSPI_PUSH(uint8_t)
IMX_ECSPI_POP(uint8_t)
IMC_ECSPI_PUSH(uint16_t)
IMX_ECSPI_POP(uint16_t)
IMC_ECSPI_PUSH(uint32_t)
IMX_ECSPI_POP(uint32_t)

static void imx_ecspi_push_uint32_t_swap(
  imx_ecspi_bus *bus,
  volatile imx_ecspi *regs
)
{
  uint32_t val = 0;

  if (bus->tx_buf != NULL) {
    val = bswap32(*(uint32_t *)bus->tx_buf);
    bus->tx_buf += sizeof(uint32_t);
  }

  bus->todo -= sizeof(uint32_t);
  regs->txdata = val;
}

static void imx_ecspi_pop_uint32_t_swap(
  imx_ecspi_bus *bus,
  volatile imx_ecspi *regs
)
{
  uint32_t val = regs->rxdata;

  if (bus->rx_buf != NULL) {
    *(uint32_t *)bus->rx_buf = bswap32(val);
    bus->rx_buf += sizeof(uint32_t);
  }
}

static void imx_ecspi_push(imx_ecspi_bus *bus, volatile imx_ecspi *regs)
{
  while (bus->todo > 0 && bus->in_transfer < IMX_ECSPI_FIFO_SIZE) {
    (*bus->push)(bus, regs);
    ++bus->in_transfer;
  }
}

static uint32_t imx_ecspi_conreg_divider(imx_ecspi_bus *bus, uint32_t speed_hz)
{
  uint32_t post;
  uint32_t pre;
  uint32_t clk_in;

  clk_in = bus->base.max_speed_hz;

  if (clk_in > speed_hz) {
    post = fls((int) clk_in) - fls((int) speed_hz);

    if (clk_in > (speed_hz << post)) {
      ++post;
    }

    /* We have 2^4 == 16, use the pre-divider for this factor */
    post = MAX(4, post) - 4;

    if (post <= 0xf) {
      pre = howmany(clk_in, speed_hz << post) - 1;
    } else {
      post = 0xf;
      pre = 0xf;
    }
  } else {
    post = 0;
    pre = 0;
  }

  return IMX_ECSPI_CONREG_POST_DIVIDER(post)
    | IMX_ECSPI_CONREG_PRE_DIVIDER(pre);
}

static void imx_ecspi_config(
  imx_ecspi_bus *bus,
  volatile imx_ecspi *regs,
  uint32_t speed_hz,
  uint8_t bits_per_word,
  uint32_t mode,
  uint8_t cs
)
{
  uint32_t conreg;
  uint32_t testreg;
  uint32_t configreg;
  uint32_t dmareg;
  uint32_t cs_bit;

  conreg = IMX_ECSPI_CONREG_CHANNEL_MODE(0xf)
    | IMX_ECSPI_CONREG_SMC | IMX_ECSPI_CONREG_EN;
  testreg = regs->testreg;
  configreg = regs->configreg;
  dmareg = regs->dmareg;
  cs_bit = 1U << cs;

  conreg |= imx_ecspi_conreg_divider(bus, speed_hz);
  conreg |= IMX_ECSPI_CONREG_CHANNEL_SELECT(cs);

  configreg |= IMX_ECSPI_CONFIGREG_SS_CTL(cs_bit);

  if ((mode & SPI_CPHA) != 0) {
    configreg |= IMX_ECSPI_CONFIGREG_SCLK_PHA(cs_bit);
  } else {
    configreg &= ~IMX_ECSPI_CONFIGREG_SCLK_PHA(cs_bit);
  }

  if ((mode & SPI_CPOL) != 0) {
    configreg |= IMX_ECSPI_CONFIGREG_SCLK_POL(cs_bit);
    configreg |= IMX_ECSPI_CONFIGREG_SCLK_CTL(cs_bit);
  } else {
    configreg &= ~IMX_ECSPI_CONFIGREG_SCLK_POL(cs_bit);
    configreg &= ~IMX_ECSPI_CONFIGREG_SCLK_CTL(cs_bit);
  }

  if ((mode & SPI_CS_HIGH) != 0) {
    configreg |= IMX_ECSPI_CONFIGREG_SS_POL(cs_bit);
  } else {
    configreg &= ~IMX_ECSPI_CONFIGREG_SS_POL(cs_bit);
  }

  if ((mode & SPI_LOOP) != 0) {
    testreg |= IMX_ECSPI_TESTREG_LBC;
  } else {
    testreg &= ~IMX_ECSPI_TESTREG_LBC;
  }

  dmareg = IMX_ECSPI_DMAREG_TX_THRESHOLD_SET(dmareg, IMX_ECSPI_FIFO_SIZE/2);

  regs->conreg = conreg;
  regs->testreg = testreg;
  regs->dmareg = dmareg;
  regs->configreg = configreg;

  bus->conreg = conreg;
  bus->speed_hz = speed_hz;
  bus->bits_per_word = bits_per_word;
  bus->mode = mode;
  bus->cs = cs;

  /* FIXME: Clock change delay */
}

static void imx_ecspi_set_push_pop(
  imx_ecspi_bus *bus,
  uint32_t len,
  uint8_t bits_per_word
)
{
  uint32_t conreg;

  conreg = bus->conreg;

  if (len % 4 == 0 && len <= IMX_ECSPI_FIFO_SIZE) {
    conreg |= IMX_ECSPI_CONREG_BURST_LENGTH((len * 8) - 1);

    bus->push = imx_ecspi_push_uint32_t_swap;
    bus->pop = imx_ecspi_pop_uint32_t_swap;
  } else {
    conreg |= IMX_ECSPI_CONREG_BURST_LENGTH(bits_per_word - 1);

    if (bits_per_word <= 8) {
      bus->push = imx_ecspi_push_uint8_t;
      bus->pop = imx_ecspi_pop_uint8_t;
    } else if (bits_per_word <= 16) {
      bus->push = imx_ecspi_push_uint16_t;
      bus->pop = imx_ecspi_pop_uint16_t;
    } else {
      bus->push = imx_ecspi_push_uint32_t;
      bus->pop = imx_ecspi_pop_uint32_t;
    }
  }

  bus->regs->conreg = conreg;
}

static void imx_ecspi_next_msg(imx_ecspi_bus *bus, volatile imx_ecspi *regs)
{
  if (bus->msg_todo > 0) {
    const spi_ioc_transfer *msg;

    msg = bus->msg;

    if (
      msg->speed_hz != bus->speed_hz
        || msg->bits_per_word != bus->bits_per_word
        || msg->mode != bus->mode
        || msg->cs != bus->cs
    ) {
      imx_ecspi_config(
        bus,
        regs,
        msg->speed_hz,
        msg->bits_per_word,
        msg->mode,
        msg->cs
      );
    }

    bus->todo = msg->len;
    bus->rx_buf = msg->rx_buf;
    bus->tx_buf = msg->tx_buf;
    imx_ecspi_set_push_pop(bus, msg->len, msg->bits_per_word);
    imx_ecspi_push(bus, regs);
    regs->intreg = IMX_ECSPI_TE | IMX_ECSPI_TDR;
  } else {
    regs->intreg = 0;
    imx_ecspi_done(bus);
  }
}

static void imx_ecspi_interrupt(void *arg)
{
  imx_ecspi_bus *bus;
  volatile imx_ecspi *regs;

  bus = arg;
  regs = bus->regs;

  while (imx_ecspi_is_rx_fifo_not_empty(regs)) {
    (*bus->pop)(bus, regs);
    --bus->in_transfer;
  }

  if (bus->todo > 0) {
    imx_ecspi_push(bus, regs);
  } else if (bus->in_transfer > 0) {
    regs->intreg = IMX_ECSPI_RR;
  } else {
    --bus->msg_todo;
    ++bus->msg;
    imx_ecspi_next_msg(bus, regs);
  }
}

static int imx_ecspi_transfer(
  spi_bus *base,
  const spi_ioc_transfer *msgs,
  uint32_t n
)
{
  imx_ecspi_bus *bus;

  bus = (imx_ecspi_bus *) base;

  bus->msg_todo = n;
  bus->msg = &msgs[0];
  bus->task_id = rtems_task_self();

  imx_ecspi_next_msg(bus, bus->regs);
  rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  return 0;
}

static void imx_ecspi_destroy(spi_bus *base)
{
  imx_ecspi_bus *bus;

  bus = (imx_ecspi_bus *) base;
  rtems_interrupt_handler_remove(bus->irq, imx_ecspi_interrupt, bus);
  spi_bus_destroy_and_free(&bus->base);
}

static int imx_ecspi_init(imx_ecspi_bus *bus, const void *fdt, int node)
{
  rtems_status_code sc;
  int len;
  const uint32_t *val;

  imx_ecspi_config(
    bus,
    bus->regs,
    bus->base.max_speed_hz,
    8,
    0,
    0
  );
  imx_ecspi_reset(bus->regs);

  sc = rtems_interrupt_handler_install(
    bus->irq,
    "ECSPI",
    RTEMS_INTERRUPT_UNIQUE,
    imx_ecspi_interrupt,
    bus
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return EAGAIN;
  }

  val = fdt_getprop(fdt, node, "pinctrl-0", &len);
  if (len == 4) {
    imx_iomux_configure_pins(fdt, fdt32_to_cpu(val[0]));
  }

  return 0;
}

static int imx_ecspi_setup(spi_bus *base)
{
  imx_ecspi_bus *bus;

  bus = (imx_ecspi_bus *) base;

  if (
    bus->base.speed_hz > imx_ccm_ipg_hz()
      || bus->base.bits_per_word > 32
  ) {
    return -EINVAL;
  }

  imx_ecspi_config(
    bus,
    bus->regs,
    bus->base.speed_hz,
    bus->base.bits_per_word,
    bus->base.mode,
    bus->base.cs
  );
  return 0;
}

int spi_bus_register_imx(const char *bus_path, const char *alias_or_path)
{
  const void *fdt;
  const char *path;
  int node;
  imx_ecspi_bus *bus;
  int eno;

  fdt = bsp_fdt_get();
  path = fdt_get_alias(fdt, alias_or_path);

  if (path == NULL) {
    path = alias_or_path;
  }

  node = fdt_path_offset(fdt, path);
  if (node < 0) {
    rtems_set_errno_and_return_minus_one(ENXIO);
  }

  bus = (imx_ecspi_bus *) spi_bus_alloc_and_init(sizeof(*bus));
  if (bus == NULL){
    return -1;
  }

  bus->base.max_speed_hz = imx_ccm_ecspi_hz();
  bus->base.delay_usecs = 1;
  bus->regs = imx_get_reg_of_node(fdt, node);
  bus->irq = imx_get_irq_of_node(fdt, node, 0);

  eno = imx_ecspi_init(bus, fdt, node);
  if (eno != 0) {
    (*bus->base.destroy)(&bus->base);
    rtems_set_errno_and_return_minus_one(eno);
  }

  bus->base.transfer = imx_ecspi_transfer;
  bus->base.destroy = imx_ecspi_destroy;
  bus->base.setup = imx_ecspi_setup;

  return spi_bus_register(&bus->base, bus_path);
}
