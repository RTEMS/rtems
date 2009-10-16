/* rsPMCQ1.h - Radstone PMCQ1 private header
 *
 * Copyright 2000 Radstone Technology
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
 *  COPYRIGHT (c) 2005.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#include <libcpu/io.h>
#include <bsp/irq.h>

/*
   modification history
   --------------------
   01a,20Dec00,jpb         created
 */

#ifndef __RSPMCQ1_H
#define __RSPMCQ1_H

/*
 * PMCQ1 definitions
 */

/* 
 * 360 definitions 
 */

#define Q1_360_MBAR		0x0003ff00				/* master base address register */

#define REG_B_OFFSET		0x1000					/* offset to the internal registers */

#define Q1_360_SIM_MCR		(REG_B_OFFSET+0x00)		
#define Q1_360_SIM_PEPAR	(REG_B_OFFSET+0x16)		
#define Q1_360_SIM_SYPCR	(REG_B_OFFSET+0x22)		
#define Q1_360_SIM_PICR		(REG_B_OFFSET+0x26)		
#define Q1_360_SIM_PITR		(REG_B_OFFSET+0x2A)		
#define Q1_360_SIM_GMR		(REG_B_OFFSET+0x40)	
#define Q1_360_SIM_BR0		(REG_B_OFFSET+0x50)	
#define Q1_360_SIM_OR0		(REG_B_OFFSET+0x54)	
#define Q1_360_SIM_BR1		(REG_B_OFFSET+0x60)	
#define Q1_360_SIM_OR1		(REG_B_OFFSET+0x64)	

#define Q1_360_CPM_ICCR		(REG_B_OFFSET+0x500)		
#define Q1_360_CPM_SDCR		(REG_B_OFFSET+0x51E)		
#define Q1_360_CPM_CICR		(REG_B_OFFSET+0x540)		

/*
 * EPLD offsets
 *
 * Only top 4 data bits are used
 */
#define PMCQ1_CODE_VERSION	0x00040000	/* Code Version */

#define PMCQ1_BOARD_REVISION	0x00040004	/* Board Revision */

#define PMCQ1_BUILD_OPTION	0x00040008	/* Build Option */
#define PMCQ1_ACE_FITTED        0x80000000
#define PMCQ1_QUICC_FITTED      0x40000000
#define PMCQ1_SRAM_SIZE         0x30000000         /* 01 - 1MB */
#define PMCQ1_SRAM_FITTED       0x20000000

#define PMCQ1_INT_STATUS	0x0004000c	/* Interrupt Status */
#define PMCQ1_INT_STATUS_MA	0x20000000
#define PMCQ1_INT_STATUS_QUICC	0x10000000

#define PMCQ1_INT_MASK		0x00040010	/* Interrupt Mask */
#define PMCQ1_INT_MASK_QUICC	0x20000000
#define PMCQ1_INT_MASK_MA	0x10000000

#define PMCQ1_RT_ADDRESS	0x00040014	/* RT Address Latch */

#define PMCQ1_DRIVER_ENABLE	0x0004001c	/* Channel Drive Enable */
#define PMCQ1_DRIVER_ENABLE_3	0x80000000
#define PMCQ1_DRIVER_ENABLE_2	0x40000000
#define PMCQ1_DRIVER_ENABLE_1	0x20000000
#define PMCQ1_DRIVER_ENABLE_0	0x10000000

#define PMCQ1_MINIACE_REGS      0x000c0000
#define PMCQ1_MINIACE_MEM       0x00100000
#define PMCQ1_RAM               0x00200000

uint32_t PMCQ1_Read_EPLD( uint32_t base, uint32_t reg );
void     PMCQ1_Write_EPLD( uint32_t base, uint32_t reg, uint32_t data );

/*
 * QSPAN-II register offsets
 */

#define QSPAN2_INT_STATUS	0x00000600

typedef void (*PMCQ1_FUNCTION_PTR) (void *);

#define PCI_ID(v, d) ((d << 16) | v)


#define PCI_VEN_ID_RADSTONE	 	0x11b5
#define	PCI_DEV_ID_PMC1553 		0x0001
#define	PCI_DEV_ID_PMCF1 		0x0002
#define PCI_DEV_ID_PMCMMA		0x0003
#define	PCI_DEV_ID_PMCQ1 		0x0007
#define	PCI_DEV_ID_PMCQ2 		0x0008
#define PCI_DEV_ID_PMCF1V2		0x0012



typedef struct _PMCQ1BoardData 
{
    struct _PMCQ1BoardData              *pNext;     
    unsigned long                       busNo;
    unsigned long                       slotNo;
    unsigned long                       funcNo;
    unsigned long                       baseaddr;
    unsigned long                       bridgeaddr;
    PMCQ1_FUNCTION_PTR                  quiccInt;
    void *                              quiccArg;
    PMCQ1_FUNCTION_PTR                  maInt;
    void *                              maArg;
} PMCQ1BoardData, *PPMCQ1BoardData;

extern PPMCQ1BoardData  pmcq1BoardData;

/*
 * Function declarations
 */
extern unsigned int rsPMCQ1QuiccIntConnect(
  unsigned long         busNo, 
  unsigned long         slotNo, 
  unsigned long         funcNo, 
  PMCQ1_FUNCTION_PTR    routine, 
  void *                arg
);
unsigned int rsPMCQ1Init();
unsigned int rsPMCQ1MaIntConnect (
    unsigned long       busNo,  /* Pci Bus number of PMCQ1 */
    unsigned long       slotNo, /* Pci Slot number of PMCQ1 */
    unsigned long       funcNo, /* Pci Function number of PMCQ1 */
    PMCQ1_FUNCTION_PTR  routine,/* interrupt routine */
    void *              arg     /* argument to pass to interrupt routine */
);

void rsPMCQ1ShowIntrStatus(void );

#endif				/* __RSPMCQ1_H */
