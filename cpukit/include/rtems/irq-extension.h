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
 * Copyright (C) 2008, 2019 embedded brains GmbH
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
 * @retval RTEMS_SUCCESSFUL Successful operation.
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
 * @retval RTEMS_SUCCESSFUL Successful operation.
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
 * @retval RTEMS_SUCCESSFUL Successful operation.
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
 * @brief Sets the processor affinity set of an interrupt vector.
 *
 * @param[in] vector The interrupt vector number.
 * @param[in] affinity_size The storage size of the affinity set.
 * @param[in] affinity_set The new processor affinity set for the interrupt
 *   vector.  This pointer must not be @c NULL.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID The vector number is invalid.
 * @retval RTEMS_INVALID_SIZE Invalid affinity set size.
 * @retval RTEMS_INVALID_NUMBER Invalid processor affinity set.
 */
rtems_status_code rtems_interrupt_set_affinity(
  rtems_vector_number  vector,
  size_t               affinity_size,
  const cpu_set_t     *affinity
);

/**
 * @brief Gets the processor affinity set of an interrupt vector.
 *
 * @param[in] vector The interrupt vector number.
 * @param[in] affinity_size The storage size of the affinity set.
 * @param[out] affinity_set The current processor affinity set for the
 *   interrupt vector.  This pointer must not be @c NULL.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID The vector number is invalid.
 * @retval RTEMS_INVALID_SIZE Invalid affinity set size.
 */
