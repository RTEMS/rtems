/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief Zero the Various BSS Areas
 */

/*
 * Copyright (c) 1989-2014 On-Line Applications Research Corporation (OAR).
 *
 * Modified to support the MCP750.
 * Modifications Copyright (c) 1999 Eric Valette <eric.valette@free.fr>
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

#include <string.h>
#include <bsp.h>

/* prevent these from being accessed in the short data areas */
extern unsigned long __bss_start[], __SBSS_START__[], __SBSS_END__[];
extern unsigned long __SBSS2_START__[], __SBSS2_END__[];
extern unsigned long __bss_end[];

void zero_bss(void)
{
  memset(
    __SBSS_START__,
    0,
    ((unsigned) __SBSS_END__) - ((unsigned)__SBSS_START__)
  );
  memset(
    __SBSS2_START__,
    0,
    ((unsigned) __SBSS2_END__) - ((unsigned)__SBSS2_START__)
  );
  memset(
    __bss_start,
    0,
    ((unsigned) __bss_end) - ((unsigned)__bss_start)
  );
}
