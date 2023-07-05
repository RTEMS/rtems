/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @defgroup RegulatorTests Regulator Test Cases
 *
 * @brief Unit test cases for the Regulator
 *
 * This is a set of unit test cases for the regulator.
 */

/**
 * @ingroup RegulatorTests
 *
 * @file
 *
 * @brief Test 01 for Regulator Library
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

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/test-info.h>
#include <tmacros.h>

#include <rtems/regulator.h>

/**
 * @brief Regulator Test Name
 */
const char rtems_test_name[] = "Regulator 1";

/*
 * Prototypes for wrapped functions
 */
void *__wrap_malloc(size_t size);
void *__real_malloc(size_t size);

/**
 * @ingroup RegulatorTests
 * @brief Calloc Wrapper Trigger Count
 */
static int malloc_trigger_count;

/**
 * @ingroup RegulatorTests
 * @brief Calloc Wrapper Call Count
 */
static int malloc_call_count;

/**
 * @ingroup RegulatorTests
 * @brief Calloc Wrapper Trigger enable
 */
static bool malloc_trigger_enabled;

/**
 * @ingroup RegulatorTests
 * @brief Enable Calloc Wrapper Trigger
 */
static void malloc_trigger_enable(
  int trigger_count
)
{
  malloc_trigger_enabled = true;
  malloc_trigger_count = trigger_count;
  malloc_call_count = 0;
}
/**
 * @ingroup RegulatorTests
 * @brief Reset Calloc Wrapper Trigger and Count
 */
static void malloc_trigger_reset(void)
{
  malloc_trigger_enabled = 0;
  malloc_trigger_count = 0;
  malloc_call_count = 0;
}
/**
 * @ingroup RegulatorTests
 * @brief Calloc Wrapper to Trigger Allocation Errors
 */
void *__wrap_malloc(size_t size)
{
  if (malloc_trigger_enabled) {
    malloc_call_count++;
    if (malloc_call_count == malloc_trigger_count) {
      return NULL;
    }
  }

  return __real_malloc(size);
}

/**
 * @brief Constant to simpify code
 */
#define FIVE_SECONDS (5 * rtems_clock_get_ticks_per_second())

/**
 * @ingroup RegulatorTests
 * @brief Empty Deliver Method for Testing
 */
static bool test_regulator_deliverer(
  void     *context,
  void     *message,
  size_t    length
)
{
  (void) context;
  (void) message;
  (void) length;

  return true;
}

/**
 * @ingroup RegulatorTests
 * @brief Maximum length of a test message that is delivered
 */
#define MAXIMUM_MESSAGE_LENGTH 32

/**
 * @ingroup RegulatorTests
 * @brief Maximum number of test messages to buffer
 */
#define MAXIMUM_MESSAGES_TO_BUFFER 10

/**
 * @ingroup RegulatorTests
 * @brief Structure for capturing messages as delivered
 */
typedef struct {
  rtems_interval processed;
  char           message[MAXIMUM_MESSAGE_LENGTH];
} message_log_t;

/**
 * @ingroup RegulatorTests
 * @brief Set of Delivered messages
 */
message_log_t delivered_messages[MAXIMUM_MESSAGES_TO_BUFFER];

/**
 * @ingroup RegulatorTests
 * @brief Count of Delivered messages
 */
int delivered_message_count;

/**
 * @ingroup RegulatorTests
 * @brief Reset Delivered Message Set
 *
 * This is used at the beginning of a test case which is going to
 * check that message contents and delivery times were as expected.
 */
static void delivered_messages_reset(void)
{
  delivered_message_count = 0;
  memset(delivered_messages, 0xc5, sizeof(delivered_messages));
}

/**
 * @brief Context for Logger Delivery Function
 */
typedef struct {
  /** Regulator instance being operated on */
  rtems_regulator_instance   *regulator;
} deliverer_logger_context_t;

/**
 * @brief Context Instance for Logger Delivery Function
 */
static deliverer_logger_context_t deliverer_logger_context;

/**
 * @ingroup RegulatorTests
 * @brief Empty Deliver Method for Testing
 *
 * This deliverer method implementation logs the messages along with
 * their time of arrival. This is used by the test cases to verify
 * proper delivery.
 */
