/*
 * Copyright (c) 2016-2017 Chris Johns <chrisj@rtems.org>  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

/*
 * Xilinx AXI IIC Interface v2.0. See PG090.pdf.
 *
 * Note, only master support is provided and no dynamic mode by design.
 */

#include <stdarg.h>

#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/irq-extension.h>
#include <rtems/score/assert.h>

#include <dev/i2c/i2c.h>
#include <dev/i2c/xilinx-axi-i2c.h>

/*
 * Register map.
 */
#define REG_GIE                    0x01c
#define REG_ISR                    0x020
#define REG_IER                    0x028
#define REG_SOFTR                  0x040
#define REG_CR                     0x100
#define REG_SR                     0x104
#define REG_TX_FIFO                0x108
#define REG_RX_FIFO                0x10c
#define REG_ADR                    0x110
#define REG_TX_FIFO_OCY            0x114
#define REG_RX_FIFO_OCY            0x118
#define REG_TEN_ADR                0x11c
#define REG_RX_FIFO_PIRQ           0x120
#define REG_GPO                    0x124
#define REG_TSUSTA                 0x128
#define REG_TSUSTO                 0x12c
#define REG_THDSTA                 0x130
#define REG_TSUDAT                 0x134
#define REG_TBUF                   0x138
#define REG_THIGH                  0x13c
#define REG_TLOW                   0x140
#define REG_THDDAT                 0x144

/*
 * Interrupts.
 */
#define INT_ARB_LOST               (1 << 0)
#define INT_TX_ERROR               (1 << 1)
#define INT_TX_FIFO_EMPTY          (1 << 2)
#define INT_RX_FIFO_FULL           (1 << 3)
#define INT_BUS_NOT_BUSY           (1 << 4)
#define INT_ADDRESSED_AS_SLAVE     (1 << 5)
#define INT_NOT_ADDRESSED_AS_SLAVE (1 << 6)
#define INT_TX_FIFO_HALF_FULL      (1 << 7)
#define INT_ALL                    (0xff)

/*
 * Command register.
 */
#define CR_EN                      (1 << 0)
#define CR_TX_FIFO_RESET           (1 << 1)
#define CR_MSMS                    (1 << 2)
#define CR_TX                      (1 << 3)
#define CR_TXAK                    (1 << 4)
#define CR_RSTA                    (1 << 5)
#define CR_GC_EN                   (1 << 6)

/*
 * Status register.
 */
#define SR_ABGC                    (1 << 0)
#define SR_AAS                     (1 << 1)
#define SR_BB                      (1 << 2)
#define SR_SRW                     (1 << 3)
#define SR_TX_FIFO_FULL            (1 << 4)
#define SR_RX_FIFO_FULL            (1 << 5)
#define SR_RX_FIFO_EMPTY           (1 << 6)
#define SR_TX_FIFO_EMPTY           (1 << 7)

/*
 * FIFO Sizes.
 */
#define TX_FIFO_SIZE      16
#define TX_FIFO_HALF_SIZE (TX_FIFO_SIZE / 2)
#define RX_FIFO_SIZE      16

/*
 * Address flags.
 */
#define ADDR_TEN  (1 << 31)
#define ADDR_GPO  (1 << 30)

typedef struct {
  i2c_bus               base;
  uintptr_t             regs;
  i2c_msg*              msgs;
  uint32_t              msgs_remaining;
  i2c_msg*              current_msg;
  uint32_t              current_msg_todo;
  uint8_t*              current_msg_byte;
  uint32_t              current_todo;
  uint32_t              irqstatus;
  bool                  read;
  uint32_t              addr;
  rtems_id              task_id;
  bool                  gpo_address;
  xilinx_aix_i2c_timing timing;
  rtems_vector_number   irq;
} xilinx_axi_i2c_bus;

xilinx_axi_i2c_bus* axi_i2c_bus;

/*
 * Real-time trace buffering with a small overhead. The data can be dumped from
 * gdb with:
 *
 * define axi-i2c
 *  set $i = 0
 *  while $i < axi_trace_in
 *    printf "%4d %08x %08x %08x : ", \
 *     $i, axi_trace[$i].vars[0], axi_trace[$i].vars[1], axi_trace[$i].vars[2]
 *    output axi_trace[$i].state
 *    printf "\n"
 *    set $i = $i + 1
 *  end
 * end
 */

