/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Console driver for for KIT637_V6 (CSB637)
 *
 *  This driver uses the shared console driver in
 *  bsps/shared/dev/serial/legacy-console.c
 */

/*
 * Copyright (C) 2003 Cogent Computer Systems
 *  Written by Jay Monkman <jtm@lopingdog.com>
 *
 * Modified by Fernando Nicodemos <fgnicodemos@terra.com.br>
 * from NCB - Sistemas Embarcados Ltda. (Brazil)
 *
 *  Modified and FrameBuffer Console Device Support added by
 *  Joel Sherrill, 2009.
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

#include <at91rm9200.h>
#include <at91rm9200_dbgu.h>
#include <libchip/serial.h>
#include <libchip/sersupp.h>
#include <bspopts.h>

extern const console_fns dbgu_fns;

#if ENABLE_LCD
  extern const console_fns fbcons_fns;
  #define LCD_DEV 1
#else
  #define LCD_DEV 0
#endif

#if (ENABLE_UMON && ENABLE_UMON_CONSOLE)
  extern const console_fns umoncons_fns;
  #define UMON_CONS_DEV 1
#else
  #define UMON_CONS_DEV 0
#endif

#if ENABLE_USART0 || ENABLE_USART1 || ENABLE_USART2 || ENABLE_USART3
  extern const console_fns usart_polling_fns;
#endif

#if ENABLE_USART0
  #define USART0_DEV 1
#else
  #define USART0_DEV 0
#endif

#if ENABLE_USART1
  #define USART1_DEV 1
#else
  #define USART1_DEV 0
#endif

#if ENABLE_USART2
  #define USART2_DEV 1
#else
  #define USART2_DEV 0
#endif

#if ENABLE_USART3
  #define USART3_DEV 1
#else
  #define USART3_DEV 0
#endif

#define NUM_DEVS \
  (1 + LCD_DEV + UMON_CONS_DEV + \
  USART0_DEV + USART1_DEV + USART2_DEV + USART3_DEV)

/* These are used by code in console.c */
unsigned long Console_Configuration_Count = NUM_DEVS;

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
    &dbgu_fns,         /* pDeviceFns */
    NULL,              /* deviceProbe */
    NULL,              /* pDeviceFlow */
    0,                 /* ulMargin - NOT USED */
    0,                 /* ulHysteresis - NOT USED */
    NULL,              /* pDeviceParams */
    DBGU_BASE,         /* ulCtrlPort1  - Pointer to DBGU regs */
    0,                 /* ulCtrlPort2  - NOT USED */
    0,                 /* ulDataPort  - NOT USED */
    NULL,              /* getRegister - NOT USED */
    NULL,              /* setRegister - NOT USED */
    NULL,              /* getData - NOT USED */
    NULL,              /* setData - NOT USED */
    0,                 /* ulClock - NOT USED */
    0                  /* ulIntVector - NOT USED */
  },
#if ENABLE_LCD
  {
    "/dev/fbcons",     /* sDeviceName */
    SERIAL_CUSTOM,     /* deviceType */
    &fbcons_fns,       /* pDeviceFns */
    NULL,              /* deviceProbe */
    NULL,              /* pDeviceFlow */
    0,                 /* ulMargin - NOT USED */
    0,                 /* ulHysteresis - NOT USED */
    NULL,              /* pDeviceParams */
    0,                 /* ulCtrlPort1  - Pointer to DBGU regs */
    0,                 /* ulCtrlPort2  - NOT USED */
    0,                 /* ulDataPort  - NOT USED */
    NULL,              /* getRegister - NOT USED */
    NULL,              /* setRegister - NOT USED */
    NULL,              /* getData - NOT USED */
    NULL,              /* setData - NOT USED */
    0,                 /* ulClock - NOT USED */
    0                  /* ulIntVector - NOT USED */
  },
#endif
#if (ENABLE_UMON && ENABLE_UMON_CONSOLE)
  {
    "/dev/umon",       /* sDeviceName */
    SERIAL_CUSTOM,     /* deviceType */
    &umoncons_fns,     /* pDeviceFns */
    NULL,              /* deviceProbe */
    NULL,              /* pDeviceFlow */
    0,                 /* ulMargin - NOT USED */
    0,                 /* ulHysteresis - NOT USED */
    NULL,              /* pDeviceParams */
    0,                 /* ulCtrlPort1  - Pointer to UMON regs */
    0,                 /* ulCtrlPort2  - NOT USED */
    0,                 /* ulDataPort  - NOT USED */
    NULL,              /* getRegister - NOT USED */
    NULL,              /* setRegister - NOT USED */
    NULL,              /* getData - NOT USED */
    NULL,              /* setData - NOT USED */
    0,                 /* ulClock - NOT USED */
    0                  /* ulIntVector - NOT USED */
  },
