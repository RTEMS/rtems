/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup DevGrlibValIo
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
#include <grlib/io.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup DevGrlibValIo spec:/dev/grlib/val/io
 *
 * @ingroup TestsuitesBspsValidationBsp0
 *
 * @brief Tests some @ref RTEMSDeviceGRLIB directives.
 *
 * This test case performs the following actions:
 *
 * - Call grlib_load_8() to load a prepared value.
 *
 *   - Check that the returned value is equal to the prepared value.
 *
 * - Call grlib_load_16() to load a prepared value.
 *
 *   - Check that the returned value is equal to the prepared value.
 *
 * - Call grlib_load_32() to load a prepared value.
 *
 *   - Check that the returned value is equal to the prepared value.
 *
 * - Call grlib_load_64() to load a prepared value.
 *
 *   - Check that the returned value is equal to the prepared value.
 *
 * - Call grlib_store_8() to store a value to an object.
 *
 *   - Check that the value of the object is equal to the stored value.
 *
 * - Call grlib_store_16() to store a value to an object.
 *
 *   - Check that the value of the object is equal to the stored value.
 *
 * - Call grlib_store_32() to store a value to an object.
 *
 *   - Check that the value of the object is equal to the stored value.
 *
 * - Call grlib_store_64() to store a value to an object.
 *
 *   - Check that the value of the object is equal to the stored value.
 *
 * - Call apbuart_outbyte_polled() to store a character to the data register.
 *   The transmitter FIFO shall be initially non-empty.  The status is checked
 *   by apbuart_outbyte_wait().
 *
 *   - Check that the transmitter FIFO empty flag was set by ApbuartIORelax().
 *
 *   - Check that the data register was written by apbuart_outbyte_polled().
 *
 * @{
 */

static void ApbuartIORelax( void *arg )
{
  apbuart *regs;

  regs = arg;
  regs->status = 0x4;
  T_quiet_eq_u32( regs->data, 0 );
}

/**
 * @brief Call grlib_load_8() to load a prepared value.
 */
static void DevGrlibValIo_Action_0( void )
{
  uint8_t reg_8;
  uint8_t val_8;

  reg_8 = 0x81;
  val_8 = grlib_load_8( &reg_8 );

  /*
   * Check that the returned value is equal to the prepared value.
   */
  T_step_eq_u8( 0, val_8, 0x81 );
}

/**
 * @brief Call grlib_load_16() to load a prepared value.
 */
static void DevGrlibValIo_Action_1( void )
{
  uint16_t reg_16;
  uint16_t val_16;

  reg_16 = 0x8001;
  val_16 = grlib_load_16( &reg_16 );

  /*
   * Check that the returned value is equal to the prepared value.
   */
  T_step_eq_u16( 1, val_16, 0x8001 );
}

/**
 * @brief Call grlib_load_32() to load a prepared value.
 */
static void DevGrlibValIo_Action_2( void )
{
  uint32_t reg_32;
  uint32_t val_32;

  reg_32 = 0x80000001;
  val_32 = grlib_load_32( &reg_32 );

  /*
   * Check that the returned value is equal to the prepared value.
   */
  T_step_eq_u32( 2, val_32, 0x80000001 );
}

/**
 * @brief Call grlib_load_64() to load a prepared value.
 */
static void DevGrlibValIo_Action_3( void )
{
  uint64_t reg_64;
  uint64_t val_64;

  reg_64 = 0x8000000000000001;
  val_64 = grlib_load_64( &reg_64 );

  /*
   * Check that the returned value is equal to the prepared value.
   */
  T_step_eq_u64( 3, val_64, 0x8000000000000001 );
}

/**
 * @brief Call grlib_store_8() to store a value to an object.
 */
static void DevGrlibValIo_Action_4( void )
{
  uint8_t reg_8;

  grlib_store_8( &reg_8, 0x81 );

  /*
   * Check that the value of the object is equal to the stored value.
   */
  T_step_eq_u8( 4, reg_8, 0x81 );
}

/**
 * @brief Call grlib_store_16() to store a value to an object.
 */
static void DevGrlibValIo_Action_5( void )
{
  uint16_t reg_16;

  grlib_store_16( &reg_16, 0x8001 );

  /*
   * Check that the value of the object is equal to the stored value.
   */
  T_step_eq_u16( 5, reg_16, 0x8001 );
}

/**
 * @brief Call grlib_store_32() to store a value to an object.
 */
static void DevGrlibValIo_Action_6( void )
{
  uint32_t reg_32;

  grlib_store_32( &reg_32, 0x80000001 );

  /*
   * Check that the value of the object is equal to the stored value.
   */
  T_step_eq_u32( 6, reg_32, 0x80000001 );
}

/**
 * @brief Call grlib_store_64() to store a value to an object.
 */
static void DevGrlibValIo_Action_7( void )
{
  uint64_t reg_64;

  grlib_store_64( &reg_64, 0x8000000000000001 );

  /*
   * Check that the value of the object is equal to the stored value.
   */
  T_step_eq_u64( 7, reg_64, 0x8000000000000001 );
}

/**
 * @brief Call apbuart_outbyte_polled() to store a character to the data
 *   register.  The transmitter FIFO shall be initially non-empty.  The status
 *   is checked by apbuart_outbyte_wait().
 */
static void DevGrlibValIo_Action_8( void )
{
  apbuart regs;

  memset( &regs, 0, sizeof( regs ) );
  SetIORelaxHandler( ApbuartIORelax, &regs );
  apbuart_outbyte_polled( &regs, (char) 0xff );
  SetIORelaxHandler( NULL, NULL );

  /*
   * Check that the transmitter FIFO empty flag was set by ApbuartIORelax().
   */
  T_step_eq_u32( 8, regs.status, APBUART_STATUS_TE );

  /*
   * Check that the data register was written by apbuart_outbyte_polled().
   */
  T_step_eq_u32( 9, regs.data, 0xff );
}

/**
 * @fn void T_case_body_DevGrlibValIo( void )
 */
T_TEST_CASE( DevGrlibValIo )
{
  T_plan( 10 );

  DevGrlibValIo_Action_0();
  DevGrlibValIo_Action_1();
  DevGrlibValIo_Action_2();
  DevGrlibValIo_Action_3();
  DevGrlibValIo_Action_4();
  DevGrlibValIo_Action_5();
  DevGrlibValIo_Action_6();
  DevGrlibValIo_Action_7();
  DevGrlibValIo_Action_8();
}

/** @} */
