/*
 * RTEMS generic MPC5200 BSP
 *
 * RTEMS MPC5x00 PCMCIA IDE harddisk header file
 * This file contains declarations for the PCMCIA IDE Interface.
 */

/*
 * Copyright (c) 2003 IPR Engineering
 * Copyright (c) 2005 embedded brains GmbH & Co. KG
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef __PCMCIA_IDE_h
#define __PCMCIA_IDE_h

#include <bsp.h>

#define GPIOPCR_ATA_CS_4_5                           (1 << 24)

/*#define DEBUG_OUT*/
#define GET_UP_BYTE_OF_MPC5200_ATA_DRIVE_REG(val32)  ((uint16_t)((val32) >> 24))
#define SET_UP_BYTE_OF_MPC5200_ATA_DRIVE_REG(val8)   ((uint32_t)((val8)  << 24))
#define GET_UP_WORD_OF_MPC5200_ATA_DRIVE_REG(val32)  ((uint16_t)((val32) >> 16))
#define SET_UP_WORD_OF_MPC5200_ATA_DRIVE_REG(val16)  ((uint32_t)((val16) << 16))

#define ATA_HCFG_SMR                                 (1 << 31)
#define ATA_HCFG_FR                                  (1 << 30)
#define ATA_HCFG_IORDY                               (1 << 24)
#define ATA_HCFG_IE                                  (1 << 25)

#define COUNT_VAL(nsec)                              (((nsec) * (IPB_CLOCK / 1000000) + 999) / 1000)

#define PIO_3                                        0
#define PIO_4                                        1

#define T0                                           0
#define T2_8                                         1
#define T2_16                                        2
#define T4                                           3
#define T1                                           4
#define TA                                           5

#define ATA_PIO_TIMING_1(t0,t2_8,t2_16)              (((COUNT_VAL(t0)) << 24) | ((COUNT_VAL(t2_8)) << 16) | ((COUNT_VAL(t2_16)) << 8))
#define ATA_PIO_TIMING_2(t4,t1,ta)                   (((COUNT_VAL(t4)) << 24) | ((COUNT_VAL(t1)) << 16) | ((COUNT_VAL(ta)) << 8))

#endif
