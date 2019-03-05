/**
 *  @file
 *
 *  @ingroup RTEMSBSPsARMShared
 *
 *  @brief ARM PL011 Register definitions
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_SHARED_ARM_PL011_REGS_H
#define LIBBSP_ARM_SHARED_ARM_PL011_REGS_H

#include <bsp/utility.h>

typedef struct {
  uint32_t uartdr;
#define PL011_UARTDR_OE BSP_BIT32(11)
#define PL011_UARTDR_BE BSP_BIT32(10)
#define PL011_UARTDR_PE BSP_BIT32(9)
#define PL011_UARTDR_FE BSP_BIT32(8)
#define PL011_UARTDR_DATA(val) BSP_FLD32(val, 0, 7)
#define PL011_UARTDR_DATA_GET(reg) BSP_FLD32GET(reg, 0, 7)
#define PL011_UARTDR_DATA_SET(reg, val) BSP_FLD32SET(reg, val, 0, 7)
  uint32_t uartrsr_uartecr;
#define PL011_UARTRSR_UARTECR_OE BSP_BIT32(3)
#define PL011_UARTRSR_UARTECR_BE BSP_BIT32(2)
#define PL011_UARTRSR_UARTECR_PE BSP_BIT32(1)
#define PL011_UARTRSR_UARTECR_FE BSP_BIT32(0)
  uint32_t reserved_08[4];
  uint32_t uartfr;
#define PL011_UARTFR_RI BSP_BIT32(8)
#define PL011_UARTFR_TXFE BSP_BIT32(7)
#define PL011_UARTFR_RXFF BSP_BIT32(6)
#define PL011_UARTFR_TXFF BSP_BIT32(5)
#define PL011_UARTFR_RXFE BSP_BIT32(4)
#define PL011_UARTFR_BUSY BSP_BIT32(3)
#define PL011_UARTFR_DCD BSP_BIT32(2)
#define PL011_UARTFR_DSR BSP_BIT32(1)
#define PL011_UARTFR_CTS BSP_BIT32(0)
  uint32_t reserved_1c;
  uint32_t uartilpr;
#define PL011_UARTILPR_ILPDVSR(val) BSP_FLD32(val, 0, 7)
#define PL011_UARTILPR_ILPDVSR_GET(reg) BSP_FLD32GET(reg, 0, 7)
#define PL011_UARTILPR_ILPDVSR_SET(reg, val) BSP_FLD32SET(reg, val, 0, 7)
  uint32_t uartibrd;
#define PL011_UARTIBRD_BAUD_DIVINT(val) BSP_FLD32(val, 0, 15)
#define PL011_UARTIBRD_BAUD_DIVINT_GET(reg) BSP_FLD32GET(reg, 0, 15)
#define PL011_UARTIBRD_BAUD_DIVINT_SET(reg, val) BSP_FLD32SET(reg, val, 0, 15)
  uint32_t uartfbrd;
#define PL011_UARTFBRD_BAUD_DIVFRAC(val) BSP_FLD32(val, 0, 5)
#define PL011_UARTFBRD_BAUD_DIVFRAC_GET(reg) BSP_FLD32GET(reg, 0, 5)
#define PL011_UARTFBRD_BAUD_DIVFRAC_SET(reg, val) BSP_FLD32SET(reg, val, 0, 5)
  uint32_t uartlcr_h;
#define PL011_UARTLCR_H_SPS BSP_BIT32(7)
#define PL011_UARTLCR_H_WLEN(val) BSP_FLD32(val, 5, 6)
#define PL011_UARTLCR_H_WLEN_GET(reg) BSP_FLD32GET(reg, 5, 6)
#define PL011_UARTLCR_H_WLEN_SET(reg, val) BSP_FLD32SET(reg, val, 5, 6)
#define PL011_UARTLCR_H_WLEN_5 0x00U
#define PL011_UARTLCR_H_WLEN_6 0x01U
#define PL011_UARTLCR_H_WLEN_7 0x02U
#define PL011_UARTLCR_H_WLEN_8 0x03U
#define PL011_UARTLCR_H_FEN BSP_BIT32(4)
#define PL011_UARTLCR_H_STP2 BSP_BIT32(3)
#define PL011_UARTLCR_H_EPS BSP_BIT32(2)
#define PL011_UARTLCR_H_PEN BSP_BIT32(1)
#define PL011_UARTLCR_H_BRK BSP_BIT32(0)
  uint32_t uartcr;
#define PL011_UARTCR_CTSEN BSP_BIT32(15)
#define PL011_UARTCR_RTSEN BSP_BIT32(14)
#define PL011_UARTCR_OUT2 BSP_BIT32(13)
#define PL011_UARTCR_OUT1 BSP_BIT32(12)
#define PL011_UARTCR_RTS BSP_BIT32(11)
#define PL011_UARTCR_DTR BSP_BIT32(10)
#define PL011_UARTCR_RXE BSP_BIT32(9)
#define PL011_UARTCR_TXE BSP_BIT32(8)
#define PL011_UARTCR_LBE BSP_BIT32(7)
#define PL011_UARTCR_SIRLP BSP_BIT32(3)
#define PL011_UARTCR_SIREN BSP_BIT32(2)
#define PL011_UARTCR_UARTEN BSP_BIT32(1)
  uint32_t uartifls;
#define PL011_UARTIFLS_RXIFLSEL(val) BSP_FLD32(val, 3, 5)
#define PL011_UARTIFLS_RXIFLSEL_GET(reg) BSP_FLD32GET(reg, 3, 5)
#define PL011_UARTIFLS_RXIFLSEL_SET(reg, val) BSP_FLD32SET(reg, val, 3, 5)
#define PL011_UARTIFLS_TXIFLSEL(val) BSP_FLD32(val, 0, 2)
#define PL011_UARTIFLS_TXIFLSEL_GET(reg) BSP_FLD32GET(reg, 0, 2)
#define PL011_UARTIFLS_TXIFLSEL_SET(reg, val) BSP_FLD32SET(reg, val, 0, 2)
  uint32_t uartimsc;
  uint32_t uartris;
  uint32_t uartmis;
  uint32_t uarticr;
#define PL011_UARTI_OEI BSP_BIT32(10)
#define PL011_UARTI_BEI BSP_BIT32(9)
#define PL011_UARTI_PEI BSP_BIT32(8)
#define PL011_UARTI_FEI BSP_BIT32(7)
#define PL011_UARTI_RTI BSP_BIT32(6)
#define PL011_UARTI_TXI BSP_BIT32(5)
#define PL011_UARTI_RXI BSP_BIT32(4)
#define PL011_UARTI_DSRMI BSP_BIT32(3)
#define PL011_UARTI_DCDMI BSP_BIT32(2)
#define PL011_UARTI_CTSMI BSP_BIT32(1)
#define PL011_UARTI_RIMI BSP_BIT32(0)
  uint32_t uartdmacr;
#define PL011_UARTDMACR_DMAONERR BSP_BIT32(2)
#define PL011_UARTDMACR_TXDMAE BSP_BIT32(1)
#define PL011_UARTDMACR_RXDMAE BSP_BIT32(0)
  uint32_t reserved_4c[997];
  uint32_t uartperiphid0;
  uint32_t uartperiphid1;
  uint32_t uartperiphid2;
  uint32_t uartperiphid3;
  uint32_t uartpcellid0;
  uint32_t uartpcellid1;
  uint32_t uartpcellid2;
  uint32_t uartpcellid3;
} pl011;

#endif /* LIBBSP_ARM_SHARED_ARM_PL011_REGS_H */
