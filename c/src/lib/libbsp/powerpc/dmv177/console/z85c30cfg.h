/*
 *  This include file contains all console driver definitions for the z85c30
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
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

unsigned8 Read_z85c30_register(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum
);

void  Write_z85c30_register(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum,
  unsigned8   ucData
);

unsigned8 Read_z85c30_data(
  unsigned32  ulDataPort
);

void  Write_z85c30_data(
  unsigned32  ulDataPort,
  unsigned8   ucData
);

#ifdef __cplusplus
}
#endif

#endif
