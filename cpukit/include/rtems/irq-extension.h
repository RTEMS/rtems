/**
 * @file
 *
 * @ingroup rtems_interrupt_extension
 *
 * @brief Header file for the Interrupt Manager Extension.
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
 * @defgroup rtems_interrupt_extension Interrupt Manager Extension
 *
 * @ingroup ClassicINTR
 *
 * In addition to the Classic API interrupt handler with a handle are
 * supported.  You can also install multiple shared handler for one interrupt
 * vector.
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

/**
 * @brief Interrupt handler routine type.
 */
typedef void (*rtems_interrupt_handler)(void *);

/**
 * @brief Installs the interrupt handler routine @a handler for the interrupt
 * vector with number @a vector.
 *
 * You can set one of the mutually exclusive options
 *
 * - @ref RTEMS_INTERRUPT_UNIQUE
 * - @ref RTEMS_INTERRUPT_SHARED
 *
 * with the @a options parameter for the interrupt handler.
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
 * This function may block.
 *
 * @retval RTEMS_SUCCESSFUL Shall be returned in case of success.
 * @retval RTEMS_CALLED_FROM_ISR If this function is called from interrupt
 * context this shall be returned.
 * @retval RTEMS_INVALID_ADDRESS If the handler address is NULL this shall be
 * returned.
 * @retval RTEMS_INVALID_ID If the vector number is out of range this shall be
 * returned.
 * @retval RTEMS_INVALID_NUMBER If an option is not applicable this shall be
 * returned.
 * @retval RTEMS_RESOURCE_IN_USE If the vector is already occupied with a
 * unique handler this shall be returned.  If a unique handler should be
 * installed and there is already a handler installed this shall be returned.
 * @retval RTEMS_TOO_MANY If a handler with this argument is already installed
 * for the vector this shall be returned.
 * @retval RTEMS_IO_ERROR Reserved for board support package specific error
 * conditions.
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
 * This function may block.
 *
 * @retval RTEMS_SUCCESSFUL Shall be returned in case of success.
 * @retval RTEMS_CALLED_FROM_ISR If this function is called from interrupt
 * context this shall be returned.
 * @retval RTEMS_INVALID_ADDRESS If the handler address is NULL this shall be
 * returned.
 * @retval RTEMS_INVALID_ID If the vector number is out of range this shall be
 * returned.
 * @retval RTEMS_UNSATISFIED If the handler with its argument is not installed
 * for the vector this shall be returned.
 * @retval RTEMS_IO_ERROR Reserved for board support package specific error
 * conditions.
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
 * This function may block.  Never install or remove an interrupt handler
 * within the iteration routine.  This may result in a deadlock.
 *
 * @retval RTEMS_SUCCESSFUL Shall be returned in case of success.
 * @retval RTEMS_CALLED_FROM_ISR If this function is called from interrupt
 * context this shall be returned.
 * @retval RTEMS_INVALID_ID If the vector number is out of range this shall be
 * returned.
 * @retval RTEMS_IO_ERROR Reserved for board support package specific error
 * conditions.
 */
rtems_status_code rtems_interrupt_handler_iterate(
  rtems_vector_number vector,
  rtems_interrupt_per_handler_routine routine,
  void *arg
);

/**
 * @brief Initializes an interrupt server task.
 *
 * The task will have the priority @a priority, the stack size @a stack_size,
 * the modes @a modes and the attributes @a attributes.  The identifier of the
 * server task will be returned in @a server.  Interrupt handlers can be
 * installed on the server with rtems_interrupt_server_handler_install() and
 * removed with rtems_interrupt_server_handler_remove() using this identifier.
 * In case of an interrupt the request will be forwarded to the server.  The
 * handlers are executed within the server context.  If one handler blocks on
 * something this may delay the processing of other handlers.
 *
 * The server identifier pointer @a server may be @a NULL to initialize the
 * default server.
 *
 * This function may block.
 *
 * @see rtems_task_create().
 *
 * @retval RTEMS_SUCCESSFUL Shall be returned in case of success.
 * @retval RTEMS_INCORRECT_STATE If the default server is already initialized
 * this shall be returned.
 * @retval RTEMS_IO_ERROR Reserved for board support package specific error
 * conditions.
 */
rtems_status_code rtems_interrupt_server_initialize(
  rtems_task_priority priority,
  size_t stack_size,
  rtems_mode modes,
  rtems_attribute attributes,
  rtems_id *server
);

/**
 * @brief Installs the interrupt handler routine @a handler for the interrupt
 * vector with number @a vector on the server @a server.
 *
 * The handler routine will be executed on the corresponding interrupt server
 * task.  A server identifier @a server of @c RTEMS_ID_NONE may be used to
 * install the handler on the default server.
 *
 * This function may block.
 *
 * @see rtems_interrupt_handler_install().
 *
 * @retval RTEMS_SUCCESSFUL Shall be returned in case of success.
 * @retval RTEMS_INCORRECT_STATE If the interrupt handler server is not
 * initialized this shall be returned.
 * @retval * For other errors see rtems_interrupt_handler_install().
 */
rtems_status_code rtems_interrupt_server_handler_install(
  rtems_id server,
  rtems_vector_number vector,
  const char *info,
  rtems_option options,
  rtems_interrupt_handler handler,
  void *arg
);

/**
 * @brief Removes the interrupt handler routine @a handler with argument @a arg
 * for the interrupt vector with number @a vector from the server @a server.
 *
 * A server identifier @a server of @c RTEMS_ID_NONE may be used to remove the
 * handler from the default server.
 *
 * This function may block.
 *
 * @see rtems_interrupt_handler_remove().
 *
 * @retval RTEMS_SUCCESSFUL Shall be returned in case of success.
 * @retval RTEMS_INCORRECT_STATE If the interrupt handler server is not
 * initialized this shall be returned.
 * @retval * For other errors see rtems_interrupt_handler_remove().
 */
rtems_status_code rtems_interrupt_server_handler_remove(
  rtems_id server,
  rtems_vector_number vector,
  rtems_interrupt_handler handler,
  void *arg
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTEMS_IRQ_EXTENSION_H */
