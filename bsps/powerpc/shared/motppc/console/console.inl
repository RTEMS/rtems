/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMotorola
 *
 * @brief Console I/O routines.
 */

/*
 * Copyright (C) 2001 Till Straumann <strauman@slac.stanford.edu>
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

/* inline routines for console i/o
 *
 * The purpose of this file is to provide generic inline functions,
 * i.e. not using hardcoded base addresses. These are provided by
 * the BSP header.
 */

#include <bsp.h>
#include <stdint.h>

#define INL_IN_DECL(name,base) \
static inline unsigned char name(int off) \
{ \
	return in_8((uint8_t*)(((unsigned long)base) + BSP_REG_OFF(off))); \
}

#define INL_OUT_DECL(name,base) \
static inline void name(int off, unsigned int val) \
{ \
	out_8((uint8_t*)(((unsigned long)base) + BSP_REG_OFF(off)), val); \
}

#ifdef BSP_UART_IOBASE_COM1
INL_IN_DECL(com1_inb,	BSP_UART_IOBASE_COM1)
INL_OUT_DECL(com1_outb,	BSP_UART_IOBASE_COM1)
#endif
#ifdef BSP_UART_IOBASE_COM2
INL_IN_DECL(com2_inb,	BSP_UART_IOBASE_COM2)
INL_OUT_DECL(com2_outb,	BSP_UART_IOBASE_COM2)
#endif

#if defined(BSP_CONSOLE_PORT)
#if   (BSP_CONSOLE_PORT == BSP_UART_COM1) && defined(BSP_UART_IOBASE_COM1)
#define INL_CONSOLE_INB		com1_inb
#define INL_CONSOLE_OUTB	com1_outb
#elif (BSP_CONSOLE_PORT == BSP_UART_COM2) && defined(BSP_UART_IOBASE_COM2)
#define INL_CONSOLE_INB		com2_inb
#define INL_CONSOLE_OUTB	com2_outb
#endif
#endif

#ifdef BSP_KBD_IOBASE
INL_IN_DECL(kbd_inb,	BSP_KBD_IOBASE)
INL_OUT_DECL(kbd_outb,	BSP_KBD_IOBASE)
#endif

#ifdef BSP_VGA_IOBASE
INL_OUT_DECL(vga_outb,  BSP_VGA_IOBASE)
#endif
