/*
 * Copyright (c) 2010-2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef BESTCOMM_OPS_H
#define BESTCOMM_OPS_H

#include <bsp/utility.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup BestCommOps BestComm Ops
 *
 * @ingroup BestComm
 *
 * @brief BestComm ops.
 *
 * @{
 */

#define VAR(i) (i)
#define VAR_COUNT 24
#define INC(i) (24 + (i))
#define INC_COUNT 8
#define IDX(i) (48 + (i))
#define IDX_COUNT 8

#define COND_ONCE 0
#define COND_LT 1
#define COND_GT 2
#define COND_NE 3
#define COND_EQ 4
#define COND_LE 5
#define COND_GE 6
#define COND_FOREVER 7

#define INC_INIT(cond, val) \
  (BSP_FLD32(cond, 29, 31) \
    | BSP_FLD32((int16_t) (val), 0, 15))

#define TERM_FIRST 0
#define TERM_SECOND 1
#define TERM_INIT 2
#define TERM_UNUSED 3

#define DEREF 1

#define LCD_TERM(val) BSP_FLD32(val, 13, 14)

#define LCD(deref0, iniidx0, deref1, iniidx1, term, termop, inc0, inc1) \
  (BSP_BIT32(31) \
    | BSP_FLD32(deref0, 29, 29) \
    | BSP_FLD32(iniidx0, 23, 28) \
    | BSP_FLD32(deref1, 21, 21) \
    | BSP_FLD32(iniidx1, 15, 20) \
    | LCD_TERM(term) \
    | BSP_FLD32(termop, 6, 11) \
    | BSP_FLD32(inc0, 3, 5) \
    | BSP_FLD32(inc1, 0, 2))

#define LCDEXT(deref0, iniidx0, deref1, iniidx1, term, termop, inc0, inc1) \
  (BSP_BIT32(30) \
    | LCD(deref0, iniidx0, deref1, iniidx1, term, termop, inc0, inc1))

#define LCDPLUS(deref0, iniidx0, deref1, iniidx1, term, termop, inc0, inc1) \
  (BSP_BIT32(22) \
    | LCD(deref0, iniidx0, deref1, iniidx1, term, termop, inc0, inc1))

#define LCDINIT(val) \
  (BSP_BIT32(31) \
    | BSP_FLD32((val) >> 13, 15, 29) \
    | LCD_TERM(TERM_INIT) \
    | BSP_FLD32(val, 0, 12))

#define MORE 0x4

#define TFD 0x2

#define INT 0x1

#define DRD_FLAGS(val) BSP_FLD32(val, 26, 28)

#define INIT_ALWAYS 0
#define INIT_SCTMR_0 1
#define INIT_SCTMR_1 2
#define INIT_FEC_RX 3
#define INIT_FEC_TX 4
#define INIT_ATA_RX 5
#define INIT_ATA_TX 6
#define INIT_SCPCI_RX 7
#define INIT_SCPCI_TX 8
#define INIT_PSC3_RX 9
#define INIT_PSC3_TX 10
#define INIT_PSC2_RX 11
#define INIT_PSC2_TX 12
#define INIT_PSC1_RX 13
#define INIT_PSC1_TX 14
#define INIT_SCTMR_2 15
#define INIT_SCLPC 16
#define INIT_PSC5_RX 17
#define INIT_PSC5_TX 18
#define INIT_PSC4_RX 19
#define INIT_PSC4_TX 20
#define INIT_I2C2_RX 21
#define INIT_I2C2_TX 22
#define INIT_I2C1_RX 23
#define INIT_I2C1_TX 24
#define INIT_PSC6_RX 25
#define INIT_PSC6_TX 26
#define INIT_IRDA_RX 25
#define INIT_IRDA_TX 26
#define INIT_SCTMR_3 27
#define INIT_SCTMR_4 28
#define INIT_SCTMR_5 29
#define INIT_SCTMR_6 30
#define INIT_SCTMR_7 31

#define DRD_INIT(val) BSP_FLD32(val, 21, 25)

#define SZ_8 1
#define SZ_16 2
#define SZ_32 0
#define SZ_DYN 3

