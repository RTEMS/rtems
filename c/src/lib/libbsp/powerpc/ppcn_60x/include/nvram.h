/*
 *  This file contains the NvRAM driver definitions for the PPCn_60x
 *
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

#ifndef _NVRAM_H
#define _NVRAM_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This initializes the NvRAM driver
 */

void InitializeNvRAM(void);


/*
 * These routines access data in the NvRAM's OS area
 */
extern rtems_status_code ReadNvRAM8(unsigned32 ulOffset, unsigned8 *pucData);
extern rtems_status_code WriteNvRAM8(unsigned32 ulOffset, unsigned8 ucValue);
extern rtems_status_code ReadNvRAM16(unsigned32 ulOffset, unsigned16 *pusData);
extern rtems_status_code WriteNvRAM16(unsigned32 ulOffset, unsigned16 usValue);
extern rtems_status_code ReadNvRAM32(unsigned32 ulOffset, unsigned32 *pulData);
extern rtems_status_code WriteNvRAM32(unsigned32 ulOffset, unsigned32 ulValue);
rtems_status_code ReadNvRAMBlock(
  unsigned32 ulOffset, unsigned8 *pucData, unsigned32 length);
rtems_status_code WriteNvRAMBlock(
  unsigned32 ulOffset, unsigned8 *ucValue, unsigned32 length);
/*
 * This routine returns the size of the NvRAM
 */
extern unsigned32 SizeNvRAM();

/*
 * This routine commits changes to the NvRAM
 */
extern void CommitNvRAM();

#ifdef __cplusplus
}
#endif

#endif /* _NVRAM_H */
