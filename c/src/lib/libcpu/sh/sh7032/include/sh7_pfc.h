/*
 * Bit values for the pin function controller of the Hitachi SH703X
 *
 * From Hitachi tutorials
 *
 *  Authors: Ralf Corsepius (corsepiu@faw.uni-ulm.de) and
 *           Bernd Becker (becker@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *  COPYRIGHT (c) 1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _sh7_pfc_h
#define _sh7_pfc_h

#include <rtems/score/iosh7032.h>

/*
 * Port B IO Register (PBIOR)
 */
#define PBIOR           PFC_PBIOR
#define PB15IOR         0x8000
#define PB14IOR         0x4000
#define PB13IOR         0x2000
#define PB12IOR         0x1000
#define PB11IOR         0x0800
#define PB10IOR         0x0400
#define PB9IOR          0x0200
#define PB8IOR          0x0100
#define PB7IOR          0x0080
#define PB6IOR          0x0040
#define PB5IOR          0x0020
#define PB4IOR          0x0010
#define PB3IOR          0x0008
#define PB2IOR          0x0004
#define PB1IOR          0x0002
#define PB0IOR          0x0001

/*
 * Port B Control Register (PBCR1)
 */
#define PBCR1           PFC_PBCR1
#define PB15MD1         0x8000
#define PB15MD0         0x4000
#define PB14MD1         0x2000
#define PB14MD0         0x1000
#define PB13MD1         0x0800
#define PB13MD0         0x0400
#define PB12MD1         0x0200
#define PB12MD0         0x0100
#define PB11MD1         0x0080
#define PB11MD0         0x0040
#define PB10MD1         0x0020
#define PB10MD0         0x0010
#define PB9MD1          0x0008
#define PB9MD0          0x0004
#define PB8MD1          0x0002
#define PB8MD0          0x0001

#define PB15MD          PB15MD1|PB14MD0
#define PB14MD          PB14MD1|PB14MD0
#define PB13MD          PB13MD1|PB13MD0
#define PB12MD          PB12MD1|PB12MD0
#define PB11MD          PB11MD1|PB11MD0
#define PB10MD          PB10MD1|PB10MD0
#define PB9MD           PB9MD1|PB9MD0
#define PB8MD           PB8MD1|PB8MD0

#define PB_TXD1         PB11MD1
#define PB_RXD1         PB10MD1
#define PB_TXD0         PB9MD1
#define PB_RXD0         PB8MD1

/*
 * Port B Control Register (PBCR2)
 */
#define PBCR2   PFC_PBCR2
#define PB7MD1  0x8000
#define PB7MD0  0x4000
#define PB6MD1  0x2000
#define PB6MD0  0x1000
#define PB5MD1  0x0800
#define PB5MD0  0x0400
#define PB4MD1  0x0200
#define PB4MD0  0x0100
#define PB3MD1  0x0080
#define PB3MD0  0x0040
#define PB2MD1  0x0020
#define PB2MD0  0x0010
#define PB1MD1  0x0008
#define PB1MD0  0x0004
#define PB0MD1  0x0002
#define PB0MD0  0x0001

#define PB7MD   PB7MD1|PB7MD0
#define PB6MD   PB6MD1|PB6MD0
#define PB5MD   PB5MD1|PB5MD0
#define PB4MD   PB4MD1|PB4MD0
#define PB3MD   PB3MD1|PB3MD0
#define PB2MD   PB2MD1|PB2MD0
#define PB1MD   PB1MD1|PB1MD0
#define PB0MD   PB0MD1|PB0MD0

#endif /* _sh7_pfc_h */
