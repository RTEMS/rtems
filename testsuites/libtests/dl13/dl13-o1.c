/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2024 On-Line Applications Research (OAR). All rights reserved.
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

#include "dl13-arm_mode.h"

#include <rtems/test-printer.h>

#define printf(...) rtems_printf(&rtems_test_printer, __VA_ARGS__);

/*
 * Yes a decl in the source. This is a modules main and I could not find which
 * header main is defined in.
 */
void dl_o1_func (void);
void dl_o1_func (void)
{
  printf("About to call arm_mode_func from %s\n", __func__);

  /*
   * Non-trailing calls to arm_mode_func generate an offset prefix and BLX via
   * THM_PC22
   */
  arm_mode_func();

  printf("About to call arm_mode_func from %s as a trailing call\n", __func__);

  /*
   * Trailing calls to arm_mode_func generate a b.w instruction via THM_JUMP24
   * relocation
   */
  arm_mode_func();

  return;
}
