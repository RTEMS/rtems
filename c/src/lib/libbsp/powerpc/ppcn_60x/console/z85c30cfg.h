/*  z85c30cfg.h
 *
 *  This include file contains all console driver definations for the z85c30
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
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __Z85C30_CONFIG_H
#define __Z85C30_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Board specific register access routines
 */

unsigned8 Read_85c30_register(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum
);

void  Write_85c30_register(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum,
  unsigned8   ucData
);

unsigned8 Read_85c30_data(
  unsigned32  ulDataPort
);

void  Write_85c30_data(
  unsigned32  ulDataPort,
  unsigned8   ucData
);

#ifdef __cplusplus
}
#endif

#endif
