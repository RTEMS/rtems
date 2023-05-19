/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsCacheValCache
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
 * @defgroup RtemsCacheValCache spec:/rtems/cache/val/cache
 *
 * @ingroup TestsuitesValidationCache
 *
 * @brief Tests some @ref RTEMSAPIClassicCache directives.
 *
 * This test case performs the following actions:
 *
 * - Call the rtems_cache_disable_data() and rtems_cache_enable_data()
 *   directives.
 *
 * - Call the rtems_cache_disable_data() and rtems_cache_enable_data()
 *   directives with maskable interrupts disabled.
 *
 * - Call the rtems_cache_disable_instruction() and
 *   rtems_cache_enable_instruction() directives.
 *
 * - Call the rtems_cache_disable_instruction() and
 *   rtems_cache_enable_instruction() directives with maskable interrupts
 *   disabled.
 *
 * - Call the rtems_cache_freeze_data() and rtems_cache_unfreeze_data()
 *   directives.
 *
 * - Call the rtems_cache_freeze_data() and rtems_cache_unfreeze_data()
 *   directives with maskable interrupts disabled.
 *
 * - Call the rtems_cache_freeze_instruction() and
 *   rtems_cache_unfreeze_instruction() directives.
 *
 * - Call the rtems_cache_freeze_instruction() and
 *   rtems_cache_unfreeze_instruction() directives with maskable interrupts
 *   disabled.
 *
 * - Call the rtems_cache_invalidate_entire_data() directive with maskable
 *   interrupts disabled.
 *
 * - Call the rtems_cache_invalidate_entire_instruction() directive.
 *
 * - Call the rtems_cache_invalidate_entire_instruction() directive with
 *   maskable interrupts disabled.
 *
 * - Call the rtems_cache_flush_entire_data() directive.
 *
 * - Call the rtems_cache_flush_entire_data() directive with maskable
 *   interrupts disabled.
 *
 * - Call the rtems_cache_flush_multiple_data_lines() directive with a sample
 *   set of memory areas.
 *
 * - Call the rtems_cache_flush_multiple_data_lines() directive with a sample
 *   set of memory areas with maskable interrupts disabled.
 *
 * - Call the rtems_cache_invalidate_multiple_data_lines() directive with a
 *   sample set of memory areas.
 *
 * - Call the rtems_cache_invalidate_multiple_data_lines() directive with a
 *   sample set of memory areas with maskable interrupts disabled.
 *
 * - Call the rtems_cache_invalidate_multiple_instruction_lines() directive
 *   with a sample set of memory areas.
 *
 * - Call the rtems_cache_invalidate_multiple_instruction_lines() directive
 *   with a sample set of memory areas with maskable interrupts disabled.
 *
 * - Call the rtems_cache_instruction_sync_after_code_change() directive with a
 *   sample set of memory areas.
 *
 * - Call the rtems_cache_instruction_sync_after_code_change() directive with a
 *   sample set of memory areas with maskable interrupts disabled.
 *
 * - Call the rtems_cache_get_data_line_size(),
 *   rtems_cache_get_instruction_line_size(), and the
 *   rtems_cache_get_maximal_line_size() directives.
 *
 *   - Check that the maximal cache line size is greater than or equal to the
 *     data cache line size.
 *
 *   - Check that the maximal cache line size is greater than or equal to the
 *     instruction cache line size.
 *
 * - Call the rtems_cache_get_data_line_size(),
 *   rtems_cache_get_instruction_line_size(), and the
 *   rtems_cache_get_maximal_line_size() directives with maskable interrupts
 *   disabled.
 *
 *   - Check that the maximal cache line size is greater than or equal to the
 *     data cache line size.
 *
 *   - Check that the maximal cache line size is greater than or equal to the
 *     instruction cache line size.
 *
 * - Call the rtems_cache_get_data_cache_size() directive with increasing level
 *   starting with zero until it returns zero.
 *
 * - Call the rtems_cache_get_data_cache_size() directive with increasing level
 *   starting with zero until it returns zero with maskable interrupts
 *   disabled.
 *
 * - Call the rtems_cache_get_instruction_cache_size() directive with
 *   increasing level starting with zero until it returns zero.
 *
 * - Call the rtems_cache_get_instruction_cache_size() directive with
 *   increasing level starting with zero until it returns zero with maskable
 *   interrupts disabled.
 *
 * @{
 */

static void CallFlushMultipleDataLines( void )
{
  uint8_t buf[256];
  uintptr_t data;
  uintptr_t n;
  uintptr_t i;

  rtems_cache_flush_multiple_data_lines( NULL, 0 );
  data = RTEMS_ALIGN_UP( (uintptr_t) &buf[ 1 ], 128 );

  for ( n = 16; n <= 128 ; n *= 2 ) {
    for ( i = 0; i < 3; ++i ) {
      uintptr_t j;

      for ( j = 0; j < 3; ++j ) {
        rtems_cache_flush_multiple_data_lines(
          (const void *) ( data + 1 - i ),
          n + 1 - j
        );
      }
    }
  }
}

static void CallInvalidateMultipleDataLines( void )
{
  uint8_t buf[256];
  uintptr_t data;
  uintptr_t n;
  uintptr_t i;

  rtems_cache_invalidate_multiple_data_lines( NULL, 0 );
  data = RTEMS_ALIGN_UP( (uintptr_t) &buf[ 1 ], 128 );

  for ( n = 16; n <= 128 ; n *= 2 ) {
    for ( i = 0; i < 3; ++i ) {
      uintptr_t j;

      for ( j = 0; j < 3; ++j ) {
        rtems_cache_invalidate_multiple_data_lines(
          (const void *) ( data + 1 - i ),
          n + 1 - j
        );
      }
    }
  }
}

static void CallInvalidateMultipleInstructionLines( void )
{
  uintptr_t data;
  uintptr_t n;
  uintptr_t i;

  rtems_cache_invalidate_multiple_instruction_lines( NULL, 0 );
  data = (uintptr_t) rtems_cache_invalidate_multiple_instruction_lines;

  for ( n = 16; n <= 128 ; n *= 2 ) {
    for ( i = 0; i < 3; ++i ) {
      uintptr_t j;

      for ( j = 0; j < 3; ++j ) {
        rtems_cache_invalidate_multiple_instruction_lines(
          (const void *) ( data + 1 - i ),
          n + 1 - j
        );
      }
    }
  }
}

static void CallInstructionSyncAfterCodeChange( void )
{
  uintptr_t data;
  uintptr_t n;
  uintptr_t i;

  rtems_cache_instruction_sync_after_code_change( NULL, 0 );
  data = (uintptr_t) rtems_cache_instruction_sync_after_code_change;

  for ( n = 16; n <= 128 ; n *= 2 ) {
    for ( i = 0; i < 3; ++i ) {
      uintptr_t j;

      for ( j = 0; j < 3; ++j ) {
        rtems_cache_instruction_sync_after_code_change(
          (const void *) ( data + 1 - i ),
          n + 1 - j
        );
      }
    }
  }
}

static void CallGetDataSize( void )
{
  uint32_t level;
  size_t   n;

  level = 0;

  do {
    n = rtems_cache_get_data_cache_size( level );
    ++level;
  } while (n != 0 );
}

static void CallGetInstructionSize( void )
{
  uint32_t level;
  size_t   n;

  level = 0;

  do {
    n = rtems_cache_get_instruction_cache_size( level );
    ++level;
  } while (n != 0 );
}

/**
 * @brief Call the rtems_cache_disable_data() and rtems_cache_enable_data()
 *   directives.
 */
static void RtemsCacheValCache_Action_0( void )
{
  rtems_cache_disable_data();
  rtems_cache_enable_data();
}

/**
 * @brief Call the rtems_cache_disable_data() and rtems_cache_enable_data()
 *   directives with maskable interrupts disabled.
 */
static void RtemsCacheValCache_Action_1( void )
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  rtems_cache_disable_data();
  rtems_cache_enable_data();
  rtems_interrupt_local_enable(level);
}

