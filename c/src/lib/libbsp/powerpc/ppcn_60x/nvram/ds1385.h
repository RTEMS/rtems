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
 * Definitions for the ds1385 NvRAM
 */

#ifndef _DS1385_H
#define _DS1385_H

#include "prepnvr.h"

#define DS1385_NVSIZE 4096       /* size of NVRAM */
#define DS1385_GESIZE (DS1385_NVSIZE-CONFSIZE-OSAREASIZE-sizeof(HEADER))
#define DS1385_PORT_BASE 0x74

/* Here is the whole map of the DS1385 NVRAM */
typedef struct _DS1385_NVRAM_MAP {
  HEADER Header;
  unsigned char GEArea[DS1385_GESIZE];
  unsigned char OSArea[OSAREASIZE];
  unsigned char ConfigArea[CONFSIZE];
  } DS1385_NVRAM_MAP, *PDS1385_NVRAM_MAP;

#endif /* _DS1385_H */

