/*
 *  This file contains the console driver chip level routines for the
 *  z85c30 chip.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>

/* 
 *  Read_z85c30_register
 */

unsigned8 Read_z85c30_register(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum
)
{
  unsigned8 *port;

  port = (unsigned8 *)ulCtrlPort;

  if(ucRegNum) {
    *port = ucRegNum;
  }
  return *port;

}

/*
 *  Write_z85c30_register
 */

void  Write_z85c30_register(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum,
  unsigned8   ucData
)
{
  unsigned8 *port;

  port = (unsigned8 *)ulCtrlPort;

  if(ucRegNum) {
    *port = ucRegNum;
  }
  *port = ucData;
}

/* 
 *  Read_z85c30_data
 */

unsigned8 Read_z85c30_data(
  unsigned32  ulDataPort
)
{
  unsigned8 *port;

  port = (unsigned8 *)ulDataPort;
  return *port;
}

/*
 *  Write_z85c30_data
 */

void  Write_z85c30_data(
  unsigned32  ulDataPort,
  unsigned8   ucData
)
{
  unsigned8 *port;

  port = (unsigned8 *)ulDataPort;
  *port = ucData;
}
