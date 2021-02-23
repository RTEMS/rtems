/**
 * @file
 *
 * @ingroup arm_beagle
 *
 * @brief Support for eQEP for the BeagleBone Black.
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020, 2021 James Fitzsimons <james.fitzsimons@gmail.com>
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
#include <stdio.h>
#include <stdlib.h>
#include <bsp/gpio.h>
#include <bsp/bbb-gpio.h>
#include <bsp.h>
#include <bsp/pwmss.h>
#include <bsp/qep.h>
#include <bsp/beagleboneblack.h>


/**
 * @brief Represents all the PWMSS QEP modules and their default values.
 */
static bbb_eqep bbb_eqep_table[ BBB_PWMSS_COUNT ] =
{
 {
  .pwmss_id = BBB_PWMSS0,
  .mmio_base = AM335X_EQEP_0_REGS,
  .irq = AM335X_INT_eQEP0INT,
  .timer_callback = NULL,
  .user = NULL,
  .count_mode = QUADRATURE_COUNT,
  .quadrature_mode = ABSOLUTE,
  .invert_qa = 0,
  .invert_qb = 0,
  .invert_qi = 0,
  .invert_qs = 0,
  .swap_inputs = 0
 },
 {
  .pwmss_id = BBB_PWMSS1,
  .mmio_base = AM335X_EQEP_1_REGS,
  .irq = AM335X_INT_eQEP1INT,
  .timer_callback = NULL,
  .user = NULL,
  .count_mode = QUADRATURE_COUNT,
  .quadrature_mode = ABSOLUTE,
  .invert_qa = 0,
  .invert_qb = 0,
  .invert_qi = 0,
  .invert_qs = 0,
  .swap_inputs = 0
 },
 {
  .pwmss_id = BBB_PWMSS2,
  .mmio_base = AM335X_EQEP_2_REGS,
  .irq = AM335X_INT_eQEP2INT,
  .timer_callback = NULL,
  .user = NULL,
  .count_mode = QUADRATURE_COUNT,
  .quadrature_mode = ABSOLUTE,
  .invert_qa = 0,
  .invert_qb = 0,
  .invert_qi = 0,
  .invert_qs = 0,
  .swap_inputs = 0
 }
};

/* eQEP Interrupt handler */
static void beagle_eqep_irq_handler(void *arg)
{
  uint16_t flags;
  int32_t position = 0;
  bbb_eqep* eqep = arg;

  /* Use the interrupt register (QFLG) mask to determine what caused the
   * interrupt. */
  flags = REG16(eqep->mmio_base + AM335x_EQEP_QFLG) & AM335x_EQEP_QFLG_MASK;
  /* Check the interrupt source to see if it was a unit timer overflow  */
  if (flags & AM335x_EQEP_QFLG_UTO && eqep->timer_callback != NULL) {
    /* Handle the unit timer overflow interrupt */
    position = beagle_qep_get_position(eqep->pwmss_id);
    eqep->timer_callback(eqep->pwmss_id, position, eqep->user);
  }

  /* Clear interrupt flags (write back triggered flags to the clear register) */
  REG16(eqep->mmio_base + AM335x_EQEP_QCLR) = flags;
}

