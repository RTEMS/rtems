/**
 * @file
 *
 * @ingroup ScoreCPU
 *
 * @brief ARM assembler support API.
 */

/*
 *  COPYRIGHT (c) 2002 by Advent Networks, Inc.
 *          Jay Monkman <jmonkman@adventnetworks.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  This file is the include file for cpu_asm.S
 */

#ifndef _RTEMS_SCORE_CPU_ASM_H
#define _RTEMS_SCORE_CPU_ASM_H


/* Registers saved in context switch: */
.set REG_CPSR,   0
.set REG_R4,     4
.set REG_R5,     8
.set REG_R6,     12
.set REG_R7,     16
.set REG_R8,     20
.set REG_R9,     24
.set REG_R10,    28
.set REG_R11,    32
.set REG_SP,     36
.set REG_LR,     40
.set REG_PC,     44
.set SIZE_REGS,  REG_PC + 4

#endif
