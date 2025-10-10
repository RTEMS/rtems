/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  This file contains the hardware specific portions of the TTY driver
 *  for the serial ports on the jmr3904.
 *
 *  Logic based on the jmr3904-io.c file in newlib 1.8.2
 */

/*
 *  COPYRIGHT (c) 1989-2000.
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

#include <bsp.h>
#include <bsp/console-polled.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>

/* external prototypes for monitor interface routines */

#define READ_UINT8( _register_, _value_ ) \
        ((_value_) = *((volatile unsigned char *)(_register_)))

#define WRITE_UINT8( _register_, _value_ ) \
        (*((volatile unsigned char *)(_register_)) = (_value_))

#define READ_UINT16( _register_, _value_ ) \
     ((_value_) = *((volatile unsigned short *)(_register_)))

#define WRITE_UINT16( _register_, _value_ ) \
     (*((volatile unsigned short *)(_register_)) = (_value_))

 /* - Board specific addresses for serial chip */
#define DIAG_BASE       0xfffff300
#define DIAG_SLCR       (DIAG_BASE+0x00)
#define DIAG_SLSR       (DIAG_BASE+0x04)
#define DIAG_SLDICR     (DIAG_BASE+0x08)
#define DIAG_SLDISR     (DIAG_BASE+0x0C)
#define DIAG_SFCR       (DIAG_BASE+0x10)
#define DIAG_SBRG       (DIAG_BASE+0x14)
#define DIAG_TFIFO      (DIAG_BASE+0x20)
#define DIAG_RFIFO      (DIAG_BASE+0x30)

#define BRG_T0          0x0000
#define BRG_T2          0x0100
#define BRG_T4          0x0200
#define BRG_T5          0x0300

/*
 *  Eventually console-polled.c should hook to this better.
 */

/*
 *  console_initialize_hardware
 *
 *  This routine initializes the console hardware.
 *
 */

void console_initialize_hardware(void)
{
  WRITE_UINT16 (DIAG_SLCR, 0x0020);
  WRITE_UINT16 (DIAG_SLDICR, 0x0000);
  WRITE_UINT16 (DIAG_SFCR, 0x0000);
  WRITE_UINT16 (DIAG_SBRG, BRG_T2 | 5);
}

/*
 *  console_outbyte_polled
 *
 *  This routine transmits a character using polling.
 */

void console_outbyte_polled(
  int  port,
  char ch
)
{
  (void) port;

  unsigned short disr;

  for (;;) {
    READ_UINT16 (DIAG_SLDISR, disr);
    if (disr & 0x0002)
      break;
  }
  disr = disr & ~0x0002;
  WRITE_UINT8 (DIAG_TFIFO, (unsigned char) ch);
  WRITE_UINT16 (DIAG_SLDISR, disr);
}

/*
 *  console_inbyte_nonblocking
 *
 *  This routine polls for a character.
 */

int console_inbyte_nonblocking(
  int port
)
{
  (void) port;

  unsigned char c;
  unsigned short disr;

  READ_UINT16 (DIAG_SLDISR, disr);
  if (disr & 0x0001) {
    disr = disr & ~0x0001;
    READ_UINT8 (DIAG_RFIFO, c);
    WRITE_UINT16 (DIAG_SLDISR, disr);
    return (char) c;
  }
  return -1;
}

#include <rtems/bspIo.h>

static void JMR3904_output_char(char c) { console_outbyte_polled( 0, c ); }

BSP_output_char_function_type           BSP_output_char = JMR3904_output_char;
BSP_polling_getchar_function_type       BSP_poll_char = NULL;