typedef enum
{
  AXI_I2C_NOP,
  AXI_I2C_BEGIN,
  AXI_I2C_END,
  AXI_I2C_TRANSFER,
  AXI_I2C_ADDRESS,
  AXI_I2C_START_TRANSFER,
  AXI_I2C_WRITE,
  AXI_I2C_READ,
  AXI_I2C_TX_FIFO,
  AXI_I2C_RX_FIFO,
  AXI_I2C_RX_FIFO_LEVEL,
  AXI_I2C_INT,
  AXI_I2C_INT_DONE,
  AXI_I2C_INT_ERROR,
  AXI_I2C_BUS_NOT_BUSY,
  AXI_I2C_REG_WRITE,
  AXI_I2C_REG_READ,
  AXI_I2C_TIMEOUT,
  AXI_I2C_WAKE
} axi_i2c_state;

#define RTEMS_AXI_I2C_TRACE 0
#if RTEMS_AXI_I2C_TRACE

#define DRIVER_REG_TRACE 1

typedef struct
{
  axi_i2c_state state;
  uint32_t      vars[3];
} axi_i2c_trace;

#define AXI_I2C_TRACE 5000
axi_i2c_trace axi_trace[AXI_I2C_TRACE];
int axi_trace_in;

static inline void axi_trace_reset(void)
{
  axi_trace_in = 0;
}

static inline void axi_trace_append(axi_i2c_state state,
                                    uint32_t      v1,
                                    uint32_t      v2,
                                    uint32_t      v3)
{
  if (axi_trace_in < AXI_I2C_TRACE) {
    axi_i2c_trace rec = { state, { v1, v2, v3 } };
    axi_trace[axi_trace_in++] = rec;
  }
}
#else
#define axi_trace_reset()
#define axi_trace_append(s, v1, v2, v3)
#endif

#define DRIVER_DEBUG 0
#define DRIVER_DEBUG_DEFAULT true
#if DRIVER_DEBUG
#ifndef RTEMS_PRINTFLIKE
#define RTEMS_PRINTFLIKE( _format_pos, _ap_pos ) \
__attribute__((__format__(__printf__, _format_pos, _ap_pos)))
#endif
static bool drv_printk_enable = DRIVER_DEBUG_DEFAULT;
static void drv_printk(const char* format, ...) RTEMS_PRINTFLIKE(1, 2);
static void
drv_printk(const char* format, ...)
{
  va_list ap;
  va_start(ap, format);
  if (drv_printk_enable)
    vprintk(format, ap);
  va_end(ap);
}
#else
#define drv_printk(_fmt, ...)
#endif

static inline void
xilinx_axi_i2c_reg_write(const xilinx_axi_i2c_bus* bus, uint32_t reg, uint32_t value)
{
  #if DRIVER_REG_TRACE
    axi_trace_append(AXI_I2C_REG_WRITE, reg, value, 0);
  #endif
  *((volatile uint32_t*) (bus->regs + reg))= value;
}

static inline uint32_t
xilinx_axi_i2c_reg_read(const xilinx_axi_i2c_bus* bus, uint32_t reg)
{
  uint32_t value = *((volatile uint32_t*) (bus->regs + reg));
  #if DRIVER_REG_TRACE
    axi_trace_append(AXI_I2C_REG_READ, reg, value, 0);
  #endif
  return value;
}

static uint32_t
xilinx_axi_i2c_read_irq_status(const xilinx_axi_i2c_bus* bus)
{
  return xilinx_axi_i2c_reg_read(bus, REG_ISR);
}

static uint32_t
xilinx_axi_i2c_read_irq_enabled(const xilinx_axi_i2c_bus* bus)
{
  return xilinx_axi_i2c_reg_read(bus, REG_IER);
}

static void
xilinx_axi_i2c_clear_irq(const xilinx_axi_i2c_bus* bus, uint32_t mask)
{
  /*
   * The ISR bits can be toggled so only write a 1 if set.
   */
  xilinx_axi_i2c_reg_write(bus, REG_ISR,
                           xilinx_axi_i2c_reg_read(bus, REG_ISR) & mask);
}

static inline void
xilinx_axi_i2c_enable_irq(const xilinx_axi_i2c_bus* bus, uint32_t mask)
{
  xilinx_axi_i2c_reg_write(bus, REG_IER,
                           xilinx_axi_i2c_reg_read(bus, REG_IER) | mask);
}

