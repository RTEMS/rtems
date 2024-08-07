/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *
 * Copyright (C) 2024 Kevin Kirspel
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

/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2008 Altera Corporation, San Jose, California, USA.           *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
*                                                                             *
******************************************************************************/

#ifndef _ALTERA_AVALON_JTAG_UART_REGS_H
#define _ALTERA_AVALON_JTAG_UART_REGS_H

#include <bsp_system.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ALTERA_AVALON_JTAG_UART_DATA_DATA_MSK             (0x000000FFu)
#define ALTERA_AVALON_JTAG_UART_DATA_DATA_OFST            (0)
#define ALTERA_AVALON_JTAG_UART_DATA_RVALID_MSK           (0x00008000u)
#define ALTERA_AVALON_JTAG_UART_DATA_RVALID_OFST          (15)
#define ALTERA_AVALON_JTAG_UART_DATA_RAVAIL_MSK           (0xFFFF0000u)
#define ALTERA_AVALON_JTAG_UART_DATA_RAVAIL_OFST          (16)

#define ALTERA_AVALON_JTAG_UART_CONTROL_RE_MSK            (0x00000001u)
#define ALTERA_AVALON_JTAG_UART_CONTROL_RE_OFST           (0)
#define ALTERA_AVALON_JTAG_UART_CONTROL_WE_MSK            (0x00000002u)
#define ALTERA_AVALON_JTAG_UART_CONTROL_WE_OFST           (1)
#define ALTERA_AVALON_JTAG_UART_CONTROL_RI_MSK            (0x00000100u)
#define ALTERA_AVALON_JTAG_UART_CONTROL_RI_OFST           (8)
#define ALTERA_AVALON_JTAG_UART_CONTROL_WI_MSK            (0x00000200u)
#define ALTERA_AVALON_JTAG_UART_CONTROL_WI_OFST           (9)
#define ALTERA_AVALON_JTAG_UART_CONTROL_AC_MSK            (0x00000400u)
#define ALTERA_AVALON_JTAG_UART_CONTROL_AC_OFST           (10)
#define ALTERA_AVALON_JTAG_UART_CONTROL_WSPACE_MSK        (0xFFFF0000u)
#define ALTERA_AVALON_JTAG_UART_CONTROL_WSPACE_OFST       (16)

typedef struct
{
  volatile uint32_t data;
  volatile uint32_t control;
}altera_avalon_jtag_uart_regs;

#define JTAG_UART_REGS \
  ((volatile altera_avalon_jtag_uart_regs*)JTAG_UART_BASE)

#ifdef __cplusplus
}
#endif

#endif
