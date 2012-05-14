/*
 *  This file contains a typical set of register access routines which may be
 *  used with the m48t08 chip if accesses to the chip are as follows:
 *
 *    + registers are accessed as bytes
 *    + registers are on 64-bit boundaries
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#define _M48T08_MULTIPLIER 8
#define _M48T08_NAME(_X) _X##_8
#define _M48T08_TYPE uint8_t

#include "m48t08_reg.c"
