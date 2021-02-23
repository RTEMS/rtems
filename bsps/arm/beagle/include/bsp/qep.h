/**
 * @file
 *
 * @ingroup arm_beagle
 *
 * @brief eQEP (enhanced Quadrature Encoder Pulse) support API.
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

/**
 *
 * For details of the Enhanced Quadrature Encoder Pulse (eQEP) Module refer to
 * page 2511 of the TI Technical Reference Manual
 * (https://www.ti.com/lit/ug/spruh73q/spruh73q.pdf)
 *
 * This driver supports using the QEP modules in Quadrature-clock Mode.
 * Direction-count Mode is not currently supported. Similarly the QEPI: Index
 * or Zero Marker and QEPS: Strobe Input pins are not currently supported.
 *
 * The mode can be any one of:
 *  - Quadrature-count mode - For encoders that generate pulses 90 degrees
 *      out of phase for determining direction and speed.
 *  - Direction-count mode - for position encoders that provide direction and
 *      clock outputs, instead of quadrature outputs.
 *  - UP-count mode - The counter direction signal is hard-wired for up count
 *      and the position counter is used to measure the frequency of the QEPA
 *      input.
 *  - DOWN-count mode - The counter direction signal is hard-wired for a down
 *      count and the position counter is used to measure the frequency of the
 *      QEPA input.
 *
 * When the eQEP module is configured in quadrature mode, the module
 * can either provide an absolute position, or a relative position. Absolute
 * simply increments or decrements depending on the direction. Relative
 * increments until the unit timer overflows at which point it latches the
 * position value, resets the position count to zero and starts again.
 */

#ifndef LIBBSP_ARM_BEAGLE_QEP_H
#define LIBBSP_ARM_BEAGLE_QEP_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define AM335X_EQEP_REGS                       (0x00000180)
#define AM335X_EQEP_0_REGS                     (AM335X_PWMSS0_MMAP_ADDR + AM335X_EQEP_REGS)
#define AM335X_EQEP_1_REGS                     (AM335X_PWMSS1_MMAP_ADDR + AM335X_EQEP_REGS)
#define AM335X_EQEP_2_REGS                     (AM335X_PWMSS2_MMAP_ADDR + AM335X_EQEP_REGS)

/* eQEP registers of the PWMSS modules - see page 1672 of the TRM for details */
#define AM335x_EQEP_QPOSCNT       0x0   /* eQEP Position Counter */
#define AM335x_EQEP_QPOSINIT      0x4   /* eQEP Position Counter Initialization */
#define AM335x_EQEP_QPOSMAX       0x8   /* eQEP Maximum Position Count */
#define AM335x_EQEP_QPOSCMP       0xC   /* eQEP Position-Compare */
#define AM335x_EQEP_QPOSILAT      0x10  /* eQEP Index Position Latch */
#define AM335x_EQEP_QPOSSLAT      0x14  /* eQEP Strobe Position Latch */
#define AM335x_EQEP_QPOSLAT       0x18  /* eQEP Position Counter Latch */
#define AM335x_EQEP_QUTMR         0x1C  /* eQEP Unit Timer */
#define AM335x_EQEP_QUPRD         0x20  /* eQEP Unit Period */
#define AM335x_EQEP_QWDTMR        0x24  /* eQEP Watchdog Timer */
#define AM335x_EQEP_QWDPRD        0x26  /* eQEP Watchdog Period */
#define AM335x_EQEP_QDECCTL       0x28  /* eQEP Decoder Control */
#define AM335x_EQEP_QEPCTL        0x2A  /* eQEP Control */
#define AM335x_EQEP_QCAPCTL       0x2C  /* eQEP Capture Control */
#define AM335x_EQEP_QPOSCTL       0x2E  /* eQEP Position-Compare Control */
#define AM335x_EQEP_QEINT         0x30  /* eQEP Interrupt Enable */
#define AM335x_EQEP_QFLG          0x32  /* eQEP Interrupt Flag */
#define AM335x_EQEP_QCLR          0x34  /* eQEP Interrupt Clear */
#define AM335x_EQEP_QFRC          0x36  /* eQEP Interrupt Force */
#define AM335x_EQEP_QEPSTS        0x38  /* eQEP Status */
#define AM335x_EQEP_QCTMR         0x3A  /* eQEP Capture Timer */
#define AM335x_EQEP_QCPRD         0x3C  /* eQEP Capture Period */
#define AM335x_EQEP_QCTMRLAT      0x3E  /* eQEP Capture Timer Latch */
#define AM335x_EQEP_QCPRDLAT      0x40  /* eQEP Capture Period Latch */
#define AM335x_EQEP_REVID         0x5C  /* eQEP Revision ID */