static inline void
xilinx_axi_i2c_disable_irq(const xilinx_axi_i2c_bus* bus, uint32_t mask)
{
  xilinx_axi_i2c_reg_write(bus, REG_IER,
                           xilinx_axi_i2c_reg_read(bus, REG_IER) & ~mask);
}

static void
xilinx_axi_i2c_clear_enable_irq(const xilinx_axi_i2c_bus* bus, uint32_t mask)
{
  xilinx_axi_i2c_clear_irq(bus, mask);
  xilinx_axi_i2c_enable_irq(bus, mask);
}

static void
xilinx_axi_i2c_disable_clear_irq(const xilinx_axi_i2c_bus* bus, uint32_t mask)
{
  xilinx_axi_i2c_disable_irq(bus, mask);
  xilinx_axi_i2c_clear_irq(bus, mask);
}

static inline void
xilinx_axi_i2c_disable_all_irq(const xilinx_axi_i2c_bus* bus)
{
  xilinx_axi_i2c_reg_write(bus, REG_GIE, 0);
}

static void
xilinx_axi_i2c_enable_interrupts(const xilinx_axi_i2c_bus* bus)
{
  xilinx_axi_i2c_reg_write(bus, REG_GIE, 1 << 31);
}

static void
xilinx_axi_i2c_disable_interrupts(const xilinx_axi_i2c_bus* bus)
{
  xilinx_axi_i2c_reg_write(bus, REG_GIE, 0);
  xilinx_axi_i2c_reg_write(bus, REG_IER, 0);
}

static inline void
xilinx_axi_i2c_write_cr(const xilinx_axi_i2c_bus* bus, uint32_t value)
{
  xilinx_axi_i2c_reg_write(bus, REG_CR, value);
}

static inline uint32_t
xilinx_axi_i2c_read_cr(const xilinx_axi_i2c_bus* bus)
{
  return xilinx_axi_i2c_reg_read(bus, REG_CR);
}

static inline void
xilinx_axi_i2c_set_cr(const xilinx_axi_i2c_bus* bus, uint32_t mask)
{
  xilinx_axi_i2c_reg_write(bus, REG_CR,
                           xilinx_axi_i2c_reg_read(bus, REG_CR) | mask);
}

static inline void
xilinx_axi_i2c_clear_cr(const xilinx_axi_i2c_bus* bus, uint32_t mask)
{
  xilinx_axi_i2c_write_cr(bus, xilinx_axi_i2c_read_cr(bus) & ~mask);
}

static inline uint32_t
xilinx_axi_i2c_read_sr(const xilinx_axi_i2c_bus* bus)
{
  return xilinx_axi_i2c_reg_read(bus, REG_SR);
}

static inline uint32_t
xilinx_axi_i2c_read_rx_level(const xilinx_axi_i2c_bus* bus)
{
  if ((xilinx_axi_i2c_read_sr(bus) & SR_RX_FIFO_EMPTY) != 0)
    return 0;
  return xilinx_axi_i2c_reg_read(bus, REG_RX_FIFO_OCY) + 1;
}

static inline void
xilinx_axi_i2c_write_rx_pirq(const xilinx_axi_i2c_bus* bus, uint32_t level)
{
  if (level != 0)
    xilinx_axi_i2c_reg_write(bus, REG_RX_FIFO_PIRQ, level - 1);
  else
    xilinx_axi_i2c_reg_write(bus, REG_RX_FIFO_PIRQ, level);
}

static inline uint32_t
xilinx_axi_i2c_read_tx_space(const xilinx_axi_i2c_bus* bus)
{
  if ((xilinx_axi_i2c_read_sr(bus) & SR_TX_FIFO_EMPTY) != 0)
    return TX_FIFO_SIZE;
  return TX_FIFO_SIZE - xilinx_axi_i2c_reg_read(bus, REG_TX_FIFO_OCY) - 1;
}

static void
xilinx_axi_i2c_write_tx_fifo_data(xilinx_axi_i2c_bus* bus, uint32_t data)
{
  axi_trace_append(AXI_I2C_TX_FIFO, data, 0, 0);
  xilinx_axi_i2c_reg_write(bus, REG_TX_FIFO, data);
}

static inline uint32_t
xilinx_axi_i2c_read_rx_fifo_data(xilinx_axi_i2c_bus* bus)
{
  uint32_t data = xilinx_axi_i2c_reg_read(bus, REG_RX_FIFO);
  axi_trace_append(AXI_I2C_RX_FIFO, data, 0, 0);
  return data;
}

