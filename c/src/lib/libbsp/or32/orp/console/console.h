/* console.h -- console header file for the Bender board using the
 *              Or1k architecture.
 *	
 *  Copyright (C) 2001 Chris Ziomkowski, chris@asics.ws
 *
 *  This file is distributed as part of the RTEMS package from
 *  OAR Corporation, and follows the licensing and distribution
 *  terms as stated for RTEMS. 
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 */

#ifndef _CONSOLE_H
#define _CONSOLE_H

typedef enum {
  TERM_LOCAL_ECHO,
  TERM_BIT_RATE,
} ConsoleIOCTLCommand;

typedef struct {
  ConsoleIOCTLCommand command;
  void*                data;
} ConsoleIOCTLRequest;

typedef struct {
  unsigned char RBR;
  unsigned char IER;
  unsigned char IIR;
  unsigned char LCR;
  unsigned char MCR;
  unsigned char LSR;
  unsigned char MSR;
  unsigned char SCR;
} UART_16450_Read;

typedef struct {
  unsigned char THR;
  unsigned char IER;
  unsigned char IIR;
  unsigned char LCR;
  unsigned char MCR;
  unsigned char LSR;
  unsigned char MSR;
  unsigned char SCR;
} UART_16450_Write;

typedef struct {
  unsigned char DLM;
  unsigned char DLL;
  unsigned char IIR;
  unsigned char LCR;
  unsigned char MCR;
  unsigned char LSR;
  unsigned char MSR;
  unsigned char SCR;
} UART_16450_Latch;

typedef union {
  UART_16450_Read   read;
  UART_16450_Write  write;
  UART_16450_Latch  latch;
} UART_16450;

#endif
