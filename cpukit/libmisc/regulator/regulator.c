/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Regulator Library Implementation
 */

/*
 * Copyright (C) 2022 On-Line Applications Research Corporation (OAR)
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

#include <stdlib.h>

#include <rtems.h>
#include <rtems/regulator.h>
#include <string.h>

#include <rtems/regulatorimpl.h>

/**
 * @ingroup RegulatorInternalAPI
 *
 * This method is the body for the task which delivers the output for
 * this regulator instance at the configured rate.
 *
 * @param[in] arg points to the regulator instance this thread
 *                is associated with
 *
 * @note The argument passed in cannot be NULL if the
 *       rtems_regulator_create worked.
 */
static rtems_task _Regulator_Output_task_body(
  rtems_task_argument arg
)
{
  _Regulator_Control   *the_regulator = (_Regulator_Control *)arg;
  rtems_status_code     sc;
  size_t                to_dequeue;
  _Regulator_Message_t  regulator_message;
  size_t                regulator_message_size;
  bool                  release_it;

  the_regulator->delivery_thread_is_running   = true;

  /**
   * This thread uses a rate monotonic period object instance. A rate
   * monotonic period object must be created by the thread using it.
   * It can be deleted by any thread which simplifies clean up.
   *
   * The rate_monotonic_create() call can fail if the application
   * is incorrectly configured. This thread has no way to report the
   * failure. If it continues with an invalid id, then the thread will
   * not block on the period and spin continuously consuming CPU. The only
   * alternatives are to invoke rtems_fatal_error_occurred() or silently
   * exit the thread.
   */
  sc = rtems_rate_monotonic_create(
    rtems_build_name('P', 'E', 'R', 'D'),
    &the_regulator->delivery_thread_period_id
  );
  if (sc != RTEMS_SUCCESSFUL) {
    goto exit_delivery_thread;
  }

  /**
   * Loop on the rate_monotonic_period() based on the specified period.
   */
  while (1) {
    sc = rtems_rate_monotonic_period(
      the_regulator->delivery_thread_period_id,
      the_regulator->Attributes.delivery_thread_period
    );
     _Assert_Unused_variable_equals(sc, RTEMS_SUCCESSFUL);

    /**
     * If the delivery thread has been requested to exit, then
     * quit processing messages, break out of this loop, and exit
     * this thread.
     */
    if (the_regulator->delivery_thread_request_exit) {
      break;
    }

    /**
     * Loop for the configured number of messages to deliver per period.
     * If we reach the point, there are no more messages, block for the
     * rest of this period. If there are messages, deliver them.
     */
    for (to_dequeue = 0;
         to_dequeue < the_regulator->Attributes.maximum_to_dequeue_per_period;
         to_dequeue++) {
      regulator_message_size = sizeof(_Regulator_Message_t);
      sc = rtems_message_queue_receive(
        the_regulator->queue_id,
        &regulator_message,
        &regulator_message_size,
        RTEMS_NO_WAIT,
        0
      );
     _Assert_Unused_variable_equals(sc, RTEMS_SUCCESSFUL);
      if (sc != RTEMS_SUCCESSFUL) {
        break;
      }

      release_it = the_regulator->Attributes.deliverer(
        the_regulator->Attributes.deliverer_context,
        regulator_message.buffer,
        regulator_message.length
      );

      the_regulator->Statistics.delivered++;

      /**
       * The message was successfully delivered. If the delivery function
       * wants the buffer returned, do it now. The delivery to the Destination
       * may involve handing the buffer off to something like DMA
       * and need to wait for it to complete before releasing the buffer.
       *
       * Note that this is the underlying RTEMS service
       * used by @a rtems_regulator_obtain_buffer() and @a
       * rtems_regulator_release_buffer().
       */
      if (release_it == true) {
        the_regulator->Statistics.released++;
        sc = rtems_partition_return_buffer(
          the_regulator->messages_partition_id,
          regulator_message.buffer
        );
        _Assert_Unused_variable_equals(sc, RTEMS_SUCCESSFUL);
      }
    }
  }

  /**
   * This thread was requested to exit. Do so.
   */
exit_delivery_thread:
  the_regulator->delivery_thread_is_running   = false;
  the_regulator->delivery_thread_has_exited   = true;

  (void) rtems_rate_monotonic_delete(the_regulator->delivery_thread_period_id);

  rtems_task_exit();
}

/**
 * @ingroup RegulatorInternalAPI
 *
 * This method frees the resources associated with a regulator instance.
 * The resources are freed in the opposite of the order in which they are
 * allocated. This is used on error cases in @a rtems_regulator_create() and in
 * @a rtems_regulator_delete().
 *
 * @param[in] the_regulator is the instance to operate upon
 * @param[in] ticks is the length of time to wait for the delivery thread
 *            to exit
 *
 * @return This method returns true is successful and false on timeout.
 */
