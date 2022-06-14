/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  This file contains a typical set of register access routines which may be
 *  used with the MC146818A chip if accesses to the chip are as follows:
 *
 *    + registers are in I/O space
 *    + registers are accessed as bytes
 *    + registers are only byte-aligned (no address gaps)
 */

/*
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
#include <bsp.h>
#include <libchip/rtc.h>
#include <libchip/mc146818a.h>

/*
 *  At this point, not all CPUs or BSPs have defined in/out port routines.
 */
#if defined(__i386__) || defined(__PPC__)
#if defined(inport_byte)
uint32_t mc146818a_get_register(
  uintptr_t  ulCtrlPort,
  uint8_t    ucRegNum
)
{
  uint8_t   val;
  uint8_t   tmp;

  (void) tmp;                 /* eliminate warning for set but not used */

  outport_byte( ulCtrlPort, ucRegNum );
  inport_byte( 0x84, tmp );   /* Hack a delay to give chip time to settle */
  inport_byte( ulCtrlPort+1, val );
  inport_byte( 0x84, tmp );   /* Hack a delay to give chip time to settle */
  return val;
}

void  mc146818a_set_register(
  uintptr_t  ulCtrlPort,
  uint8_t    ucRegNum,
  uint32_t   ucData
)
{
  outport_byte( ulCtrlPort, ucRegNum );
  outport_byte( ulCtrlPort+1, (uint8_t)ucData );
}
#endif
#endif
