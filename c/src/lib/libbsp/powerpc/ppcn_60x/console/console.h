/*
 *  This file contains the TTY driver table definition for the PPCn_60x
 *
 *  This driver uses the termios pseudo driver.
 *
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *
 * THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 * AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 * You are hereby granted permission to use, copy, modify, and distribute
 * this file, provided that this notice, plus the above copyright notice
 * and disclaimer, appears in all copies. Radstone Technology will provide
 * no support for this code.
 *
 *  $Id$
 */

#include <ringbuf.h>
#include <libchip/serial.h>
#include <libchip/ns16550.h>
#include <libchip/z85c30.h>

extern console_tbl	Console_Port_Tbl[];
extern console_data	Console_Port_Data[];
extern unsigned long	Console_Port_Count;