static bool _Regulator_Free_helper(
  _Regulator_Control *the_regulator,
  rtems_interval      ticks
)
{
  rtems_status_code  sc;


  /*
   * If the output thread has not started running, then we can just delete it.
   */

  if (ticks == 0 || the_regulator->delivery_thread_is_running == false) {
    sc = rtems_task_delete(the_regulator->delivery_thread_id);
    _Assert_Unused_variable_equals(sc, RTEMS_SUCCESSFUL);
  } else {
    rtems_interval remaining = ticks;

    the_regulator->delivery_thread_request_exit = true;

    while (1) {
      if (the_regulator->delivery_thread_has_exited) {
        break;
      }
    
      if (remaining == 0) {
        return false;
      }

      (void) rtems_task_wake_after(1);
      remaining--;
    }
  }

  /*
   * The output thread deletes the rate monotonic period that it created.
   */

  /*
   * The regulator's message_queue_storage is implicitly freed by this call.
   */
  sc = rtems_message_queue_delete(the_regulator->queue_id);
  _Assert_Unused_variable_equals(sc, RTEMS_SUCCESSFUL);

  sc = rtems_partition_delete(the_regulator->messages_partition_id);
  _Assert_Unused_variable_equals(sc, RTEMS_SUCCESSFUL);

  if (the_regulator->message_memory) {
    free(the_regulator->message_memory);
  }

  the_regulator->initialized = 0;
  free(the_regulator);
  return true;
}

/**
 * @ingroup RegulatorInternalAPI
 */
rtems_status_code rtems_regulator_create(
  rtems_regulator_attributes  *attributes,
  rtems_regulator_instance   **regulator
)
{
  _Regulator_Control *the_regulator;
  rtems_status_code  sc;
  size_t             alloc_size;

  /**
   * Perform basic validation of parameters
   */
  if (attributes == NULL) {
    return RTEMS_INVALID_ADDRESS;
  }

  if (regulator == NULL) {
    return RTEMS_INVALID_ADDRESS;
  }

  /**
   * Verify attributes are OK. Some are checked by calls to object create
   * methods. Specifically the following are not checked:
   *
   * - delivery_thread_priority by rtems_task_create()
   * - delivery_thread_stack_size can be any value
   */
  if (attributes->deliverer == NULL) {
    return RTEMS_INVALID_ADDRESS;
  }

  if (attributes->maximum_messages == 0) {
    return RTEMS_INVALID_NUMBER;
  }

  if (attributes->maximum_message_size == 0) {
    return RTEMS_INVALID_SIZE;
  }

  if (attributes->maximum_to_dequeue_per_period == 0) {
    return RTEMS_INVALID_NUMBER;
  }

  if (attributes->delivery_thread_period == 0) {
    return RTEMS_INVALID_NUMBER;
  }

  /**
   * Allocate memory for regulator instance
   */
  the_regulator = (_Regulator_Control *) calloc(sizeof(_Regulator_Control), 1);
  if (the_regulator == NULL) {
    return RTEMS_NO_MEMORY;
  }

  /**
   * We do NOT want the delivery_thread_id field to be initialized to 0. If the
   * @a rtems_task_create() fails, then the field will not be overwritten.
   * This results in an attempt to rtems_task_delete(0) during clean
   * up. The thread ID of 0 is self which results in the calling thread
   * accidentally deleting itself.
   */
  the_regulator->delivery_thread_id = (rtems_id) -1;

  /**
   * Copy the attributes to an internal area for later use
   */
  the_regulator->Attributes = *attributes;

  /**
   * Allocate memory for the messages. There is no need to zero out the
   * message memory because the user should fill that in.
   */
  alloc_size = attributes->maximum_message_size * attributes->maximum_messages;
  the_regulator->message_memory = calloc(alloc_size, 1);
  if (the_regulator->message_memory == NULL) {
    _Regulator_Free_helper(the_regulator, 0);
    return RTEMS_NO_MEMORY;
  }

  /**
   * Associate message memory with a partition so allocations are atomic
   */
  sc = rtems_partition_create(
    rtems_build_name('P', 'O', 'O', 'L'),
    the_regulator->message_memory,
    alloc_size,
    attributes->maximum_message_size,
    RTEMS_DEFAULT_ATTRIBUTES,
    &the_regulator->messages_partition_id
  );
  if (sc != RTEMS_SUCCESSFUL) {
    _Regulator_Free_helper(the_regulator, 0);
    return sc;
  }

  /**
   * Create the message queue between the sender and output thread
   */
  RTEMS_MESSAGE_QUEUE_BUFFER(sizeof(_Regulator_Message_t)) regulator_message_t;

  size_t storage_size = sizeof(regulator_message_t) * attributes->maximum_messages;

  the_regulator->message_queue_storage = malloc(storage_size);
  if (the_regulator->message_queue_storage == NULL) {
    _Regulator_Free_helper(the_regulator, 0);
    return RTEMS_NO_MEMORY;
  }

  rtems_message_queue_config mq_config = {
    .name = rtems_build_name('S', 'N', 'D', 'Q'),
    .maximum_pending_messages =  attributes->maximum_messages,
    .maximum_message_size = sizeof(_Regulator_Message_t),
    .storage_area = the_regulator->message_queue_storage,
    .storage_size = storage_size,
    .storage_free = free,
    .attributes = RTEMS_DEFAULT_ATTRIBUTES
  };
  sc = rtems_message_queue_construct(
    &mq_config,
    &the_regulator->queue_id
  );
  if (sc != RTEMS_SUCCESSFUL) {
    _Regulator_Free_helper(the_regulator, 0);
    return sc;
  }

  /**
   * @note A rate monotonic period object must be created by the thread
   *       using it. Thus that specific create operation is not included
   *       in this method. All other resources are allocated here.
   */

  /**
   * Create the output thread Using the priority and stack size attributes
   * specified by the user.
   */
  sc = rtems_task_create(
    rtems_build_name('R', 'E', 'G', 'U'),
    attributes->delivery_thread_priority,
    attributes->delivery_thread_stack_size,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &the_regulator->delivery_thread_id
  );
  if (sc != RTEMS_SUCCESSFUL) {
    _Regulator_Free_helper(the_regulator, 0);
    return sc;
  }

  /**
   * Start the output thread.
   *
   * @note There should be no way this call can fail. The task id is valid,
   *       the regulator output thread entry point is valid, and the argument
   *       is valid.
   */
  the_regulator->delivery_thread_is_running   = true;
  the_regulator->delivery_thread_request_exit = false;
  the_regulator->delivery_thread_has_exited   = false;

  sc = rtems_task_start(
    the_regulator->delivery_thread_id,
    _Regulator_Output_task_body,
    (rtems_task_argument) the_regulator
  );
 _Assert_Unused_variable_equals(sc, RTEMS_SUCCESSFUL);

  /**
   * The regulator is successfully initialized. Set the initialized field
   * to reflect this and return the instance pointer.
   */
  the_regulator->initialized = REGULATOR_INITIALIZED;

  *regulator = (void *)the_regulator;

  return RTEMS_SUCCESSFUL;
}

