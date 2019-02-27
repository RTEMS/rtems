/**
 * @file
 *
 * @ingroup raspberrypi_gpio
 *
 * @brief Support for the Raspberry PI GPIO.
 */

/*
 *  Copyright (c) 2014-2015 Andre Marques <andre.lousa.marques at gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/raspberrypi.h>
#include <bsp/irq-generic.h>
#include <bsp/gpio.h>
#include <bsp/rpi-gpio.h>

#include <stdlib.h>

RTEMS_INTERRUPT_LOCK_DEFINE( static, rtems_gpio_bsp_lock, "rtems_gpio_bsp_lock" );

/* Calculates a bitmask to assign an alternate function to a given pin. */
#define SELECT_PIN_FUNCTION(fn, pn) (fn << ((pn % 10) * 3))

rtems_gpio_specific_data alt_func_def[] = {
  {.io_function = RPI_ALT_FUNC_0, .pin_data = NULL},
  {.io_function = RPI_ALT_FUNC_1, .pin_data = NULL},
  {.io_function = RPI_ALT_FUNC_2, .pin_data = NULL},
  {.io_function = RPI_ALT_FUNC_3, .pin_data = NULL},
  {.io_function = RPI_ALT_FUNC_4, .pin_data = NULL},
  {.io_function = RPI_ALT_FUNC_5, .pin_data = NULL}
};

/* Raspberry Pi 1 Revision 2 gpio interface definitions. */
#include "gpio-interfaces-pi1-rev2.c"

/* Waits a number of CPU cycles. */
static void arm_delay(uint8_t cycles)
{
  uint8_t i;

  for ( i = 0; i < cycles; ++i ) {
    asm volatile("nop");
  }
}

