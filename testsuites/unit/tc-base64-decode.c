/* SPDX-License-Identifier: BSD-2-Clause */

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

#include <rtems/test.h>

typedef struct {
  Base64_Decode_control base;
  uint8_t buf[64];
} test_control;

static int test_run(test_control* self, const char* payload) {
  size_t len = strlen(payload);

  for (size_t i = 0; i < len; ++i) {
    int rv = _Base64_Decode(&self->base, payload[i]);

    if (rv != 0) {
      return rv;
    }
  }

  return 0;
}

static int test_payload(test_control* self, const char* payload) {
  memset(self->buf, 0xff, sizeof(self->buf));
  _Base64_Decode_initialize(&self->base, &self->buf[0], sizeof(self->buf));
  return test_run(self, payload);
}

T_TEST_CASE(IOBase64Decode) {
  int rv;
  test_control instance;
  test_control* self = &instance;

  rv = test_payload(self, "POOL");
  T_eq_int(rv, BASE64_DECODE_SUCCESS);
  T_eq_int(self->base.state, BASE64_DECODE_STATE_0);
  T_eq_ptr(self->base.target, &self->buf[3]);
  const uint8_t expected1[] = {0x3c, 0xe3, 0x8b, 0xff};
  T_eq_mem(&self->buf[0], expected1, sizeof(expected1));

  rv = test_payload(self, "ABCDEFGH");
  T_eq_int(rv, BASE64_DECODE_SUCCESS);
  T_eq_int(self->base.state, BASE64_DECODE_STATE_0);
  T_eq_ptr(self->base.target, &self->buf[6]);
  const uint8_t expected2[] = {0x00, 0x10, 0x83, 0x10, 0x51, 0x87, 0xff};
  T_eq_mem(&self->buf[0], expected2, sizeof(expected2));

  /* Non-base64 character results in an error */
  rv = test_payload(self, "PO*OL");
  T_eq_int(rv, BASE64_DECODE_INVALID_INPUT);
  T_eq_int(self->base.state, BASE64_DECODE_STATE_2);
  T_eq_ptr(self->base.target, &self->buf[1]);
  const uint8_t expected3[] = {0x3c};
  T_eq_mem(&self->buf[0], expected3, sizeof(expected3));

  /* Other non-base64 character results in an error */
  rv = test_payload(self, "PO\x80OL");
  T_eq_int(rv, BASE64_DECODE_INVALID_INPUT);
  T_eq_int(self->base.state, BASE64_DECODE_STATE_2);
  T_eq_ptr(self->base.target, &self->buf[1]);
  T_eq_mem(&self->buf[0], expected3, sizeof(expected3));

  /* Space characters should be ignored */
  rv = test_payload(self, "P OOL");
  T_eq_int(rv, BASE64_DECODE_SUCCESS);
  T_eq_int(self->base.state, BASE64_DECODE_STATE_0);
  T_eq_ptr(self->base.target, &self->buf[3]);
  const uint8_t expected4[] = {0x3c, 0xe3, 0x8b, 0xff};
  T_eq_mem(&self->buf[0], expected4, sizeof(expected4));

  /* Handle pad characters */
  rv = test_payload(self, "POOL==");
  T_eq_int(rv, BASE64_DECODE_SUCCESS);
  T_eq_int(self->base.state, BASE64_DECODE_STATE_0);
  T_eq_ptr(self->base.target, &self->buf[3]);
  T_eq_ptr(self->base.target, self->base.target_end);
  const uint8_t expected5[] = {0x3c, 0xe3, 0x8b, 0xff};
  T_eq_mem(&self->buf[0], expected5, sizeof(expected5));

  /* If characters come after pad characters, an error results */
  rv = test_payload(self, "POOL==xy");
  T_eq_int(rv, BASE64_DECODE_OVERFLOW);
  T_eq_int(self->base.state, BASE64_DECODE_SUCCESS);
  T_eq_ptr(self->base.target, &self->buf[3]);
  T_eq_ptr(self->base.target, self->base.target_end);
  const uint8_t expected6[] = {0x3c, 0xe3, 0x8b, 0xff};
  T_eq_mem(&self->buf[0], expected6, sizeof(expected6));

  rv = test_payload(self, "POOLPOOL");
  T_eq_int(rv, BASE64_DECODE_SUCCESS);
  T_eq_int(self->base.state, BASE64_DECODE_STATE_0);
  T_eq_ptr(self->base.target, &self->buf[6]);
  const uint8_t expected7[] = {0x3c, 0xe3, 0x8b, 0x3c, 0xe3, 0x8b, 0xff};
  T_eq_mem(&self->buf[0], expected7, sizeof(expected7));

  /*
   * Test valid payload with series of target sizes. All target sizes
   * less than three are invalid for the given payload and will result
   * in an error.
   */
  const uint8_t expected9[] = {0x3c, 0xe3, 0x8b, 0xff};

  for (size_t i = 0; i < 4; ++i) {
    memset(&self->buf[0], 0xff, sizeof(self->buf));
    _Base64_Decode_initialize(&self->base, &self->buf[0], i);
    rv = test_run(self, "POOL");

    if (i < 3) {
      T_eq_int(rv, BASE64_DECODE_OVERFLOW);
      T_eq_int(self->base.state, i);
      T_ne_ptr(self->base.target, &self->buf[3]);
      T_ne_mem(&self->buf[0], expected9, sizeof(expected9));
    } else {
      T_eq_int(rv, BASE64_DECODE_SUCCESS);
      T_eq_int(self->base.state, BASE64_DECODE_STATE_0);
      T_eq_ptr(self->base.target, &self->buf[3]);
      T_eq_mem(&self->buf[0], expected9, sizeof(expected9));
    }
  }

  /* No overflow in state 1 */
  memset(&self->buf[0], 0xff, sizeof(self->buf));
  _Base64_Decode_initialize(&self->base, &self->buf[0], 1);
  rv = test_run(self, "AA");
  T_eq_int(rv, BASE64_DECODE_SUCCESS);
  T_eq_int(self->base.state, BASE64_DECODE_STATE_2);
  T_eq_ptr(self->base.target, &self->buf[1]);
  const uint8_t expected10[] = {0x00, 0xff};
  T_eq_mem(&self->buf[0], expected10, sizeof(expected10));

  /* No overflow in state 2 */
  memset(&self->buf[0], 0xff, sizeof(self->buf));
  _Base64_Decode_initialize(&self->base, &self->buf[0], 2);
  rv = test_run(self, "AAA");
  T_eq_int(rv, BASE64_DECODE_SUCCESS);
  T_eq_int(self->base.state, BASE64_DECODE_STATE_3);
  T_eq_ptr(self->base.target, &self->buf[2]);
  const uint8_t expected11[] = {0x00, 0x00, 0xff};
  T_eq_mem(&self->buf[0], expected11, sizeof(expected11));
}

T_TEST_CASE(IOBase64DecodeInitialize) {
  Base64_Decode_control instance;
  Base64_Decode_control* self = &instance;
  uint8_t buf[1];

  memset(self, 0xff, sizeof(*self));
  _Base64_Decode_initialize(self, buf, sizeof(buf));
  T_eq_int(self->state, BASE64_DECODE_STATE_0);
  T_eq_ptr(self->target, &buf[0]);
  T_eq_ptr(self->target_end, &buf[1]);
}