/**
 * @brief Validate the regulator instance provided by the user
 *
 * Validate the regulator instance provided by the user
 *
 * @param[in] regulator is the instance provided by the user
 * @param[inout] status will contain the RTEMS status for this check
 *
 * @return This method returns a @a _Regulator_Control instance pointer
 *         which is NULL if invalid or points to the internal regulator
 *         control structure if valid.
 */
static inline _Regulator_Control *_Regulator_Get(
  rtems_regulator_instance *regulator,
  rtems_status_code        *status
)
{
  _Regulator_Control *the_regulator = (_Regulator_Control *) regulator;

  if (the_regulator == NULL) {
    *status = RTEMS_INVALID_ADDRESS;
    return NULL;
  }

  if (the_regulator->initialized != REGULATOR_INITIALIZED) {
    *status = RTEMS_INCORRECT_STATE;
    return NULL;
  }

  status = RTEMS_SUCCESSFUL;
  return the_regulator;
}

/**
 * @ingroup RegulatorInternalAPI
 */
rtems_status_code rtems_regulator_delete(
  rtems_regulator_instance    *regulator,
  rtems_interval               ticks
)
{
  _Regulator_Control *the_regulator;
  rtems_status_code   status;

  /**
   * Convert external handle to internal instance pointer
   */
  the_regulator = _Regulator_Get(regulator, &status);
  if (the_regulator == NULL) {
    return status;
  }

  /**
   * There can be no buffers outstanding
   */
  _Regulator_Statistics *stats = &the_regulator->Statistics;

  if (stats->obtained != stats->released ) {
    return RTEMS_RESOURCE_IN_USE;
  }

  /**
   * Free the resources associated with this regulator instance.
   */
  bool bc;
  bc = _Regulator_Free_helper(the_regulator, ticks);
  if (bc == false) {
    return RTEMS_TIMEOUT;
  }

  return RTEMS_SUCCESSFUL;
}