#endif
#if ENABLE_USART0
  {
    "/dev/com1",       /* sDeviceName */
    SERIAL_CUSTOM,     /* deviceType */
    &usart_polling_fns,/* pDeviceFns */
    NULL,              /* deviceProbe */
    NULL,              /* pDeviceFlow */
    0,                 /* ulMargin - NOT USED */
    0,                 /* ulHysteresis - NOT USED */
    NULL,              /* pDeviceParams */
    USART0_BASE,       /* ulCtrlPort1  - Pointer to USART 0 regs */
    0,                 /* ulCtrlPort2  - NOT USED */
    0,                 /* ulDataPort  - NOT USED */
    NULL,              /* getRegister - NOT USED */
    NULL,              /* setRegister - NOT USED */
    NULL,              /* getData - NOT USED */
    NULL,              /* setData - NOT USED */
    0,                 /* ulClock - NOT USED */
    0                  /* ulIntVector - NOT USED */
  },
#endif
#if ENABLE_USART1
  {
    "/dev/com2",       /* sDeviceName */
    SERIAL_CUSTOM,     /* deviceType */
    &usart_polling_fns,/* pDeviceFns */
    NULL,              /* deviceProbe */
    NULL,              /* pDeviceFlow */
    0,                 /* ulMargin - NOT USED */
    0,                 /* ulHysteresis - NOT USED */
    NULL,              /* pDeviceParams */
    USART1_BASE,       /* ulCtrlPort1  - Pointer to USART 1 regs */
    0,                 /* ulCtrlPort2  - NOT USED */
    0,                 /* ulDataPort  - NOT USED */
    NULL,              /* getRegister - NOT USED */
    NULL,              /* setRegister - NOT USED */
    NULL,              /* getData - NOT USED */
    NULL,              /* setData - NOT USED */
    0,                 /* ulClock - NOT USED */
    0                  /* ulIntVector - NOT USED */
  },
#endif
#if ENABLE_USART2
  {
    "/dev/com3",       /* sDeviceName */
    SERIAL_CUSTOM,     /* deviceType */
    &usart_polling_fns,/* pDeviceFns */
    NULL,              /* deviceProbe */
    NULL,              /* pDeviceFlow */
    0,                 /* ulMargin - NOT USED */
    0,                 /* ulHysteresis - NOT USED */
    NULL,              /* pDeviceParams */
    USART2_BASE,       /* ulCtrlPort1  - Pointer to USART 2 regs */
    0,                 /* ulCtrlPort2  - NOT USED */
    0,                 /* ulDataPort  - NOT USED */
    NULL,              /* getRegister - NOT USED */
    NULL,              /* setRegister - NOT USED */
    NULL,              /* getData - NOT USED */
    NULL,              /* setData - NOT USED */
    0,                 /* ulClock - NOT USED */
    0                  /* ulIntVector - NOT USED */
  },
#endif
#if ENABLE_USART3
  {
    "/dev/com4",       /* sDeviceName */
    SERIAL_CUSTOM,     /* deviceType */
    &usart_polling_fns,/* pDeviceFns */
    NULL,              /* deviceProbe */
    NULL,              /* pDeviceFlow */
    0,                 /* ulMargin - NOT USED */
    0,                 /* ulHysteresis - NOT USED */
    NULL,              /* pDeviceParams */
    USART3_BASE,       /* ulCtrlPort1  - Pointer to USART 3 regs */
    0,                 /* ulCtrlPort2  - NOT USED */
    0,                 /* ulDataPort  - NOT USED */
    NULL,              /* getRegister - NOT USED */
    NULL,              /* setRegister - NOT USED */
    NULL,              /* getData - NOT USED */
    NULL,              /* setData - NOT USED */
    0,                 /* ulClock - NOT USED */
    0                  /* ulIntVector - NOT USED */
  }
#endif
};

console_tbl *BSP_get_uart_from_minor(int minor)
{
    return Console_Port_Tbl[minor];
}
