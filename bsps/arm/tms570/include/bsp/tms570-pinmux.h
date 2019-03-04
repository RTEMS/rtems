/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief I/O Multiplexing Module (IOMM) basic support
 */

/*
 * Copyright (c) 2015 Premysl Houdek <kom541000@gmail.com>
 *
 * Google Summer of Code 2014 at
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_TMS570_PINMUX_H
#define LIBBSP_ARM_TMS570_PINMUX_H

#ifndef ASM
#include <bsp/tms570.h>

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
#define TMS570_PIN_CLEAR_RQ_MASK 0x00000800

#define TMS570_PIN_FNC_SHIFT    12
#define TMS570_PIN_FNC_MASK     0x0000f000

#define TMS570_PIN_NUM_FNC_MASK 0x0000ffff

#define TMS570_PIN_IN_ALT_SHIFT 16
#define TMS570_PIN_IN_ALT_MASK  0xffff0000

#define TMS570_PIN_FNC_AUTO  (-1)

#define TMS570_PIN_AND_FNC(pin, fnc) \
  ((pin) | ((fnc) << TMS570_PIN_FNC_SHIFT))

#define TMS570_PIN_WITH_IN_ALT(pin_num_and_fnc, pin_in_alt_num_and_fnc) \
  ((pin_num_and_fnc) | ((pin_in_alt_num_and_fnc) << TMS570_PIN_IN_ALT_SHIFT))

#define TMS570_BALL_WITH_MMR(mmrx, pos) \
  ((pos) | ((mmrx) << 2))

/* Generic functions select pin to peripheral connection */

void tms570_bsp_pin_set_function(int pin_num, int pin_fnc);

void tms570_bsp_pin_clear_function(int pin_num, int pin_fnc);

void tms570_bsp_pin_config_one(uint32_t pin_num_and_fnc);

void tms570_bsp_pinmmr_config(const uint32_t *pinmmr_values, int reg_start, int reg_count);

static inline void
tms570_bsp_pin_to_pinmmrx(volatile uint32_t **pinmmrx, unsigned int *pin_shift,
                          int pin_num)
{
  pin_num = (pin_num & TMS570_PIN_NUM_MASK) >> TMS570_PIN_NUM_SHIFT;
  *pinmmrx = &TMS570_IOMM.PINMUX.PINMMR0 + (pin_num >> 2);
  *pin_shift = (pin_num & 0x3)*8;
}

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


#endif

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_TMS570_IRQ_H */
