/*
 * Copyright © 2013 Eugeniy Meshcheryakov <eugen@debian.org>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp/syscon.h>
#include <bsp/lm3s69xx.h>
#include <rtems.h>

static void delay_3_clocks(void)
{
  asm volatile(
      "nop\n\t"
      "nop\n\t"
      "nop");
}

void __attribute__((naked)) lm3s69xx_syscon_delay_3x_clocks(unsigned long x_count)
{
    asm volatile(
                 "subs  r0, #1\n\t"
                 "bne   lm3s69xx_syscon_delay_3x_clocks\n\t"
                 "bx    lr"
                 );
}

void lm3s69xx_syscon_enable_gpio_clock(unsigned int port, bool enable)
{
  volatile lm3s69xx_syscon *syscon = LM3S69XX_SYSCON;
  uint32_t mask = 1 << port;
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);

  if (enable)
    syscon->rcgc2 |= mask;
  else
    syscon->rcgc2 &= ~mask;

  delay_3_clocks();

  rtems_interrupt_enable(level);
}

void lm3s69xx_syscon_enable_uart_clock(unsigned int port, bool enable)
{
  volatile lm3s69xx_syscon *syscon = LM3S69XX_SYSCON;
  uint32_t mask = 1 << port;
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);

  if (enable)
    syscon->rcgc1 |= mask;
  else
    syscon->rcgc1 &= ~mask;

  delay_3_clocks();

  rtems_interrupt_enable(level);
}

void lm3s69xx_syscon_enable_ssi_clock(unsigned int port, bool enable)
{
  volatile lm3s69xx_syscon *syscon = LM3S69XX_SYSCON;
  uint32_t mask = 1 << (port + 4);
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);

  if (enable)
    syscon->rcgc1 |= mask;
  else
    syscon->rcgc1 &= ~mask;

  delay_3_clocks();

  rtems_interrupt_enable(level);
}

void lm3s69xx_syscon_enable_pwm_clock(bool enable)
{
  volatile lm3s69xx_syscon *syscon = LM3S69XX_SYSCON;
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);

  if (enable)
    syscon->rcgc0 |= SYSCONRCGC0_PWM;
  else
    syscon->rcgc0 &= ~SYSCONRCGC0_PWM;

  delay_3_clocks();

  rtems_interrupt_enable(level);
}

/**
 * Sets PWMDIV field in the RCC register.
 *
 * @note div should be one of SCRCC_PWMDIV_DIV?_VAL constants.
 */
void lm3s69xx_syscon_set_pwmdiv(unsigned int div)
{
  volatile lm3s69xx_syscon *syscon = LM3S69XX_SYSCON;
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  syscon->rcc = (syscon->rcc & ~SYSCONRCC_PWMDIV_MSK) | SYSCONRCC_PWMDIV(div)
      | SYSCONRCC_USEPWMDIV;
  rtems_interrupt_enable(level);
}