static void
xilinx_axi_i2c_reset(xilinx_axi_i2c_bus* bus)
{
  xilinx_axi_i2c_reg_write(bus, REG_SOFTR, 0x0a);
  if ((bus->timing.valid_mask & XILINX_AIX_I2C_ALL_REGS) != 0)
  {
    static const uint32_t r[8] = {
      REG_TSUSTA,
      REG_TSUSTO,
      REG_THDSTA,
      REG_TSUDAT,
      REG_TBUF,
      REG_THIGH,
      REG_TLOW,
      REG_THDDAT
    };
    static const uint32_t m[8] = {
      XILINX_AIX_I2C_TSUSTA,
      XILINX_AIX_I2C_TSUSTO,
      XILINX_AIX_I2C_THDSTA,
      XILINX_AIX_I2C_TSUDAT,
      XILINX_AIX_I2C_TBUF,
      XILINX_AIX_I2C_THIGH,
      XILINX_AIX_I2C_TLOW,
      XILINX_AIX_I2C_THDDAT
    };
    uint32_t        vm = bus->timing.valid_mask;
    const uint32_t* u = &bus->timing.TSUSTA;
    size_t          i;
    for (i = 0; i < (sizeof(r) / sizeof(r[0])); ++i, ++u) {
      if ((vm & m[i]) != 0) {
        xilinx_axi_i2c_reg_write(bus, r[i], *u);
      }
    }
  }
}


static void
xilinx_axi_i2c_reinit(xilinx_axi_i2c_bus* bus)
{
  drv_printk("axi-i2c: reinit\n");
  xilinx_axi_i2c_reset(bus);
  xilinx_axi_i2c_write_rx_pirq(bus, RX_FIFO_SIZE);
  xilinx_axi_i2c_write_cr(bus, CR_TX_FIFO_RESET);
  xilinx_axi_i2c_write_cr(bus, CR_EN);
  xilinx_axi_i2c_clear_enable_irq(bus, INT_ARB_LOST);
}

static void
xilinx_axi_i2c_wakeup(xilinx_axi_i2c_bus* bus)
{
  axi_trace_append(AXI_I2C_WAKE, bus->task_id, bus->irqstatus, 0);
  drv_printk("axi-i2c: wakeup: irqstatus: %08lx\n", bus->irqstatus);
  rtems_status_code sc = rtems_event_transient_send(bus->task_id);
  _Assert(sc == RTEMS_SUCCESSFUL);
  (void) sc;
}

static void
xilinx_axi_i2c_next_byte(xilinx_axi_i2c_bus* bus)
{
  --bus->current_todo;
  --bus->current_msg_todo;
  ++bus->current_msg_byte;
  if (bus->current_msg_todo == 0) {
    if (bus->msgs_remaining != 0 &&
        (bus->msgs[0].flags & I2C_M_NOSTART) != 0) {
      bus->current_msg_todo = bus->msgs[0].len;
      bus->current_msg_byte = bus->msgs[0].buf;
      ++bus->msgs;
      --bus->msgs_remaining;
    }
  }
}

static void
xilinx_axi_i2c_read_rx_byte(xilinx_axi_i2c_bus* bus)
{
  *bus->current_msg_byte = (uint8_t) xilinx_axi_i2c_read_rx_fifo_data(bus);
  xilinx_axi_i2c_next_byte(bus);
}

static void
xilinx_axi_i2c_read_rx_bytes(xilinx_axi_i2c_bus* bus, uint32_t count)
{
  while (count-- > 0)
    xilinx_axi_i2c_read_rx_byte(bus);
}

static void
xilinx_axi_i2c_set_rx_fifo_level(xilinx_axi_i2c_bus* bus)
{
  uint32_t size;
  if (bus->current_todo > RX_FIFO_SIZE) {
    size = RX_FIFO_SIZE;
  } else {
    size = bus->current_todo - 1;
  }
  axi_trace_append(AXI_I2C_RX_FIFO_LEVEL, size, 0, 0);
  xilinx_axi_i2c_write_rx_pirq(bus, size);
}

static bool xilinx_axi_i2c_start_transfer(xilinx_axi_i2c_bus* bus);

