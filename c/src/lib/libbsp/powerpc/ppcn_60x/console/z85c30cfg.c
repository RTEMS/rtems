/*
 *  This file contains the console driver chip level routines for the
 *  z85c30 chip.
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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>

/* 
 *  Read_85c30_register
 *
 *  Read a Z85c30 register
 */

unsigned8 Read_85c30_register(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum
)
{
  unsigned8 ucData;

  outport_byte(ulCtrlPort, ucRegNum);
  inport_byte(ulCtrlPort, ucData);
  return ucData;
}

/*
 *  Write_85c30_register
 *
 *  Write a Z85c30 register
 */

void  Write_85c30_register(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum,
  unsigned8   ucData
)
{
  if(ucRegNum) {
    outport_byte(ulCtrlPort, ucRegNum);
  }
  outport_byte(ulCtrlPort, ucData);
}

/* 
 *  Read_85c30_data
 *
 *  Read a Z85c30 data register
 */

unsigned8 Read_85c30_data(
  unsigned32  ulDataPort
)
{
  unsigned8 ucData;

  inport_byte(ulDataPort, ucData);
  return ucData;
}

/*
 *  Write_85c30_data
 *
 *  Write a Z85c30 data register
 */

void  Write_85c30_data(
  unsigned32  ulDataPort,
  unsigned8   ucData
)
{
  outport_byte(ulDataPort, ucData);
}