rtems_status_code beagle_qep_init(BBB_PWMSS pwmss_id)
{
  rtems_status_code sc;
  uint16_t qdecctl;

  if ( pwmss_id >= BBB_PWMSS_COUNT ) {
    return RTEMS_INVALID_ID;
  }
  bbb_eqep* eqep = &bbb_eqep_table[pwmss_id];

  sc = pwmss_module_clk_config(eqep->pwmss_id);
  if (sc != RTEMS_SUCCESSFUL) {
    /* failed to successfully configure the PWMSS module clocks */
    return sc;
  }

  /* This enables clock for EQEP module in PWMSS subsystem. */
  REG(eqep->mmio_base + AM335X_PWMSS_CLKCONFIG) |= AM335x_EQEP_CLK_EN;

  /* Setup interrupt handler */
  sc = rtems_interrupt_handler_install(
      eqep->irq,
      NULL,
      RTEMS_INTERRUPT_UNIQUE,
      (rtems_interrupt_handler)beagle_eqep_irq_handler,
      (void*)eqep
  );

  /* The QDECCTL register configures the QEP Decoder module. We use it to set */
  /* the count mode, input inversion, channel swaps, unit timer interrupt etc. */
  qdecctl = 0;
  if (eqep->count_mode <= 3) {
    qdecctl |= eqep->count_mode << 14;

    /* If the count mode is UP_COUNT or DOWN_COUNT then only count on
     * the rising edge. QUADRATURE_COUNT and DIRECTION_COUNT count on
     * both edges.  */
    if (eqep->count_mode >= 2) {
      qdecctl |= AM335x_EQEP_QDECCTL_XCR;
    }
  }

  /* Should we swap the cha and chb inputs */
  if (eqep->swap_inputs == 1) {
    qdecctl |= AM335x_EQEP_QDECCTL_SWAP;
  }
  /* Should we invert the qa input */
  if (eqep->invert_qa == 1) {
    qdecctl |= AM335x_EQEP_QDECCTL_QAP;
  }
  /* Should we invert the qb input */
  if (eqep->invert_qb == 1) {
    qdecctl |= AM335x_EQEP_QDECCTL_QBP;
  }
  /* Should we invert the index input */
  if (eqep->invert_qi == 1) {
    qdecctl |= AM335x_EQEP_QDECCTL_QIP;

  }
  /* Should we invert the strobe input */
  if (eqep->invert_qs == 1) {
    qdecctl |= AM335x_EQEP_QDECCTL_QSP;
  }

  /* Write the configured decoder control settings to the QDECCTL register */
  REG16(eqep->mmio_base + AM335x_EQEP_QDECCTL) = qdecctl;
  /* Set the position counter initialisation register */
  REG(eqep->mmio_base + AM335x_EQEP_QPOSINIT) = 0;
  /* initialise the maximum position counter value */
  REG(eqep->mmio_base + AM335x_EQEP_QPOSMAX) = ~0;
  /* initialise the position counter register */
  REG(eqep->mmio_base + AM335x_EQEP_QPOSCNT) = 0;
  /* Enable Unit Time Period interrupt. */
  REG16(eqep->mmio_base + AM335x_EQEP_QEINT) |= AM335x_EQEP_QEINT_UTO;

  /* The following bitmasks enable the eQEP module with:
   * - the unit timer disabled
   * - will latch the value in QPOSLAT to QPOSCNT upon unit timer overflow
   * - will latch QPOSILAT on index signal.
   * - Software initialisation of position counter (will be set to 0 because
   *   QPOSINIT = 0).
   */
  uint32_t value = AM335x_EQEP_QEPCTL_QCLM | AM335x_EQEP_QEPCTL_IEL |
      AM335x_EQEP_QEPCTL_PHEN | AM335x_EQEP_QEPCTL_SWI;

  /* set the enable bit of the control register */
  REG16(eqep->mmio_base + AM335x_EQEP_QEPCTL) = value;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code beagle_qep_enable(BBB_PWMSS pwmss_id)
{
  if ( pwmss_id >= BBB_PWMSS_COUNT ) {
    return RTEMS_INVALID_ID;
  }
  const bbb_eqep* eqep = &bbb_eqep_table[pwmss_id];
  /* set the enable bit of the control register */
  REG16(eqep->mmio_base + AM335x_EQEP_QEPCTL) |= AM335x_EQEP_QEPCTL_PHEN;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code beagle_qep_disable(BBB_PWMSS pwmss_id)
{
  if ( pwmss_id >= BBB_PWMSS_COUNT ) {
    return RTEMS_INVALID_ID;
  }
  const bbb_eqep* eqep = &bbb_eqep_table[pwmss_id];
  /* clear the enable bit of the control register */
  REG16(eqep->mmio_base + AM335x_EQEP_QEPCTL) &= ~AM335x_EQEP_QEPCTL_PHEN;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code beagle_qep_pinmux_setup(
    bbb_qep_pin pin_no,
    BBB_PWMSS pwmss_id,
    bool pullup_enable
)
{
  rtems_status_code result = RTEMS_SUCCESSFUL;
  if ( pwmss_id >= BBB_PWMSS_COUNT ) {
    return RTEMS_INVALID_ID;
  }
  /* enable internal pull up / pull down resistor in pull up mode, and set the
   * pin as an input. */
  uint32_t pin_mode =  BBB_RXACTIVE;
  if ( pullup_enable ) {
    pin_mode |= BBB_PU_EN;
  }
  // The offsets from AM335X_PADCONF_BASE (44e10000) are named after the mode0 mux for that pin.
  if(pwmss_id == BBB_PWMSS0) {
    if (pin_no == BBB_P9_25_0_STROBE) {
      REG(AM335X_PADCONF_BASE + AM335X_CONF_MCASP0_AHCLKX) = pin_mode | BBB_MUXMODE(BBB_P9_25_MUX_QEP);
    } else if (pin_no == BBB_P9_27_0B_IN) {
      REG(AM335X_PADCONF_BASE + AM335X_CONF_MCASP0_FSR) = pin_mode | BBB_MUXMODE(BBB_P9_27_MUX_QEP);
    } else if (pin_no == BBB_P9_41_0_IDX) {
      REG(AM335X_PADCONF_BASE + AM335X_CONF_MCASP0_AXR1) = pin_mode | BBB_MUXMODE(BBB_P9_41_MUX_QEP);
    } else if (pin_no == BBB_P9_42_0A_IN) {
      REG(AM335X_PADCONF_BASE + AM335X_CONF_MCASP0_ACLKR) = pin_mode | BBB_MUXMODE(BBB_P9_42_MUX_QEP);
    } else {
      result = RTEMS_INTERNAL_ERROR;
    }
  } else if (pwmss_id == BBB_PWMSS1) {
    if (pin_no == BBB_P8_31_1_IDX) {
      REG(AM335X_PADCONF_BASE + AM335X_CONF_LCD_DATA14) = pin_mode | BBB_MUXMODE(BBB_P8_31_MUX_QEP);
    } else if (pin_no == BBB_P8_32_1_STROBE) {
      REG(AM335X_PADCONF_BASE + AM335X_CONF_LCD_DATA15) = pin_mode | BBB_MUXMODE(BBB_P8_32_MUX_QEP);
    } else if (pin_no == BBB_P8_33_1B_IN) {
      REG(AM335X_PADCONF_BASE + AM335X_CONF_LCD_DATA13) = pin_mode | BBB_MUXMODE(BBB_P8_33_MUX_QEP);
    } else if (pin_no == BBB_P8_35_1A_IN) {
      REG(AM335X_PADCONF_BASE + AM335X_CONF_LCD_DATA12) = pin_mode | BBB_MUXMODE(BBB_P8_35_MUX_QEP);
    } else {
      result = RTEMS_INTERNAL_ERROR;
    }
  } else if (pwmss_id == BBB_PWMSS2) {
    if (pin_no == BBB_P8_11_2B_IN) {
      REG(AM335X_PADCONF_BASE + AM335X_CONF_GPMC_AD13) = pin_mode | BBB_MUXMODE(BBB_P8_11_MUX_QEP);
    } else if (pin_no == BBB_P8_12_2A_IN) {
      REG(AM335X_PADCONF_BASE + AM335X_CONF_GPMC_AD12) = pin_mode | BBB_MUXMODE(BBB_P8_12_MUX_QEP);
    } else if (pin_no == BBB_P8_15_2_STROBE) {
      REG(AM335X_PADCONF_BASE + AM335X_CONF_GPMC_AD15) = pin_mode | BBB_MUXMODE(BBB_P8_15_MUX_QEP);
    } else if (pin_no == BBB_P8_16_2_IDX) {
      REG(AM335X_PADCONF_BASE + AM335X_CONF_GPMC_AD14) = pin_mode | BBB_MUXMODE(BBB_P8_16_MUX_QEP);
    } else if (pin_no == BBB_P8_39_2_IDX) {
      REG(AM335X_PADCONF_BASE + AM335X_CONF_LCD_DATA6) = pin_mode | BBB_MUXMODE(BBB_P8_39_MUX_QEP);
    } else if (pin_no == BBB_P8_40_2_STROBE) {
      REG(AM335X_PADCONF_BASE + AM335X_CONF_LCD_DATA7) = pin_mode | BBB_MUXMODE(BBB_P8_40_MUX_QEP);
    } else if (pin_no == BBB_P8_41_2A_IN) {
      REG(AM335X_PADCONF_BASE + AM335X_CONF_LCD_DATA4) = pin_mode | BBB_MUXMODE(BBB_P8_41_MUX_QEP);
    } else if (pin_no == BBB_P8_42_2B_IN) {
      REG(AM335X_PADCONF_BASE + AM335X_CONF_LCD_DATA5) = pin_mode | BBB_MUXMODE(BBB_P8_42_MUX_QEP);
    } else {
      result = RTEMS_INTERNAL_ERROR;
    }
  } else {
    result = RTEMS_INTERNAL_ERROR;
  }
  return result;
}

int32_t beagle_qep_get_position(BBB_PWMSS pwmss_id)
{
  int32_t position = 0;
  if ( pwmss_id >= BBB_PWMSS_COUNT ) {
    return -1;
  }
  const bbb_eqep* eqep = &bbb_eqep_table[pwmss_id];

  if (eqep->quadrature_mode == ABSOLUTE) {
    /* return the current value of the QPOSCNT register */
    position = REG(eqep->mmio_base + AM335x_EQEP_QPOSCNT);
  } else if (eqep->quadrature_mode == RELATIVE) {
    /* return the latched value from the last unit timer interrupt */
    position = REG(eqep->mmio_base + AM335x_EQEP_QPOSLAT);
  }

  return position;
}

rtems_status_code beagle_qep_set_position(BBB_PWMSS pwmss_id, uint32_t position)
{
  if ( pwmss_id >= BBB_PWMSS_COUNT ) {
    return RTEMS_INVALID_ID;
  }
  const bbb_eqep* eqep = &bbb_eqep_table[pwmss_id];
  /* setting the position only really makes sense in ABSOLUTE mode. */
  if (eqep->quadrature_mode == ABSOLUTE) {
    REG(eqep->mmio_base + AM335x_EQEP_QPOSCNT) = position;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code beagle_qep_set_count_mode(
    BBB_PWMSS pwmss_id,
    BBB_QEP_COUNT_MODE mode
)
{
  if ( pwmss_id >= BBB_PWMSS_COUNT ) {
    return RTEMS_INVALID_ID;
  }
  bbb_eqep* eqep = &bbb_eqep_table[pwmss_id];
  eqep->count_mode = mode;

  return RTEMS_SUCCESSFUL;
}

BBB_QEP_COUNT_MODE beagle_qep_get_count_mode(BBB_PWMSS pwmss_id)
{
  if ( pwmss_id >= BBB_PWMSS_COUNT ) {
    return RTEMS_INVALID_ID;
  }
  const bbb_eqep* eqep = &bbb_eqep_table[pwmss_id];

  return eqep->count_mode;
}

rtems_status_code beagle_qep_set_quadrature_mode(
    BBB_PWMSS pwmss_id,
    BBB_QEP_QUADRATURE_MODE mode
)
{
  uint16_t qepctl;
  if ( pwmss_id >= BBB_PWMSS_COUNT ) {
    return RTEMS_INVALID_ID;
  }
  bbb_eqep* eqep = &bbb_eqep_table[pwmss_id];

  qepctl = REG16(eqep->mmio_base + AM335x_EQEP_QEPCTL);

  if (mode == ABSOLUTE) {
    /*
     * Disable the unit timer position reset
     */
    qepctl &= ~AM335x_EQEP_QEPCTL_PCRM;

    eqep->quadrature_mode = ABSOLUTE;
  } else if (mode == RELATIVE) {
    /*
     *  enable the unit timer position reset
     */
    qepctl |= AM335x_EQEP_QEPCTL_PCRM;

    eqep->quadrature_mode = RELATIVE;
  }

  REG16(eqep->mmio_base + AM335x_EQEP_QEPCTL) = qepctl;

  return RTEMS_SUCCESSFUL;
}

BBB_QEP_QUADRATURE_MODE beagle_qep_get_quadrature_mode(BBB_PWMSS pwmss_id)
{
  if ( pwmss_id >= BBB_PWMSS_COUNT ) {
    return -1;
  }
  const bbb_eqep* eqep = &bbb_eqep_table[pwmss_id];

  return eqep->quadrature_mode;
}


/* Function to read the period of the unit time event timer */
uint32_t beagle_eqep_get_timer_period(BBB_PWMSS pwmss_id)
{
  uint64_t period;
  uint32_t timer_period;
  if ( pwmss_id >= BBB_PWMSS_COUNT ) {
    return -1;
  }
  const bbb_eqep* eqep = &bbb_eqep_table[pwmss_id];

  /* Convert from counts per interrupt back into period_ns */
  period = REG(eqep->mmio_base + AM335x_EQEP_QUPRD);
  period = period * NANO_SEC_PER_SEC;
  timer_period = (uint32_t)(period / SYSCLKOUT);

  return timer_period;
}

rtems_status_code beagle_eqep_set_timer_period(
    BBB_PWMSS pwmss_id,
    uint64_t period,
    bbb_eqep_timer_callback timer_callback,
    void* user
)
{
  uint16_t qepctl;
  uint64_t tmp_period;
  uint32_t timer_period;
  if ( pwmss_id >= BBB_PWMSS_COUNT ) {
    return RTEMS_INVALID_ID;
  }
  bbb_eqep* eqep = &bbb_eqep_table[pwmss_id];

  /* Disable the unit timer before modifying its period register */
  qepctl = readw(eqep->mmio_base + AM335x_EQEP_QEPCTL);
  qepctl &= ~(AM335x_EQEP_QEPCTL_UTE | AM335x_EQEP_QEPCTL_QCLM);
  REG16(eqep->mmio_base + AM335x_EQEP_QEPCTL) = qepctl;

  /* Zero the unit timer counter register */
  REG(eqep->mmio_base + AM335x_EQEP_QUTMR) = 0;

  /* If the timer is enabled (a non-zero period has been passed) */
  if (period) {
    /* update the period */
    tmp_period = period * SYSCLKOUT;
    timer_period = (uint32_t)(tmp_period / NANO_SEC_PER_SEC);
    REG(eqep->mmio_base + AM335x_EQEP_QUPRD) = timer_period;

    /* Enable unit timer, and latch QPOSLAT to QPOSCNT on timer expiration */
    qepctl |= AM335x_EQEP_QEPCTL_UTE | AM335x_EQEP_QEPCTL_QCLM;
    REG16(eqep->mmio_base + AM335x_EQEP_QEPCTL) = qepctl;

    /* attach the unit timer interrupt handler if one has been supplied */
    if (timer_callback != NULL) {
      eqep->timer_callback = timer_callback;
    }
    /* attach the user data if it has been provided */
    if (user != NULL) {
      eqep->user = user;
    }
  }

  return RTEMS_SUCCESSFUL;
}