static bool
xilinx_axi_i2c_read_rx_fifo(xilinx_axi_i2c_bus* bus)
{
  drv_printk("axi-i2c: read rx fifo: length:%lu\n", bus->current_todo);

  if (bus->current_todo == 0) {
    return false;
  }

  if ((xilinx_axi_i2c_read_sr(bus) & SR_RX_FIFO_EMPTY) == 0) {
    uint32_t level = xilinx_axi_i2c_read_rx_level(bus);
    bool     active;

    drv_printk("axi-i2c: read rx fifo: level:%lu\n", level);

    if (level > bus->current_todo)
      level = bus->current_todo;

    switch (bus->current_todo - level) {
      case 1:
        drv_printk("axi-i2c: read rx fifo: one more\n");
        /*
         * One more byte to be received. This is set up by programming the RX
         * FIFO programmable depth interrupt register with a value that is 2
         * less than the number we need (the register is minus 1). When we have
         * one byte left, disable the TX error interrupt because setting the NO
         * ACK bit in the command register causes a TX error interrupt. Set the
         * TXAK bit in the CR to not-acknowledge the next byte received telling
         * the slave sender the master accepts no more data, then read the
         * FIFO. If the FIFO is ready before the TXAK bit is set the slave will
         * see a request for more data. We will come back to the next case
         * statement for the last byte once it has been received.
         */
        xilinx_axi_i2c_disable_clear_irq(bus, INT_TX_ERROR);
        xilinx_axi_i2c_set_cr(bus, CR_TXAK);
        xilinx_axi_i2c_read_rx_bytes(bus, level);
        /*
         * Set the RX PIRQ to 0 after the RX data has been read. There is an
         * observed timing issue and glitch if written before.
         */
        xilinx_axi_i2c_write_rx_pirq(bus, 0);
        break;

      case 0:
        drv_printk("axi-i2c: read rx fifo: no more\n");
        /*
         * We should have 1 byte in the FIFO which is the last byte received
         * with a NACK. If there are no more message we need to send a STOP by
         * clearing he MSMS bit in the CR and then waiting for the bus to not
         * be busy.
         */
        xilinx_axi_i2c_disable_clear_irq(bus,
                                         INT_RX_FIFO_FULL | INT_TX_ERROR);
        if (bus->msgs_remaining == 0) {
          xilinx_axi_i2c_clear_cr(bus, CR_MSMS);
          xilinx_axi_i2c_clear_enable_irq(bus, INT_BUS_NOT_BUSY);
          active = true;
        }
        xilinx_axi_i2c_read_rx_byte(bus);
        if (bus->msgs_remaining != 0)
          active = xilinx_axi_i2c_start_transfer(bus);
        return active;

      default:
        drv_printk("axi-i2c: read rx fifo: more:%lu\n", bus->current_todo - level);
        /*
         * All the requested data is in the FIFO so read it and update the PIRQ
         * level. The PIRQ size is always one less than the maximum size.
         */
        xilinx_axi_i2c_read_rx_bytes(bus, level);
        if (bus->current_todo > RX_FIFO_SIZE) {
          xilinx_axi_i2c_write_rx_pirq(bus, RX_FIFO_SIZE);
        } else {
          xilinx_axi_i2c_write_rx_pirq(bus, bus->current_todo - 1);
        }
        break;
    }
  }

  return true;
}

static void
xilinx_axi_i2c_write_tx_byte(xilinx_axi_i2c_bus* bus)
{
  xilinx_axi_i2c_write_tx_fifo_data(bus, *bus->current_msg_byte);
  xilinx_axi_i2c_next_byte(bus);
}

static void
xilinx_axi_i2c_write_tx_bytes(xilinx_axi_i2c_bus* bus)
{
  uint32_t space = xilinx_axi_i2c_read_tx_space(bus);
  uint32_t level = bus->current_todo - 1;
  uint32_t i;
  drv_printk("axi-i2c: tx fifo load: space:%lu level:%lu\n", space, level);
  if (level < space)
    space = level;
  for (i = 0; i < space; ++i)
    xilinx_axi_i2c_write_tx_byte(bus);
}

static bool
xilinx_axi_i2c_write_tx_fifo(xilinx_axi_i2c_bus* bus)
{
  bool more = true;
  drv_printk("axi-i2c: write tx fifo: current_todo: %lu\n", bus->current_todo);
  switch (bus->current_todo) {
    case 0:
      xilinx_axi_i2c_disable_clear_irq(bus,
                                       INT_TX_FIFO_EMPTY |
                                       INT_TX_FIFO_HALF_FULL |
                                       INT_TX_ERROR |
                                       INT_BUS_NOT_BUSY);
      more = xilinx_axi_i2c_start_transfer(bus);
      break;
    case 1:
      /*
       * If transmitting and the last byte issue a stop and wait for the bus to
       * not be busy.
       */
      if (!bus->read && bus->msgs_remaining == 0) {
        xilinx_axi_i2c_clear_cr(bus, CR_MSMS);
        xilinx_axi_i2c_clear_enable_irq(bus, INT_BUS_NOT_BUSY);
      }
      xilinx_axi_i2c_write_tx_byte(bus);
      break;
    default:
      xilinx_axi_i2c_write_tx_bytes(bus);
      break;
  }
  return more;
}

