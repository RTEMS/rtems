/*
 *  $Id$
 *
 *  Copyright (c) 2002 by Advent Networks, Inc.
 *          Jay Monkman <jmonkman@adventnetworks.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  This file is the include file for cpu_asm.S
 *
 *
 */

#ifndef __CPU_ASM_h
#define __CPU_ASM_h


/* Registers saved in context switch: */
.set REG_R0,      0
.set REG_R1,      4
.set REG_R2,      8
.set REG_R3,     12
.set REG_R4,     16
.set REG_R5,     20
.set REG_R6,     24
.set REG_R7,     28
.set REG_R8,     32
.set REG_R9,     36
.set REG_R10,    40
.set REG_R11,    44
.set REG_R12,    48
.set REG_SP,     52
.set REG_LR,     56
.set REG_PC,     60
.set REG_CPSR,   64
.set SIZE_REGS,  REG_CPSR + 4


#endif
