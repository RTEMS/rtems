/**
 * @file
 *
 * @ingroup arm_beagle
 *
 * @brief BeagleBoard I2C bus initialization and API Support.
 */

/*
 * Copyright (c) 2016 Punit Vara <punitvara@gmail.com>
 * Copyright (c) 2017 Sichen Zhao <zsc19940506@gmail.com>
 * Copyright (c) 2019 Christian Mauderer <christian.mauderer@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/bspIo.h>
#include <stdio.h>
#include <bsp/i2c.h>
#include <libcpu/am335x.h>
#include <rtems/irq-extension.h>
#include <rtems/counter.h>
#include <bsp/bbb-gpio.h>
#include <rtems/score/assert.h>
#include <dev/i2c/i2c.h>
#include <ofw/ofw.h>

typedef struct bbb_i2c_bus {
  i2c_bus base;
  volatile bbb_i2c_regs *regs;
  struct {
    volatile uint32_t *ctrl_clkctrl;
    volatile uint32_t *i2c_clkctrl;
    volatile uint32_t *clkstctrl;
  } clkregs;
  rtems_id task_id;
  rtems_vector_number irq;
  i2c_msg *buffer;
  size_t buffer_pos;
  int error;
  uint32_t con_reg;
} bbb_i2c_bus;

#define TRANSFER_TIMEOUT_COUNT 100
#define FIFO_THRESHOLD 5
#define min(l,r) ((l) < (r) ? (l) : (r))
#if 0
#define debug_print(fmt, args...) printk("bbb-i2c: " fmt, ## args)
#else
#define debug_print(fmt, args...)
#endif
/*
 * Here we assume the number of i2c nodes
 * will be less than 100.
 */
#define PATH_LEN (strlen("/dev/i2c-xx") + 1)

static int am335x_i2c_fill_registers(
  bbb_i2c_bus *bus,
  uint32_t base
)
{
  bus->regs = (volatile bbb_i2c_regs *)base;

  /*
   * FIXME: Implement a clock driver to parse and setup clocks
   */
  switch ((intptr_t) bus->regs) {
  case AM335X_I2C0_BASE:
    bus->clkregs.ctrl_clkctrl = &REG(AM335X_SOC_CM_WKUP_REGS +
                                 AM335X_CM_WKUP_CONTROL_CLKCTRL);
    bus->clkregs.i2c_clkctrl = &REG(AM335X_SOC_CM_WKUP_REGS +
                                 AM335X_CM_WKUP_I2C0_CLKCTRL);
    bus->clkregs.clkstctrl = &REG(AM335X_SOC_CM_WKUP_REGS +
                                   AM335X_CM_WKUP_CLKSTCTRL);
    break;
  case AM335X_I2C1_BASE:
    bus->clkregs.ctrl_clkctrl = &REG(AM335X_SOC_CM_WKUP_REGS +
                                 AM335X_CM_WKUP_CONTROL_CLKCTRL);
    bus->clkregs.i2c_clkctrl = &REG(AM335X_CM_PER_ADDR +
                                 AM335X_CM_PER_I2C1_CLKCTRL);
    bus->clkregs.clkstctrl = NULL;
    break;
  case AM335X_I2C2_BASE:
    bus->clkregs.ctrl_clkctrl = &REG(AM335X_SOC_CM_WKUP_REGS +
                                 AM335X_CM_WKUP_CONTROL_CLKCTRL);
    bus->clkregs.i2c_clkctrl = &REG(AM335X_CM_PER_ADDR +
                                 AM335X_CM_PER_I2C2_CLKCTRL);
    bus->clkregs.clkstctrl = NULL;
    break;
  default:
    return EINVAL;
  }

  return 0;
}

