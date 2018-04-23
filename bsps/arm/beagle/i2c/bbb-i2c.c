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
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

/*
 * Modified on Punit Vara<punitvara@gmail.com> works, currently
 * the i2c file is working on the Beaglebone Black board(AM335x)
 */

#include <stdio.h>
#include <bsp/i2c.h>
#include <libcpu/am335x.h>
#include <rtems/irq-extension.h>
#include <rtems/counter.h>
#include <bsp/bbb-gpio.h>
#include <rtems/score/assert.h>

static void am335x_i2c0_pinmux( bbb_i2c_bus *bus )
{
  REG( bus->regs + AM335X_CONF_I2C0_SDA ) =
    ( BBB_RXACTIVE | BBB_SLEWCTRL | BBB_PU_EN );

  REG( bus->regs + AM335X_CONF_I2C0_SCL ) =
    ( BBB_RXACTIVE | BBB_SLEWCTRL | BBB_PU_EN );
}

static void I2C0ModuleClkConfig( void )
{
  /* Writing to MODULEMODE field of AM335X_CM_WKUP_I2C0_CLKCTRL register. */
  REG( AM335X_SOC_CM_WKUP_REGS + AM335X_CM_WKUP_I2C0_CLKCTRL ) |=
    AM335X_CM_WKUP_I2C0_CLKCTRL_MODULEMODE_ENABLE;

  /* Waiting for MODULEMODE field to reflect the written value. */
  while ( AM335X_CM_WKUP_I2C0_CLKCTRL_MODULEMODE_ENABLE !=
          ( REG( AM335X_SOC_CM_WKUP_REGS + AM335X_CM_WKUP_I2C0_CLKCTRL ) &
            AM335X_CM_WKUP_I2C0_CLKCTRL_MODULEMODE ) ) ;

  /*
   * Waiting for IDLEST field in AM335X_CM_WKUP_CONTROL_CLKCTRL
   * register to attain desired value.
   */
  while ( ( AM335X_CM_WKUP_CONTROL_CLKCTRL_IDLEST_FUNC <<
            AM335X_CM_WKUP_CONTROL_CLKCTRL_IDLEST_SHIFT ) !=
          ( REG( AM335X_SOC_CM_WKUP_REGS + AM335X_CM_WKUP_CONTROL_CLKCTRL ) &
            AM335X_CM_WKUP_CONTROL_CLKCTRL_IDLEST ) ) ;

  /*
   * Waiting for CLKACTIVITY_I2C0_GFCLK field in AM335X_CM_WKUP_CLKSTCTRL
   * register to attain desired value.
   */
  while ( AM335X_CM_WKUP_CLKSTCTRL_CLKACTIVITY_I2C0_GFCLK !=
          ( REG( AM335X_SOC_CM_WKUP_REGS + AM335X_CM_WKUP_CLKSTCTRL ) &
            AM335X_CM_WKUP_CLKSTCTRL_CLKACTIVITY_I2C0_GFCLK ) ) ;

  /*
   * Waiting for IDLEST field in AM335X_CM_WKUP_I2C0_CLKCTRL register to attain
   * desired value.
   */
  while ( ( AM335X_CM_WKUP_I2C0_CLKCTRL_IDLEST_FUNC <<
            AM335X_CM_WKUP_I2C0_CLKCTRL_IDLEST_SHIFT ) !=
          ( REG( AM335X_SOC_CM_WKUP_REGS + AM335X_CM_WKUP_I2C0_CLKCTRL ) &
            AM335X_CM_WKUP_I2C0_CLKCTRL_IDLEST ) ) ;
}

static void am335x_i2c_reset( bbb_i2c_bus *bus )
{
  volatile bbb_i2c_regs *regs = bus->regs;
  int                    timeout = I2C_TIMEOUT;

  if ( REG( &regs->BBB_I2C_CON ) & BBB_I2C_CON_EN ) {
    REG( &regs->BBB_I2C_CON ) = BBB_I2C_CON_CLR;
    udelay( 50000 );
  }

  REG( &regs->BBB_I2C_SYSC ) = BBB_I2C_SYSC_SRST; /* for ES2 after soft reset */
  udelay( 1000 );
  REG( &regs->BBB_I2C_CON ) = BBB_I2C_CON_EN;

  while ( !( REG( &regs->BBB_I2C_SYSS ) & BBB_I2C_SYSS_RDONE ) && timeout-- ) {
    if ( timeout <= 0 ) {
      puts( "ERROR: Timeout in soft-reset\n" );

      return;
    }

    udelay( 1000 );
  }
}
/*
 * Possible values for msg->flag
 * - @ref I2C_M_TEN,
 * - @ref I2C_M_RD,
 * - @ref I2C_M_STOP,
 * - @ref I2C_M_NOSTART,
 * - @ref I2C_M_REV_DIR_ADDR,
 * - @ref I2C_M_IGNORE_NAK,
 * - @ref I2C_M_NO_RD_ACK, and
 * - @ref I2C_M_RECV_LEN.
 */

