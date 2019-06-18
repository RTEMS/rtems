/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_ATSAM_POWER_H
#define LIBBSP_ARM_ATSAM_POWER_H

#include <sys/types.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

/**
 * @brief Status of the Low Power Support
 */
typedef enum {
  /**
   * @brief Used for Initialization of Handlers
   */
  ATSAM_POWER_INIT,
  /**
   * @brief Used for Switching On of Handlers
   */
  ATSAM_POWER_ON,
  /**
   * @brief Used for Switching Off of Handlers
   */
  ATSAM_POWER_OFF
} atsam_power_state;

/**
 * @brief Control structure for power control handling
 */
typedef struct atsam_power_control {
  /**
   * @brief Data pointer to the handler with its desired state
   */
  void (*handler)(
      const struct atsam_power_control *control,
      atsam_power_state state
  );
  /**
   * @brief Data chunk that is used by the handler
   */
  union {
    void *arg;
    struct {
      uint8_t first;
      uint8_t last;
    } peripherals;
  } data;
} atsam_power_control;

/**
 * @brief Performs a power state change according to the state parameter.
 *
 * The handlers of the control table are invoked in forward order (invocation
 * starts with table index zero) for the ATSAM_POWER_INIT and ATSAM_POWER_OFF
 * states, otherwise the handlers are invoked in reverse order (invocation
 * starts with the last table index).
 *
 * @param controls Table with power controls.
 * @param n Count of power control table entries.
 * @param state The desired power state.
 *
 * @code
 * #include <rtems.h>
 * #include <pthread.h>
 *
 * #include <bsp/power.h>
 *
 * static atsam_power_data_rtc_driver rtc_data = { .interval = 5 };
 *
 * static const atsam_power_control power_controls[] = {
 *   ATSAM_POWER_CLOCK_DRIVER,
 *   ATSAM_POWER_RTC_DRIVER(&rtc_data),
 *   ATSAM_POWER_SLEEP_MODE
 * };
 *
 * static pthread_once_t once = PTHREAD_ONCE_INIT;
 *
 * static void init(void)
 * {
 *   atsam_power_change_state(
 *     &power_controls[0],
 *     RTEMS_ARRAY_SIZE(power_controls),
 *     ATSAM_POWER_INIT
 *   );
 * }
 *
 * void power_init(void)
 * {
 *   pthread_once(&once, init);
 * }
 *
 * void low_power(void)
 * {
 *   atsam_power_change_state(
 *     &power_controls[0],
 *     RTEMS_ARRAY_SIZE(power_controls),
 *     ATSAM_POWER_OFF
 *   );
 *   atsam_power_change_state(
 *     &power_controls[0],
 *     RTEMS_ARRAY_SIZE(power_controls),
 *     ATSAM_POWER_ON
 *   );
 * }
 * @end
 */
void atsam_power_change_state(
  const atsam_power_control *controls,
  size_t n,
  atsam_power_state state
);

/**
 * @brief Power handler for a set of peripherals according to the specified
 * peripheral indices.
 *
 * For the power off state, the peripherals are enabled in the PMC.
 *
 * For the power on state, the peripherals are disabled in the Power Management
 * Controller (PMC).
 *
 * @see ATSAM_POWER_PERIPHERAL().
 */
void atsam_power_handler_peripheral(
  const atsam_power_control *controls,
  atsam_power_state state
);

/**
 * @brief Power handler for the clock driver.
 *
 * For the power off state, the system tick is disabled.
 *
 * For the power on state, the system tick is enabled.  In case no clock driver
 * is used by the application, then this may lead to a spurious interrupt
 * resulting in a fatal error.
 *
 * @see ATSAM_POWER_CLOCK_DRIVER().
 */
void atsam_power_handler_clock_driver(
  const atsam_power_control *controls,
  atsam_power_state state
);

/**
 * @brief Power handler for the RTC driver.
 *
 * This handler installs an interrupt handler during power support initialization.
 *
 * For the power off state, the RTC alarm interrupt is set up according to the
 * interval of the corresponding handler data.
 *
 * For the power on state, the RTC alarm interrupt is disabled.
 *
 * @see ATSAM_POWER_RTC_DRIVER().
 */
void atsam_power_handler_rtc_driver(
  const atsam_power_control *controls,
  atsam_power_state state
);

/**
 * @brief Power handler to enter the processor sleep mode.
 *
 * @see ATSAM_POWER_SLEEP_MODE().
 */
void atsam_power_handler_sleep_mode(
  const atsam_power_control *controls,
  atsam_power_state state
);

/**
 * @brief Power handler to enter the processor wait mode.
 *
 * The internal flash is put into deep sleep mode.
 *
 * @see ATSAM_POWER_WAIT_MODE().
 */
void atsam_power_handler_wait_mode(
  const atsam_power_control *controls,
  atsam_power_state state
);

/**
 * @brief Initializer for a peripheral power support.
 *
 * @param f The first peripheral index.
 * @param l The last peripheral index.
 */
#define ATSAM_POWER_PERIPHERAL(f, l) \
 { \
   .handler = atsam_power_handler_peripheral, \
   .data = { .peripherals = { .first = f, .last = l } } \
 }

#define ATSAM_POWER_HANDLER(h, a) \
 { \
   .handler = h, \
   .data = { .arg = a } \
 }

#define ATSAM_POWER_CLOCK_DRIVER \
 { .handler = atsam_power_handler_clock_driver }

#define ATSAM_POWER_SLEEP_MODE \
 { .handler = atsam_power_handler_sleep_mode }

#define ATSAM_POWER_WAIT_MODE \
 { .handler = atsam_power_handler_wait_mode }

/**
 * @brief Data for RTC driver power support.
 *
 * @see ATSAM_POWER_RTC_DRIVER().
 */
typedef struct {
  /**
   * @brief Interval in seconds for which the power off mode should be active.
   *
   * An interval up to 24h is supported.
   */
  uint32_t interval;
} atsam_power_data_rtc_driver;

/**
 * @brief Initializer for RTC driver power support.
 *
 * @param a Pointer to RTC driver power data.
 *
 * @see atsam_power_data_rtc_driver.
 */
#define ATSAM_POWER_RTC_DRIVER(a) \
 { \
    .handler = atsam_power_handler_rtc_driver, \
    .data = { .arg = a } \
 }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_ATSAM_POWER_H */
