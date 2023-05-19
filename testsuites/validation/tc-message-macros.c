/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsMessageValMessageMacros
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

#include <rtems.h>

#include <rtems/test.h>

/**
 * @defgroup RtemsMessageValMessageMacros \
 *   spec:/rtems/message/val/message-macros
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests the macros of the @ref RTEMSAPIClassicMessage.
 *
 * This test case performs the following actions:
 *
 * - Check the RTEMS_MESSAGE_QUEUE_BUFFER() macro.
 *
 *   - Check that the object defined by the RTEMS_MESSAGE_QUEUE_BUFFER()
 *     expression has the desired size. rtems_message_queue_construct() will
 *     return RTEMS_UNSATISFIED instead of RTEMS_SUCCESSFUL if the object
 *     defined by the RTEMS_MESSAGE_QUEUE_BUFFER() expression has incorrect
 *     size.
 *
 * @{
 */

/**
 * @brief Check the RTEMS_MESSAGE_QUEUE_BUFFER() macro.
 */
static void RtemsMessageValMessageMacros_Action_0( void )
{
  rtems_status_code     status;
  rtems_id              id;

  static const uint32_t maximum_pending_messages_0 = 1;
  static const size_t   maximum_message_size_0 = 1;
  RTEMS_MESSAGE_QUEUE_BUFFER( maximum_message_size_0 )
                        storage_area_0[ maximum_pending_messages_0 ];
  rtems_message_queue_config config_0 = {
    .name = rtems_build_name( 'M', 'S', 'G', '0' ),
    .maximum_pending_messages = maximum_pending_messages_0,
    .maximum_message_size = maximum_message_size_0,
    .storage_area = storage_area_0,
    .storage_size = sizeof( storage_area_0 ),
    .storage_free = NULL,
    .attributes = RTEMS_DEFAULT_OPTIONS
  };

  static const uint32_t maximum_pending_messages_1 = 3;
  static const size_t   maximum_message_size_1 = 5;
  RTEMS_MESSAGE_QUEUE_BUFFER( maximum_message_size_1 )
                        storage_area_1[ maximum_pending_messages_1 ];
  rtems_message_queue_config config_1 = {
    .name = rtems_build_name( 'M', 'S', 'G', '1' ),
    .maximum_pending_messages = maximum_pending_messages_1,
    .maximum_message_size = maximum_message_size_1,
    .storage_area = storage_area_1,
    .storage_size = sizeof( storage_area_1 ),
    .storage_free = NULL,
    .attributes = RTEMS_DEFAULT_OPTIONS
  };

  /*
   * Check that the object defined by the RTEMS_MESSAGE_QUEUE_BUFFER()
   * expression has the desired size. rtems_message_queue_construct() will
   * return RTEMS_UNSATISFIED instead of RTEMS_SUCCESSFUL if the object defined
   * by the RTEMS_MESSAGE_QUEUE_BUFFER() expression has incorrect size.
   */
  status = rtems_message_queue_construct(
    &config_0,
    &id
  );
  T_step_rsc_success( 0, status );
  T_step_rsc_success( 1, rtems_message_queue_delete( id ) );

  status = rtems_message_queue_construct(
    &config_1,
    &id
  );
  T_step_rsc_success( 2, status );
  T_step_rsc_success( 3, rtems_message_queue_delete( id ) );
}

/**
 * @fn void T_case_body_RtemsMessageValMessageMacros( void )
 */
T_TEST_CASE( RtemsMessageValMessageMacros )
{
  T_plan( 4 );

  RtemsMessageValMessageMacros_Action_0();
}

/** @} */
