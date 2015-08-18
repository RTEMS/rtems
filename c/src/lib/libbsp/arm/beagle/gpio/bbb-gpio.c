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
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

/* BSP specific function definitions for BeagleBone Black.
 * It is totally beased on Generic GPIO API definition. 
 * For more details related to GPIO API please have a 
 * look at libbbsp/shared/include/gpio.h
 */

#include <bsp/beagleboneblack.h>
#include <bsp/irq-generic.h>
#include <bsp/gpio.h>
#include <bsp/bbb-gpio.h>
#include <libcpu/am335x.h>

#include <assert.h>
#include <stdlib.h>

/* Currently these definitions are for BeagleBone Black board only
 * Later on Beagle-xM board support can be added in this code.
 * After support gets added if condition should be removed
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
#define CONF_NOT_DEFINED 0x00000000

/* Mapping of gpio pin nuber to the  Control module mapped register offset */
static const uint32_t gpio_pad_conf[GPIO_BANK_COUNT][BSP_GPIO_PINS_PER_BANK] =
{
  /* GPIO Module 0 */
  { CONF_NOT_DEFINED,             /* GPIO0[0] */
    CONF_NOT_DEFINED,             /* GPIO0[1] */
    AM335X_CONF_SPI0_SCLK,        /* GPIO0[2] */
    AM335X_CONF_SPI0_D0,          /* GPIO0[3] */
    AM335X_CONF_SPI0_D1,          /* GPIO0[4] */
    AM335X_CONF_SPI0_CS0,         /* GPIO0[5] */
    CONF_NOT_DEFINED,             /* GPIO0[6] */
    CONF_NOT_DEFINED,             /* GPIO0[7] */
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
    CONF_NOT_DEFINED,             /* GPIO0[20] */
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
    CONF_NOT_DEFINED,             /* GPIO1[21] */
    CONF_NOT_DEFINED,             /* GPIO1[22] */
    CONF_NOT_DEFINED,             /* GPIO1[23] */
    CONF_NOT_DEFINED,             /* GPIO1[24] */
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
    CONF_NOT_DEFINED,             /* GPIO3[20] */
    AM335X_CONF_MCASP0_AHCLKX     /* GPIO3[21] */ }
};

/* Get the address of Base Register + Offset for pad config */
uint32_t static inline bbb_conf_reg(uint32_t bank, uint32_t pin)
{
  return (AM335X_PADCONF_BASE + gpio_pad_conf[bank][pin]);
}

/* Get the value of Base Register + Offset */
uint32_t static inline bbb_reg(uint32_t bank, uint32_t reg)
{
  return (gpio_bank_addrs[bank] + reg);
}