#define DRD_RS(val) BSP_FLD32(val, 19, 20)

#define DRD_WS(val) BSP_FLD32(val, 17, 18)

#define DEST_VAR(val) (val)
#define DEST_IDX(val) (BSP_BIT32(5) | (val))
#define DEST_DEREF_IDX(val) (BSP_BIT32(5) | BSP_BIT32(4) | (val))

#define SRC_VAR(val) (val)
#define SRC_INC(val) (BSP_BIT32(5) | (val))
#define SRC_EU_RESULT (BSP_BIT32(5) | BSP_BIT32(4) | BSP_BIT32(1) | BSP_BIT32(0))
#define SRC_DEREF_EU_RESULT (BSP_BIT32(6) | BSP_BIT32(4) | BSP_BIT32(1) | BSP_BIT32(0))
#define SRC_IDX(val) (BSP_BIT32(6) | BSP_BIT32(5) | (val))
#define SRC_DEREF_IDX(val) (BSP_BIT32(6) | BSP_BIT32(5) | BSP_BIT32(4) | (val))
#define SRC_NONE (BSP_BIT32(5) | BSP_BIT32(4) | BSP_BIT32(3) | BSP_BIT32(2) | BSP_BIT32(1) | BSP_BIT32(0))

#define DRD1A(flags, init, dest, ws, src, rs) \
  (DRD_FLAGS(flags) \
    | DRD_INIT(init) \
    | DRD_RS(rs) \
    | DRD_WS(ws) \
    | BSP_FLD32(dest, 10, 15) \
    | BSP_FLD32(src, 3, 9))

#define DRD1AEURESULT(flags, init, dest, ws, rs) \
  (DRD1A(flags, init, rs, ws, dest, SRC_EU_RESULT) \
    | BSP_FLD32(1, 0, 3))

#define FUNC_LOAD_ACC 0
#define FUNC_UNLOAD_ACC 1
#define FUNC_AND 2
#define FUNC_OR 3
#define FUNC_XOR 4
#define FUNC_ANDN 5
#define FUNC_NOT 6
#define FUNC_ADD 7
#define FUNC_SUB 8
#define FUNC_LSH 9
#define FUNC_RSH 10
#define FUNC_CRC8 11
#define FUNC_CRC16 12
#define FUNC_CRC32 13
#define FUNC_ENDIAN32 14
#define FUNC_ENDIAN16 15

#define DRD2A(flags, func) \
  (BSP_BIT32(30) | BSP_BIT32(29) \
    | DRD_FLAGS(flags) \
    | BSP_FLD32(func, 0, 3))

#define DRD2A5(flags, init, func, ws, rs) \
  (DRD2A(flags, func) \
    | DRD_RS(rs) \
    | DRD_WS(ws) \
    | DRD_INIT(init))

#define OP_VAR(val) (val)
#define OP_EU_RESULT (BSP_BIT32(4) | BSP_BIT32(3) | BSP_BIT32(1) | BSP_BIT32(0))
#define OP_NONE (BSP_BIT32(4) | BSP_BIT32(3) | BSP_BIT32(2) | BSP_BIT32(1) | BSP_BIT32(0))
#define OP_IDX(val) (BSP_BIT32(5) | (val))
#define OP_DEREF_IDX(val) (BSP_BIT32(5) | BSP_BIT32(4) | (val))

#define DRD2B1(dest, op0, op1) \
  (BSP_FLD32(dest, 22, 27) \
    | BSP_FLD32(SRC_EU_RESULT, 14, 20) \
    | BSP_FLD32(3, 12, 13) \
    | BSP_FLD32(op0, 6, 11) \
    | BSP_FLD32(op1, 0, 5))

#define DRD2B2(op0, op1) \
  (BSP_BIT32(29) \
    | BSP_FLD32(3, 26, 27) \
    | BSP_FLD32(op0, 20, 25) \
    | BSP_FLD32(op1, 14, 19) \
    | BSP_FLD32(0, 12, 13) \
    | BSP_FLD32(OP_NONE, 6, 11) \
    | BSP_FLD32(OP_NONE, 0, 5))

#define NOP 0x1f8

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BESTCOMM_OPS_H */
