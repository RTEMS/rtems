/*
 *  This driver uses the termios pseudo driver.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/ringbuf.h>
#include <libchip/serial.h>
#include <libchip/ns16550.h>

extern console_tbl	Console_Port_Tbl[];
extern console_data	Console_Port_Data[];
extern unsigned long	Console_Port_Count;

bool Console_Port_Tbl_Init_ppc8245(int minor);
