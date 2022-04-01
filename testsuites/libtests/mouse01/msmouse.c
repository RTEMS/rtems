/* SPDX-License-Identifier: BSD-2-Clause */

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>

const char *Mouse_Type_Long = "Microsoft Mouse";
const char *Mouse_Type_Short = "ms";

#define PRESSED     1 
#define NOT_PRESSED 0 

#define MS_MOUSE_BYTES( _lb, _rb, _x, _y ) \
  (0x40 | ((_lb) ? 0x20 : 0x00) | ((_rb) ? 0x10 : 0x00) | \
     ((_y & 0xC) >> 4) | (_x >> 6)), \
  (_x & 0x3F), \
  (_y & 0x3F)

const unsigned char Mouse_Actions[] = {
  MS_MOUSE_BYTES(     PRESSED, NOT_PRESSED,    1,    2),
  MS_MOUSE_BYTES( NOT_PRESSED, PRESSED,        1,    2),
  MS_MOUSE_BYTES( NOT_PRESSED, NOT_PRESSED, 0xff,    2),
  MS_MOUSE_BYTES( NOT_PRESSED, NOT_PRESSED,    1, 0xff)
};

const size_t Mouse_Actions_Size = sizeof(Mouse_Actions);
const size_t Mouse_Actions_Per_Iteration = 3;