static void am335x_i2c_set_address_size(
  const i2c_msg         *msgs,
  volatile bbb_i2c_regs *regs
)
{
  /*
   * Can be configured multiple modes here.
   * Need to think about own address modes
   */
  if ( ( msgs->flags & I2C_M_TEN ) == 0 ) {
    /* 7-bit mode slave address mode */
    REG( &regs->BBB_I2C_CON ) = AM335X_I2C_CFG_7BIT_SLAVE_ADDR;
  } else {
    /* 10-bit slave address mode */
    REG( &regs->BBB_I2C_CON ) = AM335X_I2C_CFG_10BIT_SLAVE_ADDR;
  }
}

static void am335x_i2c_next_byte( bbb_i2c_bus *bus )
{
  i2c_msg *msg;

  ++bus->msgs;
  --bus->msg_todo;
  msg = &bus->msgs[ 0 ];
  bus->current_msg_todo = msg->len;
  bus->current_msg_byte = msg->buf;
}

static void am335x_i2c_masterint_enable(
  volatile bbb_i2c_regs *regs,
  unsigned int           flag
)
{
  REG( &regs->BBB_I2C_IRQENABLE_SET ) |= flag;
}

static void am335x_i2c_masterint_disable(
  volatile bbb_i2c_regs *regs,
  unsigned int           flag
)
{
  REG( &regs->BBB_I2C_IRQENABLE_CLR ) = flag;
}

static void am335x_int_clear(
  volatile bbb_i2c_regs *regs,
  unsigned int           flag
)
{
  REG( &regs->BBB_I2C_IRQSTATUS ) = flag;
}

static void am335x_clean_interrupts( volatile bbb_i2c_regs *regs )
{
  am335x_i2c_masterint_enable( regs, BBB_I2C_ALL_FLAGS );
  am335x_int_clear( regs, BBB_I2C_ALL_FLAGS );
  am335x_i2c_masterint_disable( regs, BBB_I2C_ALL_FLAGS );
}

static void am335x_i2c_setup_read_transfer(
  bbb_i2c_bus           *bus,
  volatile bbb_i2c_regs *regs,
  const i2c_msg         *msgs,
  bool                   send_stop
)
{
  REG( &regs->BBB_I2C_CNT ) = bus->current_msg_todo;

  REG( &regs->BBB_I2C_CON ) = AM335X_I2C_CFG_MST_RX | AM335X_I2C_CON_I2C_EN;

  if ( send_stop ) {
    REG( &regs->BBB_I2C_CON ) |= AM335X_I2C_CON_START | AM335X_I2C_CON_STOP;
  } else {
    REG( &regs->BBB_I2C_CON ) |= AM335X_I2C_CON_START;
  }

  am335x_i2c_masterint_enable( regs, AM335X_I2C_INT_RECV_READY |
    AM335X_I2C_IRQSTATUS_ARDY );
}

static void am335x_i2c_continue_read_transfer(
  bbb_i2c_bus           *bus,
  volatile bbb_i2c_regs *regs
)
{
  bus->current_msg_byte[ bus->already_transferred ] =
    REG( &regs->BBB_I2C_DATA );

  bus->already_transferred++;

  REG( &regs->BBB_I2C_IRQSTATUS ) = AM335X_I2C_INT_RECV_READY;

  if ( bus->already_transferred == bus->current_msg_todo - 1 ) {
    REG( &regs->BBB_I2C_CON ) |= AM335X_I2C_CON_STOP;
  }
}

