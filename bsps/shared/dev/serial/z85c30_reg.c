/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  This file contains a typical set of register access routines which may be
 *  used with the z85c30 chip if accesses to the chip are as follows:
 *
 *    + registers are accessed as bytes
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

#include <libchip/z85c30.h>

#ifndef _Z85C30_MULTIPLIER
#define _Z85C30_MULTIPLIER 1
#define _Z85C30_NAME(_X) _X
#define _Z85C30_TYPE uint8_t
#endif

/*
 *  Z85C30 Get Register Routine
 */

uint8_t   _Z85C30_NAME(z85c30_get_register)(
  uintptr_t   ulCtrlPort,
  uint8_t     ucRegNum
)
{
  _Z85C30_TYPE          *port;
  uint8_t                data;
  rtems_interrupt_level  level;

  port = (_Z85C30_TYPE *)ulCtrlPort;

  rtems_interrupt_disable(level);

    if(ucRegNum) {
      *port = ucRegNum;
    }
    data = *port;
  rtems_interrupt_enable(level);

  return data;
}

/*
 *  Z85C30 Set Register Routine
 */

void _Z85C30_NAME(z85c30_set_register)(
  uintptr_t   ulCtrlPort,
  uint8_t     ucRegNum,
  uint8_t     ucData
)
{
  _Z85C30_TYPE          *port;
  rtems_interrupt_level  level;

  port = (_Z85C30_TYPE *)ulCtrlPort;

  rtems_interrupt_disable(level);
    if(ucRegNum) {
      *port = ucRegNum;
    }
    *port = ucData;
  rtems_interrupt_enable(level);
}
