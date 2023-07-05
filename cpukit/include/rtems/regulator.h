/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RegulatorAPI
 *
 * @brief This header file defines the Regulator API.
 *
 */

/*
 * Copyright (C) 2023 On-Line Applications Research Corporation (OAR)
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

/**
 * @defgroup RegulatorAPI Regulator API
 *
 * @brief Regulator APIs
 *
 * The Regulator provides a set of APIs to manage input sources which 
 * produces bursts of message traffic.
 *
 * The regulator is designed to sit logically between two entities -- a
 * source and a destination, where it limits the traffic sent to the
 * destination to prevent it from being flooded with messages from the
 * source. This can be used to accommodate bursts of input from a source
 * and meter it out to a destination.  The maximum number of messages
 * which can be buffered in the regulator is specified by the
 * @a maximum_messages field in the @a rtems_regulator_attributes
 * structure passed as an argument to @a rtems_regulator_create().
 *
 * The regulator library accepts an input stream of messages from a
 * source and delivers them to a destination. The regulator assumes that the
 * input stream from the source contains sporadic bursts of data which can
 * exceed the acceptable rate of the destination. By limiting the message rate,
 * the regulator prevents an overflow of messages.
 *
 * The regulator can be configured for the input buffering required to manage
 * the maximum burst and for the metering rate for the output. The output rate
 * is in messages per second. If the sender produces data too fast, the
 * regulator will buffer the configured number of messages.
 *
 * A configuration capability is provided to allow for adaptation to different
 * message streams. The regulator can also support running multiple instances,
 * which could be used on independent message streams.
 *
 * The regulator provides a simple interface to the application for avoiding
 * bursts of input from a fast source overflowing a slower destination.
 *
 * It is assumed that the application has a design limit on the number of
 * messages which may be buffered. All messages accepted by the regulator,
 * assuming no overflow on input, will eventually be output by the Delivery
 * thread.
 *
 * A regulator instance is used as follows from the producer/source side:
 *
 * @code
 *   while (1)
 *     use rtems_regulator_obtain_buffer to obtain a buffer
 *     input operation to fetch data into the buffer
 *     rtems_regulator_send(buffer, size of message)
 * @endcode
 *
 * The delivery of message buffers to the Destination and subsequent
 * release is performed in the context of the delivery thread by either
 * the delivery function or delivery thread. Details are below.
 *
 * The sequence diagram below shows the interaction between a message Source,
 * a Regulator instance, and RTEMS, given the usage described in the above
 * paragraphs.
 *
 * \startuml "Regulator Application Input Source Usage"
 *   Source -> Regulator : rtems_regulator_obtain_buffer(regulator, buffer)
 *   Regulator -> RTEMS : rtems_partition_get_buffer(id, buffer)
 *   RTEMS --> Regulator : rtems_status_code
 *   Regulator --> Source : rtems_status_code
 *   Source -> Regulator : rtems_regulator_send(regulator, message, length)
 *   Regulator -> RTEMS : rtems_message_queue_send(id, message, size)
 *   RTEMS --> Regulator : rtems_status_code
 *   Regulator --> Source : rtems_status_code
 * \enduml
 *
 * As illustrated in the sequence diagram, the Source usually corresponds
 * to application software reading a system input. The Source obtains a
 * buffer from the Regulator instance and fills it with incoming data.
 * The application explicitly obtaining a buffer and filling it in allows
 * for zero copy operations on the Source side.
 *
 * The Source then sends the buffer to the Regulator instance. The Regulator
 * the sends the buffer via a message queue which to the Delivery thread.
 * The Delivery thread executes periodically at a rate specified at
 * Regulation creation. At each period, the Delivery thread attempts to
 * receive up to a configured number of buffers and invoke the Delivery
 * function to deliver them to the Destination.
 *
 * The Delivery function is provided by the application for this
 * specific Regulator instance. Depending on the Destination, it may use
 * a function which copies the buffer contents (e.g., write()) or which
 * operates directly on the buffer contents (e.g. DMA from buffer). In
 * the case of a Destination which copies the buffer contents, the buffer
 * can be released via @a rtems_regulator_release_buffer() as soon as the
 * function or copying completes. In the case where the delivery uses the
 * buffer and returns, the call to @a rtems_regulator_release_buffer()
 * will occur when the use of the buffer is complete (e.g. completion
 * of DMA transfer).  This explicit and deliberate exposure of buffering
 * provides the application with the ability to avoid copying the contents.
 *
 * After the Source has sent the message to the Regulator instance,
 * the Source is free to process another input and the Regulator
 * instance will ensure that the buffer is delivered to the Delivery
 * function and Destination.
 *
 * The Regulator implementation uses the RTEMS Classic API Partition Manager
 * to manage the buffer pool and the RTEMS Classic API Message Queue
 * Manager to send the buffer to the Delivery thread.
 */