/**
 * @ingroup RegulatorInternalAPI
 *
 * Allocate a buffer for the caller using the internal partition.
 */
rtems_status_code rtems_regulator_obtain_buffer(
  rtems_regulator_instance    *regulator,
  void                       **buffer
)
{
  _Regulator_Control *the_regulator;
  rtems_status_code   status;

  /**
   * Convert external handle to internal instance pointer
   */
  the_regulator = _Regulator_Get(regulator, &status);
  if (the_regulator == NULL) {
    return status;
  }

  /**
   * Allocate a buffer for the user application from the buffer pool managed
   * by an Classic API partition.
   */
  status = rtems_partition_get_buffer(
    the_regulator->messages_partition_id,
    buffer
  );

  if (status == RTEMS_SUCCESSFUL) {
    the_regulator->Statistics.obtained++;
  }

  return status;
}

/**
 * @ingroup RegulatorInternalAPI
 *
 * Allocate a buffer for the caller using the internal partition.
 */
rtems_status_code rtems_regulator_release_buffer(
  rtems_regulator_instance  *regulator,
  void                      *buffer
)
{
  _Regulator_Control *the_regulator;
  rtems_status_code   status;

  /**
   * Convert external handle to internal instance pointer
   */
  the_regulator = _Regulator_Get(regulator, &status);
  if (the_regulator == NULL) {
    return status;
  }

  /**
   * Deallocate the buffer to the buffer pool managed by a Classic
   * API partition.
   */
  status = rtems_partition_return_buffer(
    the_regulator->messages_partition_id,
    buffer
  );

  if (status == RTEMS_SUCCESSFUL) {
    the_regulator->Statistics.released++;
  }

  return status;
}

/**
 * @ingroup RegulatorInternalAPI
 */
rtems_status_code rtems_regulator_send(
  rtems_regulator_instance *regulator,
  void                     *message,
  size_t                    length
)
{
  _Regulator_Control   *the_regulator;
  rtems_status_code     status;
  _Regulator_Message_t  regulator_message;

  the_regulator = (_Regulator_Control *) regulator;

  /**
   * Validate the arguments and ensure the regulator was successfully
   * initialized.
   */
  if (message == NULL) {
    return RTEMS_INVALID_ADDRESS;
  }

  if (length == 0) {
    return RTEMS_INVALID_NUMBER;
  }

  /**
   * Convert external handle to internal instance pointer
   */
  the_regulator = _Regulator_Get(regulator, &status);
  if (the_regulator == NULL) {
    return status;
  }

  /**
   * Place the message pointer and length into a temporary structure. This
   * lets the implementation internally send the message by reference and
   * have a zero-copy implementation.
   */
  regulator_message.buffer = message;
  regulator_message.length = length;

  /**
   * Send the application message to the output thread for delivery using
   * a Classic API message queue.
   */
  status = rtems_message_queue_send(
    the_regulator->queue_id,
    &regulator_message,
    sizeof(_Regulator_Message_t)
  );
  if (status != RTEMS_SUCCESSFUL) {
    return status;
  }

  return status;
}

/**
 * @ingroup RegulatorInternalAPI
 */
rtems_status_code rtems_regulator_get_statistics(
  rtems_regulator_instance   *regulator,
  rtems_regulator_statistics *statistics
)
{
  _Regulator_Control   *the_regulator;
  rtems_status_code     status;

  /**
   * Validate the arguments and ensure the regulator was successfully
   * initialized.
   */
  if (statistics == NULL) {
    return RTEMS_INVALID_ADDRESS;
  }

  /**
   * Convert external handle to internal instance pointer
   */
  the_regulator = _Regulator_Get(regulator, &status);
  if (the_regulator == NULL) {
    return status;
  }

  /**
   * Zero out the statistics structure in case the get period statistics
   * fails below.
   */
  memset(statistics, 0, sizeof(rtems_regulator_statistics)); 

  /**
   * Fill in the caller's statistics structure from information
   * maintained by the regulator instance about buffers processed.
   */
  statistics->obtained  = the_regulator->Statistics.obtained;
  statistics->released  = the_regulator->Statistics.released;
  statistics->delivered = the_regulator->Statistics.delivered;

  /**
   * Attempt to retrieve the delivery thread's period's statistics.
   *
   * NOTE; If the Delivery Thread has not run yet, the period will not
   *       exist yet. We should not fail for this reason but it is why
   *       we zeroed out the entire structure above.
   */
  (void) rtems_rate_monotonic_get_statistics(
    the_regulator->delivery_thread_period_id,
    &statistics->period_statistics
  );

  return RTEMS_SUCCESSFUL;
}