static void
xilinx_axi_i2c_write_address(xilinx_axi_i2c_bus* bus)
{
  if ((bus->addr & ADDR_GPO) != 0)
    xilinx_axi_i2c_reg_write(bus, REG_GPO, (bus->addr >> 12) & 0xf);
  if ((bus->addr & ADDR_TEN) != 0)
    xilinx_axi_i2c_write_tx_fifo_data(bus, (bus->addr >> 8) & 0xff);
  xilinx_axi_i2c_write_tx_fifo_data(bus, bus->addr & 0xff);
}

static void
xilinx_axi_i2c_start_read(xilinx_axi_i2c_bus* bus)
{
  uint32_t cr;
  uint32_t set = INT_RX_FIFO_FULL;
  axi_trace_append(AXI_I2C_READ, bus->current_todo, 0, 0);
  drv_printk("axi-i2c: start read: size: %lu\n", bus->current_todo);
  /*
   * Is this a restart? If there is no active STOP it is a restart.
   */
  cr = xilinx_axi_i2c_read_cr(bus);
  if ((cr & CR_MSMS) != 0) {
    cr |= CR_RSTA;
    xilinx_axi_i2c_write_cr(bus, cr);
  }
  xilinx_axi_i2c_write_address(bus);
  xilinx_axi_i2c_set_rx_fifo_level(bus);
  /*
   * We must NACK the last byte so if we are receiving a single byte issue a
   * NACK.
   */
  cr &= ~(CR_TX | CR_TXAK);
  if (bus->current_todo == 1) {
    cr |= CR_TXAK;
  } else {
    set |= INT_TX_ERROR;
  }
  /*
   * Issue a start.
   */
  cr |= CR_MSMS;
  xilinx_axi_i2c_clear_enable_irq(bus, set);
  xilinx_axi_i2c_write_cr(bus, cr);
}

static void
xilinx_axi_i2c_start_write(xilinx_axi_i2c_bus* bus)
{
  uint32_t space;
  uint32_t enable;
  uint32_t cr;
  axi_trace_append(AXI_I2C_WRITE, bus->current_todo, 0, 0);
  cr = xilinx_axi_i2c_read_cr(bus);
  /*
   * If a master issue a restart if there is no active STOP on the bus.
   */
  if ((cr & CR_MSMS) != 0) {
    cr |= CR_RSTA;
    xilinx_axi_i2c_write_cr(bus, cr);
  }
  xilinx_axi_i2c_write_address(bus);
  if (bus->current_todo > 1)
    xilinx_axi_i2c_write_tx_bytes(bus);
  space = xilinx_axi_i2c_read_tx_space(bus);
  enable = INT_TX_FIFO_EMPTY | INT_TX_ERROR;
  if (space > TX_FIFO_HALF_SIZE && bus->current_todo > 1) {
    enable |= INT_TX_FIFO_HALF_FULL;
  }
  xilinx_axi_i2c_clear_enable_irq(bus, enable);
  cr &= ~CR_TXAK;
  cr |= CR_MSMS | CR_TX;
  xilinx_axi_i2c_write_cr(bus, cr);
}