#ifndef REGULATOR_H
#define REGULATOR_H

#include <stdlib.h>

#include <rtems.h>

/**
 * @ingroup RegulatorAPI
 *
 * @brief Regulator Delivery Function Type
 *
 * The user provides a function which is invoked to deliver a message
 * to the output. It is invoked by the Delivery thread created as part
 * of @a rtems_regulator_create(). The priority and stack size of the
 * Delivery thread are specified in the regulator attribute set.
 *
 * It takes three parameters:
 *
 * @param[in] context is an untyped pointer to a user context
 * @param[in] message points to the message
 * @param[in] length is the message size
 *
 * The following is an example deliverer function. It assumes that the
 * application has defined the my_context_t structure and it has at least
 * the socket field. The @a message passed in originated with an
 * application source which obtained the @a message buffer using
 * @a rtems_regulator_obtain_buffer(), filled it in with source data,
 * and used @a rtems_regulator_send() to hand to the regulator instance
 * for later delivery.
 *
 * @code
 *   bool my_deliverer(
 *     void     *context,
 *     void     *message,
 *     size_t    length
 *    )
 *    {
 *       my_context_t *my_context;
 *
 *       my_context = (my_context_t *)context;
 *
 *       write(my_context->socket, message, length);
 *       rtems_regulator_release_buffer(message);
 *       // return false to indicate we released the buffer
 *       return false;
 *     }
 * @endcode
 *
 * The delivery function returns true to indicate that the delivery thread
 * should release the buffer or false to indicate that it released the
 * buffer.  If the delivery function invokes a function like @a write()
 * to deliver the message to the destination, then the buffer can be
 * released immediately after the call. If the delivery function does
 * something like setting up a DMA transfer of the buffer, it cannot be
 * released until after the DMA is complete.
 *
 * The following sequence diagram shows the behavior of the Delivery thread
 * body and its interaction with the user-supplied deliverer() function.
 *
 * \startuml "Regulator Delivery Thread Body"
 *   loop while (1)
 *     "Delivery Thread" -> RTEMS : rtems_rate_monotonic_period(id, delivery_thread_period)
 *     loop for 0 : maximum_to_dequeue_per_period
 *       "Delivery Thread" -> RTEMS : rtems_message_queue_receive(id, message, size, wait, 0)
 *       RTEMS --> "Delivery Thread" : rtems_status_code
 *       group if [rtems_status_code != RTEMS_SUCCESSFUL]
 *         RTEMS -> "Delivery Thread" : break
 *       end
 *       "Delivery Thread" -> Application : deliverer(context, buffer, length)
 *       "Delivery Thread" -> RTEMS : rtems_partition_return_buffer(id, buffer)
 *       RTEMS --> "Delivery Thread" : rtems_status_code
 *     end
 *   end
 * \enduml
 *
 * In the above sequence diagram, the key points are:
 *
 *   -# The Delivery Thread Body is periodically executed.
 *   -# During each period, up to the instance configuration parameter
 *      @a maximum_to_dequeue_per_period may be dequeued and
 *      passed the application's delivery function for processing.
 *
 * Note that the application explicitly obtains buffers from the
 * regulator instance but that the release may be done by Delivery
 * Thread, the Delivery function, or later when the buffer contents
 * are transferred.
 */
typedef bool (*rtems_regulator_deliverer)(
  void     *context,
  void     *message,
  size_t    length
);

/**
 * @ingroup RegulatorAPI
 *
 * @brief Attributes for Regulator Instance
 *
 * An instance of this structure must be populated by the application
 * before creating an instance of the regulator. These settings tailor
 * the behavior of the regulator instance.
 */
