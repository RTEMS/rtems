/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief console I/O package interface
 */

/*
 * Copyright (c) 1999 Eric Valette <eric.valette@free.fr>
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

#ifndef __CONSOLE_IO_H
#define __CONSOLE_IO_H

typedef enum {
  CONSOLE_LOG 		= 1,
  CONSOLE_SERIAL       	= 2,
  CONSOLE_VGA		= 3,
  CONSOLE_VACUUM	= 4
}ioType;

typedef volatile unsigned char * __io_ptr;

typedef struct {
  __io_ptr io_base;
  __io_ptr isa_mem_base;
} board_memory_map;

extern board_memory_map *ptr_mem_map;

extern int select_console(ioType t);
/* extern int printk(const char *, ...) __attribute__((format(printf, 1, 2))); */
extern void debug_putc(const char c);
extern void debug_putc_onlcr(const char c);
extern int debug_getc(void);
extern int debug_tstc(void);
int kbdreset(void);

#endif