static rtems_status_code bbb_select_pin_function(
  uint32_t bank,
  uint32_t pin,
  uint32_t type
) {

  if ( type == BBB_DIGITAL_IN ) {
    mmio_set(bbb_reg(bank, AM335X_GPIO_OE), BIT(pin));
  } else {
    mmio_clear(bbb_reg(bank, AM335X_GPIO_OE), BIT(pin));
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_gpio_bsp_multi_set(uint32_t bank, uint32_t bitmask)
{
  mmio_set(bbb_reg(bank, AM335X_GPIO_SETDATAOUT), bitmask);

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_gpio_bsp_multi_clear(uint32_t bank, uint32_t bitmask)
{
  mmio_set(bbb_reg(bank, AM335X_GPIO_CLEARDATAOUT), bitmask);

  return RTEMS_SUCCESSFUL;
}

uint32_t rtems_gpio_bsp_multi_read(uint32_t bank, uint32_t bitmask)
{
  return (bbb_reg(bank, AM335X_GPIO_DATAIN) & bitmask);
}

rtems_status_code rtems_gpio_bsp_set(uint32_t bank, uint32_t pin)
{
  mmio_set(bbb_reg(bank, AM335X_GPIO_SETDATAOUT), BIT(pin));

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_gpio_bsp_clear(uint32_t bank, uint32_t pin)
{
  mmio_set(bbb_reg(bank, AM335X_GPIO_CLEARDATAOUT), BIT(pin));

  return RTEMS_SUCCESSFUL;
}

uint32_t rtems_gpio_bsp_get_value(uint32_t bank, uint32_t pin)
{
  return (mmio_read(bbb_reg(bank, AM335X_GPIO_DATAIN)) & BIT(pin));
}

rtems_status_code rtems_gpio_bsp_select_input(
  uint32_t bank,
  uint32_t pin,
  void *bsp_specific
) {
  return bbb_select_pin_function(bank, pin, BBB_DIGITAL_IN);
}

rtems_status_code rtems_gpio_bsp_select_output(
  uint32_t bank,
  uint32_t pin,
  void *bsp_specific
) {
  return bbb_select_pin_function(bank, pin, BBB_DIGITAL_OUT);
}

rtems_status_code rtems_bsp_select_specific_io(
  uint32_t bank,
  uint32_t pin,
  uint32_t function,
  void *pin_data
) {
  return RTEMS_NOT_DEFINED;
}

rtems_status_code rtems_gpio_bsp_set_resistor_mode(
  uint32_t bank,
  uint32_t pin,
  rtems_gpio_pull_mode mode
) {
  /* TODO: Add support for setting up resistor moode */
  return RTEMS_NOT_DEFINED;
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
  event_status = mmio_read(bbb_reg(bank_nr, AM335X_GPIO_IRQSTATUS_0));

  /* Clear the interrupt line. */
  mmio_write(
    (bbb_reg(bank_nr, AM335X_GPIO_IRQSTATUS_0)), event_status);
  
  return event_status;
}

rtems_status_code rtems_bsp_enable_interrupt(
  uint32_t bank,
  uint32_t pin,
  rtems_gpio_interrupt interrupt
) {
  
  /* Enable IRQ generation for the specific pin */
  mmio_set(bbb_reg(bank, AM335X_GPIO_IRQSTATUS_SET_0), BIT(pin));
  
  switch ( interrupt ) {
    case FALLING_EDGE:
      /* Enables asynchronous falling edge detection. */
      mmio_set(bbb_reg(bank, AM335X_GPIO_FALLINGDETECT), BIT(pin));
      break;
    case RISING_EDGE:
      /* Enables asynchronous rising edge detection. */
      mmio_set(bbb_reg(bank, AM335X_GPIO_RISINGDETECT), BIT(pin));
      break;
    case BOTH_EDGES:
      /* Enables asynchronous falling edge detection. */
      mmio_set(bbb_reg(bank, AM335X_GPIO_FALLINGDETECT), BIT(pin));

      /* Enables asynchronous rising edge detection. */
      mmio_set(bbb_reg(bank, AM335X_GPIO_RISINGDETECT), BIT(pin));
      break;
    case LOW_LEVEL:
      /* Enables pin low level detection. */
      mmio_set(bbb_reg(bank, AM335X_GPIO_LEVELDETECT0), BIT(pin));
      break;
    case HIGH_LEVEL:
       /* Enables pin high level detection. */
      mmio_set(bbb_reg(bank, AM335X_GPIO_LEVELDETECT1), BIT(pin));
      break;
    case BOTH_LEVELS:
      /* Enables pin low level detection. */
      mmio_set(bbb_reg(bank, AM335X_GPIO_LEVELDETECT0), BIT(pin));

      /* Enables pin high level detection. */
      mmio_set(bbb_reg(bank, AM335X_GPIO_LEVELDETECT1), BIT(pin));
      break;
    case NONE:
    default:
      return RTEMS_UNSATISFIED;
  }

  /* The detection starts after 5 clock cycles as per AM335X TRM
   * This period is required to clean the synchronization edge/
   * level detection pipeline
   */
  asm volatile("nop"); asm volatile("nop"); asm volatile("nop");
  asm volatile("nop"); asm volatile("nop");
  
  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_bsp_disable_interrupt(
  uint32_t bank,
  uint32_t pin,
  rtems_gpio_interrupt interrupt
) {
  /* Clear IRQ generation for the specific pin */
  mmio_set(bbb_reg(bank, AM335X_GPIO_IRQSTATUS_CLR_0), BIT(pin));

  switch ( interrupt ) {
    case FALLING_EDGE:
      /* Disables asynchronous falling edge detection. */
      mmio_clear(bbb_reg(bank, AM335X_GPIO_FALLINGDETECT), BIT(pin));
      break;
    case RISING_EDGE:
      /* Disables asynchronous rising edge detection. */
      mmio_clear(bbb_reg(bank, AM335X_GPIO_RISINGDETECT), BIT(pin));
      break;
    case BOTH_EDGES:
      /* Disables asynchronous falling edge detection. */
      mmio_clear(bbb_reg(bank, AM335X_GPIO_FALLINGDETECT), BIT(pin));

      /* Disables asynchronous rising edge detection. */
      mmio_clear(bbb_reg(bank, AM335X_GPIO_RISINGDETECT), BIT(pin));
      break;
    case LOW_LEVEL:
      /* Disables pin low level detection. */
      mmio_clear(bbb_reg(bank, AM335X_GPIO_LEVELDETECT0), BIT(pin));
      break;
    case HIGH_LEVEL:
      /* Disables pin high level detection. */
       mmio_clear(bbb_reg(bank, AM335X_GPIO_LEVELDETECT1), BIT(pin));
      break;
    case BOTH_LEVELS:
      /* Disables pin low level detection. */
      mmio_clear(bbb_reg(bank, AM335X_GPIO_LEVELDETECT0), BIT(pin));

      /* Disables pin high level detection. */
      mmio_clear(bbb_reg(bank, AM335X_GPIO_LEVELDETECT1), BIT(pin));
      break;
    case NONE:
    default:
      return RTEMS_UNSATISFIED;
  }

  /* The detection starts after 5 clock cycles as per AM335X TRM
   * This period is required to clean the synchronization edge/
   * level detection pipeline
   */
  asm volatile("nop"); asm volatile("nop"); asm volatile("nop");
  asm volatile("nop"); asm volatile("nop");

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_gpio_bsp_multi_select(
  rtems_gpio_multiple_pin_select *pins,
  uint32_t pin_count,
  uint32_t select_bank
) {
  uint32_t register_address;
  uint32_t select_register;
  uint8_t i;

  register_address = gpio_bank_addrs[select_bank] + AM335X_GPIO_OE;

  select_register = REG(register_address);

  for ( i = 0; i < pin_count; ++i ) {
    if ( pins[i].function == DIGITAL_INPUT ) {
      select_register |= BIT(pins[i].pin_number);
    } else if ( pins[i].function == DIGITAL_OUTPUT ) {
      select_register &= ~BIT(pins[i].pin_number);
    } else { /* BSP_SPECIFIC function. */
      return RTEMS_NOT_DEFINED;
    }
  }

  REG(register_address) = select_register;

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

#endif /* IS_AM335X */

/* For support of BeagleboardxM */
#if IS_DM3730

/* Currently this section is just to satisfy
 * GPIO API and to make the build successful.
 * Later on support can be added here.
 */

rtems_status_code rtems_gpio_bsp_multi_set(uint32_t bank, uint32_t bitmask)
{
  return RTEMS_NOT_DEFINED;
}

rtems_status_code rtems_gpio_bsp_multi_clear(uint32_t bank, uint32_t bitmask)
{
  return RTEMS_NOT_DEFINED;
}

uint32_t rtems_gpio_bsp_multi_read(uint32_t bank, uint32_t bitmask)
{
  return -1;
}

rtems_status_code rtems_gpio_bsp_set(uint32_t bank, uint32_t pin)
{
  return RTEMS_NOT_DEFINED;
}

rtems_status_code rtems_gpio_bsp_clear(uint32_t bank, uint32_t pin)
{
  return RTEMS_NOT_DEFINED;
}

uint32_t rtems_gpio_bsp_get_value(uint32_t bank, uint32_t pin)
{
  return -1;
}

rtems_status_code rtems_gpio_bsp_select_input(
  uint32_t bank,
  uint32_t pin,
  void *bsp_specific
) {
  return RTEMS_NOT_DEFINED;
}

rtems_status_code rtems_gpio_bsp_select_output(
  uint32_t bank,
  uint32_t pin,
  void *bsp_specific
) {
  return RTEMS_NOT_DEFINED;
}

rtems_status_code rtems_bsp_select_specific_io(
  uint32_t bank,
  uint32_t pin,
  uint32_t function,
  void *pin_data
) {
  return RTEMS_NOT_DEFINED;
}

rtems_status_code rtems_gpio_bsp_set_resistor_mode(
  uint32_t bank,
  uint32_t pin,
  rtems_gpio_pull_mode mode
) {
  return RTEMS_NOT_DEFINED;
}

rtems_vector_number rtems_gpio_bsp_get_vector(uint32_t bank)
{
  return -1;
}

uint32_t rtems_gpio_bsp_interrupt_line(rtems_vector_number vector)
{
  return -1;
}

rtems_status_code rtems_bsp_enable_interrupt(
  uint32_t bank,
  uint32_t pin,
  rtems_gpio_interrupt interrupt
) {
  return RTEMS_NOT_DEFINED;
}

rtems_status_code rtems_bsp_disable_interrupt(
  uint32_t bank,
  uint32_t pin,
  rtems_gpio_interrupt interrupt
) {
  return RTEMS_NOT_DEFINED;
}

rtems_status_code rtems_gpio_bsp_multi_select(
  rtems_gpio_multiple_pin_select *pins,
  uint32_t pin_count,
  uint32_t select_bank
) {
  return RTEMS_NOT_DEFINED;
}

rtems_status_code rtems_gpio_bsp_specific_group_operation(
  uint32_t bank,
  uint32_t *pins,
  uint32_t pin_count,
  void *arg
) {
  return RTEMS_NOT_DEFINED;
}

#endif /* IS_DM3730 */