typedef struct {
  /** Application function to invoke to output a message to the destination*/
  rtems_regulator_deliverer deliverer;

  /** Context pointer to pass to deliver function */
  void  *deliverer_context;

  /** Maximum size message to process */
  size_t  maximum_message_size;

  /** Maximum number of messages to be able to buffer */
  size_t  maximum_messages;

  /** Priority of Delivery thread */
  rtems_task_priority delivery_thread_priority;

  /** Stack size of Delivery thread */
  size_t delivery_thread_stack_size;

  /** Period (in ticks) of Delivery thread */
  rtems_interval delivery_thread_period;

  /** Maximum messages to dequeue per period */
  size_t  maximum_to_dequeue_per_period;

} rtems_regulator_attributes;

/**
 * @ingroup RegulatorAPI
 *
 * @brief Statistics for Regulator Instance
 *
 * An instance of this structure is provided to the directive
 * @a rtems_regulator_get_statistics and is filled in by that service.
 */
typedef struct {
  /** Number of successfully obtained buffers. */
  size_t    obtained;
  
  /** Number of successfully released buffers. */
  size_t    released;

  /** Number of successfully delivered buffers. */
  size_t    delivered;

  /** Rate Monotonic Period statistics for Delivery Thread */
  rtems_rate_monotonic_period_statistics period_statistics;

} rtems_regulator_statistics;

/**
 * @ingroup RegulatorAPI
 *
 * @brief Regulator Internal Structure
 */
struct _Regulator_Control;

/**
 * @ingroup RegulatorAPI
 *
 * @brief Regulator Instance
 *
 * This is used by the application as the handle to a Regulator instance.
 */
typedef struct _Regulator_Control *rtems_regulator_instance;

/**
 * @ingroup RegulatorAPI
 *
 * @brief Create a regulator
 *
 * This function creates an instance of a regulator. It uses the provided
 * @a attributes to create the instance return in @a regulator. This instance
 * will allocate the buffers associated with the regulator instance as well
 * as the Delivery thread.
 *
 * The @a attributes structure defines the priority and stack size of
 * the Delivery thread dedicated to this regulator instance. It also
 * defines the period of the Delivery thread and the maximum number of
 * messages that may be delivered per period via invocation of the
 * delivery function. 
 *
 * For each regulator instance, the following resources are allocated:
 *
 * - A memory area for the regulator control block using @a malloc().
 * - A RTEMS Classic API Message Queue is constructed with message 
 *   buffer memory allocated using @a malloc().  Each message consists
 *   of a pointer and a length.
 * - A RTEMS Classic API Partition.
 * - A RTEMS Classic API Rate Monotonic Period.
 *
 * @param[in] attributes specify the regulator instance attributes
 * @param[inout] regulator will point to the regulator instance
 *
 * @return an RTEMS status code indicating success or failure.
 *
 * @note This function allocates memory for the buffers holding messages,
 *       an Delivery thread and an RTEMS partition. When it executes, the
 *       Delivery thread will create an RTEMS rate monotonic period.
 */
rtems_status_code rtems_regulator_create(
  rtems_regulator_attributes  *attributes,
  rtems_regulator_instance   **regulator
);

/**
 * @ingroup RegulatorAPI
 *
 * @brief Delete a regulator
 *
 * This function is used to delete the specified @a regulator instance.
 *
 * It is the responsibility of the user to ensure that any resources
 * such as sockets or open file descriptors used by the delivery
 * function are also deleted. It is likely safer to delete those 
 * delivery resources after deleting the regulator instance rather than
 * before.
 *
 * @param[in] regulator is the instance to delete
 * @param[in] ticks is the maximum number of ticks to wait for
 *            the delivery thread to shutdown.
 *
 * @return an RTEMS status code indicating success or failure.
 *
 * @note This function deallocates the resources allocated during
 *       @a rtems_regulator_create().
 */
rtems_status_code rtems_regulator_delete(
  rtems_regulator_instance    *regulator,
  rtems_interval               ticks
);