rtems_status_code rtems_interrupt_get_affinity(
  rtems_vector_number  vector,
  size_t               affinity_size,
  cpu_set_t           *affinity
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
 * @brief The interrupt server index of the default interrupt server.
 */
#define RTEMS_INTERRUPT_SERVER_DEFAULT 0

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
  void                          *server;
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
 *   rtems_interrupt_server_request_set_vector(),
 *   rtems_interrupt_server_request_submit(), and
 *   rtems_interrupt_server_request_destroy().
 */
typedef struct {
  rtems_interrupt_server_entry  entry;
  rtems_interrupt_server_action action;
} rtems_interrupt_server_request;

/**
 * @brief Initializes the interrupt server tasks.
 *
 * This function tries to create an interrupt server task for each processor in
 * the system.  The tasks will have the priority @a priority, the stack size @a
 * stack_size, the modes @a modes and the attributes @a attributes.  The count
 * of server tasks will be returned in @a server_count.  Interrupt handlers can
 * be installed on an interrupt server with
 * rtems_interrupt_server_handler_install() and removed with
 * rtems_interrupt_server_handler_remove() using a server index.  In case of an
 * interrupt, the request will be forwarded to the interrupt server.  The
 * handlers are executed within the interrupt server context.  If one handler
 * blocks on something this may delay the processing of other handlers.
 *
 * The server count pointer @a server_count may be @a NULL.
 *
 * This function may block.
 *
 * @see rtems_task_create().
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INCORRECT_STATE The interrupt servers are not initialized.
 * @retval RTEMS_NO_MEMORY Not enough memory.
 * @retval RTEMS_TOO_MANY No free task available to create at least one server task.
 * @retval RTEMS_UNSATISFIED Task stack size too large.
 * @retval RTEMS_INVALID_PRIORITY Invalid task priority.
 */
rtems_status_code rtems_interrupt_server_initialize(
  rtems_task_priority priority,
  size_t stack_size,
  rtems_mode modes,
  rtems_attribute attributes,
  uint32_t *server_count
);

/**
 * @brief Installs the interrupt handler routine @a handler for the interrupt
 * vector with number @a vector on the server @a server.
 *
 * The handler routine will be executed on the corresponding interrupt server
 * task.  A server index @a server_index of @c RTEMS_INTERRUPT_SERVER_DEFAULT
 * may be used to install the handler on the default server.
 *
 * This function may block.
 *
 * @see rtems_interrupt_handler_install().
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INCORRECT_STATE The interrupt servers are not initialized.
 * @retval RTEMS_INVALID_ID If the interrupt server index is invalid.
 * @retval * For other errors see rtems_interrupt_handler_install().
 */
rtems_status_code rtems_interrupt_server_handler_install(
  uint32_t server_index,
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
 * A server index @a server_index of @c RTEMS_INTERRUPT_SERVER_DEFAULT may be
 * used to remove the handler from the default server.
 *
 * This function may block.
 *
 * @see rtems_interrupt_handler_remove().
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INCORRECT_STATE The interrupt servers are not initialized.
 * @retval RTEMS_INVALID_ID If the interrupt server index is invalid.
 * @retval * For other errors see rtems_interrupt_handler_remove().
 */
rtems_status_code rtems_interrupt_server_handler_remove(
  uint32_t server_index,
  rtems_vector_number vector,
  rtems_interrupt_handler handler,
  void *arg
);

/**
 * @brief Iterates over all interrupt handler of the interrupt vector with
 * number @a vector which are installed on the interrupt server specified by
 * @a server.
 *
 * A server index @a server_index of @c RTEMS_INTERRUPT_SERVER_DEFAULT may be
 * used to specify the default server.
 *
 * @see rtems_interrupt_handler_iterate()
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INCORRECT_STATE The interrupt servers are not initialized.
 * @retval RTEMS_INVALID_ID If the interrupt server index is invalid.
 * @retval * For other errors see rtems_interrupt_handler_iterate().
 */
rtems_status_code rtems_interrupt_server_handler_iterate(
  uint32_t server_index,
  rtems_vector_number vector,
  rtems_interrupt_per_handler_routine routine,
  void *arg
);

/**
 * @brief Moves the interrupt handlers installed on the specified source
 * interrupt server to the destination interrupt server.
 *
 * This function must be called from thread context.  It may block.  Calling
 * this function within the context of an interrupt server is undefined
 * behaviour.
 *
 * @param[in] source_server_index The source interrupt server index.  Use
 *   @c RTEMS_INTERRUPT_SERVER_DEFAULT to specify the default server.
 * @param[in] vector The interrupt vector number.
 * @param[in] destination_server_index The destination interrupt server index.
 *   Use @c RTEMS_INTERRUPT_SERVER_DEFAULT to specify the default server.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation
 * @retval RTEMS_INCORRECT_STATE The interrupt servers are not initialized.
 * @retval RTEMS_INVALID_ID The destination interrupt server index is invalid.
 * @retval RTEMS_INVALID_ID The vector number is invalid.
 * @retval RTEMS_INVALID_ID The destination interrupt server index is invalid.
 */
rtems_status_code rtems_interrupt_server_move(
  uint32_t            source_server_index,
  rtems_vector_number vector,
  uint32_t            destination_server_index
);

/**
 * @brief Suspends the specified interrupt server.
 *
 * A suspend request is sent to the specified interrupt server.  This function
 * waits for an acknowledgment from the specified interrupt server.
 *
 * This function must be called from thread context.  It may block.  Calling
 * this function within the context of an interrupt server is undefined
 * behaviour.
 *
 * @param[in] server_index The interrupt server index.  Use
 *   @c RTEMS_INTERRUPT_SERVER_DEFAULT to specify the default server.
 *
 * @see rtems_interrupt_server_resume().
 *
 * @retval RTEMS_SUCCESSFUL Successful operation
 * @retval RTEMS_INCORRECT_STATE The interrupt servers are not initialized.
 * @retval RTEMS_INVALID_ID If the interrupt server index is invalid.
 */
rtems_status_code rtems_interrupt_server_suspend( uint32_t server_index );

/**
 * @brief Resumes the specified interrupt server.
 *
 * This function must be called from thread context.  It may block.  Calling
 * this function within the context of an interrupt server is undefined
 * behaviour.
 *
 * @param[in] server_index The interrupt server index.  Use
 *   @c RTEMS_INTERRUPT_SERVER_DEFAULT to specify the default server.
 *
 * @see rtems_interrupt_server_suspend().
 *
 * @retval RTEMS_SUCCESSFUL Successful operation
 * @retval RTEMS_INCORRECT_STATE The interrupt servers are not initialized.
 * @retval RTEMS_INVALID_ID If the interrupt server index is invalid.
 */
rtems_status_code rtems_interrupt_server_resume( uint32_t server_index );

/**
 * @brief Sets the processor affinity of the specified interrupt server.
 *
 * The scheduler is set determined by the highest numbered processor in the
 * specified affinity set.
 *
 * This operation is only reliable in case the specified interrupt was
 * suspended via rtems_interrupt_server_suspend().
 *
 * @param[in] server_index The interrupt server index.  Use
 *   @c RTEMS_INTERRUPT_SERVER_DEFAULT to specify the default server.
 * @param[in] affinity_size The storage size of the affinity set.
 * @param[in] affinity The desired processor affinity set for the specified
 *   interrupt server.
 * @param[in] priority The task priority with respect to the corresponding
 *   scheduler instance.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation
 * @retval RTEMS_INCORRECT_STATE The interrupt servers are not initialized.
 * @retval RTEMS_INVALID_ID If the interrupt server index is invalid.
 * @retval RTEMS_INVALID_SIZE Invalid affinity set size.
 * @retval RTEMS_INVALID_NAME The affinity set contains no online processor.
 * @retval RTEMS_INCORRECT_STATE The highest numbered online processor in the
 *   specified affinity set is not owned by a scheduler.
 * @retval RTEMS_INVALID_PRIORITY Invalid priority.
 * @retval RTEMS_RESOURCE_IN_USE The interrupt server owns resources which deny
 *   a scheduler change.
 * @retval RTEMS_INVALID_NUMBER Invalid processor affinity set.
 */
rtems_status_code rtems_interrupt_server_set_affinity(
  uint32_t            server_index,
  size_t              affinity_size,
  const cpu_set_t    *affinity,
  rtems_task_priority priority
);

/**
 * @brief Initializes the specified interrupt server entry.
 *
 * @param[in] server_index The interrupt server index.  Use
 *   @c RTEMS_INTERRUPT_SERVER_DEFAULT to specify the default server.
 * @param[in] entry The interrupt server entry to initialize.
 *
 * @see rtems_interrupt_server_action_prepend().
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INCORRECT_STATE The interrupt servers are not initialized.
 * @retval RTEMS_INVALID_ID If the interrupt server index is invalid.
 */
rtems_status_code rtems_interrupt_server_entry_initialize(
  uint32_t                      server_index,
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
 * @param[in] entry The interrupt server entry must be initialized before the
 *   first call to this function via rtems_interrupt_server_entry_initialize()
 *   and rtems_interrupt_server_action_prepend().  The entry and its actions
 *   must not be modified between calls to this function.  Use
 *   rtems_interrupt_server_entry_destroy() to destroy an entry in use.
 */
void rtems_interrupt_server_entry_submit(
  rtems_interrupt_server_entry *entry
);

/**
 * @brief Moves the interrupt server entry to the specified destination
 * interrupt server.
 *
 * Calling this function concurrently with rtems_interrupt_server_entry_submit()
 * with the same entry or while the entry is enqueued on the previous interrupt
 * server is undefined behaviour.
 *
 * @param[in,out] entry The interrupt server entry.  It must have be initialized
 *   before the call to this function.
 * @param destination_server_index The destination interrupt server index.
 *   Use @c RTEMS_INTERRUPT_SERVER_DEFAULT to specify the default server.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation
 * @retval RTEMS_INCORRECT_STATE The interrupt servers are not initialized.
 * @retval RTEMS_INVALID_ID The destination interrupt server index is invalid.
 */
rtems_status_code rtems_interrupt_server_entry_move(
  rtems_interrupt_server_entry *entry,
  uint32_t                      destination_server_index
);

/**
 * @brief Destroys the specified interrupt server entry.
 *
 * This function must be called from thread context.  It may block.  Calling
 * this function within the context of an interrupt server is undefined
 * behaviour.  No error checking is performed.
 *
 * @param[in] server_index The interrupt server index.  Use
 *   @c RTEMS_INTERRUPT_SERVER_DEFAULT to specify the default server.
 * @param[in] entry The interrupt server entry to destroy.  It must have been
 *   initialized via rtems_interrupt_server_entry_initialize().
 */
void rtems_interrupt_server_entry_destroy(
  rtems_interrupt_server_entry *entry
);

/**
 * @brief Initializes the specified interrupt server request.
 *
 * @param[in] server_index The interrupt server index.  Use
 *   @c RTEMS_INTERRUPT_SERVER_DEFAULT to specify the default server.
 * @param[in] request The interrupt server request to initialize.
 * @param[in] handler The interrupt handler for the request action.
 * @param[in] arg The interrupt handler argument for the request action.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INCORRECT_STATE The interrupt servers are not initialized.
 * @retval RTEMS_INVALID_ID If the interrupt server index is invalid.
 *
 * @see rtems_interrupt_server_request_set_vector().
 */
rtems_status_code rtems_interrupt_server_request_initialize(
  uint32_t                        server_index,
  rtems_interrupt_server_request *request,
  rtems_interrupt_handler         handler,
  void                           *arg
);

/**
 * @brief Sets the interrupt vector in the specified interrupt server request.
 *
 * By default, the interrupt vector of an interrupt server request is set to a
 * special value which is outside the range of vectors supported by the
 * interrupt controller hardware.
 *
 * Calls to rtems_interrupt_server_request_submit() will disable the interrupt
 * vector of the request.  After processing of the request by the interrupt
 * server the interrupt vector will be enabled again.
 *
 * @param[in] request The initialized interrupt server request.
 * @param[in] vector The interrupt vector number.
 *
 * @see rtems_interrupt_server_request_initialize().
 */
RTEMS_INLINE_ROUTINE void rtems_interrupt_server_request_set_vector(
  rtems_interrupt_server_request *request,
  rtems_vector_number             vector
)
{
  request->entry.vector = vector;
}

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
 * @param[in] request The interrupt server request must be initialized before the
 *   first call to this function via
 *   rtems_interrupt_server_request_initialize().  The request must not be
 *   modified between calls to this function.  Use
 *   rtems_interrupt_server_request_destroy() to destroy a request in use.
 */
RTEMS_INLINE_ROUTINE void rtems_interrupt_server_request_submit(
  rtems_interrupt_server_request *request
)
{
  rtems_interrupt_server_entry_submit( &request->entry );
}

/**
 * @brief Destroys the specified interrupt server request.
 *
 * This function must be called from thread context.  It may block.  Calling
 * this function within the context of an interrupt server is undefined
 * behaviour.  No error checking is performed.
 *
 * @param[in] request The interrupt server request to destroy.  It must have
 *   been initialized via rtems_interrupt_server_request_initialize().
 */
RTEMS_INLINE_ROUTINE void rtems_interrupt_server_request_destroy(
  rtems_interrupt_server_request *request
)
{
  rtems_interrupt_server_entry_destroy( &request->entry );
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTEMS_IRQ_EXTENSION_H */
