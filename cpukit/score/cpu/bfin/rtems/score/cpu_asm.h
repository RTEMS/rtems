/**
 * @file rtems/score/cpu_asm.h
 */

/*
 *  Defines a couple of Macros used in cpu_asm.S
 * 
 *  COPYRIGHT (c) 2006 by Atos Automacao Industrial Ltda.
 *             written by Alain Schaefer <alain.schaefer@easc.ch>
 *                    and Antonio Giovanini <antonio@atos.com.br>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 *
 */

#ifndef _RTEMS_SCORE_CPU_ASM_H
#define _RTEMS_SCORE_CPU_ASM_H

/* offsets for the registers in the thread context */
#define R0_OFFSET     0
#define R1_OFFSET     4
#define R2_OFFSET     8
#define R3_OFFSET     12
#define R4_OFFSET     16
#define R5_OFFSET     20
#define R6_OFFSET     24
#define R7_OFFSET     28
#define P0_OFFSET     32
#define P1_OFFSET     36
#define P2_OFFSET     40
#define P3_OFFSET     44
#define P4_OFFSET     48
#define P5_OFFSET     52
#define FP_OFFSET     56
#define SP_OFFSET     60

#define I0_OFFSET     64
#define I1_OFFSET     68
#define I2_OFFSET     72
#define I3_OFFSET     76

#define M0_OFFSET     80
#define M1_OFFSET     84
#define M2_OFFSET     88
#define M3_OFFSET     92

#define B0_OFFSET     96
#define B1_OFFSET     100
#define B2_OFFSET     104
#define B3_OFFSET     108

#define L0_OFFSET     112
#define L1_OFFSET     116
#define L2_OFFSET     120
#define L3_OFFSET     124

#define A0X_OFFSET     128
#define A0W_OFFSET     132
#define A1X_OFFSET     136
#define A1W_OFFSET     140

#define ASTAT_OFFSET   144
#define RETS_OFFSET    148
#define LC0_OFFSET     152
#define LT0_OFFSET     156

#define LB0_OFFSET     160
#define LC1_OFFSET     164
#define LT1_OFFSET     168
#define LB1_OFFSET     172

#define USP_OFFSET     174
#define SEQSTAT_OFFSET 178
#define SYSCFG_OFFSET  182
#define RETI_OFFSET    184

#define RETX_OFFSET    188
#define RETN_OFFSET    192
#define RETE_OFFSET    296

#define PC_OFFSET      200


#endif

/* end of file */
