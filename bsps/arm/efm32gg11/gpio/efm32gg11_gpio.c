/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMEFM32GG11
 *
 * @brief EFM32GG11 GPIO Support
 */

/*
 * Copyright (C) 2025 Allan N. Hessenflow
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

#include <bsp.h>
#include <bsp/processor.h>
#include <bsp/efm32gg11_gpio.h>
#include <rtems.h>
#include <rtems/bspIo.h>


static struct {
  struct {
    void (*cb)(void *);
    void *arg;
  } isr[32];
} gpio_context;


static void gpio_interrupt(void *arg)
{
  bool odd = (bool) arg;
  uint32_t mask;
  int int_number;

  mask = odd ? 0xaaaaaaaa : 0x55555555;
  mask &= GPIO->IF;
  mask &= GPIO->IEN;
  GPIO->IFC = mask;
  __DSB();
  if (odd) {
    int_number = 1;
    mask >>= 1;
  } else {
    int_number = 0;
  }
  while (mask) {
    if ((mask & 1) && gpio_context.isr[int_number].cb)
      gpio_context.isr[int_number].cb(gpio_context.isr[int_number].arg);
    int_number += 2;
    mask >>= 2;
  }
}

void efm32gg11_gpio_init_0(void)
{
  CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_GPIO;
}

void efm32gg11_gpio_init(void)
{
  rtems_interrupt_handler_install(GPIO_EVEN_IRQn, "GPIO_EVEN",
                                  RTEMS_INTERRUPT_UNIQUE,
                                  gpio_interrupt, (void *) false);
  rtems_interrupt_handler_install(GPIO_ODD_IRQn, "GPIO_ODD",
                                  RTEMS_INTERRUPT_UNIQUE,
                                  gpio_interrupt, (void *) true);
}

int efm32gg11_gpio_int_register(uint8_t port, uint8_t pin,
                                void (*cb)(void *), void *arg)
{
  rtems_interrupt_level level;
  uint8_t shift;
  int group;
  uint32_t r;
  int which;
  int i;

  which = -1;
  if (port < sizeof(GPIO->P) / sizeof(GPIO->P[0]) && pin < 16) {
    group = pin / 4;
    for (i = group * 4; i < group * 4 + 4 && which < 0; i++) {
      rtems_interrupt_disable(level);
      if (gpio_context.isr[i].cb == NULL) {
        gpio_context.isr[i].arg = arg;
        gpio_context.isr[i].cb = cb;
        if (i >= 8) {
          shift = (i - 8) * _GPIO_EXTIPSELH_EXTIPSEL9_SHIFT;
          r = GPIO->EXTIPSELH;
          r &= ~(_GPIO_EXTIPSELH_EXTIPSEL8_MASK << shift);
          r |= (uint32_t) port << shift;
          GPIO->EXTIPSELH = r;

          shift = (i - 8) * _GPIO_EXTIPINSELH_EXTIPINSEL9_SHIFT;
          r = GPIO->EXTIPINSELH;
          r &= ~(_GPIO_EXTIPINSELH_EXTIPINSEL8_MASK << shift);
          r |= (uint32_t) (pin & 0x03) << shift;
          GPIO->EXTIPINSELH = r;
        } else {
          shift = i * _GPIO_EXTIPSELL_EXTIPSEL1_SHIFT;
          r = GPIO->EXTIPSELL;
          r &= ~(_GPIO_EXTIPSELL_EXTIPSEL0_MASK << shift);
          r |= (uint32_t) port << shift;
          GPIO->EXTIPSELL = r;

          shift = i * _GPIO_EXTIPINSELL_EXTIPINSEL1_SHIFT;
          r = GPIO->EXTIPINSELL;
          r &= ~(_GPIO_EXTIPINSELL_EXTIPINSEL0_MASK << shift);
          r |= (uint32_t) (pin & 0x03) << shift;
          GPIO->EXTIPINSELH = r;
        }
        which = i;
      }
      rtems_interrupt_enable(level);
    }
  }

  return which;
}

void efm32gg11_gpio_wake_pin_int_register(unsigned int which,
                                          void (*cb)(void *), void *arg)
{

  if (which >= 16 && which < 32) {
    gpio_context.isr[which].arg = arg;
    gpio_context.isr[which].cb = cb;
  }
}

void efm32gg11_gpio_int_mode(unsigned int which, bool rising, bool falling)
{
  rtems_interrupt_level level;
  uint32_t mask;

  if (which < 16) {
    mask = 1 << which;
    rtems_interrupt_disable(level);
    if (rising)
      GPIO->EXTIRISE |= mask;
    else
      GPIO->EXTIRISE &= ~mask;
    if (falling)
      GPIO->EXTIFALL |= mask;
    else
      GPIO->EXTIFALL &= ~mask;
    rtems_interrupt_enable(level);
  }
}

void efm32gg11_gpio_wake_pin_level(unsigned int which, bool high)
{
  rtems_interrupt_level level;
  uint32_t mask;

  if (which >= 16 && which < 32) {
    mask = 1 << which;
    rtems_interrupt_disable(level);
    if (high)
      GPIO->EXTILEVEL |= mask;
    else
      GPIO->EXTILEVEL &= ~mask;
    rtems_interrupt_enable(level);
  }
}

void efm32gg11_gpio_int_enable(unsigned int which, bool enable)
{
  rtems_interrupt_level level;
  uint32_t mask;

  if (which < 32) {
    mask = 1 << which;
    rtems_interrupt_disable(level);
    if (enable)
      GPIO->IEN |= mask;
    else
      GPIO->IEN &= ~mask;
    rtems_interrupt_enable(level);
  }
}

void efm32gg11_gpio_mode(uint8_t port, uint8_t pin, uint8_t mode)
{
  rtems_interrupt_level level;
  uint32_t r;
  bool high;
  uint8_t shift;

  if (port < sizeof(GPIO->P) / sizeof(GPIO->P[0]) &&
      pin < 16 &&
      mode <= _GPIO_P_MODEL_MODE0_MASK) {
    high = pin >= 8;
    shift = (high ? pin - 8 : pin) * _GPIO_P_MODEL_MODE1_SHIFT;
    rtems_interrupt_disable(level);
    r = high ? GPIO->P[port].MODEH : GPIO->P[port].MODEL;
    r &= ~(_GPIO_P_MODEL_MODE0_MASK << shift);
    r |= (uint32_t) mode << shift;
    if (high)
      GPIO->P[port].MODEH = r;
    else
      GPIO->P[port].MODEL = r;
    rtems_interrupt_enable(level);
  }
}

void efm32gg11_gpio_clear_set(uint8_t port, uint16_t clear, uint16_t set)
{
  rtems_interrupt_level level;
  uint16_t r;

  if (port < sizeof(GPIO->P) / sizeof(GPIO->P[0])) {
    rtems_interrupt_disable(level);
    r = GPIO->P[port].DOUT;
    r &= ~(uint32_t) clear;
    r |= set;
    GPIO->P[port].DOUT = r;
    rtems_interrupt_enable(level);
  }
}