static void am335x_i2c_module_clk_enable( bbb_i2c_bus *bus )
{
  volatile uint32_t *ctrl_clkctrl = bus->clkregs.ctrl_clkctrl;
  volatile uint32_t *i2c_clkctrl = bus->clkregs.i2c_clkctrl;
  volatile uint32_t *clkstctrl = bus->clkregs.clkstctrl;

  /* Writing to MODULEMODE field of AM335X_CM_WKUP_I2C0_CLKCTRL register. */
  *i2c_clkctrl |= AM335X_CM_WKUP_I2C0_CLKCTRL_MODULEMODE_ENABLE;

  /* Waiting for MODULEMODE field to reflect the written value. */
  while ( AM335X_CM_WKUP_I2C0_CLKCTRL_MODULEMODE_ENABLE !=
          ( *i2c_clkctrl & AM335X_CM_WKUP_I2C0_CLKCTRL_MODULEMODE ) )
  { /* busy wait */ }

  /*
   * Waiting for IDLEST field in AM335X_CM_WKUP_CONTROL_CLKCTRL
   * register to attain desired value.
   */
  while ( ( AM335X_CM_WKUP_CONTROL_CLKCTRL_IDLEST_FUNC <<
            AM335X_CM_WKUP_CONTROL_CLKCTRL_IDLEST_SHIFT ) !=
          ( *ctrl_clkctrl & AM335X_CM_WKUP_I2C0_CLKCTRL_IDLEST ) )
  { /* busy wait */ }

  if ( clkstctrl != NULL ) {
    /*
     * Waiting for CLKACTIVITY_I2C0_GFCLK field in AM335X_CM_WKUP_CLKSTCTRL
     * register to attain desired value.
     */
    while ( AM335X_CM_WKUP_CLKSTCTRL_CLKACTIVITY_I2C0_GFCLK !=
            ( *clkstctrl & AM335X_CM_WKUP_CLKSTCTRL_CLKACTIVITY_I2C0_GFCLK ) )
    { /* busy wait */ }
  }

  /*
   * Waiting for IDLEST field in AM335X_CM_WKUP_I2C0_CLKCTRL register to attain
   * desired value.
   */
  while ( ( AM335X_CM_WKUP_I2C0_CLKCTRL_IDLEST_FUNC <<
            AM335X_CM_WKUP_I2C0_CLKCTRL_IDLEST_SHIFT ) !=
          ( *i2c_clkctrl & AM335X_CM_WKUP_I2C0_CLKCTRL_IDLEST ) ) ;
}

static int am335x_i2c_set_clock(
  i2c_bus      *base,
  unsigned long clock
)
{
  bbb_i2c_bus           *bus = (bbb_i2c_bus *) base;
  uint32_t               prescaler, divider;

  prescaler = ( BBB_I2C_SYSCLK / BBB_I2C_INTERNAL_CLK ) - 1;
  bus->regs->BBB_I2C_PSC = prescaler;
  divider = BBB_I2C_INTERNAL_CLK / ( 2 * clock );
  bus->regs->BBB_I2C_SCLL = ( divider - 7 );
  bus->regs->BBB_I2C_SCLH = ( divider - 5 );

  return 0;
}

static int am335x_i2c_reset( bbb_i2c_bus *bus )
{
  volatile bbb_i2c_regs *regs = bus->regs;
  int                    timeout = 100;
  int                    err;

  bus->con_reg = 0;
  regs->BBB_I2C_CON = bus->con_reg;
  rtems_counter_delay_nanoseconds(50000000);

  regs->BBB_I2C_SYSC = AM335X_I2C_SYSC_SRST;
  rtems_counter_delay_nanoseconds(1000000);
  regs->BBB_I2C_CON = AM335X_I2C_CON_I2C_EN;

  while ( !( regs->BBB_I2C_SYSS & AM335X_I2C_SYSS_RDONE )
          && timeout >= 0 ) {
    --timeout;
    rtems_counter_delay_nanoseconds(100000);
  }

  if ( timeout <= 0 ) {
    debug_print( "ERROR: Timeout in soft-reset\n" );
    return ETIMEDOUT;
  }

  /* Disable again after reset */
  regs->BBB_I2C_CON = bus->con_reg;

  err = am335x_i2c_set_clock( &bus->base, I2C_BUS_CLOCK_DEFAULT );
  if (err) {
    return err;
  }

  regs->BBB_I2C_BUF = AM335X_I2C_BUF_TXTRSH(FIFO_THRESHOLD) |
                              AM335X_I2C_BUF_RXTRSH(FIFO_THRESHOLD);

  /* Enable the I2C controller in master mode. */
  bus->con_reg |= AM335X_I2C_CON_I2C_EN | AM335X_I2C_CON_MST;
  regs->BBB_I2C_CON = bus->con_reg;

  regs->BBB_I2C_IRQENABLE_SET =
      AM335X_I2C_IRQSTATUS_XDR | AM335X_I2C_IRQSTATUS_XRDY |
      AM335X_I2C_IRQSTATUS_RDR | AM335X_I2C_IRQSTATUS_RRDY |
      AM335X_I2C_IRQSTATUS_ARDY | AM335X_I2C_IRQSTATUS_NACK |
      AM335X_I2C_IRQSTATUS_AL;

  return 0;
}

