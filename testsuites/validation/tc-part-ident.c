/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsPartValIdent
 */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
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

#include "tr-object-ident.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsPartValIdent spec:/rtems/part/val/ident
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Test the rtems_partition_ident() directive.
 *
 * This test case performs the following actions:
 *
 * - Run the generic object identification tests for Classic API partition
 *   class objects defined by spec:/rtems/req/ident.
 *
 * @{
 */

#define NAME_LOCAL_OBJECT rtems_build_name( 'P', 'A', 'R', 'T' )

static rtems_status_code ClassicPartIdentAction(
  rtems_name name,
  uint32_t   node,
  rtems_id  *id
)
{
  return rtems_partition_ident( name, node, id );
}

/**
 * @brief Run the generic object identification tests for Classic API partition
 *   class objects defined by spec:/rtems/req/ident.
 */
static void RtemsPartValIdent_Action_0( void )
{
  static RTEMS_ALIGNED( RTEMS_PARTITION_ALIGNMENT ) long area[32];
  rtems_status_code                                      sc;
  rtems_id                                               id_local_object;

  sc = rtems_partition_create(
    NAME_LOCAL_OBJECT,
    area,
    sizeof( area ),
    sizeof( area ),
    RTEMS_DEFAULT_ATTRIBUTES,
    &id_local_object
  );
  T_assert_rsc_success( sc );

  RtemsReqIdent_Run(
    id_local_object,
    NAME_LOCAL_OBJECT,
    ClassicPartIdentAction
  );

  sc = rtems_partition_delete( id_local_object );
  T_rsc_success( sc );
}

/**
 * @fn void T_case_body_RtemsPartValIdent( void )
 */
T_TEST_CASE( RtemsPartValIdent )
{
  RtemsPartValIdent_Action_0();
}

/** @} */
