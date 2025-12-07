/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Console driver for GUMSTIX
 *
 *  This driver uses the shared console driver in
 *  bsps/shared/dev/serial/legacy-console.c
 */

/*
 * Copyright (C) 2008 Yang Xi <hiyangxi@gmail.com>
 * Copyright (C) 2003 by Cogent Computer Systems
 * Written by Jay Monkman <jtm@lopingdog.com>
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

#include <bsp.h>
#include <rtems/libio.h>
#include <termios.h>
#include <rtems/bspIo.h>

#include <pxa255.h>
#include <libchip/serial.h>
#include <libchip/sersupp.h>

/* How many serial ports? */
#define NUM_DEVS       1

/* These are used by code in console.c */
unsigned long Console_Configuration_Count = NUM_DEVS;

extern const console_fns ffuart_fns;

/*
 * There's one item in array for each UART.
 *
 * Some of these fields are marked "NOT USED". They are not used
 * by console.c, but may be used by drivers in libchip
 *
 * when we add other types of UARTS we will need to move this
 * structure to a generic uart.c file with only this in it
 */
console_tbl Console_Configuration_Ports[] = {
    {
        "/dev/com0",       /* sDeviceName */
        SERIAL_CUSTOM,     /* deviceType */
        &ffuart_fns,       /* pDeviceFns */
        NULL,              /* deviceProbe */
        NULL,              /* pDeviceFlow */
        0,                 /* ulMargin - NOT USED */
        0,                 /* ulHysteresis - NOT USED */
        NULL,              /* pDeviceParams */
        FFUART_BASE,       /* ulCtrlPort1  - Pointer to DBGU regs */
        0,                 /* ulCtrlPort2  - NOT USED */
        0,                 /* ulDataPort  - NOT USED */
        NULL,              /* getRegister - NOT USED */
        NULL,              /* setRegister - NOT USED */
        NULL,              /* getData - NOT USED */
        NULL,              /* setData - NOT USED */
        0,                 /* ulClock - NOT USED */
        0                  /* ulIntVector - NOT USED */
    }};


console_tbl *BSP_get_uart_from_minor(int minor)
{
    return Console_Port_Tbl[minor];
}
