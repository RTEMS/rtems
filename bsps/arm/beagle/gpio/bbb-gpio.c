/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup arm_beagle
 *
 * @brief Support for the BeagleBone Black.
 */

/**
 * Copyright (c) 2015 Ketul Shah <ketulshah1993 at gmail.com>
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

/* BSP specific function definitions for BeagleBone Black.
 * It is totally beased on Generic GPIO API definition. 
 * For more details related to GPIO API please have a 
 * look at libbbsp/shared/include/gpio.h
 */

/* This driver uses the standard multiplexing modes of the GPIO pins, as defined in
   bsps/arm/include/libcpu/am335x.h. To use all GPIO pins as digital I/O,
   you can set the MUXMODE to '7' by defining BBB_USE_ALL_GPIO_PINS.
   Then MUXMODE 7 get set in rtems_gpio_bsp_set_resistor_mode(). 
*/

#define BBB_USE_ALL_GPIO_PINS 1 

#include <bsp/beagleboneblack.h>
#include <bsp/irq-generic.h>
#include <bsp/gpio.h>
#include <bsp/bbb-gpio.h>
#include <libcpu/am335x.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/* Currently these definitions are for BeagleBone Black board only
 * with modifications from ketul93
 * (https://gist.github.com/ketul93/d717555951174a74c8b4)
 * Later on Beagle-xM board support can be added in this code.
 * After support gets added if condition should be removed
 * HPJ: set almost all I/O to Mode7 to use all for own application
 * This is not yet well structured and good ideas are still needed
 */
#if IS_AM335X

static const uint32_t gpio_bank_addrs[] =
  { AM335X_GPIO0_BASE,
	AM335X_GPIO1_BASE,
	AM335X_GPIO2_BASE,
	AM335X_GPIO3_BASE };

static const rtems_vector_number gpio_bank_vector[] =
  { AM335X_INT_GPIOINT0A,
  	AM335X_INT_GPIOINT1A,
  	AM335X_INT_GPIOINT2A,
  	AM335X_INT_GPIOINT3A };

/* Macro for the gpio pin not having control module offset mapping */
#define CONF_NOT_DEFINED 0x00

/* Mapping of gpio pin number to the Control module mapped register offset */
static const uint32_t gpio_pad_conf [GPIO_BANK_COUNT][BSP_GPIO_PINS_PER_BANK] =
{
  /* GPIO Module 0 */
  { CONF_NOT_DEFINED,             /* GPIO0[0] */
    CONF_NOT_DEFINED,             /* GPIO0[1] */
    AM335X_CONF_SPI0_SCLK,        /* GPIO0[2] */
    AM335X_CONF_SPI0_D0,          /* GPIO0[3] */
    AM335X_CONF_SPI0_D1,          /* GPIO0[4] */
    AM335X_CONF_SPI0_CS0,         /* GPIO0[5] */
    CONF_NOT_DEFINED,             /* GPIO0[6] */
    AM335X_CONF_ECAP0_IN_PWM0_OUT,/* GPIO0[7] */
    AM335X_CONF_LCD_DATA12,       /* GPIO0[8] */
    AM335X_CONF_LCD_DATA13,       /* GPIO0[9] */
    AM335X_CONF_LCD_DATA14,       /* GPIO0[10] */
    AM335X_CONF_LCD_DATA15,       /* GPIO0[11] */
    AM335X_CONF_UART1_CTSN,       /* GPIO0[12] */
    AM335X_CONF_UART1_RTSN,       /* GPIO0[13] */
    AM335X_CONF_UART1_RXD,        /* GPIO0[14] */
    AM335X_CONF_UART1_TXD,        /* GPIO0[15] */
    CONF_NOT_DEFINED,             /* GPIO0[16] */
    CONF_NOT_DEFINED,             /* GPIO0[17] */
    CONF_NOT_DEFINED,             /* GPIO0[18] */
    CONF_NOT_DEFINED,             /* GPIO0[19] */
    AM335X_CONF_XDMA_EVENT_INTR1, /* GPIO0[20] */
    CONF_NOT_DEFINED,             /* GPIO0[21] */
    AM335X_CONF_GPMC_AD8,         /* GPIO0[22] */
    AM335X_CONF_GPMC_AD9,         /* GPIO0[23] */
    CONF_NOT_DEFINED,             /* GPIO0[24] */
    CONF_NOT_DEFINED,             /* GPIO0[25] */
    AM335X_CONF_GPMC_AD10,        /* GPIO0[26] */
    AM335X_CONF_GPMC_AD11,        /* GPIO0[27] */
    CONF_NOT_DEFINED,             /* GPIO0[28] */
    CONF_NOT_DEFINED,             /* GPIO0[29] */
    AM335X_CONF_GPMC_WAIT0,       /* GPIO0[30] */
    AM335X_CONF_GPMC_WPN          /* GPIO0[31] */ },

  /* GPIO Module 1 */
  { AM335X_CONF_GPMC_AD0,         /* GPIO1[0] */
    AM335X_CONF_GPMC_AD1,         /* GPIO1[1] */
    AM335X_CONF_GPMC_AD2,         /* GPIO1[2] */
    AM335X_CONF_GPMC_AD3,         /* GPIO1[3] */
    AM335X_CONF_GPMC_AD4,         /* GPIO1[4] */
    AM335X_CONF_GPMC_AD5,         /* GPIO1[5] */
    AM335X_CONF_GPMC_AD6,         /* GPIO1[6] */
    AM335X_CONF_GPMC_AD7,         /* GPIO1[7] */
    CONF_NOT_DEFINED,             /* GPIO1[8] */
    CONF_NOT_DEFINED,             /* GPIO1[9] */
    CONF_NOT_DEFINED,             /* GPIO1[10] */
    CONF_NOT_DEFINED,             /* GPIO1[11] */
    AM335X_CONF_GPMC_AD12,        /* GPIO1[12] */
    AM335X_CONF_GPMC_AD13,        /* GPIO1[13] */
    AM335X_CONF_GPMC_AD14,        /* GPIO1[14] */
    AM335X_CONF_GPMC_AD15,        /* GPIO1[15] */
    AM335X_CONF_GPMC_A0,          /* GPIO1[16] */
    AM335X_CONF_GPMC_A1,          /* GPIO1[17] */
    AM335X_CONF_GPMC_A2,          /* GPIO1[18] */
    AM335X_CONF_GPMC_A3,          /* GPIO1[19] */
    CONF_NOT_DEFINED,             /* GPIO1[20] */
    AM335X_CONF_GPMC_A5,          /* GPIO1[21] */
    AM335X_CONF_GPMC_A6,          /* GPIO1[22] */
    AM335X_CONF_GPMC_A7,          /* GPIO1[23] */
    AM335X_CONF_GPMC_A8,          /* GPIO1[24] */
    CONF_NOT_DEFINED,             /* GPIO1[25] */
    CONF_NOT_DEFINED,             /* GPIO1[26] */
    CONF_NOT_DEFINED,             /* GPIO1[27] */
    AM335X_CONF_GPMC_BEN1,        /* GPIO1[28] */
    AM335X_CONF_GPMC_CSN0,        /* GPIO1[29] */
    AM335X_CONF_GPMC_CSN1,        /* GPIO1[30] */
    AM335X_CONF_GPMC_CSN2         /* GPIO1[31] */ },

  /* GPIO Module 2 */
  { CONF_NOT_DEFINED,             /* GPIO2[0] */
    AM335X_CONF_GPMC_CLK,         /* GPIO2[1] */
    AM335X_CONF_GPMC_ADVN_ALE,    /* GPIO2[2] */
    AM335X_CONF_GPMC_OEN_REN,     /* GPIO2[3] */
    AM335X_CONF_GPMC_WEN,         /* GPIO2[4] */
    AM335X_CONF_GPMC_BEN0_CLE,    /* GPIO2[5] */
    AM335X_CONF_LCD_DATA0,        /* GPIO2[6] */
    AM335X_CONF_LCD_DATA1,        /* GPIO2[7] */
    AM335X_CONF_LCD_DATA2,        /* GPIO2[8] */
    AM335X_CONF_LCD_DATA3,        /* GPIO2[9] */
    AM335X_CONF_LCD_DATA4,        /* GPIO2[10] */
    AM335X_CONF_LCD_DATA5,        /* GPIO2[11] */
    AM335X_CONF_LCD_DATA6,        /* GPIO2[12] */
    AM335X_CONF_LCD_DATA7,        /* GPIO2[13] */
    AM335X_CONF_LCD_DATA8,        /* GPIO2[14] */
    AM335X_CONF_LCD_DATA9,        /* GPIO2[15] */
    AM335X_CONF_LCD_DATA10,       /* GPIO2[16] */
    AM335X_CONF_LCD_DATA11,       /* GPIO2[17] */
    CONF_NOT_DEFINED,             /* GPIO2[18] */
    CONF_NOT_DEFINED,             /* GPIO2[19] */
    CONF_NOT_DEFINED,             /* GPIO2[20] */
    CONF_NOT_DEFINED,             /* GPIO2[21] */
    AM335X_CONF_LCD_VSYNC,        /* GPIO2[22] */
    AM335X_CONF_LCD_HSYNC,        /* GPIO2[23] */
    AM335X_CONF_LCD_PCLK,         /* GPIO2[24] */
    AM335X_CONF_LCD_AC_BIAS_EN    /* GPIO2[25] */ },

  /* GPIO Module 3 */
  { CONF_NOT_DEFINED,             /* GPIO3[0] */
    CONF_NOT_DEFINED,             /* GPIO3[1] */
    CONF_NOT_DEFINED,             /* GPIO3[2] */
    CONF_NOT_DEFINED,             /* GPIO3[3] */
    CONF_NOT_DEFINED,             /* GPIO3[4] */
    CONF_NOT_DEFINED,             /* GPIO3[5] */
    CONF_NOT_DEFINED,             /* GPIO3[6] */
    CONF_NOT_DEFINED,             /* GPIO3[7] */
    CONF_NOT_DEFINED,             /* GPIO3[8] */
    CONF_NOT_DEFINED,             /* GPIO3[9] */
    CONF_NOT_DEFINED,             /* GPIO3[10] */
    CONF_NOT_DEFINED,             /* GPIO3[11] */
    CONF_NOT_DEFINED,             /* GPIO3[12] */
    CONF_NOT_DEFINED,             /* GPIO3[13] */
    AM335X_CONF_MCASP0_ACLKX,     /* GPIO3[14] */
    AM335X_CONF_MCASP0_FSX,       /* GPIO3[15] */
    AM335X_CONF_MCASP0_AXR0,      /* GPIO3[16] */
    AM335X_CONF_MCASP0_AHCLKR,    /* GPIO3[17] */
    CONF_NOT_DEFINED,             /* GPIO3[18] */
    AM335X_CONF_MCASP0_FSR,       /* GPIO3[19] */
    AM335X_CONF_MCASP0_AXR1,      /* GPIO3[20] */
    AM335X_CONF_MCASP0_AHCLKX     /* GPIO3[21] */ }
};

/* Get the address of Base Register + Offset for pad config */
static inline uint32_t bbb_conf_reg(uint32_t bank, uint32_t pin)
{
  /* Asserts if invalid pin is supplied */
  assert(gpio_pad_conf[bank][pin] != CONF_NOT_DEFINED);
  return (AM335X_PADCONF_BASE + gpio_pad_conf[bank][pin]);
}

/* Get the value of Base Register + Offset */
static inline uint32_t bbb_reg(uint32_t bank, uint32_t reg)
{
  return REG(gpio_bank_addrs[bank] + reg);
}

/* Waits a number of CPU cycles. */
static void arm_delay(uint8_t cycles)
{
  uint8_t i;

  for ( i = 0; i < cycles; ++i ) {
    asm volatile("nop");
  }
}

static rtems_status_code bbb_select_pin_function(
  uint32_t bank,
  uint32_t pin,
  uint32_t type
) {

  if ( type == BBB_DIGITAL_IN ) {
    mmio_set((gpio_bank_addrs[bank] + AM335X_GPIO_OE), BIT(pin));
  }
  else {
    mmio_clear((gpio_bank_addrs[bank] + AM335X_GPIO_OE), BIT(pin));
  }
  return RTEMS_SUCCESSFUL;

}

rtems_status_code rtems_gpio_bsp_multi_set(uint32_t bank, uint32_t bitmask)
{
  mmio_write(bbb_reg(bank, AM335X_GPIO_SETDATAOUT), bitmask);

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_gpio_bsp_multi_clear(uint32_t bank, uint32_t bitmask)
{
  mmio_write(bbb_reg(bank, AM335X_GPIO_CLEARDATAOUT), bitmask);

  return RTEMS_SUCCESSFUL;
}

uint32_t rtems_gpio_bsp_multi_read(uint32_t bank, uint32_t bitmask)
{
  return (mmio_read(gpio_bank_addrs[bank] + AM335X_GPIO_DATAIN) & bitmask);
}

rtems_status_code rtems_gpio_bsp_set(uint32_t bank, uint32_t pin)
{
  mmio_write(bbb_reg(bank, AM335X_GPIO_SETDATAOUT), BIT(pin));

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_gpio_bsp_clear(uint32_t bank, uint32_t pin)
{
  mmio_write(bbb_reg(bank, AM335X_GPIO_CLEARDATAOUT), BIT(pin));

  return RTEMS_SUCCESSFUL;
}

uint32_t rtems_gpio_bsp_get_value(uint32_t bank, uint32_t pin)
{
  return (mmio_read(gpio_bank_addrs[bank] + AM335X_GPIO_DATAIN) & BIT(pin) ? 1:0);
}

rtems_status_code rtems_gpio_bsp_select_input(
  uint32_t bank,
  uint32_t pin,
  void *bsp_specific
) {
  (void) bsp_specific;

  return bbb_select_pin_function(bank, pin, BBB_DIGITAL_IN);
}

rtems_status_code rtems_gpio_bsp_select_output(
  uint32_t bank,
  uint32_t pin,
  void *bsp_specific
) {
  (void) bsp_specific;

  return bbb_select_pin_function(bank, pin, BBB_DIGITAL_OUT);
}

rtems_status_code rtems_gpio_bsp_select_specific_io(
  uint32_t bank,
  uint32_t pin,
  uint32_t function,
  void *pin_data
) {
  (void) bank;
  (void) pin;
  (void) function;
  (void) pin_data;

  return RTEMS_NOT_DEFINED;
}

rtems_status_code rtems_bsp_select_specific_io(
  uint32_t bank,
  uint32_t pin,
  uint32_t function,
  void *pin_data
) {
  (void) bank;
  (void) pin;
  (void) function;
  (void) pin_data;

  return RTEMS_NOT_DEFINED;
}

rtems_status_code rtems_gpio_bsp_set_resistor_mode(
  uint32_t bank,
  uint32_t pin,
  rtems_gpio_pull_mode mode
) {
  uint32_t register_content;

/* Here we set the Pin to  MUXMMODE 7 (GPIO), here we want to use almost any Pin for GPIO
   digital I/O. 
   This is not a very elegant solution. It might be better if it were more configurable.
   Via `pad_config`, but I couldn't think of anything better here. So this is then defined via the
   application software by calling `rtems_gpio_bsp_set_resistor_mode()`
*/

#ifdef BBB_USE_ALL_GPIO_PINS
  register_content = BBB_MUXMODE(7);
#endif

  /* Set control signal. */
  switch ( mode ) {
    case PULL_UP:
      register_content = register_content | BBB_PU_EN | BBB_RXACTIVE; // assume input
      break;
    case PULL_DOWN:
      break;
    case NO_PULL_RESISTOR:
      register_content = register_content | BBB_PUDDIS;
      break;
    default:
      return RTEMS_UNSATISFIED;
  }
 
  mmio_write(bbb_conf_reg(bank,pin), register_content);

  return RTEMS_SUCCESSFUL;
}

rtems_vector_number rtems_gpio_bsp_get_vector(uint32_t bank)
{
  return gpio_bank_vector[bank];
}

uint32_t rtems_gpio_bsp_interrupt_line(rtems_vector_number vector)
{
  uint32_t event_status;
  uint8_t bank_nr = 0;

  /* Following loop will get the bank number from vector number */
  while (bank_nr < GPIO_BANK_COUNT && vector != gpio_bank_vector[bank_nr])
  {
  	bank_nr++;
  }

  /* Retrieve the interrupt event status. */
  event_status = bbb_reg(bank_nr, AM335X_GPIO_IRQSTATUS_0);

  /* Clear the interrupt line. */
  mmio_write(gpio_bank_addrs[bank_nr] + AM335X_GPIO_IRQSTATUS_0, event_status);

  return event_status;
}

rtems_status_code rtems_gpio_bsp_enable_interrupt(
  uint32_t bank,
  uint32_t pin,
  rtems_gpio_interrupt interrupt
) {
  return rtems_bsp_enable_interrupt(bank, pin, interrupt);
}

rtems_status_code rtems_bsp_enable_interrupt(
  uint32_t bank,
  uint32_t pin,
  rtems_gpio_interrupt interrupt
) {
  switch ( interrupt ) {
    case FALLING_EDGE:
      /* Enables asynchronous falling edge detection. */
      mmio_set(gpio_bank_addrs[bank] + AM335X_GPIO_FALLINGDETECT, BIT(pin));
      break;
    case RISING_EDGE:
      /* Enables asynchronous rising edge detection. */
      mmio_set(gpio_bank_addrs[bank] + AM335X_GPIO_RISINGDETECT, BIT(pin));
      break;
    case BOTH_EDGES:
      /* Enables asynchronous falling edge detection. */
      mmio_set(gpio_bank_addrs[bank] + AM335X_GPIO_FALLINGDETECT, BIT(pin));
      /* Enables asynchronous rising edge detection. */
      mmio_set(gpio_bank_addrs[bank] + AM335X_GPIO_RISINGDETECT, BIT(pin));
      break;
    case LOW_LEVEL:
      /* Enables pin low level detection. */
      mmio_set(gpio_bank_addrs[bank] + AM335X_GPIO_LEVELDETECT0, BIT(pin));
      break;
    case HIGH_LEVEL:
      /* Enables pin high level detection. */
      mmio_set(gpio_bank_addrs[bank] + AM335X_GPIO_LEVELDETECT1, BIT(pin));
      break;
    case BOTH_LEVELS:
      /* Enables pin low level detection. */
      mmio_set(gpio_bank_addrs[bank] + AM335X_GPIO_LEVELDETECT0, BIT(pin));

      /* Enables pin high level detection. */
      mmio_set(gpio_bank_addrs[bank] + AM335X_GPIO_LEVELDETECT1, BIT(pin));
      break;
    case NONE:
    default:
      return RTEMS_UNSATISFIED;
  }
/*
  printf(" Register for interrupt detection are : \n");
  printf("\t FALLING : 0x%x\n", mmio_read(gpio_bank_addrs[bank] + AM335X_GPIO_FALLINGDETECT)); 
  printf("\t RISING : 0x%x\n", mmio_read(gpio_bank_addrs[bank] + AM335X_GPIO_RISINGDETECT)); 
  printf("\t Low Level : 0x%x\n", mmio_read(gpio_bank_addrs[bank] + AM335X_GPIO_LEVELDETECT0)); 
  printf("\t High Level : 0x%x\n", mmio_read(gpio_bank_addrs[bank] + AM335X_GPIO_LEVELDETECT1)); 
*/

  mmio_write(gpio_bank_addrs[bank] + AM335X_GPIO_IRQSTATUS_SET_0, BIT(pin));
  mmio_write(gpio_bank_addrs[bank] + AM335X_GPIO_IRQSTATUS_SET_1, BIT(pin));

  /* The detection starts after 5 clock cycles as per AM335X TRM
   * This period is required to clean the synchronization edge/
   * level detection pipeline
   */
  arm_delay(5);
  return RTEMS_SUCCESSFUL;
}


rtems_status_code rtems_gpio_bsp_disable_interrupt(
  uint32_t bank,
  uint32_t pin,
  rtems_gpio_interrupt interrupt
) {
  return rtems_bsp_disable_interrupt(bank, pin, interrupt);
}

rtems_status_code rtems_bsp_disable_interrupt(
  uint32_t bank,
  uint32_t pin,
  rtems_gpio_interrupt interrupt
) {
  switch ( interrupt ) {
    case FALLING_EDGE:
      /* Disables asynchronous falling edge detection. */
      mmio_clear(gpio_bank_addrs[bank] + AM335X_GPIO_FALLINGDETECT, BIT(pin));
      break;
    case RISING_EDGE:
      /* Disables asynchronous rising edge detection. */
      mmio_clear(gpio_bank_addrs[bank] + AM335X_GPIO_RISINGDETECT, BIT(pin));
      break;
    case BOTH_EDGES:
      /* Disables asynchronous falling edge detection. */
      mmio_clear(gpio_bank_addrs[bank] + AM335X_GPIO_FALLINGDETECT, BIT(pin));

      /* Disables asynchronous rising edge detection. */
      mmio_clear(gpio_bank_addrs[bank] + AM335X_GPIO_RISINGDETECT, BIT(pin));
      break;
    case LOW_LEVEL:
      /* Disables pin low level detection. */
      mmio_clear(gpio_bank_addrs[bank] + AM335X_GPIO_LEVELDETECT0, BIT(pin));
      break;
    case HIGH_LEVEL:
      /* Disables pin high level detection. */
      mmio_clear(gpio_bank_addrs[bank] + AM335X_GPIO_LEVELDETECT1, BIT(pin));
      break;
    case BOTH_LEVELS:
      /* Disables pin low level detection. */
      mmio_clear(gpio_bank_addrs[bank] + AM335X_GPIO_LEVELDETECT0, BIT(pin));

      /* Disables pin high level detection. */
      mmio_clear(gpio_bank_addrs[bank] + AM335X_GPIO_LEVELDETECT1, BIT(pin));
      break;
    case NONE:
    default:
      return RTEMS_UNSATISFIED;
  }

  mmio_write((gpio_bank_addrs[bank] + AM335X_GPIO_IRQSTATUS_CLR_0), BIT(pin));
  mmio_write((gpio_bank_addrs[bank] + AM335X_GPIO_IRQSTATUS_CLR_1), BIT(pin));

  return RTEMS_SUCCESSFUL;
}

/* I have unfortunately lost the overview a little ....
*/
rtems_status_code rtems_gpio_bsp_multi_select(
  rtems_gpio_multiple_pin_select *pins,
  uint32_t pin_count,
  uint32_t select_bank
) {
  (void) select_bank;

  uint8_t i;

  for ( i = 0; i < pin_count; ++i ) {
      bbb_select_pin_function((pins[i].pin_number)/32, (pins[i].pin_number)%32, pins[i].function);
  }
  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_gpio_bsp_specific_group_operation(
  uint32_t bank,
  uint32_t *pins,
  uint32_t pin_count,
  void *arg
) {
  (void) bank;
  (void) pins;
  (void) pin_count;
  (void) arg;


  return RTEMS_NOT_DEFINED;
}
#endif /* IS_AM335X */

/* For support of BeagleboardxM */
#if IS_DM3730

/* Currently this section is just to satisfy
 * GPIO API and to make the build successful.
 * Later on support can be added here.
 */

rtems_status_code rtems_gpio_bsp_multi_set(uint32_t bank, uint32_t bitmask)
{
  (void) bank;
  (void) bitmask;

  return RTEMS_NOT_DEFINED;
}

rtems_status_code rtems_gpio_bsp_multi_clear(uint32_t bank, uint32_t bitmask)
{
  (void) bank;
  (void) bitmask;

  return RTEMS_NOT_DEFINED;
}

uint32_t rtems_gpio_bsp_multi_read(uint32_t bank, uint32_t bitmask)
{
  (void) bank;
  (void) bitmask;

  return -1;
}

rtems_status_code rtems_gpio_bsp_set(uint32_t bank, uint32_t pin)
{
  (void) bank;
  (void) pin;

  return RTEMS_NOT_DEFINED;
}

rtems_status_code rtems_gpio_bsp_clear(uint32_t bank, uint32_t pin)
{
  (void) bank;
  (void) pin;

  return RTEMS_NOT_DEFINED;
}

uint32_t rtems_gpio_bsp_get_value(uint32_t bank, uint32_t pin)
{
  (void) bank;
  (void) pin;

  return -1;
}

rtems_status_code rtems_gpio_bsp_select_input(
  uint32_t bank,
  uint32_t pin,
  void *bsp_specific
) {
  (void) bank;
  (void) pin;
  (void) bsp_specific;

  return RTEMS_NOT_DEFINED;
}

rtems_status_code rtems_gpio_bsp_select_output(
  uint32_t bank,
  uint32_t pin,
  void *bsp_specific
) {
  (void) bank;
  (void) pin;
  (void) bsp_specific;

  return RTEMS_NOT_DEFINED;
}

rtems_status_code rtems_gpio_bsp_select_specific_io(
  uint32_t bank,
  uint32_t pin,
  uint32_t function,
  void *pin_data
) {
  (void) bank;
  (void) pin;
  (void) function;
  (void) pin_data;

  return RTEMS_NOT_DEFINED;
}

rtems_status_code rtems_gpio_bsp_set_resistor_mode(
  uint32_t bank,
  uint32_t pin,
  rtems_gpio_pull_mode mode
) {
  (void) bank;
  (void) pin;
  (void) mode;

  return RTEMS_NOT_DEFINED;
}

rtems_vector_number rtems_gpio_bsp_get_vector(uint32_t bank)
{
  (void) bank;

  return -1;
}

uint32_t rtems_gpio_bsp_interrupt_line(rtems_vector_number vector)
{
  (void) vector;

  return -1;
}

rtems_status_code rtems_gpio_bsp_enable_interrupt(
  uint32_t bank,
  uint32_t pin,
  rtems_gpio_interrupt interrupt
) {
  (void) bank;
  (void) pin;
  (void) interrupt;

  return RTEMS_NOT_DEFINED;
}

rtems_status_code rtems_gpio_bsp_disable_interrupt(
  uint32_t bank,
  uint32_t pin,
  rtems_gpio_interrupt interrupt
) {
  (void) bank;
  (void) pin;
  (void) interrupt;

  return RTEMS_NOT_DEFINED;
}

rtems_status_code rtems_gpio_bsp_multi_select(
  rtems_gpio_multiple_pin_select *pins,
  uint32_t pin_count,
  uint32_t select_bank
) {
  (void) pins;
  (void) pin_count;
  (void) select_bank;

  return RTEMS_NOT_DEFINED;
}

rtems_status_code rtems_gpio_bsp_specific_group_operation(
  uint32_t bank,
  uint32_t *pins,
  uint32_t pin_count,
  void *arg
) {
  (void) bank;
  (void) pins;
  (void) pin_count;;
  (void) arg;

  return RTEMS_NOT_DEFINED;
}

#endif /* IS_DM3730 */