static void am335x_i2c_continue_write(
  bbb_i2c_bus           *bus,
  volatile bbb_i2c_regs *regs
)
{
  if ( bus->already_transferred == bus->msg_todo ) {
    REG( &regs->BBB_I2C_DATA ) =
      bus->current_msg_byte[ bus->already_transferred ];
    REG( &regs->BBB_I2C_IRQSTATUS ) = AM335X_I2C_IRQSTATUS_XRDY;
    am335x_i2c_masterint_disable( regs, AM335X_I2C_IRQSTATUS_XRDY );
    REG( &regs->BBB_I2C_CON ) |= AM335X_I2C_CON_STOP;
  } else {
    writeb( bus->current_msg_byte[ bus->already_transferred ],
      &regs->BBB_I2C_DATA );
    REG( &regs->BBB_I2C_IRQSTATUS ) = AM335X_I2C_IRQSTATUS_XRDY;
    bus->already_transferred++;
  }
}

static void am335x_i2c_setup_write_transfer(
  bbb_i2c_bus           *bus,
  volatile bbb_i2c_regs *regs,
  const i2c_msg         *msgs
)
{
  volatile unsigned int no_bytes;

  REG( &regs->BBB_I2C_CNT ) = bus->current_msg_todo;
  no_bytes = REG( &regs->BBB_I2C_CNT );
  (void) no_bytes; /* indicate we know that no_bytes is not referenced again */
  REG( &regs->BBB_I2C_SA ) = msgs->addr;
  REG( &regs->BBB_I2C_CON ) = AM335X_I2C_CFG_MST_TX | AM335X_I2C_CON_I2C_EN;
  am335x_clean_interrupts( regs );
  am335x_i2c_masterint_enable( regs, AM335X_I2C_IRQSTATUS_XRDY );
  REG( &regs->BBB_I2C_CON ) |= AM335X_I2C_CON_START | AM335X_I2C_CON_STOP;
}

static void am335x_i2c_setup_transfer(
  bbb_i2c_bus           *bus,
  volatile bbb_i2c_regs *regs
)
{
  const i2c_msg *msgs = bus->msgs;
  uint32_t       msg_todo = bus->msg_todo;
  bool           send_stop = false;
  uint32_t       i;

  bus->current_todo = msgs[ 0 ].len;

  for ( i = 1; i < msg_todo && ( msgs[ i ].flags & I2C_M_NOSTART ) != 0;
        ++i ) {
    bus->current_todo += msgs[ i ].len;
  }

  regs = bus->regs;
  REG( &bus->regs->BBB_I2C_BUF ) |= AM335X_I2C_BUF_TXFIFO_CLR;
  REG( &bus->regs->BBB_I2C_BUF ) |= AM335X_I2C_BUF_RXFIFO_CLR;
  am335x_i2c_set_address_size( msgs, regs );
  bus->read = ( msgs->flags & I2C_M_RD ) != 0;
  bus->already_transferred = ( bus->read == true ) ? 0 : 1;

  if ( bus->read ) {
    if ( bus->current_msg_todo == 1 ) {
      send_stop = true;
    }

    am335x_i2c_setup_read_transfer( bus, regs, msgs, send_stop );
  } else {
    am335x_i2c_setup_write_transfer( bus, regs, msgs );
  }
}

static void am335x_i2c_interrupt( void *arg )
{
  bbb_i2c_bus           *bus = arg;
  volatile bbb_i2c_regs *regs = bus->regs;
  /* Get status of enabled interrupts */
  uint32_t irqstatus = REG( &regs->BBB_I2C_IRQSTATUS );
  bool     done = false;

  /*
   * Clear all enabled interrupt except receive ready
   * and transmit ready interrupt in status register
   */
  REG( &regs->BBB_I2C_IRQSTATUS ) =
    ( irqstatus & ~( AM335X_I2C_IRQSTATUS_RRDY |
                     AM335X_I2C_IRQSTATUS_XRDY ) );

  if ( irqstatus & AM335X_I2C_INT_RECV_READY ) {
    am335x_i2c_continue_read_transfer( bus, regs );
  }

  if ( irqstatus & AM335X_I2C_IRQSTATUS_XRDY ) {
    am335x_i2c_continue_write( bus, regs );
  }

  if ( irqstatus & AM335X_I2C_IRQSTATUS_NACK ) {
    done = true;
    am335x_i2c_masterint_disable( regs, AM335X_I2C_IRQSTATUS_NACK );
  }

  if ( irqstatus & AM335X_I2C_IRQSTATUS_ARDY ) {
    done = true;
    REG( &regs->BBB_I2C_IRQSTATUS ) = BBB_I2C_STAT_ARDY;
  }

  if ( irqstatus & AM335X_I2C_IRQSTATUS_BF ) {
    REG( &regs->BBB_I2C_IRQSTATUS ) = AM335X_I2C_IRQSTATUS_BF;
  }

  if ( done ) {
    uint32_t err = irqstatus & BBB_I2C_IRQ_ERROR;
    am335x_i2c_next_byte( bus );

    if ( bus->msg_todo == 0 ) {
      rtems_status_code sc;
      am335x_i2c_masterint_disable( regs, ( AM335X_I2C_IRQSTATUS_RRDY |
                                            AM335X_I2C_IRQSTATUS_XRDY |
                                            AM335X_I2C_IRQSTATUS_BF ) );
      REG( &regs->BBB_I2C_IRQSTATUS ) = err;

      sc = rtems_event_transient_send( bus->task_id );
      _Assert( sc == RTEMS_SUCCESSFUL );
      (void) sc;
    } else {
      am335x_i2c_setup_transfer( bus, regs );
    }
  }
}

