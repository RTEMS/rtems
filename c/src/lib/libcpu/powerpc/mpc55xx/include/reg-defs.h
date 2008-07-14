/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief Register definitions.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBCPU_POWERPC_MPC55XX_REG_DEFS_H
#define LIBCPU_POWERPC_MPC55XX_REG_DEFS_H

/*
 * Register addresses
 */

#define FMPLL_SYNCR 0xC3F80000
#define FMPLL_SYNSR 0xC3F80004
#define FLASH_BIUCR 0xC3F8801C
#define SIU_ECCR 0xC3F90984
#define SIU_SRCR 0xC3F90010

/*
 * Special purpose registers
 */

#define BUCSR 1013

/*
 * Branch Unit Control and Status Register (BUCSR)
 */

#define BUCSR_BBFI 0x00000200
#define BUCSR_BPEN 0x00000001

/*
 * Definitions for FMPLL_SYNCR (FMPLL Synthesizer Control Register)
 */

/* Fields used for PREDIV (Pre-Divider bits [1:3]) */
#define FMPLL_SYNCR_PREDIV_0 0x00000000

/* Fields used for MFD (Muliplication Factor Divider bits [4:8]) */
#define FMPLL_SYNCR_MFD_0 0x00000000
#define FMPLL_SYNCR_MFD_2 0x01000000
#define FMPLL_SYNCR_MFD_4 0x02000000
#define FMPLL_SYNCR_MFD_6 0x03000000
#define FMPLL_SYNCR_MFD_8 0x04000000
#define FMPLL_SYNCR_MFD_10 0x05000000
#define FMPLL_SYNCR_MFD_12 0x06000000

/* Fields used for RFD (Reduced Frequency Divider bits [10:12]) */
#define FMPLL_SYNCR_RFD_0 0x00000000
#define FMPLL_SYNCR_RFD_1 0x00080000
#define FMPLL_SYNCR_RFD_2 0x00100000
#define FMPLL_SYNCR_RFD_3 0x00180000
#define FMPLL_SYNCR_RFD_4 0x00200000
#define FMPLL_SYNCR_RFD_5 0x00280000
#define FMPLL_SYNCR_RFD_6 0x00300000
#define FMPLL_SYNCR_RFD_7 0x00380000

/* Fields for LOCEN (Loss-of-clock enable bit [13]) */
#define FMPLL_SYNCR_LOCEN 0x00040000

/* Fields for LOLRE (Loss-of-lock reset enable bit [14]) */
#define FMPLL_SYNCR_LOLRE 0x00020000

/* Fields for LOCRE (Loss-of-clock reset enable bit [15]) */
#define FMPLL_SYNCR_LOCRE 0x00010000

/* Fields for DISCLK (Disable CLKOUT bit [16]) */
#define FMPLL_SYNCR_DISCLK 0x00008000

/* Fields for LOLIRQ (Loss-of-lock interrupt request bit [17]) */
#define FMPLL_SYNCR_LOLIRQ 0x00004000

/* Fields for LOCIRQ (Loss-of-clock interrupt request bit [18]) */
#define FMPLL_SYNCR_LOCIRQ 0x00002000

/* Fields for RATE (Modulation rate bit [19]) */
#define FMPLL_SYNCR_RATE_FREF 0x00001000

/* Fields for DEPTH (Modulation depth percentage bits [20:21]) */
#define FMPLL_SYNCR_DEPTH_0 0x00000000
#define FMPLL_SYNCR_DEPTH_1 0x00000400
#define FMPLL_SYNCR_DEPTH_2 0x00000800

/* Fields for EXP (Expected difference bits [22:31]) */
#define FMPLL_SYNCR_EXP_0 0x00000000

/*
 * Definitions for the FMPLL_SYNSR (Synthesizer Status Register)
 */

/* Fields for LOLF (Loss-of-lock flag bit [22]) */
#define FMPLL_SYNSR_LOLF 0x00000200

/* Fields for LOCK (Lock status bit [28]) */
#define FMPLL_SYNSR_LOCK 0x00000008