/* Return true if done. */
static bool am335x_i2c_transfer_intr(bbb_i2c_bus *bus, uint32_t status)
{
  size_t i;
  size_t amount = 0;
  volatile bbb_i2c_regs *regs = bus->regs;

  /* Handle errors */
  if ((status & AM335X_I2C_IRQSTATUS_NACK) != 0) {
    debug_print("NACK\n");
    regs->BBB_I2C_IRQSTATUS = AM335X_I2C_IRQSTATUS_NACK;
    bus->error = ENXIO;
  } else if ((status & AM335X_I2C_IRQSTATUS_AL) != 0) {
    debug_print("Arbitration lost\n");
    regs->BBB_I2C_IRQSTATUS = AM335X_I2C_IRQSTATUS_AL;
    bus->error = ENXIO;
  }

  /* Transfer finished? */
  if ((status & AM335X_I2C_IRQSTATUS_ARDY) != 0) {
    debug_print("ARDY transaction complete\n");
    if (bus->error != 0 && (bus->buffer->flags & I2C_M_STOP) == 0) {
      regs->BBB_I2C_CON = bus->con_reg | AM335X_I2C_CON_STOP;
    }
    regs->BBB_I2C_IRQSTATUS = AM335X_I2C_IRQSTATUS_ARDY |
                              AM335X_I2C_IRQSTATUS_RDR |
                              AM335X_I2C_IRQSTATUS_RRDY |
                              AM335X_I2C_IRQSTATUS_XDR |
                              AM335X_I2C_IRQSTATUS_XRDY;
    return true;
  }

  if (bus->buffer->flags & I2C_M_RD) {
    if (status & AM335X_I2C_IRQSTATUS_RDR) {
      debug_print("RDR\n");
      /* last data received */
      amount = bus->buffer->len - bus->buffer_pos;
    } else if (status & AM335X_I2C_IRQSTATUS_RRDY) {
      debug_print("RRDY\n");
      /* FIFO threshold reached */
      amount = min(FIFO_THRESHOLD, bus->buffer->len - bus->buffer_pos);
    }

    debug_print("Read %d bytes\n", amount);
    for (i = 0; i < amount; i++) {
      bus->buffer->buf[bus->buffer_pos] = (uint8_t)(regs->BBB_I2C_DATA);
      ++bus->buffer_pos;
    }

    if (status & AM335X_I2C_IRQSTATUS_RDR) {
      regs->BBB_I2C_IRQSTATUS =AM335X_I2C_IRQSTATUS_RDR;
    }
    if (status & AM335X_I2C_IRQSTATUS_RRDY) {
      regs->BBB_I2C_IRQSTATUS =AM335X_I2C_IRQSTATUS_RRDY;
    }
  } else {
    if (status & AM335X_I2C_IRQSTATUS_XDR) {
      debug_print("XDR\n");
      /* Remaining TX data won't reach the FIFO threshold. */
      amount = bus->buffer->len - bus->buffer_pos;
    } else if (status & AM335X_I2C_IRQSTATUS_XRDY) {
      debug_print("XRDY\n");
      /* FIFO threshold reached */
      amount = min(FIFO_THRESHOLD, bus->buffer->len - bus->buffer_pos);
    }

    debug_print("Write %d bytes\n", amount);
    for (i = 0; i < amount; i++) {
      regs->BBB_I2C_DATA = bus->buffer->buf[bus->buffer_pos];
      ++bus->buffer_pos;
    }

    if (status & AM335X_I2C_IRQSTATUS_XDR) {
      regs->BBB_I2C_IRQSTATUS = AM335X_I2C_IRQSTATUS_XDR;
    }
    if (status & AM335X_I2C_IRQSTATUS_XRDY) {
      regs->BBB_I2C_IRQSTATUS = AM335X_I2C_IRQSTATUS_XRDY;
    }
  }

  return false;
}

