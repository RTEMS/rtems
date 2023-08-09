/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplBase64
 *
 * @brief This source file contains the implementation of
 *   _Base64_Decode_initialize() and _Base64_Decode().
 */

/*
 * Copyright (C) 2023 embedded brains GmbH & Co. KG
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

#include <rtems/base64.h>

#define SPACE 253

#define PAD 254

#define INVALID 255

const uint8_t _Base64_Decoding[128] = {
    ['A'] = 0,       ['B'] = 1,       ['C'] = 2,       ['D'] = 3,
    ['E'] = 4,       ['F'] = 5,       ['G'] = 6,       ['H'] = 7,
    ['I'] = 8,       ['J'] = 9,       ['K'] = 10,      ['L'] = 11,
    ['M'] = 12,      ['N'] = 13,      ['O'] = 14,      ['P'] = 15,
    ['Q'] = 16,      ['R'] = 17,      ['S'] = 18,      ['T'] = 19,
    ['U'] = 20,      ['V'] = 21,      ['W'] = 22,      ['X'] = 23,
    ['Y'] = 24,      ['Z'] = 25,      ['a'] = 26,      ['b'] = 27,
    ['c'] = 28,      ['d'] = 29,      ['e'] = 30,      ['f'] = 31,
    ['g'] = 32,      ['h'] = 33,      ['i'] = 34,      ['j'] = 35,
    ['k'] = 36,      ['l'] = 37,      ['m'] = 38,      ['n'] = 39,
    ['o'] = 40,      ['p'] = 41,      ['q'] = 42,      ['r'] = 43,
    ['s'] = 44,      ['t'] = 45,      ['u'] = 46,      ['v'] = 47,
    ['w'] = 48,      ['x'] = 49,      ['y'] = 50,      ['z'] = 51,
    ['0'] = 52,      ['1'] = 53,      ['2'] = 54,      ['3'] = 55,
    ['4'] = 56,      ['5'] = 57,      ['6'] = 58,      ['7'] = 59,
    ['8'] = 60,      ['9'] = 61,      ['+'] = 62,      ['-'] = 62,
    ['/'] = 63,      ['_'] = 63,      ['='] = PAD,     [' '] = SPACE,
    ['\t'] = SPACE,  ['\n'] = SPACE,  ['\v'] = SPACE,  ['\f'] = SPACE,
    ['\r'] = SPACE,  [0] = INVALID,   [1] = INVALID,   [2] = INVALID,
    [3] = INVALID,   [4] = INVALID,   [5] = INVALID,   [6] = INVALID,
    [7] = INVALID,   [8] = INVALID,   [14] = INVALID,  [15] = INVALID,
    [16] = INVALID,  [17] = INVALID,  [18] = INVALID,  [19] = INVALID,
    [20] = INVALID,  [21] = INVALID,  [22] = INVALID,  [23] = INVALID,
    [24] = INVALID,  [25] = INVALID,  [26] = INVALID,  [27] = INVALID,
    [28] = INVALID,  [29] = INVALID,  [30] = INVALID,  [31] = INVALID,
    [33] = INVALID,  [34] = INVALID,  [35] = INVALID,  [36] = INVALID,
    [37] = INVALID,  [38] = INVALID,  [39] = INVALID,  [40] = INVALID,
    [41] = INVALID,  [42] = INVALID,  [44] = INVALID,  [46] = INVALID,
    [58] = INVALID,  [59] = INVALID,  [60] = INVALID,  [62] = INVALID,
    [63] = INVALID,  [64] = INVALID,  [91] = INVALID,  [92] = INVALID,
    [93] = INVALID,  [94] = INVALID,  [96] = INVALID,  [123] = INVALID,
    [124] = INVALID, [125] = INVALID, [126] = INVALID, [127] = INVALID};

void _Base64_Decode_initialize(Base64_Decode_control* self,
                               uint8_t* target,
                               size_t target_size) {
  self->state = BASE64_DECODE_STATE_0;
  self->target = target;
  self->target_end = target + target_size;
}

Base64_Decode_status _Base64_Decode(Base64_Decode_control* self, char ch) {
  uint8_t decoded_ch;
  uint8_t next_ch;
  uint8_t* target;
  const uint8_t* target_end;
  Base64_Decode_state next_state;

  if ((unsigned char)ch >= 128) {
    return BASE64_DECODE_INVALID_INPUT;
  }

  decoded_ch = _Base64_Decoding[(unsigned char)ch];

  if (decoded_ch == SPACE) {
    return BASE64_DECODE_SUCCESS;
  }

  target = self->target;

  if (decoded_ch == PAD) {
    self->target_end = target;
    return BASE64_DECODE_SUCCESS;
  }

  if (decoded_ch == INVALID) {
    return BASE64_DECODE_INVALID_INPUT;
  }

  target_end = self->target_end;

  if (target == target_end) {
    return BASE64_DECODE_OVERFLOW;
  }

  switch (self->state) {
    case BASE64_DECODE_STATE_0:
      *target = decoded_ch << 2;
      next_state = BASE64_DECODE_STATE_1;
      break;

    case BASE64_DECODE_STATE_1:
      *target |= decoded_ch >> 4;
      next_ch = (decoded_ch & 0x0fU) << 4;
      ++target;

      if (target != target_end) {
        *target = next_ch;
      } else if (next_ch != 0) {
        return BASE64_DECODE_OVERFLOW;
      }

      next_state = BASE64_DECODE_STATE_2;
      break;

    case BASE64_DECODE_STATE_2:
      *target |= decoded_ch >> 2;
      next_ch = (decoded_ch & 0x03U) << 6;
      ++target;

      if (target != target_end) {
        *target = next_ch;
      } else if (next_ch != 0) {
        return BASE64_DECODE_OVERFLOW;
      }

      next_state = BASE64_DECODE_STATE_3;
      break;

    default: /* BASE64_DECODE_STATE_3 */
      *target |= decoded_ch;
      ++target;
      next_state = BASE64_DECODE_STATE_0;
      break;
  }

  self->state = next_state;
  self->target = target;
  return BASE64_DECODE_SUCCESS;
}