/* bitmasks for eQEP registers  */
#define AM335x_EQEP_QEPCTL_UTE    (1 << 1)
#define AM335x_EQEP_QEPCTL_QCLM   (1 << 2)
#define AM335x_EQEP_QEPCTL_PHEN   (1 << 3)
#define AM335x_EQEP_QEPCTL_IEL    (1 << 4)
#define AM335x_EQEP_QEPCTL_SWI    (1 << 7)
#define AM335x_EQEP_QEPCTL_PCRM   (3 << 12)
#define AM335x_EQEP_QDECCTL_QSRC  (3 << 14)
#define AM335x_EQEP_QDECCTL_XCR   (1 << 11)
#define AM335x_EQEP_QDECCTL_SWAP  (1 << 10)
#define AM335x_EQEP_QDECCTL_IGATE (1 << 9)
#define AM335x_EQEP_QDECCTL_QAP   (1 << 8)
#define AM335x_EQEP_QDECCTL_QBP   (1 << 7)
#define AM335x_EQEP_QDECCTL_QIP   (1 << 6)
#define AM335x_EQEP_QDECCTL_QSP   (1 << 5)
#define AM335x_EQEP_CLK_EN        (1 << 4)
#define AM335x_EQEP_QEINT_UTO     (1 << 11)
#define AM335x_EQEP_QFLG_UTO      (1 << 11)
#define AM335x_EQEP_QFLG_MASK     0x0FFF

/* The pin mux modes for the QEP input pins on the P8 and P9 headers */
#define BBB_P8_11_MUX_QEP 4
#define BBB_P8_12_MUX_QEP 4
#define BBB_P8_15_MUX_QEP 4
#define BBB_P8_16_MUX_QEP 4
#define BBB_P8_31_MUX_QEP 2
#define BBB_P8_32_MUX_QEP 2
#define BBB_P8_33_MUX_QEP 2
#define BBB_P8_35_MUX_QEP 2
#define BBB_P8_39_MUX_QEP 3
#define BBB_P8_40_MUX_QEP 3
#define BBB_P8_41_MUX_QEP 3
#define BBB_P8_42_MUX_QEP 3
#define BBB_P9_25_MUX_QEP 1
#define BBB_P9_27_MUX_QEP 1
#define BBB_P9_41_MUX_QEP 1
#define BBB_P9_42_MUX_QEP 1

#define NANO_SEC_PER_SEC  1000000000
/* This is the max clock rate for the EPWMSS module. See 15.1.2.2 of the TRM.
 * If the CPU was using dynamic scaling this could potentially be wrong */
#define SYSCLKOUT         100000000

/**
 * @brief The set of possible eQEP Position Counter Input Modes
 *
 * Enumerated type to define various modes for the eQEP module. The values
 * correspond to the values for the QSRC bits of the QDECCTL register.
 */
typedef enum {
  QUADRATURE_COUNT = 0,
  DIRECTION_COUNT,
  UP_COUNT,
  DOWN_COUNT
} BBB_QEP_COUNT_MODE;

/**
 * @brief The set of possible modes for Quadrature decode
 *
 */
typedef enum {
  ABSOLUTE = 0,
  RELATIVE
} BBB_QEP_QUADRATURE_MODE;

/**
 * @brief The set of possible eQEP input pins
 *
 */
typedef enum {
  BBB_P8_11_2B_IN,
  BBB_P8_12_2A_IN,
  BBB_P8_15_2_STROBE,
  BBB_P8_16_2_IDX,
  BBB_P8_31_1_IDX,
  BBB_P8_32_1_STROBE,
  BBB_P8_33_1B_IN,
  BBB_P8_35_1A_IN,
  BBB_P8_39_2_IDX,
  BBB_P8_40_2_STROBE,
  BBB_P8_41_2A_IN,
  BBB_P8_42_2B_IN,
  BBB_P9_25_0_STROBE,
  BBB_P9_27_0B_IN,
  BBB_P9_41_0_IDX,
  BBB_P9_42_0A_IN
} bbb_qep_pin;


