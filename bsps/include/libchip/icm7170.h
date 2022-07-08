/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  This file contains the definitions for the following real-time clocks:
 *
 *    + Harris Semiconduction ICM7170
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

#ifndef __LIBCHIP_ICM7170_h
#define __LIBCHIP_ICM7170_h

/*
 *  Register indices
 */

#define ICM7170_CONTROL             0x11


#define ICM7170_COUNTER_HUNDREDTHS  0x00
#define ICM7170_HOUR                0x01
#define ICM7170_MINUTE              0x02
#define ICM7170_SECOND              0x03
#define ICM7170_MONTH               0x04
#define ICM7170_DATE                0x05
#define ICM7170_YEAR                0x06
#define ICM7170_DAY_OF_WEEK         0x07

/*
 *  Configuration information in the parameters field
 */

#define ICM7170_AT_32_KHZ  0x00
#define ICM7170_AT_1_MHZ   0x01
#define ICM7170_AT_2_MHZ   0x02
#define ICM7170_AT_4_MHZ   0x03

/*
 *  Driver function table
 */

extern rtc_fns icm7170_fns;

/*
 * Default register access routines
 */

uint32_t   icm7170_get_register(    /* registers are at 1 byte boundaries */
  uintptr_t   ulCtrlPort,           /*   and accessed as bytes            */
  uint8_t     ucRegNum
);

void  icm7170_set_register(
  uintptr_t   ulCtrlPort,
  uint8_t     ucRegNum,
  uint32_t    ucData
);

uint32_t   icm7170_get_register_2(  /* registers are at 2 byte boundaries */
  uintptr_t   ulCtrlPort,           /*   and accessed as bytes            */
  uint8_t     ucRegNum
);

void  icm7170_set_register_2(
  uintptr_t   ulCtrlPort,
  uint8_t     ucRegNum,
  uint32_t    ucData
);

uint32_t   icm7170_get_register_4(  /* registers are at 4 byte boundaries */
  uintptr_t   ulCtrlPort,           /*   and accessed as bytes            */
  uint8_t     ucRegNum
);

void  icm7170_set_register_4(
  uintptr_t   ulCtrlPort,
  uint8_t     ucRegNum,
  uint32_t    ucData
);

uint32_t   icm7170_get_register_8(  /* registers are at 8 byte boundaries */
  uintptr_t   ulCtrlPort,           /*   and accessed as bytes            */
  uint8_t     ucRegNum
);

void  icm7170_set_register_8(
  uintptr_t   ulCtrlPort,
  uint8_t     ucRegNum,
  uint32_t    ucData
);

#endif
/* end of include file */
