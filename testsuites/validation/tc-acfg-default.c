/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup AcfgValDefault
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp.h>
#include <string.h>
#include <rtems/score/userextdata.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup AcfgValDefault spec:/acfg/val/default
 *
 * @ingroup TestsuitesValidationAcfg0
 *
 * @brief Tests the default values of application configuration options.
 *
 * This test case performs the following actions:
 *
 * - Check the effect of application configuration options with optional
 *   BSP-provided settings.
 *
 *   - Check the configured CONFIGURE_IDLE_TASK_BODY.
 *
 *   - Check the default value of CONFIGURE_IDLE_TASK_STACK_SIZE where the
 *     optional BSP-provided default value is enabled.
 *
 *   - Check the default value of CONFIGURE_INTERRUPT_STACK_SIZE where the
 *     optional BSP-provided default value is enabled.
 *
 *   - Check the BSP-provided initial extension is registered.
 *
 * - Try to create a barrier.
 *
 *   - Check that the returned status code is RTEMS_TOO_MANY.
 *
 * - Try to construct a message queue.
 *
 *   - Check that the returned status code is RTEMS_TOO_MANY.
 *
 * - Try to create a partition.
 *
 *   - Check that the returned status code is RTEMS_TOO_MANY.
 *
 * - Try to create a period.
 *
 *   - Check that the returned status code is RTEMS_TOO_MANY.
 *
 * - Check that the processor maximum is one.
 *
 * - Try to create a semaphore.
 *
 *   - Check that the returned status code is RTEMS_TOO_MANY.
 *
 * - Try to construct a task.
 *
 *   - Check that the returned status code is RTEMS_TOO_MANY.
 *
 *   - Check the default CONFIGURE_MAXIMUM_TASKS value.  A maximum Classic API
 *     task value of zero is only configurable if
 *     CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION is defined or
 *     CONFIGURE_MAXIMUM_POSIX_THREADS is set to a positive value.  The default
 *     value of zero for CONFIGURE_MAXIMUM_POSIX_THREADS is used by the test
 *     suite containing the test case.  The test suite defines
 *     CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION.
 *
 * - Try to create a timer.
 *
 *   - Check that the returned status code is RTEMS_TOO_MANY.
 *
 * - Try to create a user extension set.
 *
 *   - Check that the returned status code is RTEMS_TOO_MANY.
 *
 * @{
 */

#define NAME rtems_build_name( 'N', 'A', 'M', 'E' )

RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT) static char task_storage[
  RTEMS_TASK_STORAGE_SIZE(
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES
  )
];

static const rtems_task_config task_config = {
  .name = NAME,
  .initial_priority = 1,
  .storage_area = task_storage,
  .storage_size = sizeof( task_storage ),
  .maximum_thread_local_storage_size = 0,
  .initial_modes = RTEMS_DEFAULT_MODES,
  .attributes = RTEMS_DEFAULT_ATTRIBUTES
};

/**
 * @brief Check the effect of application configuration options with optional
 *   BSP-provided settings.
 */
static void AcfgValDefault_Action_0( void )
{
  rtems_extensions_table bsp = BSP_INITIAL_EXTENSION;

  /*
   * Check the configured CONFIGURE_IDLE_TASK_BODY.
   */
  T_step_eq_ptr( 0, rtems_configuration_get_idle_task(), IdleBody );

  /*
   * Check the default value of CONFIGURE_IDLE_TASK_STACK_SIZE where the
   * optional BSP-provided default value is enabled.
   */
  T_step_eq_sz(
    1,
    rtems_configuration_get_idle_task_stack_size(),
  #if defined(BSP_IDLE_TASK_STACK_SIZE)
    BSP_IDLE_TASK_STACK_SIZE
  #else
    CPU_STACK_MINIMUM_SIZE
  #endif
  );

  /*
   * Check the default value of CONFIGURE_INTERRUPT_STACK_SIZE where the
   * optional BSP-provided default value is enabled.
   */
  T_step_eq_sz(
    2,
    rtems_configuration_get_interrupt_stack_size(),
  #if defined(BSP_INTERRUPT_STACK_SIZE)
    BSP_INTERRUPT_STACK_SIZE
  #else
    CPU_STACK_MINIMUM_SIZE
  #endif
  );

  /*
   * Check the BSP-provided initial extension is registered.
   */
  T_step_eq_sz( 3, _User_extensions_Initial_count, 1 );
  T_step_eq_ptr(
    4,
    _User_extensions_Initial_extensions[ 0 ].fatal,
    bsp.fatal
  );
}

/**
 * @brief Try to create a barrier.
 */
static void AcfgValDefault_Action_1( void )
{
  rtems_status_code sc;
  rtems_id          id;

  sc = rtems_barrier_create(
    NAME,
    RTEMS_DEFAULT_ATTRIBUTES,
    1,
    &id
  );

  /*
   * Check that the returned status code is RTEMS_TOO_MANY.
   */
  T_step_rsc( 5, sc, RTEMS_TOO_MANY );
}

