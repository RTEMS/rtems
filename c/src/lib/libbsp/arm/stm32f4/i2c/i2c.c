/*
 * Copyright (c) 2013 Christian Mauderer.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

/* The I2C-module can not run with libi2c. The reason for this is, that libi2c
 * needs a possibility to generate a stop condition separately. This controller
 * wants to generate the condition automatically when sending or receiving data.
 */

#include <bsp.h>
#include <bsp/i2c.h>
#include <bsp/rcc.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <assert.h>

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <rtems/status-checks.h>

#define STM32F4_I2C_INITIAL_BITRATE 100000

#define I2C_RW_BIT 0x1

stm32f4_rcc_index i2c_rcc_index [] = {
  STM32F4_RCC_I2C1,
  STM32F4_RCC_I2C2,
};

static stm32f4_rcc_index i2c_get_rcc_index(stm32f4_i2c_bus_entry *e)
{
  return i2c_rcc_index [e->index];
}

static uint32_t i2c_get_pclk(stm32f4_i2c_bus_entry *e)
{
  return STM32F4_PCLK1;
}

rtems_status_code stm32f4_i2c_set_bitrate(
  stm32f4_i2c_bus_entry *e,
  uint32_t br
)
{
  volatile stm32f4_i2c *regs = e->regs;
  uint32_t ccr;
  uint32_t trise;
  uint32_t pclk = i2c_get_pclk(e);

  /* Make sure, that the module is disabled */
  if((regs->cr1 & STM32F4_I2C_CR1_PE) != 0)
  {
    return RTEMS_RESOURCE_IN_USE;
  }

  /* Configure clock control register and rise time register */
  ccr = regs->ccr;
  trise = regs->trise;

  if(br <= 100000)
  {
    uint32_t ccr_val = pclk / (2 * br);
    /* according to datasheet, the rise time for standard mode is 1us -> 1MHz */
    uint32_t trise_val = pclk / 1000000 + 1;
    trise = STM32F4_I2C_TRISE_SET(trise, trise_val);

    if(ccr_val > STM32F4_I2C_CCR_CCR_MAX)
    {
      return RTEMS_INVALID_NUMBER;
    }

    /* standard mode */
    ccr &= ~STM32F4_I2C_CCR_FS;
    ccr = STM32F4_I2C_CCR_CCR_SET(ccr, ccr_val);
  }
  else
  {
    /* FIXME: Implement speeds 100kHz < f <= 400kHz (fast mode) */
    return RTEMS_NOT_IMPLEMENTED;
  }

  regs->ccr = ccr;
  regs->trise = trise;

  return RTEMS_SUCCESSFUL;
}

static void stm32f4_i2c_handler(void *arg)
{
  /* This handler implements the suggested read method from stm32f103xx
   * reference manual if the handler is not the one with the highest priority */
  stm32f4_i2c_bus_entry *e = arg;
  volatile stm32f4_i2c *regs = e->regs;
  uint32_t sr1 = regs->sr1;
  uint8_t *data = e->data;
  uint8_t *last = e->last;
  bool read = e->read;
  bool wake_task = false;
  uint32_t cr1;

  if(sr1 & STM32F4_I2C_SR1_SB) {
    /* Start condition sent. */
    regs->dr = e->addr_with_rw;
  }

  if(read) {
    size_t len = e->len;

    if(len == 1) {
      /* special case for one single byte */
      if(sr1 & STM32F4_I2C_SR1_ADDR) {
        cr1 = regs->cr1;
        cr1 &= ~STM32F4_I2C_CR1_ACK;
        regs->cr1 = cr1;

        /* Read sr2 to clear flag */
        regs->sr2;

        cr1 = regs->cr1;
        cr1 |= STM32F4_I2C_CR1_STOP;
        regs->cr1 = cr1;
      } else if(sr1 & STM32F4_I2C_SR1_RxNE) {
        *data = regs->dr;
        wake_task = true;
      }
    } else if (len == 2) {
      /* special case for two bytes */
      if(sr1 & STM32F4_I2C_SR1_ADDR) {
        /* Read sr2 to clear flag */
        regs->sr2;

        cr1 = regs->cr1;
        cr1 &= ~STM32F4_I2C_CR1_ACK;
        regs->cr1 = cr1;
      } else if(sr1 & STM32F4_I2C_SR1_BTF) {
        cr1 = regs->cr1;
        cr1 |= STM32F4_I2C_CR1_STOP;
        regs->cr1 = cr1;

        *data = regs->dr;
        ++data;
        *data = regs->dr;
        wake_task = true;
      }
    } else {
      /* more than two bytes */
      if(sr1 & STM32F4_I2C_SR1_ADDR) {
        /* Read sr2 to clear flag */
        regs->sr2;
      } else if(sr1 & STM32F4_I2C_SR1_BTF && data == last - 2) {
        cr1 = regs->cr1;
        cr1 &= ~STM32F4_I2C_CR1_ACK;
        regs->cr1 = cr1;

        *data = regs->dr;
        ++data;

        cr1 = regs->cr1;
        cr1 |= STM32F4_I2C_CR1_STOP;
        regs->cr1 = cr1;

        *data = regs->dr;
        ++data;
      } else if((sr1 & STM32F4_I2C_SR1_RxNE) && (data != last - 2)) {
        *data = regs->dr;

        if(data == last) {
          wake_task = true;
        } else {
          ++data;
        }
      }
    }
  } else /* write */ {
    if(sr1 & STM32F4_I2C_SR1_ADDR) {
      /* Address sent */
      regs->sr2;
    }

    if((sr1 & (STM32F4_I2C_SR1_ADDR | STM32F4_I2C_SR1_TxE)) && (data <= last)) {
      regs->dr = *data;
      ++data;
    } else if(sr1 & STM32F4_I2C_SR1_BTF) {
      uint32_t cr1 = regs->cr1;
      cr1 |= STM32F4_I2C_CR1_STOP;
      regs->cr1 = cr1;
      wake_task = true;
    }
  }

  e->data = data;

  if(wake_task) {
    bsp_interrupt_vector_disable(e->vector);
    rtems_event_transient_send(e->task_id);
  }
}

