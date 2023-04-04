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

#include <bsp/tms570.h>
#include <bsp/tms570-pinmux.h>
#include <bsp/irq.h>

RTEMS_STATIC_ASSERT(
  TMS570_PIN_CLEAR_RQ_MASK == TMS570_PIN_FNC_CLEAR << TMS570_PIN_FNC_SHIFT,
  TMS570_PIN_CONFIG
);

static inline void
tms570_bsp_pin_to_pinmmrx(volatile uint32_t **pinmmrx, uint32_t *pin_shift,
                          uint32_t config)
{
  uint32_t pin_num = (config & TMS570_PIN_NUM_MASK) >> TMS570_PIN_NUM_SHIFT;
  *pinmmrx = &TMS570_IOMM.PINMUX.PINMMR0 + (pin_num >> 2);
  *pin_shift = (pin_num & 0x3)*8;
}

/**
 * @brief select desired function of pin/ball
 *
 * The function setups multiplexer to interconnect pin with
 * specified function/peripheral. Pin number is index into pinmux
 * entries array. Predefined values for pins are in a format
 * TMS570_BALL_ \c column \c row (for example \c TMS570_BALL_N19).
 * The multiplexer allows to interconnect one pin to multiple
 * signal sources/sinks in the theory but it is usually bad choice.
 * The function sets only specified function and clears all other
 * connections.
 *
 * @param[in] pin_num  pin/ball identifier (index into pinmux array),
 * @param[in] pin_fnc  function number 0 .. 7, if value \c TMS570_PIN_FNC_AUTO
 *                     is specified then pin function is extracted from
 *                     pin_num argument
 * @retval Void
 */
void
tms570_bsp_pin_set_function(int pin_num, int pin_fnc)
{
  unsigned int pin_shift;
  volatile uint32_t *pinmmrx;

  if ( pin_fnc == TMS570_PIN_FNC_AUTO ) {
    pin_fnc = (pin_num & TMS570_PIN_FNC_MASK) >> TMS570_PIN_FNC_SHIFT;
  }
  tms570_bsp_pin_to_pinmmrx(&pinmmrx, &pin_shift, pin_num);
  *pinmmrx = (*pinmmrx & ~(0xff << pin_shift)) | (1 << (pin_fnc + pin_shift));
}

/**
 * @brief clear connection between pin and specified peripherals/function
 *
 * This function switches off given connection and leaves rest
 * of multiplexer setup intact.
 *
 * @param[in] pin_num  pin/ball identifier (index into pinmux array)
 * @param[in] pin_fnc  function number 0 .. 7, if value \c TMS570_PIN_FNC_AUTO
 *                     is specified then pin function is extracted from
 *                     pin_num argument
 * @retval Void
 */
void
tms570_bsp_pin_clear_function(int pin_num, int pin_fnc)
{
  unsigned int pin_shift;
  volatile uint32_t *pinmmrx;

  if ( pin_fnc == TMS570_PIN_FNC_AUTO ) {
    pin_fnc = (pin_num & TMS570_PIN_FNC_MASK) >> TMS570_PIN_FNC_SHIFT;
  }
  tms570_bsp_pin_to_pinmmrx(&pinmmrx, &pin_shift, pin_num);
  *pinmmrx = *pinmmrx & ~(1 << (pin_fnc+pin_shift));
}

/**
 * @brief configure one pin according to its function specification
 *
 * The function setups multiplexer to interconnect pin with
 * specified function/peripheral. Predefined values for pins combined with
 * function are in a format TMS570_BALL_ \c column \c row \c function
 * (for example \c TMS570_BALL_W3_SCIRX).
 * If the function can be connected to more pins then specification
 * includes infomation which allows to disconnect alternative pin to peripheral
 * input connection or switch input multiplexer to right pin.
 *
 * @param[in] pin_num_and_fnc pin function descriptor is build by macro
 *               \c TMS570_PIN_AND_FNC which takes pin/pinmmr specification
 *               build by \c TMS570_BALL_WITH_MMR and function index in output
 *               multiplexer. If the peripheral can be connected to other input
 *               alternative then actual pin description and alternative to
 *               disconnected/reconnect are combined together by
 *               \c TMS570_PIN_WITH_IN_ALT macro. If clear of alternative
 *               connection is required then flag \c TMS570_PIN_CLEAR_RQ_MASK
 *               is ored to alternative description.
 *
 * @retval Void
 */