/**
 * @brief Call the rtems_cache_disable_instruction() and
 *   rtems_cache_enable_instruction() directives.
 */
static void RtemsCacheValCache_Action_2( void )
{
  rtems_cache_disable_instruction();
  rtems_cache_enable_instruction();
}

/**
 * @brief Call the rtems_cache_disable_instruction() and
 *   rtems_cache_enable_instruction() directives with maskable interrupts
 *   disabled.
 */
static void RtemsCacheValCache_Action_3( void )
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  rtems_cache_disable_instruction();
  rtems_cache_enable_instruction();
  rtems_interrupt_local_enable(level);
}

/**
 * @brief Call the rtems_cache_freeze_data() and rtems_cache_unfreeze_data()
 *   directives.
 */
static void RtemsCacheValCache_Action_4( void )
{
  rtems_cache_freeze_data();
  rtems_cache_unfreeze_data();
}

/**
 * @brief Call the rtems_cache_freeze_data() and rtems_cache_unfreeze_data()
 *   directives with maskable interrupts disabled.
 */
static void RtemsCacheValCache_Action_5( void )
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  rtems_cache_freeze_data();
  rtems_cache_unfreeze_data();
  rtems_interrupt_local_enable(level);
}

/**
 * @brief Call the rtems_cache_freeze_instruction() and
 *   rtems_cache_unfreeze_instruction() directives.
 */
