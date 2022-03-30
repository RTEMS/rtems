/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Convert Bxxx Constant to Index
 *  @ingroup TermiostypesSupport
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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

#include <termios.h>
#include <rtems/termiostypes.h>

int rtems_termios_baud_to_index(
  rtems_termios_baud_t termios_baud
)
{
  int baud_index;

  switch (termios_baud) {
    case B0:        baud_index =  0;  break;
    case B50:       baud_index =  1;  break;
    case B75:       baud_index =  2;  break;
    case B110:      baud_index =  3;  break;
    case B134:      baud_index =  4;  break;
    case B150:      baud_index =  5;  break;
    case B200:      baud_index =  6;  break;
    case B300:      baud_index =  7;  break;
    case B600:      baud_index =  8;  break;
    case B1200:     baud_index =  9;  break;
    case B1800:     baud_index = 10;  break;
    case B2400:     baud_index = 11;  break;
    case B4800:     baud_index = 12;  break;
    case B9600:     baud_index = 13;  break;
    case B19200:    baud_index = 14;  break;
    case B38400:    baud_index = 15;  break;
    case B7200:     baud_index = 16;  break;
    case B14400:    baud_index = 17;  break;
    case B28800:    baud_index = 18;  break;
    case B57600:    baud_index = 19;  break;
    case B76800:    baud_index = 20;  break;
    case B115200:   baud_index = 21;  break;
    case B230400:   baud_index = 22;  break;
    case B460800:   baud_index = 23;  break;
    case B921600:   baud_index = 24;  break;
    default:        baud_index = -1;  break;
  }

  return baud_index;
}
