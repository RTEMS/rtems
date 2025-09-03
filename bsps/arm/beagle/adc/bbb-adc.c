/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup arm_beagle
 *
 * @brief ADC definitions.
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

#include <libcpu/am335x.h>
#include <bsp/irq-generic.h>
#include <rtems.h>
#include <bsp/bbb-adc.h>
#include <bsp/irq.h>
#include <bsp/beagleboneblack.h>
#include <bsp.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

RTEMS_INTERRUPT_LOCK_DEFINE( static, rtems_adc_bsp_lock, "rtems_adc_bsp_lock" );

/* Waits a number of CPU cycles. */
static void arm_delay(uint8_t cycles)
{
  uint8_t i;

  for ( i = 0; i < cycles; ++i ) {
    asm volatile("nop");
  }
}

static inline void reg_update(uint32_t reg, uint32_t val)
{
  mmio_write(reg, val);
}

static inline void reg_update_or(uint32_t reg, uint32_t val)
{
  uint32_t reg_val=mmio_read(reg);
  reg_val |= val;
  mmio_write(reg, reg_val);
}

/* Configure STEPCONFIG register with average of 16 */
static inline void stepc_update_16(uint32_t reg, uint32_t val)
{
  mmio_write(reg,(val | AM335X_ADC_AVG16));
}

/* Configure STEPDELAY register */
static inline void stepd_update(uint32_t reg)
{
  mmio_write(reg,(0x0F)<<24);
}

static inline uint32_t get_val(uint32_t reg, uint32_t val)
{
  return (mmio_read(reg) & val);
}

void rtems_adc_init(void)
{

  rtems_interrupt_lock_context lock_context;

  rtems_interrupt_lock_acquire(&rtems_adc_bsp_lock, &lock_context);
  /* Enable the CM_WKUP_ADC_CLKCTRL with CM_WKUP_MODUELEMODE_ENABLE */
  reg_update_or(AM335X_CM_WKUP_ADC_CLKCTRL, AM335X_CM_WKUP_MODULEMODE_ENABLE); /* 0x02 */

  while(!( get_val(AM335X_CM_WKUP_ADC_CLKCTRL,AM335X_CM_WKUP_MODULEMODE_ENABLE))){
    /* Here waiting period for intialization of adc clock module */
  }

  /* Make sure STEPCONFIG write protect is off */
  reg_update_or(AM335X_ADC_CTRL, AM335X_ADC_STEPCONFIG_WRITE_PROTECT_OFF);

  /* ADC_STEPCONFIG for each AIN pin */
  stepc_update_16(AM335X_ADC_STEPCONFIG1,0x00<<19);
  stepd_update(AM335X_ADC_STEPDELAY1);
  stepc_update_16(AM335X_ADC_STEPCONFIG2,0x01<<19);
  stepd_update(AM335X_ADC_STEPDELAY2);
  stepc_update_16(AM335X_ADC_STEPCONFIG3,0x02<<19);
  stepd_update(AM335X_ADC_STEPDELAY3);
  stepc_update_16(AM335X_ADC_STEPCONFIG4,0x03<<19);
  stepd_update(AM335X_ADC_STEPDELAY4);
  stepc_update_16(AM335X_ADC_STEPCONFIG5,0x04<<19);
  stepd_update(AM335X_ADC_STEPDELAY5);
  stepc_update_16(AM335X_ADC_STEPCONFIG6,0x05<<19);
  stepd_update(AM335X_ADC_STEPDELAY6);
  stepc_update_16(AM335X_ADC_STEPCONFIG7,0x06<<19);
  stepd_update(AM335X_ADC_STEPDELAY7);
  stepc_update_16(AM335X_ADC_STEPCONFIG8,0x07<<19);
  stepd_update(AM335X_ADC_STEPDELAY8);

  /* ADC_CTRL is enabled + store the Step ID-number with the captured ADC data in the Fifo */
  reg_update_or(AM335X_ADC_CTRL,0x03);

  rtems_interrupt_lock_release(&rtems_gpio_bsp_lock, &lock_context);
}

int rtems_adc_read(unsigned int pin_number)
{
  rtems_interrupt_lock_context lock_context;

  /* The clock module is not enabled */
  if(get_val(AM335X_CM_WKUP_ADC_CLKCTRL,AM335X_CM_WKUP_IDLEST_DISABLED))
     rtems_adc_init();

  rtems_interrupt_lock_acquire(&rtems_adc_bsp_lock, &lock_context);
  /* Enable the step sequencer for the given pin */
  reg_update(AM335X_ADC_STEPENABLE,(0x01<<(pin_number+1)));

  rtems_interrupt_lock_release(&rtems_gpio_bsp_lock, &lock_context);
  /* Return the value of data register FIFO0 */
  return (get_val(AM335X_ADC_FIFO0DATA,AM335X_ADC_FIFO_MASK));
}

unsigned int rtems_adc_read_multiple(unsigned int len, unsigned int wait, unsigned int mask, unsigned int data[])
{
  rtems_interrupt_lock_context lock_context;

  /* The clock module is not enabled */
  if(get_val(AM335X_CM_WKUP_ADC_CLKCTRL,AM335X_CM_WKUP_IDLEST_DISABLED))
     rtems_adc_init();

  rtems_interrupt_lock_acquire(&rtems_adc_bsp_lock, &lock_context);
  /* Enable the step sequencer for the given pins/mask */
  reg_update(AM335X_ADC_STEPENABLE, mask);
  int timeOut = len * wait; 
  uint32_t fifo;
  do {
    fifo = get_val(AM335X_ADC_FIFO0COUNT, 0x7F);
    timeOut-=1;
    arm_delay(5);
  } while( (len != fifo) && timeOut);

  unsigned int d;
  for(unsigned int i = 0; i < fifo; i++) {
    d = get_val(AM335X_ADC_FIFO0DATA, 0x000F0FFF);
    if (d>>16 < len) // chid in head of data
     data[d>>16] = d & 0xfff;
  }
  rtems_interrupt_lock_release(&rtems_gpio_bsp_lock, &lock_context);
  return (fifo);
}

