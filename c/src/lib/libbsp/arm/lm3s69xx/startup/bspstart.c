/*
 * Copyright © 2013 Eugeniy Meshcheryakov <eugen@debian.org>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bspopts.h>
#include <bsp/irq-generic.h>
#include <bsp/lm3s69xx.h>
#include <bsp/io.h>
#include <bsp/syscon.h>
#include <assert.h>

static void init_main_osc(void)
{
  volatile lm3s69xx_syscon *syscon = LM3S69XX_SYSCON;

  uint32_t sysdiv_val = LM3S69XX_PLL_FREQUENCY / LM3S69XX_SYSTEM_CLOCK;
  assert(sysdiv_val * LM3S69XX_SYSTEM_CLOCK == LM3S69XX_PLL_FREQUENCY);
  assert((sysdiv_val >= 4) && (sysdiv_val <= 16));

  uint32_t rcc = syscon->rcc;

  rcc = (rcc & ~SYSCONRCC_USESYSDIV) | SYSCONRCC_BYPASS;
  syscon->rcc = rcc;

  rcc = (rcc & ~(SYSCONRCC_PWRDN | SYSCONRCC_XTAL_MSK | SYSCONRCC_OSCSRC_MSK))
      | SYSCONRCC_XTAL(LM3S69XX_XTAL_CONFIG) | SYSCONRCC_OSCSRC_MOSC;
  syscon->rcc = rcc;

  rcc = (rcc & ~SYSCONRCC_SYSDIV_MSK) | SYSCONRCC_SYSDIV(sysdiv_val / 2 - 1)
      | SYSCONRCC_USESYSDIV;
  syscon->rcc = rcc;

  while ((syscon->ris & SYSCONRIS_PLLLRIS) == 0)
      /* Wait for PLL lock */;

  rcc &= ~SYSCONRCC_BYPASS;
  syscon->rcc = rcc;
}

static const lm3s69xx_gpio_config start_config_gpio[] = {
#ifdef LM3S69XX_ENABLE_UART_0
#if defined(LM3S69XX_MCU_LM3S3749) || defined(LM3S69XX_MCU_LM3S6965)
  LM3S69XX_PIN_UART_RX(LM3S69XX_PORT_A, 0),
  LM3S69XX_PIN_UART_TX(LM3S69XX_PORT_A, 1),
#else
#error No GPIO pin configuration for UART 0
#endif
#endif /* LM3S69XX_ENABLE_UART_0 */

#ifdef LM3S69XX_ENABLE_UART_1
#if defined(LM3S69XX_MCU_LM3S3749)
  LM3S69XX_PIN_UART_RX(LM3S69XX_PORT_B, 0),
  LM3S69XX_PIN_UART_TX(LM3S69XX_PORT_B, 1),
#elif defined(LM3S69XX_MCU_LM3S6965)
  LM3S69XX_PIN_UART_RX(LM3S69XX_PORT_D, 2),
  LM3S69XX_PIN_UART_TX(LM3S69XX_PORT_D, 3),
#else
#error No GPIO pin configuration for UART 1
#endif
#endif /* LM3S69XX_ENABLE_UART_1 */

#ifdef LM3S69XX_ENABLE_UART_2
#if defined(LM3S69XX_MCU_LM3S3749)
  LM3S69XX_PIN_UART_RX(LM3S69XX_PORT_D, 0),
  LM3S69XX_PIN_UART_TX(LM3S69XX_PORT_D, 1),
#elif defined(LM3S69XX_MCU_LM3S6965)
  LM3S69XX_PIN_UART_RX(LM3S69XX_PORT_G, 0),
  LM3S69XX_PIN_UART_TX(LM3S69XX_PORT_G, 1),
#else
#error No GPIO pin configuration for UART 2
#endif
#endif /* LM3S69XX_ENABLE_UART_2 */
};

static void init_gpio(void)
{
#if LM3S69XX_USE_AHB_FOR_GPIO
  volatile lm3s69xx_syscon *syscon = LM3S69XX_SYSCON;

  syscon->gpiohbctl |= SYSCONGPIOHBCTL_PORTA | SYSCONGPIOHBCTL_PORTB
      | SYSCONGPIOHBCTL_PORTC | SYSCONGPIOHBCTL_PORTD
      | SYSCONGPIOHBCTL_PORTE | SYSCONGPIOHBCTL_PORTF
      | SYSCONGPIOHBCTL_PORTG
#if LM3S69XX_NUM_GPIO_BLOCKS > 7
      | SYSCONGPIOHBCTL_PORTH
#endif
      ;

#endif /* LM3S69XX_USE_AHB_FOR_GPIO */

  lm3s69xx_gpio_set_config_array(start_config_gpio,
      sizeof(start_config_gpio) / sizeof(start_config_gpio[0]));
}

void bsp_start(void)
{
  init_main_osc();
  init_gpio();
  bsp_interrupt_initialize();
}
