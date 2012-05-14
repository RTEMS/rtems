/*
 *  hw_init.c: set up sh7045F internal subunits
 *             Pin and memory assignments assume
 *             target is Hitachi SH7045F EVB ("lcevb")
 *
 *             Provides two initialization routines:
 *             A. 'void early_hw_init(void)' for 'start.S'
 *                sets up hw needed for early RTEMS boot, and
 *             B. 'void bsp_hw_init(void)' for 'bspstart.c'
 *                sets up hardware used by this BSP.
 *
 *  Author: John M. Mills (jmills@tga.com)
 *  COPYRIGHT(c) 2000, TGA Technologies, Inc
 *                     Norcross, GA 30071 U.S.A
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Adapted from Hitachi EVB7045F tutorial files by:
 *  John M. Mills (jmills@tga.com)
 *  TGA Technologies, Inc.
 *  100 Pinnacle Way, Suite 140
 *  Norcross, GA 30071 U.S.A.
 *
 *
 *  This file may be copied and distributed in accordance
 *  the above-referenced license. It is provided for critique and
 *  developmental purposes without any warranty nor representation
 *  by the authors or by TGA Technologies.
 */

#include <bsp.h>

#include <stdlib.h>

#include <rtems/libio.h>
#include <rtems/iosupp.h>
#include <rtems/score/sh_io.h>
#include <rtems/score/iosh7045.h>

/* exported entries */
extern void bsp_hw_init (void);
extern void early_hw_init (void);

/*  called from 'start.S' on "#ifdef START_HW_INIT" */
void early_hw_init (void)
{
#ifdef STANDALONE_EVB
  /* STANDALONE_EVB minimally sets up bus and DRAM here */
  /* no STANDALONE_EVB accepts defaults from debug monitor */

  /* FIXME: replace 'magic numbers' with logical names */

  write16(0x2020, BSC_BCR1);  /* Bus width access - 32-bit on CS1 */
  write16(0xF3DD, BSC_BCR2);  /* Idle cycles CS3-CS0 - 0 idle cycles*/
  write16(0xFF3F, BSC_WCR1);  /* Waits for CS3-CS0 - 3 waits on CS1 */
  write16(0x000F, BSC_WCR2);  /* Waits for DRAM/DMA access - default */
  write16(0x0000, BSC_DCR);   /* DRAM control - default */
  write16(0x0000, BSC_RTCSR); /* DRAM refresh - default */
  write16(0x0000, BSC_RTCNT); /* DRAM refresh counter - default*/
  write16(0x0000, BSC_RTCOR); /* DRAM refresh compare match - default */
#endif

  /*  add early-init functions here */

};

/*  to be called from 'bspstart.c' */
void bsp_hw_init (void)
{
  uint16_t   temp16;

#ifdef STANDALONE_EVB
  /* STANDALONE_EVB: sets up PFC */
  /* no STANDALONE_EVB: accepts defaults, adds RESET */

  /* FIXME: replace 'magic numbers' */

  write16(0x5000, PFC_PACRH);  /* Pin function controller - WRHH, WRHL */
  write16(0x1550, PFC_PACRL1); /* Pin fun. controller - WRH,WRL,RD,CS1 */
  write16(0x0000, PFC_PBCR1);  /* Pin function controller - default */
  write16(0x2005, PFC_PBCR2);  /* Pin fcn. controller - A18,A17,A16 */
  write16(0xFFFF, PFC_PCCR);   /* Pin function controller - A15-A0  */
  write16(0x5555, PFC_PDCRH1); /* Pin function controller - D31-D24 */
  write16(0x5555, PFC_PDCRH2); /* Pin function controller - D23-D16 */
  write16(0xFFFF, PFC_PDCRL);  /* Pin function controller - D15-D0  */
  write16(0x0000, PFC_IFCR);   /* Pin function controller - default */
  write16(0x0000, PFC_PACRL2); /* default disconnects all I/O pins;*/
	                       /*  [re-connected by DEVICE_open()] */
#endif

  /* default hardware setup for SH7045F EVB */

  /* PFC: General I/O except pin 13 (reset): */
  temp16 = read16(PFC_PECR1);
  temp16 |= 0x0800;
  write16(temp16, PFC_PECR1);

  /* All I/O lines bits 7-0: */
  write16(0x00, PFC_PECR2);

  /* P5 (LED) out, all other pins in: */
  temp16 = read16(PFC_PEIOR);
  temp16 |= 0x0020;
  write16(temp16, PFC_PEIOR);

}