static rtems_status_code i2c_wait_done(stm32f4_i2c_bus_entry *e)
{
  bsp_interrupt_vector_enable(e->vector);
  e->task_id = rtems_task_self();
  return rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
}

rtems_status_code stm32f4_i2c_init(stm32f4_i2c_bus_entry *e)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  volatile stm32f4_i2c *regs = e->regs;
  stm32f4_rcc_index rcc_index = i2c_get_rcc_index(e);
  uint32_t pclk = i2c_get_pclk(e);
  uint32_t cr1 = 0;
  uint32_t cr2 = 0;

  assert(pclk >= 2000000);

  /* Create mutex */
  sc = rtems_semaphore_create (
    rtems_build_name ('I', '2', 'C', '1' + e->index),
    0,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY,
    0,
    &e->mutex
  );
  RTEMS_CHECK_SC(sc, "create mutex");

  /* Install interrupt handler and disable this vector */
  sc = rtems_interrupt_handler_install(
    e->vector,
    "I2C",
    RTEMS_INTERRUPT_UNIQUE,
    stm32f4_i2c_handler,
    e
  );
  RTEMS_CHECK_SC(sc, "install interrupt handler");
  bsp_interrupt_vector_disable(e->vector);

  /* Enable module clock */
  stm32f4_rcc_set_clock(rcc_index, true);

  /* Setup initial bit rate */
  sc = stm32f4_i2c_set_bitrate(e, STM32F4_I2C_INITIAL_BITRATE);
  RTEMS_CHECK_SC(sc, "set bitrate");

  /* Set config registers */
  cr2 = regs->cr2;
  cr2 = STM32F4_I2C_CR2_FREQ_SET(cr2, pclk / 1000000);
  cr2 |= STM32F4_I2C_CR2_ITEVTEN;
  cr2 |= STM32F4_I2C_CR2_ITBUFEN;
  regs->cr2 = cr2;

  cr1 = regs->cr1;
  cr1 |= STM32F4_I2C_CR1_PE;
  regs->cr1 = cr1;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code stm32f4_i2c_process_message(
  stm32f4_i2c_bus_entry *e,
  stm32f4_i2c_message *msg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_status_code sc_return = RTEMS_SUCCESSFUL;
  volatile stm32f4_i2c *regs = e->regs;
  uint16_t max_7_bit_address = (1 << 7) - 1;
  uint32_t cr1 = regs->cr1;

  if(msg->addr > max_7_bit_address) {
    return RTEMS_NOT_IMPLEMENTED;
  }

  if(msg->len == 0) {
    return RTEMS_INVALID_SIZE;
  }

  sc = rtems_semaphore_obtain(e->mutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  RTEMS_CHECK_SC(sc, "obtaining mutex");

  e->data = msg->buf;
  e->last = msg->buf + msg->len - 1;
  e->len = msg->len;

  e->addr_with_rw = msg->addr << 1;
  if(msg->read) {
    e->addr_with_rw |= I2C_RW_BIT;
  }
  e->read = msg->read;

  /* Check if no stop is active. */
  if(cr1 & STM32F4_I2C_CR1_STOP) {
    return RTEMS_IO_ERROR;
  }

  /* Start */
  cr1 = regs->cr1;
  if(e->len == 2) {
    cr1 |= STM32F4_I2C_CR1_POS;
  } else {
    cr1 &= ~STM32F4_I2C_CR1_POS;
  }
  cr1 |= STM32F4_I2C_CR1_ACK;
  cr1 |= STM32F4_I2C_CR1_START;
  regs->cr1 = cr1;

  /* Wait for end of message */
  sc = i2c_wait_done(e);

  if(sc != RTEMS_SUCCESSFUL) {
    sc_return = sc;
  }

  sc = rtems_semaphore_release(e->mutex);
  RTEMS_CHECK_SC(sc, "releasing mutex");

  return sc_return;
}

