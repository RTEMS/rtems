/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  This file contains a typical set of register access routines which may be
 *  used with the icm7170 chip if accesses to the chip are as follows:
 *
 *    + registers are accessed as bytes
 *    + registers are only byte-aligned (no address gaps)
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
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

#include <rtems.h>
#include <libchip/rtc.h>
#include <libchip/icm7170.h>

#ifndef _ICM7170_MULTIPLIER
#define _ICM7170_MULTIPLIER 1
#define _ICM7170_NAME(_X) _X
#define _ICM7170_TYPE uint8_t
#endif

#define CALCULATE_REGISTER_ADDRESS( _base, _reg ) \
  (_ICM7170_TYPE *)((_base) + ((_reg) * _ICM7170_MULTIPLIER ))

/*
 *  ICM7170 Get Register Routine
 */

uint32_t   _ICM7170_NAME(icm7170_get_register)(
  uintptr_t   ulCtrlPort,
  uint8_t     ucRegNum
)
{
  _ICM7170_TYPE *port;

  port = CALCULATE_REGISTER_ADDRESS( ulCtrlPort, ucRegNum );

  return *port;
}

/*
 *  ICM7170 Set Register Routine
 */

void  _ICM7170_NAME(icm7170_set_register)(
  uintptr_t   ulCtrlPort,
  uint8_t     ucRegNum,
  uint32_t    ucData
)
{
  _ICM7170_TYPE *port;

  port = CALCULATE_REGISTER_ADDRESS( ulCtrlPort, ucRegNum );

  *port = ucData;
}
