/**
 * @file
 *
 * @ingroup rtems_interrupt_extension
 *
 * @brief Header file for the RTEMS Interrupt Manager Extension.
 */

/*
 * Based on concepts of Pavel Pisa, Till Straumann and Eric Valette.
 *
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#ifndef RTEMS_IRQ_EXTENSION_H
#define RTEMS_IRQ_EXTENSION_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup rtems_interrupt_extension RTEMS Interrupt Manager Extension
 *
 * @ingroup ClassicINTR
 *
 * In addition to the classic API interrupt handler with a handle are
 * supported.  You can also install multiple shared handler for one interrupt
 * vector.
 *
 * @{
 */

/**
 * @brief Interrupt handler routine type.
 */
typedef void (*rtems_interrupt_handler)( rtems_vector_number, void *);

/**
 * @brief Installs the interrupt handler routine @a handler for the interrupt
 * vector with number @a vector.
 *
 * You can set some @ref rtems_interrupt_extension_options "options" with @a
 * options for the interrupt handler.
 *
 * The handler routine shall be called with argument @a arg when dispatched.
 * The order in which the shared interrupt handlers are dispatched for one
 * vector is BSP dependent.
 *
 * If the option @ref RTEMS_INTERRUPT_UNIQUE is set then it shall be ensured
 * that this handler will be the only one for this vector.
 *
 * You can provide an informative description @a info.  This may be used for
 * system debugging and status tools.  The string has to be persistent during
 * the handler life time.
 *
 * @note This function may block.
 *
 * @return
 * - On success RTEMS_SUCCESSFUL shall be returned.
 * - If the vector is already occupied with a unique handler the
 * RTEMS_RESOURCE_IN_USE status code shall be returned.
 * - If you want to install a unique handler and there is already a handler
 * installed RTEMS_RESOURCE_IN_USE shall be returned.
 * - If this function is called within interrupt context RTEMS_CALLED_FROM_ISR
 * shall be returned.
 * - If the vector number is out of range RTEMS_INVALID_NUMBER shall be
 * returned.
 * - If the handler address is NULL a RTEMS_INVALID_ADDRESS shall be returned.
 * - If a handler with this argument is already installed for this vector
 * RTEMS_TOO_MANY shall be returned.
 * - Other error states are BSP specific.
 */
rtems_status_code rtems_interrupt_handler_install(
  rtems_vector_number vector,
  const char *info,
  rtems_option options,
  rtems_interrupt_handler handler,
  void *arg
);

/**
 * @brief Removes the interrupt handler routine @a handler with argument @a arg
 * for the interrupt vector with number @a vector.
 *
 * @note This function may block.
 *
 * @return
 * - On success RTEMS_SUCCESSFUL shall be returned.
 * - If this function is called within interrupt context RTEMS_CALLED_FROM_ISR
 * shall be returned.
 * - If the vector number is out of range RTEMS_INVALID_NUMBER shall be
 * returned.
 * - If the handler address is NULL a RTEMS_INVALID_ADDRESS shall be returned.
 * - If the handler with this argument is not installed for this vector
 * RTEMS_UNSATISFIED shall be returned.
 * - Other error states are BSP specific.
 */
rtems_status_code rtems_interrupt_handler_remove(
  rtems_vector_number vector,
  rtems_interrupt_handler handler,
  void *arg
);

/**
 * @brief Interrupt handler iteration routine type.
 *
 * @see rtems_interrupt_handler_iterate()
 */
typedef void (*rtems_interrupt_per_handler_routine)(
  void *, const char *, rtems_option, rtems_interrupt_handler, void *
);

/**
 * @brief Iterates over all installed interrupt handler of the interrupt vector
 * with number @a vector.
 *
 * For each installed handler of the vector the function @a routine will be
 * called with the supplied argument @a arg and the handler information,
 * options, routine and argument.
 *
 * This function is intended for system information and diagnostics.
 *
 * @note This function may block.  Never install or remove an interrupt handler
 * within the iteration routine.  This may result in a deadlock.
 *
 * @return
 * - On success RTEMS_SUCCESSFUL shall be returned.
 * - If this function is called within interrupt context RTEMS_CALLED_FROM_ISR
 * shall be returned.
 * - If the vector number is out of range RTEMS_INVALID_NUMBER shall be
 * returned.
 * - Other error states are BSP specific.
 */
rtems_status_code rtems_interrupt_handler_iterate(
  rtems_vector_number vector,
  rtems_interrupt_per_handler_routine routine,
  void *arg
);

/** @} */

/**
 * @defgroup rtems_interrupt_extension_options Interrupt Handler Options
 *
 * @ingroup rtems_interrupt_extension
 *
 * @{
 */

/**
 * @name Options
 *
 * @{
 */

/**
 * @brief Makes the interrupt handler unique.  Prevents other handler from
 * using the same interrupt vector.
 */
#define RTEMS_INTERRUPT_UNIQUE ((rtems_option) 0x00000001)

/**
 * @brief Allows that this interrupt handler may share a common interrupt
 * vector with other handler.
 */
#define RTEMS_INTERRUPT_SHARED ((rtems_option) 0x00000000)

/** @} */

/**
 * @name Option Set Checks
 *
 * @{
 */

/**
 * @brief Returns true if the interrupt handler unique option is set.
 */
#define RTEMS_INTERRUPT_IS_UNIQUE( options) \
  ((options) & RTEMS_INTERRUPT_UNIQUE)

/**
 * @brief Returns true if the interrupt handler shared option is set.
 */
#define RTEMS_INTERRUPT_IS_SHARED( options) \
  (!RTEMS_INTERRUPT_IS_UNIQUE( options))

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTEMS_IRQ_EXTENSION_H */
