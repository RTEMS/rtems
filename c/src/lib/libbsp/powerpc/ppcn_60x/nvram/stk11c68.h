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
 * Definitions for the stk11C68 NvRAM
 */

#ifndef _STK11C68_H
#define _STK11C68_H

#include "prepnvr.h"

/*
 * STK11C68 definitions
 */
#define STK11C68_NVSIZE 8192
#define STK11C68_GESIZE (STK11C68_NVSIZE-CONFSIZE-OSAREASIZE-sizeof(HEADER))
#define STK11C68_BASE (PSTK11C68_NVRAM_MAP)((unsigned8 *)PCI_MEM_BASE+0x00800000)

/*
 * STK11C88 definitions
 */
#define STK11C88_NVSIZE 0x8000-sizeof(MK48T18_CMOS_MAP)

/* Here is the whole map of the STK11C68 NVRAM */
typedef struct _STK11C68_NVRAM_MAP {
	HEADER		Header;
	unsigned8	GEArea[STK11C68_GESIZE];
	unsigned8	OSArea[OSAREASIZE];
	unsigned8	ConfigArea[CONFSIZE];
} STK11C68_NVRAM_MAP, *PSTK11C68_NVRAM_MAP;

#endif /* _STK11C68_H */