/**
 * @brief Try to construct a message queue.
 */
static void AcfgValDefault_Action_2( void )
{
  rtems_message_queue_config      config;
  RTEMS_MESSAGE_QUEUE_BUFFER( 1 ) buffers[ 1 ];
  rtems_status_code               sc;
  rtems_id                        id;

  memset( &config, 0, sizeof( config ) );
  config.name = NAME;
  config.maximum_pending_messages = 1;
  config.maximum_message_size = 1;
  config.storage_size = sizeof( buffers );
  config.storage_area = buffers;
  config.attributes = RTEMS_DEFAULT_ATTRIBUTES;

  sc = rtems_message_queue_construct( &config, &id );

  /*
   * Check that the returned status code is RTEMS_TOO_MANY.
   */
  T_step_rsc( 6, sc, RTEMS_TOO_MANY );
}

/**
 * @brief Try to create a partition.
 */
static void AcfgValDefault_Action_3( void )
{
  RTEMS_ALIGNED( RTEMS_PARTITION_ALIGNMENT ) uint8_t buffers[ 1 ][ 32 ];
  rtems_status_code                                  sc;
  rtems_id                                           id;

  sc = rtems_partition_create(
    NAME,
    buffers,
    sizeof( buffers ),
    sizeof( buffers[ 0 ] ),
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );

  /*
   * Check that the returned status code is RTEMS_TOO_MANY.
   */
  T_step_rsc( 7, sc, RTEMS_TOO_MANY );
}

/**
 * @brief Try to create a period.
 */
static void AcfgValDefault_Action_4( void )
{
  rtems_status_code sc;
  rtems_id          id;

  sc = rtems_rate_monotonic_create( NAME, &id );

  /*
   * Check that the returned status code is RTEMS_TOO_MANY.
   */
  T_step_rsc( 8, sc, RTEMS_TOO_MANY );
}

/**
 * @brief Check that the processor maximum is one.
 */
static void AcfgValDefault_Action_5( void )
{
  T_step_eq_u32( 9, rtems_scheduler_get_processor_maximum(), 1 );
}

/**
 * @brief Try to create a semaphore.
 */
static void AcfgValDefault_Action_6( void )
{
  rtems_status_code sc;
  rtems_id          id;

  sc = rtems_semaphore_create(
    NAME,
    0,
    RTEMS_DEFAULT_ATTRIBUTES,
    0,
    &id
  );

  /*
   * Check that the returned status code is RTEMS_TOO_MANY.
   */
  T_step_rsc( 10, sc, RTEMS_TOO_MANY );
}

/**
 * @brief Try to construct a task.
 */
static void AcfgValDefault_Action_7( void )
{
  rtems_status_code sc;
  rtems_id          id;

  sc = rtems_task_construct( &task_config, &id );

  /*
   * Check that the returned status code is RTEMS_TOO_MANY.
   */
  T_step_rsc( 11, sc, RTEMS_TOO_MANY );

  /*
   * Check the default CONFIGURE_MAXIMUM_TASKS value.  A maximum Classic API
   * task value of zero is only configurable if
   * CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION is defined or
   * CONFIGURE_MAXIMUM_POSIX_THREADS is set to a positive value.  The default
   * value of zero for CONFIGURE_MAXIMUM_POSIX_THREADS is used by the test
   * suite containing the test case.  The test suite defines
   * CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION.
   */
  T_step_eq_u32( 12, rtems_configuration_get_maximum_tasks(), 0 );
}

/**
 * @brief Try to create a timer.
 */
static void AcfgValDefault_Action_8( void )
{
  rtems_status_code sc;
  rtems_id          id;

  sc = rtems_timer_create( NAME, &id );

  /*
   * Check that the returned status code is RTEMS_TOO_MANY.
   */
  T_step_rsc( 13, sc, RTEMS_TOO_MANY );
}

/**
 * @brief Try to create a user extension set.
 */
static void AcfgValDefault_Action_9( void )
{
  rtems_extensions_table table;
  rtems_status_code      sc;
  rtems_id               id;

  memset( &table, 0, sizeof( table ) );
  sc = rtems_extension_create( NAME, &table, &id );

  /*
   * Check that the returned status code is RTEMS_TOO_MANY.
   */
  T_step_rsc( 14, sc, RTEMS_TOO_MANY );
}

/**
 * @fn void T_case_body_AcfgValDefault( void )
 */
T_TEST_CASE( AcfgValDefault )
{
  T_plan( 15 );

  AcfgValDefault_Action_0();
  AcfgValDefault_Action_1();
  AcfgValDefault_Action_2();
  AcfgValDefault_Action_3();
  AcfgValDefault_Action_4();
  AcfgValDefault_Action_5();
  AcfgValDefault_Action_6();
  AcfgValDefault_Action_7();
  AcfgValDefault_Action_8();
  AcfgValDefault_Action_9();
}

/** @} */
