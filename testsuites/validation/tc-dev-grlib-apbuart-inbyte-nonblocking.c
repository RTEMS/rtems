/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup DevGrlibReqApbuartInbyteNonblocking
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

#include <string.h>
#include <grlib/apbuart.h>

#include <rtems/test.h>

/**
 * @defgroup DevGrlibReqApbuartInbyteNonblocking \
 *   spec:/dev/grlib/req/apbuart-inbyte-nonblocking
 *
 * @ingroup TestsuitesBspsValidationBsp0
 *
 * @{
 */

typedef enum {
  DevGrlibReqApbuartInbyteNonblocking_Pre_DataReady_Yes,
  DevGrlibReqApbuartInbyteNonblocking_Pre_DataReady_No,
  DevGrlibReqApbuartInbyteNonblocking_Pre_DataReady_NA
} DevGrlibReqApbuartInbyteNonblocking_Pre_DataReady;

typedef enum {
  DevGrlibReqApbuartInbyteNonblocking_Post_Result_Data,
  DevGrlibReqApbuartInbyteNonblocking_Post_Result_MinusOne,
  DevGrlibReqApbuartInbyteNonblocking_Post_Result_NA
} DevGrlibReqApbuartInbyteNonblocking_Post_Result;

typedef enum {
  DevGrlibReqApbuartInbyteNonblocking_Post_ErrorFlags_Cleared,
  DevGrlibReqApbuartInbyteNonblocking_Post_ErrorFlags_NA
} DevGrlibReqApbuartInbyteNonblocking_Post_ErrorFlags;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_DataReady_NA : 1;
  uint8_t Post_Result : 2;
  uint8_t Post_ErrorFlags : 1;
} DevGrlibReqApbuartInbyteNonblocking_Entry;

/**
 * @brief Test context for spec:/dev/grlib/req/apbuart-inbyte-nonblocking test
 *   case.
 */
typedef struct {
  /**
   * @brief This member provides the APBUART register block.
   */
  apbuart regs;

  /**
   * @brief This member contains the return value of the
   *   apbuart_inbyte_nonblocking() call.
   */
  int result;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 1 ];

    /**
     * @brief If this member is true, then the test action loop is executed.
     */
    bool in_action_loop;

    /**
     * @brief This member contains the next transition map index.
     */
    size_t index;

    /**
     * @brief This member contains the current transition map entry.
     */
    DevGrlibReqApbuartInbyteNonblocking_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} DevGrlibReqApbuartInbyteNonblocking_Context;

static DevGrlibReqApbuartInbyteNonblocking_Context
  DevGrlibReqApbuartInbyteNonblocking_Instance;