static void RtemsCacheValCache_Action_6( void )
{
  rtems_cache_freeze_instruction();
  rtems_cache_unfreeze_instruction();
}

/**
 * @brief Call the rtems_cache_freeze_instruction() and
 *   rtems_cache_unfreeze_instruction() directives with maskable interrupts
 *   disabled.
 */
static void RtemsCacheValCache_Action_7( void )
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  rtems_cache_freeze_instruction();
  rtems_cache_unfreeze_instruction();
  rtems_interrupt_local_enable(level);
}

/**
 * @brief Call the rtems_cache_invalidate_entire_data() directive with maskable
 *   interrupts disabled.
 */
static void RtemsCacheValCache_Action_8( void )
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  rtems_cache_disable_data();
  rtems_cache_invalidate_entire_data();
  rtems_cache_enable_data();
  rtems_interrupt_local_enable(level);
}

/**
 * @brief Call the rtems_cache_invalidate_entire_instruction() directive.
 */
static void RtemsCacheValCache_Action_9( void )
{
  rtems_cache_invalidate_entire_instruction();
}

/**
 * @brief Call the rtems_cache_invalidate_entire_instruction() directive with
 *   maskable interrupts disabled.
 */
static void RtemsCacheValCache_Action_10( void )
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  rtems_cache_invalidate_entire_instruction();
  rtems_interrupt_local_enable(level);
}

/**
 * @brief Call the rtems_cache_flush_entire_data() directive.
 */
static void RtemsCacheValCache_Action_11( void )
{
  rtems_cache_flush_entire_data();
}

/**
 * @brief Call the rtems_cache_flush_entire_data() directive with maskable
 *   interrupts disabled.
 */
static void RtemsCacheValCache_Action_12( void )
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  rtems_cache_flush_entire_data();
  rtems_interrupt_local_enable(level);
}

/**
 * @brief Call the rtems_cache_flush_multiple_data_lines() directive with a
 *   sample set of memory areas.
 */
static void RtemsCacheValCache_Action_13( void )
{
  CallFlushMultipleDataLines();
}

/**
 * @brief Call the rtems_cache_flush_multiple_data_lines() directive with a
 *   sample set of memory areas with maskable interrupts disabled.
 */
static void RtemsCacheValCache_Action_14( void )
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  CallFlushMultipleDataLines();
  rtems_interrupt_local_enable(level);
}

/**
 * @brief Call the rtems_cache_invalidate_multiple_data_lines() directive with
 *   a sample set of memory areas.
 */
static void RtemsCacheValCache_Action_15( void )
{
  CallInvalidateMultipleDataLines();
}

/**
 * @brief Call the rtems_cache_invalidate_multiple_data_lines() directive with
 *   a sample set of memory areas with maskable interrupts disabled.
 */
static void RtemsCacheValCache_Action_16( void )
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  CallInvalidateMultipleDataLines();
  rtems_interrupt_local_enable(level);
}

/**
 * @brief Call the rtems_cache_invalidate_multiple_instruction_lines()
 *   directive with a sample set of memory areas.
 */
static void RtemsCacheValCache_Action_17( void )
{
  CallInvalidateMultipleInstructionLines();
}

/**
 * @brief Call the rtems_cache_invalidate_multiple_instruction_lines()
 *   directive with a sample set of memory areas with maskable interrupts
 *   disabled.
 */
static void RtemsCacheValCache_Action_18( void )
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  CallInvalidateMultipleInstructionLines();
  rtems_interrupt_local_enable(level);
}

/**
 * @brief Call the rtems_cache_instruction_sync_after_code_change() directive
 *   with a sample set of memory areas.
 */
static void RtemsCacheValCache_Action_19( void )
{
  CallInstructionSyncAfterCodeChange();
}

/**
 * @brief Call the rtems_cache_instruction_sync_after_code_change() directive
 *   with a sample set of memory areas with maskable interrupts disabled.
 */
