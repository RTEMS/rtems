/* SPDX-License-Identifier: BSD-2-Clause */

/*
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

#ifndef _MC68681_H_
#define _MC68681_H_

#include <rtems/termiostypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  These are just used in the interface between this driver and
 *  the read/write register routines when accessing the first
 *  control port.
 */

#define MC68681_STATUS          1
#define MC68681_RX_BUFFER       3

#define MC68681_MODE            0
#define MC68681_CLOCK_SELECT    1
#define MC68681_COMMAND         2
#define MC68681_TX_BUFFER       3

/*
 *  Data Port bit map configuration
 *
 *   D0      : Baud Rate Set Selection
 *   D1 - D2 : Extended Baud Rate Setting
 */

#define MC68681_DATA_BAUD_RATE_SET_1      0  /* ACR[7] = 0 */
#define MC68681_DATA_BAUD_RATE_SET_2      1  /* ACR[7] = 1 */

#define MC68681_XBRG_IGNORED              (0 << 1)
#define MC68681_XBRG_ENABLED              (1 << 1)
#define MC68681_XBRG_DISABLED             (2 << 1)
#define MC68681_XBRG_MASK                 (3 << 1)

/*
 *  Custom baud rate table information
 */

typedef unsigned char mc68681_baud_t;
typedef mc68681_baud_t mc68681_baud_table_t[RTEMS_TERMIOS_NUMBER_BAUD_RATES];

#define MC68681_BAUD_NOT_VALID 0xFF

extern mc68681_baud_t
    mc68681_baud_rate_table[4][RTEMS_TERMIOS_NUMBER_BAUD_RATES];


/*
 * Driver function table
 */

extern const console_fns mc68681_fns;
extern const console_fns mc68681_fns_polled;

/*
 * Default register access routines
 */

uint8_t   mc68681_get_register(     /* registers are at 1 byte boundaries */
  uintptr_t   ulCtrlPort,           /*   and accessed as bytes            */
  uint8_t     ucRegNum
);

void  mc68681_set_register(
  uintptr_t   ulCtrlPort,
  uint8_t     ucRegNum,
  uint8_t     ucData
);

uint8_t   mc68681_get_register_2(   /* registers are at 2 byte boundaries */
  uintptr_t   ulCtrlPort,           /*   and accessed as bytes            */
  uint8_t     ucRegNum
);

void  mc68681_set_register_2(
  uintptr_t   ulCtrlPort,
  uint8_t     ucRegNum,
  uint8_t     ucData
);

uint8_t   mc68681_get_register_4(   /* registers are at 4 byte boundaries */
  uintptr_t   ulCtrlPort,           /*   and accessed as bytes            */
  uint8_t     ucRegNum
);

void  mc68681_set_register_4(
  uintptr_t   ulCtrlPort,
  uint8_t     ucRegNum,
  uint8_t     ucData
);

uint8_t   mc68681_get_register_8(   /* registers are at 8 byte boundaries */
  uintptr_t   ulCtrlPort,           /*   and accessed as bytes            */
  uint8_t     ucRegNum
);

void  mc68681_set_register_8(
  uintptr_t   ulCtrlPort,
  uint8_t     ucRegNum,
  uint8_t     ucData
);


#ifdef __cplusplus
}
#endif

#endif /* _MC68681_H_ */
