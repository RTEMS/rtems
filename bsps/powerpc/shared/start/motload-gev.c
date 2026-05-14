/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2026 Chris Johns <chrisj@rtems.org>.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <bsp/motload-gev.h>

int motload_gev_get(uintptr_t nvram_base, const char* name, char* var,
                    const size_t var_len) {
  /*
   * The MOTLoad requires 8K bytes at the top end of NVRAM. The amount
   * of space set aside in the NVRAM for storage of GEVs is 3592
   * bytes. The offset into the NVRAM is 0x70f8.
   */
  volatile char* gev = (volatile char*)nvram_base + 0x70f8;
  const size_t gev_len = 3592;
  char state = 's';
  size_t g = 0;
  size_t n = 0;
  size_t v = 0;
  memset(var, 0, var_len);
  while (g < gev_len) {
    switch (state) {
    case 's':
      /* Search for the start of the name */
      if (gev[g] == '\0') {
        return -1;
      }
      if (gev[g] != name[0]) {
        ++g;
        break;
      }
      state = 'n';
      n = 0;
      RTEMS_FALL_THROUGH();
    case 'n':
      /* Match the name */
      if (name[n] == 0) {
        if (gev[g] == '=') {
          v = 0;
          state = 'v';
        }
      } else if (gev[g] == name[n]) {
        ++n;
      } else {
        state = 'z';
      }
      ++g;
      break;
    case 'v':
      /* Copy the variable */
      var[v] = gev[g];
      if (gev[g] == '\0') {
        return v;
      }
      ++v;
      if (v == var_len - 1) {
        return v;
      }
      ++g;
      break;
    case 'z':
      /* Drop data until the next nul character */
      if (gev[g] == '\0') {
        state = 's';
      }
      ++g;
      break;
    }
  }
  return -1;
}