static void RtemsCacheValCache_Action_20( void )
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  CallInstructionSyncAfterCodeChange();
  rtems_interrupt_local_enable(level);
}

/**
 * @brief Call the rtems_cache_get_data_line_size(),
 *   rtems_cache_get_instruction_line_size(), and the
 *   rtems_cache_get_maximal_line_size() directives.
 */
static void RtemsCacheValCache_Action_21( void )
{
  size_t data_line_size;
  size_t instruction_line_size;
  size_t maximal_line_size;

  data_line_size = rtems_cache_get_data_line_size();
  instruction_line_size = rtems_cache_get_instruction_line_size();
  maximal_line_size = rtems_cache_get_maximal_line_size();

  /*
   * Check that the maximal cache line size is greater than or equal to the
   * data cache line size.
   */
  T_step_ge_sz( 0, maximal_line_size, data_line_size );

  /*
   * Check that the maximal cache line size is greater than or equal to the
   * instruction cache line size.
   */
  T_step_ge_sz( 1, maximal_line_size, instruction_line_size );
}

/**
 * @brief Call the rtems_cache_get_data_line_size(),
 *   rtems_cache_get_instruction_line_size(), and the
 *   rtems_cache_get_maximal_line_size() directives with maskable interrupts
 *   disabled.
 */
static void RtemsCacheValCache_Action_22( void )
{
  size_t data_line_size;
  size_t instruction_line_size;
  size_t maximal_line_size;
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  data_line_size = rtems_cache_get_data_line_size();
  instruction_line_size = rtems_cache_get_instruction_line_size();
  maximal_line_size = rtems_cache_get_maximal_line_size();
  rtems_interrupt_local_enable(level);

  /*
   * Check that the maximal cache line size is greater than or equal to the
   * data cache line size.
   */
  T_step_ge_sz( 2, maximal_line_size, data_line_size );

  /*
   * Check that the maximal cache line size is greater than or equal to the
   * instruction cache line size.
   */
  T_step_ge_sz( 3, maximal_line_size, instruction_line_size );
}

/**
 * @brief Call the rtems_cache_get_data_cache_size() directive with increasing
 *   level starting with zero until it returns zero.
 */
static void RtemsCacheValCache_Action_23( void )
{
  CallGetDataSize();
}

/**
 * @brief Call the rtems_cache_get_data_cache_size() directive with increasing
 *   level starting with zero until it returns zero with maskable interrupts
 *   disabled.
 */
static void RtemsCacheValCache_Action_24( void )
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  CallGetDataSize();
  rtems_interrupt_local_enable(level);
}

/**
 * @brief Call the rtems_cache_get_instruction_cache_size() directive with
 *   increasing level starting with zero until it returns zero.
 */
static void RtemsCacheValCache_Action_25( void )
{
  CallGetInstructionSize();
}

/**
 * @brief Call the rtems_cache_get_instruction_cache_size() directive with
 *   increasing level starting with zero until it returns zero with maskable
 *   interrupts disabled.
 */
static void RtemsCacheValCache_Action_26( void )
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  CallGetInstructionSize();
  rtems_interrupt_local_enable(level);
}

/**
 * @fn void T_case_body_RtemsCacheValCache( void )
 */
T_TEST_CASE( RtemsCacheValCache )
{
  T_plan( 4 );

  RtemsCacheValCache_Action_0();
  RtemsCacheValCache_Action_1();
  RtemsCacheValCache_Action_2();
  RtemsCacheValCache_Action_3();
  RtemsCacheValCache_Action_4();
  RtemsCacheValCache_Action_5();
  RtemsCacheValCache_Action_6();
  RtemsCacheValCache_Action_7();
  RtemsCacheValCache_Action_8();
  RtemsCacheValCache_Action_9();
  RtemsCacheValCache_Action_10();
  RtemsCacheValCache_Action_11();
  RtemsCacheValCache_Action_12();
  RtemsCacheValCache_Action_13();
  RtemsCacheValCache_Action_14();
  RtemsCacheValCache_Action_15();
  RtemsCacheValCache_Action_16();
  RtemsCacheValCache_Action_17();
  RtemsCacheValCache_Action_18();
  RtemsCacheValCache_Action_19();
  RtemsCacheValCache_Action_20();
  RtemsCacheValCache_Action_21();
  RtemsCacheValCache_Action_22();
  RtemsCacheValCache_Action_23();
  RtemsCacheValCache_Action_24();
  RtemsCacheValCache_Action_25();
  RtemsCacheValCache_Action_26();
}

/** @} */
