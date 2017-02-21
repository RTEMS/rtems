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
 * Copyright (c) 2008, 2017 embedded brains GmbH.
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

#ifndef RTEMS_IRQ_EXTENSION_H
#define RTEMS_IRQ_EXTENSION_H

#include <rtems.h>
#include <rtems/chain.h>

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
 */
/**@{**/

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
 * @brief Forces that this interrupt handler replaces the first handler with
 * the same argument.
 */
#define RTEMS_INTERRUPT_REPLACE ((rtems_option) 0x00000002)

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
 * @brief Returns true if the interrupt handler replace option is set.
 */
#define RTEMS_INTERRUPT_IS_REPLACE( options) \
  ((options) & RTEMS_INTERRUPT_REPLACE)

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
 * - @ref RTEMS_INTERRUPT_REPLACE
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
 * If the option @ref RTEMS_INTERRUPT_REPLACE is set then it shall be ensured
 * that this handler will replace the first handler with the same argument for
 * this vector if it exists, otherwise an error status shall be returned.  A
 * second handler with the same argument for this vector shall remain
 * unchanged.  The new handler will inherit the unique or shared option from
 * the replaced handler.
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
 * @retval RTEMS_UNSATISFIED If no handler exists to replace with the specified
 * argument and vector this shall be returned.
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
 * @brief An interrupt server action.
 *
 * This structure must be treated as an opaque data type.  Members must not be
 * accessed directly.
 *
 * @see rtems_interrupt_server_action_prepend().
 */
typedef struct rtems_interrupt_server_action {
  struct rtems_interrupt_server_action *next;
  rtems_interrupt_handler               handler;
  void                                 *arg;
} rtems_interrupt_server_action;

/**
 * @brief An interrupt server entry.
 *
 * This structure must be treated as an opaque data type.  Members must not be
 * accessed directly.
 *
 * @see rtems_interrupt_server_entry_initialize(),
 *   rtems_interrupt_server_action_prepend(),
 *   rtems_interrupt_server_entry_submit(), and
 *   rtems_interrupt_server_entry_destroy().
 */
typedef struct {
  rtems_chain_node               node;
  rtems_vector_number            vector;
  rtems_interrupt_server_action *actions;
} rtems_interrupt_server_entry;

/**
 * @brief An interrupt server request.
 *
 * This structure must be treated as an opaque data type.  Members must not be
 * accessed directly.
 *
 * @see rtems_interrupt_server_request_initialize(),
 *   rtems_interrupt_server_request_submit(), and
 *   rtems_interrupt_server_request_destroy().
 */
typedef struct {
  rtems_interrupt_server_entry  entry;
  rtems_interrupt_server_action action;
} rtems_interrupt_server_request;

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
 * @retval RTEMS_TOO_MANY No free task available to create the server task.
 * @retval RTEMS_UNSATISFIED Task stack size too large.
 * @retval RTEMS_INVALID_PRIORITY Invalid task priority.
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

/**
 * @brief Initializes the specified interrupt server entry.
 *
 * @param[in] entry The interrupt server entry to initialize.
 *
 * @see rtems_interrupt_server_action_prepend().
 */
void rtems_interrupt_server_entry_initialize(
  rtems_interrupt_server_entry *entry
);

/**
 * @brief Prepends the specified interrupt server action to the list of actions
 * of the specified interrupt server entry.
 *
 * No error checking is performed.
 *
 * @param[in] entry The interrupt server entry to prepend the interrupt server
 *   action.  It must have been initialized via
 *   rtems_interrupt_server_entry_initialize().
 * @param[in] action The interrupt server action to prepend the list of actions
 *   of the entry.
 * @param[in] handler The interrupt handler for the action.
 * @param[in] arg The interrupt handler argument for the action.
 */
void rtems_interrupt_server_action_prepend(
  rtems_interrupt_server_entry  *entry,
  rtems_interrupt_server_action *action,
  rtems_interrupt_handler        handler,
  void                          *arg
);

/**
 * @brief Submits the specified interrupt server entry so that its interrupt
 * server actions can be invoked by the specified interrupt server.
 *
 * This function may be used to do a two-step interrupt processing.  The first
 * step is done in interrupt context which calls this function.  The second
 * step is then done in the context of the interrupt server.
 *
 * This function may be called from thread or interrupt context.  It does not
 * block.  No error checking is performed.
 *
 * @param[in] server The server identifier.  Use @c RTEMS_ID_NONE to specify
 *   the default server.
 * @param[in] entry The interrupt server entry must be initialized before the
 *   first call to this function via rtems_interrupt_server_entry_initialize()
 *   and rtems_interrupt_server_action_prepend().  The entry and its actions
 *   must not be modified between calls to this function.  Use
 *   rtems_interrupt_server_entry_destroy() to destroy an entry in use.
 */
void rtems_interrupt_server_entry_submit(
  rtems_id                      server,
  rtems_interrupt_server_entry *entry
);

/**
 * @brief Destroys the specified interrupt server entry.
 *
 * This function must be called from thread context.  It may block.  No error
 * checking is performed.
 *
 * @param[in] server The server identifier.  Use @c RTEMS_ID_NONE to specify
 *   the default server.
 * @param[in] entry The interrupt server entry to destroy.  It must have been
 *   initialized via rtems_interrupt_server_entry_initialize().
 */
void rtems_interrupt_server_entry_destroy(
  rtems_id                      server,
  rtems_interrupt_server_entry *entry
);

/**
 * @brief Initializes the specified interrupt server request.
 *
 * No error checking is performed.
 *
 * @param[in] request The interrupt server request to initialize.
 * @param[in] handler The interrupt handler for the request action.
 * @param[in] arg The interrupt handler argument for the request action.
 */
void rtems_interrupt_server_request_initialize(
  rtems_interrupt_server_request *request,
  rtems_interrupt_handler         handler,
  void                           *arg
);

/**
 * @brief Submits the specified interrupt server request so that its interrupt
 * server action can be invoked by the specified interrupt server.
 *
 * This function may be used to do a two-step interrupt processing.  The first
 * step is done in interrupt context which calls this function.  The second
 * step is then done in the context of the interrupt server.
 *
 * This function may be called from thread or interrupt context.  It does not
 * block.  No error checking is performed.
 *
 * @param[in] server The server identifier.  Use @c RTEMS_ID_NONE to specify
 *   the default server.
 * @param[in] request The interrupt server request must be initialized before the
 *   first call to this function via
 *   rtems_interrupt_server_request_initialize().  The request must not be
 *   modified between calls to this function.  Use
 *   rtems_interrupt_server_request_destroy() to destroy a request in use.
 */
RTEMS_INLINE_ROUTINE void rtems_interrupt_server_request_submit(
  rtems_id                        server,
  rtems_interrupt_server_request *request
)
{
  rtems_interrupt_server_entry_submit( server, &request->entry );
}

/**
 * @brief Destroys the specified interrupt server request.
 *
 * This function must be called from thread context.  It may block.  No error
 * checking is performed.
 *
 * @param[in] server The server identifier.  Use @c RTEMS_ID_NONE to specify
 *   the default server.
 * @param[in] request The interrupt server request to destroy.  It must have
 *   been initialized via rtems_interrupt_server_request_initialize().
 */
RTEMS_INLINE_ROUTINE void rtems_interrupt_server_request_destroy(
  rtems_id                        server,
  rtems_interrupt_server_request *request
)
{
  rtems_interrupt_server_entry_destroy( server, &request->entry );
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTEMS_IRQ_EXTENSION_H */
