/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  This file contains the definitions for the following real-time clocks:
 *
 *    + Mostek M48T08
 *    + Mostek M48T18
 *    + Dallas Semiconductor DS1643
 *
 *  COPYRIGHT (c) 1989-1999.
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

#ifndef __LIBCHIP_M48T08_h
#define __LIBCHIP_M48T08_h

/*
 *  Register indices
 */

#define M48T08_CONTROL     0
#define M48T08_SECOND      1
#define M48T08_MINUTE      2
#define M48T08_HOUR        3
#define M48T08_DAY_OF_WEEK 4
#define M48T08_DATE        5
#define M48T08_MONTH       6
#define M48T08_YEAR        7

/*
 *  Driver function table
 */

extern rtc_fns m48t08_fns;

/*
 * Default register access routines
 */

uint32_t   m48t08_get_register(     /* registers are at 1 byte boundaries */
  uintptr_t   ulCtrlPort,           /*   and accessed as bytes            */
  uint8_t     ucRegNum
);

void  m48t08_set_register(
  uintptr_t   ulCtrlPort,
  uint8_t     ucRegNum,
  uint32_t    ucData
);

uint32_t   m48t08_get_register_2(   /* registers are at 2 byte boundaries */
  uintptr_t   ulCtrlPort,           /*   and accessed as bytes            */
  uint8_t     ucRegNum
);

void  m48t08_set_register_2(
  uintptr_t   ulCtrlPort,
  uint8_t     ucRegNum,
  uint32_t    ucData
);

uint32_t   m48t08_get_register_4(   /* registers are at 4 byte boundaries */
  uintptr_t   ulCtrlPort,           /*   and accessed as bytes            */
  uint8_t     ucRegNum
);

void  m48t08_set_register_4(
  uintptr_t   ulCtrlPort,
  uint8_t     ucRegNum,
  uint32_t    ucData
);

uint32_t   m48t08_get_register_8(   /* registers are at 8 byte boundaries */
  uintptr_t   ulCtrlPort,           /*   and accessed as bytes            */
  uint8_t     ucRegNum
);

void  m48t08_set_register_8(
  uintptr_t   ulCtrlPort,
  uint8_t     ucRegNum,
  uint32_t    ucData
);

#endif
/* end of include file */
