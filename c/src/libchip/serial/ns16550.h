/**
 *  @file
 *  
 */

/*
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *  THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 *  AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 *  You are hereby granted permission to use, copy, modify, and distribute
 *  this file, provided that this notice, plus the above copyright notice
 *  and disclaimer, appears in all copies. Radstone Technology will provide
 *  no support for this code.
 *
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _NS16550_H_
#define _NS16550_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Driver function table
 */

extern console_fns ns16550_fns;
extern console_fns ns16550_fns_polled;

/*
 * Flow control function tables
 */

extern console_flow ns16550_flow_RTSCTS;
extern console_flow ns16550_flow_DTRCTS;

/*
 *  Helpers for printk
 */
void ns16550_outch_polled(console_tbl *c, char out);
int ns16550_inch_polled(console_tbl *c);

#ifdef __cplusplus
}
#endif

#endif /* _NS16550_H_ */