static void am335x_i2c_interrupt( void *arg )
{
  bbb_i2c_bus *bus = arg;
  volatile bbb_i2c_regs *regs = bus->regs;
  uint32_t status;

  status = regs->BBB_I2C_IRQSTATUS;

  debug_print("interrupt: %08x\n", status);

  if (status == 0) {
    /* Why can this even happen? */
    return;
  }

  if (bus->buffer == NULL) {
    debug_print("Buffer is NULL\n");
    bus->error = EINVAL;
  }

  if (bus->buffer == NULL || am335x_i2c_transfer_intr(bus, status)) {
    rtems_status_code sc;
    sc = rtems_event_transient_send( bus->task_id );
    _Assert( sc == RTEMS_SUCCESSFUL );
    (void) sc; /* suppress warning in case of no assert */
  }
}

static int am335x_i2c_transfer(
  i2c_bus *base,
  i2c_msg *msgs,
  uint32_t nmsgs
)
{
  size_t i;
  int err = 0;
  bool repstart = false;
  int timeout = 0;
  bbb_i2c_bus *bus = (bbb_i2c_bus *) base;
  volatile bbb_i2c_regs *regs = bus->regs;
  uint32_t reg;
  rtems_status_code sc;

  bus->task_id = rtems_task_self();

  for (i = 0; i < nmsgs; i++) {
    bus->buffer = &msgs[i];
    bus->buffer_pos = 0;
    bus->error = 0;

    debug_print("processing %2d/%d: addr: 0x%04x, flags: 0x%04x, len: %d, buf: %p\n",
        i, nmsgs, msgs[i].addr, msgs[i].flags, msgs[i].len, msgs[i].buf);

    if (bus->buffer == NULL || bus->buffer->buf == NULL ||
        bus->buffer->len == 0) {
      err = EINVAL;
      break;
    }

    /*
     * Send START when bus is busy on repeated starts.
     * Otherwise wait some time.
     */
    if (!repstart) {
      timeout = 0;
      while ((regs->BBB_I2C_IRQSTATUS_RAW & AM335X_I2C_IRQSTATUS_BB) != 0
              && timeout <= TRANSFER_TIMEOUT_COUNT) {
        ++timeout;
        rtems_task_wake_after(RTEMS_MICROSECONDS_TO_TICKS(1000));
      }
      if (timeout > TRANSFER_TIMEOUT_COUNT) {
        err = EBUSY;
        break;
      }
      timeout = 0;
    } else {
      repstart = false;
    }

    if ((bus->buffer->flags & I2C_M_STOP) == 0) {
      repstart = true;
    }

    regs->BBB_I2C_SA = bus->buffer->addr;
    regs->BBB_I2C_CNT = bus->buffer->len;

    regs->BBB_I2C_BUF |= AM335X_I2C_BUF_RXFIFO_CLR | AM335X_I2C_BUF_TXFIFO_CLR;

    reg = bus->con_reg | AM335X_I2C_CON_START;
    if (!repstart) {
      reg |= AM335X_I2C_CON_STOP;
    }
    if ((bus->buffer->flags & I2C_M_RD) == 0) {
      reg |= AM335X_I2C_CON_TRX;
    }
    /* Implicit stop on last message. */
    if (i == nmsgs - 1) {
      reg |= AM335X_I2C_CON_STOP;
    }
    regs->BBB_I2C_CON = reg;

    sc = rtems_event_transient_receive( RTEMS_WAIT, bus->base.timeout );
    if ( sc != RTEMS_SUCCESSFUL ) {
      rtems_event_transient_clear();
      err = ETIMEDOUT;
      break;
    }
    if (bus->error) {
      err = bus->error;
      break;
    }
  }

  if (timeout == 0) {
    while ((regs->BBB_I2C_IRQSTATUS_RAW & AM335X_I2C_IRQSTATUS_BB) != 0
            && timeout <= TRANSFER_TIMEOUT_COUNT) {
      ++timeout;
      rtems_task_wake_after(RTEMS_MICROSECONDS_TO_TICKS(1000));
    }
  }

  if ((regs->BBB_I2C_CON & AM335X_I2C_CON_MST) == 0) {
    regs->BBB_I2C_CON = bus->con_reg;
  }

  bus->buffer = NULL;

  return -err;
}