static bool
xilinx_axi_i2c_start_transfer(xilinx_axi_i2c_bus* bus)
{
  const i2c_msg* msgs = bus->msgs;
  uint32_t       msg_todo = bus->msgs_remaining;
  uint32_t       i;

  axi_trace_append(AXI_I2C_START_TRANSFER, msg_todo, 0, 0);
  drv_printk("axi-i2c: start transfer: messages: %lu\n", msg_todo);

  if (msg_todo == 0) {
    xilinx_axi_i2c_clear_cr(bus, CR_MSMS);
    return false;
  }

  /*
   * Get the amount of data to transfer. It can span message buffers if the
   * I2C_M_NOSTART flag is set.
   */
  bus->current_todo = msgs[0].len;
  for (i = 1; i < msg_todo && (msgs[i].flags & I2C_M_NOSTART) != 0; ++i) {
    bus->current_todo += msgs[i].len;
  }

  bus->read = (msgs->flags & I2C_M_RD) != 0;

  if ((msgs->flags & I2C_M_TEN) != 0) {
    bus->addr = (ADDR_TEN |
                 ((msgs->addr & (3 << 8)) << 1) |
                 ((bus->read ? 1 : 0) << 8) |
                 (msgs->addr & 0xff));
  }
  else {
    bus->addr = (msgs->addr & 0x7f) << 1 | (bus->read ? 1 : 0);
  }

  if (bus->gpo_address)
    bus->addr |= ADDR_GPO | (msgs->addr & 0xf000);

  axi_trace_append(AXI_I2C_TRANSFER,
                   bus->msgs_remaining,
                   bus->current_todo, bus->addr);

  /*
   * The bus->msgs is left pointing to the next message because we may need to
   * start a new message while completing the current message.
   */
  bus->current_msg_todo = msgs[0].len;
  bus->current_msg_byte = msgs[0].buf;
  ++bus->msgs;
  --bus->msgs_remaining;

  if (bus->read) {
    xilinx_axi_i2c_start_read(bus);
  } else {
    xilinx_axi_i2c_start_write(bus);
  }

  return true;
}

static void
  xilinx_axi_i2c_interrupt(void* arg)
{
  xilinx_axi_i2c_bus* bus = arg;
  uint32_t            status = xilinx_axi_i2c_read_irq_status(bus);
  uint32_t            enabled = xilinx_axi_i2c_read_irq_enabled(bus);
  uint32_t            active = status & enabled;
  uint32_t            clear = 0;
  int                 done = 0;

  axi_trace_append(AXI_I2C_INT, active, status, enabled);

  drv_printk("axi-i2c: interrupt: active:%02lx isr:%02lx ier:%02lx\n",
             active, status, enabled);

  /*
   * An error or we lost arbitration. If transmitting and there is more data to
   * send a INT_TX_ERROR means the slave issue a NOT ACK because there was not
   * slave at the address or the addressed slave will not accept any more data.
   *
   * Clean up and wake the user.
   */
  if (((active & INT_ARB_LOST) != 0) ||
      (!bus->read && (active & INT_TX_ERROR) != 0)) {
    bus->irqstatus = active & (INT_ARB_LOST | INT_TX_ERROR);
    axi_trace_append(AXI_I2C_INT_ERROR, bus->irqstatus, 0, 0);
    xilinx_axi_i2c_reinit(bus);
    xilinx_axi_i2c_clear_cr(bus, CR_EN);
    xilinx_axi_i2c_wakeup(bus);
    return;
  }

  /*
   * RX FIFO full?
   */
  if ((active & INT_RX_FIFO_FULL) != 0) {
    clear |= INT_RX_FIFO_FULL;

    if (bus->read && !xilinx_axi_i2c_read_rx_fifo(bus)) {
      ++done;
      axi_trace_append(AXI_I2C_INT_DONE, done, clear, 0);
    }

    if (bus->current_todo == 0) {
      clear |= status & INT_TX_ERROR;
    }
  }

  /*
   * TX FIFO empty or half empty?
   */
  if ((active & (INT_TX_FIFO_EMPTY | INT_TX_FIFO_HALF_FULL)) != 0) {
    clear |= active & (INT_TX_FIFO_EMPTY | INT_TX_FIFO_HALF_FULL);

    if (!bus->read && !xilinx_axi_i2c_write_tx_fifo(bus)) {
      ++done;
      axi_trace_append(AXI_I2C_INT_DONE, done, clear, 1);
    }
  }

  /*
   * Gate the bus not busy interrupt with the bus busy status to know the bus
   * is really not busy. It could be an interrupt left over from starting the
   * transmission.
   */
  if ((active & INT_BUS_NOT_BUSY) != 0) {
    if ((xilinx_axi_i2c_read_sr(bus) & SR_BB) == 0) {
      xilinx_axi_i2c_disable_clear_irq(bus, INT_BUS_NOT_BUSY);
      if (bus->read && !xilinx_axi_i2c_read_rx_fifo(bus)) {
        ++done;
        axi_trace_append(AXI_I2C_BUS_NOT_BUSY, done, clear, 0);
      }
      else if (!bus->read && !xilinx_axi_i2c_write_tx_fifo(bus)) {
        ++done;
        axi_trace_append(AXI_I2C_BUS_NOT_BUSY, done, clear, 1);
      }
    }
    else {
      clear |= INT_BUS_NOT_BUSY;
    }
  }

  if (clear != 0)
    xilinx_axi_i2c_clear_irq(bus, clear);

  if (done != 0) {
    xilinx_axi_i2c_disable_interrupts(bus);
    xilinx_axi_i2c_clear_cr(bus, CR_EN);
    xilinx_axi_i2c_wakeup(bus);
  }
}