/**
 * @ingroup RegulatorAPI
 *
 * @brief Obtain Buffer from Regulator
 *
 * This function is used to obtain a buffer from the regulator's pool. The
 * @a buffer returned is assumed to be filled in with contents and used
 * in a subsequent call to @a rtems_regulator_send(). When the @a buffer is
 * delivered, it is expected to be released. If the @a buffer is not
 * successfully accepted by this function,  then it should be returned
 * using @a rtems_regulator_release_buffer() or used to send another message.
 *
 * The @a buffer is of the maximum_message_size specified in the attributes
 * passed in to @a rtems_regulator_create().
 *
 * @param[in] regulator is the regulator instance to operate upon
 * @param[out] buffer will point to the allocated buffer
 *
 * @return an RTEMS status code indicating success or failure.
 *
 * @note This function does not perform dynamic allocation. It obtains a
 *       buffer from the pool allocated during @a rtems_regulator_create().
 *
 * @note Any attempt to write outside the buffer area is undefined.
 */
rtems_status_code rtems_regulator_obtain_buffer(
  rtems_regulator_instance   *regulator,
  void                      **buffer
);

/**
 * @ingroup RegulatorAPI
 *
 * @brief Release Previously Obtained Regulator Buffer
 *
 * This function is used to release a buffer to the regulator's pool. It is
 * assumed that the @a buffer returned will not be used by the application
 * anymore. The @a buffer must have previously been allocated by
 * @a rtems_regulator_obtain_buffer() and NOT passed to
 * @a rtems_regulator_send().
 *
 * If a subsequent @a rtems_regulator_send() using this @a buffer is
 * successful, the @a buffer will eventually be processed by the delivery
 * thread and released.
 *
 * @param[in] regulator is the regulator instance to operate upon
 * @param[out] buffer will point to the buffer to release
 *
 * @return an RTEMS status code indicating success or failure.
 *
 * @note This function does not perform dynamic deallocation. It releases a
 *       buffer to the pool allocated during @a rtems_regulator_create().
 */
rtems_status_code rtems_regulator_release_buffer(
  rtems_regulator_instance   *regulator,
  void                       *buffer
);

/**
 * @ingroup RegulatorAPI
 *
 * @brief Send to regulator instance
 *
 * This function is used by the producer to send a @a message to the
 * @a regulator for later delivery by the Delivery thread. The message is
 * contained in the memory pointed to by @a message and is @a length
 * bytes in length.
 *
 * It is required that the @a message buffer was obtained via
 * @a rtems_regulator_obtain_buffer().
 *
 * It is assumed that the @a message buffer has been filled in with
 * application content to deliver.
 *
 * If the @a rtems_regulator_send() is successful, the buffer is enqueued
 * inside the regulator instance for subsequent delivery. After the
 * @a message is delivered, it may be released by either delivery
 * function or the application code depending on the implementation.
 *
 * The status @a RTEMS_TOO_MANY is returned if the regulator's
 * internal queue is full. This indicates that the configured
 * maximum number of messages was insufficient. It is the
 * responsibility of the caller to decide whether to hold messages,
 * drop them, or print a message that the maximum number of messages
 * should be increased.
 *
 * If @a rtems_regulator_send() is unsuccessful, it is the application's
 * responsibility to release the buffer. If it is successfully sent,
 * then it becomes the responsibility of the delivery function to
 * release it.
 *
 * @param[in] regulator is the regulator instance to operate upon
 * @param[out] message points to the message to deliver
 * @param[out] length is the size of the message in bytes
 *
 * @return an RTEMS status code indicating success or failure.
 *
 */
rtems_status_code rtems_regulator_send(
  rtems_regulator_instance  *regulator,
  void                      *message,
  size_t                     length
);

/**
 * @ingroup RegulatorAPI
 *
 * @brief Obtain statistics for regulator instance
 *
 * This function is used by the application to obtain statistics
 * information about the regulator instance.  
 *
 * If the @a obtained and @a released fields in the returned 
 * @a statistics structure are equal, then there are no buffers
 * outstanding from this regulator instance.
 *
 * @param[in] regulator is the regulator instance to operate upon
 * @param[inout] statistics points to the statistics structure to fill in
 *
 * @return an RTEMS status code indicating success or failure.
 *
 */
rtems_status_code rtems_regulator_get_statistics(
  rtems_regulator_instance   *regulator,
  rtems_regulator_statistics *statistics
);

#endif /* REGULATOR_H */
