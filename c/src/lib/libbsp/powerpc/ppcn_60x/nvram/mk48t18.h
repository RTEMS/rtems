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
    unsigned8 SystemDependentArea2[8];
    unsigned8 FeatureByte0[1];
    unsigned8 FeatureByte1[1];
    unsigned8 Century;	     /* century byte in BCD */
    unsigned8 FeatureByte3[1];
    unsigned8 FeatureByte4[1];
    unsigned8 FeatureByte5[1];
    unsigned8 FeatureByte6[1];
    unsigned8 FeatureByte7[1];
    unsigned8 BootPW[14];
    rtems_unsigned16 BootCrc; /* CRC on BootPW */
    unsigned8 ConfigPW[14];
    rtems_unsigned16 ConfigCrc; /* CRC on ConfigPW */
    unsigned8 SystemDependentArea1[8];
    /*
     * The following are the RTC registers
     */
    volatile unsigned8 Control;
    volatile unsigned8 Second:7;	/* 0-59 */
    volatile unsigned8 Stop:1;
    volatile unsigned8 Minute;	/* 0-59 */
    volatile unsigned8 Hour;	/* 0-23 */
    volatile unsigned8 Day:3;	/* 1-7 */
    volatile unsigned8 Resvd1:3;	/* 0 */
    volatile unsigned8 FT:1;	/* Frequency test bit - must be 0 */
    volatile unsigned8 Resvd2:1;	/* 0 */
    volatile unsigned8 Date;	/* 1-31 */
    volatile unsigned8 Month;	/* 1-12 */
    volatile unsigned8 Year;	/* 0-99 */
} MK48T18_CMOS_MAP, *PMK48T18_CMOS_MAP;

/*
 * Control register definitions
 */
#define MK48T18_CTRL_WRITE	0x80
#define MK48T18_CTRL_READ	0x40
#define MK48T18_CTRL_SIGN	0x20

#define MK48T18_NVSIZE 8192-sizeof(MK48T18_CMOS_MAP)
#define MK48T18_GESIZE (MK48T18_NVSIZE-CONFSIZE-OSAREASIZE-sizeof(HEADER))
#define MK48T18_BASE (PMK48T18_NVRAM_MAP)((unsigned8 *)PCI_MEM_BASE+0x00800000)

/* Here is the whole map of the MK48T18 NVRAM */
typedef struct _MK48T18_NVRAM_MAP {
    HEADER	Header;
    unsigned8	GEArea[MK48T18_GESIZE];
    unsigned8	OSArea[OSAREASIZE];
    unsigned8	ConfigArea[CONFSIZE];
    MK48T18_CMOS_MAP	CMOS;
} MK48T18_NVRAM_MAP, *PMK48T18_NVRAM_MAP;

#endif /* _MK48T18_H */