static int xilinx_axi_i2c_transfer(i2c_bus* base,
                                   i2c_msg* msgs,
                                   uint32_t msg_count)
{
  xilinx_axi_i2c_bus* bus = (xilinx_axi_i2c_bus *) base;
  rtems_status_code   sc;
  int                 r = 0;

  axi_trace_reset();
  axi_trace_append(AXI_I2C_BEGIN, msg_count, 0, 0);

  drv_printk("axi-i2c: i2c transfer\n");

  _Assert(msg_count > 0);

  bus->msgs = &msgs[0];
  bus->msgs_remaining = msg_count;
  bus->irqstatus = 0;
  bus->task_id = rtems_task_self();

  xilinx_axi_i2c_reinit(bus);
  xilinx_axi_i2c_start_transfer(bus);
  xilinx_axi_i2c_enable_interrupts(bus);

  sc = rtems_event_transient_receive(RTEMS_WAIT, bus->base.timeout);
  if (sc != RTEMS_SUCCESSFUL) {
    axi_trace_append(AXI_I2C_TIMEOUT, 0, 0, 0);
    xilinx_axi_i2c_reinit(bus);
    rtems_event_transient_clear();
    r = -ETIMEDOUT;
  }

  if (r == 0 && bus->irqstatus != 0)
    r = -EIO;

  axi_trace_append(AXI_I2C_END, bus->irqstatus, r, 0);

  return r;
}

static int xilinx_axi_i2c_set_clock(i2c_bus *base, unsigned long clock)
{
  xilinx_axi_i2c_bus* bus = (xilinx_axi_i2c_bus*) base;

  if ((bus->timing.valid_mask & XILINX_AIX_I2C_AXI_CLOCK) == 0)
    return -EIO;

  bus->timing.THIGH =
    (bus->timing.AXI_CLOCK / (2 * clock)) - 7 - bus->timing.SCL_INERTIAL_DELAY;
  bus->timing.TLOW = bus->timing.THIGH;

  bus->timing.valid_mask |= XILINX_AIX_I2C_THIGH | XILINX_AIX_I2C_TLOW;

  return 0;
}

static void xilinx_axi_i2c_destroy(i2c_bus* base)
{
  xilinx_axi_i2c_bus* bus = (xilinx_axi_i2c_bus*) base;
  rtems_status_code sc;

  sc = rtems_interrupt_handler_remove(bus->irq, xilinx_axi_i2c_interrupt, bus);
  _Assert(sc == RTEMS_SUCCESSFUL);
  (void) sc;

  i2c_bus_destroy_and_free(&bus->base);
}

int
i2c_bus_register_xilinx_aix_i2c(const char*                  bus_path,
                                uintptr_t                    register_base,
                                rtems_vector_number          irq,
                                bool                         gpo_address,
                                const xilinx_aix_i2c_timing* timing)
{
  xilinx_axi_i2c_bus* bus;
  rtems_status_code   sc;

  bus = (xilinx_axi_i2c_bus*) i2c_bus_alloc_and_init(sizeof(*bus));
  if (bus == NULL) {
    return -1;
  }

  bus->regs = register_base;
  bus->irq = irq;
  bus->gpo_address = gpo_address;
  bus->timing = *timing;

  sc = rtems_interrupt_handler_install(irq,
                                       "Xilinx AXI I2C",
                                       RTEMS_INTERRUPT_UNIQUE,
                                       xilinx_axi_i2c_interrupt,
                                       bus);
  if (sc != RTEMS_SUCCESSFUL) {
    drv_printk("axi-i2c: interrupt attach failed\n");
    (*bus->base.destroy)(&bus->base);
    rtems_set_errno_and_return_minus_one(EIO);
  }

  bus->base.transfer = xilinx_axi_i2c_transfer;
  bus->base.set_clock = xilinx_axi_i2c_set_clock;
  bus->base.destroy = xilinx_axi_i2c_destroy;

  axi_i2c_bus = bus;

  return i2c_bus_register(&bus->base, bus_path);
}
