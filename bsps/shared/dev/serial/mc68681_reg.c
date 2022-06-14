/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  This file contains a typical set of register access routines which may be
 *  used with the mc68681 chip if accesses to the chip are as follows:
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

#include <libchip/serial.h>
#include <libchip/mc68681.h>

#ifndef _MC68681_MULTIPLIER
#define _MC68681_MULTIPLIER 1
#define _MC68681_NAME(_X) _X
#define _MC68681_TYPE uint8_t
#endif

#define CALCULATE_REGISTER_ADDRESS( _base, _reg ) \
  (_MC68681_TYPE *)((_base) + ((_reg) * _MC68681_MULTIPLIER ))

/*
 *  MC68681 Get Register Routine
 */

uint8_t   _MC68681_NAME(mc68681_get_register)(
  uintptr_t   ulCtrlPort,
  uint8_t     ucRegNum
)
{
  _MC68681_TYPE *port;

  port = CALCULATE_REGISTER_ADDRESS( ulCtrlPort, ucRegNum );

  return *port;
}

/*
 *  MC68681 Set Register Routine
 */

void  _MC68681_NAME(mc68681_set_register)(
  uintptr_t   ulCtrlPort,
  uint8_t     ucRegNum,
  uint8_t     ucData
)
{
  _MC68681_TYPE *port;

  port = CALCULATE_REGISTER_ADDRESS( ulCtrlPort, ucRegNum );

  *port = ucData;
}