static void am335x_i2c_destroy( i2c_bus *base )
{
  bbb_i2c_bus      *bus = (bbb_i2c_bus *) base;
  rtems_status_code sc;

  bus->regs->BBB_I2C_IRQENABLE_CLR = 0xFFFF;
  bus->regs->BBB_I2C_CON = 0;
  sc = rtems_interrupt_handler_remove( bus->irq, am335x_i2c_interrupt, bus );
  _Assert( sc == RTEMS_SUCCESSFUL );
  (void) sc;
  i2c_bus_destroy_and_free( &bus->base );
}

static int am335x_i2c_bus_register(
  uint32_t reg_base,
  rtems_vector_number irq,
  const char *bus_path
)
{
  bbb_i2c_bus        *bus;
  rtems_status_code   sc;
  int                 err;

  bus = (bbb_i2c_bus *) i2c_bus_alloc_and_init( sizeof( *bus ) );
  bus->irq = irq;

  if ( bus == NULL ) {
    return -1;
  }

  err = am335x_i2c_fill_registers(bus, reg_base);
  if (err != 0) {
    printf("i2c: invalid register base\n");
    ( *bus->base.destroy )( &bus->base );
    rtems_set_errno_and_return_minus_one( err );
  }

  am335x_i2c_module_clk_enable( bus );
  err = am335x_i2c_reset( bus );
  if (err != 0) {
    printk("i2c: reset timed out\n");
    ( *bus->base.destroy )( &bus->base );
    rtems_set_errno_and_return_minus_one( err );
  }

  sc = rtems_interrupt_handler_install(
    bus->irq,
    "BBB_I2C",
    RTEMS_INTERRUPT_UNIQUE,
    (rtems_interrupt_handler) am335x_i2c_interrupt,
    bus
  );

  if ( sc != RTEMS_SUCCESSFUL ) {
    ( *bus->base.destroy )( &bus->base );
    rtems_set_errno_and_return_minus_one( EIO );
  }

  bus->base.transfer = am335x_i2c_transfer;
  bus->base.set_clock = am335x_i2c_set_clock;
  bus->base.destroy = am335x_i2c_destroy;

  return i2c_bus_register( &bus->base, bus_path );
}

void beagle_i2c_init(phandle_t node)
{
  int                   err;
  int                   unit;
  char                  bus_path[PATH_LEN];
  rtems_vector_number   irq;
  rtems_ofw_memory_area reg;

  if (!rtems_ofw_is_node_compatible(node, "ti,omap4-i2c"))
    /* We cannot handle this device */
    return ;

  unit = beagle_get_node_unit(node);
  if (unit < 0 || unit >= 100) {
    printk("i2c: cannot register device, node unit number invalid.");
    printk(" Valid range is 0 <= unit < 100\n");
    return ;
  }

  err = rtems_ofw_get_prop(node, "rtems,path", (void *)bus_path, PATH_LEN);
  if (err < 0) {
    /* No path was provided in the device tree therefore use the default one */
    snprintf(bus_path, PATH_LEN, "/dev/i2c-%d", unit);
  } else if (err >= PATH_LEN) {
    /* Null terminate the string */
    bus_path[PATH_LEN - 1] = 0;
    printk("i2c: bus path too long, trucated %s\n", bus_path);
  }

  err = rtems_ofw_get_interrupts(node, &irq, sizeof(irq));
  if (err < 1) {
    printk("i2c: cannot register device, irq missing in device tree\n");
    return ;
  }

  err = rtems_ofw_get_reg(node, &reg, sizeof(reg));
  if (err <= 0) {
    printk("i2c: cannot register device, regs field missing\n");
    return ;
  }

  err = am335x_i2c_bus_register(reg.start, irq, bus_path);
  if (err != 0)
    printk("i2c: Could not register device (%d)\n", err);
}