/**
 * @brief This function definition is used to declare a callback function that
 * will be called by the interrupt handler of the QEP driver. In order for the
 * interrupt event to trigger the driver must be configured in RELATIVE mode
 * (using the beagle_qep_get_quadrature_mode function), and the unit timer must
 * have been configured (using the beagle_eqep_set_timer_period function).
 *
 * @param BBB_PWMSS This argument is provided to the user call back function so
 * that the user can tell which QEP module raised the interrupt.
 *
 * @param position The value of the position counter that was latched when the
 * unit timer raised this interrupt. This is the value that would be returned
 * by calling "beagle_qep_get_position".
 *
 * @param user This a pointer to a user provided data structure. The user sets
 * this pointer value when configuring the unit timer callback via the
 * beagle_eqep_set_timer_period function and it is returned here as an argument.
 * The driver does not touch this value.
 */
typedef void (*bbb_eqep_timer_callback)(
  BBB_PWMSS,
  uint32_t position,
  void* user
);


/**
 * @brief This structure represents an eQEP module instance. The members
 * represent the configuration of a specific eQEP module. There are three
 * eQEP modules in the AM335x, one associated with each PWMSS unit.
 * @var bbb_eqep::pwmss_id The PWMSS unit this eQEP module belongs to.
 * @var bbb_eqep::mmio_base The base address for this eQEP modules registers
 * @var bbb_eqep::irq The IRQ vector for this eQEP module
 * @var bbb_eqep::timer_callback An optional user provided callback function
 * when the driver is configured in RELATIVE mode using the unit timer
 * @var bbb_eqep::user An optional pointer to user provided data that will be
 * handed to the callback function as an argument.
 * @var bbb_eqep::count_mode The count mode for this eQEP module. Defaults to
 * QUADRATURE.
 * @var bbb_eqep::quadrature_mode The mode for QUADRATURE operation. Defaults
 * to ABSOLUTE - will count up to overflow or until the user resets the
 * position. Can be set to RELATIVE which will trigger a call back of the unit
 * timer if configured.
 * @var bbb_eqep::invert_qa 1 to invert the A channel input, 0 to leave as is.
 * @var bbb_eqep::invert_qb 1 to invert the B channel input, 0 to leave as is.
 * @var bbb_eqep::invert_qi 1 to invert the INDEX input, 0 to leave as is.
 * @var bbb_eqep::invert_qs 1 to invert the STROBE input, 0 to leave as is.
 * @var bbb_eqep::swap_inputs 1 to swap the A and B channel inputs, 0 to leave
 * as is.
 *
 */
typedef struct {
  const BBB_PWMSS pwmss_id;
  const uint32_t mmio_base;
  const rtems_vector_number irq;
  bbb_eqep_timer_callback timer_callback;
  void* user;
  BBB_QEP_COUNT_MODE count_mode;
  BBB_QEP_QUADRATURE_MODE quadrature_mode;
  uint32_t invert_qa;
  uint32_t invert_qb;
  uint32_t invert_qi;
  uint32_t invert_qs;
  uint32_t swap_inputs;
} bbb_eqep;


/**
 * @brief Initialises the eQEP module of the specified PWMSS unit. This
 * configures the clocks, sets up the interrupt handler and unit timer,
 * The module is configured in Quadrature decode mode using
 * absolute position by default.
 *
 * @param pwmss_id The PWMSS module to configure the eQEP for.
 * @return RTEMS_SUCCESSFUL if ok, RTEMS_INVALID_ID if an invalid pwmss_id is
 * supplied.
 */
rtems_status_code beagle_qep_init(BBB_PWMSS pwmss_id);

/**
 * @brief Enables the eQEP module of the specified PWMSS unit.
 *
 * @param pwmss_id The PWMSS module which will have the eQEP function enabled.
 * @return RTEMS_SUCCESSFUL if ok, RTEMS_INVALID_ID if an invalid pwmss_id is
 * supplied.
 */
rtems_status_code beagle_qep_enable(BBB_PWMSS pwmss_id);

/**
 * @brief Disables the eQEP module of the specified PWMSS unit.
 *
 * @param pwmss_id The PWMSS module which will have the eQEP function disabled.
 * @return RTEMS_SUCCESSFUL if ok, RTEMS_INVALID_ID if an invalid pwmss_id is
 * supplied.
 */
rtems_status_code beagle_qep_disable(BBB_PWMSS pwmss_id);

