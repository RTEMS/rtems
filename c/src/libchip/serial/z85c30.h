/**
 *  @file
 *
 *  This include file contains all console driver definitions for the
 *  Zilog z85c30.
 */

/*
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
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __Z85C30_H
#define __Z85C30_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Driver function table
 */

extern console_fns z85c30_fns;
extern console_fns z85c30_fns_polled;

/*
 *  Flow control function tables
 */

extern console_flow z85c30_flow_RTSCTS;
extern console_flow z85c30_flow_DTRCTS;

/*
 * Default register access routines
 */

uint8_t   z85c30_get_register(   /* registers are byte-wide */
  uint32_t    ulCtrlPort,
  uint8_t     ucRegNum
);

void z85c30_set_register(
  uint32_t    ulCtrlPort,
  uint8_t     ucRegNum,
  uint8_t     ucData
);

uint8_t   z85c30_get_data(
  uint32_t    ulDataPort
);

void z85c30_set_data(
  uint32_t    ulDataPort,
  uint8_t     ucData
);

#ifdef __cplusplus
}
#endif

#endif
