/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief Input/output module methods definitions.
 */

/*
 * Copyright (c) 2014 Taller Technologies.
 *
 * @author  Boretto Martin    (martin.boretto@tallertechnologies.com)
 * @author  Diaz Marcos (marcos.diaz@tallertechnologies.com)
 * @author  Lenarduzzi Federico  (federico.lenarduzzi@tallertechnologies.com)
 * @author  Daniel Chicco  (daniel.chicco@tallertechnologies.com)
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_LPC176X_IO_H
#define LIBBSP_ARM_LPC176X_IO_H

#include <assert.h>
#include <rtems.h>
#include <bsp/io-defs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Sets pin to the selected function.
 *
 * @param pin The pin to set.
 * @param function Defines the function to set.
 */
void lpc176x_pin_select(
  uint32_t             pin,
  lpc176x_pin_function function
);

/**
 * @brief Sets pin to the selected mode.
 *
 * @param pin The pin to set.
 * @param mode Defines the mode to set.
 */
void lpc176x_pin_set_mode(
  const uint32_t             pin,
  const lpc176x_pin_mode mode
);

/**
 * @brief Enables the module power and clock.
 *
 * @param  module Represents the module to be enabled.
 * @param  clock  Represents the clock to set for this module.
 * @return RTEMS_SUCCESFULL if the module was enabled succesfully.
 */
rtems_status_code lpc176x_module_enable(
  lpc176x_module       module,
  lpc176x_module_clock clock
);

/**
 * @brief Checks if the current module is turned off and disables a module.
 *
 * @param  module Represents the module to be disabled.
 * @return RTEMS_SUCCESFULL if the module was disabled succesfully.
 */
rtems_status_code lpc176x_module_disable( lpc176x_module module );

/**
 * @brief Checks if the current module is enabled or not.
 *
 * @param  module Represents the module to be checked.
 * @return TRUE if the module is enabled.
 *         FALSE otherwise.
 */
bool lpc176x_module_is_enabled( lpc176x_module module );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC176X_IO_H */
