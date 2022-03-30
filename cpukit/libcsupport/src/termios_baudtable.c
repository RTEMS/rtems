/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief  RTEMS Termios Baud Table
 *  @ingroup TermiostypesSupport
 */

/*
 *  COPYRIGHT (c) 1989-2010.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/termiostypes.h>

const rtems_assoc_t rtems_termios_baud_table [] = {
  { "B0",      0,      B0 },
  { "B50",     50,     B50 },
  { "B75",     75,     B75 },
  { "B110",    110,    B110 },
  { "B134",    134,    B134 },
  { "B150",    150,    B150 },
  { "B200",    200,    B200 },
  { "B300",    300,    B300 },
  { "B600",    600,    B600 },
  { "B1200",   1200,   B1200 },
  { "B1800",   1800,   B1800 },
  { "B2400",   2400,   B2400 },
  { "B4800",   4800,   B4800 },
  { "B9600",   9600,   B9600 },
  { "B19200",  19200,  B19200 },
  { "B38400",  38400,  B38400 },
  { "B7200",   7200,   B7200 },
  { "B14400",  14400,  B14400 },
  { "B28800",  28800,  B28800 },
  { "B57600",  57600,  B57600 },
  { "B76800",  76800,  B76800 },
  { "B115200", 115200, B115200 },
  { "B230400", 230400, B230400 },
  { "B460800", 460800, B460800 },
  { "B921600", 921600, B921600 },
  { NULL,      0,      0 }
};