void
tms570_bsp_pin_config_one(uint32_t pin_num_and_fnc)
{
  rtems_interrupt_level intlev;

  rtems_interrupt_disable(intlev);
  tms570_pin_config_prepare();
  tms570_pin_config_apply(pin_num_and_fnc);
  tms570_pin_config_complete();
  rtems_interrupt_enable(intlev);
}

/**
 * @brief configure block or whole pin multiplexer
 *
 * Function change multiplexer content. It is intended for initial
 * chip setup and does not use locking. If complete reconfiguration
 * is required at runtime then it is application responsibility
 * to protect and serialize change with peripherals drivers
 * and parallel calls
 *
 * @param[in] pinmmr_values pointer to array with required multiplexer setup
 * @param[in] reg_start starting register, this allows to configure non-consecutive
 *                      registers groups found on some MCU family members
 * @param[in] reg_count number of words in initialization array to set
 *                      to corresponding registers
 *
 * @retval Void
 */
void
tms570_bsp_pinmmr_config(const uint32_t *pinmmr_values, int reg_start, int reg_count)
{
  volatile uint32_t *pinmmrx;
  const uint32_t *pval;
  int cnt;

  if ( reg_count <= 0)
    return;

  tms570_pin_config_prepare();

  pinmmrx = (&TMS570_IOMM.PINMUX.PINMMR0) + reg_start;
  pval = pinmmr_values;
  cnt = reg_count;

  do {
    *pinmmrx = *pinmmrx & *pval;
    pinmmrx++;
    pval++;
  } while( --cnt );

  pinmmrx = (&TMS570_IOMM.PINMUX.PINMMR0) + reg_start;
  pval = pinmmr_values;
  cnt = reg_count;

  do {
    *pinmmrx = *pval;
    pinmmrx++;
    pval++;
  } while( --cnt );

  tms570_pin_config_complete();
}

void tms570_pin_config_prepare(void)
{
  TMS570_IOMM.KICK_REG0 = 0x83E70B13U;
  TMS570_IOMM.KICK_REG1 = 0x95A4F1E0U;
}

static void
tms570_pin_set_function(uint32_t config)
{
  volatile uint32_t *pinmmrx;
  uint32_t pin_shift;
  uint32_t pin_fnc;
  uint32_t bit;
  uint32_t val;

  tms570_bsp_pin_to_pinmmrx(&pinmmrx, &pin_shift, config);
  pin_fnc = (config & TMS570_PIN_FNC_MASK) >> TMS570_PIN_FNC_SHIFT;
  bit = 1U << (pin_fnc + pin_shift);
  val = *pinmmrx;
  val &= ~(0xffU << pin_shift);

  if ((config & TMS570_PIN_CLEAR_RQ_MASK) == 0) {
    val |= bit;
  }

  *pinmmrx = val;
}

void tms570_pin_config_apply(uint32_t config)
{
  uint32_t pin_in_alt;
  uint32_t pin_num_and_fnc;

  pin_in_alt = config & TMS570_PIN_IN_ALT_MASK;
  if (pin_in_alt != 0) {
    pin_in_alt >>= TMS570_PIN_IN_ALT_SHIFT;
    tms570_pin_set_function(pin_in_alt);
  }

  pin_num_and_fnc = config & TMS570_PIN_NUM_FNC_MASK;
  tms570_pin_set_function(pin_num_and_fnc);
}

void tms570_pin_config_array_apply(const uint32_t *config, size_t count)
{
  size_t i;

  for (i = 0; i < count; ++i) {
    tms570_pin_config_apply(config[i]);
  }
}

void tms570_pin_config_complete(void)
{
  TMS570_IOMM.KICK_REG0 = 0;
  TMS570_IOMM.KICK_REG1 = 0;
}