static int am335x_i2c_transfer(
  i2c_bus *base,
  i2c_msg *msgs,
  uint32_t msg_count
)
{
  rtems_status_code      sc;
  bbb_i2c_bus           *bus = (bbb_i2c_bus *) base;
  volatile bbb_i2c_regs *regs;
  uint32_t               i;

  rtems_task_wake_after( 1 );

  if ( msg_count < 1 ) {
    return 1;
  }

  for ( i = 0; i < msg_count; ++i ) {
    if ( ( msgs[ i ].flags & I2C_M_RECV_LEN ) != 0 ) {
      return -EINVAL;
    }
  }

  bus->msgs = &msgs[ 0 ];
  bus->msg_todo = msg_count;
  bus->current_msg_todo = msgs[ 0 ].len;
  bus->current_msg_byte = msgs[ 0 ].buf;
  bus->task_id = rtems_task_self();
  regs = bus->regs;
  am335x_i2c_setup_transfer( bus, regs );
  REG( &regs->BBB_I2C_IRQENABLE_SET ) = BBB_I2C_IRQ_USED;

  sc = rtems_event_transient_receive( RTEMS_WAIT, bus->base.timeout );

  if ( sc != RTEMS_SUCCESSFUL ) {
    am335x_i2c_reset( bus );
    rtems_event_transient_clear();

    return -ETIMEDOUT;
  }

  return 0;
}

static int am335x_i2c_set_clock(
  i2c_bus      *base,
  unsigned long clock
)
{
  bbb_i2c_bus           *bus = (bbb_i2c_bus *) base;
  uint32_t               prescaler, divider;

  prescaler = ( BBB_I2C_SYSCLK / BBB_I2C_INTERNAL_CLK ) - 1;
  REG( &bus->regs->BBB_I2C_PSC ) = prescaler;
  divider = BBB_I2C_INTERNAL_CLK / ( 2 * clock );
  REG( &bus->regs->BBB_I2C_SCLL ) = ( divider - 7 );
  REG( &bus->regs->BBB_I2C_SCLH ) = ( divider - 5 );

  return 0;
}

static void am335x_i2c_destroy( i2c_bus *base )
{
  bbb_i2c_bus      *bus = (bbb_i2c_bus *) base;
  rtems_status_code sc;

  sc = rtems_interrupt_handler_remove( bus->irq, am335x_i2c_interrupt, bus );
  _Assert( sc == RTEMS_SUCCESSFUL );
  (void) sc;
  i2c_bus_destroy_and_free( &bus->base );
}

int am335x_i2c_bus_register(
  const char         *bus_path,
  uintptr_t           register_base,
  uint32_t            input_clock,
  rtems_vector_number irq
)
{
  bbb_i2c_bus      *bus;
  rtems_status_code sc;
  int               err;

  /* Check bus number is >0 & <MAX */
  bus = (bbb_i2c_bus *) i2c_bus_alloc_and_init( sizeof( *bus ) );

  if ( bus == NULL ) {
    return -1;
  }

  bus->regs = (volatile bbb_i2c_regs *) register_base;

  I2C0ModuleClkConfig();
  am335x_i2c0_pinmux( bus );
  am335x_i2c_reset( bus );
  bus->input_clock = input_clock;
  err = am335x_i2c_set_clock( &bus->base, I2C_BUS_CLOCK_DEFAULT );

  if ( err != 0 ) {
    ( *bus->base.destroy )( &bus->base );
    rtems_set_errno_and_return_minus_one( -err );
  }

  bus->irq = irq;
  REG( &bus->regs->BBB_I2C_IRQSTATUS ) = BBB_I2C_ALL_IRQ_FLAGS;

  sc = rtems_interrupt_handler_install(
    irq,
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
