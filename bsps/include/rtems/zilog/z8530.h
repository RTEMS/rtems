/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Information Related to a Zilog Z8530 SCC Chip
 *
 * This include file defines information related to a Zilog Z8530
 * SCC Chip.  It is a IO mapped part.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
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

#ifndef _RTEMS_ZILOG_Z8530_H
#define _RTEMS_ZILOG_Z8530_H

#ifdef __cplusplus
extern "C" {
#endif

/* macros */

#define VOL8( ptr )   ((volatile uint8_t   *)(ptr))

#define Z8x30_STATE0 ( z8530 ) \
  { char *garbage; \
    (garbage) = *(VOL8(z8530)) \
  }

#define Z8x30_WRITE_CONTROL( z8530, reg, data ) \
   *(VOL8(z8530)) = (reg); \
   *(VOL8(z8530)) = (data)

#define Z8x30_READ_CONTROL( z8530, reg, data ) \
   *(VOL8(z8530)) = (reg); \
   (data) = *(VOL8(z8530))

#define Z8x30_WRITE_DATA( z8530, data ) \
   *(VOL8(z8530)) = (data);

#define Z8x30_READ_DATA( z8530, data ) \
   (data) = *(VOL8(z8530));


/* RR_0 Bit Definitions */

#define RR_0_TX_BUFFER_EMPTY   0x04
#define RR_0_RX_DATA_AVAILABLE 0x01

/* read registers */

#define RR_0       0x00
#define RR_1       0x01
#define RR_2       0x02
#define RR_3       0x03
#define RR_4       0x04
#define RR_5       0x05
#define RR_6       0x06
#define RR_7       0x07
#define RR_8       0x08
#define RR_9       0x09
#define RR_10      0x0A
#define RR_11      0x0B
#define RR_12      0x0C
#define RR_13      0x0D
#define RR_14      0x0E
#define RR_15      0x0F

/* write registers */

#define WR_0       0x00
#define WR_1       0x01
#define WR_2       0x02
#define WR_3       0x03
#define WR_4       0x04
#define WR_5       0x05
#define WR_6       0x06
#define WR_7       0x07
#define WR_8       0x08
#define WR_9       0x09
#define WR_10      0x0A
#define WR_11      0x0B
#define WR_12      0x0C
#define WR_13      0x0D
#define WR_14      0x0E
#define WR_15      0x0F

#ifdef __cplusplus
}
#endif

#endif
