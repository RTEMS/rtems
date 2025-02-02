/*
 * RTEMS generic MPC5200 BSP
 *
 * RTEMS MPC5x00 PCMCIA IDE harddisk header file
 * This file contains declarations for the PCMCIA IDE Interface.
 */

/*
 * Copyright (C) 2003 IPR Engineering
 * Copyright (C) 2005 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
