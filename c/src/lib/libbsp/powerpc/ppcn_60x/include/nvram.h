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
extern rtems_status_code ReadNvRAM8(uint32_t   ulOffset, uint8_t   *pucData);
extern rtems_status_code WriteNvRAM8(uint32_t   ulOffset, uint8_t   ucValue);
extern rtems_status_code ReadNvRAM16(uint32_t   ulOffset, uint16_t   *pusData);
extern rtems_status_code WriteNvRAM16(uint32_t   ulOffset, uint16_t   usValue);
extern rtems_status_code ReadNvRAM32(uint32_t   ulOffset, uint32_t   *pulData);
extern rtems_status_code WriteNvRAM32(uint32_t   ulOffset, uint32_t   ulValue);
rtems_status_code ReadNvRAMBlock(
  uint32_t   ulOffset, uint8_t   *pucData, uint32_t   length);
rtems_status_code WriteNvRAMBlock(
  uint32_t   ulOffset, uint8_t   *ucValue, uint32_t   length);
/*
 * This routine returns the size of the NvRAM
 */
extern uint32_t   SizeNvRAM();

/*
 * This routine commits changes to the NvRAM
 */
extern void CommitNvRAM();

#ifdef __cplusplus
}
#endif

#endif /* _NVRAM_H */