/**
 * @brief Configures a given pin for use with the eQEP function of the supplied
 * PWMSS module.
 *
 * @param pin_no The P9 or P8 header pin to be configured for the eQEP function.
 * @param pwmss_id The PWMSS module which will have the eQEP function enabled.
 * @param pullup_enable If true then the internal pull up resistor on the
 * specified pin will be enabled, if false the pull down will be enabled.
 * @return RTEMS_SUCCESSFUL if ok, RTEMS_INVALID_ID if an invalid pwmss_id is
 * supplied.
 */
rtems_status_code beagle_qep_pinmux_setup(
  bbb_qep_pin pin_no,
  BBB_PWMSS pwmss_id,
  bool pullup_enable
);

/**
 * @brief Returns the current position value of the eQEP function for the
 * specified PWMSS module.
 *
 * @param pwmss_id Identifies which PWMSS module to return the eQEP position for
 * @return int32_t The current position value.
 */
int32_t beagle_qep_get_position(BBB_PWMSS pwmss_id);

/**
 * @brief Sets the initial position value of the eQEP function for the
 * specified PWMSS module.
 *
 * @param pwmss_id Identifies which PWMSS module to set the eQEP position for
 * @param position The value to initialise the position register with.
 * @return RTEMS_SUCCESSFUL if ok, RTEMS_INVALID_ID if an invalid pwmss_id is
 * supplied.
 */
rtems_status_code beagle_qep_set_position(
    BBB_PWMSS pwmss_id,
    uint32_t position
);

/**
 * @brief Sets the count mode for the eQEP module.
 * @param pwmss_id Identifies which PWMSS module to set the eQEP count mode for.
 * @param mode One of the above modes to configure the eQEP module for.
 * @return RTEMS_SUCCESSFUL if ok, RTEMS_INVALID_ID if an invalid pwmss_id is
 * supplied.
 */
rtems_status_code beagle_qep_set_count_mode(
  BBB_PWMSS pwmss_id,
  BBB_QEP_COUNT_MODE mode
);

/**
 * @brief Gets the currently configured count mode for the eQEP module.
 * @param pwmss_id Identifies which PWMSS module to set the eQEP count mode for.
 * @return An enum value representing the current count mode.
 */
BBB_QEP_COUNT_MODE beagle_qep_get_count_mode(BBB_PWMSS pwmss_id);

/**
 * @brief Returns the currently configured quadrature mode - either absolute,
 * or relative.
 * @param pwmss_id Identifies which PWMSS module to get the eQEP quadrature
 * mode for.
 * @return BBB_QEP_QUADRATURE_MODE The currently configured quadrature mode.
 */
BBB_QEP_QUADRATURE_MODE beagle_qep_get_quadrature_mode(BBB_PWMSS pwmss_id);

/**
 * @brief Sets the quadrature mode to either absolute or relative.
 * @param pwmss_id Identifies which PWMSS module to set the eQEP quadrature
 * mode for.
 * @param mode BBB_QEP_QUADRATURE_MODE Set the mode of the eQEP to either
 * absolute or relative.
 * @return RTEMS_SUCCESSFUL if ok, RTEMS_INVALID_ID if an invalid pwmss_id is
 * supplied.
 */
rtems_status_code beagle_qep_set_quadrature_mode(
    BBB_PWMSS pwmss_id,
    BBB_QEP_QUADRATURE_MODE mode
);

/**
 * @brief Returns the the currently configured unit timer period.
 * @param pwmss_id Identifies which PWMSS module to get the eQEP timer value for
 * @return uint32_t The current unit timer value in nanoseconds
 */
uint32_t beagle_eqep_get_timer_period(BBB_PWMSS pwmss_id);

/**
 * @brief Sets the unit timer period for the eQEP module.
 * 0 = off, greater than zero sets the period.
 * @param pwmss_id Identifies which PWMSS module to set the eQEP unit timer for.
 * @param period The value in nanoseconds to set the unit timer period to.
 * @param timer_callback This is the user provided callback function that will
 * be called by the interrupt event handler on expiry of the unit timer. The
 * user can provide NULL if they don't require a call back.
 * @param user This is a pointer to a user provided data structure that will be
 * handed back as an argument to the timer callback. The driver does not touch
 * this value.
 * @return RTEMS_SUCCESSFUL if ok, RTEMS_INVALID_ID if an invalid pwmss_id is
 * supplied.
 */
rtems_status_code beagle_eqep_set_timer_period(
    BBB_PWMSS pwmss_id,
    uint64_t period,
    bbb_eqep_timer_callback timer_callback,
    void* user
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_BEAGLE_QEP_H */
