/*
 *  console.c  -- console I/O package
 *
 *  Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 * This code is based on the pc386 BSP console.c so the following
 * copyright also applies :
 *
 * (C) Copyright 1997 -
 * - NavIST Group - Real-Time Distributed Systems and Industrial Automation
 *
 * http://pandora.ist.utl.pt
 *
 * Instituto Superior Tecnico * Lisboa * PORTUGAL
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 * $Id$
 */
  
#include <stdlib.h>
#include <assert.h>
#include <stdlib.h>

#undef __assert
void __assert (const char *file, int line, const char *msg);
extern int close(int fd);

#include <bsp.h>
#include <bsp/irq.h>
#include <rtems/libio.h>
#include <termios.h>
#include <bsp/uart.h>
#include <bsp/consoleIo.h>

/* Definitions for BSPConsolePort */
#define BSP_CONSOLE_PORT_CONSOLE (-1)
#define BSP_CONSOLE_PORT_COM1    (BSP_UART_COM1)
#define BSP_CONSOLE_PORT_COM2    (BSP_UART_COM2)
/*
 * Possible value for console input/output :
 *	BSP_CONSOLE_PORT_CONSOLE
 *	BSP_UART_COM1
 *	BSP_UART_COM2
 */

int BSPConsolePort = BSP_UART_COM1;

/* int BSPConsolePort = BSP_UART_COM2;  */
int BSPBaseBaud    = 115200;

void console_reserve_resources(rtems_configuration_table *conf)
{
    if(BSPConsolePort != BSP_CONSOLE_PORT_CONSOLE)
    {
      rtems_termios_reserve_resources(conf, 1);
    }
   
  return;
}