static bool test_regulator_deliverer_logger(
  void     *context,
  void     *message,
  size_t    length
)
{
  deliverer_logger_context_t *the_context;

  the_context = (deliverer_logger_context_t *)context;

  static bool caller_releases_buffer = true;

  size_t             len;
  rtems_interval     ticks;
  rtems_status_code  sc;

  len = strnlen(message, MAXIMUM_MESSAGE_LENGTH) + 1;
  rtems_test_assert(len = length);

  ticks = rtems_clock_get_ticks_since_boot();

  delivered_messages[delivered_message_count].processed = ticks;

  strcpy(delivered_messages[delivered_message_count].message, message);

  delivered_message_count++;

  /*
   * Alternate releasing the buffer here and allowing the calling Delivery
   * Thread to do it. This increases coverage of that logic.
   */
  if (caller_releases_buffer == true) {
    caller_releases_buffer = false;
    return true;
  }

  sc = rtems_regulator_release_buffer(the_context->regulator, message);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  return false;
}


/**
 * @ingroup RegulatorTests
 * @brief Helper to create a Regulator instance
 *
 * This helper creates a regulator instance with some arbitrary attributes.
 * This is used in multiple test cases to have a valie regulator instance to
 * trigger error cases.
 */
static rtems_regulator_instance *test_regulator_create_regulator_OK(void)
{
  rtems_status_code     sc;
  rtems_regulator_attributes  attributes = {
    .deliverer = test_regulator_deliverer,
    .deliverer_context = NULL,
    .maximum_message_size = 16,
    .maximum_messages = 10,
    .delivery_thread_priority = 16,
    .delivery_thread_stack_size = 0,
    .delivery_thread_period = RTEMS_MILLISECONDS_TO_TICKS(1000),
    .maximum_to_dequeue_per_period = 3
  };
  rtems_regulator_instance   *regulator;

  regulator = NULL;

  sc = rtems_regulator_create(&attributes, &regulator);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(regulator != NULL);

  return regulator;
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_create() maximum_to_dequeue_per_period
 * attributes error
 *
 * This unit test verifies that rtems_regulator_create() returns an error when
 * the maximum_to_dequeue_per_period attribute is zero.
 */
static void test_regulator_create_max_dequeue_zero(void)
{
  rtems_status_code     sc;
  rtems_regulator_attributes  attributes = {
    .deliverer = test_regulator_deliverer,
    .deliverer_context = NULL,
    .maximum_message_size = 16,
    .maximum_messages = 10,
    .delivery_thread_priority = 16,
    .delivery_thread_stack_size = 0,
    .delivery_thread_period = RTEMS_MILLISECONDS_TO_TICKS(1000),
    .maximum_to_dequeue_per_period = 0
  };
  rtems_regulator_instance   *regulator;

  regulator = NULL;

  sc = rtems_regulator_create(&attributes, &regulator);
  rtems_test_assert(sc == RTEMS_INVALID_NUMBER);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_create() NULL attributes error
 *
 * This unit test verifies that rtems_regulator_create() returns an error when
 * the attributes argument is NULL.
 */
static void test_regulator_create_null_attributes(void)
{
  rtems_status_code          sc;
  rtems_regulator_instance  *regulator;

  sc = rtems_regulator_create(NULL, &regulator);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_create NULL regulator error
 *
 * This unit test verifies that rtems_regulator_create() returns an error when
 * the regulator argument is NULL.
 */
static void test_regulator_create_null_regulator(void)
{
  rtems_status_code           sc;
  rtems_regulator_attributes  attributes;

  sc = rtems_regulator_create(&attributes, NULL);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_create deliverer is NULL
 *
 * This unit test verifies that rtems_regulator_create() returns an error when
 * the the attributes deliverer field is NULL.
 */
static void test_regulator_create_deliverer_is_null(void)
{
  rtems_status_code           sc;
  rtems_regulator_attributes  attributes;
  rtems_regulator_instance   *regulator;

  (void) memset(&attributes, 0, sizeof(rtems_regulator_attributes));

  attributes.deliverer                     = NULL;
  attributes.maximum_messages              = 0;
  attributes.maximum_message_size          = 16;
  attributes.maximum_to_dequeue_per_period = 1;

  sc = rtems_regulator_create(&attributes, &regulator);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_create maximum_messages is 0 error
 *
 * This unit test verifies that rtems_regulator_create() returns an error when
 * the the attributes maximum_messages field is 0.
 */
static void test_regulator_create_maximum_messages_is_zero(void)
{
  rtems_status_code           sc;
  rtems_regulator_attributes  attributes;
  rtems_regulator_instance   *regulator;

  (void) memset(&attributes, 0, sizeof(rtems_regulator_attributes));

  attributes.deliverer                     = test_regulator_deliverer;
  attributes.maximum_messages              = 0;
  attributes.maximum_message_size          = 16;
  attributes.maximum_to_dequeue_per_period = 1;

  sc = rtems_regulator_create(&attributes, &regulator);
  rtems_test_assert(sc == RTEMS_INVALID_NUMBER);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_create maximum_message_size is 0 error
 *
 * This unit test verifies that rtems_regulator_create() returns an error when
 * the the attributes maximum_message_size field is 0.
 */
static void test_regulator_create_maximum_message_size_is_zero(void)
{
  rtems_status_code           sc;
  rtems_regulator_attributes  attributes;
  rtems_regulator_instance   *regulator;

  (void) memset(&attributes, 0, sizeof(rtems_regulator_attributes));

  attributes.deliverer                     = test_regulator_deliverer;
  attributes.maximum_messages              = 10;
  attributes.maximum_message_size          = 0;
  attributes.maximum_to_dequeue_per_period = 1;

  sc = rtems_regulator_create(&attributes, &regulator);
  rtems_test_assert(sc == RTEMS_INVALID_SIZE);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_create maximum_to_dequeue_per_period is 0 error
 *
 * This unit test verifies that rtems_regulator_create() returns an error when
 * the the attributes maximum_to_dequeue_per_period field is 0.
 */
static void test_regulator_create_maximum_to_dequeue_per_period_is_zero(void)
{
  rtems_status_code           sc;
  rtems_regulator_attributes  attributes;
  rtems_regulator_instance   *regulator;

  (void) memset(&attributes, 0, sizeof(rtems_regulator_attributes));

  attributes.deliverer            = test_regulator_deliverer;
  attributes.maximum_messages     = 10;
  attributes.maximum_message_size = 0;

  sc = rtems_regulator_create(&attributes, &regulator);
  rtems_test_assert(sc == RTEMS_INVALID_SIZE);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_create returns error on failure to allocate regulator
 *
 * This unit test verifies that rtems_regulator_create() returns an error when
 * it is unable to allocate the mmemory for the regulator instance.
 */
static void test_regulator_create_malloc_regulator_fails(void)
{
  rtems_status_code           sc;
  rtems_regulator_attributes  attributes;
  rtems_regulator_instance         *regulator;

  (void) memset(&attributes, 0, sizeof(rtems_regulator_attributes));

  attributes.deliverer                     = test_regulator_deliverer;
  attributes.maximum_messages              = 10;
  attributes.maximum_message_size          = 16;
  attributes.delivery_thread_priority      = 32;
  attributes.maximum_to_dequeue_per_period = 1;
  attributes.delivery_thread_period        = RTEMS_MILLISECONDS_TO_TICKS(1000);

  malloc_trigger_enable(1);

  sc = rtems_regulator_create(&attributes, &regulator);
  rtems_test_assert(sc == RTEMS_NO_MEMORY);

  malloc_trigger_reset();
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_create returns error on failure to allocate buffers
 *
 * This unit test verifies that rtems_regulator_create() returns an error when
 * it is unable to allocate the mmemory for the regulator buffers.
 */
static void test_regulator_create_malloc_buffers_fails(void)
{
  rtems_status_code           sc;
  rtems_regulator_attributes  attributes;
  rtems_regulator_instance         *regulator;

  (void) memset(&attributes, 0, sizeof(rtems_regulator_attributes));

  attributes.deliverer                     = test_regulator_deliverer;
  attributes.maximum_messages              = 10;
  attributes.maximum_message_size          = 16;
  attributes.delivery_thread_priority      = 32;
  attributes.maximum_to_dequeue_per_period = 1;
  attributes.delivery_thread_period        = RTEMS_MILLISECONDS_TO_TICKS(1000);

  malloc_trigger_enable(2);

  sc = rtems_regulator_create(&attributes, &regulator);
  rtems_test_assert(sc == RTEMS_NO_MEMORY);

  malloc_trigger_reset();
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_create and delete work
 *
 * This unit test verifies that rtems_regulator_create() can successfully create
 * the the attributes delivery_thread_priority field is 0.
 */
static void test_regulator_create_delivery_thread_priority_is_zero(void)
{
  rtems_status_code           sc;
  rtems_regulator_attributes  attributes;
  rtems_regulator_instance         *regulator;

  (void) memset(&attributes, 0, sizeof(rtems_regulator_attributes));

  attributes.deliverer                     = test_regulator_deliverer;
  attributes.maximum_messages              = 10;
  attributes.maximum_message_size          = 16;
  attributes.delivery_thread_priority      = 0;
  attributes.maximum_to_dequeue_per_period = 1;
  attributes.delivery_thread_period        = RTEMS_MILLISECONDS_TO_TICKS(1000);

  sc = rtems_regulator_create(&attributes, &regulator);
  rtems_test_assert(sc == RTEMS_INVALID_PRIORITY);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_create rtems_partition_create error
 *
 * This unit test verifies that rtems_regulator_create() correctly returns an
 * error when the call to rtems_partition_create() fails.
 */
static void test_regulator_create_partition_create_fails(void)
{
  rtems_status_code           sc;
  rtems_id                    partition_id;
  unsigned long               partition_area[16];
  rtems_regulator_attributes  attributes;
  rtems_regulator_instance   *regulator;

  sc = rtems_partition_create(
    rtems_build_name('T', 'P', 'T', 'P'),
    partition_area,
    16 * sizeof(unsigned long),
    2 * sizeof(unsigned long),
    RTEMS_DEFAULT_ATTRIBUTES,
    &partition_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  (void) memset(&attributes, 0, sizeof(rtems_regulator_attributes));

  attributes.deliverer                     = test_regulator_deliverer;
  attributes.maximum_messages              = 10;
  attributes.maximum_message_size          = 16;
  attributes.delivery_thread_priority      = 8;
  attributes.maximum_to_dequeue_per_period = 1;
  attributes.delivery_thread_period        = RTEMS_MILLISECONDS_TO_TICKS(1000);

  sc = rtems_regulator_create(&attributes, &regulator);
  rtems_test_assert(sc == RTEMS_TOO_MANY);

  sc = rtems_partition_delete(partition_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_create rtems_message_queue_create error
 *
 * This unit test verifies that rtems_regulator_create() correctly returns an
 * error when the call to rtems_message_queue_create() fails.
 */
static void test_regulator_create_message_queue_create_fails(void)
{
  rtems_status_code           sc;
  rtems_id                    queue_id;
  rtems_regulator_attributes  attributes;
  rtems_regulator_instance   *regulator;

  sc = rtems_message_queue_create(
    rtems_build_name('T', 'Q', 'T', 'Q'),
    4,
    sizeof(unsigned long),
    RTEMS_DEFAULT_ATTRIBUTES,
    &queue_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  (void) memset(&attributes, 0, sizeof(rtems_regulator_attributes));

  attributes.deliverer                     = test_regulator_deliverer;
  attributes.maximum_messages              = 10;
  attributes.maximum_message_size          = 16;
  attributes.delivery_thread_priority      = 8;
  attributes.maximum_to_dequeue_per_period = 1;
  attributes.delivery_thread_period        = RTEMS_MILLISECONDS_TO_TICKS(1000);

  sc = rtems_regulator_create(&attributes, &regulator);
  rtems_test_assert(sc == RTEMS_TOO_MANY);

  sc = rtems_message_queue_delete(queue_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_create rtems_task_create error
 *
 * This unit test verifies that rtems_regulator_create() correctly returns an
 * error when the call to rtems_task_create() fails.
 */
static void test_regulator_create_task_create_fails(void)
{
  rtems_status_code           sc;
  rtems_id                    task_id;
  rtems_regulator_attributes  attributes;
  rtems_regulator_instance   *regulator;

  sc = rtems_task_create(
    rtems_build_name('T', 'T', 'T', 'T'),
    80,
    0,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  (void) memset(&attributes, 0, sizeof(rtems_regulator_attributes));

  attributes.deliverer                     = test_regulator_deliverer;
  attributes.maximum_messages              = 10;
  attributes.maximum_message_size          = 16;
  attributes.delivery_thread_priority      = 8;
  attributes.maximum_to_dequeue_per_period = 1;
  attributes.delivery_thread_period        = RTEMS_MILLISECONDS_TO_TICKS(1000);

  sc = rtems_regulator_create(&attributes, &regulator);
  rtems_test_assert(sc == RTEMS_TOO_MANY);

  sc = rtems_task_delete(task_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify Regulator Output Thread Handles Error on Period Create
 *
 * This unit test verifies that regulator output thread correctly exits
 * when the call to rtems_rate_monotonic_create() fails.
 *
 * This error condition/path cannot be directly detected via a return code,
 * It is verified via a debugger or code coverage reports.
 */
static void test_regulator_create_rate_monotonic_create_fails(void)
{
  rtems_status_code           sc;
  rtems_id                    period_id;
  rtems_regulator_attributes  attributes;
  rtems_regulator_instance   *regulator;

  sc = rtems_rate_monotonic_create(
    rtems_build_name('T', 'S', 'T', 'P'),
    &period_id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  (void) memset(&attributes, 0, sizeof(rtems_regulator_attributes));

  attributes.deliverer                     = test_regulator_deliverer;
  attributes.maximum_messages              = 10;
  attributes.maximum_message_size          = 16;
  attributes.delivery_thread_priority      = 8;
  attributes.maximum_to_dequeue_per_period = 1;
  attributes.delivery_thread_period        = RTEMS_MILLISECONDS_TO_TICKS(1000);

  sc = rtems_regulator_create(&attributes, &regulator);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  /*
   * Let the output thread execute and encounter the create error.
   */

  sleep(1);

  /*
   * Now deallocate the resources allocated earlier
   */
  sc = rtems_regulator_delete(regulator, FIVE_SECONDS);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_rate_monotonic_delete(period_id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_delete NULL regulator error
 *
 * This unit test verifies that rtems_regulator_delete() returns an error when
 * the regulator argument is NULL.
 */
static void test_regulator_delete_null_regulator(void)
{
  rtems_status_code     sc;

  sc = rtems_regulator_delete(NULL, FIVE_SECONDS);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_delete uninitialized regulator error
 *
 * This unit test verifies that rtems_regulator_delete() returns an error when
 * the regulator argument is uninitialized.
 */
static void test_regulator_delete_uninitialized_regulator(void)
{
  rtems_status_code         sc;
  rtems_regulator_instance  regulator;

  (void) memset(&regulator, 0, sizeof(regulator));

  sc = rtems_regulator_delete(&regulator, 0);
  rtems_test_assert(sc == RTEMS_INCORRECT_STATE);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_delete successful case
 *
 * This unit test verifies that rtems_regulator_delete() can be successfully
 * deleted.
 */
static void test_regulator_delete_OK(void)
{
  rtems_status_code         sc;
  rtems_regulator_instance *regulator;

  regulator = test_regulator_create_regulator_OK();
  rtems_test_assert(regulator != NULL);

  sc = rtems_regulator_delete(regulator, FIVE_SECONDS);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_obtain_buffer NULL regulator error
 *
 * This unit test verifies that rtems_regulator_obtain_buffer() returns an error when
 * the regulator argument is NULL.
 */
static void test_regulator_obtain_buffer_null_regulator(void)
{
  rtems_status_code   sc;
  void               *buffer;

  sc = rtems_regulator_obtain_buffer(NULL, &buffer);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_obtain_buffer uninitialized regulator error
 *
 * This unit test verifies that rtems_regulator_obtain_buffer() returns an error when
 * the regulator argument is uninitialized.
 */
static void test_regulator_obtain_buffer_uninitialized_regulator(void)
{
  rtems_status_code         sc;
  rtems_regulator_instance  regulator;
  void                     *buffer;

  (void) memset(&regulator, 0, sizeof(regulator));

  sc = rtems_regulator_obtain_buffer(&regulator, &buffer);
  rtems_test_assert(sc == RTEMS_INCORRECT_STATE);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_obtain_buffer successful case
 *
 * This unit test verifies that rtems_regulator_obtain_buffer() can be successfully
 * obtained from an initialized regulator.
 */
static void test_regulator_obtain_buffer_OK(void)
{
  rtems_status_code         sc;
  rtems_regulator_instance *regulator;
  void                     *buffer;

  regulator = test_regulator_create_regulator_OK();
  rtems_test_assert(regulator != NULL);

  sc = rtems_regulator_obtain_buffer(regulator, &buffer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(buffer != NULL);

  /*
   * Not really testing this here but cannot delete underlying partition
   * if there are buffers outstanding.
   */
  sc = rtems_regulator_release_buffer(regulator, buffer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(buffer != NULL);

  sc = rtems_regulator_delete(regulator, FIVE_SECONDS);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_release_buffer NULL regulator error
 *
 * This unit test verifies that rtems_regulator_release_buffer() returns an error when
 * the regulator argument is NULL.
 */
static void test_regulator_release_buffer_null_regulator(void)
{
  rtems_status_code   sc;
  void               *buffer;

  sc = rtems_regulator_release_buffer(NULL, &buffer);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_release_buffer uninitialized regulator error
 *
 * This unit test verifies that rtems_regulator_release_buffer() returns an
 * error when the regulator argument is uninitialized.
 */
static void test_regulator_release_buffer_uninitialized_regulator(void)
{
  rtems_status_code         sc;
  rtems_regulator_instance  regulator;
  void                     *buffer;

  (void) memset(&regulator, 0, sizeof(regulator));

  sc = rtems_regulator_release_buffer(&regulator, &buffer);
  rtems_test_assert(sc == RTEMS_INCORRECT_STATE);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_release_buffer successful case
 *
 * This unit test verifies that rtems_regulator_release_buffer() can be successfully
 * invoked with a buffer previously allocated by rtems_regulator_obtain_buffer().
 */
static void test_regulator_release_buffer_OK(void)
{
  rtems_status_code         sc;
  rtems_regulator_instance *regulator;
  void                     *buffer;

  regulator = test_regulator_create_regulator_OK();
  rtems_test_assert(regulator != NULL);

  sc = rtems_regulator_obtain_buffer(regulator, &buffer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(buffer != NULL);

  sc = rtems_regulator_release_buffer(regulator, buffer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_regulator_delete(regulator, FIVE_SECONDS);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_send NULL regulator error
 *
 * This unit test verifies that rtems_regulator_send() returns an error when
 * the regulator argument is NULL.
 */
static void test_regulator_send_null_regulator(void)
{
  rtems_status_code   sc;
  void               *buffer;
  size_t              length;

  buffer = &length;
  length = sizeof(size_t);

  sc = rtems_regulator_send(NULL, buffer, length);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_send NULL message error
 *
 * This unit test verifies that rtems_regulator_send() returns an error when
 * the message argument is NULL.
 */
static void test_regulator_send_null_message(void)
{
  rtems_status_code         sc;
  size_t                    length;
  rtems_regulator_instance  regulator;

  length = sizeof(size_t);

  sc = rtems_regulator_send(&regulator, NULL, length);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_send zero length message error
 *
 * This unit test verifies that rtems_regulator_send() returns an
 * error when the message length is 0.
 */
static void test_regulator_send_length_is_zero(void)
{
  rtems_status_code         sc;
  rtems_regulator_instance  regulator;
  void                     *buffer;
  size_t                    length;

  buffer = &length;
  length = 0;

  sc = rtems_regulator_send(&regulator, buffer, length);
  rtems_test_assert(sc == RTEMS_INVALID_NUMBER);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_send uninitialized regulator error
 *
 * This unit test verifies that rtems_regulator_send() returns an
 * error when the regulator argument is uninitialized.
 */
static void test_regulator_send_uninitialized_regulator(void)
{
  rtems_status_code         sc;
  rtems_regulator_instance  regulator;
  void                     *buffer;
  size_t                    length;

  buffer = &length;
  length = sizeof(size_t);

  (void) memset(&regulator, 0, sizeof(regulator));

  sc = rtems_regulator_send(&regulator, buffer, length);
  rtems_test_assert(sc == RTEMS_INCORRECT_STATE);
}

/**
 * @ingroup RegulatorTests
 * @brief Verify Cannot Delete with Message Outstanding
 *
 * This unit test verifies that when the regulator is successfully
 * initialized, that it cannot be deleted with an undelivered message.
 * It also verifies some basic statistics are working.
 */
static void test_regulator_cannot_delete_with_outstanding(void)
{
  rtems_status_code         sc;
  rtems_regulator_instance *regulator;
  char                      message[MAXIMUM_MESSAGE_LENGTH];
  void                     *buffer;
  size_t                    length;
  int                       match;
  rtems_regulator_attributes  attributes = {
    .deliverer = test_regulator_deliverer_logger,
    .deliverer_context = &deliverer_logger_context,
    .maximum_message_size = 16,
    .maximum_messages = 10,
    .delivery_thread_priority = 16,
    .delivery_thread_stack_size = 0,
    .delivery_thread_period = RTEMS_MILLISECONDS_TO_TICKS(250),
    .maximum_to_dequeue_per_period = 3
  };
  rtems_regulator_statistics stats;

  sc = rtems_regulator_create(&attributes, &regulator);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(regulator != NULL);

  deliverer_logger_context.regulator = regulator;

  delivered_messages_reset();

  // Ensure statistics show no buffers obtained or processed
  sc = rtems_regulator_get_statistics(regulator, &stats);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(stats.obtained == 0);
  rtems_test_assert(stats.released == 0);
  rtems_test_assert(stats.delivered == 0);
  rtems_test_assert(stats.period_statistics.count == 0);
  rtems_test_assert(stats.period_statistics.missed_count == 0);

  // Obtain a buffer which should change the statistics
  sc = rtems_regulator_obtain_buffer(regulator, &buffer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(buffer != NULL);

  // Ensure statistics show one buffer obtained
  sc = rtems_regulator_get_statistics(regulator, &stats);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(stats.obtained == 1);
  rtems_test_assert(stats.released == 0);
  rtems_test_assert(stats.delivered == 0);
  rtems_test_assert(stats.period_statistics.count == 0);
  rtems_test_assert(stats.period_statistics.missed_count == 0);


  // Format and send the message -- statistics do not change
  length = snprintf(message, MAXIMUM_MESSAGE_LENGTH, "message %d", 1024) + 1;
  strcpy(buffer, message);

  sc = rtems_regulator_send(regulator, buffer, length);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  // Ensure statistics show one buffer obtained
  sc = rtems_regulator_get_statistics(regulator, &stats);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(stats.obtained == 1);
  rtems_test_assert(stats.released == 0);
  rtems_test_assert(stats.delivered == 0);
  rtems_test_assert(stats.period_statistics.count == 0);
  rtems_test_assert(stats.period_statistics.missed_count == 0);

  // This is the actual failing case -- cannot delete w/outstanding
  sc = rtems_regulator_delete(regulator, FIVE_SECONDS);
  rtems_test_assert(sc == RTEMS_RESOURCE_IN_USE);

  // Now let the deliveries happen
  sleep(1);

  // Ensure statistics show all buffers released
  sc = rtems_regulator_get_statistics(regulator, &stats);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(stats.obtained == 1);
  rtems_test_assert(stats.released == 1);
  rtems_test_assert(stats.delivered == 1);
  rtems_test_assert(stats.period_statistics.count != 0);
  rtems_test_assert(stats.period_statistics.missed_count == 0);

  rtems_test_assert(delivered_message_count == 1);
  match = strncmp(
    delivered_messages[0].message,
    message,
    MAXIMUM_MESSAGE_LENGTH
  );
  rtems_test_assert(match == 0);

  sc = rtems_regulator_delete(regulator, FIVE_SECONDS);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  deliverer_logger_context.regulator = NULL;
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_send and output thread delivers message
 *
 * This unit test verifies that when the regulator is
 * successfully initialized and used as expected, a message sent via
 * rtems_regulator_send() is delivered as expected.
 */
static void test_regulator_send_one_message_OK(void)
{
  rtems_status_code         sc;
  rtems_regulator_instance *regulator;
  char                      message[MAXIMUM_MESSAGE_LENGTH];
  void                     *buffer;
  size_t                    length;
  int                       match;
  rtems_regulator_attributes  attributes = {
    .deliverer = test_regulator_deliverer_logger,
    .deliverer_context = &deliverer_logger_context,
    .maximum_message_size = 16,
    .maximum_messages = 10,
    .delivery_thread_priority = 16,
    .delivery_thread_stack_size = 0,
    .delivery_thread_period = RTEMS_MILLISECONDS_TO_TICKS(250),
    .maximum_to_dequeue_per_period = 3
  };

  sc = rtems_regulator_create(&attributes, &regulator);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(regulator != NULL);

  deliverer_logger_context.regulator = regulator;

  delivered_messages_reset();

  sc = rtems_regulator_obtain_buffer(regulator, &buffer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(buffer != NULL);

  length = snprintf(message, MAXIMUM_MESSAGE_LENGTH, "message %d", 1024) + 1;
  strcpy(buffer, message);

  sc = rtems_regulator_send(regulator, buffer, length);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sleep(1);

  rtems_test_assert(delivered_message_count == 1);
  match = strncmp(
    delivered_messages[0].message,
    message,
    MAXIMUM_MESSAGE_LENGTH
  );
  rtems_test_assert(match == 0);

  sc = rtems_regulator_delete(regulator, FIVE_SECONDS);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  deliverer_logger_context.regulator = NULL;
}

/**
 * @ingroup RegulatorTests
 * @brief Verify rtems_regulator_send and output thread delivers messages
 *
 * This unit test verifies that when the regulator is successfully initialized
 * and used as expected, and multiple messages are sent via rtems_regulator_send()
 * that they are delivered as expected.
 */
#include <stdio.h>
static void test_regulator_send_multiple_messages_OK(void)
{
  rtems_status_code         sc;
  rtems_regulator_instance *regulator;
  char                      message[MAXIMUM_MESSAGE_LENGTH];
  void                     *buffer;
  size_t                    length;
  int                       match;
  int                       i;
  time_t                    base_time;
  time_t                    tmp_time;
  rtems_interval            base_ticks;
  rtems_interval            ticks;
  rtems_interval            ticks_per_second;

  rtems_regulator_attributes  attributes = {
    .deliverer = test_regulator_deliverer_logger,
    .deliverer_context = &deliverer_logger_context,
    .maximum_message_size = MAXIMUM_MESSAGE_LENGTH,
    .maximum_messages = 10,
    .delivery_thread_priority = 16,
    .delivery_thread_stack_size = 0,
    .delivery_thread_period = RTEMS_MILLISECONDS_TO_TICKS(1000),
    .maximum_to_dequeue_per_period = 2
  };

  delivered_messages_reset();

  sc = rtems_regulator_create(&attributes, &regulator);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(regulator != NULL);

  deliverer_logger_context.regulator = regulator;

  /*
   * Ensure the messages are sent on a second boundary to ensure the
   * output thread will process them as expected.
   */
  tmp_time = time(NULL);
  do {
    base_time = time(NULL);
  } while (tmp_time == base_time);

  /**
   * Send five messages as a burst which will need to be smoothly sent at
   * the configured rate.
   */
  for (i=1 ; i <= 5 ; i++) {
    sc = rtems_regulator_obtain_buffer(regulator, &buffer);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    rtems_test_assert(buffer != NULL);

    length = snprintf(message, MAXIMUM_MESSAGE_LENGTH, "message %d", i);
    strcpy(buffer, message);

    sc = rtems_regulator_send(regulator, buffer, length);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  /*
   * Let the output thread executed and deliver the messages.
   */
  sleep(5);

  /**
   * Ensure the five messages were delivered as follows:
   *
   *   - deliver all 5
   *   - contents are "message N" where N is 1 to 5
   *   - message 1 and 2 delivered during the first second
   *   - message 3 and 4 delivered during the second second
   *   - message 5 delivered during the third second
   *   - no further messages delivered
   */

  rtems_test_assert(delivered_message_count == 5);

  for (i=0 ; i < 5 ; i++) {
    (void) snprintf(message, MAXIMUM_MESSAGE_LENGTH, "message %d", i+1);
// printf("%d %s\n", i, delivered_messages[i].message);
    match = strncmp(
      delivered_messages[i].message,
      message,
      MAXIMUM_MESSAGE_LENGTH
    );
    rtems_test_assert(match == 0);
  }

  /**
   * Verify that messages were delivered in the proper groups. Check that
   * the delivery time matches expectations.
   */
  rtems_test_assert(delivered_messages[0].processed == delivered_messages[1].processed);
  rtems_test_assert(delivered_messages[1].processed != delivered_messages[2].processed);
  rtems_test_assert(delivered_messages[2].processed == delivered_messages[3].processed);
  rtems_test_assert(delivered_messages[3].processed != delivered_messages[4].processed);

  /**
   * Verify that the message groups were properly spaced temporally. They
   * should be one second apart.
   */
  ticks_per_second = rtems_clock_get_ticks_per_second();

  base_ticks = delivered_messages[1].processed;
  ticks      = delivered_messages[2].processed;
  rtems_test_assert(ticks_per_second == ticks - base_ticks);

  base_ticks = delivered_messages[3].processed;
  ticks      = delivered_messages[4].processed;
  rtems_test_assert(ticks_per_second == ticks - base_ticks);

  sc = rtems_regulator_delete(regulator, FIVE_SECONDS);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  deliverer_logger_context.regulator = NULL;
}

/* Necessary prototype */
rtems_task test_regulator(rtems_task_argument);

/**
 * @ingroup RegulatorTestshttps://devel.rtems.org/milestone/6.1
 * @brief Test entry task which invokes test cases
 */
rtems_task test_regulator(rtems_task_argument arg)
{
  (void) arg;

  TEST_BEGIN();

  malloc_trigger_reset();

  test_regulator_create_max_dequeue_zero();
  test_regulator_create_null_attributes();
  test_regulator_create_null_regulator();
  test_regulator_create_deliverer_is_null();
  test_regulator_create_maximum_messages_is_zero();
  test_regulator_create_maximum_message_size_is_zero();
  test_regulator_create_maximum_to_dequeue_per_period_is_zero();
  test_regulator_create_malloc_regulator_fails();
  test_regulator_create_malloc_buffers_fails();
  test_regulator_create_delivery_thread_priority_is_zero();
  test_regulator_create_partition_create_fails();
  test_regulator_create_message_queue_create_fails();
  test_regulator_create_task_create_fails();
  test_regulator_create_rate_monotonic_create_fails();

  test_regulator_delete_null_regulator();
  test_regulator_delete_uninitialized_regulator();
  test_regulator_delete_OK();

  test_regulator_obtain_buffer_null_regulator();
  test_regulator_obtain_buffer_uninitialized_regulator();
  test_regulator_obtain_buffer_OK();

  test_regulator_release_buffer_null_regulator();
  test_regulator_release_buffer_uninitialized_regulator();
  test_regulator_release_buffer_OK();

  test_regulator_send_null_regulator();
  test_regulator_send_null_message();
  test_regulator_send_length_is_zero();
  test_regulator_send_uninitialized_regulator();

  test_regulator_send_one_message_OK();
  test_regulator_cannot_delete_with_outstanding();

  test_regulator_send_multiple_messages_OK();

  TEST_END();

  rtems_test_exit(0);
}