static const char * const DevGrlibReqApbuartInbyteNonblocking_PreDesc_DataReady[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const DevGrlibReqApbuartInbyteNonblocking_PreDesc[] = {
  DevGrlibReqApbuartInbyteNonblocking_PreDesc_DataReady,
  NULL
};

static void DevGrlibReqApbuartInbyteNonblocking_Pre_DataReady_Prepare(
  DevGrlibReqApbuartInbyteNonblocking_Context      *ctx,
  DevGrlibReqApbuartInbyteNonblocking_Pre_DataReady state
)
{
  switch ( state ) {
    case DevGrlibReqApbuartInbyteNonblocking_Pre_DataReady_Yes: {
      /*
       * While the data ready flag is set in the status register of the
       * register block specified by ``regs`` parameter.
       */
      ctx->regs.status |= APBUART_STATUS_DR;
      break;
    }

    case DevGrlibReqApbuartInbyteNonblocking_Pre_DataReady_No: {
      /*
       * While the data ready flag is cleared in the status register of the
       * register block specified by ``regs`` parameter.
       */
      ctx->regs.status &= ~APBUART_STATUS_DR;
      break;
    }

    case DevGrlibReqApbuartInbyteNonblocking_Pre_DataReady_NA:
      break;
  }
}

static void DevGrlibReqApbuartInbyteNonblocking_Post_Result_Check(
  DevGrlibReqApbuartInbyteNonblocking_Context    *ctx,
  DevGrlibReqApbuartInbyteNonblocking_Post_Result state
)
{
  switch ( state ) {
    case DevGrlibReqApbuartInbyteNonblocking_Post_Result_Data: {
      /*
       * The return value of apbuart_inbyte_nonblocking() shall be the data
       * read from the data register of the register block specified by
       * ``regs``.
       */
      T_eq_int( ctx->result, 0xff );
      break;
    }

    case DevGrlibReqApbuartInbyteNonblocking_Post_Result_MinusOne: {
      /*
       * The return value of apbuart_inbyte_nonblocking() shall be minus one.
       */
      T_eq_int( ctx->result, -1 );
      break;
    }

    case DevGrlibReqApbuartInbyteNonblocking_Post_Result_NA:
      break;
  }
}

static void DevGrlibReqApbuartInbyteNonblocking_Post_ErrorFlags_Check(
  DevGrlibReqApbuartInbyteNonblocking_Context        *ctx,
  DevGrlibReqApbuartInbyteNonblocking_Post_ErrorFlags state
)
{
  switch ( state ) {
    case DevGrlibReqApbuartInbyteNonblocking_Post_ErrorFlags_Cleared: {
      /*
       * The framing error, parity error, overrun, and break received flags in
       * the status register of the register block specified by ``regs`` shall
       * be cleared.
       */
      T_eq_u32( ctx->regs.status & 0x78, 0 );
      break;
    }

    case DevGrlibReqApbuartInbyteNonblocking_Post_ErrorFlags_NA:
      break;
  }
}

static void DevGrlibReqApbuartInbyteNonblocking_Prepare(
  DevGrlibReqApbuartInbyteNonblocking_Context *ctx
)
{
  memset( &ctx->regs, 0, sizeof( ctx->regs ) );
  ctx->regs.status = 0x78;
  ctx->regs.data = 0xff;
}

static void DevGrlibReqApbuartInbyteNonblocking_Action(
  DevGrlibReqApbuartInbyteNonblocking_Context *ctx
)
{
  ctx->result = apbuart_inbyte_nonblocking( &ctx->regs );
}

static const DevGrlibReqApbuartInbyteNonblocking_Entry
DevGrlibReqApbuartInbyteNonblocking_Entries[] = {
  { 0, 0, DevGrlibReqApbuartInbyteNonblocking_Post_Result_Data,
    DevGrlibReqApbuartInbyteNonblocking_Post_ErrorFlags_Cleared },
  { 0, 0, DevGrlibReqApbuartInbyteNonblocking_Post_Result_MinusOne,
    DevGrlibReqApbuartInbyteNonblocking_Post_ErrorFlags_Cleared }
};

static const uint8_t
DevGrlibReqApbuartInbyteNonblocking_Map[] = {
  0, 1
};

static size_t DevGrlibReqApbuartInbyteNonblocking_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  DevGrlibReqApbuartInbyteNonblocking_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      DevGrlibReqApbuartInbyteNonblocking_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture DevGrlibReqApbuartInbyteNonblocking_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = DevGrlibReqApbuartInbyteNonblocking_Scope,
  .initial_context = &DevGrlibReqApbuartInbyteNonblocking_Instance
};

static inline DevGrlibReqApbuartInbyteNonblocking_Entry
DevGrlibReqApbuartInbyteNonblocking_PopEntry(
  DevGrlibReqApbuartInbyteNonblocking_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return DevGrlibReqApbuartInbyteNonblocking_Entries[
    DevGrlibReqApbuartInbyteNonblocking_Map[ index ]
  ];
}

static void DevGrlibReqApbuartInbyteNonblocking_TestVariant(
  DevGrlibReqApbuartInbyteNonblocking_Context *ctx
)
{
  DevGrlibReqApbuartInbyteNonblocking_Pre_DataReady_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );
  DevGrlibReqApbuartInbyteNonblocking_Action( ctx );
  DevGrlibReqApbuartInbyteNonblocking_Post_Result_Check(
    ctx,
    ctx->Map.entry.Post_Result
  );
  DevGrlibReqApbuartInbyteNonblocking_Post_ErrorFlags_Check(
    ctx,
    ctx->Map.entry.Post_ErrorFlags
  );
}

/**
 * @fn void T_case_body_DevGrlibReqApbuartInbyteNonblocking( void )
 */
T_TEST_CASE_FIXTURE(
  DevGrlibReqApbuartInbyteNonblocking,
  &DevGrlibReqApbuartInbyteNonblocking_Fixture
)
{
  DevGrlibReqApbuartInbyteNonblocking_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = DevGrlibReqApbuartInbyteNonblocking_Pre_DataReady_Yes;
    ctx->Map.pcs[ 0 ] < DevGrlibReqApbuartInbyteNonblocking_Pre_DataReady_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = DevGrlibReqApbuartInbyteNonblocking_PopEntry( ctx );
    DevGrlibReqApbuartInbyteNonblocking_Prepare( ctx );
    DevGrlibReqApbuartInbyteNonblocking_TestVariant( ctx );
  }
}

/** @} */
