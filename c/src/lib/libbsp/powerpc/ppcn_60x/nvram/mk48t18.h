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
 */

/*
 * Definitions for the mk48t18 RTC/NvRAM
 */

#ifndef _MK48T18_H
#define _MK48T18_H

#include "prepnvr.h"

/*
 * This structure maps to the top of the NvRAM. It is based on the standard
 * CMOS.h file for the ds1385. Feature and system dependant areas are preserved
 * for potential compatibility issues.
 *
 * The CRC's are computed with x**16+x**12+x**5 + 1 polynomial
 * The clock is kept in 24 hour BCD mode and should be set to UT(GMT)
 */
 
typedef struct _MK48T18_CMOS_MAP {
    uint8_t   SystemDependentArea2[8];
    uint8_t   FeatureByte0[1];
    uint8_t   FeatureByte1[1];
    uint8_t   Century;	     /* century byte in BCD */
    uint8_t   FeatureByte3[1];
    uint8_t   FeatureByte4[1];
    uint8_t   FeatureByte5[1];
    uint8_t   FeatureByte6[1];
    uint8_t   FeatureByte7[1];
    uint8_t   BootPW[14];
    uint16_t         BootCrc; /* CRC on BootPW */
    uint8_t   ConfigPW[14];
    uint16_t         ConfigCrc; /* CRC on ConfigPW */
    uint8_t   SystemDependentArea1[8];
    /*
     * The following are the RTC registers
     */
    volatile uint8_t   Control;
    volatile uint8_t   Second:7;	/* 0-59 */
    volatile uint8_t   Stop:1;
    volatile uint8_t   Minute;	/* 0-59 */
    volatile uint8_t   Hour;	/* 0-23 */
    volatile uint8_t   Day:3;	/* 1-7 */
    volatile uint8_t   Resvd1:3;	/* 0 */
    volatile uint8_t   FT:1;	/* Frequency test bit - must be 0 */
    volatile uint8_t   Resvd2:1;	/* 0 */
    volatile uint8_t   Date;	/* 1-31 */
    volatile uint8_t   Month;	/* 1-12 */
    volatile uint8_t   Year;	/* 0-99 */
} MK48T18_CMOS_MAP, *PMK48T18_CMOS_MAP;

/*
 * Control register definitions
 */
#define MK48T18_CTRL_WRITE	0x80
#define MK48T18_CTRL_READ	0x40
#define MK48T18_CTRL_SIGN	0x20

#define MK48T18_NVSIZE 8192-sizeof(MK48T18_CMOS_MAP)
#define MK48T18_GESIZE (MK48T18_NVSIZE-CONFSIZE-OSAREASIZE-sizeof(HEADER))
#define MK48T18_BASE (PMK48T18_NVRAM_MAP)((uint8_t*)PCI_MEM_BASE+0x00800000)

/* Here is the whole map of the MK48T18 NVRAM */
typedef struct _MK48T18_NVRAM_MAP {
    HEADER	Header;
    uint8_t  	GEArea[MK48T18_GESIZE];
    uint8_t  	OSArea[OSAREASIZE];
    uint8_t  	ConfigArea[CONFSIZE];
    MK48T18_CMOS_MAP	CMOS;
} MK48T18_NVRAM_MAP, *PMK48T18_NVRAM_MAP;

#endif /* _MK48T18_H */
