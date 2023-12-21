/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This header file provides interfaces of the I/O Multiplexing Module
 *   (IOMM) support.
 */

/*
 * Copyright (C) 2015 Premysl Houdek <kom541000@gmail.com>
 *
 * Google Summer of Code 2014 at
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
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

#ifndef LIBBSP_ARM_TMS570_PINMUX_H
#define LIBBSP_ARM_TMS570_PINMUX_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define TMS570_PIN_NUM_SHIFT    0
#define TMS570_PIN_NUM_MASK     0x000007ff

/*
 * Request clear of interconnection in setup
 * to ensure that previous peripheral to pin
 * connection is not enabled in parallel to other one.
 * Mask is ored with pin number in such list.
 */
#define TMS570_PIN_CLEAR_RQ_MASK 0x00008000

#define TMS570_PIN_FNC_SHIFT    11
#define TMS570_PIN_FNC_MASK     0x00007800

/**
 * @brief This constant indicates that all eight function bits associated with
 *   the pin shall be cleared.
 *
 * Use it as a special value for the pin function in TMS570_PIN_AND_FNC().
 */
#define TMS570_PIN_FNC_CLEAR 0x10U

#define TMS570_PIN_NUM_FNC_MASK 0x0000ffff

#define TMS570_PIN_IN_ALT_SHIFT 16
#define TMS570_PIN_IN_ALT_MASK  0xffff0000

#define TMS570_PIN_FNC_AUTO  (-1)

/**
 * @brief Defines the function of the pin.
 *
 * @param pin is the pin identifier.  Use TMS570_BALL_WITH_MMR() to define the
 *   pin identifier.
 *
 * param fnc is the pin function.  The pin function shall be the function bit
 *   index or TMS570_PIN_FNC_CLEAR.
 */
#define TMS570_PIN_AND_FNC(pin, fnc) \
  ((pin) | ((fnc) << TMS570_PIN_FNC_SHIFT))

#define TMS570_PIN_WITH_IN_ALT(pin_num_and_fnc, pin_in_alt_num_and_fnc) \
  ((pin_num_and_fnc) | ((pin_in_alt_num_and_fnc) << TMS570_PIN_IN_ALT_SHIFT))

#define TMS570_BALL_WITH_MMR(mmrx, pos) \
  ((pos) | ((mmrx) << 2))

/**
 * @brief Prepares a pin configuration sequence.
 *
 * Use tms570_pin_config_apply() to apply pin configurations.  Use
 * tms570_pin_config_complete() to complete the pin configuration sequence.
 */
void tms570_pin_config_prepare(void);

/**
 * @brief Applies a pin configuration.
 *
 * This function can only be used if the pin configuration was prepared by
 * tms570_pin_config_prepare().
 *
 * @param config is the pin configuration defined by TMS570_PIN_AND_FNC() or
 *   TMS570_PIN_WITH_IN_ALT().
 */
void tms570_pin_config_apply(uint32_t config);

/**
 * @brief Applies a pin configuration array.
 *
 * This function can only be used if the pin configuration was prepared by
 * tms570_pin_config_prepare().
 *
 * @param config is the pin configuration array.  Calls
 *   tms570_pin_config_apply() for each pin configuration in the array.
 *
 * @param count is the element count of the pin configuration array.
 */
void tms570_pin_config_array_apply(const uint32_t *config, size_t count);

/**
 * @brief Completes a pin configuration sequence.
 */
void tms570_pin_config_complete(void);

/* Generic functions select pin to peripheral connection */

void tms570_bsp_pin_set_function(int pin_num, int pin_fnc);

void tms570_bsp_pin_clear_function(int pin_num, int pin_fnc);

void tms570_bsp_pin_config_one(uint32_t pin_num_and_fnc);

void tms570_bsp_pinmmr_config(const uint32_t *pinmmr_values, int reg_start, int reg_count);

#define TMS570_PINMMR_REG_SINGLE_VAL_ACTION(reg, pin) \
  (((((pin) & TMS570_PIN_NUM_MASK) >> 2 != (reg)) || ((pin) & TMS570_PIN_CLEAR_RQ_MASK))? 0: \
   1 << ((((pin) & TMS570_PIN_FNC_MASK) >> TMS570_PIN_FNC_SHIFT) + \
   ((pin) & 3) * 8) \
  )

#define TMS570_PINMMR_REG_VAL_ACTION(reg, pin) \
  TMS570_PINMMR_REG_SINGLE_VAL_ACTION(reg, pin) | \
  ((pin) & TMS570_PIN_IN_ALT_MASK? \
  TMS570_PINMMR_REG_SINGLE_VAL_ACTION(reg, (pin) >> TMS570_PIN_IN_ALT_SHIFT ): \
  0) |

/**
 * Macro which computes value for PINMMRx register from pin list
 * which is defined as macro calling action macro for each pin
 *
 * @param reg      PINMMR register number (0 .. 30 for TMS570LS3137)
 * @param pin_list declared as macro with parameters
 *                 \c per_pin_action and \c common_arg which expands
 *                 to list of \c per_pin_action(\c common_arg, \c TMS570_BALL_xx_function)
 *
 * @retval number which represents connections which should be enabled
 *                in given PINMMR register. Pin setup for other registers than specified
 *                are ignored
 */
#define TMS570_PINMMR_REG_VAL(reg, pin_list) \
  pin_list(TMS570_PINMMR_REG_VAL_ACTION, reg) 0

#define TMS570_PINMMR_COMA_LIST_ACTION(reg, pin) \
  (pin),

/**
 * Macro which generates list of pin and function specification from
 * from pin list which is defined as macro calling action macro for each pin
 *
 * @param pin_list declared as macro with parameters
 *                 \c per_pin_action and \c common_arg which expands
 *                 to list of \c per_pin_action(\c common_arg, \c TMS570_BALL_xx_function)
 *
 * @retval list of coma separated pin+function combined values which is terminated by coma
 *              at the end
 */
#define TMS570_PINMMR_COMA_LIST(pin_list) \
  pin_list(TMS570_PINMMR_COMA_LIST_ACTION, 0)

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_TMS570_IRQ_H */