/* Fields for LOCF (Loss-of-clock flag bit [29]) */
#define FMPLL_SYNSR_LOCF 0x00000004

/*
 * Definitions for the SIU_SRCR (System Reset Control Register)
 */

/* Fields for SSR (software system reset bit [0]) */
#define SIU_SRCR_SSR 0x80000000

/* Fields for SER (external system reset bit [1]) */
#define SIU_SRCR_SER 0x40000000

/* Fields for CRE (checkstop reset enable bit [16]) */
#define SIU_SRCR_CRE 0x00008000

/*
 * Definitions for FLASH_BIUCR (Flash BIU Control Register)
 */

/* Fields for Flash Bus Interface Control */
/* Fields for Prefetch Control (MnPFE Master n Prefetch Enable) */

/* Fields for M3PFE (Master 3 (EBI) prefetch enable bit [12]) */
#define FLASH_BUICR_EBI_PREFTCH 0x00080000

/* Fields for M2PFE (Master 2 (eDMA) prefetch enable bit [13]) */
#define FLASH_BUICR_EDMA_PREFTCH 0x00040000

/* Fields for M1PFE (Master 1 (Nexus) prefetch enable bit [14]) */
#define FLASH_BUICR_NEX_PREFTCH 0x00020000

/* Fields for M0PFE (Master 0 (e200z core) prefetch enable bit [15]) */
#define FLASH_BUICR_CPU_PREFTCH 0x00010000

/* Fields for APC (access pipelining control bits [16:18]) */
#define FLASH_BUICR_APC_1 0x00002000
#define FLASH_BUICR_APC_2 0x00004000
#define FLASH_BUICR_APC_3 0x00006000
#define FLASH_BUICR_APC_4 0x00008000
#define FLASH_BUICR_APC_5 0x0000A000
#define FLASH_BUICR_APC_6 0x0000C000
#define FLASH_BUICR_APC_NO 0x0000E000

/* Fields for WWSC (write wait state control bits [19:20]) */
#define FLASH_BUICR_WWSC_1 0x00000800
#define FLASH_BUICR_WWSC_2 0x00001000
#define FLASH_BUICR_WWSC_3 0x00001800

/* Fields for RWSC (read wait state control bits [21:23]) */
#define FLASH_BUICR_RWSC_0 0x00000000
#define FLASH_BUICR_RWSC_1 0x00000100
#define FLASH_BUICR_RWSC_2 0x00000200
#define FLASH_BUICR_RWSC_3 0x00000300
#define FLASH_BUICR_RWSC_4 0x00000400
#define FLASH_BUICR_RWSC_5 0x00000500
#define FLASH_BUICR_RWSC_6 0x00000600
#define FLASH_BUICR_RWSC_7 0x00000700

/* Fields for DPFEN (data prefetch enable bits [24:25]) */
#define FLASH_BUICR_DPFEN_0 0x00000000
#define FLASH_BUICR_DPFEN_1 0x00000040
#define FLASH_BUICR_DPFEN_3 0x000000C0

/* Fields for IPFEN (instruction prefetch enable bits [26:27]) */
#define FLASH_BUICR_IPFEN_0 0x00000000
#define FLASH_BUICR_IPFEN_1 0x00000010
#define FLASH_BUICR_IPFEN_3 0x00000030

/* Fields for PFLIM (additional line prefetch (limit) bits [28:30]) */
#define FLASH_BUICR_PFLIM_0 0x00000000
#define FLASH_BUICR_PFLIM_1 0x00000002
#define FLASH_BUICR_PFLIM_2 0x00000004
#define FLASH_BUICR_PFLIM_3 0x00000006
#define FLASH_BUICR_PFLIM_4 0x00000008
#define FLASH_BUICR_PFLIM_5 0x0000000A
#define FLASH_BUICR_PFLIM_6 0x0000000C

/* Fields for BFEN (enable line read buffer hits bit [31]) */
#define FLASH_BUICR_BFEN 0x00000001

#endif /* LIBCPU_POWERPC_MPC55XX_REG_DEFS_H */