static rtems_status_code rpi_select_pin_function(
  uint32_t bank,
  uint32_t pin,
  uint32_t type
) {
  /* Calculate the pin function select register address. */
  volatile uint32_t *pin_addr = (uint32_t *) BCM2835_GPIO_REGS_BASE +
                                             (pin / 10);
  uint32_t reg_old;
  uint32_t reg_new;
  rtems_interrupt_lock_context lock_context;

  rtems_interrupt_lock_acquire(&rtems_gpio_bsp_lock, &lock_context);
  reg_new = reg_old = *pin_addr;
  reg_new &= ~SELECT_PIN_FUNCTION(RPI_ALT_FUNC_MASK, pin);
  reg_new |= SELECT_PIN_FUNCTION(type, pin);
  *pin_addr = reg_new;
  rtems_interrupt_lock_release(&rtems_gpio_bsp_lock, &lock_context);

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_gpio_bsp_multi_set(uint32_t bank, uint32_t bitmask)
{
  BCM2835_REG(BCM2835_GPIO_GPSET0) = bitmask;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_gpio_bsp_multi_clear(uint32_t bank, uint32_t bitmask)
{
  BCM2835_REG(BCM2835_GPIO_GPCLR0) = bitmask;

  return RTEMS_SUCCESSFUL;
}

uint32_t rtems_gpio_bsp_multi_read(uint32_t bank, uint32_t bitmask)
{
  return (BCM2835_REG(BCM2835_GPIO_GPLEV0) & bitmask);
}

rtems_status_code rtems_gpio_bsp_set(uint32_t bank, uint32_t pin)
{
  BCM2835_REG(BCM2835_GPIO_GPSET0) = (1 << pin);

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_gpio_bsp_clear(uint32_t bank, uint32_t pin)
{
  BCM2835_REG(BCM2835_GPIO_GPCLR0) = (1 << pin);

  return RTEMS_SUCCESSFUL;
}

uint32_t rtems_gpio_bsp_get_value(uint32_t bank, uint32_t pin)
{
  return (BCM2835_REG(BCM2835_GPIO_GPLEV0) & (1 << pin));
}

rtems_status_code rtems_gpio_bsp_select_input(
  uint32_t bank,
  uint32_t pin,
  void *bsp_specific
) {
  return rpi_select_pin_function(bank, pin, RPI_DIGITAL_IN);
}

rtems_status_code rtems_gpio_bsp_select_output(
  uint32_t bank,
  uint32_t pin,
  void *bsp_specific
) {
  return rpi_select_pin_function(bank, pin, RPI_DIGITAL_OUT);
}

rtems_status_code rtems_gpio_bsp_select_specific_io(
  uint32_t bank,
  uint32_t pin,
  uint32_t function,
  void *pin_data
) {
  return rpi_select_pin_function(bank, pin, function);
}

rtems_status_code rtems_gpio_bsp_set_resistor_mode(
  uint32_t bank,
  uint32_t pin,
  rtems_gpio_pull_mode mode
) {
  /* Set control signal. */
  switch ( mode ) {
    case PULL_UP:
      BCM2835_REG(BCM2835_GPIO_GPPUD) = (1 << 1);
      break;
    case PULL_DOWN:
      BCM2835_REG(BCM2835_GPIO_GPPUD) = (1 << 0);
      break;
    case NO_PULL_RESISTOR:
      BCM2835_REG(BCM2835_GPIO_GPPUD) = 0;
      break;
    default:
      return RTEMS_UNSATISFIED;
  }

  /* Wait 150 cyles, as per BCM2835 documentation. */
  arm_delay(150);

  /* Setup clock for the control signal. */
  BCM2835_REG(BCM2835_GPIO_GPPUDCLK0) = (1 << pin);

  arm_delay(150);

  /* Remove the control signal. */
  BCM2835_REG(BCM2835_GPIO_GPPUD) = 0;

  /* Remove the clock. */
  BCM2835_REG(BCM2835_GPIO_GPPUDCLK0) = 0;

  return RTEMS_SUCCESSFUL;
}

rtems_vector_number rtems_gpio_bsp_get_vector(uint32_t bank)
{
  return BCM2835_IRQ_ID_GPIO_0;
}

uint32_t rtems_gpio_bsp_interrupt_line(rtems_vector_number vector)
{
  uint32_t event_status;

  /* Retrieve the interrupt event status. */
  event_status = BCM2835_REG(BCM2835_GPIO_GPEDS0);

  /* Clear the interrupt line. */
  BCM2835_REG(BCM2835_GPIO_GPEDS0) = event_status;

  return event_status;
}

rtems_status_code rtems_gpio_bsp_enable_interrupt(
  uint32_t bank,
  uint32_t pin,
  rtems_gpio_interrupt interrupt
) {
  switch ( interrupt ) {
    case FALLING_EDGE:
      /* Enables asynchronous falling edge detection. */
      BCM2835_REG(BCM2835_GPIO_GPAFEN0) |= (1 << pin);
      break;
    case RISING_EDGE:
      /* Enables asynchronous rising edge detection. */
      BCM2835_REG(BCM2835_GPIO_GPAREN0) |= (1 << pin);
      break;
    case BOTH_EDGES:
      /* Enables asynchronous falling edge detection. */
      BCM2835_REG(BCM2835_GPIO_GPAFEN0) |= (1 << pin);

      /* Enables asynchronous rising edge detection. */
      BCM2835_REG(BCM2835_GPIO_GPAREN0) |= (1 << pin);
      break;
    case LOW_LEVEL:
      /* Enables pin low level detection. */
      BCM2835_REG(BCM2835_GPIO_GPLEN0) |= (1 << pin);
      break;
    case HIGH_LEVEL:
      /* Enables pin high level detection. */
      BCM2835_REG(BCM2835_GPIO_GPHEN0) |= (1 << pin);
      break;
    case BOTH_LEVELS:
      /* Enables pin low level detection. */
      BCM2835_REG(BCM2835_GPIO_GPLEN0) |= (1 << pin);

      /* Enables pin high level detection. */
      BCM2835_REG(BCM2835_GPIO_GPHEN0) |= (1 << pin);
      break;
    case NONE:
    default:
      return RTEMS_UNSATISFIED;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_gpio_bsp_disable_interrupt(
  uint32_t bank,
  uint32_t pin,
  rtems_gpio_interrupt interrupt
) {
  switch ( interrupt ) {
    case FALLING_EDGE:
      /* Disables asynchronous falling edge detection. */
      BCM2835_REG(BCM2835_GPIO_GPAFEN0) &= ~(1 << pin);
      break;
    case RISING_EDGE:
      /* Disables asynchronous rising edge detection. */
      BCM2835_REG(BCM2835_GPIO_GPAREN0) &= ~(1 << pin);
      break;
    case BOTH_EDGES:
      /* Disables asynchronous falling edge detection. */
      BCM2835_REG(BCM2835_GPIO_GPAFEN0) &= ~(1 << pin);

      /* Disables asynchronous rising edge detection. */
      BCM2835_REG(BCM2835_GPIO_GPAREN0) &= ~(1 << pin);
      break;
    case LOW_LEVEL:
      /* Disables pin low level detection. */
      BCM2835_REG(BCM2835_GPIO_GPLEN0) &= ~(1 << pin);
      break;
    case HIGH_LEVEL:
      /* Disables pin high level detection. */
      BCM2835_REG(BCM2835_GPIO_GPHEN0) &= ~(1 << pin);
      break;
    case BOTH_LEVELS:
      /* Disables pin low level detection. */
      BCM2835_REG(BCM2835_GPIO_GPLEN0) &= ~(1 << pin);

      /* Disables pin high level detection. */
      BCM2835_REG(BCM2835_GPIO_GPHEN0) &= ~(1 << pin);
      break;
    case NONE:
    default:
      return RTEMS_UNSATISFIED;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rpi_gpio_select_jtag(void)
{
  return rtems_gpio_multi_select(jtag_config, JTAG_PIN_COUNT);
}

rtems_status_code rpi_gpio_select_spi(void)
{
  return rtems_gpio_multi_select(spi_config, SPI_PIN_COUNT);
}

rtems_status_code rpi_gpio_select_i2c(void)
{
  return rtems_gpio_multi_select(i2c_config, I2C_PIN_COUNT);
}

rtems_status_code rtems_gpio_bsp_multi_select(
  rtems_gpio_multiple_pin_select *pins,
  uint32_t pin_count,
  uint32_t select_bank
) {
  uint32_t register_address;
  uint32_t select_register;
  uint8_t i;

  register_address = BCM2835_GPIO_REGS_BASE + (select_bank * 0x04);

  select_register = BCM2835_REG(register_address);

  for ( i = 0; i < pin_count; ++i ) {
    if ( pins[i].function == DIGITAL_INPUT ) {
      select_register &=
        ~SELECT_PIN_FUNCTION(RPI_DIGITAL_IN, pins[i].pin_number);
    }
    else if ( pins[i].function == DIGITAL_OUTPUT ) {
      select_register |=
        SELECT_PIN_FUNCTION(RPI_DIGITAL_OUT, pins[i].pin_number);
    }
    else { /* BSP_SPECIFIC function. */
      select_register |=
        SELECT_PIN_FUNCTION(pins[i].io_function, pins[i].pin_number);
    }
  }

  BCM2835_REG(register_address) = select_register;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_gpio_bsp_specific_group_operation(
  uint32_t bank,
  uint32_t *pins,
  uint32_t pin_count,
  void *arg
) {
  return RTEMS_NOT_DEFINED;
}
