/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUARMSysReg
 *
 * @brief This header file provides the API to read and write the AArch32
 *   system registers.
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTEMS_SCORE_AARCH32_SYSTEM_REGISTERS_H
#define _RTEMS_SCORE_AARCH32_SYSTEM_REGISTERS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScoreCPUARMSysReg AArch32 System Registers
 *
 * @ingroup RTEMSScoreCPUARM
 *
 * @brief This group provides functions to read and write the AArch32 system
 *   registers.
 *
 * The ordering of the registers is according to the document
 * "Arm Architecture Reference Manual, Armv8, for Armv8-A architecture profile,
 * Issue F.c" followed by the additional registers defined by the document
 * "Arm Architecture Reference Manual, Armv8, for the Armv8-R AArch32
 * architecture profile, Issue A.c".
 *
 * @{
 */

/* ACTLR, Auxiliary Control Register */

static inline uint32_t _AArch32_Read_actlr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c1, c0, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_actlr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c1, c0, 1" : : "r" ( value ) : "memory"
  );
}

/* ACTLR2, Auxiliary Control Register 2 */

static inline uint32_t _AArch32_Read_actlr2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c1, c0, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_actlr2( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c1, c0, 3" : : "r" ( value ) : "memory"
  );
}

/* ADFSR, Auxiliary Data Fault Status Register */

static inline uint32_t _AArch32_Read_adfsr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c5, c1, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_adfsr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c5, c1, 0" : : "r" ( value ) : "memory"
  );
}

/* AIDR, Auxiliary ID Register */

static inline uint32_t _AArch32_Read_aidr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c0, c0, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AIFSR, Auxiliary Instruction Fault Status Register */

static inline uint32_t _AArch32_Read_aifsr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c5, c1, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_aifsr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c5, c1, 1" : : "r" ( value ) : "memory"
  );
}

/* AMAIR0, Auxiliary Memory Attribute Indirection Register 0 */

static inline uint32_t _AArch32_Read_amair0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c10, c3, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amair0( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c10, c3, 0" : : "r" ( value ) : "memory"
  );
}

/* AMAIR1, Auxiliary Memory Attribute Indirection Register 1 */

static inline uint32_t _AArch32_Read_amair1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c10, c3, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amair1( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c10, c3, 1" : : "r" ( value ) : "memory"
  );
}

/* APSR, Application Program Status Register */

#define AARCH32_APSR_GE( _val ) ( ( _val ) << 16 )
#define AARCH32_APSR_GE_SHIFT 16
#define AARCH32_APSR_GE_MASK 0xf0000U
#define AARCH32_APSR_GE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_APSR_Q 0x8000000U

#define AARCH32_APSR_V 0x10000000U

#define AARCH32_APSR_C 0x20000000U

#define AARCH32_APSR_Z 0x40000000U

#define AARCH32_APSR_N 0x80000000U

/* ATS12NSOPR, Address Translate Stages 1 and 2 Non-secure Only PL1 Read */

static inline void _AArch32_Write_ats12nsopr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c8, 4" : : "r" ( value ) : "memory"
  );
}

/* ATS12NSOPW, Address Translate Stages 1 and 2 Non-secure Only PL1 Write */

static inline void _AArch32_Write_ats12nsopw( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c8, 5" : : "r" ( value ) : "memory"
  );
}

/* ATS12NSOUR, Address Translate Stages 1 and 2 Non-secure Only Unprivileged Read */

static inline void _AArch32_Write_ats12nsour( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c8, 6" : : "r" ( value ) : "memory"
  );
}

/* ATS12NSOUW, Address Translate Stages 1 and 2 Non-secure Only Unprivileged Write */

static inline void _AArch32_Write_ats12nsouw( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c8, 7" : : "r" ( value ) : "memory"
  );
}

/* ATS1CPR, Address Translate Stage 1 Current state PL1 Read */

static inline void _AArch32_Write_ats1cpr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c8, 0" : : "r" ( value ) : "memory"
  );
}

/* ATS1CPRP, Address Translate Stage 1 Current state PL1 Read PAN */

static inline void _AArch32_Write_ats1cprp( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c9, 0" : : "r" ( value ) : "memory"
  );
}

/* ATS1CPW, Address Translate Stage 1 Current state PL1 Write */

static inline void _AArch32_Write_ats1cpw( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c8, 1" : : "r" ( value ) : "memory"
  );
}

/* ATS1CPWP, Address Translate Stage 1 Current state PL1 Write PAN */

static inline void _AArch32_Write_ats1cpwp( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c9, 1" : : "r" ( value ) : "memory"
  );
}

/* ATS1CUR, Address Translate Stage 1 Current state Unprivileged Read */

static inline void _AArch32_Write_ats1cur( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c8, 2" : : "r" ( value ) : "memory"
  );
}

/* ATS1CUW, Address Translate Stage 1 Current state Unprivileged Write */

static inline void _AArch32_Write_ats1cuw( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c8, 3" : : "r" ( value ) : "memory"
  );
}

/* ATS1HR, Address Translate Stage 1 Hyp mode Read */

static inline void _AArch32_Write_ats1hr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c7, c8, 0" : : "r" ( value ) : "memory"
  );
}

/* ATS1HW, Address Translate Stage 1 Hyp mode Write */

static inline void _AArch32_Write_ats1hw( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c7, c8, 1" : : "r" ( value ) : "memory"
  );
}

/* BPIALL, Branch Predictor Invalidate All */

static inline void _AArch32_Write_bpiall( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c5, 6" : : "r" ( value ) : "memory"
  );
}

/* BPIALLIS, Branch Predictor Invalidate All, Inner Shareable */

static inline void _AArch32_Write_bpiallis( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c1, 6" : : "r" ( value ) : "memory"
  );
}

/* BPIMVA, Branch Predictor Invalidate by VA */

static inline void _AArch32_Write_bpimva( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c5, 7" : : "r" ( value ) : "memory"
  );
}

/* CCSIDR, Current Cache Size ID Register */

#define AARCH32_CCSIDR_LINESIZE( _val ) ( ( _val ) << 0 )
#define AARCH32_CCSIDR_LINESIZE_SHIFT 0
#define AARCH32_CCSIDR_LINESIZE_MASK 0x7U
#define AARCH32_CCSIDR_LINESIZE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x7U )

#define AARCH32_CCSIDR_ASSOCIATIVITY_0( _val ) ( ( _val ) << 3 )
#define AARCH32_CCSIDR_ASSOCIATIVITY_SHIFT_0 3
#define AARCH32_CCSIDR_ASSOCIATIVITY_MASK_0 0x1ff8U
#define AARCH32_CCSIDR_ASSOCIATIVITY_GET_0( _reg ) \
  ( ( ( _reg ) >> 3 ) & 0x3ffU )

#define AARCH32_CCSIDR_ASSOCIATIVITY_1( _val ) ( ( _val ) << 3 )
#define AARCH32_CCSIDR_ASSOCIATIVITY_SHIFT_1 3
#define AARCH32_CCSIDR_ASSOCIATIVITY_MASK_1 0xfffff8U
#define AARCH32_CCSIDR_ASSOCIATIVITY_GET_1( _reg ) \
  ( ( ( _reg ) >> 3 ) & 0x1fffffU )

#define AARCH32_CCSIDR_NUMSETS( _val ) ( ( _val ) << 13 )
#define AARCH32_CCSIDR_NUMSETS_SHIFT 13
#define AARCH32_CCSIDR_NUMSETS_MASK 0xfffe000U
#define AARCH32_CCSIDR_NUMSETS_GET( _reg ) \
  ( ( ( _reg ) >> 13 ) & 0x7fffU )

static inline uint32_t _AArch32_Read_ccsidr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c0, c0, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* CCSIDR2, Current Cache Size ID Register 2 */

#define AARCH32_CCSIDR2_NUMSETS( _val ) ( ( _val ) << 0 )
#define AARCH32_CCSIDR2_NUMSETS_SHIFT 0
#define AARCH32_CCSIDR2_NUMSETS_MASK 0xffffffU
#define AARCH32_CCSIDR2_NUMSETS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffU )

static inline uint32_t _AArch32_Read_ccsidr2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c0, c0, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* CFPRCTX, Control Flow Prediction Restriction by Context */

#define AARCH32_CFPRCTX_ASID( _val ) ( ( _val ) << 0 )
#define AARCH32_CFPRCTX_ASID_SHIFT 0
#define AARCH32_CFPRCTX_ASID_MASK 0xffU
#define AARCH32_CFPRCTX_ASID_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

#define AARCH32_CFPRCTX_GASID 0x100U

#define AARCH32_CFPRCTX_VMID( _val ) ( ( _val ) << 16 )
#define AARCH32_CFPRCTX_VMID_SHIFT 16
#define AARCH32_CFPRCTX_VMID_MASK 0xff0000U
#define AARCH32_CFPRCTX_VMID_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xffU )

#define AARCH32_CFPRCTX_EL( _val ) ( ( _val ) << 24 )
#define AARCH32_CFPRCTX_EL_SHIFT 24
#define AARCH32_CFPRCTX_EL_MASK 0x3000000U
#define AARCH32_CFPRCTX_EL_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0x3U )

#define AARCH32_CFPRCTX_NS 0x4000000U

#define AARCH32_CFPRCTX_GVMID 0x8000000U

static inline void _AArch32_Write_cfprctx( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c3, 4" : : "r" ( value ) : "memory"
  );
}

/* CLIDR, Cache Level ID Register */

#define AARCH32_CLIDR_LOUIS( _val ) ( ( _val ) << 21 )
#define AARCH32_CLIDR_LOUIS_SHIFT 21
#define AARCH32_CLIDR_LOUIS_MASK 0xe00000U
#define AARCH32_CLIDR_LOUIS_GET( _reg ) \
  ( ( ( _reg ) >> 21 ) & 0x7U )

#define AARCH32_CLIDR_LOC( _val ) ( ( _val ) << 24 )
#define AARCH32_CLIDR_LOC_SHIFT 24
#define AARCH32_CLIDR_LOC_MASK 0x7000000U
#define AARCH32_CLIDR_LOC_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0x7U )

#define AARCH32_CLIDR_LOUU( _val ) ( ( _val ) << 27 )
#define AARCH32_CLIDR_LOUU_SHIFT 27
#define AARCH32_CLIDR_LOUU_MASK 0x38000000U
#define AARCH32_CLIDR_LOUU_GET( _reg ) \
  ( ( ( _reg ) >> 27 ) & 0x7U )

#define AARCH32_CLIDR_ICB( _val ) ( ( _val ) << 30 )
#define AARCH32_CLIDR_ICB_SHIFT 30
#define AARCH32_CLIDR_ICB_MASK 0xc0000000U
#define AARCH32_CLIDR_ICB_GET( _reg ) \
  ( ( ( _reg ) >> 30 ) & 0x3U )

static inline uint32_t _AArch32_Read_clidr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c0, c0, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* CONTEXTIDR, Context ID Register */

#define AARCH32_CONTEXTIDR_ASID( _val ) ( ( _val ) << 0 )
#define AARCH32_CONTEXTIDR_ASID_SHIFT 0
#define AARCH32_CONTEXTIDR_ASID_MASK 0xffU
#define AARCH32_CONTEXTIDR_ASID_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

#define AARCH32_CONTEXTIDR_PROCID( _val ) ( ( _val ) << 8 )
#define AARCH32_CONTEXTIDR_PROCID_SHIFT 8
#define AARCH32_CONTEXTIDR_PROCID_MASK 0xffffff00U
#define AARCH32_CONTEXTIDR_PROCID_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xffffffU )

static inline uint32_t _AArch32_Read_contextidr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c0, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_contextidr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c0, 1" : : "r" ( value ) : "memory"
  );
}

/* CP15DMB, Data Memory Barrier System instruction */

static inline void _AArch32_Write_cp15dmb( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c10, 5" : : "r" ( value ) : "memory"
  );
}

/* CP15DSB, Data Synchronization Barrier System instruction */

static inline void _AArch32_Write_cp15dsb( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c10, 4" : : "r" ( value ) : "memory"
  );
}

/* CP15ISB, Instruction Synchronization Barrier System instruction */

static inline void _AArch32_Write_cp15isb( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c5, 4" : : "r" ( value ) : "memory"
  );
}

/* CPACR, Architectural Feature Access Control Register */

#define AARCH32_CPACR_CP10( _val ) ( ( _val ) << 20 )
#define AARCH32_CPACR_CP10_SHIFT 20
#define AARCH32_CPACR_CP10_MASK 0x300000U
#define AARCH32_CPACR_CP10_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0x3U )

#define AARCH32_CPACR_CP11( _val ) ( ( _val ) << 22 )
#define AARCH32_CPACR_CP11_SHIFT 22
#define AARCH32_CPACR_CP11_MASK 0xc00000U
#define AARCH32_CPACR_CP11_GET( _reg ) \
  ( ( ( _reg ) >> 22 ) & 0x3U )

#define AARCH32_CPACR_TRCDIS 0x10000000U

#define AARCH32_CPACR_ASEDIS 0x80000000U

static inline uint32_t _AArch32_Read_cpacr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c1, c0, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_cpacr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c1, c0, 2" : : "r" ( value ) : "memory"
  );
}

/* CPSR, Current Program Status Register */

#define AARCH32_CPSR_M( _val ) ( ( _val ) << 0 )
#define AARCH32_CPSR_M_SHIFT 0
#define AARCH32_CPSR_M_MASK 0xfU
#define AARCH32_CPSR_M_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_CPSR_F 0x40U

#define AARCH32_CPSR_I 0x80U

#define AARCH32_CPSR_A 0x100U

#define AARCH32_CPSR_E 0x200U

#define AARCH32_CPSR_GE( _val ) ( ( _val ) << 16 )
#define AARCH32_CPSR_GE_SHIFT 16
#define AARCH32_CPSR_GE_MASK 0xf0000U
#define AARCH32_CPSR_GE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_CPSR_DIT 0x200000U

#define AARCH32_CPSR_PAN 0x400000U

#define AARCH32_CPSR_SSBS 0x800000U

#define AARCH32_CPSR_Q 0x8000000U

#define AARCH32_CPSR_V 0x10000000U

#define AARCH32_CPSR_C 0x20000000U

#define AARCH32_CPSR_Z 0x40000000U

#define AARCH32_CPSR_N 0x80000000U

/* CPPRCTX, Cache Prefetch Prediction Restriction by Context */

#define AARCH32_CPPRCTX_ASID( _val ) ( ( _val ) << 0 )
#define AARCH32_CPPRCTX_ASID_SHIFT 0
#define AARCH32_CPPRCTX_ASID_MASK 0xffU
#define AARCH32_CPPRCTX_ASID_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

#define AARCH32_CPPRCTX_GASID 0x100U

#define AARCH32_CPPRCTX_VMID( _val ) ( ( _val ) << 16 )
#define AARCH32_CPPRCTX_VMID_SHIFT 16
#define AARCH32_CPPRCTX_VMID_MASK 0xff0000U
#define AARCH32_CPPRCTX_VMID_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xffU )

#define AARCH32_CPPRCTX_EL( _val ) ( ( _val ) << 24 )
#define AARCH32_CPPRCTX_EL_SHIFT 24
#define AARCH32_CPPRCTX_EL_MASK 0x3000000U
#define AARCH32_CPPRCTX_EL_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0x3U )

#define AARCH32_CPPRCTX_NS 0x4000000U

#define AARCH32_CPPRCTX_GVMID 0x8000000U

static inline void _AArch32_Write_cpprctx( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c3, 7" : : "r" ( value ) : "memory"
  );
}

/* CSSELR, Cache Size Selection Register */

#define AARCH32_CSSELR_IND 0x1U

#define AARCH32_CSSELR_LEVEL( _val ) ( ( _val ) << 1 )
#define AARCH32_CSSELR_LEVEL_SHIFT 1
#define AARCH32_CSSELR_LEVEL_MASK 0xeU
#define AARCH32_CSSELR_LEVEL_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x7U )

static inline uint32_t _AArch32_Read_csselr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 2, %0, c0, c0, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_csselr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 2, %0, c0, c0, 0" : : "r" ( value ) : "memory"
  );
}

/* CTR, Cache Type Register */

#define AARCH32_CTR_IMINLINE( _val ) ( ( _val ) << 0 )
#define AARCH32_CTR_IMINLINE_SHIFT 0
#define AARCH32_CTR_IMINLINE_MASK 0xfU
#define AARCH32_CTR_IMINLINE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_CTR_L1IP( _val ) ( ( _val ) << 14 )
#define AARCH32_CTR_L1IP_SHIFT 14
#define AARCH32_CTR_L1IP_MASK 0xc000U
#define AARCH32_CTR_L1IP_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x3U )

#define AARCH32_CTR_DMINLINE( _val ) ( ( _val ) << 16 )
#define AARCH32_CTR_DMINLINE_SHIFT 16
#define AARCH32_CTR_DMINLINE_MASK 0xf0000U
#define AARCH32_CTR_DMINLINE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_CTR_ERG( _val ) ( ( _val ) << 20 )
#define AARCH32_CTR_ERG_SHIFT 20
#define AARCH32_CTR_ERG_MASK 0xf00000U
#define AARCH32_CTR_ERG_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH32_CTR_CWG( _val ) ( ( _val ) << 24 )
#define AARCH32_CTR_CWG_SHIFT 24
#define AARCH32_CTR_CWG_MASK 0xf000000U
#define AARCH32_CTR_CWG_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH32_CTR_IDC 0x10000000U

#define AARCH32_CTR_DIC 0x20000000U

static inline uint32_t _AArch32_Read_ctr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c0, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* DACR, Domain Access Control Register */

static inline uint32_t _AArch32_Read_dacr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c3, c0, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dacr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c3, c0, 0" : : "r" ( value ) : "memory"
  );
}

/* DCCIMVAC, Data Cache line Clean and Invalidate by VA to PoC */

static inline void _AArch32_Write_dccimvac( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c14, 1" : : "r" ( value ) : "memory"
  );
}

/* DCCISW, Data Cache line Clean and Invalidate by Set/Way */

#define AARCH32_DCCISW_LEVEL( _val ) ( ( _val ) << 1 )
#define AARCH32_DCCISW_LEVEL_SHIFT 1
#define AARCH32_DCCISW_LEVEL_MASK 0xeU
#define AARCH32_DCCISW_LEVEL_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x7U )

#define AARCH32_DCCISW_SETWAY( _val ) ( ( _val ) << 4 )
#define AARCH32_DCCISW_SETWAY_SHIFT 4
#define AARCH32_DCCISW_SETWAY_MASK 0xfffffff0U
#define AARCH32_DCCISW_SETWAY_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfffffffU )

static inline void _AArch32_Write_dccisw( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c14, 2" : : "r" ( value ) : "memory"
  );
}

/* DCCMVAC, Data Cache line Clean by VA to PoC */

static inline void _AArch32_Write_dccmvac( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c10, 1" : : "r" ( value ) : "memory"
  );
}

/* DCCMVAU, Data Cache line Clean by VA to PoU */

static inline void _AArch32_Write_dccmvau( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c11, 1" : : "r" ( value ) : "memory"
  );
}

/* DCCSW, Data Cache line Clean by Set/Way */

#define AARCH32_DCCSW_LEVEL( _val ) ( ( _val ) << 1 )
#define AARCH32_DCCSW_LEVEL_SHIFT 1
#define AARCH32_DCCSW_LEVEL_MASK 0xeU
#define AARCH32_DCCSW_LEVEL_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x7U )

#define AARCH32_DCCSW_SETWAY( _val ) ( ( _val ) << 4 )
#define AARCH32_DCCSW_SETWAY_SHIFT 4
#define AARCH32_DCCSW_SETWAY_MASK 0xfffffff0U
#define AARCH32_DCCSW_SETWAY_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfffffffU )

static inline void _AArch32_Write_dccsw( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c10, 2" : : "r" ( value ) : "memory"
  );
}

/* DCIMVAC, Data Cache line Invalidate by VA to PoC */

static inline void _AArch32_Write_dcimvac( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c6, 1" : : "r" ( value ) : "memory"
  );
}

/* DCISW, Data Cache line Invalidate by Set/Way */

#define AARCH32_DCISW_LEVEL( _val ) ( ( _val ) << 1 )
#define AARCH32_DCISW_LEVEL_SHIFT 1
#define AARCH32_DCISW_LEVEL_MASK 0xeU
#define AARCH32_DCISW_LEVEL_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x7U )

#define AARCH32_DCISW_SETWAY( _val ) ( ( _val ) << 4 )
#define AARCH32_DCISW_SETWAY_SHIFT 4
#define AARCH32_DCISW_SETWAY_MASK 0xfffffff0U
#define AARCH32_DCISW_SETWAY_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfffffffU )

static inline void _AArch32_Write_dcisw( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c6, 2" : : "r" ( value ) : "memory"
  );
}

/* DFAR, Data Fault Address Register */

static inline uint32_t _AArch32_Read_dfar( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c0, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dfar( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c0, 0" : : "r" ( value ) : "memory"
  );
}

/* DFSR, Data Fault Status Register */

#define AARCH32_DFSR_FS_3_0( _val ) ( ( _val ) << 0 )
#define AARCH32_DFSR_FS_3_0_SHIFT 0
#define AARCH32_DFSR_FS_3_0_MASK 0xfU
#define AARCH32_DFSR_FS_3_0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_DFSR_STATUS( _val ) ( ( _val ) << 0 )
#define AARCH32_DFSR_STATUS_SHIFT 0
#define AARCH32_DFSR_STATUS_MASK 0x3fU
#define AARCH32_DFSR_STATUS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )

#define AARCH32_DFSR_DOMAIN( _val ) ( ( _val ) << 4 )
#define AARCH32_DFSR_DOMAIN_SHIFT 4
#define AARCH32_DFSR_DOMAIN_MASK 0xf0U
#define AARCH32_DFSR_DOMAIN_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH32_DFSR_LPAE 0x200U

#define AARCH32_DFSR_FS_4 0x400U

#define AARCH32_DFSR_WNR 0x800U

#define AARCH32_DFSR_EXT 0x1000U

#define AARCH32_DFSR_CM 0x2000U

#define AARCH32_DFSR_AET( _val ) ( ( _val ) << 14 )
#define AARCH32_DFSR_AET_SHIFT 14
#define AARCH32_DFSR_AET_MASK 0xc000U
#define AARCH32_DFSR_AET_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x3U )

#define AARCH32_DFSR_FNV 0x10000U

static inline uint32_t _AArch32_Read_dfsr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c5, c0, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dfsr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c5, c0, 0" : : "r" ( value ) : "memory"
  );
}

/* DTLBIALL, Data TLB Invalidate All */

static inline void _AArch32_Write_dtlbiall( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c8, c6, 0" : : "r" ( value ) : "memory"
  );
}

/* DTLBIASID, Data TLB Invalidate by ASID match */

#define AARCH32_DTLBIASID_ASID( _val ) ( ( _val ) << 0 )
#define AARCH32_DTLBIASID_ASID_SHIFT 0
#define AARCH32_DTLBIASID_ASID_MASK 0xffU
#define AARCH32_DTLBIASID_ASID_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

static inline void _AArch32_Write_dtlbiasid( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c8, c6, 2" : : "r" ( value ) : "memory"
  );
}

/* DTLBIMVA, Data TLB Invalidate by VA */

#define AARCH32_DTLBIMVA_ASID( _val ) ( ( _val ) << 0 )
#define AARCH32_DTLBIMVA_ASID_SHIFT 0
#define AARCH32_DTLBIMVA_ASID_MASK 0xffU
#define AARCH32_DTLBIMVA_ASID_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

#define AARCH32_DTLBIMVA_VA( _val ) ( ( _val ) << 12 )
#define AARCH32_DTLBIMVA_VA_SHIFT 12
#define AARCH32_DTLBIMVA_VA_MASK 0xfffff000U
#define AARCH32_DTLBIMVA_VA_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfffffU )

static inline void _AArch32_Write_dtlbimva( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c8, c6, 1" : : "r" ( value ) : "memory"
  );
}

/* DVPRCTX, Data Value Prediction Restriction by Context */

#define AARCH32_DVPRCTX_ASID( _val ) ( ( _val ) << 0 )
#define AARCH32_DVPRCTX_ASID_SHIFT 0
#define AARCH32_DVPRCTX_ASID_MASK 0xffU
#define AARCH32_DVPRCTX_ASID_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

#define AARCH32_DVPRCTX_GASID 0x100U

#define AARCH32_DVPRCTX_VMID( _val ) ( ( _val ) << 16 )
#define AARCH32_DVPRCTX_VMID_SHIFT 16
#define AARCH32_DVPRCTX_VMID_MASK 0xff0000U
#define AARCH32_DVPRCTX_VMID_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xffU )

#define AARCH32_DVPRCTX_EL( _val ) ( ( _val ) << 24 )
#define AARCH32_DVPRCTX_EL_SHIFT 24
#define AARCH32_DVPRCTX_EL_MASK 0x3000000U
#define AARCH32_DVPRCTX_EL_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0x3U )

#define AARCH32_DVPRCTX_NS 0x4000000U

#define AARCH32_DVPRCTX_GVMID 0x8000000U

static inline void _AArch32_Write_dvprctx( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c3, 5" : : "r" ( value ) : "memory"
  );
}

/* FCSEIDR, FCSE Process ID Register */

static inline uint32_t _AArch32_Read_fcseidr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c0, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_fcseidr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c0, 0" : : "r" ( value ) : "memory"
  );
}

/* FPEXC, Floating-Point Exception Control Register */

#define AARCH32_FPEXC_IOF 0x1U

#define AARCH32_FPEXC_DZF 0x2U

#define AARCH32_FPEXC_OFF 0x4U

#define AARCH32_FPEXC_UFF 0x8U

#define AARCH32_FPEXC_IXF 0x10U

#define AARCH32_FPEXC_IDF 0x80U

#define AARCH32_FPEXC_VECITR( _val ) ( ( _val ) << 8 )
#define AARCH32_FPEXC_VECITR_SHIFT 8
#define AARCH32_FPEXC_VECITR_MASK 0x700U
#define AARCH32_FPEXC_VECITR_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0x7U )

#define AARCH32_FPEXC_TFV 0x4000000U

#define AARCH32_FPEXC_VV 0x8000000U

#define AARCH32_FPEXC_FP2V 0x10000000U

#define AARCH32_FPEXC_DEX 0x20000000U

#define AARCH32_FPEXC_EN 0x40000000U

#define AARCH32_FPEXC_EX 0x80000000U

/* FPSCR, Floating-Point Status and Control Register */

#define AARCH32_FPSCR_IOC 0x1U

#define AARCH32_FPSCR_DZC 0x2U

#define AARCH32_FPSCR_OFC 0x4U

#define AARCH32_FPSCR_UFC 0x8U

#define AARCH32_FPSCR_IXC 0x10U

#define AARCH32_FPSCR_IDC 0x80U

#define AARCH32_FPSCR_IOE 0x100U

#define AARCH32_FPSCR_DZE 0x200U

#define AARCH32_FPSCR_OFE 0x400U

#define AARCH32_FPSCR_UFE 0x800U

#define AARCH32_FPSCR_IXE 0x1000U

#define AARCH32_FPSCR_IDE 0x8000U

#define AARCH32_FPSCR_LEN( _val ) ( ( _val ) << 16 )
#define AARCH32_FPSCR_LEN_SHIFT 16
#define AARCH32_FPSCR_LEN_MASK 0x70000U
#define AARCH32_FPSCR_LEN_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0x7U )

#define AARCH32_FPSCR_FZ16 0x80000U

#define AARCH32_FPSCR_STRIDE( _val ) ( ( _val ) << 20 )
#define AARCH32_FPSCR_STRIDE_SHIFT 20
#define AARCH32_FPSCR_STRIDE_MASK 0x300000U
#define AARCH32_FPSCR_STRIDE_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0x3U )

#define AARCH32_FPSCR_RMODE( _val ) ( ( _val ) << 22 )
#define AARCH32_FPSCR_RMODE_SHIFT 22
#define AARCH32_FPSCR_RMODE_MASK 0xc00000U
#define AARCH32_FPSCR_RMODE_GET( _reg ) \
  ( ( ( _reg ) >> 22 ) & 0x3U )

#define AARCH32_FPSCR_FZ 0x1000000U

#define AARCH32_FPSCR_DN 0x2000000U

#define AARCH32_FPSCR_AHP 0x4000000U

#define AARCH32_FPSCR_QC 0x8000000U

#define AARCH32_FPSCR_V 0x10000000U

#define AARCH32_FPSCR_C 0x20000000U

#define AARCH32_FPSCR_Z 0x40000000U

#define AARCH32_FPSCR_N 0x80000000U

/* FPSID, Floating-Point System ID Register */

#define AARCH32_FPSID_REVISION( _val ) ( ( _val ) << 0 )
#define AARCH32_FPSID_REVISION_SHIFT 0
#define AARCH32_FPSID_REVISION_MASK 0xfU
#define AARCH32_FPSID_REVISION_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_FPSID_VARIANT( _val ) ( ( _val ) << 4 )
#define AARCH32_FPSID_VARIANT_SHIFT 4
#define AARCH32_FPSID_VARIANT_MASK 0xf0U
#define AARCH32_FPSID_VARIANT_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH32_FPSID_PARTNUM( _val ) ( ( _val ) << 8 )
#define AARCH32_FPSID_PARTNUM_SHIFT 8
#define AARCH32_FPSID_PARTNUM_MASK 0xff00U
#define AARCH32_FPSID_PARTNUM_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xffU )

#define AARCH32_FPSID_SUBARCHITECTURE( _val ) ( ( _val ) << 16 )
#define AARCH32_FPSID_SUBARCHITECTURE_SHIFT 16
#define AARCH32_FPSID_SUBARCHITECTURE_MASK 0x7f0000U
#define AARCH32_FPSID_SUBARCHITECTURE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0x7fU )

#define AARCH32_FPSID_SW 0x800000U

#define AARCH32_FPSID_IMPLEMENTER( _val ) ( ( _val ) << 24 )
#define AARCH32_FPSID_IMPLEMENTER_SHIFT 24
#define AARCH32_FPSID_IMPLEMENTER_MASK 0xff000000U
#define AARCH32_FPSID_IMPLEMENTER_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xffU )

/* HACR, Hyp Auxiliary Configuration Register */

static inline uint32_t _AArch32_Read_hacr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c1, c1, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hacr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c1, c1, 7" : : "r" ( value ) : "memory"
  );
}

/* HACTLR, Hyp Auxiliary Control Register */

static inline uint32_t _AArch32_Read_hactlr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c1, c0, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hactlr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c1, c0, 1" : : "r" ( value ) : "memory"
  );
}

/* HACTLR2, Hyp Auxiliary Control Register 2 */

static inline uint32_t _AArch32_Read_hactlr2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c1, c0, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hactlr2( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c1, c0, 3" : : "r" ( value ) : "memory"
  );
}

/* HADFSR, Hyp Auxiliary Data Fault Status Register */

static inline uint32_t _AArch32_Read_hadfsr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c5, c1, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hadfsr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c5, c1, 0" : : "r" ( value ) : "memory"
  );
}

/* HAIFSR, Hyp Auxiliary Instruction Fault Status Register */

static inline uint32_t _AArch32_Read_haifsr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c5, c1, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_haifsr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c5, c1, 1" : : "r" ( value ) : "memory"
  );
}

/* HAMAIR0, Hyp Auxiliary Memory Attribute Indirection Register 0 */

static inline uint32_t _AArch32_Read_hamair0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c10, c3, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hamair0( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c10, c3, 0" : : "r" ( value ) : "memory"
  );
}

/* HAMAIR1, Hyp Auxiliary Memory Attribute Indirection Register 1 */

static inline uint32_t _AArch32_Read_hamair1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c10, c3, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hamair1( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c10, c3, 1" : : "r" ( value ) : "memory"
  );
}

/* HCPTR, Hyp Architectural Feature Trap Register */

#define AARCH32_HCPTR_TCP10 0x400U

#define AARCH32_HCPTR_TCP11 0x800U

#define AARCH32_HCPTR_TASE 0x8000U

#define AARCH32_HCPTR_TTA 0x100000U

#define AARCH32_HCPTR_TAM 0x40000000U

#define AARCH32_HCPTR_TCPAC 0x80000000U

static inline uint32_t _AArch32_Read_hcptr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c1, c1, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hcptr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c1, c1, 2" : : "r" ( value ) : "memory"
  );
}

/* HCR, Hyp Configuration Register */

#define AARCH32_HCR_VM 0x1U

#define AARCH32_HCR_SWIO 0x2U

#define AARCH32_HCR_PTW 0x4U

#define AARCH32_HCR_FMO 0x8U

#define AARCH32_HCR_IMO 0x10U

#define AARCH32_HCR_AMO 0x20U

#define AARCH32_HCR_VF 0x40U

#define AARCH32_HCR_VI 0x80U

#define AARCH32_HCR_VA 0x100U

#define AARCH32_HCR_FB 0x200U

#define AARCH32_HCR_BSU( _val ) ( ( _val ) << 10 )
#define AARCH32_HCR_BSU_SHIFT 10
#define AARCH32_HCR_BSU_MASK 0xc00U
#define AARCH32_HCR_BSU_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3U )

#define AARCH32_HCR_DC 0x1000U

#define AARCH32_HCR_TWI 0x2000U

#define AARCH32_HCR_TWE 0x4000U

#define AARCH32_HCR_TID0 0x8000U

#define AARCH32_HCR_TID1 0x10000U

#define AARCH32_HCR_TID2 0x20000U

#define AARCH32_HCR_TID3 0x40000U

#define AARCH32_HCR_TSC 0x80000U

#define AARCH32_HCR_TIDCP 0x100000U

#define AARCH32_HCR_TAC 0x200000U

#define AARCH32_HCR_TSW 0x400000U

#define AARCH32_HCR_TPC 0x800000U

#define AARCH32_HCR_TPU 0x1000000U

#define AARCH32_HCR_TTLB 0x2000000U

#define AARCH32_HCR_TVM 0x4000000U

#define AARCH32_HCR_TGE 0x8000000U

#define AARCH32_HCR_HCD 0x20000000U

#define AARCH32_HCR_TRVM 0x40000000U

static inline uint32_t _AArch32_Read_hcr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c1, c1, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hcr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c1, c1, 0" : : "r" ( value ) : "memory"
  );
}

/* HCR2, Hyp Configuration Register 2 */

#define AARCH32_HCR2_CD 0x1U

#define AARCH32_HCR2_ID 0x2U

#define AARCH32_HCR2_TERR 0x10U

#define AARCH32_HCR2_TEA 0x20U

#define AARCH32_HCR2_MIOCNCE 0x40U

#define AARCH32_HCR2_TID4 0x20000U

#define AARCH32_HCR2_TICAB 0x40000U

#define AARCH32_HCR2_TOCU 0x100000U

#define AARCH32_HCR2_TTLBIS 0x400000U

static inline uint32_t _AArch32_Read_hcr2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c1, c1, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hcr2( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c1, c1, 4" : : "r" ( value ) : "memory"
  );
}

/* HDFAR, Hyp Data Fault Address Register */

static inline uint32_t _AArch32_Read_hdfar( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c0, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hdfar( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c0, 0" : : "r" ( value ) : "memory"
  );
}

/* HIFAR, Hyp Instruction Fault Address Register */

static inline uint32_t _AArch32_Read_hifar( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c0, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hifar( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c0, 2" : : "r" ( value ) : "memory"
  );
}

/* HMAIR0, Hyp Memory Attribute Indirection Register 0 */

static inline uint32_t _AArch32_Read_hmair0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c10, c2, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hmair0( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c10, c2, 0" : : "r" ( value ) : "memory"
  );
}

/* HMAIR1, Hyp Memory Attribute Indirection Register 1 */

static inline uint32_t _AArch32_Read_hmair1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c10, c2, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hmair1( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c10, c2, 1" : : "r" ( value ) : "memory"
  );
}

/* HPFAR, Hyp IPA Fault Address Register */

#define AARCH32_HPFAR_FIPA_39_12( _val ) ( ( _val ) << 4 )
#define AARCH32_HPFAR_FIPA_39_12_SHIFT 4
#define AARCH32_HPFAR_FIPA_39_12_MASK 0xfffffff0U
#define AARCH32_HPFAR_FIPA_39_12_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfffffffU )

static inline uint32_t _AArch32_Read_hpfar( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c0, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hpfar( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c0, 4" : : "r" ( value ) : "memory"
  );
}

/* HRMR, Hyp Reset Management Register */

#define AARCH32_HRMR_AA64 0x1U

#define AARCH32_HRMR_RR 0x2U

static inline uint32_t _AArch32_Read_hrmr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c12, c0, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hrmr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c12, c0, 2" : : "r" ( value ) : "memory"
  );
}

/* HSCTLR, Hyp System Control Register */

#define AARCH32_HSCTLR_M 0x1U

#define AARCH32_HSCTLR_A 0x2U

#define AARCH32_HSCTLR_C 0x4U

#define AARCH32_HSCTLR_NTLSMD 0x8U

#define AARCH32_HSCTLR_LSMAOE 0x10U

#define AARCH32_HSCTLR_CP15BEN 0x20U

#define AARCH32_HSCTLR_ITD 0x80U

#define AARCH32_HSCTLR_SED 0x100U

#define AARCH32_HSCTLR_I 0x1000U

#define AARCH32_HSCTLR_BR 0x20000U

#define AARCH32_HSCTLR_WXN 0x80000U

#define AARCH32_HSCTLR_FI 0x200000U

#define AARCH32_HSCTLR_EE 0x2000000U

#define AARCH32_HSCTLR_TE 0x40000000U

#define AARCH32_HSCTLR_DSSBS 0x80000000U

static inline uint32_t _AArch32_Read_hsctlr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c1, c0, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hsctlr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c1, c0, 0" : : "r" ( value ) : "memory"
  );
}

/* HSR, Hyp Syndrome Register */

#define AARCH32_HSR_DIRECTION 0x1U

#define AARCH32_HSR_TI 0x1U

#define AARCH32_HSR_COPROC( _val ) ( ( _val ) << 0 )
#define AARCH32_HSR_COPROC_SHIFT 0
#define AARCH32_HSR_COPROC_MASK 0xfU
#define AARCH32_HSR_COPROC_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_HSR_DFSC( _val ) ( ( _val ) << 0 )
#define AARCH32_HSR_DFSC_SHIFT 0
#define AARCH32_HSR_DFSC_MASK 0x3fU
#define AARCH32_HSR_DFSC_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )

#define AARCH32_HSR_IFSC( _val ) ( ( _val ) << 0 )
#define AARCH32_HSR_IFSC_SHIFT 0
#define AARCH32_HSR_IFSC_MASK 0x3fU
#define AARCH32_HSR_IFSC_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )

#define AARCH32_HSR_IMM16( _val ) ( ( _val ) << 0 )
#define AARCH32_HSR_IMM16_SHIFT 0
#define AARCH32_HSR_IMM16_MASK 0xffffU
#define AARCH32_HSR_IMM16_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )

#define AARCH32_HSR_ISS( _val ) ( ( _val ) << 0 )
#define AARCH32_HSR_ISS_SHIFT 0
#define AARCH32_HSR_ISS_MASK 0x1ffffffU
#define AARCH32_HSR_ISS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1ffffffU )

#define AARCH32_HSR_AM( _val ) ( ( _val ) << 1 )
#define AARCH32_HSR_AM_SHIFT 1
#define AARCH32_HSR_AM_MASK 0xeU
#define AARCH32_HSR_AM_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x7U )

#define AARCH32_HSR_CRM( _val ) ( ( _val ) << 1 )
#define AARCH32_HSR_CRM_SHIFT 1
#define AARCH32_HSR_CRM_MASK 0x1eU
#define AARCH32_HSR_CRM_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0xfU )

#define AARCH32_HSR_OFFSET 0x10U

#define AARCH32_HSR_TA 0x20U

#define AARCH32_HSR_RN( _val ) ( ( _val ) << 5 )
#define AARCH32_HSR_RN_SHIFT 5
#define AARCH32_HSR_RN_MASK 0x1e0U
#define AARCH32_HSR_RN_GET( _reg ) \
  ( ( ( _reg ) >> 5 ) & 0xfU )

#define AARCH32_HSR_RT( _val ) ( ( _val ) << 5 )
#define AARCH32_HSR_RT_SHIFT 5
#define AARCH32_HSR_RT_MASK 0x1e0U
#define AARCH32_HSR_RT_GET( _reg ) \
  ( ( ( _reg ) >> 5 ) & 0xfU )

#define AARCH32_HSR_WNR 0x40U

#define AARCH32_HSR_S1PTW 0x80U

#define AARCH32_HSR_CM 0x100U

#define AARCH32_HSR_EA 0x200U

#define AARCH32_HSR_FNV 0x400U

#define AARCH32_HSR_AET( _val ) ( ( _val ) << 10 )
#define AARCH32_HSR_AET_SHIFT 10
#define AARCH32_HSR_AET_MASK 0xc00U
#define AARCH32_HSR_AET_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3U )

#define AARCH32_HSR_CRN( _val ) ( ( _val ) << 10 )
#define AARCH32_HSR_CRN_SHIFT 10
#define AARCH32_HSR_CRN_MASK 0x3c00U
#define AARCH32_HSR_CRN_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0xfU )

#define AARCH32_HSR_RT2( _val ) ( ( _val ) << 10 )
#define AARCH32_HSR_RT2_SHIFT 10
#define AARCH32_HSR_RT2_MASK 0x3c00U
#define AARCH32_HSR_RT2_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0xfU )

#define AARCH32_HSR_IMM8( _val ) ( ( _val ) << 12 )
#define AARCH32_HSR_IMM8_SHIFT 12
#define AARCH32_HSR_IMM8_MASK 0xff000U
#define AARCH32_HSR_IMM8_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xffU )

#define AARCH32_HSR_AR 0x4000U

#define AARCH32_HSR_OPC1_0( _val ) ( ( _val ) << 14 )
#define AARCH32_HSR_OPC1_SHIFT_0 14
#define AARCH32_HSR_OPC1_MASK_0 0x1c000U
#define AARCH32_HSR_OPC1_GET_0( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x7U )

#define AARCH32_HSR_OPC1_1( _val ) ( ( _val ) << 16 )
#define AARCH32_HSR_OPC1_SHIFT_1 16
#define AARCH32_HSR_OPC1_MASK_1 0xf0000U
#define AARCH32_HSR_OPC1_GET_1( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_HSR_SRT( _val ) ( ( _val ) << 16 )
#define AARCH32_HSR_SRT_SHIFT 16
#define AARCH32_HSR_SRT_MASK 0xf0000U
#define AARCH32_HSR_SRT_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_HSR_OPC2( _val ) ( ( _val ) << 17 )
#define AARCH32_HSR_OPC2_SHIFT 17
#define AARCH32_HSR_OPC2_MASK 0xe0000U
#define AARCH32_HSR_OPC2_GET( _reg ) \
  ( ( ( _reg ) >> 17 ) & 0x7U )

#define AARCH32_HSR_CCKNOWNPASS 0x80000U

#define AARCH32_HSR_COND( _val ) ( ( _val ) << 20 )
#define AARCH32_HSR_COND_SHIFT 20
#define AARCH32_HSR_COND_MASK 0xf00000U
#define AARCH32_HSR_COND_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH32_HSR_SSE 0x200000U

#define AARCH32_HSR_SAS( _val ) ( ( _val ) << 22 )
#define AARCH32_HSR_SAS_SHIFT 22
#define AARCH32_HSR_SAS_MASK 0xc00000U
#define AARCH32_HSR_SAS_GET( _reg ) \
  ( ( ( _reg ) >> 22 ) & 0x3U )

#define AARCH32_HSR_CV 0x1000000U

#define AARCH32_HSR_ISV 0x1000000U

#define AARCH32_HSR_IL 0x2000000U

#define AARCH32_HSR_EC( _val ) ( ( _val ) << 26 )
#define AARCH32_HSR_EC_SHIFT 26
#define AARCH32_HSR_EC_MASK 0xfc000000U
#define AARCH32_HSR_EC_GET( _reg ) \
  ( ( ( _reg ) >> 26 ) & 0x3fU )

static inline uint32_t _AArch32_Read_hsr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c5, c2, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hsr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c5, c2, 0" : : "r" ( value ) : "memory"
  );
}

/* HSTR, Hyp System Trap Register */

static inline uint32_t _AArch32_Read_hstr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c1, c1, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hstr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c1, c1, 3" : : "r" ( value ) : "memory"
  );
}

/* HTCR, Hyp Translation Control Register */

#define AARCH32_HTCR_T0SZ( _val ) ( ( _val ) << 0 )
#define AARCH32_HTCR_T0SZ_SHIFT 0
#define AARCH32_HTCR_T0SZ_MASK 0x7U
#define AARCH32_HTCR_T0SZ_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x7U )

#define AARCH32_HTCR_IRGN0( _val ) ( ( _val ) << 8 )
#define AARCH32_HTCR_IRGN0_SHIFT 8
#define AARCH32_HTCR_IRGN0_MASK 0x300U
#define AARCH32_HTCR_IRGN0_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0x3U )

#define AARCH32_HTCR_ORGN0( _val ) ( ( _val ) << 10 )
#define AARCH32_HTCR_ORGN0_SHIFT 10
#define AARCH32_HTCR_ORGN0_MASK 0xc00U
#define AARCH32_HTCR_ORGN0_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3U )

#define AARCH32_HTCR_SH0( _val ) ( ( _val ) << 12 )
#define AARCH32_HTCR_SH0_SHIFT 12
#define AARCH32_HTCR_SH0_MASK 0x3000U
#define AARCH32_HTCR_SH0_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0x3U )

#define AARCH32_HTCR_HPD 0x1000000U

#define AARCH32_HTCR_HWU59 0x2000000U

#define AARCH32_HTCR_HWU60 0x4000000U

#define AARCH32_HTCR_HWU61 0x8000000U

#define AARCH32_HTCR_HWU62 0x10000000U

static inline uint32_t _AArch32_Read_htcr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c2, c0, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_htcr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c2, c0, 2" : : "r" ( value ) : "memory"
  );
}

/* HTPIDR, Hyp Software Thread ID Register */

static inline uint32_t _AArch32_Read_htpidr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c13, c0, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_htpidr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c13, c0, 2" : : "r" ( value ) : "memory"
  );
}

/* HTTBR, Hyp Translation Table Base Register */

#define AARCH32_HTTBR_CNP 0x1U

#define AARCH32_HTTBR_BADDR( _val ) ( ( _val ) << 1 )
#define AARCH32_HTTBR_BADDR_SHIFT 1
#define AARCH32_HTTBR_BADDR_MASK 0xfffffffffffeULL
#define AARCH32_HTTBR_BADDR_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x7fffffffffffULL )

static inline uint64_t _AArch32_Read_httbr( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 4, %Q0, %R0, c2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_httbr( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 4, %Q0, %R0, c2" : : "r" ( value ) : "memory"
  );
}

/* HVBAR, Hyp Vector Base Address Register */

static inline uint32_t _AArch32_Read_hvbar( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c12, c0, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hvbar( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c12, c0, 0" : : "r" ( value ) : "memory"
  );
}

/* ICIALLU, Instruction Cache Invalidate All to PoU */

static inline void _AArch32_Write_iciallu( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c5, 0" : : "r" ( value ) : "memory"
  );
}

/* ICIALLUIS, Instruction Cache Invalidate All to PoU, Inner Shareable */

static inline void _AArch32_Write_icialluis( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c1, 0" : : "r" ( value ) : "memory"
  );
}

/* ICIMVAU, Instruction Cache line Invalidate by VA to PoU */

static inline void _AArch32_Write_icimvau( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c5, 1" : : "r" ( value ) : "memory"
  );
}

/* ID_AFR0, Auxiliary Feature Register 0 */

static inline uint32_t _AArch32_Read_id_afr0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c1, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_DFR0, Debug Feature Register 0 */

#define AARCH32_ID_DFR0_COPDBG( _val ) ( ( _val ) << 0 )
#define AARCH32_ID_DFR0_COPDBG_SHIFT 0
#define AARCH32_ID_DFR0_COPDBG_MASK 0xfU
#define AARCH32_ID_DFR0_COPDBG_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_ID_DFR0_COPSDBG( _val ) ( ( _val ) << 4 )
#define AARCH32_ID_DFR0_COPSDBG_SHIFT 4
#define AARCH32_ID_DFR0_COPSDBG_MASK 0xf0U
#define AARCH32_ID_DFR0_COPSDBG_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH32_ID_DFR0_MMAPDBG( _val ) ( ( _val ) << 8 )
#define AARCH32_ID_DFR0_MMAPDBG_SHIFT 8
#define AARCH32_ID_DFR0_MMAPDBG_MASK 0xf00U
#define AARCH32_ID_DFR0_MMAPDBG_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH32_ID_DFR0_COPTRC( _val ) ( ( _val ) << 12 )
#define AARCH32_ID_DFR0_COPTRC_SHIFT 12
#define AARCH32_ID_DFR0_COPTRC_MASK 0xf000U
#define AARCH32_ID_DFR0_COPTRC_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH32_ID_DFR0_MMAPTRC( _val ) ( ( _val ) << 16 )
#define AARCH32_ID_DFR0_MMAPTRC_SHIFT 16
#define AARCH32_ID_DFR0_MMAPTRC_MASK 0xf0000U
#define AARCH32_ID_DFR0_MMAPTRC_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_ID_DFR0_MPROFDBG( _val ) ( ( _val ) << 20 )
#define AARCH32_ID_DFR0_MPROFDBG_SHIFT 20
#define AARCH32_ID_DFR0_MPROFDBG_MASK 0xf00000U
#define AARCH32_ID_DFR0_MPROFDBG_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH32_ID_DFR0_PERFMON( _val ) ( ( _val ) << 24 )
#define AARCH32_ID_DFR0_PERFMON_SHIFT 24
#define AARCH32_ID_DFR0_PERFMON_MASK 0xf000000U
#define AARCH32_ID_DFR0_PERFMON_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH32_ID_DFR0_TRACEFILT( _val ) ( ( _val ) << 28 )
#define AARCH32_ID_DFR0_TRACEFILT_SHIFT 28
#define AARCH32_ID_DFR0_TRACEFILT_MASK 0xf0000000U
#define AARCH32_ID_DFR0_TRACEFILT_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint32_t _AArch32_Read_id_dfr0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c1, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_DFR1, Debug Feature Register 1 */

#define AARCH32_ID_DFR1_MTPMU( _val ) ( ( _val ) << 0 )
#define AARCH32_ID_DFR1_MTPMU_SHIFT 0
#define AARCH32_ID_DFR1_MTPMU_MASK 0xfU
#define AARCH32_ID_DFR1_MTPMU_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

static inline uint32_t _AArch32_Read_id_dfr1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c3, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_ISAR0, Instruction Set Attribute Register 0 */

#define AARCH32_ID_ISAR0_SWAP( _val ) ( ( _val ) << 0 )
#define AARCH32_ID_ISAR0_SWAP_SHIFT 0
#define AARCH32_ID_ISAR0_SWAP_MASK 0xfU
#define AARCH32_ID_ISAR0_SWAP_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_ID_ISAR0_BITCOUNT( _val ) ( ( _val ) << 4 )
#define AARCH32_ID_ISAR0_BITCOUNT_SHIFT 4
#define AARCH32_ID_ISAR0_BITCOUNT_MASK 0xf0U
#define AARCH32_ID_ISAR0_BITCOUNT_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH32_ID_ISAR0_BITFIELD( _val ) ( ( _val ) << 8 )
#define AARCH32_ID_ISAR0_BITFIELD_SHIFT 8
#define AARCH32_ID_ISAR0_BITFIELD_MASK 0xf00U
#define AARCH32_ID_ISAR0_BITFIELD_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH32_ID_ISAR0_CMPBRANCH( _val ) ( ( _val ) << 12 )
#define AARCH32_ID_ISAR0_CMPBRANCH_SHIFT 12
#define AARCH32_ID_ISAR0_CMPBRANCH_MASK 0xf000U
#define AARCH32_ID_ISAR0_CMPBRANCH_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH32_ID_ISAR0_COPROC( _val ) ( ( _val ) << 16 )
#define AARCH32_ID_ISAR0_COPROC_SHIFT 16
#define AARCH32_ID_ISAR0_COPROC_MASK 0xf0000U
#define AARCH32_ID_ISAR0_COPROC_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_ID_ISAR0_DEBUG( _val ) ( ( _val ) << 20 )
#define AARCH32_ID_ISAR0_DEBUG_SHIFT 20
#define AARCH32_ID_ISAR0_DEBUG_MASK 0xf00000U
#define AARCH32_ID_ISAR0_DEBUG_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH32_ID_ISAR0_DIVIDE( _val ) ( ( _val ) << 24 )
#define AARCH32_ID_ISAR0_DIVIDE_SHIFT 24
#define AARCH32_ID_ISAR0_DIVIDE_MASK 0xf000000U
#define AARCH32_ID_ISAR0_DIVIDE_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

static inline uint32_t _AArch32_Read_id_isar0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c2, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_ISAR1, Instruction Set Attribute Register 1 */

#define AARCH32_ID_ISAR1_ENDIAN( _val ) ( ( _val ) << 0 )
#define AARCH32_ID_ISAR1_ENDIAN_SHIFT 0
#define AARCH32_ID_ISAR1_ENDIAN_MASK 0xfU
#define AARCH32_ID_ISAR1_ENDIAN_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_ID_ISAR1_EXCEPT( _val ) ( ( _val ) << 4 )
#define AARCH32_ID_ISAR1_EXCEPT_SHIFT 4
#define AARCH32_ID_ISAR1_EXCEPT_MASK 0xf0U
#define AARCH32_ID_ISAR1_EXCEPT_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH32_ID_ISAR1_EXCEPT_AR( _val ) ( ( _val ) << 8 )
#define AARCH32_ID_ISAR1_EXCEPT_AR_SHIFT 8
#define AARCH32_ID_ISAR1_EXCEPT_AR_MASK 0xf00U
#define AARCH32_ID_ISAR1_EXCEPT_AR_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH32_ID_ISAR1_EXTEND( _val ) ( ( _val ) << 12 )
#define AARCH32_ID_ISAR1_EXTEND_SHIFT 12
#define AARCH32_ID_ISAR1_EXTEND_MASK 0xf000U
#define AARCH32_ID_ISAR1_EXTEND_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH32_ID_ISAR1_IFTHEN( _val ) ( ( _val ) << 16 )
#define AARCH32_ID_ISAR1_IFTHEN_SHIFT 16
#define AARCH32_ID_ISAR1_IFTHEN_MASK 0xf0000U
#define AARCH32_ID_ISAR1_IFTHEN_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_ID_ISAR1_IMMEDIATE( _val ) ( ( _val ) << 20 )
#define AARCH32_ID_ISAR1_IMMEDIATE_SHIFT 20
#define AARCH32_ID_ISAR1_IMMEDIATE_MASK 0xf00000U
#define AARCH32_ID_ISAR1_IMMEDIATE_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH32_ID_ISAR1_INTERWORK( _val ) ( ( _val ) << 24 )
#define AARCH32_ID_ISAR1_INTERWORK_SHIFT 24
#define AARCH32_ID_ISAR1_INTERWORK_MASK 0xf000000U
#define AARCH32_ID_ISAR1_INTERWORK_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH32_ID_ISAR1_JAZELLE( _val ) ( ( _val ) << 28 )
#define AARCH32_ID_ISAR1_JAZELLE_SHIFT 28
#define AARCH32_ID_ISAR1_JAZELLE_MASK 0xf0000000U
#define AARCH32_ID_ISAR1_JAZELLE_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint32_t _AArch32_Read_id_isar1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c2, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_ISAR2, Instruction Set Attribute Register 2 */

#define AARCH32_ID_ISAR2_LOADSTORE( _val ) ( ( _val ) << 0 )
#define AARCH32_ID_ISAR2_LOADSTORE_SHIFT 0
#define AARCH32_ID_ISAR2_LOADSTORE_MASK 0xfU
#define AARCH32_ID_ISAR2_LOADSTORE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_ID_ISAR2_MEMHINT( _val ) ( ( _val ) << 4 )
#define AARCH32_ID_ISAR2_MEMHINT_SHIFT 4
#define AARCH32_ID_ISAR2_MEMHINT_MASK 0xf0U
#define AARCH32_ID_ISAR2_MEMHINT_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH32_ID_ISAR2_MULTIACCESSINT( _val ) ( ( _val ) << 8 )
#define AARCH32_ID_ISAR2_MULTIACCESSINT_SHIFT 8
#define AARCH32_ID_ISAR2_MULTIACCESSINT_MASK 0xf00U
#define AARCH32_ID_ISAR2_MULTIACCESSINT_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH32_ID_ISAR2_MULT( _val ) ( ( _val ) << 12 )
#define AARCH32_ID_ISAR2_MULT_SHIFT 12
#define AARCH32_ID_ISAR2_MULT_MASK 0xf000U
#define AARCH32_ID_ISAR2_MULT_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH32_ID_ISAR2_MULTS( _val ) ( ( _val ) << 16 )
#define AARCH32_ID_ISAR2_MULTS_SHIFT 16
#define AARCH32_ID_ISAR2_MULTS_MASK 0xf0000U
#define AARCH32_ID_ISAR2_MULTS_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_ID_ISAR2_MULTU( _val ) ( ( _val ) << 20 )
#define AARCH32_ID_ISAR2_MULTU_SHIFT 20
#define AARCH32_ID_ISAR2_MULTU_MASK 0xf00000U
#define AARCH32_ID_ISAR2_MULTU_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH32_ID_ISAR2_PSR_AR( _val ) ( ( _val ) << 24 )
#define AARCH32_ID_ISAR2_PSR_AR_SHIFT 24
#define AARCH32_ID_ISAR2_PSR_AR_MASK 0xf000000U
#define AARCH32_ID_ISAR2_PSR_AR_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH32_ID_ISAR2_REVERSAL( _val ) ( ( _val ) << 28 )
#define AARCH32_ID_ISAR2_REVERSAL_SHIFT 28
#define AARCH32_ID_ISAR2_REVERSAL_MASK 0xf0000000U
#define AARCH32_ID_ISAR2_REVERSAL_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint32_t _AArch32_Read_id_isar2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c2, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_ISAR3, Instruction Set Attribute Register 3 */

#define AARCH32_ID_ISAR3_SATURATE( _val ) ( ( _val ) << 0 )
#define AARCH32_ID_ISAR3_SATURATE_SHIFT 0
#define AARCH32_ID_ISAR3_SATURATE_MASK 0xfU
#define AARCH32_ID_ISAR3_SATURATE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_ID_ISAR3_SIMD( _val ) ( ( _val ) << 4 )
#define AARCH32_ID_ISAR3_SIMD_SHIFT 4
#define AARCH32_ID_ISAR3_SIMD_MASK 0xf0U
#define AARCH32_ID_ISAR3_SIMD_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH32_ID_ISAR3_SVC( _val ) ( ( _val ) << 8 )
#define AARCH32_ID_ISAR3_SVC_SHIFT 8
#define AARCH32_ID_ISAR3_SVC_MASK 0xf00U
#define AARCH32_ID_ISAR3_SVC_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH32_ID_ISAR3_SYNCHPRIM( _val ) ( ( _val ) << 12 )
#define AARCH32_ID_ISAR3_SYNCHPRIM_SHIFT 12
#define AARCH32_ID_ISAR3_SYNCHPRIM_MASK 0xf000U
#define AARCH32_ID_ISAR3_SYNCHPRIM_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH32_ID_ISAR3_TABBRANCH( _val ) ( ( _val ) << 16 )
#define AARCH32_ID_ISAR3_TABBRANCH_SHIFT 16
#define AARCH32_ID_ISAR3_TABBRANCH_MASK 0xf0000U
#define AARCH32_ID_ISAR3_TABBRANCH_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_ID_ISAR3_T32COPY( _val ) ( ( _val ) << 20 )
#define AARCH32_ID_ISAR3_T32COPY_SHIFT 20
#define AARCH32_ID_ISAR3_T32COPY_MASK 0xf00000U
#define AARCH32_ID_ISAR3_T32COPY_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH32_ID_ISAR3_TRUENOP( _val ) ( ( _val ) << 24 )
#define AARCH32_ID_ISAR3_TRUENOP_SHIFT 24
#define AARCH32_ID_ISAR3_TRUENOP_MASK 0xf000000U
#define AARCH32_ID_ISAR3_TRUENOP_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH32_ID_ISAR3_T32EE( _val ) ( ( _val ) << 28 )
#define AARCH32_ID_ISAR3_T32EE_SHIFT 28
#define AARCH32_ID_ISAR3_T32EE_MASK 0xf0000000U
#define AARCH32_ID_ISAR3_T32EE_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint32_t _AArch32_Read_id_isar3( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c2, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_ISAR4, Instruction Set Attribute Register 4 */

#define AARCH32_ID_ISAR4_UNPRIV( _val ) ( ( _val ) << 0 )
#define AARCH32_ID_ISAR4_UNPRIV_SHIFT 0
#define AARCH32_ID_ISAR4_UNPRIV_MASK 0xfU
#define AARCH32_ID_ISAR4_UNPRIV_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_ID_ISAR4_WITHSHIFTS( _val ) ( ( _val ) << 4 )
#define AARCH32_ID_ISAR4_WITHSHIFTS_SHIFT 4
#define AARCH32_ID_ISAR4_WITHSHIFTS_MASK 0xf0U
#define AARCH32_ID_ISAR4_WITHSHIFTS_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH32_ID_ISAR4_WRITEBACK( _val ) ( ( _val ) << 8 )
#define AARCH32_ID_ISAR4_WRITEBACK_SHIFT 8
#define AARCH32_ID_ISAR4_WRITEBACK_MASK 0xf00U
#define AARCH32_ID_ISAR4_WRITEBACK_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH32_ID_ISAR4_SMC( _val ) ( ( _val ) << 12 )
#define AARCH32_ID_ISAR4_SMC_SHIFT 12
#define AARCH32_ID_ISAR4_SMC_MASK 0xf000U
#define AARCH32_ID_ISAR4_SMC_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH32_ID_ISAR4_BARRIER( _val ) ( ( _val ) << 16 )
#define AARCH32_ID_ISAR4_BARRIER_SHIFT 16
#define AARCH32_ID_ISAR4_BARRIER_MASK 0xf0000U
#define AARCH32_ID_ISAR4_BARRIER_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_ID_ISAR4_SYNCHPRIM_FRAC( _val ) ( ( _val ) << 20 )
#define AARCH32_ID_ISAR4_SYNCHPRIM_FRAC_SHIFT 20
#define AARCH32_ID_ISAR4_SYNCHPRIM_FRAC_MASK 0xf00000U
#define AARCH32_ID_ISAR4_SYNCHPRIM_FRAC_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH32_ID_ISAR4_PSR_M( _val ) ( ( _val ) << 24 )
#define AARCH32_ID_ISAR4_PSR_M_SHIFT 24
#define AARCH32_ID_ISAR4_PSR_M_MASK 0xf000000U
#define AARCH32_ID_ISAR4_PSR_M_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH32_ID_ISAR4_SWP_FRAC( _val ) ( ( _val ) << 28 )
#define AARCH32_ID_ISAR4_SWP_FRAC_SHIFT 28
#define AARCH32_ID_ISAR4_SWP_FRAC_MASK 0xf0000000U
#define AARCH32_ID_ISAR4_SWP_FRAC_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint32_t _AArch32_Read_id_isar4( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c2, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_ISAR5, Instruction Set Attribute Register 5 */

#define AARCH32_ID_ISAR5_SEVL( _val ) ( ( _val ) << 0 )
#define AARCH32_ID_ISAR5_SEVL_SHIFT 0
#define AARCH32_ID_ISAR5_SEVL_MASK 0xfU
#define AARCH32_ID_ISAR5_SEVL_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_ID_ISAR5_AES( _val ) ( ( _val ) << 4 )
#define AARCH32_ID_ISAR5_AES_SHIFT 4
#define AARCH32_ID_ISAR5_AES_MASK 0xf0U
#define AARCH32_ID_ISAR5_AES_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH32_ID_ISAR5_SHA1( _val ) ( ( _val ) << 8 )
#define AARCH32_ID_ISAR5_SHA1_SHIFT 8
#define AARCH32_ID_ISAR5_SHA1_MASK 0xf00U
#define AARCH32_ID_ISAR5_SHA1_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH32_ID_ISAR5_SHA2( _val ) ( ( _val ) << 12 )
#define AARCH32_ID_ISAR5_SHA2_SHIFT 12
#define AARCH32_ID_ISAR5_SHA2_MASK 0xf000U
#define AARCH32_ID_ISAR5_SHA2_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH32_ID_ISAR5_CRC32( _val ) ( ( _val ) << 16 )
#define AARCH32_ID_ISAR5_CRC32_SHIFT 16
#define AARCH32_ID_ISAR5_CRC32_MASK 0xf0000U
#define AARCH32_ID_ISAR5_CRC32_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_ID_ISAR5_RDM( _val ) ( ( _val ) << 24 )
#define AARCH32_ID_ISAR5_RDM_SHIFT 24
#define AARCH32_ID_ISAR5_RDM_MASK 0xf000000U
#define AARCH32_ID_ISAR5_RDM_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH32_ID_ISAR5_VCMA( _val ) ( ( _val ) << 28 )
#define AARCH32_ID_ISAR5_VCMA_SHIFT 28
#define AARCH32_ID_ISAR5_VCMA_MASK 0xf0000000U
#define AARCH32_ID_ISAR5_VCMA_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint32_t _AArch32_Read_id_isar5( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c2, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_ISAR6, Instruction Set Attribute Register 6 */

#define AARCH32_ID_ISAR6_JSCVT( _val ) ( ( _val ) << 0 )
#define AARCH32_ID_ISAR6_JSCVT_SHIFT 0
#define AARCH32_ID_ISAR6_JSCVT_MASK 0xfU
#define AARCH32_ID_ISAR6_JSCVT_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_ID_ISAR6_DP( _val ) ( ( _val ) << 4 )
#define AARCH32_ID_ISAR6_DP_SHIFT 4
#define AARCH32_ID_ISAR6_DP_MASK 0xf0U
#define AARCH32_ID_ISAR6_DP_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH32_ID_ISAR6_FHM( _val ) ( ( _val ) << 8 )
#define AARCH32_ID_ISAR6_FHM_SHIFT 8
#define AARCH32_ID_ISAR6_FHM_MASK 0xf00U
#define AARCH32_ID_ISAR6_FHM_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH32_ID_ISAR6_SB( _val ) ( ( _val ) << 12 )
#define AARCH32_ID_ISAR6_SB_SHIFT 12
#define AARCH32_ID_ISAR6_SB_MASK 0xf000U
#define AARCH32_ID_ISAR6_SB_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH32_ID_ISAR6_SPECRES( _val ) ( ( _val ) << 16 )
#define AARCH32_ID_ISAR6_SPECRES_SHIFT 16
#define AARCH32_ID_ISAR6_SPECRES_MASK 0xf0000U
#define AARCH32_ID_ISAR6_SPECRES_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_ID_ISAR6_BF16( _val ) ( ( _val ) << 20 )
#define AARCH32_ID_ISAR6_BF16_SHIFT 20
#define AARCH32_ID_ISAR6_BF16_MASK 0xf00000U
#define AARCH32_ID_ISAR6_BF16_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH32_ID_ISAR6_I8MM( _val ) ( ( _val ) << 24 )
#define AARCH32_ID_ISAR6_I8MM_SHIFT 24
#define AARCH32_ID_ISAR6_I8MM_MASK 0xf000000U
#define AARCH32_ID_ISAR6_I8MM_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

static inline uint32_t _AArch32_Read_id_isar6( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c2, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_MMFR0, Memory Model Feature Register 0 */

#define AARCH32_ID_MMFR0_VMSA( _val ) ( ( _val ) << 0 )
#define AARCH32_ID_MMFR0_VMSA_SHIFT 0
#define AARCH32_ID_MMFR0_VMSA_MASK 0xfU
#define AARCH32_ID_MMFR0_VMSA_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_ID_MMFR0_PMSA( _val ) ( ( _val ) << 4 )
#define AARCH32_ID_MMFR0_PMSA_SHIFT 4
#define AARCH32_ID_MMFR0_PMSA_MASK 0xf0U
#define AARCH32_ID_MMFR0_PMSA_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH32_ID_MMFR0_OUTERSHR( _val ) ( ( _val ) << 8 )
#define AARCH32_ID_MMFR0_OUTERSHR_SHIFT 8
#define AARCH32_ID_MMFR0_OUTERSHR_MASK 0xf00U
#define AARCH32_ID_MMFR0_OUTERSHR_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH32_ID_MMFR0_SHARELVL( _val ) ( ( _val ) << 12 )
#define AARCH32_ID_MMFR0_SHARELVL_SHIFT 12
#define AARCH32_ID_MMFR0_SHARELVL_MASK 0xf000U
#define AARCH32_ID_MMFR0_SHARELVL_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH32_ID_MMFR0_TCM( _val ) ( ( _val ) << 16 )
#define AARCH32_ID_MMFR0_TCM_SHIFT 16
#define AARCH32_ID_MMFR0_TCM_MASK 0xf0000U
#define AARCH32_ID_MMFR0_TCM_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_ID_MMFR0_AUXREG( _val ) ( ( _val ) << 20 )
#define AARCH32_ID_MMFR0_AUXREG_SHIFT 20
#define AARCH32_ID_MMFR0_AUXREG_MASK 0xf00000U
#define AARCH32_ID_MMFR0_AUXREG_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH32_ID_MMFR0_FCSE( _val ) ( ( _val ) << 24 )
#define AARCH32_ID_MMFR0_FCSE_SHIFT 24
#define AARCH32_ID_MMFR0_FCSE_MASK 0xf000000U
#define AARCH32_ID_MMFR0_FCSE_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH32_ID_MMFR0_INNERSHR( _val ) ( ( _val ) << 28 )
#define AARCH32_ID_MMFR0_INNERSHR_SHIFT 28
#define AARCH32_ID_MMFR0_INNERSHR_MASK 0xf0000000U
#define AARCH32_ID_MMFR0_INNERSHR_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint32_t _AArch32_Read_id_mmfr0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c1, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_MMFR1, Memory Model Feature Register 1 */

#define AARCH32_ID_MMFR1_L1HVDVA( _val ) ( ( _val ) << 0 )
#define AARCH32_ID_MMFR1_L1HVDVA_SHIFT 0
#define AARCH32_ID_MMFR1_L1HVDVA_MASK 0xfU
#define AARCH32_ID_MMFR1_L1HVDVA_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_ID_MMFR1_L1UNIVA( _val ) ( ( _val ) << 4 )
#define AARCH32_ID_MMFR1_L1UNIVA_SHIFT 4
#define AARCH32_ID_MMFR1_L1UNIVA_MASK 0xf0U
#define AARCH32_ID_MMFR1_L1UNIVA_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH32_ID_MMFR1_L1HVDSW( _val ) ( ( _val ) << 8 )
#define AARCH32_ID_MMFR1_L1HVDSW_SHIFT 8
#define AARCH32_ID_MMFR1_L1HVDSW_MASK 0xf00U
#define AARCH32_ID_MMFR1_L1HVDSW_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH32_ID_MMFR1_L1UNISW( _val ) ( ( _val ) << 12 )
#define AARCH32_ID_MMFR1_L1UNISW_SHIFT 12
#define AARCH32_ID_MMFR1_L1UNISW_MASK 0xf000U
#define AARCH32_ID_MMFR1_L1UNISW_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH32_ID_MMFR1_L1HVD( _val ) ( ( _val ) << 16 )
#define AARCH32_ID_MMFR1_L1HVD_SHIFT 16
#define AARCH32_ID_MMFR1_L1HVD_MASK 0xf0000U
#define AARCH32_ID_MMFR1_L1HVD_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_ID_MMFR1_L1UNI( _val ) ( ( _val ) << 20 )
#define AARCH32_ID_MMFR1_L1UNI_SHIFT 20
#define AARCH32_ID_MMFR1_L1UNI_MASK 0xf00000U
#define AARCH32_ID_MMFR1_L1UNI_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH32_ID_MMFR1_L1TSTCLN( _val ) ( ( _val ) << 24 )
#define AARCH32_ID_MMFR1_L1TSTCLN_SHIFT 24
#define AARCH32_ID_MMFR1_L1TSTCLN_MASK 0xf000000U
#define AARCH32_ID_MMFR1_L1TSTCLN_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH32_ID_MMFR1_BPRED( _val ) ( ( _val ) << 28 )
#define AARCH32_ID_MMFR1_BPRED_SHIFT 28
#define AARCH32_ID_MMFR1_BPRED_MASK 0xf0000000U
#define AARCH32_ID_MMFR1_BPRED_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint32_t _AArch32_Read_id_mmfr1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c1, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_MMFR2, Memory Model Feature Register 2 */

#define AARCH32_ID_MMFR2_L1HVDFG( _val ) ( ( _val ) << 0 )
#define AARCH32_ID_MMFR2_L1HVDFG_SHIFT 0
#define AARCH32_ID_MMFR2_L1HVDFG_MASK 0xfU
#define AARCH32_ID_MMFR2_L1HVDFG_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_ID_MMFR2_L1HVDBG( _val ) ( ( _val ) << 4 )
#define AARCH32_ID_MMFR2_L1HVDBG_SHIFT 4
#define AARCH32_ID_MMFR2_L1HVDBG_MASK 0xf0U
#define AARCH32_ID_MMFR2_L1HVDBG_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH32_ID_MMFR2_L1HVDRNG( _val ) ( ( _val ) << 8 )
#define AARCH32_ID_MMFR2_L1HVDRNG_SHIFT 8
#define AARCH32_ID_MMFR2_L1HVDRNG_MASK 0xf00U
#define AARCH32_ID_MMFR2_L1HVDRNG_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH32_ID_MMFR2_HVDTLB( _val ) ( ( _val ) << 12 )
#define AARCH32_ID_MMFR2_HVDTLB_SHIFT 12
#define AARCH32_ID_MMFR2_HVDTLB_MASK 0xf000U
#define AARCH32_ID_MMFR2_HVDTLB_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH32_ID_MMFR2_UNITLB( _val ) ( ( _val ) << 16 )
#define AARCH32_ID_MMFR2_UNITLB_SHIFT 16
#define AARCH32_ID_MMFR2_UNITLB_MASK 0xf0000U
#define AARCH32_ID_MMFR2_UNITLB_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_ID_MMFR2_MEMBARR( _val ) ( ( _val ) << 20 )
#define AARCH32_ID_MMFR2_MEMBARR_SHIFT 20
#define AARCH32_ID_MMFR2_MEMBARR_MASK 0xf00000U
#define AARCH32_ID_MMFR2_MEMBARR_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH32_ID_MMFR2_WFISTALL( _val ) ( ( _val ) << 24 )
#define AARCH32_ID_MMFR2_WFISTALL_SHIFT 24
#define AARCH32_ID_MMFR2_WFISTALL_MASK 0xf000000U
#define AARCH32_ID_MMFR2_WFISTALL_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH32_ID_MMFR2_HWACCFLG( _val ) ( ( _val ) << 28 )
#define AARCH32_ID_MMFR2_HWACCFLG_SHIFT 28
#define AARCH32_ID_MMFR2_HWACCFLG_MASK 0xf0000000U
#define AARCH32_ID_MMFR2_HWACCFLG_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint32_t _AArch32_Read_id_mmfr2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c1, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_MMFR3, Memory Model Feature Register 3 */

#define AARCH32_ID_MMFR3_CMAINTVA( _val ) ( ( _val ) << 0 )
#define AARCH32_ID_MMFR3_CMAINTVA_SHIFT 0
#define AARCH32_ID_MMFR3_CMAINTVA_MASK 0xfU
#define AARCH32_ID_MMFR3_CMAINTVA_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_ID_MMFR3_CMAINTSW( _val ) ( ( _val ) << 4 )
#define AARCH32_ID_MMFR3_CMAINTSW_SHIFT 4
#define AARCH32_ID_MMFR3_CMAINTSW_MASK 0xf0U
#define AARCH32_ID_MMFR3_CMAINTSW_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH32_ID_MMFR3_BPMAINT( _val ) ( ( _val ) << 8 )
#define AARCH32_ID_MMFR3_BPMAINT_SHIFT 8
#define AARCH32_ID_MMFR3_BPMAINT_MASK 0xf00U
#define AARCH32_ID_MMFR3_BPMAINT_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH32_ID_MMFR3_MAINTBCST( _val ) ( ( _val ) << 12 )
#define AARCH32_ID_MMFR3_MAINTBCST_SHIFT 12
#define AARCH32_ID_MMFR3_MAINTBCST_MASK 0xf000U
#define AARCH32_ID_MMFR3_MAINTBCST_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH32_ID_MMFR3_PAN( _val ) ( ( _val ) << 16 )
#define AARCH32_ID_MMFR3_PAN_SHIFT 16
#define AARCH32_ID_MMFR3_PAN_MASK 0xf0000U
#define AARCH32_ID_MMFR3_PAN_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_ID_MMFR3_COHWALK( _val ) ( ( _val ) << 20 )
#define AARCH32_ID_MMFR3_COHWALK_SHIFT 20
#define AARCH32_ID_MMFR3_COHWALK_MASK 0xf00000U
#define AARCH32_ID_MMFR3_COHWALK_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH32_ID_MMFR3_CMEMSZ( _val ) ( ( _val ) << 24 )
#define AARCH32_ID_MMFR3_CMEMSZ_SHIFT 24
#define AARCH32_ID_MMFR3_CMEMSZ_MASK 0xf000000U
#define AARCH32_ID_MMFR3_CMEMSZ_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH32_ID_MMFR3_SUPERSEC( _val ) ( ( _val ) << 28 )
#define AARCH32_ID_MMFR3_SUPERSEC_SHIFT 28
#define AARCH32_ID_MMFR3_SUPERSEC_MASK 0xf0000000U
#define AARCH32_ID_MMFR3_SUPERSEC_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint32_t _AArch32_Read_id_mmfr3( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c1, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_MMFR4, Memory Model Feature Register 4 */

#define AARCH32_ID_MMFR4_SPECSEI( _val ) ( ( _val ) << 0 )
#define AARCH32_ID_MMFR4_SPECSEI_SHIFT 0
#define AARCH32_ID_MMFR4_SPECSEI_MASK 0xfU
#define AARCH32_ID_MMFR4_SPECSEI_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_ID_MMFR4_AC2( _val ) ( ( _val ) << 4 )
#define AARCH32_ID_MMFR4_AC2_SHIFT 4
#define AARCH32_ID_MMFR4_AC2_MASK 0xf0U
#define AARCH32_ID_MMFR4_AC2_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH32_ID_MMFR4_XNX( _val ) ( ( _val ) << 8 )
#define AARCH32_ID_MMFR4_XNX_SHIFT 8
#define AARCH32_ID_MMFR4_XNX_MASK 0xf00U
#define AARCH32_ID_MMFR4_XNX_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH32_ID_MMFR4_CNP( _val ) ( ( _val ) << 12 )
#define AARCH32_ID_MMFR4_CNP_SHIFT 12
#define AARCH32_ID_MMFR4_CNP_MASK 0xf000U
#define AARCH32_ID_MMFR4_CNP_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH32_ID_MMFR4_HPDS( _val ) ( ( _val ) << 16 )
#define AARCH32_ID_MMFR4_HPDS_SHIFT 16
#define AARCH32_ID_MMFR4_HPDS_MASK 0xf0000U
#define AARCH32_ID_MMFR4_HPDS_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_ID_MMFR4_LSM( _val ) ( ( _val ) << 20 )
#define AARCH32_ID_MMFR4_LSM_SHIFT 20
#define AARCH32_ID_MMFR4_LSM_MASK 0xf00000U
#define AARCH32_ID_MMFR4_LSM_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH32_ID_MMFR4_CCIDX( _val ) ( ( _val ) << 24 )
#define AARCH32_ID_MMFR4_CCIDX_SHIFT 24
#define AARCH32_ID_MMFR4_CCIDX_MASK 0xf000000U
#define AARCH32_ID_MMFR4_CCIDX_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH32_ID_MMFR4_EVT( _val ) ( ( _val ) << 28 )
#define AARCH32_ID_MMFR4_EVT_SHIFT 28
#define AARCH32_ID_MMFR4_EVT_MASK 0xf0000000U
#define AARCH32_ID_MMFR4_EVT_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint32_t _AArch32_Read_id_mmfr4( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c2, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_MMFR5, Memory Model Feature Register 5 */

#define AARCH32_ID_MMFR5_ETS( _val ) ( ( _val ) << 0 )
#define AARCH32_ID_MMFR5_ETS_SHIFT 0
#define AARCH32_ID_MMFR5_ETS_MASK 0xfU
#define AARCH32_ID_MMFR5_ETS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

static inline uint32_t _AArch32_Read_id_mmfr5( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c3, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_PFR0, Processor Feature Register 0 */

#define AARCH32_ID_PFR0_STATE0( _val ) ( ( _val ) << 0 )
#define AARCH32_ID_PFR0_STATE0_SHIFT 0
#define AARCH32_ID_PFR0_STATE0_MASK 0xfU
#define AARCH32_ID_PFR0_STATE0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_ID_PFR0_STATE1( _val ) ( ( _val ) << 4 )
#define AARCH32_ID_PFR0_STATE1_SHIFT 4
#define AARCH32_ID_PFR0_STATE1_MASK 0xf0U
#define AARCH32_ID_PFR0_STATE1_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH32_ID_PFR0_STATE2( _val ) ( ( _val ) << 8 )
#define AARCH32_ID_PFR0_STATE2_SHIFT 8
#define AARCH32_ID_PFR0_STATE2_MASK 0xf00U
#define AARCH32_ID_PFR0_STATE2_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH32_ID_PFR0_STATE3( _val ) ( ( _val ) << 12 )
#define AARCH32_ID_PFR0_STATE3_SHIFT 12
#define AARCH32_ID_PFR0_STATE3_MASK 0xf000U
#define AARCH32_ID_PFR0_STATE3_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH32_ID_PFR0_CSV2( _val ) ( ( _val ) << 16 )
#define AARCH32_ID_PFR0_CSV2_SHIFT 16
#define AARCH32_ID_PFR0_CSV2_MASK 0xf0000U
#define AARCH32_ID_PFR0_CSV2_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_ID_PFR0_AMU( _val ) ( ( _val ) << 20 )
#define AARCH32_ID_PFR0_AMU_SHIFT 20
#define AARCH32_ID_PFR0_AMU_MASK 0xf00000U
#define AARCH32_ID_PFR0_AMU_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH32_ID_PFR0_DIT( _val ) ( ( _val ) << 24 )
#define AARCH32_ID_PFR0_DIT_SHIFT 24
#define AARCH32_ID_PFR0_DIT_MASK 0xf000000U
#define AARCH32_ID_PFR0_DIT_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH32_ID_PFR0_RAS( _val ) ( ( _val ) << 28 )
#define AARCH32_ID_PFR0_RAS_SHIFT 28
#define AARCH32_ID_PFR0_RAS_MASK 0xf0000000U
#define AARCH32_ID_PFR0_RAS_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint32_t _AArch32_Read_id_pfr0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c1, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_PFR1, Processor Feature Register 1 */

#define AARCH32_ID_PFR1_PROGMOD( _val ) ( ( _val ) << 0 )
#define AARCH32_ID_PFR1_PROGMOD_SHIFT 0
#define AARCH32_ID_PFR1_PROGMOD_MASK 0xfU
#define AARCH32_ID_PFR1_PROGMOD_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_ID_PFR1_SECURITY( _val ) ( ( _val ) << 4 )
#define AARCH32_ID_PFR1_SECURITY_SHIFT 4
#define AARCH32_ID_PFR1_SECURITY_MASK 0xf0U
#define AARCH32_ID_PFR1_SECURITY_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH32_ID_PFR1_MPROGMOD( _val ) ( ( _val ) << 8 )
#define AARCH32_ID_PFR1_MPROGMOD_SHIFT 8
#define AARCH32_ID_PFR1_MPROGMOD_MASK 0xf00U
#define AARCH32_ID_PFR1_MPROGMOD_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH32_ID_PFR1_VIRTUALIZATION( _val ) ( ( _val ) << 12 )
#define AARCH32_ID_PFR1_VIRTUALIZATION_SHIFT 12
#define AARCH32_ID_PFR1_VIRTUALIZATION_MASK 0xf000U
#define AARCH32_ID_PFR1_VIRTUALIZATION_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH32_ID_PFR1_GENTIMER( _val ) ( ( _val ) << 16 )
#define AARCH32_ID_PFR1_GENTIMER_SHIFT 16
#define AARCH32_ID_PFR1_GENTIMER_MASK 0xf0000U
#define AARCH32_ID_PFR1_GENTIMER_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_ID_PFR1_SEC_FRAC( _val ) ( ( _val ) << 20 )
#define AARCH32_ID_PFR1_SEC_FRAC_SHIFT 20
#define AARCH32_ID_PFR1_SEC_FRAC_MASK 0xf00000U
#define AARCH32_ID_PFR1_SEC_FRAC_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH32_ID_PFR1_VIRT_FRAC( _val ) ( ( _val ) << 24 )
#define AARCH32_ID_PFR1_VIRT_FRAC_SHIFT 24
#define AARCH32_ID_PFR1_VIRT_FRAC_MASK 0xf000000U
#define AARCH32_ID_PFR1_VIRT_FRAC_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH32_ID_PFR1_GIC( _val ) ( ( _val ) << 28 )
#define AARCH32_ID_PFR1_GIC_SHIFT 28
#define AARCH32_ID_PFR1_GIC_MASK 0xf0000000U
#define AARCH32_ID_PFR1_GIC_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint32_t _AArch32_Read_id_pfr1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c1, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_PFR2, Processor Feature Register 2 */

#define AARCH32_ID_PFR2_CSV3( _val ) ( ( _val ) << 0 )
#define AARCH32_ID_PFR2_CSV3_SHIFT 0
#define AARCH32_ID_PFR2_CSV3_MASK 0xfU
#define AARCH32_ID_PFR2_CSV3_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_ID_PFR2_SSBS( _val ) ( ( _val ) << 4 )
#define AARCH32_ID_PFR2_SSBS_SHIFT 4
#define AARCH32_ID_PFR2_SSBS_MASK 0xf0U
#define AARCH32_ID_PFR2_SSBS_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH32_ID_PFR2_RAS_FRAC( _val ) ( ( _val ) << 8 )
#define AARCH32_ID_PFR2_RAS_FRAC_SHIFT 8
#define AARCH32_ID_PFR2_RAS_FRAC_MASK 0xf00U
#define AARCH32_ID_PFR2_RAS_FRAC_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

static inline uint32_t _AArch32_Read_id_pfr2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c3, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* IFAR, Instruction Fault Address Register */

static inline uint32_t _AArch32_Read_ifar( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c0, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_ifar( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c0, 2" : : "r" ( value ) : "memory"
  );
}

/* IFSR, Instruction Fault Status Register */

#define AARCH32_IFSR_FS_3_0( _val ) ( ( _val ) << 0 )
#define AARCH32_IFSR_FS_3_0_SHIFT 0
#define AARCH32_IFSR_FS_3_0_MASK 0xfU
#define AARCH32_IFSR_FS_3_0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_IFSR_STATUS( _val ) ( ( _val ) << 0 )
#define AARCH32_IFSR_STATUS_SHIFT 0
#define AARCH32_IFSR_STATUS_MASK 0x3fU
#define AARCH32_IFSR_STATUS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )

#define AARCH32_IFSR_LPAE 0x200U

#define AARCH32_IFSR_FS_4 0x400U

#define AARCH32_IFSR_EXT 0x1000U

#define AARCH32_IFSR_FNV 0x10000U

static inline uint32_t _AArch32_Read_ifsr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c5, c0, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_ifsr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c5, c0, 1" : : "r" ( value ) : "memory"
  );
}

/* ISR, Interrupt Status Register */

#define AARCH32_ISR_F 0x40U

#define AARCH32_ISR_I 0x80U

#define AARCH32_ISR_A 0x100U

static inline uint32_t _AArch32_Read_isr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c12, c1, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ITLBIALL, Instruction TLB Invalidate All */

static inline void _AArch32_Write_itlbiall( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c8, c5, 0" : : "r" ( value ) : "memory"
  );
}

/* ITLBIASID, Instruction TLB Invalidate by ASID match */

#define AARCH32_ITLBIASID_ASID( _val ) ( ( _val ) << 0 )
#define AARCH32_ITLBIASID_ASID_SHIFT 0
#define AARCH32_ITLBIASID_ASID_MASK 0xffU
#define AARCH32_ITLBIASID_ASID_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

static inline void _AArch32_Write_itlbiasid( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c8, c5, 2" : : "r" ( value ) : "memory"
  );
}

/* ITLBIMVA, Instruction TLB Invalidate by VA */

#define AARCH32_ITLBIMVA_ASID( _val ) ( ( _val ) << 0 )
#define AARCH32_ITLBIMVA_ASID_SHIFT 0
#define AARCH32_ITLBIMVA_ASID_MASK 0xffU
#define AARCH32_ITLBIMVA_ASID_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

#define AARCH32_ITLBIMVA_VA( _val ) ( ( _val ) << 12 )
#define AARCH32_ITLBIMVA_VA_SHIFT 12
#define AARCH32_ITLBIMVA_VA_MASK 0xfffff000U
#define AARCH32_ITLBIMVA_VA_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfffffU )

static inline void _AArch32_Write_itlbimva( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c8, c5, 1" : : "r" ( value ) : "memory"
  );
}

/* JIDR, Jazelle ID Register */

static inline uint32_t _AArch32_Read_jidr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 7, %0, c0, c0, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* JMCR, Jazelle Main Configuration Register */

static inline uint32_t _AArch32_Read_jmcr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 7, %0, c2, c0, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_jmcr( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 7, %0, c2, c0, 0" : : "r" ( value ) : "memory"
  );
}

/* JOSCR, Jazelle OS Control Register */

static inline uint32_t _AArch32_Read_joscr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 7, %0, c1, c0, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_joscr( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 7, %0, c1, c0, 0" : : "r" ( value ) : "memory"
  );
}

/* MAIR0, Memory Attribute Indirection Register 0 */

static inline uint32_t _AArch32_Read_mair0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c10, c2, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_mair0( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c10, c2, 0" : : "r" ( value ) : "memory"
  );
}

/* MAIR1, Memory Attribute Indirection Register 1 */

static inline uint32_t _AArch32_Read_mair1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c10, c2, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_mair1( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c10, c2, 1" : : "r" ( value ) : "memory"
  );
}

/* MIDR, Main ID Register */

#define AARCH32_MIDR_REVISION( _val ) ( ( _val ) << 0 )
#define AARCH32_MIDR_REVISION_SHIFT 0
#define AARCH32_MIDR_REVISION_MASK 0xfU
#define AARCH32_MIDR_REVISION_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_MIDR_PARTNUM( _val ) ( ( _val ) << 4 )
#define AARCH32_MIDR_PARTNUM_SHIFT 4
#define AARCH32_MIDR_PARTNUM_MASK 0xfff0U
#define AARCH32_MIDR_PARTNUM_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfffU )

#define AARCH32_MIDR_ARCHITECTURE( _val ) ( ( _val ) << 16 )
#define AARCH32_MIDR_ARCHITECTURE_SHIFT 16
#define AARCH32_MIDR_ARCHITECTURE_MASK 0xf0000U
#define AARCH32_MIDR_ARCHITECTURE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_MIDR_VARIANT( _val ) ( ( _val ) << 20 )
#define AARCH32_MIDR_VARIANT_SHIFT 20
#define AARCH32_MIDR_VARIANT_MASK 0xf00000U
#define AARCH32_MIDR_VARIANT_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH32_MIDR_IMPLEMENTER( _val ) ( ( _val ) << 24 )
#define AARCH32_MIDR_IMPLEMENTER_SHIFT 24
#define AARCH32_MIDR_IMPLEMENTER_MASK 0xff000000U
#define AARCH32_MIDR_IMPLEMENTER_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xffU )

static inline uint32_t _AArch32_Read_midr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c0, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* MPIDR, Multiprocessor Affinity Register */

#define AARCH32_MPIDR_AFF0( _val ) ( ( _val ) << 0 )
#define AARCH32_MPIDR_AFF0_SHIFT 0
#define AARCH32_MPIDR_AFF0_MASK 0xffU
#define AARCH32_MPIDR_AFF0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

#define AARCH32_MPIDR_AFF1( _val ) ( ( _val ) << 8 )
#define AARCH32_MPIDR_AFF1_SHIFT 8
#define AARCH32_MPIDR_AFF1_MASK 0xff00U
#define AARCH32_MPIDR_AFF1_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xffU )

#define AARCH32_MPIDR_AFF2( _val ) ( ( _val ) << 16 )
#define AARCH32_MPIDR_AFF2_SHIFT 16
#define AARCH32_MPIDR_AFF2_MASK 0xff0000U
#define AARCH32_MPIDR_AFF2_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xffU )

#define AARCH32_MPIDR_MT 0x1000000U

#define AARCH32_MPIDR_U 0x40000000U

#define AARCH32_MPIDR_M 0x80000000U

static inline uint32_t _AArch32_Read_mpidr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c0, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* MVBAR, Monitor Vector Base Address Register */

#define AARCH32_MVBAR_RESERVED( _val ) ( ( _val ) << 0 )
#define AARCH32_MVBAR_RESERVED_SHIFT 0
#define AARCH32_MVBAR_RESERVED_MASK 0x1fU
#define AARCH32_MVBAR_RESERVED_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1fU )

static inline uint32_t _AArch32_Read_mvbar( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c12, c0, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_mvbar( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c12, c0, 1" : : "r" ( value ) : "memory"
  );
}

/* MVFR0, Media and VFP Feature Register 0 */

#define AARCH32_MVFR0_SIMDREG( _val ) ( ( _val ) << 0 )
#define AARCH32_MVFR0_SIMDREG_SHIFT 0
#define AARCH32_MVFR0_SIMDREG_MASK 0xfU
#define AARCH32_MVFR0_SIMDREG_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_MVFR0_FPSP( _val ) ( ( _val ) << 4 )
#define AARCH32_MVFR0_FPSP_SHIFT 4
#define AARCH32_MVFR0_FPSP_MASK 0xf0U
#define AARCH32_MVFR0_FPSP_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH32_MVFR0_FPDP( _val ) ( ( _val ) << 8 )
#define AARCH32_MVFR0_FPDP_SHIFT 8
#define AARCH32_MVFR0_FPDP_MASK 0xf00U
#define AARCH32_MVFR0_FPDP_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH32_MVFR0_FPTRAP( _val ) ( ( _val ) << 12 )
#define AARCH32_MVFR0_FPTRAP_SHIFT 12
#define AARCH32_MVFR0_FPTRAP_MASK 0xf000U
#define AARCH32_MVFR0_FPTRAP_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH32_MVFR0_FPDIVIDE( _val ) ( ( _val ) << 16 )
#define AARCH32_MVFR0_FPDIVIDE_SHIFT 16
#define AARCH32_MVFR0_FPDIVIDE_MASK 0xf0000U
#define AARCH32_MVFR0_FPDIVIDE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_MVFR0_FPSQRT( _val ) ( ( _val ) << 20 )
#define AARCH32_MVFR0_FPSQRT_SHIFT 20
#define AARCH32_MVFR0_FPSQRT_MASK 0xf00000U
#define AARCH32_MVFR0_FPSQRT_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH32_MVFR0_FPSHVEC( _val ) ( ( _val ) << 24 )
#define AARCH32_MVFR0_FPSHVEC_SHIFT 24
#define AARCH32_MVFR0_FPSHVEC_MASK 0xf000000U
#define AARCH32_MVFR0_FPSHVEC_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH32_MVFR0_FPROUND( _val ) ( ( _val ) << 28 )
#define AARCH32_MVFR0_FPROUND_SHIFT 28
#define AARCH32_MVFR0_FPROUND_MASK 0xf0000000U
#define AARCH32_MVFR0_FPROUND_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

/* MVFR1, Media and VFP Feature Register 1 */

#define AARCH32_MVFR1_FPFTZ( _val ) ( ( _val ) << 0 )
#define AARCH32_MVFR1_FPFTZ_SHIFT 0
#define AARCH32_MVFR1_FPFTZ_MASK 0xfU
#define AARCH32_MVFR1_FPFTZ_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_MVFR1_FPDNAN( _val ) ( ( _val ) << 4 )
#define AARCH32_MVFR1_FPDNAN_SHIFT 4
#define AARCH32_MVFR1_FPDNAN_MASK 0xf0U
#define AARCH32_MVFR1_FPDNAN_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH32_MVFR1_SIMDLS( _val ) ( ( _val ) << 8 )
#define AARCH32_MVFR1_SIMDLS_SHIFT 8
#define AARCH32_MVFR1_SIMDLS_MASK 0xf00U
#define AARCH32_MVFR1_SIMDLS_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH32_MVFR1_SIMDINT( _val ) ( ( _val ) << 12 )
#define AARCH32_MVFR1_SIMDINT_SHIFT 12
#define AARCH32_MVFR1_SIMDINT_MASK 0xf000U
#define AARCH32_MVFR1_SIMDINT_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH32_MVFR1_SIMDSP( _val ) ( ( _val ) << 16 )
#define AARCH32_MVFR1_SIMDSP_SHIFT 16
#define AARCH32_MVFR1_SIMDSP_MASK 0xf0000U
#define AARCH32_MVFR1_SIMDSP_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_MVFR1_SIMDHP( _val ) ( ( _val ) << 20 )
#define AARCH32_MVFR1_SIMDHP_SHIFT 20
#define AARCH32_MVFR1_SIMDHP_MASK 0xf00000U
#define AARCH32_MVFR1_SIMDHP_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH32_MVFR1_FPHP( _val ) ( ( _val ) << 24 )
#define AARCH32_MVFR1_FPHP_SHIFT 24
#define AARCH32_MVFR1_FPHP_MASK 0xf000000U
#define AARCH32_MVFR1_FPHP_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH32_MVFR1_SIMDFMAC( _val ) ( ( _val ) << 28 )
#define AARCH32_MVFR1_SIMDFMAC_SHIFT 28
#define AARCH32_MVFR1_SIMDFMAC_MASK 0xf0000000U
#define AARCH32_MVFR1_SIMDFMAC_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

/* MVFR2, Media and VFP Feature Register 2 */

#define AARCH32_MVFR2_SIMDMISC( _val ) ( ( _val ) << 0 )
#define AARCH32_MVFR2_SIMDMISC_SHIFT 0
#define AARCH32_MVFR2_SIMDMISC_MASK 0xfU
#define AARCH32_MVFR2_SIMDMISC_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_MVFR2_FPMISC( _val ) ( ( _val ) << 4 )
#define AARCH32_MVFR2_FPMISC_SHIFT 4
#define AARCH32_MVFR2_FPMISC_MASK 0xf0U
#define AARCH32_MVFR2_FPMISC_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

/* NMRR, Normal Memory Remap Register */

static inline uint32_t _AArch32_Read_nmrr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c10, c2, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_nmrr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c10, c2, 1" : : "r" ( value ) : "memory"
  );
}

/* NSACR, Non-Secure Access Control Register */

#define AARCH32_NSACR_CP10 0x400U

#define AARCH32_NSACR_CP11 0x800U

#define AARCH32_NSACR_NSASEDIS 0x8000U

#define AARCH32_NSACR_NSTRCDIS 0x100000U

static inline uint32_t _AArch32_Read_nsacr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c1, c1, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_nsacr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c1, c1, 2" : : "r" ( value ) : "memory"
  );
}

/* PAR, Physical Address Register */

#define AARCH32_PAR_F 0x1U

#define AARCH32_PAR_SS 0x2U

#define AARCH32_PAR_FS_4_0( _val ) ( ( _val ) << 1 )
#define AARCH32_PAR_FS_4_0_SHIFT 1
#define AARCH32_PAR_FS_4_0_MASK 0x3eU
#define AARCH32_PAR_FS_4_0_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x1fU )

#define AARCH32_PAR_FST( _val ) ( ( _val ) << 1 )
#define AARCH32_PAR_FST_SHIFT 1
#define AARCH32_PAR_FST_MASK 0x7eU
#define AARCH32_PAR_FST_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x3fU )

#define AARCH32_PAR_OUTER_1_0( _val ) ( ( _val ) << 2 )
#define AARCH32_PAR_OUTER_1_0_SHIFT 2
#define AARCH32_PAR_OUTER_1_0_MASK 0xcU
#define AARCH32_PAR_OUTER_1_0_GET( _reg ) \
  ( ( ( _reg ) >> 2 ) & 0x3U )

#define AARCH32_PAR_INNER_2_0( _val ) ( ( _val ) << 4 )
#define AARCH32_PAR_INNER_2_0_SHIFT 4
#define AARCH32_PAR_INNER_2_0_MASK 0x70U
#define AARCH32_PAR_INNER_2_0_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0x7U )

#define AARCH32_PAR_FS_5 0x40U

#define AARCH32_PAR_SH_0 0x80U

#define AARCH32_PAR_SH_1( _val ) ( ( _val ) << 7 )
#define AARCH32_PAR_SH_SHIFT_1 7
#define AARCH32_PAR_SH_MASK_1 0x180U
#define AARCH32_PAR_SH_GET_1( _reg ) \
  ( ( ( _reg ) >> 7 ) & 0x3U )

#define AARCH32_PAR_S2WLK 0x100U

#define AARCH32_PAR_FSTAGE 0x200U

#define AARCH32_PAR_NS 0x200U

#define AARCH32_PAR_NOS 0x400U

#define AARCH32_PAR_LPAE 0x800U

#define AARCH32_PAR_PA_0( _val ) ( ( _val ) << 12 )
#define AARCH32_PAR_PA_SHIFT_0 12
#define AARCH32_PAR_PA_MASK_0 0xfffff000U
#define AARCH32_PAR_PA_GET_0( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfffffU )

#define AARCH32_PAR_PA_1( _val ) ( ( _val ) << 12 )
#define AARCH32_PAR_PA_SHIFT_1 12
#define AARCH32_PAR_PA_MASK_1 0xfffffff000ULL
#define AARCH32_PAR_PA_GET_1( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfffffffULL )

#define AARCH32_PAR_ATTR( _val ) ( ( _val ) << 56 )
#define AARCH32_PAR_ATTR_SHIFT 56
#define AARCH32_PAR_ATTR_MASK 0xff00000000000000ULL
#define AARCH32_PAR_ATTR_GET( _reg ) \
  ( ( ( _reg ) >> 56 ) & 0xffULL )

static inline uint32_t _AArch32_Read_32_par( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c7, c4, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_32_par( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c7, c4, 0" : : "r" ( value ) : "memory"
  );
}

/* PAR, Physical Address Register */

static inline uint64_t _AArch32_Read_64_par( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 0, %Q0, %R0, c7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_64_par( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 0, %Q0, %R0, c7" : : "r" ( value ) : "memory"
  );
}

/* PRRR, Primary Region Remap Register */

#define AARCH32_PRRR_DS0 0x10000U

#define AARCH32_PRRR_DS1 0x20000U

#define AARCH32_PRRR_NS0 0x40000U

#define AARCH32_PRRR_NS1 0x80000U

static inline uint32_t _AArch32_Read_prrr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c10, c2, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prrr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c10, c2, 0" : : "r" ( value ) : "memory"
  );
}

/* REVIDR, Revision ID Register */

static inline uint32_t _AArch32_Read_revidr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c0, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* RMR, Reset Management Register */

#define AARCH32_RMR_AA64 0x1U

#define AARCH32_RMR_RR 0x2U

static inline uint32_t _AArch32_Read_rmr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c12, c0, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_rmr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c12, c0, 2" : : "r" ( value ) : "memory"
  );
}

/* RVBAR, Reset Vector Base Address Register */

static inline uint32_t _AArch32_Read_rvbar( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c12, c0, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* SCR, Secure Configuration Register */

#define AARCH32_SCR_NS 0x1U

#define AARCH32_SCR_IRQ 0x2U

#define AARCH32_SCR_FIQ 0x4U

#define AARCH32_SCR_EA 0x8U

#define AARCH32_SCR_FW 0x10U

#define AARCH32_SCR_AW 0x20U

#define AARCH32_SCR_NET 0x40U

#define AARCH32_SCR_SCD 0x80U

#define AARCH32_SCR_HCE 0x100U

#define AARCH32_SCR_SIF 0x200U

#define AARCH32_SCR_TWI 0x1000U

#define AARCH32_SCR_TWE 0x2000U

#define AARCH32_SCR_TERR 0x8000U

static inline uint32_t _AArch32_Read_scr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c1, c1, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_scr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c1, c1, 0" : : "r" ( value ) : "memory"
  );
}

/* SCTLR, System Control Register */

#define AARCH32_SCTLR_M 0x1U

#define AARCH32_SCTLR_A 0x2U

#define AARCH32_SCTLR_C 0x4U

#define AARCH32_SCTLR_NTLSMD 0x8U

#define AARCH32_SCTLR_LSMAOE 0x10U

#define AARCH32_SCTLR_CP15BEN 0x20U

#define AARCH32_SCTLR_UNK 0x40U

#define AARCH32_SCTLR_ITD 0x80U

#define AARCH32_SCTLR_SED 0x100U

#define AARCH32_SCTLR_ENRCTX 0x400U

#define AARCH32_SCTLR_I 0x1000U

#define AARCH32_SCTLR_V 0x2000U

#define AARCH32_SCTLR_NTWI 0x10000U

#define AARCH32_SCTLR_BR 0x20000U

#define AARCH32_SCTLR_NTWE 0x40000U

#define AARCH32_SCTLR_WXN 0x80000U

#define AARCH32_SCTLR_UWXN 0x100000U

#define AARCH32_SCTLR_FI 0x200000U

#define AARCH32_SCTLR_SPAN 0x800000U

#define AARCH32_SCTLR_EE 0x2000000U

#define AARCH32_SCTLR_TRE 0x10000000U

#define AARCH32_SCTLR_AFE 0x20000000U

#define AARCH32_SCTLR_TE 0x40000000U

#define AARCH32_SCTLR_DSSBS 0x80000000U

static inline uint32_t _AArch32_Read_sctlr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c1, c0, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_sctlr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c1, c0, 0" : : "r" ( value ) : "memory"
  );
}

/* SPSR, Saved Program Status Register */

#define AARCH32_SPSR_M_4_0( _val ) ( ( _val ) << 0 )
#define AARCH32_SPSR_M_4_0_SHIFT 0
#define AARCH32_SPSR_M_4_0_MASK 0x1fU
#define AARCH32_SPSR_M_4_0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1fU )

#define AARCH32_SPSR_T 0x20U

#define AARCH32_SPSR_F 0x40U

#define AARCH32_SPSR_I 0x80U

#define AARCH32_SPSR_A 0x100U

#define AARCH32_SPSR_E 0x200U

#define AARCH32_SPSR_IT_7_2( _val ) ( ( _val ) << 10 )
#define AARCH32_SPSR_IT_7_2_SHIFT 10
#define AARCH32_SPSR_IT_7_2_MASK 0xfc00U
#define AARCH32_SPSR_IT_7_2_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3fU )

#define AARCH32_SPSR_GE( _val ) ( ( _val ) << 16 )
#define AARCH32_SPSR_GE_SHIFT 16
#define AARCH32_SPSR_GE_MASK 0xf0000U
#define AARCH32_SPSR_GE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_SPSR_IL 0x100000U

#define AARCH32_SPSR_DIT 0x200000U

#define AARCH32_SPSR_PAN 0x400000U

#define AARCH32_SPSR_SSBS 0x800000U

#define AARCH32_SPSR_J 0x1000000U

#define AARCH32_SPSR_IT_1_0( _val ) ( ( _val ) << 25 )
#define AARCH32_SPSR_IT_1_0_SHIFT 25
#define AARCH32_SPSR_IT_1_0_MASK 0x6000000U
#define AARCH32_SPSR_IT_1_0_GET( _reg ) \
  ( ( ( _reg ) >> 25 ) & 0x3U )

#define AARCH32_SPSR_Q 0x8000000U

#define AARCH32_SPSR_V 0x10000000U

#define AARCH32_SPSR_C 0x20000000U

#define AARCH32_SPSR_Z 0x40000000U

#define AARCH32_SPSR_N 0x80000000U

/* SPSR_ABT, Saved Program Status Register (Abort mode) */

#define AARCH32_SPSR_ABT_M_4_0( _val ) ( ( _val ) << 0 )
#define AARCH32_SPSR_ABT_M_4_0_SHIFT 0
#define AARCH32_SPSR_ABT_M_4_0_MASK 0x1fU
#define AARCH32_SPSR_ABT_M_4_0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1fU )

#define AARCH32_SPSR_ABT_T 0x20U

#define AARCH32_SPSR_ABT_F 0x40U

#define AARCH32_SPSR_ABT_I 0x80U

#define AARCH32_SPSR_ABT_A 0x100U

#define AARCH32_SPSR_ABT_E 0x200U

#define AARCH32_SPSR_ABT_IT_7_2( _val ) ( ( _val ) << 10 )
#define AARCH32_SPSR_ABT_IT_7_2_SHIFT 10
#define AARCH32_SPSR_ABT_IT_7_2_MASK 0xfc00U
#define AARCH32_SPSR_ABT_IT_7_2_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3fU )

#define AARCH32_SPSR_ABT_GE( _val ) ( ( _val ) << 16 )
#define AARCH32_SPSR_ABT_GE_SHIFT 16
#define AARCH32_SPSR_ABT_GE_MASK 0xf0000U
#define AARCH32_SPSR_ABT_GE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_SPSR_ABT_IL 0x100000U

#define AARCH32_SPSR_ABT_DIT 0x200000U

#define AARCH32_SPSR_ABT_PAN 0x400000U

#define AARCH32_SPSR_ABT_SSBS 0x800000U

#define AARCH32_SPSR_ABT_J 0x1000000U

#define AARCH32_SPSR_ABT_IT_1_0( _val ) ( ( _val ) << 25 )
#define AARCH32_SPSR_ABT_IT_1_0_SHIFT 25
#define AARCH32_SPSR_ABT_IT_1_0_MASK 0x6000000U
#define AARCH32_SPSR_ABT_IT_1_0_GET( _reg ) \
  ( ( ( _reg ) >> 25 ) & 0x3U )

#define AARCH32_SPSR_ABT_Q 0x8000000U

#define AARCH32_SPSR_ABT_V 0x10000000U

#define AARCH32_SPSR_ABT_C 0x20000000U

#define AARCH32_SPSR_ABT_Z 0x40000000U

#define AARCH32_SPSR_ABT_N 0x80000000U

/* SPSR_FIQ, Saved Program Status Register (FIQ mode) */

#define AARCH32_SPSR_FIQ_M_4_0( _val ) ( ( _val ) << 0 )
#define AARCH32_SPSR_FIQ_M_4_0_SHIFT 0
#define AARCH32_SPSR_FIQ_M_4_0_MASK 0x1fU
#define AARCH32_SPSR_FIQ_M_4_0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1fU )

#define AARCH32_SPSR_FIQ_T 0x20U

#define AARCH32_SPSR_FIQ_F 0x40U

#define AARCH32_SPSR_FIQ_I 0x80U

#define AARCH32_SPSR_FIQ_A 0x100U

#define AARCH32_SPSR_FIQ_E 0x200U

#define AARCH32_SPSR_FIQ_IT_7_2( _val ) ( ( _val ) << 10 )
#define AARCH32_SPSR_FIQ_IT_7_2_SHIFT 10
#define AARCH32_SPSR_FIQ_IT_7_2_MASK 0xfc00U
#define AARCH32_SPSR_FIQ_IT_7_2_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3fU )

#define AARCH32_SPSR_FIQ_GE( _val ) ( ( _val ) << 16 )
#define AARCH32_SPSR_FIQ_GE_SHIFT 16
#define AARCH32_SPSR_FIQ_GE_MASK 0xf0000U
#define AARCH32_SPSR_FIQ_GE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_SPSR_FIQ_IL 0x100000U

#define AARCH32_SPSR_FIQ_DIT 0x200000U

#define AARCH32_SPSR_FIQ_PAN 0x400000U

#define AARCH32_SPSR_FIQ_SSBS 0x800000U

#define AARCH32_SPSR_FIQ_J 0x1000000U

#define AARCH32_SPSR_FIQ_IT_1_0( _val ) ( ( _val ) << 25 )
#define AARCH32_SPSR_FIQ_IT_1_0_SHIFT 25
#define AARCH32_SPSR_FIQ_IT_1_0_MASK 0x6000000U
#define AARCH32_SPSR_FIQ_IT_1_0_GET( _reg ) \
  ( ( ( _reg ) >> 25 ) & 0x3U )

#define AARCH32_SPSR_FIQ_Q 0x8000000U

#define AARCH32_SPSR_FIQ_V 0x10000000U

#define AARCH32_SPSR_FIQ_C 0x20000000U

#define AARCH32_SPSR_FIQ_Z 0x40000000U

#define AARCH32_SPSR_FIQ_N 0x80000000U

/* SPSR_HYP, Saved Program Status Register (Hyp mode) */

#define AARCH32_SPSR_HYP_M_4_0( _val ) ( ( _val ) << 0 )
#define AARCH32_SPSR_HYP_M_4_0_SHIFT 0
#define AARCH32_SPSR_HYP_M_4_0_MASK 0x1fU
#define AARCH32_SPSR_HYP_M_4_0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1fU )

#define AARCH32_SPSR_HYP_T 0x20U

#define AARCH32_SPSR_HYP_F 0x40U

#define AARCH32_SPSR_HYP_I 0x80U

#define AARCH32_SPSR_HYP_A 0x100U

#define AARCH32_SPSR_HYP_E 0x200U

#define AARCH32_SPSR_HYP_IT_7_2( _val ) ( ( _val ) << 10 )
#define AARCH32_SPSR_HYP_IT_7_2_SHIFT 10
#define AARCH32_SPSR_HYP_IT_7_2_MASK 0xfc00U
#define AARCH32_SPSR_HYP_IT_7_2_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3fU )

#define AARCH32_SPSR_HYP_GE( _val ) ( ( _val ) << 16 )
#define AARCH32_SPSR_HYP_GE_SHIFT 16
#define AARCH32_SPSR_HYP_GE_MASK 0xf0000U
#define AARCH32_SPSR_HYP_GE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_SPSR_HYP_IL 0x100000U

#define AARCH32_SPSR_HYP_DIT 0x200000U

#define AARCH32_SPSR_HYP_PAN 0x400000U

#define AARCH32_SPSR_HYP_SSBS 0x800000U

#define AARCH32_SPSR_HYP_J 0x1000000U

#define AARCH32_SPSR_HYP_IT_1_0( _val ) ( ( _val ) << 25 )
#define AARCH32_SPSR_HYP_IT_1_0_SHIFT 25
#define AARCH32_SPSR_HYP_IT_1_0_MASK 0x6000000U
#define AARCH32_SPSR_HYP_IT_1_0_GET( _reg ) \
  ( ( ( _reg ) >> 25 ) & 0x3U )

#define AARCH32_SPSR_HYP_Q 0x8000000U

#define AARCH32_SPSR_HYP_V 0x10000000U

#define AARCH32_SPSR_HYP_C 0x20000000U

#define AARCH32_SPSR_HYP_Z 0x40000000U

#define AARCH32_SPSR_HYP_N 0x80000000U

/* SPSR_IRQ, Saved Program Status Register (IRQ mode) */

#define AARCH32_SPSR_IRQ_M_4_0( _val ) ( ( _val ) << 0 )
#define AARCH32_SPSR_IRQ_M_4_0_SHIFT 0
#define AARCH32_SPSR_IRQ_M_4_0_MASK 0x1fU
#define AARCH32_SPSR_IRQ_M_4_0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1fU )

#define AARCH32_SPSR_IRQ_T 0x20U

#define AARCH32_SPSR_IRQ_F 0x40U

#define AARCH32_SPSR_IRQ_I 0x80U

#define AARCH32_SPSR_IRQ_A 0x100U

#define AARCH32_SPSR_IRQ_E 0x200U

#define AARCH32_SPSR_IRQ_IT_7_2( _val ) ( ( _val ) << 10 )
#define AARCH32_SPSR_IRQ_IT_7_2_SHIFT 10
#define AARCH32_SPSR_IRQ_IT_7_2_MASK 0xfc00U
#define AARCH32_SPSR_IRQ_IT_7_2_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3fU )

#define AARCH32_SPSR_IRQ_GE( _val ) ( ( _val ) << 16 )
#define AARCH32_SPSR_IRQ_GE_SHIFT 16
#define AARCH32_SPSR_IRQ_GE_MASK 0xf0000U
#define AARCH32_SPSR_IRQ_GE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_SPSR_IRQ_IL 0x100000U

#define AARCH32_SPSR_IRQ_DIT 0x200000U

#define AARCH32_SPSR_IRQ_PAN 0x400000U

#define AARCH32_SPSR_IRQ_SSBS 0x800000U

#define AARCH32_SPSR_IRQ_J 0x1000000U

#define AARCH32_SPSR_IRQ_IT_1_0( _val ) ( ( _val ) << 25 )
#define AARCH32_SPSR_IRQ_IT_1_0_SHIFT 25
#define AARCH32_SPSR_IRQ_IT_1_0_MASK 0x6000000U
#define AARCH32_SPSR_IRQ_IT_1_0_GET( _reg ) \
  ( ( ( _reg ) >> 25 ) & 0x3U )

#define AARCH32_SPSR_IRQ_Q 0x8000000U

#define AARCH32_SPSR_IRQ_V 0x10000000U

#define AARCH32_SPSR_IRQ_C 0x20000000U

#define AARCH32_SPSR_IRQ_Z 0x40000000U

#define AARCH32_SPSR_IRQ_N 0x80000000U

/* SPSR_MON, Saved Program Status Register (Monitor mode) */

#define AARCH32_SPSR_MON_M_4_0( _val ) ( ( _val ) << 0 )
#define AARCH32_SPSR_MON_M_4_0_SHIFT 0
#define AARCH32_SPSR_MON_M_4_0_MASK 0x1fU
#define AARCH32_SPSR_MON_M_4_0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1fU )

#define AARCH32_SPSR_MON_T 0x20U

#define AARCH32_SPSR_MON_F 0x40U

#define AARCH32_SPSR_MON_I 0x80U

#define AARCH32_SPSR_MON_A 0x100U

#define AARCH32_SPSR_MON_E 0x200U

#define AARCH32_SPSR_MON_IT_7_2( _val ) ( ( _val ) << 10 )
#define AARCH32_SPSR_MON_IT_7_2_SHIFT 10
#define AARCH32_SPSR_MON_IT_7_2_MASK 0xfc00U
#define AARCH32_SPSR_MON_IT_7_2_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3fU )

#define AARCH32_SPSR_MON_GE( _val ) ( ( _val ) << 16 )
#define AARCH32_SPSR_MON_GE_SHIFT 16
#define AARCH32_SPSR_MON_GE_MASK 0xf0000U
#define AARCH32_SPSR_MON_GE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_SPSR_MON_IL 0x100000U

#define AARCH32_SPSR_MON_DIT 0x200000U

#define AARCH32_SPSR_MON_PAN 0x400000U

#define AARCH32_SPSR_MON_SSBS 0x800000U

#define AARCH32_SPSR_MON_J 0x1000000U

#define AARCH32_SPSR_MON_IT_1_0( _val ) ( ( _val ) << 25 )
#define AARCH32_SPSR_MON_IT_1_0_SHIFT 25
#define AARCH32_SPSR_MON_IT_1_0_MASK 0x6000000U
#define AARCH32_SPSR_MON_IT_1_0_GET( _reg ) \
  ( ( ( _reg ) >> 25 ) & 0x3U )

#define AARCH32_SPSR_MON_Q 0x8000000U

#define AARCH32_SPSR_MON_V 0x10000000U

#define AARCH32_SPSR_MON_C 0x20000000U

#define AARCH32_SPSR_MON_Z 0x40000000U

#define AARCH32_SPSR_MON_N 0x80000000U

/* SPSR_SVC, Saved Program Status Register (Supervisor mode) */

#define AARCH32_SPSR_SVC_M_4_0( _val ) ( ( _val ) << 0 )
#define AARCH32_SPSR_SVC_M_4_0_SHIFT 0
#define AARCH32_SPSR_SVC_M_4_0_MASK 0x1fU
#define AARCH32_SPSR_SVC_M_4_0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1fU )

#define AARCH32_SPSR_SVC_T 0x20U

#define AARCH32_SPSR_SVC_F 0x40U

#define AARCH32_SPSR_SVC_I 0x80U

#define AARCH32_SPSR_SVC_A 0x100U

#define AARCH32_SPSR_SVC_E 0x200U

#define AARCH32_SPSR_SVC_IT_7_2( _val ) ( ( _val ) << 10 )
#define AARCH32_SPSR_SVC_IT_7_2_SHIFT 10
#define AARCH32_SPSR_SVC_IT_7_2_MASK 0xfc00U
#define AARCH32_SPSR_SVC_IT_7_2_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3fU )

#define AARCH32_SPSR_SVC_GE( _val ) ( ( _val ) << 16 )
#define AARCH32_SPSR_SVC_GE_SHIFT 16
#define AARCH32_SPSR_SVC_GE_MASK 0xf0000U
#define AARCH32_SPSR_SVC_GE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_SPSR_SVC_IL 0x100000U

#define AARCH32_SPSR_SVC_DIT 0x200000U

#define AARCH32_SPSR_SVC_PAN 0x400000U

#define AARCH32_SPSR_SVC_SSBS 0x800000U

#define AARCH32_SPSR_SVC_J 0x1000000U

#define AARCH32_SPSR_SVC_IT_1_0( _val ) ( ( _val ) << 25 )
#define AARCH32_SPSR_SVC_IT_1_0_SHIFT 25
#define AARCH32_SPSR_SVC_IT_1_0_MASK 0x6000000U
#define AARCH32_SPSR_SVC_IT_1_0_GET( _reg ) \
  ( ( ( _reg ) >> 25 ) & 0x3U )

#define AARCH32_SPSR_SVC_Q 0x8000000U

#define AARCH32_SPSR_SVC_V 0x10000000U

#define AARCH32_SPSR_SVC_C 0x20000000U

#define AARCH32_SPSR_SVC_Z 0x40000000U

#define AARCH32_SPSR_SVC_N 0x80000000U

/* SPSR_UND, Saved Program Status Register (Undefined mode) */

#define AARCH32_SPSR_UND_M_4_0( _val ) ( ( _val ) << 0 )
#define AARCH32_SPSR_UND_M_4_0_SHIFT 0
#define AARCH32_SPSR_UND_M_4_0_MASK 0x1fU
#define AARCH32_SPSR_UND_M_4_0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1fU )

#define AARCH32_SPSR_UND_T 0x20U

#define AARCH32_SPSR_UND_F 0x40U

#define AARCH32_SPSR_UND_I 0x80U

#define AARCH32_SPSR_UND_A 0x100U

#define AARCH32_SPSR_UND_E 0x200U

#define AARCH32_SPSR_UND_IT_7_2( _val ) ( ( _val ) << 10 )
#define AARCH32_SPSR_UND_IT_7_2_SHIFT 10
#define AARCH32_SPSR_UND_IT_7_2_MASK 0xfc00U
#define AARCH32_SPSR_UND_IT_7_2_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3fU )

#define AARCH32_SPSR_UND_GE( _val ) ( ( _val ) << 16 )
#define AARCH32_SPSR_UND_GE_SHIFT 16
#define AARCH32_SPSR_UND_GE_MASK 0xf0000U
#define AARCH32_SPSR_UND_GE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_SPSR_UND_IL 0x100000U

#define AARCH32_SPSR_UND_DIT 0x200000U

#define AARCH32_SPSR_UND_PAN 0x400000U

#define AARCH32_SPSR_UND_SSBS 0x800000U

#define AARCH32_SPSR_UND_J 0x1000000U

#define AARCH32_SPSR_UND_IT_1_0( _val ) ( ( _val ) << 25 )
#define AARCH32_SPSR_UND_IT_1_0_SHIFT 25
#define AARCH32_SPSR_UND_IT_1_0_MASK 0x6000000U
#define AARCH32_SPSR_UND_IT_1_0_GET( _reg ) \
  ( ( ( _reg ) >> 25 ) & 0x3U )

#define AARCH32_SPSR_UND_Q 0x8000000U

#define AARCH32_SPSR_UND_V 0x10000000U

#define AARCH32_SPSR_UND_C 0x20000000U

#define AARCH32_SPSR_UND_Z 0x40000000U

#define AARCH32_SPSR_UND_N 0x80000000U

/* TCMTR, TCM Type Register */

static inline uint32_t _AArch32_Read_tcmtr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c0, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* TLBIALL, TLB Invalidate All */

static inline void _AArch32_Write_tlbiall( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c8, c7, 0" : : "r" ( value ) : "memory"
  );
}

/* TLBIALLH, TLB Invalidate All, Hyp mode */

static inline void _AArch32_Write_tlbiallh( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c8, c7, 0" : : "r" ( value ) : "memory"
  );
}

/* TLBIALLHIS, TLB Invalidate All, Hyp mode, Inner Shareable */

static inline void _AArch32_Write_tlbiallhis( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c8, c3, 0" : : "r" ( value ) : "memory"
  );
}

/* TLBIALLIS, TLB Invalidate All, Inner Shareable */

static inline void _AArch32_Write_tlbiallis( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c8, c3, 0" : : "r" ( value ) : "memory"
  );
}

/* TLBIALLNSNH, TLB Invalidate All, Non-Secure Non-Hyp */

static inline void _AArch32_Write_tlbiallnsnh( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c8, c7, 4" : : "r" ( value ) : "memory"
  );
}

/* TLBIALLNSNHIS, TLB Invalidate All, Non-Secure Non-Hyp, Inner Shareable */

static inline void _AArch32_Write_tlbiallnsnhis( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c8, c3, 4" : : "r" ( value ) : "memory"
  );
}

/* TLBIASID, TLB Invalidate by ASID match */

#define AARCH32_TLBIASID_ASID( _val ) ( ( _val ) << 0 )
#define AARCH32_TLBIASID_ASID_SHIFT 0
#define AARCH32_TLBIASID_ASID_MASK 0xffU
#define AARCH32_TLBIASID_ASID_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

static inline void _AArch32_Write_tlbiasid( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c8, c7, 2" : : "r" ( value ) : "memory"
  );
}

/* TLBIASIDIS, TLB Invalidate by ASID match, Inner Shareable */

#define AARCH32_TLBIASIDIS_ASID( _val ) ( ( _val ) << 0 )
#define AARCH32_TLBIASIDIS_ASID_SHIFT 0
#define AARCH32_TLBIASIDIS_ASID_MASK 0xffU
#define AARCH32_TLBIASIDIS_ASID_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

static inline void _AArch32_Write_tlbiasidis( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c8, c3, 2" : : "r" ( value ) : "memory"
  );
}

/* TLBIIPAS2, TLB Invalidate by Intermediate Physical Address, Stage 2 */

#define AARCH32_TLBIIPAS2_IPA_39_12( _val ) ( ( _val ) << 0 )
#define AARCH32_TLBIIPAS2_IPA_39_12_SHIFT 0
#define AARCH32_TLBIIPAS2_IPA_39_12_MASK 0xfffffffU
#define AARCH32_TLBIIPAS2_IPA_39_12_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfffffffU )

static inline void _AArch32_Write_tlbiipas2( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c8, c4, 1" : : "r" ( value ) : "memory"
  );
}

/* TLBIIPAS2IS, TLB Invalidate by Intermediate Physical Address, Stage 2, Inner Shareable */

#define AARCH32_TLBIIPAS2IS_IPA_39_12( _val ) ( ( _val ) << 0 )
#define AARCH32_TLBIIPAS2IS_IPA_39_12_SHIFT 0
#define AARCH32_TLBIIPAS2IS_IPA_39_12_MASK 0xfffffffU
#define AARCH32_TLBIIPAS2IS_IPA_39_12_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfffffffU )

static inline void _AArch32_Write_tlbiipas2is( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c8, c0, 1" : : "r" ( value ) : "memory"
  );
}

/* TLBIIPAS2L, TLB Invalidate by Intermediate Physical Address, Stage 2, Last level */

#define AARCH32_TLBIIPAS2L_IPA_39_12( _val ) ( ( _val ) << 0 )
#define AARCH32_TLBIIPAS2L_IPA_39_12_SHIFT 0
#define AARCH32_TLBIIPAS2L_IPA_39_12_MASK 0xfffffffU
#define AARCH32_TLBIIPAS2L_IPA_39_12_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfffffffU )

static inline void _AArch32_Write_tlbiipas2l( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c8, c4, 5" : : "r" ( value ) : "memory"
  );
}

/* TLBIIPAS2LIS, TLB Invalidate by Intermediate Physical Address, Stage 2, Last level, Inner */

#define AARCH32_TLBIIPAS2LIS_IPA_39_12( _val ) ( ( _val ) << 0 )
#define AARCH32_TLBIIPAS2LIS_IPA_39_12_SHIFT 0
#define AARCH32_TLBIIPAS2LIS_IPA_39_12_MASK 0xfffffffU
#define AARCH32_TLBIIPAS2LIS_IPA_39_12_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfffffffU )

static inline void _AArch32_Write_tlbiipas2lis( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c8, c0, 5" : : "r" ( value ) : "memory"
  );
}

/* TLBIMVA, TLB Invalidate by VA */

#define AARCH32_TLBIMVA_ASID( _val ) ( ( _val ) << 0 )
#define AARCH32_TLBIMVA_ASID_SHIFT 0
#define AARCH32_TLBIMVA_ASID_MASK 0xffU
#define AARCH32_TLBIMVA_ASID_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

#define AARCH32_TLBIMVA_VA( _val ) ( ( _val ) << 12 )
#define AARCH32_TLBIMVA_VA_SHIFT 12
#define AARCH32_TLBIMVA_VA_MASK 0xfffff000U
#define AARCH32_TLBIMVA_VA_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfffffU )

static inline void _AArch32_Write_tlbimva( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c8, c7, 1" : : "r" ( value ) : "memory"
  );
}

/* TLBIMVAA, TLB Invalidate by VA, All ASID */

#define AARCH32_TLBIMVAA_VA( _val ) ( ( _val ) << 12 )
#define AARCH32_TLBIMVAA_VA_SHIFT 12
#define AARCH32_TLBIMVAA_VA_MASK 0xfffff000U
#define AARCH32_TLBIMVAA_VA_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfffffU )

static inline void _AArch32_Write_tlbimvaa( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c8, c7, 3" : : "r" ( value ) : "memory"
  );
}

/* TLBIMVAAIS, TLB Invalidate by VA, All ASID, Inner Shareable */

#define AARCH32_TLBIMVAAIS_VA( _val ) ( ( _val ) << 12 )
#define AARCH32_TLBIMVAAIS_VA_SHIFT 12
#define AARCH32_TLBIMVAAIS_VA_MASK 0xfffff000U
#define AARCH32_TLBIMVAAIS_VA_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfffffU )

static inline void _AArch32_Write_tlbimvaais( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c8, c3, 3" : : "r" ( value ) : "memory"
  );
}

/* TLBIMVAAL, TLB Invalidate by VA, All ASID, Last level */

#define AARCH32_TLBIMVAAL_VA( _val ) ( ( _val ) << 12 )
#define AARCH32_TLBIMVAAL_VA_SHIFT 12
#define AARCH32_TLBIMVAAL_VA_MASK 0xfffff000U
#define AARCH32_TLBIMVAAL_VA_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfffffU )

static inline void _AArch32_Write_tlbimvaal( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c8, c7, 7" : : "r" ( value ) : "memory"
  );
}

/* TLBIMVAALIS, TLB Invalidate by VA, All ASID, Last level, Inner Shareable */

#define AARCH32_TLBIMVAALIS_VA( _val ) ( ( _val ) << 12 )
#define AARCH32_TLBIMVAALIS_VA_SHIFT 12
#define AARCH32_TLBIMVAALIS_VA_MASK 0xfffff000U
#define AARCH32_TLBIMVAALIS_VA_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfffffU )

static inline void _AArch32_Write_tlbimvaalis( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c8, c3, 7" : : "r" ( value ) : "memory"
  );
}

/* TLBIMVAH, TLB Invalidate by VA, Hyp mode */

#define AARCH32_TLBIMVAH_VA( _val ) ( ( _val ) << 12 )
#define AARCH32_TLBIMVAH_VA_SHIFT 12
#define AARCH32_TLBIMVAH_VA_MASK 0xfffff000U
#define AARCH32_TLBIMVAH_VA_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfffffU )

static inline void _AArch32_Write_tlbimvah( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c8, c7, 1" : : "r" ( value ) : "memory"
  );
}

/* TLBIMVAHIS, TLB Invalidate by VA, Hyp mode, Inner Shareable */

#define AARCH32_TLBIMVAHIS_VA( _val ) ( ( _val ) << 12 )
#define AARCH32_TLBIMVAHIS_VA_SHIFT 12
#define AARCH32_TLBIMVAHIS_VA_MASK 0xfffff000U
#define AARCH32_TLBIMVAHIS_VA_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfffffU )

static inline void _AArch32_Write_tlbimvahis( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c8, c3, 1" : : "r" ( value ) : "memory"
  );
}

/* TLBIMVAIS, TLB Invalidate by VA, Inner Shareable */

#define AARCH32_TLBIMVAIS_ASID( _val ) ( ( _val ) << 0 )
#define AARCH32_TLBIMVAIS_ASID_SHIFT 0
#define AARCH32_TLBIMVAIS_ASID_MASK 0xffU
#define AARCH32_TLBIMVAIS_ASID_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

#define AARCH32_TLBIMVAIS_VA( _val ) ( ( _val ) << 12 )
#define AARCH32_TLBIMVAIS_VA_SHIFT 12
#define AARCH32_TLBIMVAIS_VA_MASK 0xfffff000U
#define AARCH32_TLBIMVAIS_VA_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfffffU )

static inline void _AArch32_Write_tlbimvais( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c8, c3, 1" : : "r" ( value ) : "memory"
  );
}

/* TLBIMVAL, TLB Invalidate by VA, Last level */

#define AARCH32_TLBIMVAL_ASID( _val ) ( ( _val ) << 0 )
#define AARCH32_TLBIMVAL_ASID_SHIFT 0
#define AARCH32_TLBIMVAL_ASID_MASK 0xffU
#define AARCH32_TLBIMVAL_ASID_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

#define AARCH32_TLBIMVAL_VA( _val ) ( ( _val ) << 12 )
#define AARCH32_TLBIMVAL_VA_SHIFT 12
#define AARCH32_TLBIMVAL_VA_MASK 0xfffff000U
#define AARCH32_TLBIMVAL_VA_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfffffU )

static inline void _AArch32_Write_tlbimval( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c8, c7, 5" : : "r" ( value ) : "memory"
  );
}

/* TLBIMVALH, TLB Invalidate by VA, Last level, Hyp mode */

#define AARCH32_TLBIMVALH_VA( _val ) ( ( _val ) << 12 )
#define AARCH32_TLBIMVALH_VA_SHIFT 12
#define AARCH32_TLBIMVALH_VA_MASK 0xfffff000U
#define AARCH32_TLBIMVALH_VA_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfffffU )

static inline void _AArch32_Write_tlbimvalh( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c8, c7, 5" : : "r" ( value ) : "memory"
  );
}

/* TLBIMVALHIS, TLB Invalidate by VA, Last level, Hyp mode, Inner Shareable */

#define AARCH32_TLBIMVALHIS_VA( _val ) ( ( _val ) << 12 )
#define AARCH32_TLBIMVALHIS_VA_SHIFT 12
#define AARCH32_TLBIMVALHIS_VA_MASK 0xfffff000U
#define AARCH32_TLBIMVALHIS_VA_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfffffU )

static inline void _AArch32_Write_tlbimvalhis( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c8, c3, 5" : : "r" ( value ) : "memory"
  );
}

/* TLBIMVALIS, TLB Invalidate by VA, Last level, Inner Shareable */

#define AARCH32_TLBIMVALIS_ASID( _val ) ( ( _val ) << 0 )
#define AARCH32_TLBIMVALIS_ASID_SHIFT 0
#define AARCH32_TLBIMVALIS_ASID_MASK 0xffU
#define AARCH32_TLBIMVALIS_ASID_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

#define AARCH32_TLBIMVALIS_VA( _val ) ( ( _val ) << 12 )
#define AARCH32_TLBIMVALIS_VA_SHIFT 12
#define AARCH32_TLBIMVALIS_VA_MASK 0xfffff000U
#define AARCH32_TLBIMVALIS_VA_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfffffU )

static inline void _AArch32_Write_tlbimvalis( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c8, c3, 5" : : "r" ( value ) : "memory"
  );
}

/* TLBTR, TLB Type Register */

#define AARCH32_TLBTR_NU 0x1U

static inline uint32_t _AArch32_Read_tlbtr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c0, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* TPIDRPRW, PL1 Software Thread ID Register */

static inline uint32_t _AArch32_Read_tpidrprw( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c0, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_tpidrprw( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c0, 4" : : "r" ( value ) : "memory"
  );
}

/* TPIDRURO, PL0 Read-Only Software Thread ID Register */

static inline uint32_t _AArch32_Read_tpidruro( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c0, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_tpidruro( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c0, 3" : : "r" ( value ) : "memory"
  );
}

/* TPIDRURW, PL0 Read/Write Software Thread ID Register */

static inline uint32_t _AArch32_Read_tpidrurw( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c0, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_tpidrurw( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c0, 2" : : "r" ( value ) : "memory"
  );
}

/* TTBCR, Translation Table Base Control Register */

#define AARCH32_TTBCR_N( _val ) ( ( _val ) << 0 )
#define AARCH32_TTBCR_N_SHIFT 0
#define AARCH32_TTBCR_N_MASK 0x7U
#define AARCH32_TTBCR_N_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x7U )

#define AARCH32_TTBCR_T0SZ( _val ) ( ( _val ) << 0 )
#define AARCH32_TTBCR_T0SZ_SHIFT 0
#define AARCH32_TTBCR_T0SZ_MASK 0x7U
#define AARCH32_TTBCR_T0SZ_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x7U )

#define AARCH32_TTBCR_PD0 0x10U

#define AARCH32_TTBCR_PD1 0x20U

#define AARCH32_TTBCR_T2E 0x40U

#define AARCH32_TTBCR_EPD0 0x80U

#define AARCH32_TTBCR_IRGN0( _val ) ( ( _val ) << 8 )
#define AARCH32_TTBCR_IRGN0_SHIFT 8
#define AARCH32_TTBCR_IRGN0_MASK 0x300U
#define AARCH32_TTBCR_IRGN0_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0x3U )

#define AARCH32_TTBCR_ORGN0( _val ) ( ( _val ) << 10 )
#define AARCH32_TTBCR_ORGN0_SHIFT 10
#define AARCH32_TTBCR_ORGN0_MASK 0xc00U
#define AARCH32_TTBCR_ORGN0_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3U )

#define AARCH32_TTBCR_SH0( _val ) ( ( _val ) << 12 )
#define AARCH32_TTBCR_SH0_SHIFT 12
#define AARCH32_TTBCR_SH0_MASK 0x3000U
#define AARCH32_TTBCR_SH0_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0x3U )

#define AARCH32_TTBCR_T1SZ( _val ) ( ( _val ) << 16 )
#define AARCH32_TTBCR_T1SZ_SHIFT 16
#define AARCH32_TTBCR_T1SZ_MASK 0x70000U
#define AARCH32_TTBCR_T1SZ_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0x7U )

#define AARCH32_TTBCR_A1 0x400000U

#define AARCH32_TTBCR_EPD1 0x800000U

#define AARCH32_TTBCR_IRGN1( _val ) ( ( _val ) << 24 )
#define AARCH32_TTBCR_IRGN1_SHIFT 24
#define AARCH32_TTBCR_IRGN1_MASK 0x3000000U
#define AARCH32_TTBCR_IRGN1_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0x3U )

#define AARCH32_TTBCR_ORGN1( _val ) ( ( _val ) << 26 )
#define AARCH32_TTBCR_ORGN1_SHIFT 26
#define AARCH32_TTBCR_ORGN1_MASK 0xc000000U
#define AARCH32_TTBCR_ORGN1_GET( _reg ) \
  ( ( ( _reg ) >> 26 ) & 0x3U )

#define AARCH32_TTBCR_SH1( _val ) ( ( _val ) << 28 )
#define AARCH32_TTBCR_SH1_SHIFT 28
#define AARCH32_TTBCR_SH1_MASK 0x30000000U
#define AARCH32_TTBCR_SH1_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0x3U )

#define AARCH32_TTBCR_EAE 0x80000000U

static inline uint32_t _AArch32_Read_ttbcr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c2, c0, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_ttbcr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c2, c0, 2" : : "r" ( value ) : "memory"
  );
}

/* TTBCR2, Translation Table Base Control Register 2 */

#define AARCH32_TTBCR2_HPD0 0x200U

#define AARCH32_TTBCR2_HPD1 0x400U

#define AARCH32_TTBCR2_HWU059 0x800U

#define AARCH32_TTBCR2_HWU060 0x1000U

#define AARCH32_TTBCR2_HWU061 0x2000U

#define AARCH32_TTBCR2_HWU062 0x4000U

#define AARCH32_TTBCR2_HWU159 0x8000U

#define AARCH32_TTBCR2_HWU160 0x10000U

#define AARCH32_TTBCR2_HWU161 0x20000U

#define AARCH32_TTBCR2_HWU162 0x40000U

static inline uint32_t _AArch32_Read_ttbcr2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c2, c0, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_ttbcr2( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c2, c0, 3" : : "r" ( value ) : "memory"
  );
}

/* TTBR0, Translation Table Base Register 0 */

#define AARCH32_TTBR0_CNP 0x1U

#define AARCH32_TTBR0_IRGN_1 0x1U

#define AARCH32_TTBR0_S 0x2U

#define AARCH32_TTBR0_BADDR( _val ) ( ( _val ) << 1 )
#define AARCH32_TTBR0_BADDR_SHIFT 1
#define AARCH32_TTBR0_BADDR_MASK 0xfffffffffffeULL
#define AARCH32_TTBR0_BADDR_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x7fffffffffffULL )

#define AARCH32_TTBR0_IMP 0x4U

#define AARCH32_TTBR0_RGN( _val ) ( ( _val ) << 3 )
#define AARCH32_TTBR0_RGN_SHIFT 3
#define AARCH32_TTBR0_RGN_MASK 0x18U
#define AARCH32_TTBR0_RGN_GET( _reg ) \
  ( ( ( _reg ) >> 3 ) & 0x3U )

#define AARCH32_TTBR0_NOS 0x20U

#define AARCH32_TTBR0_IRGN_0 0x40U

#define AARCH32_TTBR0_TTB0( _val ) ( ( _val ) << 7 )
#define AARCH32_TTBR0_TTB0_SHIFT 7
#define AARCH32_TTBR0_TTB0_MASK 0xffffff80U
#define AARCH32_TTBR0_TTB0_GET( _reg ) \
  ( ( ( _reg ) >> 7 ) & 0x1ffffffU )

#define AARCH32_TTBR0_ASID( _val ) ( ( _val ) << 48 )
#define AARCH32_TTBR0_ASID_SHIFT 48
#define AARCH32_TTBR0_ASID_MASK 0xff000000000000ULL
#define AARCH32_TTBR0_ASID_GET( _reg ) \
  ( ( ( _reg ) >> 48 ) & 0xffULL )

static inline uint32_t _AArch32_Read_32_ttbr0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c2, c0, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_32_ttbr0( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c2, c0, 0" : : "r" ( value ) : "memory"
  );
}

/* TTBR0, Translation Table Base Register 0 */

static inline uint64_t _AArch32_Read_64_ttbr0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 0, %Q0, %R0, c2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_64_ttbr0( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 0, %Q0, %R0, c2" : : "r" ( value ) : "memory"
  );
}

/* TTBR1, Translation Table Base Register 1 */

#define AARCH32_TTBR1_CNP 0x1U

#define AARCH32_TTBR1_IRGN_0 0x1U

#define AARCH32_TTBR1_S 0x2U

#define AARCH32_TTBR1_BADDR( _val ) ( ( _val ) << 1 )
#define AARCH32_TTBR1_BADDR_SHIFT 1
#define AARCH32_TTBR1_BADDR_MASK 0xfffffffffffeULL
#define AARCH32_TTBR1_BADDR_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x7fffffffffffULL )

#define AARCH32_TTBR1_IMP 0x4U

#define AARCH32_TTBR1_RGN( _val ) ( ( _val ) << 3 )
#define AARCH32_TTBR1_RGN_SHIFT 3
#define AARCH32_TTBR1_RGN_MASK 0x18U
#define AARCH32_TTBR1_RGN_GET( _reg ) \
  ( ( ( _reg ) >> 3 ) & 0x3U )

#define AARCH32_TTBR1_NOS 0x20U

#define AARCH32_TTBR1_IRGN_1 0x40U

#define AARCH32_TTBR1_TTB1( _val ) ( ( _val ) << 7 )
#define AARCH32_TTBR1_TTB1_SHIFT 7
#define AARCH32_TTBR1_TTB1_MASK 0xffffff80U
#define AARCH32_TTBR1_TTB1_GET( _reg ) \
  ( ( ( _reg ) >> 7 ) & 0x1ffffffU )

#define AARCH32_TTBR1_ASID( _val ) ( ( _val ) << 48 )
#define AARCH32_TTBR1_ASID_SHIFT 48
#define AARCH32_TTBR1_ASID_MASK 0xff000000000000ULL
#define AARCH32_TTBR1_ASID_GET( _reg ) \
  ( ( ( _reg ) >> 48 ) & 0xffULL )

static inline uint32_t _AArch32_Read_32_ttbr1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c2, c0, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_32_ttbr1( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c2, c0, 1" : : "r" ( value ) : "memory"
  );
}

/* TTBR1, Translation Table Base Register 1 */

static inline uint64_t _AArch32_Read_64_ttbr1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 1, %Q0, %R0, c2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_64_ttbr1( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 1, %Q0, %R0, c2" : : "r" ( value ) : "memory"
  );
}

/* VBAR, Vector Base Address Register */

static inline uint32_t _AArch32_Read_vbar( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c12, c0, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_vbar( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c12, c0, 0" : : "r" ( value ) : "memory"
  );
}

/* VMPIDR, Virtualization Multiprocessor ID Register */

#define AARCH32_VMPIDR_AFF0( _val ) ( ( _val ) << 0 )
#define AARCH32_VMPIDR_AFF0_SHIFT 0
#define AARCH32_VMPIDR_AFF0_MASK 0xffU
#define AARCH32_VMPIDR_AFF0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

#define AARCH32_VMPIDR_AFF1( _val ) ( ( _val ) << 8 )
#define AARCH32_VMPIDR_AFF1_SHIFT 8
#define AARCH32_VMPIDR_AFF1_MASK 0xff00U
#define AARCH32_VMPIDR_AFF1_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xffU )

#define AARCH32_VMPIDR_AFF2( _val ) ( ( _val ) << 16 )
#define AARCH32_VMPIDR_AFF2_SHIFT 16
#define AARCH32_VMPIDR_AFF2_MASK 0xff0000U
#define AARCH32_VMPIDR_AFF2_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xffU )

#define AARCH32_VMPIDR_MT 0x1000000U

#define AARCH32_VMPIDR_U 0x40000000U

#define AARCH32_VMPIDR_M 0x80000000U

static inline uint32_t _AArch32_Read_vmpidr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c0, c0, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_vmpidr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c0, c0, 5" : : "r" ( value ) : "memory"
  );
}

/* VPIDR, Virtualization Processor ID Register */

#define AARCH32_VPIDR_REVISION( _val ) ( ( _val ) << 0 )
#define AARCH32_VPIDR_REVISION_SHIFT 0
#define AARCH32_VPIDR_REVISION_MASK 0xfU
#define AARCH32_VPIDR_REVISION_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_VPIDR_PARTNUM( _val ) ( ( _val ) << 4 )
#define AARCH32_VPIDR_PARTNUM_SHIFT 4
#define AARCH32_VPIDR_PARTNUM_MASK 0xfff0U
#define AARCH32_VPIDR_PARTNUM_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfffU )

#define AARCH32_VPIDR_ARCHITECTURE( _val ) ( ( _val ) << 16 )
#define AARCH32_VPIDR_ARCHITECTURE_SHIFT 16
#define AARCH32_VPIDR_ARCHITECTURE_MASK 0xf0000U
#define AARCH32_VPIDR_ARCHITECTURE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_VPIDR_VARIANT( _val ) ( ( _val ) << 20 )
#define AARCH32_VPIDR_VARIANT_SHIFT 20
#define AARCH32_VPIDR_VARIANT_MASK 0xf00000U
#define AARCH32_VPIDR_VARIANT_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH32_VPIDR_IMPLEMENTER( _val ) ( ( _val ) << 24 )
#define AARCH32_VPIDR_IMPLEMENTER_SHIFT 24
#define AARCH32_VPIDR_IMPLEMENTER_MASK 0xff000000U
#define AARCH32_VPIDR_IMPLEMENTER_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xffU )

static inline uint32_t _AArch32_Read_vpidr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c0, c0, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_vpidr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c0, c0, 0" : : "r" ( value ) : "memory"
  );
}

/* VTCR, Virtualization Translation Control Register */

#define AARCH32_VTCR_T0SZ( _val ) ( ( _val ) << 0 )
#define AARCH32_VTCR_T0SZ_SHIFT 0
#define AARCH32_VTCR_T0SZ_MASK 0xfU
#define AARCH32_VTCR_T0SZ_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_VTCR_S 0x10U

#define AARCH32_VTCR_SL0( _val ) ( ( _val ) << 6 )
#define AARCH32_VTCR_SL0_SHIFT 6
#define AARCH32_VTCR_SL0_MASK 0xc0U
#define AARCH32_VTCR_SL0_GET( _reg ) \
  ( ( ( _reg ) >> 6 ) & 0x3U )

#define AARCH32_VTCR_IRGN0( _val ) ( ( _val ) << 8 )
#define AARCH32_VTCR_IRGN0_SHIFT 8
#define AARCH32_VTCR_IRGN0_MASK 0x300U
#define AARCH32_VTCR_IRGN0_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0x3U )

#define AARCH32_VTCR_ORGN0( _val ) ( ( _val ) << 10 )
#define AARCH32_VTCR_ORGN0_SHIFT 10
#define AARCH32_VTCR_ORGN0_MASK 0xc00U
#define AARCH32_VTCR_ORGN0_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3U )

#define AARCH32_VTCR_SH0( _val ) ( ( _val ) << 12 )
#define AARCH32_VTCR_SH0_SHIFT 12
#define AARCH32_VTCR_SH0_MASK 0x3000U
#define AARCH32_VTCR_SH0_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0x3U )

#define AARCH32_VTCR_HWU59 0x2000000U

#define AARCH32_VTCR_HWU60 0x4000000U

#define AARCH32_VTCR_HWU61 0x8000000U

#define AARCH32_VTCR_HWU62 0x10000000U

static inline uint32_t _AArch32_Read_vtcr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c2, c1, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_vtcr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c2, c1, 2" : : "r" ( value ) : "memory"
  );
}

/* VTTBR, Virtualization Translation Table Base Register */

#define AARCH32_VTTBR_CNP 0x1U

#define AARCH32_VTTBR_BADDR( _val ) ( ( _val ) << 1 )
#define AARCH32_VTTBR_BADDR_SHIFT 1
#define AARCH32_VTTBR_BADDR_MASK 0xfffffffffffeULL
#define AARCH32_VTTBR_BADDR_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x7fffffffffffULL )

#define AARCH32_VTTBR_VMID( _val ) ( ( _val ) << 48 )
#define AARCH32_VTTBR_VMID_SHIFT 48
#define AARCH32_VTTBR_VMID_MASK 0xff000000000000ULL
#define AARCH32_VTTBR_VMID_GET( _reg ) \
  ( ( ( _reg ) >> 48 ) & 0xffULL )

static inline uint64_t _AArch32_Read_vttbr( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 6, %Q0, %R0, c2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_vttbr( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 6, %Q0, %R0, c2" : : "r" ( value ) : "memory"
  );
}

/* DBGAUTHSTATUS, Debug Authentication Status Register */

#define AARCH32_DBGAUTHSTATUS_NSID( _val ) ( ( _val ) << 0 )
#define AARCH32_DBGAUTHSTATUS_NSID_SHIFT 0
#define AARCH32_DBGAUTHSTATUS_NSID_MASK 0x3U
#define AARCH32_DBGAUTHSTATUS_NSID_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3U )

#define AARCH32_DBGAUTHSTATUS_NSNID( _val ) ( ( _val ) << 2 )
#define AARCH32_DBGAUTHSTATUS_NSNID_SHIFT 2
#define AARCH32_DBGAUTHSTATUS_NSNID_MASK 0xcU
#define AARCH32_DBGAUTHSTATUS_NSNID_GET( _reg ) \
  ( ( ( _reg ) >> 2 ) & 0x3U )

#define AARCH32_DBGAUTHSTATUS_SID( _val ) ( ( _val ) << 4 )
#define AARCH32_DBGAUTHSTATUS_SID_SHIFT 4
#define AARCH32_DBGAUTHSTATUS_SID_MASK 0x30U
#define AARCH32_DBGAUTHSTATUS_SID_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0x3U )

#define AARCH32_DBGAUTHSTATUS_SNID( _val ) ( ( _val ) << 6 )
#define AARCH32_DBGAUTHSTATUS_SNID_SHIFT 6
#define AARCH32_DBGAUTHSTATUS_SNID_MASK 0xc0U
#define AARCH32_DBGAUTHSTATUS_SNID_GET( _reg ) \
  ( ( ( _reg ) >> 6 ) & 0x3U )

static inline uint32_t _AArch32_Read_dbgauthstatus( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c7, c14, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* DBGBCR, Debug Breakpoint Control Registers */

#define AARCH32_DBGBCR_E 0x1U

#define AARCH32_DBGBCR_PMC( _val ) ( ( _val ) << 1 )
#define AARCH32_DBGBCR_PMC_SHIFT 1
#define AARCH32_DBGBCR_PMC_MASK 0x6U
#define AARCH32_DBGBCR_PMC_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x3U )

#define AARCH32_DBGBCR_BAS( _val ) ( ( _val ) << 5 )
#define AARCH32_DBGBCR_BAS_SHIFT 5
#define AARCH32_DBGBCR_BAS_MASK 0x1e0U
#define AARCH32_DBGBCR_BAS_GET( _reg ) \
  ( ( ( _reg ) >> 5 ) & 0xfU )

#define AARCH32_DBGBCR_HMC 0x2000U

#define AARCH32_DBGBCR_SSC( _val ) ( ( _val ) << 14 )
#define AARCH32_DBGBCR_SSC_SHIFT 14
#define AARCH32_DBGBCR_SSC_MASK 0xc000U
#define AARCH32_DBGBCR_SSC_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x3U )

#define AARCH32_DBGBCR_LBN( _val ) ( ( _val ) << 16 )
#define AARCH32_DBGBCR_LBN_SHIFT 16
#define AARCH32_DBGBCR_LBN_MASK 0xf0000U
#define AARCH32_DBGBCR_LBN_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_DBGBCR_BT( _val ) ( ( _val ) << 20 )
#define AARCH32_DBGBCR_BT_SHIFT 20
#define AARCH32_DBGBCR_BT_MASK 0xf00000U
#define AARCH32_DBGBCR_BT_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

/* DBGBCR_0, Debug Breakpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgbcr_0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c0, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbcr_0( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c0, 5" : : "r" ( value ) : "memory"
  );
}

/* DBGBCR_1, Debug Breakpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgbcr_1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c1, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbcr_1( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c1, 5" : : "r" ( value ) : "memory"
  );
}

/* DBGBCR_2, Debug Breakpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgbcr_2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c2, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbcr_2( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c2, 5" : : "r" ( value ) : "memory"
  );
}

/* DBGBCR_3, Debug Breakpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgbcr_3( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c3, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbcr_3( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c3, 5" : : "r" ( value ) : "memory"
  );
}

/* DBGBCR_4, Debug Breakpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgbcr_4( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c4, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbcr_4( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c4, 5" : : "r" ( value ) : "memory"
  );
}

/* DBGBCR_5, Debug Breakpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgbcr_5( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c5, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbcr_5( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c5, 5" : : "r" ( value ) : "memory"
  );
}

/* DBGBCR_6, Debug Breakpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgbcr_6( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c6, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbcr_6( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c6, 5" : : "r" ( value ) : "memory"
  );
}

/* DBGBCR_7, Debug Breakpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgbcr_7( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c7, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbcr_7( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c7, 5" : : "r" ( value ) : "memory"
  );
}

/* DBGBCR_8, Debug Breakpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgbcr_8( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c8, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbcr_8( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c8, 5" : : "r" ( value ) : "memory"
  );
}

/* DBGBCR_9, Debug Breakpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgbcr_9( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c9, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbcr_9( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c9, 5" : : "r" ( value ) : "memory"
  );
}

/* DBGBCR_10, Debug Breakpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgbcr_10( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c10, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbcr_10( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c10, 5" : : "r" ( value ) : "memory"
  );
}

/* DBGBCR_11, Debug Breakpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgbcr_11( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c11, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbcr_11( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c11, 5" : : "r" ( value ) : "memory"
  );
}

/* DBGBCR_12, Debug Breakpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgbcr_12( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c12, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbcr_12( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c12, 5" : : "r" ( value ) : "memory"
  );
}

/* DBGBCR_13, Debug Breakpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgbcr_13( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c13, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbcr_13( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c13, 5" : : "r" ( value ) : "memory"
  );
}

/* DBGBCR_14, Debug Breakpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgbcr_14( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c14, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbcr_14( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c14, 5" : : "r" ( value ) : "memory"
  );
}

/* DBGBCR_15, Debug Breakpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgbcr_15( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c15, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbcr_15( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c15, 5" : : "r" ( value ) : "memory"
  );
}

/* DBGBVR, Debug Breakpoint Value Registers */

#define AARCH32_DBGBVR_CONTEXTID( _val ) ( ( _val ) << 0 )
#define AARCH32_DBGBVR_CONTEXTID_SHIFT 0
#define AARCH32_DBGBVR_CONTEXTID_MASK 0xffffffffU
#define AARCH32_DBGBVR_CONTEXTID_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )

#define AARCH32_DBGBVR_VA_31_2( _val ) ( ( _val ) << 2 )
#define AARCH32_DBGBVR_VA_31_2_SHIFT 2
#define AARCH32_DBGBVR_VA_31_2_MASK 0xfffffffcU
#define AARCH32_DBGBVR_VA_31_2_GET( _reg ) \
  ( ( ( _reg ) >> 2 ) & 0x3fffffffU )

/* DBGBVR_0, Debug Breakpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgbvr_0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c0, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbvr_0( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c0, 4" : : "r" ( value ) : "memory"
  );
}

/* DBGBVR_1, Debug Breakpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgbvr_1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c1, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbvr_1( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c1, 4" : : "r" ( value ) : "memory"
  );
}

/* DBGBVR_2, Debug Breakpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgbvr_2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c2, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbvr_2( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c2, 4" : : "r" ( value ) : "memory"
  );
}

/* DBGBVR_3, Debug Breakpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgbvr_3( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c3, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbvr_3( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c3, 4" : : "r" ( value ) : "memory"
  );
}

/* DBGBVR_4, Debug Breakpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgbvr_4( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c4, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbvr_4( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c4, 4" : : "r" ( value ) : "memory"
  );
}

/* DBGBVR_5, Debug Breakpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgbvr_5( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c5, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbvr_5( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c5, 4" : : "r" ( value ) : "memory"
  );
}

/* DBGBVR_6, Debug Breakpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgbvr_6( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c6, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbvr_6( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c6, 4" : : "r" ( value ) : "memory"
  );
}

/* DBGBVR_7, Debug Breakpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgbvr_7( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c7, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbvr_7( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c7, 4" : : "r" ( value ) : "memory"
  );
}

/* DBGBVR_8, Debug Breakpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgbvr_8( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c8, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbvr_8( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c8, 4" : : "r" ( value ) : "memory"
  );
}

/* DBGBVR_9, Debug Breakpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgbvr_9( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c9, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbvr_9( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c9, 4" : : "r" ( value ) : "memory"
  );
}

/* DBGBVR_10, Debug Breakpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgbvr_10( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c10, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbvr_10( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c10, 4" : : "r" ( value ) : "memory"
  );
}

/* DBGBVR_11, Debug Breakpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgbvr_11( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c11, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbvr_11( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c11, 4" : : "r" ( value ) : "memory"
  );
}

/* DBGBVR_12, Debug Breakpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgbvr_12( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c12, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbvr_12( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c12, 4" : : "r" ( value ) : "memory"
  );
}

/* DBGBVR_13, Debug Breakpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgbvr_13( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c13, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbvr_13( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c13, 4" : : "r" ( value ) : "memory"
  );
}

/* DBGBVR_14, Debug Breakpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgbvr_14( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c14, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbvr_14( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c14, 4" : : "r" ( value ) : "memory"
  );
}

/* DBGBVR_15, Debug Breakpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgbvr_15( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c15, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbvr_15( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c15, 4" : : "r" ( value ) : "memory"
  );
}

/* DBGBXVR, Debug Breakpoint Extended Value Registers */

#define AARCH32_DBGBXVR_VMID_7_0( _val ) ( ( _val ) << 0 )
#define AARCH32_DBGBXVR_VMID_7_0_SHIFT 0
#define AARCH32_DBGBXVR_VMID_7_0_MASK 0xffU
#define AARCH32_DBGBXVR_VMID_7_0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

#define AARCH32_DBGBXVR_CONTEXTID2( _val ) ( ( _val ) << 0 )
#define AARCH32_DBGBXVR_CONTEXTID2_SHIFT 0
#define AARCH32_DBGBXVR_CONTEXTID2_MASK 0xffffffffU
#define AARCH32_DBGBXVR_CONTEXTID2_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )

#define AARCH32_DBGBXVR_VMID_15_8( _val ) ( ( _val ) << 8 )
#define AARCH32_DBGBXVR_VMID_15_8_SHIFT 8
#define AARCH32_DBGBXVR_VMID_15_8_MASK 0xff00U
#define AARCH32_DBGBXVR_VMID_15_8_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xffU )

/* DBGBXVR_0, Debug Breakpoint Extended Value Registers */

static inline uint32_t _AArch32_Read_dbgbxvr_0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c1, c0, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbxvr_0( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c1, c0, 1" : : "r" ( value ) : "memory"
  );
}

/* DBGBXVR_1, Debug Breakpoint Extended Value Registers */

static inline uint32_t _AArch32_Read_dbgbxvr_1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c1, c1, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbxvr_1( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c1, c1, 1" : : "r" ( value ) : "memory"
  );
}

/* DBGBXVR_2, Debug Breakpoint Extended Value Registers */

static inline uint32_t _AArch32_Read_dbgbxvr_2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c1, c2, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbxvr_2( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c1, c2, 1" : : "r" ( value ) : "memory"
  );
}

/* DBGBXVR_3, Debug Breakpoint Extended Value Registers */

static inline uint32_t _AArch32_Read_dbgbxvr_3( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c1, c3, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbxvr_3( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c1, c3, 1" : : "r" ( value ) : "memory"
  );
}

/* DBGBXVR_4, Debug Breakpoint Extended Value Registers */

static inline uint32_t _AArch32_Read_dbgbxvr_4( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c1, c4, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbxvr_4( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c1, c4, 1" : : "r" ( value ) : "memory"
  );
}

/* DBGBXVR_5, Debug Breakpoint Extended Value Registers */

static inline uint32_t _AArch32_Read_dbgbxvr_5( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c1, c5, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbxvr_5( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c1, c5, 1" : : "r" ( value ) : "memory"
  );
}

/* DBGBXVR_6, Debug Breakpoint Extended Value Registers */

static inline uint32_t _AArch32_Read_dbgbxvr_6( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c1, c6, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbxvr_6( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c1, c6, 1" : : "r" ( value ) : "memory"
  );
}

/* DBGBXVR_7, Debug Breakpoint Extended Value Registers */

static inline uint32_t _AArch32_Read_dbgbxvr_7( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c1, c7, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbxvr_7( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c1, c7, 1" : : "r" ( value ) : "memory"
  );
}

/* DBGBXVR_8, Debug Breakpoint Extended Value Registers */

static inline uint32_t _AArch32_Read_dbgbxvr_8( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c1, c8, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbxvr_8( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c1, c8, 1" : : "r" ( value ) : "memory"
  );
}

/* DBGBXVR_9, Debug Breakpoint Extended Value Registers */

static inline uint32_t _AArch32_Read_dbgbxvr_9( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c1, c9, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbxvr_9( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c1, c9, 1" : : "r" ( value ) : "memory"
  );
}

/* DBGBXVR_10, Debug Breakpoint Extended Value Registers */

static inline uint32_t _AArch32_Read_dbgbxvr_10( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c1, c10, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbxvr_10( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c1, c10, 1" : : "r" ( value ) : "memory"
  );
}

/* DBGBXVR_11, Debug Breakpoint Extended Value Registers */

static inline uint32_t _AArch32_Read_dbgbxvr_11( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c1, c11, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbxvr_11( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c1, c11, 1" : : "r" ( value ) : "memory"
  );
}

/* DBGBXVR_12, Debug Breakpoint Extended Value Registers */

static inline uint32_t _AArch32_Read_dbgbxvr_12( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c1, c12, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbxvr_12( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c1, c12, 1" : : "r" ( value ) : "memory"
  );
}

/* DBGBXVR_13, Debug Breakpoint Extended Value Registers */

static inline uint32_t _AArch32_Read_dbgbxvr_13( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c1, c13, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbxvr_13( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c1, c13, 1" : : "r" ( value ) : "memory"
  );
}

/* DBGBXVR_14, Debug Breakpoint Extended Value Registers */

static inline uint32_t _AArch32_Read_dbgbxvr_14( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c1, c14, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbxvr_14( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c1, c14, 1" : : "r" ( value ) : "memory"
  );
}

/* DBGBXVR_15, Debug Breakpoint Extended Value Registers */

static inline uint32_t _AArch32_Read_dbgbxvr_15( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c1, c15, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgbxvr_15( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c1, c15, 1" : : "r" ( value ) : "memory"
  );
}

/* DBGCLAIMCLR, Debug CLAIM Tag Clear Register */

#define AARCH32_DBGCLAIMCLR_CLAIM( _val ) ( ( _val ) << 0 )
#define AARCH32_DBGCLAIMCLR_CLAIM_SHIFT 0
#define AARCH32_DBGCLAIMCLR_CLAIM_MASK 0xffU
#define AARCH32_DBGCLAIMCLR_CLAIM_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

static inline uint32_t _AArch32_Read_dbgclaimclr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c7, c9, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgclaimclr( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c7, c9, 6" : : "r" ( value ) : "memory"
  );
}

/* DBGCLAIMSET, Debug CLAIM Tag Set Register */

#define AARCH32_DBGCLAIMSET_CLAIM( _val ) ( ( _val ) << 0 )
#define AARCH32_DBGCLAIMSET_CLAIM_SHIFT 0
#define AARCH32_DBGCLAIMSET_CLAIM_MASK 0xffU
#define AARCH32_DBGCLAIMSET_CLAIM_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

static inline uint32_t _AArch32_Read_dbgclaimset( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c7, c8, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgclaimset( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c7, c8, 6" : : "r" ( value ) : "memory"
  );
}

/* DBGDCCINT, DCC Interrupt Enable Register */

#define AARCH32_DBGDCCINT_TX 0x20000000U

#define AARCH32_DBGDCCINT_RX 0x40000000U

static inline uint32_t _AArch32_Read_dbgdccint( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c2, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgdccint( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c2, 0" : : "r" ( value ) : "memory"
  );
}

/* DBGDEVID, Debug Device ID Register 0 */

#define AARCH32_DBGDEVID_PCSAMPLE( _val ) ( ( _val ) << 0 )
#define AARCH32_DBGDEVID_PCSAMPLE_SHIFT 0
#define AARCH32_DBGDEVID_PCSAMPLE_MASK 0xfU
#define AARCH32_DBGDEVID_PCSAMPLE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_DBGDEVID_WPADDRMASK( _val ) ( ( _val ) << 4 )
#define AARCH32_DBGDEVID_WPADDRMASK_SHIFT 4
#define AARCH32_DBGDEVID_WPADDRMASK_MASK 0xf0U
#define AARCH32_DBGDEVID_WPADDRMASK_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH32_DBGDEVID_BPADDRMASK( _val ) ( ( _val ) << 8 )
#define AARCH32_DBGDEVID_BPADDRMASK_SHIFT 8
#define AARCH32_DBGDEVID_BPADDRMASK_MASK 0xf00U
#define AARCH32_DBGDEVID_BPADDRMASK_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH32_DBGDEVID_VECTORCATCH( _val ) ( ( _val ) << 12 )
#define AARCH32_DBGDEVID_VECTORCATCH_SHIFT 12
#define AARCH32_DBGDEVID_VECTORCATCH_MASK 0xf000U
#define AARCH32_DBGDEVID_VECTORCATCH_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH32_DBGDEVID_VIRTEXTNS( _val ) ( ( _val ) << 16 )
#define AARCH32_DBGDEVID_VIRTEXTNS_SHIFT 16
#define AARCH32_DBGDEVID_VIRTEXTNS_MASK 0xf0000U
#define AARCH32_DBGDEVID_VIRTEXTNS_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_DBGDEVID_DOUBLELOCK( _val ) ( ( _val ) << 20 )
#define AARCH32_DBGDEVID_DOUBLELOCK_SHIFT 20
#define AARCH32_DBGDEVID_DOUBLELOCK_MASK 0xf00000U
#define AARCH32_DBGDEVID_DOUBLELOCK_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH32_DBGDEVID_AUXREGS( _val ) ( ( _val ) << 24 )
#define AARCH32_DBGDEVID_AUXREGS_SHIFT 24
#define AARCH32_DBGDEVID_AUXREGS_MASK 0xf000000U
#define AARCH32_DBGDEVID_AUXREGS_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH32_DBGDEVID_CIDMASK( _val ) ( ( _val ) << 28 )
#define AARCH32_DBGDEVID_CIDMASK_SHIFT 28
#define AARCH32_DBGDEVID_CIDMASK_MASK 0xf0000000U
#define AARCH32_DBGDEVID_CIDMASK_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint32_t _AArch32_Read_dbgdevid( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c7, c2, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* DBGDEVID1, Debug Device ID Register 1 */

#define AARCH32_DBGDEVID1_PCSROFFSET( _val ) ( ( _val ) << 0 )
#define AARCH32_DBGDEVID1_PCSROFFSET_SHIFT 0
#define AARCH32_DBGDEVID1_PCSROFFSET_MASK 0xfU
#define AARCH32_DBGDEVID1_PCSROFFSET_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

static inline uint32_t _AArch32_Read_dbgdevid1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c7, c1, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* DBGDEVID2, Debug Device ID Register 2 */

static inline uint32_t _AArch32_Read_dbgdevid2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c7, c0, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* DBGDIDR, Debug ID Register */

#define AARCH32_DBGDIDR_SE_IMP 0x1000U

#define AARCH32_DBGDIDR_NSUHD_IMP 0x4000U

#define AARCH32_DBGDIDR_VERSION( _val ) ( ( _val ) << 16 )
#define AARCH32_DBGDIDR_VERSION_SHIFT 16
#define AARCH32_DBGDIDR_VERSION_MASK 0xf0000U
#define AARCH32_DBGDIDR_VERSION_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_DBGDIDR_CTX_CMPS( _val ) ( ( _val ) << 20 )
#define AARCH32_DBGDIDR_CTX_CMPS_SHIFT 20
#define AARCH32_DBGDIDR_CTX_CMPS_MASK 0xf00000U
#define AARCH32_DBGDIDR_CTX_CMPS_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH32_DBGDIDR_BRPS( _val ) ( ( _val ) << 24 )
#define AARCH32_DBGDIDR_BRPS_SHIFT 24
#define AARCH32_DBGDIDR_BRPS_MASK 0xf000000U
#define AARCH32_DBGDIDR_BRPS_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH32_DBGDIDR_WRPS( _val ) ( ( _val ) << 28 )
#define AARCH32_DBGDIDR_WRPS_SHIFT 28
#define AARCH32_DBGDIDR_WRPS_MASK 0xf0000000U
#define AARCH32_DBGDIDR_WRPS_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint32_t _AArch32_Read_dbgdidr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c0, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* DBGDRAR, Debug ROM Address Register */

#define AARCH32_DBGDRAR_VALID( _val ) ( ( _val ) << 0 )
#define AARCH32_DBGDRAR_VALID_SHIFT 0
#define AARCH32_DBGDRAR_VALID_MASK 0x3U
#define AARCH32_DBGDRAR_VALID_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3U )

#define AARCH32_DBGDRAR_ROMADDR_47_12( _val ) ( ( _val ) << 12 )
#define AARCH32_DBGDRAR_ROMADDR_47_12_SHIFT 12
#define AARCH32_DBGDRAR_ROMADDR_47_12_MASK 0xfffffffff000ULL
#define AARCH32_DBGDRAR_ROMADDR_47_12_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfffffffffULL )

static inline uint32_t _AArch32_Read_32_dbgdrar( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c1, c0, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* DBGDRAR, Debug ROM Address Register */

static inline uint64_t _AArch32_Read_64_dbgdrar( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p14, 0, %Q0, %R0, c1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* DBGDSAR, Debug Self Address Register */

static inline uint32_t _AArch32_Read_32_dbgdsar( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c2, c0, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* DBGDSAR, Debug Self Address Register */

static inline uint64_t _AArch32_Read_64_dbgdsar( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p14, 0, %Q0, %R0, c2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* DBGDSCREXT, Debug Status and Control Register, External View */

#define AARCH32_DBGDSCREXT_MOE( _val ) ( ( _val ) << 2 )
#define AARCH32_DBGDSCREXT_MOE_SHIFT 2
#define AARCH32_DBGDSCREXT_MOE_MASK 0x3cU
#define AARCH32_DBGDSCREXT_MOE_GET( _reg ) \
  ( ( ( _reg ) >> 2 ) & 0xfU )

#define AARCH32_DBGDSCREXT_ERR 0x40U

#define AARCH32_DBGDSCREXT_UDCCDIS 0x1000U

#define AARCH32_DBGDSCREXT_HDE 0x4000U

#define AARCH32_DBGDSCREXT_MDBGEN 0x8000U

#define AARCH32_DBGDSCREXT_SPIDDIS 0x10000U

#define AARCH32_DBGDSCREXT_SPNIDDIS 0x20000U

#define AARCH32_DBGDSCREXT_NS 0x40000U

#define AARCH32_DBGDSCREXT_SC2 0x80000U

#define AARCH32_DBGDSCREXT_TDA 0x200000U

#define AARCH32_DBGDSCREXT_INTDIS( _val ) ( ( _val ) << 22 )
#define AARCH32_DBGDSCREXT_INTDIS_SHIFT 22
#define AARCH32_DBGDSCREXT_INTDIS_MASK 0xc00000U
#define AARCH32_DBGDSCREXT_INTDIS_GET( _reg ) \
  ( ( ( _reg ) >> 22 ) & 0x3U )

#define AARCH32_DBGDSCREXT_TXU 0x4000000U

#define AARCH32_DBGDSCREXT_RXO 0x8000000U

#define AARCH32_DBGDSCREXT_TXFULL 0x20000000U

#define AARCH32_DBGDSCREXT_RXFULL 0x40000000U

#define AARCH32_DBGDSCREXT_TFO 0x80000000U

static inline uint32_t _AArch32_Read_dbgdscrext( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c2, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgdscrext( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c2, 2" : : "r" ( value ) : "memory"
  );
}

/* DBGDSCRINT, Debug Status and Control Register, Internal View */

#define AARCH32_DBGDSCRINT_MOE( _val ) ( ( _val ) << 2 )
#define AARCH32_DBGDSCRINT_MOE_SHIFT 2
#define AARCH32_DBGDSCRINT_MOE_MASK 0x3cU
#define AARCH32_DBGDSCRINT_MOE_GET( _reg ) \
  ( ( ( _reg ) >> 2 ) & 0xfU )

#define AARCH32_DBGDSCRINT_UDCCDIS 0x1000U

#define AARCH32_DBGDSCRINT_MDBGEN 0x8000U

#define AARCH32_DBGDSCRINT_SPIDDIS 0x10000U

#define AARCH32_DBGDSCRINT_SPNIDDIS 0x20000U

#define AARCH32_DBGDSCRINT_NS 0x40000U

#define AARCH32_DBGDSCRINT_TXFULL 0x20000000U

#define AARCH32_DBGDSCRINT_RXFULL 0x40000000U

static inline uint32_t _AArch32_Read_dbgdscrint( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c1, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* DBGDTRRXEXT, Debug OS Lock Data Transfer Register, Receive, External View */

static inline uint32_t _AArch32_Read_dbgdtrrxext( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c0, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgdtrrxext( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c0, 2" : : "r" ( value ) : "memory"
  );
}

/* DBGDTRRXINT, Debug Data Transfer Register, Receive */

static inline uint32_t _AArch32_Read_dbgdtrrxint( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c5, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* DBGDTRTXEXT, Debug OS Lock Data Transfer Register, Transmit */

static inline uint32_t _AArch32_Read_dbgdtrtxext( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c3, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgdtrtxext( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c3, 2" : : "r" ( value ) : "memory"
  );
}

/* DBGDTRTXINT, Debug Data Transfer Register, Transmit */

static inline void _AArch32_Write_dbgdtrtxint( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c5, 0" : : "r" ( value ) : "memory"
  );
}

/* DBGOSDLR, Debug OS Double Lock Register */

#define AARCH32_DBGOSDLR_DLK 0x1U

static inline uint32_t _AArch32_Read_dbgosdlr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c1, c3, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgosdlr( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c1, c3, 4" : : "r" ( value ) : "memory"
  );
}

/* DBGOSECCR, Debug OS Lock Exception Catch Control Register */

static inline uint32_t _AArch32_Read_dbgoseccr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c6, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgoseccr( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c6, 2" : : "r" ( value ) : "memory"
  );
}

/* DBGOSLAR, Debug OS Lock Access Register */

static inline void _AArch32_Write_dbgoslar( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c1, c0, 4" : : "r" ( value ) : "memory"
  );
}

/* DBGOSLSR, Debug OS Lock Status Register */

#define AARCH32_DBGOSLSR_OSLM_0 0x1U

#define AARCH32_DBGOSLSR_OSLK 0x2U

#define AARCH32_DBGOSLSR_NTT 0x4U

#define AARCH32_DBGOSLSR_OSLM_1 0x8U

static inline uint32_t _AArch32_Read_dbgoslsr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c1, c1, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* DBGPRCR, Debug Power Control Register */

#define AARCH32_DBGPRCR_CORENPDRQ 0x1U

static inline uint32_t _AArch32_Read_dbgprcr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c1, c4, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgprcr( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c1, c4, 4" : : "r" ( value ) : "memory"
  );
}

/* DBGVCR, Debug Vector Catch Register */

#define AARCH32_DBGVCR_SU 0x2U

#define AARCH32_DBGVCR_U 0x2U

#define AARCH32_DBGVCR_S 0x4U

#define AARCH32_DBGVCR_SS 0x4U

#define AARCH32_DBGVCR_P 0x8U

#define AARCH32_DBGVCR_SP 0x8U

#define AARCH32_DBGVCR_D 0x10U

#define AARCH32_DBGVCR_SD 0x10U

#define AARCH32_DBGVCR_I 0x40U

#define AARCH32_DBGVCR_SI 0x40U

#define AARCH32_DBGVCR_F 0x80U

#define AARCH32_DBGVCR_SF 0x80U

#define AARCH32_DBGVCR_MS 0x400U

#define AARCH32_DBGVCR_MP 0x800U

#define AARCH32_DBGVCR_MD 0x1000U

#define AARCH32_DBGVCR_MI 0x4000U

#define AARCH32_DBGVCR_MF 0x8000U

#define AARCH32_DBGVCR_NSU 0x2000000U

#define AARCH32_DBGVCR_NSS 0x4000000U

#define AARCH32_DBGVCR_NSP 0x8000000U

#define AARCH32_DBGVCR_NSD 0x10000000U

#define AARCH32_DBGVCR_NSI 0x40000000U

#define AARCH32_DBGVCR_NSF 0x80000000U

static inline uint32_t _AArch32_Read_dbgvcr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c7, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgvcr( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c7, 0" : : "r" ( value ) : "memory"
  );
}

/* DBGWCR, Debug Watchpoint Control Registers */

#define AARCH32_DBGWCR_E 0x1U

#define AARCH32_DBGWCR_PAC( _val ) ( ( _val ) << 1 )
#define AARCH32_DBGWCR_PAC_SHIFT 1
#define AARCH32_DBGWCR_PAC_MASK 0x6U
#define AARCH32_DBGWCR_PAC_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x3U )

#define AARCH32_DBGWCR_LSC( _val ) ( ( _val ) << 3 )
#define AARCH32_DBGWCR_LSC_SHIFT 3
#define AARCH32_DBGWCR_LSC_MASK 0x18U
#define AARCH32_DBGWCR_LSC_GET( _reg ) \
  ( ( ( _reg ) >> 3 ) & 0x3U )

#define AARCH32_DBGWCR_BAS( _val ) ( ( _val ) << 5 )
#define AARCH32_DBGWCR_BAS_SHIFT 5
#define AARCH32_DBGWCR_BAS_MASK 0x1fe0U
#define AARCH32_DBGWCR_BAS_GET( _reg ) \
  ( ( ( _reg ) >> 5 ) & 0xffU )

#define AARCH32_DBGWCR_HMC 0x2000U

#define AARCH32_DBGWCR_SSC( _val ) ( ( _val ) << 14 )
#define AARCH32_DBGWCR_SSC_SHIFT 14
#define AARCH32_DBGWCR_SSC_MASK 0xc000U
#define AARCH32_DBGWCR_SSC_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x3U )

#define AARCH32_DBGWCR_LBN( _val ) ( ( _val ) << 16 )
#define AARCH32_DBGWCR_LBN_SHIFT 16
#define AARCH32_DBGWCR_LBN_MASK 0xf0000U
#define AARCH32_DBGWCR_LBN_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_DBGWCR_WT 0x100000U

#define AARCH32_DBGWCR_MASK( _val ) ( ( _val ) << 24 )
#define AARCH32_DBGWCR_MASK_SHIFT 24
#define AARCH32_DBGWCR_MASK_MASK 0x1f000000U
#define AARCH32_DBGWCR_MASK_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0x1fU )

/* DBGWCR_0, Debug Watchpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgwcr_0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c0, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwcr_0( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c0, 7" : : "r" ( value ) : "memory"
  );
}

/* DBGWCR_1, Debug Watchpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgwcr_1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c1, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwcr_1( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c1, 7" : : "r" ( value ) : "memory"
  );
}

/* DBGWCR_2, Debug Watchpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgwcr_2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c2, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwcr_2( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c2, 7" : : "r" ( value ) : "memory"
  );
}

/* DBGWCR_3, Debug Watchpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgwcr_3( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c3, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwcr_3( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c3, 7" : : "r" ( value ) : "memory"
  );
}

/* DBGWCR_4, Debug Watchpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgwcr_4( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c4, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwcr_4( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c4, 7" : : "r" ( value ) : "memory"
  );
}

/* DBGWCR_5, Debug Watchpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgwcr_5( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c5, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwcr_5( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c5, 7" : : "r" ( value ) : "memory"
  );
}

/* DBGWCR_6, Debug Watchpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgwcr_6( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c6, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwcr_6( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c6, 7" : : "r" ( value ) : "memory"
  );
}

/* DBGWCR_7, Debug Watchpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgwcr_7( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c7, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwcr_7( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c7, 7" : : "r" ( value ) : "memory"
  );
}

/* DBGWCR_8, Debug Watchpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgwcr_8( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c8, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwcr_8( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c8, 7" : : "r" ( value ) : "memory"
  );
}

/* DBGWCR_9, Debug Watchpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgwcr_9( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c9, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwcr_9( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c9, 7" : : "r" ( value ) : "memory"
  );
}

/* DBGWCR_10, Debug Watchpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgwcr_10( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c10, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwcr_10( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c10, 7" : : "r" ( value ) : "memory"
  );
}

/* DBGWCR_11, Debug Watchpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgwcr_11( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c11, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwcr_11( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c11, 7" : : "r" ( value ) : "memory"
  );
}

/* DBGWCR_12, Debug Watchpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgwcr_12( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c12, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwcr_12( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c12, 7" : : "r" ( value ) : "memory"
  );
}

/* DBGWCR_13, Debug Watchpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgwcr_13( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c13, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwcr_13( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c13, 7" : : "r" ( value ) : "memory"
  );
}

/* DBGWCR_14, Debug Watchpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgwcr_14( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c14, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwcr_14( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c14, 7" : : "r" ( value ) : "memory"
  );
}

/* DBGWCR_15, Debug Watchpoint Control Registers */

static inline uint32_t _AArch32_Read_dbgwcr_15( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c15, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwcr_15( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c15, 7" : : "r" ( value ) : "memory"
  );
}

/* DBGWFAR, Debug Watchpoint Fault Address Register */

static inline uint32_t _AArch32_Read_dbgwfar( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c6, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwfar( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c6, 0" : : "r" ( value ) : "memory"
  );
}

/* DBGWVR, Debug Watchpoint Value Registers */

#define AARCH32_DBGWVR_VA( _val ) ( ( _val ) << 2 )
#define AARCH32_DBGWVR_VA_SHIFT 2
#define AARCH32_DBGWVR_VA_MASK 0xfffffffcU
#define AARCH32_DBGWVR_VA_GET( _reg ) \
  ( ( ( _reg ) >> 2 ) & 0x3fffffffU )

/* DBGWVR_0, Debug Watchpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgwvr_0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c0, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwvr_0( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c0, 6" : : "r" ( value ) : "memory"
  );
}

/* DBGWVR_1, Debug Watchpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgwvr_1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c1, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwvr_1( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c1, 6" : : "r" ( value ) : "memory"
  );
}

/* DBGWVR_2, Debug Watchpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgwvr_2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c2, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwvr_2( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c2, 6" : : "r" ( value ) : "memory"
  );
}

/* DBGWVR_3, Debug Watchpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgwvr_3( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c3, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwvr_3( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c3, 6" : : "r" ( value ) : "memory"
  );
}

/* DBGWVR_4, Debug Watchpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgwvr_4( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c4, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwvr_4( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c4, 6" : : "r" ( value ) : "memory"
  );
}

/* DBGWVR_5, Debug Watchpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgwvr_5( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c5, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwvr_5( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c5, 6" : : "r" ( value ) : "memory"
  );
}

/* DBGWVR_6, Debug Watchpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgwvr_6( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c6, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwvr_6( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c6, 6" : : "r" ( value ) : "memory"
  );
}

/* DBGWVR_7, Debug Watchpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgwvr_7( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c7, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwvr_7( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c7, 6" : : "r" ( value ) : "memory"
  );
}

/* DBGWVR_8, Debug Watchpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgwvr_8( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c8, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwvr_8( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c8, 6" : : "r" ( value ) : "memory"
  );
}

/* DBGWVR_9, Debug Watchpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgwvr_9( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c9, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwvr_9( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c9, 6" : : "r" ( value ) : "memory"
  );
}

/* DBGWVR_10, Debug Watchpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgwvr_10( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c10, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwvr_10( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c10, 6" : : "r" ( value ) : "memory"
  );
}

/* DBGWVR_11, Debug Watchpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgwvr_11( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c11, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwvr_11( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c11, 6" : : "r" ( value ) : "memory"
  );
}

/* DBGWVR_12, Debug Watchpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgwvr_12( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c12, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwvr_12( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c12, 6" : : "r" ( value ) : "memory"
  );
}

/* DBGWVR_13, Debug Watchpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgwvr_13( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c13, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwvr_13( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c13, 6" : : "r" ( value ) : "memory"
  );
}

/* DBGWVR_14, Debug Watchpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgwvr_14( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c14, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwvr_14( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c14, 6" : : "r" ( value ) : "memory"
  );
}

/* DBGWVR_15, Debug Watchpoint Value Registers */

static inline uint32_t _AArch32_Read_dbgwvr_15( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p14, 0, %0, c0, c15, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dbgwvr_15( uint32_t value )
{
  __asm__ volatile (
    "mcr p14, 0, %0, c0, c15, 6" : : "r" ( value ) : "memory"
  );
}

/* DLR, Debug Link Register */

static inline uint32_t _AArch32_Read_dlr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 3, %0, c4, c5, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dlr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 3, %0, c4, c5, 1" : : "r" ( value ) : "memory"
  );
}

/* DSPSR, Debug Saved Program Status Register */

#define AARCH32_DSPSR_M_4_0( _val ) ( ( _val ) << 0 )
#define AARCH32_DSPSR_M_4_0_SHIFT 0
#define AARCH32_DSPSR_M_4_0_MASK 0x1fU
#define AARCH32_DSPSR_M_4_0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1fU )

#define AARCH32_DSPSR_T 0x20U

#define AARCH32_DSPSR_F 0x40U

#define AARCH32_DSPSR_I 0x80U

#define AARCH32_DSPSR_A 0x100U

#define AARCH32_DSPSR_E 0x200U

#define AARCH32_DSPSR_IT_7_2( _val ) ( ( _val ) << 10 )
#define AARCH32_DSPSR_IT_7_2_SHIFT 10
#define AARCH32_DSPSR_IT_7_2_MASK 0xfc00U
#define AARCH32_DSPSR_IT_7_2_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3fU )

#define AARCH32_DSPSR_GE( _val ) ( ( _val ) << 16 )
#define AARCH32_DSPSR_GE_SHIFT 16
#define AARCH32_DSPSR_GE_MASK 0xf0000U
#define AARCH32_DSPSR_GE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH32_DSPSR_IL 0x100000U

#define AARCH32_DSPSR_SS 0x200000U

#define AARCH32_DSPSR_PAN 0x400000U

#define AARCH32_DSPSR_SSBS 0x800000U

#define AARCH32_DSPSR_DIT 0x1000000U

#define AARCH32_DSPSR_IT_1_0( _val ) ( ( _val ) << 25 )
#define AARCH32_DSPSR_IT_1_0_SHIFT 25
#define AARCH32_DSPSR_IT_1_0_MASK 0x6000000U
#define AARCH32_DSPSR_IT_1_0_GET( _reg ) \
  ( ( ( _reg ) >> 25 ) & 0x3U )

#define AARCH32_DSPSR_Q 0x8000000U

#define AARCH32_DSPSR_V 0x10000000U

#define AARCH32_DSPSR_C 0x20000000U

#define AARCH32_DSPSR_Z 0x40000000U

#define AARCH32_DSPSR_N 0x80000000U

static inline uint32_t _AArch32_Read_dspsr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 3, %0, c4, c5, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_dspsr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 3, %0, c4, c5, 0" : : "r" ( value ) : "memory"
  );
}

/* HDCR, Hyp Debug Control Register */

#define AARCH32_HDCR_HPMN( _val ) ( ( _val ) << 0 )
#define AARCH32_HDCR_HPMN_SHIFT 0
#define AARCH32_HDCR_HPMN_MASK 0x1fU
#define AARCH32_HDCR_HPMN_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1fU )

#define AARCH32_HDCR_TPMCR 0x20U

#define AARCH32_HDCR_TPM 0x40U

#define AARCH32_HDCR_HPME 0x80U

#define AARCH32_HDCR_TDE 0x100U

#define AARCH32_HDCR_TDA 0x200U

#define AARCH32_HDCR_TDOSA 0x400U

#define AARCH32_HDCR_TDRA 0x800U

#define AARCH32_HDCR_HPMD 0x20000U

#define AARCH32_HDCR_TTRF 0x80000U

#define AARCH32_HDCR_HCCD 0x800000U

#define AARCH32_HDCR_HLP 0x4000000U

#define AARCH32_HDCR_TDCC 0x8000000U

#define AARCH32_HDCR_MTPME 0x10000000U

static inline uint32_t _AArch32_Read_hdcr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c1, c1, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hdcr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c1, c1, 1" : : "r" ( value ) : "memory"
  );
}

/* HTRFCR, Hyp Trace Filter Control Register */

#define AARCH32_HTRFCR_E0HTRE 0x1U

#define AARCH32_HTRFCR_E2TRE 0x2U

#define AARCH32_HTRFCR_CX 0x8U

#define AARCH32_HTRFCR_TS( _val ) ( ( _val ) << 5 )
#define AARCH32_HTRFCR_TS_SHIFT 5
#define AARCH32_HTRFCR_TS_MASK 0x60U
#define AARCH32_HTRFCR_TS_GET( _reg ) \
  ( ( ( _reg ) >> 5 ) & 0x3U )

static inline uint32_t _AArch32_Read_htrfcr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c1, c2, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_htrfcr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c1, c2, 1" : : "r" ( value ) : "memory"
  );
}

/* PMMIR, Performance Monitors Machine Identification Register */

#define AARCH32_PMMIR_SLOTS( _val ) ( ( _val ) << 0 )
#define AARCH32_PMMIR_SLOTS_SHIFT 0
#define AARCH32_PMMIR_SLOTS_MASK 0xffU
#define AARCH32_PMMIR_SLOTS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

static inline uint32_t _AArch32_Read_pmmir( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c9, c14, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* SDCR, Secure Debug Control Register */

#define AARCH32_SDCR_SPD( _val ) ( ( _val ) << 14 )
#define AARCH32_SDCR_SPD_SHIFT 14
#define AARCH32_SDCR_SPD_MASK 0xc000U
#define AARCH32_SDCR_SPD_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x3U )

#define AARCH32_SDCR_SPME 0x20000U

#define AARCH32_SDCR_STE 0x40000U

#define AARCH32_SDCR_TTRF 0x80000U

#define AARCH32_SDCR_EDAD 0x100000U

#define AARCH32_SDCR_EPMAD 0x200000U

#define AARCH32_SDCR_SCCD 0x800000U

#define AARCH32_SDCR_TDCC 0x8000000U

#define AARCH32_SDCR_MTPME 0x10000000U

static inline uint32_t _AArch32_Read_sdcr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c1, c3, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_sdcr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c1, c3, 1" : : "r" ( value ) : "memory"
  );
}

/* SDER, Secure Debug Enable Register */

#define AARCH32_SDER_SUIDEN 0x1U

#define AARCH32_SDER_SUNIDEN 0x2U

static inline uint32_t _AArch32_Read_sder( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c1, c1, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_sder( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c1, c1, 1" : : "r" ( value ) : "memory"
  );
}

/* TRFCR, Trace Filter Control Register */

#define AARCH32_TRFCR_E0TRE 0x1U

#define AARCH32_TRFCR_E1TRE 0x2U

#define AARCH32_TRFCR_TS( _val ) ( ( _val ) << 5 )
#define AARCH32_TRFCR_TS_SHIFT 5
#define AARCH32_TRFCR_TS_MASK 0x60U
#define AARCH32_TRFCR_TS_GET( _reg ) \
  ( ( ( _reg ) >> 5 ) & 0x3U )

static inline uint32_t _AArch32_Read_trfcr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c1, c2, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_trfcr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c1, c2, 1" : : "r" ( value ) : "memory"
  );
}

/* PMCCFILTR, Performance Monitors Cycle Count Filter Register */

#define AARCH32_PMCCFILTR_NSH 0x8000000U

#define AARCH32_PMCCFILTR_NSU 0x10000000U

#define AARCH32_PMCCFILTR_NSK 0x20000000U

#define AARCH32_PMCCFILTR_U 0x40000000U

#define AARCH32_PMCCFILTR_P 0x80000000U

static inline uint32_t _AArch32_Read_pmccfiltr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c15, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmccfiltr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c15, 7" : : "r" ( value ) : "memory"
  );
}

/* PMCCNTR, Performance Monitors Cycle Count Register */

#define AARCH32_PMCCNTR_CCNT( _val ) ( ( _val ) << 0 )
#define AARCH32_PMCCNTR_CCNT_SHIFT 0
#define AARCH32_PMCCNTR_CCNT_MASK 0xffffffffffffffffULL
#define AARCH32_PMCCNTR_CCNT_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffffffffffULL )

static inline uint32_t _AArch32_Read_32_pmccntr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c9, c13, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_32_pmccntr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c9, c13, 0" : : "r" ( value ) : "memory"
  );
}

/* PMCCNTR, Performance Monitors Cycle Count Register */

static inline uint64_t _AArch32_Read_64_pmccntr( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 0, %Q0, %R0, c9" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_64_pmccntr( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 0, %Q0, %R0, c9" : : "r" ( value ) : "memory"
  );
}

/* PMCEID0, Performance Monitors Common Event Identification Register 0 */

static inline uint32_t _AArch32_Read_pmceid0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c9, c12, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* PMCEID1, Performance Monitors Common Event Identification Register 1 */

static inline uint32_t _AArch32_Read_pmceid1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c9, c12, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* PMCEID2, Performance Monitors Common Event Identification Register 2 */

static inline uint32_t _AArch32_Read_pmceid2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c9, c14, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* PMCEID3, Performance Monitors Common Event Identification Register 3 */

static inline uint32_t _AArch32_Read_pmceid3( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c9, c14, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* PMCNTENCLR, Performance Monitors Count Enable Clear Register */

#define AARCH32_PMCNTENCLR_C 0x80000000U

static inline uint32_t _AArch32_Read_pmcntenclr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c9, c12, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmcntenclr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c9, c12, 2" : : "r" ( value ) : "memory"
  );
}

/* PMCNTENSET, Performance Monitors Count Enable Set Register */

#define AARCH32_PMCNTENSET_C 0x80000000U

static inline uint32_t _AArch32_Read_pmcntenset( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c9, c12, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmcntenset( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c9, c12, 1" : : "r" ( value ) : "memory"
  );
}

/* PMCR, Performance Monitors Control Register */

#define AARCH32_PMCR_E 0x1U

#define AARCH32_PMCR_P 0x2U

#define AARCH32_PMCR_C 0x4U

#define AARCH32_PMCR_D 0x8U

#define AARCH32_PMCR_X 0x10U

#define AARCH32_PMCR_DP 0x20U

#define AARCH32_PMCR_LC 0x40U

#define AARCH32_PMCR_LP 0x80U

#define AARCH32_PMCR_N( _val ) ( ( _val ) << 11 )
#define AARCH32_PMCR_N_SHIFT 11
#define AARCH32_PMCR_N_MASK 0xf800U
#define AARCH32_PMCR_N_GET( _reg ) \
  ( ( ( _reg ) >> 11 ) & 0x1fU )

#define AARCH32_PMCR_IDCODE( _val ) ( ( _val ) << 16 )
#define AARCH32_PMCR_IDCODE_SHIFT 16
#define AARCH32_PMCR_IDCODE_MASK 0xff0000U
#define AARCH32_PMCR_IDCODE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xffU )

#define AARCH32_PMCR_IMP( _val ) ( ( _val ) << 24 )
#define AARCH32_PMCR_IMP_SHIFT 24
#define AARCH32_PMCR_IMP_MASK 0xff000000U
#define AARCH32_PMCR_IMP_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xffU )

static inline uint32_t _AArch32_Read_pmcr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c9, c12, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmcr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c9, c12, 0" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_0, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c8, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_0( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c8, 0" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_1, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c8, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_1( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c8, 1" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_2, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c8, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_2( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c8, 2" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_3, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_3( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c8, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_3( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c8, 3" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_4, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_4( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c8, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_4( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c8, 4" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_5, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_5( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c8, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_5( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c8, 5" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_6, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_6( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c8, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_6( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c8, 6" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_7, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_7( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c8, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_7( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c8, 7" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_8, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_8( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c9, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_8( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c9, 0" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_9, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_9( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c9, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_9( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c9, 1" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_10, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_10( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c9, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_10( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c9, 2" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_11, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_11( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c9, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_11( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c9, 3" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_12, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_12( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c9, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_12( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c9, 4" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_13, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_13( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c9, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_13( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c9, 5" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_14, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_14( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c9, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_14( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c9, 6" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_15, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_15( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c9, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_15( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c9, 7" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_16, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_16( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c10, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_16( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c10, 0" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_17, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_17( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c10, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_17( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c10, 1" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_18, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_18( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c10, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_18( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c10, 2" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_19, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_19( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c10, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_19( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c10, 3" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_20, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_20( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c10, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_20( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c10, 4" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_21, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_21( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c10, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_21( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c10, 5" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_22, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_22( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c10, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_22( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c10, 6" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_23, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_23( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c10, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_23( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c10, 7" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_24, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_24( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c11, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_24( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c11, 0" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_25, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_25( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c11, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_25( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c11, 1" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_26, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_26( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c11, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_26( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c11, 2" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_27, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_27( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c11, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_27( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c11, 3" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_28, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_28( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c11, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_28( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c11, 4" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_29, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_29( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c11, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_29( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c11, 5" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_30, Performance Monitors Event Count Registers */

static inline uint32_t _AArch32_Read_pmevcntr_30( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c11, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevcntr_30( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c11, 6" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER, Performance Monitors Event Type Registers */

#define AARCH32_PMEVTYPER_EVTCOUNT_9_0( _val ) ( ( _val ) << 0 )
#define AARCH32_PMEVTYPER_EVTCOUNT_9_0_SHIFT 0
#define AARCH32_PMEVTYPER_EVTCOUNT_9_0_MASK 0x3ffU
#define AARCH32_PMEVTYPER_EVTCOUNT_9_0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3ffU )

#define AARCH32_PMEVTYPER_EVTCOUNT_15_10( _val ) ( ( _val ) << 10 )
#define AARCH32_PMEVTYPER_EVTCOUNT_15_10_SHIFT 10
#define AARCH32_PMEVTYPER_EVTCOUNT_15_10_MASK 0xfc00U
#define AARCH32_PMEVTYPER_EVTCOUNT_15_10_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3fU )

#define AARCH32_PMEVTYPER_MT 0x2000000U

#define AARCH32_PMEVTYPER_NSH 0x8000000U

#define AARCH32_PMEVTYPER_NSU 0x10000000U

#define AARCH32_PMEVTYPER_NSK 0x20000000U

#define AARCH32_PMEVTYPER_U 0x40000000U

#define AARCH32_PMEVTYPER_P 0x80000000U

/* PMEVTYPER_0, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c12, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_0( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c12, 0" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_1, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c12, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_1( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c12, 1" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_2, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c12, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_2( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c12, 2" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_3, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_3( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c12, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_3( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c12, 3" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_4, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_4( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c12, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_4( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c12, 4" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_5, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_5( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c12, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_5( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c12, 5" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_6, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_6( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c12, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_6( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c12, 6" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_7, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_7( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c12, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_7( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c12, 7" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_8, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_8( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c13, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_8( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c13, 0" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_9, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_9( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c13, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_9( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c13, 1" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_10, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_10( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c13, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_10( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c13, 2" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_11, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_11( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c13, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_11( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c13, 3" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_12, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_12( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c13, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_12( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c13, 4" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_13, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_13( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c13, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_13( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c13, 5" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_14, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_14( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c13, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_14( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c13, 6" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_15, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_15( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c13, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_15( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c13, 7" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_16, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_16( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c14, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_16( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c14, 0" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_17, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_17( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c14, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_17( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c14, 1" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_18, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_18( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c14, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_18( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c14, 2" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_19, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_19( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c14, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_19( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c14, 3" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_20, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_20( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c14, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_20( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c14, 4" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_21, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_21( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c14, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_21( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c14, 5" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_22, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_22( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c14, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_22( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c14, 6" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_23, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_23( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c14, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_23( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c14, 7" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_24, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_24( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c15, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_24( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c15, 0" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_25, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_25( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c15, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_25( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c15, 1" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_26, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_26( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c15, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_26( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c15, 2" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_27, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_27( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c15, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_27( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c15, 3" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_28, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_28( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c15, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_28( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c15, 4" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_29, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_29( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c15, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_29( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c15, 5" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_30, Performance Monitors Event Type Registers */

static inline uint32_t _AArch32_Read_pmevtyper_30( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c15, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmevtyper_30( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c15, 6" : : "r" ( value ) : "memory"
  );
}

/* PMINTENCLR, Performance Monitors Interrupt Enable Clear Register */

#define AARCH32_PMINTENCLR_C 0x80000000U

static inline uint32_t _AArch32_Read_pmintenclr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c9, c14, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmintenclr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c9, c14, 2" : : "r" ( value ) : "memory"
  );
}

/* PMINTENSET, Performance Monitors Interrupt Enable Set Register */

#define AARCH32_PMINTENSET_C 0x80000000U

static inline uint32_t _AArch32_Read_pmintenset( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c9, c14, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmintenset( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c9, c14, 1" : : "r" ( value ) : "memory"
  );
}

/* PMOVSR, Performance Monitors Overflow Flag Status Register */

#define AARCH32_PMOVSR_C 0x80000000U

static inline uint32_t _AArch32_Read_pmovsr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c9, c12, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmovsr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c9, c12, 3" : : "r" ( value ) : "memory"
  );
}

/* PMOVSSET, Performance Monitors Overflow Flag Status Set Register */

#define AARCH32_PMOVSSET_C 0x80000000U

static inline uint32_t _AArch32_Read_pmovsset( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c9, c14, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmovsset( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c9, c14, 3" : : "r" ( value ) : "memory"
  );
}

/* PMSELR, Performance Monitors Event Counter Selection Register */

#define AARCH32_PMSELR_SEL( _val ) ( ( _val ) << 0 )
#define AARCH32_PMSELR_SEL_SHIFT 0
#define AARCH32_PMSELR_SEL_MASK 0x1fU
#define AARCH32_PMSELR_SEL_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1fU )

static inline uint32_t _AArch32_Read_pmselr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c9, c12, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmselr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c9, c12, 5" : : "r" ( value ) : "memory"
  );
}

/* PMSWINC, Performance Monitors Software Increment Register */

static inline void _AArch32_Write_pmswinc( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c9, c12, 4" : : "r" ( value ) : "memory"
  );
}

/* PMUSERENR, Performance Monitors User Enable Register */

#define AARCH32_PMUSERENR_EN 0x1U

#define AARCH32_PMUSERENR_SW 0x2U

#define AARCH32_PMUSERENR_CR 0x4U

#define AARCH32_PMUSERENR_ER 0x8U

static inline uint32_t _AArch32_Read_pmuserenr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c9, c14, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmuserenr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c9, c14, 0" : : "r" ( value ) : "memory"
  );
}

/* PMXEVCNTR, Performance Monitors Selected Event Count Register */

static inline uint32_t _AArch32_Read_pmxevcntr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c9, c13, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmxevcntr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c9, c13, 2" : : "r" ( value ) : "memory"
  );
}

/* PMXEVTYPER, Performance Monitors Selected Event Type Register */

static inline uint32_t _AArch32_Read_pmxevtyper( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c9, c13, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_pmxevtyper( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c9, c13, 1" : : "r" ( value ) : "memory"
  );
}

/* AMCFGR, Activity Monitors Configuration Register */

#define AARCH32_AMCFGR_N( _val ) ( ( _val ) << 0 )
#define AARCH32_AMCFGR_N_SHIFT 0
#define AARCH32_AMCFGR_N_MASK 0xffU
#define AARCH32_AMCFGR_N_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

#define AARCH32_AMCFGR_SIZE( _val ) ( ( _val ) << 8 )
#define AARCH32_AMCFGR_SIZE_SHIFT 8
#define AARCH32_AMCFGR_SIZE_MASK 0x3f00U
#define AARCH32_AMCFGR_SIZE_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0x3fU )

#define AARCH32_AMCFGR_HDBG 0x1000000U

#define AARCH32_AMCFGR_NCG( _val ) ( ( _val ) << 28 )
#define AARCH32_AMCFGR_NCG_SHIFT 28
#define AARCH32_AMCFGR_NCG_MASK 0xf0000000U
#define AARCH32_AMCFGR_NCG_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint32_t _AArch32_Read_amcfgr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c2, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AMCGCR, Activity Monitors Counter Group Configuration Register */

#define AARCH32_AMCGCR_CG0NC( _val ) ( ( _val ) << 0 )
#define AARCH32_AMCGCR_CG0NC_SHIFT 0
#define AARCH32_AMCGCR_CG0NC_MASK 0xffU
#define AARCH32_AMCGCR_CG0NC_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

#define AARCH32_AMCGCR_CG1NC( _val ) ( ( _val ) << 8 )
#define AARCH32_AMCGCR_CG1NC_SHIFT 8
#define AARCH32_AMCGCR_CG1NC_MASK 0xff00U
#define AARCH32_AMCGCR_CG1NC_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xffU )

static inline uint32_t _AArch32_Read_amcgcr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c2, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AMCNTENCLR0, Activity Monitors Count Enable Clear Register 0 */

static inline uint32_t _AArch32_Read_amcntenclr0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c2, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amcntenclr0( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c2, 4" : : "r" ( value ) : "memory"
  );
}

/* AMCNTENCLR1, Activity Monitors Count Enable Clear Register 1 */

static inline uint32_t _AArch32_Read_amcntenclr1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c3, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amcntenclr1( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c3, 0" : : "r" ( value ) : "memory"
  );
}

/* AMCNTENSET0, Activity Monitors Count Enable Set Register 0 */

static inline uint32_t _AArch32_Read_amcntenset0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c2, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amcntenset0( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c2, 5" : : "r" ( value ) : "memory"
  );
}

/* AMCNTENSET1, Activity Monitors Count Enable Set Register 1 */

static inline uint32_t _AArch32_Read_amcntenset1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c3, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amcntenset1( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c3, 1" : : "r" ( value ) : "memory"
  );
}

/* AMCR, Activity Monitors Control Register */

#define AARCH32_AMCR_HDBG 0x400U

#define AARCH32_AMCR_CG1RZ 0x20000U

static inline uint32_t _AArch32_Read_amcr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c2, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amcr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c2, 0" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR0, Activity Monitors Event Counter Registers 0 */

#define AARCH32_AMEVCNTR0_ACNT( _val ) ( ( _val ) << 0 )
#define AARCH32_AMEVCNTR0_ACNT_SHIFT 0
#define AARCH32_AMEVCNTR0_ACNT_MASK 0xffffffffffffffffULL
#define AARCH32_AMEVCNTR0_ACNT_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffffffffffULL )

/* AMEVCNTR0_0, Activity Monitors Event Counter Registers 0 */

static inline uint64_t _AArch32_Read_amevcntr0_0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 0, %Q0, %R0, c0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr0_0( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 0, %Q0, %R0, c0" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR0_1, Activity Monitors Event Counter Registers 0 */

static inline uint64_t _AArch32_Read_amevcntr0_1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 1, %Q0, %R0, c0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr0_1( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 1, %Q0, %R0, c0" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR0_2, Activity Monitors Event Counter Registers 0 */

static inline uint64_t _AArch32_Read_amevcntr0_2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 2, %Q0, %R0, c0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr0_2( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 2, %Q0, %R0, c0" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR0_3, Activity Monitors Event Counter Registers 0 */

static inline uint64_t _AArch32_Read_amevcntr0_3( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 3, %Q0, %R0, c0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr0_3( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 3, %Q0, %R0, c0" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR0_4, Activity Monitors Event Counter Registers 0 */

static inline uint64_t _AArch32_Read_amevcntr0_4( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 4, %Q0, %R0, c0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr0_4( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 4, %Q0, %R0, c0" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR0_5, Activity Monitors Event Counter Registers 0 */

static inline uint64_t _AArch32_Read_amevcntr0_5( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 5, %Q0, %R0, c0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr0_5( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 5, %Q0, %R0, c0" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR0_6, Activity Monitors Event Counter Registers 0 */

static inline uint64_t _AArch32_Read_amevcntr0_6( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 6, %Q0, %R0, c0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr0_6( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 6, %Q0, %R0, c0" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR0_7, Activity Monitors Event Counter Registers 0 */

static inline uint64_t _AArch32_Read_amevcntr0_7( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 7, %Q0, %R0, c0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr0_7( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 7, %Q0, %R0, c0" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR0_8, Activity Monitors Event Counter Registers 0 */

static inline uint64_t _AArch32_Read_amevcntr0_8( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 0, %Q0, %R0, c1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr0_8( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 0, %Q0, %R0, c1" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR0_9, Activity Monitors Event Counter Registers 0 */

static inline uint64_t _AArch32_Read_amevcntr0_9( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 1, %Q0, %R0, c1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr0_9( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 1, %Q0, %R0, c1" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR0_10, Activity Monitors Event Counter Registers 0 */

static inline uint64_t _AArch32_Read_amevcntr0_10( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 2, %Q0, %R0, c1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr0_10( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 2, %Q0, %R0, c1" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR0_11, Activity Monitors Event Counter Registers 0 */

static inline uint64_t _AArch32_Read_amevcntr0_11( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 3, %Q0, %R0, c1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr0_11( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 3, %Q0, %R0, c1" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR0_12, Activity Monitors Event Counter Registers 0 */

static inline uint64_t _AArch32_Read_amevcntr0_12( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 4, %Q0, %R0, c1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr0_12( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 4, %Q0, %R0, c1" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR0_13, Activity Monitors Event Counter Registers 0 */

static inline uint64_t _AArch32_Read_amevcntr0_13( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 5, %Q0, %R0, c1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr0_13( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 5, %Q0, %R0, c1" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR0_14, Activity Monitors Event Counter Registers 0 */

static inline uint64_t _AArch32_Read_amevcntr0_14( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 6, %Q0, %R0, c1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr0_14( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 6, %Q0, %R0, c1" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR0_15, Activity Monitors Event Counter Registers 0 */

static inline uint64_t _AArch32_Read_amevcntr0_15( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 7, %Q0, %R0, c1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr0_15( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 7, %Q0, %R0, c1" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR1, Activity Monitors Event Counter Registers 1 */

#define AARCH32_AMEVCNTR1_ACNT( _val ) ( ( _val ) << 0 )
#define AARCH32_AMEVCNTR1_ACNT_SHIFT 0
#define AARCH32_AMEVCNTR1_ACNT_MASK 0xffffffffffffffffULL
#define AARCH32_AMEVCNTR1_ACNT_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffffffffffULL )

/* AMEVCNTR1_0, Activity Monitors Event Counter Registers 1 */

static inline uint64_t _AArch32_Read_amevcntr1_0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 0, %Q0, %R0, c4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr1_0( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 0, %Q0, %R0, c4" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR1_1, Activity Monitors Event Counter Registers 1 */

static inline uint64_t _AArch32_Read_amevcntr1_1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 1, %Q0, %R0, c4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr1_1( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 1, %Q0, %R0, c4" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR1_2, Activity Monitors Event Counter Registers 1 */

static inline uint64_t _AArch32_Read_amevcntr1_2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 2, %Q0, %R0, c4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr1_2( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 2, %Q0, %R0, c4" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR1_3, Activity Monitors Event Counter Registers 1 */

static inline uint64_t _AArch32_Read_amevcntr1_3( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 3, %Q0, %R0, c4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr1_3( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 3, %Q0, %R0, c4" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR1_4, Activity Monitors Event Counter Registers 1 */

static inline uint64_t _AArch32_Read_amevcntr1_4( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 4, %Q0, %R0, c4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr1_4( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 4, %Q0, %R0, c4" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR1_5, Activity Monitors Event Counter Registers 1 */

static inline uint64_t _AArch32_Read_amevcntr1_5( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 5, %Q0, %R0, c4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr1_5( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 5, %Q0, %R0, c4" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR1_6, Activity Monitors Event Counter Registers 1 */

static inline uint64_t _AArch32_Read_amevcntr1_6( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 6, %Q0, %R0, c4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr1_6( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 6, %Q0, %R0, c4" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR1_7, Activity Monitors Event Counter Registers 1 */

static inline uint64_t _AArch32_Read_amevcntr1_7( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 7, %Q0, %R0, c4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr1_7( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 7, %Q0, %R0, c4" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR1_8, Activity Monitors Event Counter Registers 1 */

static inline uint64_t _AArch32_Read_amevcntr1_8( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 0, %Q0, %R0, c5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr1_8( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 0, %Q0, %R0, c5" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR1_9, Activity Monitors Event Counter Registers 1 */

static inline uint64_t _AArch32_Read_amevcntr1_9( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 1, %Q0, %R0, c5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr1_9( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 1, %Q0, %R0, c5" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR1_10, Activity Monitors Event Counter Registers 1 */

static inline uint64_t _AArch32_Read_amevcntr1_10( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 2, %Q0, %R0, c5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr1_10( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 2, %Q0, %R0, c5" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR1_11, Activity Monitors Event Counter Registers 1 */

static inline uint64_t _AArch32_Read_amevcntr1_11( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 3, %Q0, %R0, c5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr1_11( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 3, %Q0, %R0, c5" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR1_12, Activity Monitors Event Counter Registers 1 */

static inline uint64_t _AArch32_Read_amevcntr1_12( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 4, %Q0, %R0, c5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr1_12( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 4, %Q0, %R0, c5" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR1_13, Activity Monitors Event Counter Registers 1 */

static inline uint64_t _AArch32_Read_amevcntr1_13( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 5, %Q0, %R0, c5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr1_13( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 5, %Q0, %R0, c5" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR1_14, Activity Monitors Event Counter Registers 1 */

static inline uint64_t _AArch32_Read_amevcntr1_14( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 6, %Q0, %R0, c5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr1_14( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 6, %Q0, %R0, c5" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR1_15, Activity Monitors Event Counter Registers 1 */

static inline uint64_t _AArch32_Read_amevcntr1_15( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 7, %Q0, %R0, c5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevcntr1_15( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 7, %Q0, %R0, c5" : : "r" ( value ) : "memory"
  );
}

/* AMEVTYPER0, Activity Monitors Event Type Registers 0 */

#define AARCH32_AMEVTYPER0_EVTCOUNT( _val ) ( ( _val ) << 0 )
#define AARCH32_AMEVTYPER0_EVTCOUNT_SHIFT 0
#define AARCH32_AMEVTYPER0_EVTCOUNT_MASK 0xffffU
#define AARCH32_AMEVTYPER0_EVTCOUNT_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )

/* AMEVTYPER0_0, Activity Monitors Event Type Registers 0 */

static inline uint32_t _AArch32_Read_amevtyper0_0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c6, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AMEVTYPER0_1, Activity Monitors Event Type Registers 0 */

static inline uint32_t _AArch32_Read_amevtyper0_1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c6, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AMEVTYPER0_2, Activity Monitors Event Type Registers 0 */

static inline uint32_t _AArch32_Read_amevtyper0_2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c6, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AMEVTYPER0_3, Activity Monitors Event Type Registers 0 */

static inline uint32_t _AArch32_Read_amevtyper0_3( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c6, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AMEVTYPER0_4, Activity Monitors Event Type Registers 0 */

static inline uint32_t _AArch32_Read_amevtyper0_4( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c6, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AMEVTYPER0_5, Activity Monitors Event Type Registers 0 */

static inline uint32_t _AArch32_Read_amevtyper0_5( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c6, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AMEVTYPER0_6, Activity Monitors Event Type Registers 0 */

static inline uint32_t _AArch32_Read_amevtyper0_6( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c6, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AMEVTYPER0_7, Activity Monitors Event Type Registers 0 */

static inline uint32_t _AArch32_Read_amevtyper0_7( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c6, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AMEVTYPER0_8, Activity Monitors Event Type Registers 0 */

static inline uint32_t _AArch32_Read_amevtyper0_8( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c7, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AMEVTYPER0_9, Activity Monitors Event Type Registers 0 */

static inline uint32_t _AArch32_Read_amevtyper0_9( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c7, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AMEVTYPER0_10, Activity Monitors Event Type Registers 0 */

static inline uint32_t _AArch32_Read_amevtyper0_10( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c7, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AMEVTYPER0_11, Activity Monitors Event Type Registers 0 */

static inline uint32_t _AArch32_Read_amevtyper0_11( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c7, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AMEVTYPER0_12, Activity Monitors Event Type Registers 0 */

static inline uint32_t _AArch32_Read_amevtyper0_12( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c7, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AMEVTYPER0_13, Activity Monitors Event Type Registers 0 */

static inline uint32_t _AArch32_Read_amevtyper0_13( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c7, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AMEVTYPER0_14, Activity Monitors Event Type Registers 0 */

static inline uint32_t _AArch32_Read_amevtyper0_14( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c7, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AMEVTYPER0_15, Activity Monitors Event Type Registers 0 */

static inline uint32_t _AArch32_Read_amevtyper0_15( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c7, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AMEVTYPER1, Activity Monitors Event Type Registers 1 */

#define AARCH32_AMEVTYPER1_EVTCOUNT( _val ) ( ( _val ) << 0 )
#define AARCH32_AMEVTYPER1_EVTCOUNT_SHIFT 0
#define AARCH32_AMEVTYPER1_EVTCOUNT_MASK 0xffffU
#define AARCH32_AMEVTYPER1_EVTCOUNT_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )

/* AMEVTYPER1_0, Activity Monitors Event Type Registers 1 */

static inline uint32_t _AArch32_Read_amevtyper1_0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c14, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevtyper1_0( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c14, 0" : : "r" ( value ) : "memory"
  );
}

/* AMEVTYPER1_1, Activity Monitors Event Type Registers 1 */

static inline uint32_t _AArch32_Read_amevtyper1_1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c14, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevtyper1_1( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c14, 1" : : "r" ( value ) : "memory"
  );
}

/* AMEVTYPER1_2, Activity Monitors Event Type Registers 1 */

static inline uint32_t _AArch32_Read_amevtyper1_2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c14, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevtyper1_2( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c14, 2" : : "r" ( value ) : "memory"
  );
}

/* AMEVTYPER1_3, Activity Monitors Event Type Registers 1 */

static inline uint32_t _AArch32_Read_amevtyper1_3( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c14, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevtyper1_3( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c14, 3" : : "r" ( value ) : "memory"
  );
}

/* AMEVTYPER1_4, Activity Monitors Event Type Registers 1 */

static inline uint32_t _AArch32_Read_amevtyper1_4( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c14, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevtyper1_4( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c14, 4" : : "r" ( value ) : "memory"
  );
}

/* AMEVTYPER1_5, Activity Monitors Event Type Registers 1 */

static inline uint32_t _AArch32_Read_amevtyper1_5( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c14, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevtyper1_5( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c14, 5" : : "r" ( value ) : "memory"
  );
}

/* AMEVTYPER1_6, Activity Monitors Event Type Registers 1 */

static inline uint32_t _AArch32_Read_amevtyper1_6( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c14, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevtyper1_6( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c14, 6" : : "r" ( value ) : "memory"
  );
}

/* AMEVTYPER1_7, Activity Monitors Event Type Registers 1 */

static inline uint32_t _AArch32_Read_amevtyper1_7( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c14, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevtyper1_7( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c14, 7" : : "r" ( value ) : "memory"
  );
}

/* AMEVTYPER1_8, Activity Monitors Event Type Registers 1 */

static inline uint32_t _AArch32_Read_amevtyper1_8( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c15, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevtyper1_8( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c15, 0" : : "r" ( value ) : "memory"
  );
}

/* AMEVTYPER1_9, Activity Monitors Event Type Registers 1 */

static inline uint32_t _AArch32_Read_amevtyper1_9( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c15, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevtyper1_9( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c15, 1" : : "r" ( value ) : "memory"
  );
}

/* AMEVTYPER1_10, Activity Monitors Event Type Registers 1 */

static inline uint32_t _AArch32_Read_amevtyper1_10( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c15, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevtyper1_10( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c15, 2" : : "r" ( value ) : "memory"
  );
}

/* AMEVTYPER1_11, Activity Monitors Event Type Registers 1 */

static inline uint32_t _AArch32_Read_amevtyper1_11( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c15, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevtyper1_11( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c15, 3" : : "r" ( value ) : "memory"
  );
}

/* AMEVTYPER1_12, Activity Monitors Event Type Registers 1 */

static inline uint32_t _AArch32_Read_amevtyper1_12( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c15, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevtyper1_12( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c15, 4" : : "r" ( value ) : "memory"
  );
}

/* AMEVTYPER1_13, Activity Monitors Event Type Registers 1 */

static inline uint32_t _AArch32_Read_amevtyper1_13( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c15, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevtyper1_13( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c15, 5" : : "r" ( value ) : "memory"
  );
}

/* AMEVTYPER1_14, Activity Monitors Event Type Registers 1 */

static inline uint32_t _AArch32_Read_amevtyper1_14( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c15, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevtyper1_14( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c15, 6" : : "r" ( value ) : "memory"
  );
}

/* AMEVTYPER1_15, Activity Monitors Event Type Registers 1 */

static inline uint32_t _AArch32_Read_amevtyper1_15( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c15, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amevtyper1_15( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c15, 7" : : "r" ( value ) : "memory"
  );
}

/* AMUSERENR, Activity Monitors User Enable Register */

#define AARCH32_AMUSERENR_EN 0x1U

static inline uint32_t _AArch32_Read_amuserenr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c13, c2, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_amuserenr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c13, c2, 3" : : "r" ( value ) : "memory"
  );
}

/* DISR, Deferred Interrupt Status Register */

#define AARCH32_DISR_FS_3_0( _val ) ( ( _val ) << 0 )
#define AARCH32_DISR_FS_3_0_SHIFT 0
#define AARCH32_DISR_FS_3_0_MASK 0xfU
#define AARCH32_DISR_FS_3_0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_DISR_DFSC( _val ) ( ( _val ) << 0 )
#define AARCH32_DISR_DFSC_SHIFT 0
#define AARCH32_DISR_DFSC_MASK 0x3fU
#define AARCH32_DISR_DFSC_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )

#define AARCH32_DISR_STATUS( _val ) ( ( _val ) << 0 )
#define AARCH32_DISR_STATUS_SHIFT 0
#define AARCH32_DISR_STATUS_MASK 0x3fU
#define AARCH32_DISR_STATUS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )

#define AARCH32_DISR_EA 0x200U

#define AARCH32_DISR_LPAE 0x200U

#define AARCH32_DISR_FS_4 0x400U

#define AARCH32_DISR_AET_0( _val ) ( ( _val ) << 10 )
#define AARCH32_DISR_AET_SHIFT_0 10
#define AARCH32_DISR_AET_MASK_0 0xc00U
#define AARCH32_DISR_AET_GET_0( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3U )

#define AARCH32_DISR_EXT 0x1000U

#define AARCH32_DISR_AET_1( _val ) ( ( _val ) << 14 )
#define AARCH32_DISR_AET_SHIFT_1 14
#define AARCH32_DISR_AET_MASK_1 0xc000U
#define AARCH32_DISR_AET_GET_1( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x3U )

#define AARCH32_DISR_A 0x80000000U

static inline uint32_t _AArch32_Read_disr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c12, c1, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_disr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c12, c1, 1" : : "r" ( value ) : "memory"
  );
}

/* ERRIDR, Error Record ID Register */

#define AARCH32_ERRIDR_NUM( _val ) ( ( _val ) << 0 )
#define AARCH32_ERRIDR_NUM_SHIFT 0
#define AARCH32_ERRIDR_NUM_MASK 0xffffU
#define AARCH32_ERRIDR_NUM_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )

static inline uint32_t _AArch32_Read_erridr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c5, c3, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ERRSELR, Error Record Select Register */

#define AARCH32_ERRSELR_SEL( _val ) ( ( _val ) << 0 )
#define AARCH32_ERRSELR_SEL_SHIFT 0
#define AARCH32_ERRSELR_SEL_MASK 0xffffU
#define AARCH32_ERRSELR_SEL_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )

static inline uint32_t _AArch32_Read_errselr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c5, c3, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_errselr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c5, c3, 1" : : "r" ( value ) : "memory"
  );
}

/* ERXADDR, Selected Error Record Address Register */

static inline uint32_t _AArch32_Read_erxaddr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c5, c4, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_erxaddr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c5, c4, 3" : : "r" ( value ) : "memory"
  );
}

/* ERXADDR2, Selected Error Record Address Register 2 */

static inline uint32_t _AArch32_Read_erxaddr2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c5, c4, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_erxaddr2( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c5, c4, 7" : : "r" ( value ) : "memory"
  );
}

/* ERXCTLR, Selected Error Record Control Register */

static inline uint32_t _AArch32_Read_erxctlr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c5, c4, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_erxctlr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c5, c4, 1" : : "r" ( value ) : "memory"
  );
}

/* ERXCTLR2, Selected Error Record Control Register 2 */

static inline uint32_t _AArch32_Read_erxctlr2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c5, c4, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_erxctlr2( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c5, c4, 5" : : "r" ( value ) : "memory"
  );
}

/* ERXFR, Selected Error Record Feature Register */

static inline uint32_t _AArch32_Read_erxfr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c5, c4, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ERXFR2, Selected Error Record Feature Register 2 */

static inline uint32_t _AArch32_Read_erxfr2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c5, c4, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ERXMISC0, Selected Error Record Miscellaneous Register 0 */

static inline uint32_t _AArch32_Read_erxmisc0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c5, c5, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_erxmisc0( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c5, c5, 0" : : "r" ( value ) : "memory"
  );
}

/* ERXMISC1, Selected Error Record Miscellaneous Register 1 */

static inline uint32_t _AArch32_Read_erxmisc1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c5, c5, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_erxmisc1( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c5, c5, 1" : : "r" ( value ) : "memory"
  );
}

/* ERXMISC2, Selected Error Record Miscellaneous Register 2 */

static inline uint32_t _AArch32_Read_erxmisc2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c5, c5, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_erxmisc2( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c5, c5, 4" : : "r" ( value ) : "memory"
  );
}

/* ERXMISC3, Selected Error Record Miscellaneous Register 3 */

static inline uint32_t _AArch32_Read_erxmisc3( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c5, c5, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_erxmisc3( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c5, c5, 5" : : "r" ( value ) : "memory"
  );
}

/* ERXMISC4, Selected Error Record Miscellaneous Register 4 */

static inline uint32_t _AArch32_Read_erxmisc4( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c5, c5, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_erxmisc4( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c5, c5, 2" : : "r" ( value ) : "memory"
  );
}

/* ERXMISC5, Selected Error Record Miscellaneous Register 5 */

static inline uint32_t _AArch32_Read_erxmisc5( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c5, c5, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_erxmisc5( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c5, c5, 3" : : "r" ( value ) : "memory"
  );
}

/* ERXMISC6, Selected Error Record Miscellaneous Register 6 */

static inline uint32_t _AArch32_Read_erxmisc6( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c5, c5, 6" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_erxmisc6( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c5, c5, 6" : : "r" ( value ) : "memory"
  );
}

/* ERXMISC7, Selected Error Record Miscellaneous Register 7 */

static inline uint32_t _AArch32_Read_erxmisc7( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c5, c5, 7" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_erxmisc7( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c5, c5, 7" : : "r" ( value ) : "memory"
  );
}

/* ERXSTATUS, Selected Error Record Primary Status Register */

static inline uint32_t _AArch32_Read_erxstatus( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c5, c4, 2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_erxstatus( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c5, c4, 2" : : "r" ( value ) : "memory"
  );
}

/* VDFSR, Virtual SError Exception Syndrome Register */

#define AARCH32_VDFSR_EXT 0x1000U

#define AARCH32_VDFSR_AET( _val ) ( ( _val ) << 14 )
#define AARCH32_VDFSR_AET_SHIFT 14
#define AARCH32_VDFSR_AET_MASK 0xc000U
#define AARCH32_VDFSR_AET_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x3U )

static inline uint32_t _AArch32_Read_vdfsr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c5, c2, 3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_vdfsr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c5, c2, 3" : : "r" ( value ) : "memory"
  );
}

/* VDISR, Virtual Deferred Interrupt Status Register */

#define AARCH32_VDISR_FS_3_0( _val ) ( ( _val ) << 0 )
#define AARCH32_VDISR_FS_3_0_SHIFT 0
#define AARCH32_VDISR_FS_3_0_MASK 0xfU
#define AARCH32_VDISR_FS_3_0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH32_VDISR_STATUS( _val ) ( ( _val ) << 0 )
#define AARCH32_VDISR_STATUS_SHIFT 0
#define AARCH32_VDISR_STATUS_MASK 0x3fU
#define AARCH32_VDISR_STATUS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )

#define AARCH32_VDISR_LPAE 0x200U

#define AARCH32_VDISR_FS_4 0x400U

#define AARCH32_VDISR_EXT 0x1000U

#define AARCH32_VDISR_AET( _val ) ( ( _val ) << 14 )
#define AARCH32_VDISR_AET_SHIFT 14
#define AARCH32_VDISR_AET_MASK 0xc000U
#define AARCH32_VDISR_AET_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x3U )

#define AARCH32_VDISR_A 0x80000000U

static inline uint32_t _AArch32_Read_vdisr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c12, c1, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_vdisr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c12, c1, 1" : : "r" ( value ) : "memory"
  );
}

/* CNTFRQ, Counter-timer Frequency Register */

static inline uint32_t _AArch32_Read_cntfrq( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c0, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_cntfrq( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c0, 0" : : "r" ( value ) : "memory"
  );
}

/* CNTHCTL, Counter-timer Hyp Control Register */

#define AARCH32_CNTHCTL_PL1PCTEN 0x1U

#define AARCH32_CNTHCTL_PL1PCEN 0x2U

#define AARCH32_CNTHCTL_EVNTEN 0x4U

#define AARCH32_CNTHCTL_EVNTDIR 0x8U

#define AARCH32_CNTHCTL_EVNTI( _val ) ( ( _val ) << 4 )
#define AARCH32_CNTHCTL_EVNTI_SHIFT 4
#define AARCH32_CNTHCTL_EVNTI_MASK 0xf0U
#define AARCH32_CNTHCTL_EVNTI_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH32_CNTHCTL_EVNTIS 0x20000U

static inline uint32_t _AArch32_Read_cnthctl( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c14, c1, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_cnthctl( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c14, c1, 0" : : "r" ( value ) : "memory"
  );
}

/* CNTHP_CTL, Counter-timer Hyp Physical Timer Control Register */

#define AARCH32_CNTHP_CTL_ENABLE 0x1U

#define AARCH32_CNTHP_CTL_IMASK 0x2U

#define AARCH32_CNTHP_CTL_ISTATUS 0x4U

static inline uint32_t _AArch32_Read_cnthp_ctl( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c14, c2, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_cnthp_ctl( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c14, c2, 1" : : "r" ( value ) : "memory"
  );
}

/* CNTHP_CVAL, Counter-timer Hyp Physical CompareValue Register */

static inline uint64_t _AArch32_Read_cnthp_cval( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 6, %Q0, %R0, c14" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_cnthp_cval( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 6, %Q0, %R0, c14" : : "r" ( value ) : "memory"
  );
}

/* CNTHP_TVAL, Counter-timer Hyp Physical Timer TimerValue Register */

static inline uint32_t _AArch32_Read_cnthp_tval( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c14, c2, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_cnthp_tval( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c14, c2, 0" : : "r" ( value ) : "memory"
  );
}

/* CNTHPS_CTL, Counter-timer Secure Physical Timer Control Register (EL2) */

#define AARCH32_CNTHPS_CTL_ENABLE 0x1U

#define AARCH32_CNTHPS_CTL_IMASK 0x2U

#define AARCH32_CNTHPS_CTL_ISTATUS 0x4U

static inline uint32_t _AArch32_Read_cnthps_ctl( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c2, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_cnthps_ctl( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c2, 1" : : "r" ( value ) : "memory"
  );
}

/* CNTHPS_CVAL, Counter-timer Secure Physical Timer CompareValue Register (EL2) */

static inline uint64_t _AArch32_Read_cnthps_cval( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 2, %Q0, %R0, c14" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_cnthps_cval( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 2, %Q0, %R0, c14" : : "r" ( value ) : "memory"
  );
}

/* CNTHPS_TVAL, Counter-timer Secure Physical Timer TimerValue Register (EL2) */

static inline uint32_t _AArch32_Read_cnthps_tval( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c2, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_cnthps_tval( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c2, 0" : : "r" ( value ) : "memory"
  );
}

/* CNTHV_CTL, Counter-timer Virtual Timer Control Register (EL2) */

#define AARCH32_CNTHV_CTL_ENABLE 0x1U

#define AARCH32_CNTHV_CTL_IMASK 0x2U

#define AARCH32_CNTHV_CTL_ISTATUS 0x4U

static inline uint32_t _AArch32_Read_cnthv_ctl( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c3, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_cnthv_ctl( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c3, 1" : : "r" ( value ) : "memory"
  );
}

/* CNTHV_CVAL, Counter-timer Virtual Timer CompareValue Register (EL2) */

static inline uint64_t _AArch32_Read_cnthv_cval( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 3, %Q0, %R0, c14" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_cnthv_cval( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 3, %Q0, %R0, c14" : : "r" ( value ) : "memory"
  );
}

/* CNTHV_TVAL, Counter-timer Virtual Timer TimerValue Register (EL2) */

static inline uint32_t _AArch32_Read_cnthv_tval( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c3, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_cnthv_tval( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c3, 0" : : "r" ( value ) : "memory"
  );
}

/* CNTHVS_CTL, Counter-timer Secure Virtual Timer Control Register (EL2) */

#define AARCH32_CNTHVS_CTL_ENABLE 0x1U

#define AARCH32_CNTHVS_CTL_IMASK 0x2U

#define AARCH32_CNTHVS_CTL_ISTATUS 0x4U

static inline uint32_t _AArch32_Read_cnthvs_ctl( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c3, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_cnthvs_ctl( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c3, 1" : : "r" ( value ) : "memory"
  );
}

/* CNTHVS_CVAL, Counter-timer Secure Virtual Timer CompareValue Register (EL2) */

static inline uint64_t _AArch32_Read_cnthvs_cval( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 3, %Q0, %R0, c14" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_cnthvs_cval( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 3, %Q0, %R0, c14" : : "r" ( value ) : "memory"
  );
}

/* CNTHVS_TVAL, Counter-timer Secure Virtual Timer TimerValue Register (EL2) */

static inline uint32_t _AArch32_Read_cnthvs_tval( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c3, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_cnthvs_tval( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c3, 0" : : "r" ( value ) : "memory"
  );
}

/* CNTKCTL, Counter-timer Kernel Control Register */

#define AARCH32_CNTKCTL_PL0PCTEN 0x1U

#define AARCH32_CNTKCTL_PL0VCTEN 0x2U

#define AARCH32_CNTKCTL_EVNTEN 0x4U

#define AARCH32_CNTKCTL_EVNTDIR 0x8U

#define AARCH32_CNTKCTL_EVNTI( _val ) ( ( _val ) << 4 )
#define AARCH32_CNTKCTL_EVNTI_SHIFT 4
#define AARCH32_CNTKCTL_EVNTI_MASK 0xf0U
#define AARCH32_CNTKCTL_EVNTI_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH32_CNTKCTL_PL0VTEN 0x100U

#define AARCH32_CNTKCTL_PL0PTEN 0x200U

#define AARCH32_CNTKCTL_EVNTIS 0x20000U

static inline uint32_t _AArch32_Read_cntkctl( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c1, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_cntkctl( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c1, 0" : : "r" ( value ) : "memory"
  );
}

/* CNTP_CTL, Counter-timer Physical Timer Control Register */

#define AARCH32_CNTP_CTL_ENABLE 0x1U

#define AARCH32_CNTP_CTL_IMASK 0x2U

#define AARCH32_CNTP_CTL_ISTATUS 0x4U

static inline uint32_t _AArch32_Read_cntp_ctl( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c2, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_cntp_ctl( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c2, 1" : : "r" ( value ) : "memory"
  );
}

/* CNTP_CVAL, Counter-timer Physical Timer CompareValue Register */

static inline uint64_t _AArch32_Read_cntp_cval( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 2, %Q0, %R0, c14" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_cntp_cval( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 2, %Q0, %R0, c14" : : "r" ( value ) : "memory"
  );
}

/* CNTP_TVAL, Counter-timer Physical Timer TimerValue Register */

static inline uint32_t _AArch32_Read_cntp_tval( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c2, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_cntp_tval( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c2, 0" : : "r" ( value ) : "memory"
  );
}

/* CNTPCT, Counter-timer Physical Count Register */

static inline uint64_t _AArch32_Read_cntpct( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 0, %Q0, %R0, c14" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* CNTPCTSS, Counter-timer Self-Synchronized Physical Count Register */

static inline uint64_t _AArch32_Read_cntpctss( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 8, %Q0, %R0, c14" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* CNTV_CTL, Counter-timer Virtual Timer Control Register */

#define AARCH32_CNTV_CTL_ENABLE 0x1U

#define AARCH32_CNTV_CTL_IMASK 0x2U

#define AARCH32_CNTV_CTL_ISTATUS 0x4U

static inline uint32_t _AArch32_Read_cntv_ctl( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c3, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_cntv_ctl( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c3, 1" : : "r" ( value ) : "memory"
  );
}

/* CNTV_CVAL, Counter-timer Virtual Timer CompareValue Register */

static inline uint64_t _AArch32_Read_cntv_cval( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 3, %Q0, %R0, c14" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_cntv_cval( uint64_t value )
{
  __asm__ volatile (
    "mcrr p15, 3, %Q0, %R0, c14" : : "r" ( value ) : "memory"
  );
}

/* CNTV_TVAL, Counter-timer Virtual Timer TimerValue Register */

static inline uint32_t _AArch32_Read_cntv_tval( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c14, c3, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_cntv_tval( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c14, c3, 0" : : "r" ( value ) : "memory"
  );
}

/* CNTVCT, Counter-timer Virtual Count Register */

static inline uint64_t _AArch32_Read_cntvct( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 1, %Q0, %R0, c14" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* CNTVCTSS, Counter-timer Self-Synchronized Virtual Count Register */

static inline uint64_t _AArch32_Read_cntvctss( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrrc p15, 9, %Q0, %R0, c14" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* HMPUIR, Hypervisor MPU Type Register */

#define AARCH32_HMPUIR_REGION( _val ) ( ( _val ) << 0 )
#define AARCH32_HMPUIR_REGION_SHIFT 0
#define AARCH32_HMPUIR_REGION_MASK 0xffU
#define AARCH32_HMPUIR_REGION_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

static inline uint32_t _AArch32_Read_hmpuir( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c0, c0, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hmpuir( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c0, c0, 4" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR, Hypervisor Protection Region Base Address Register */

#define AARCH32_HPRBAR_XN 0x1U

#define AARCH32_HPRBAR_AP_2_1( _val ) ( ( _val ) << 1 )
#define AARCH32_HPRBAR_AP_2_1_SHIFT 1
#define AARCH32_HPRBAR_AP_2_1_MASK 0x6U
#define AARCH32_HPRBAR_AP_2_1_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x3U )

#define AARCH32_HPRBAR_SH_1_0( _val ) ( ( _val ) << 3 )
#define AARCH32_HPRBAR_SH_1_0_SHIFT 3
#define AARCH32_HPRBAR_SH_1_0_MASK 0x18U
#define AARCH32_HPRBAR_SH_1_0_GET( _reg ) \
  ( ( ( _reg ) >> 3 ) & 0x3U )

#define AARCH32_HPRBAR_BASE( _val ) ( ( _val ) << 6 )
#define AARCH32_HPRBAR_BASE_SHIFT 6
#define AARCH32_HPRBAR_BASE_MASK 0xffffffc0U
#define AARCH32_HPRBAR_BASE_GET( _reg ) \
  ( ( ( _reg ) >> 6 ) & 0x3ffffffU )

static inline uint32_t _AArch32_Read_hprbar( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c3, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c3, 0" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_0, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c8, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_0( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c8, 0" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_1, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c8, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_1( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c8, 4" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_2, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c9, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_2( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c9, 0" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_3, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_3( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c9, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_3( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c9, 4" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_4, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_4( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c10, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_4( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c10, 0" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_5, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_5( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c10, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_5( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c10, 4" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_6, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_6( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c11, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_6( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c11, 0" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_7, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_7( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c11, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_7( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c11, 4" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_8, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_8( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c12, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_8( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c12, 0" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_9, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_9( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c12, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_9( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c12, 4" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_10, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_10( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c13, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_10( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c13, 0" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_11, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_11( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c13, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_11( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c13, 4" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_12, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_12( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c14, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_12( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c14, 0" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_13, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_13( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c14, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_13( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c14, 4" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_14, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_14( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c15, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_14( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c15, 0" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_15, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_15( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c15, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_15( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c15, 4" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_16, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_16( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c8, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_16( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c8, 0" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_17, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_17( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c8, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_17( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c8, 4" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_18, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_18( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c9, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_18( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c9, 0" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_19, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_19( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c9, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_19( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c9, 4" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_20, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_20( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c10, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_20( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c10, 0" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_21, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_21( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c10, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_21( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c10, 4" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_22, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_22( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c11, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_22( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c11, 0" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_23, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_23( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c11, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_23( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c11, 4" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_24, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_24( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c12, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_24( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c12, 0" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_25, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_25( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c12, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_25( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c12, 4" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_26, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_26( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c13, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_26( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c13, 0" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_27, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_27( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c13, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_27( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c13, 4" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_28, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_28( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c14, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_28( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c14, 0" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_29, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_29( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c14, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_29( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c14, 4" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_30, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_30( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c15, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_30( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c15, 0" : : "r" ( value ) : "memory"
  );
}

/* HPRBAR_31, Hypervisor Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_hprbar_31( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c15, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprbar_31( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c15, 4" : : "r" ( value ) : "memory"
  );
}

/* HPRENR, Hypervisor Protection Region Enable Register */

static inline uint32_t _AArch32_Read_hprenr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c1, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprenr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c1, 1" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR, Hypervisor Protection Region Limit Address Register */

#define AARCH32_HPRLAR_EN 0x1U

#define AARCH32_HPRLAR_ATTRINDX_2_0( _val ) ( ( _val ) << 1 )
#define AARCH32_HPRLAR_ATTRINDX_2_0_SHIFT 1
#define AARCH32_HPRLAR_ATTRINDX_2_0_MASK 0xeU
#define AARCH32_HPRLAR_ATTRINDX_2_0_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x7U )

#define AARCH32_HPRLAR_LIMIT( _val ) ( ( _val ) << 6 )
#define AARCH32_HPRLAR_LIMIT_SHIFT 6
#define AARCH32_HPRLAR_LIMIT_MASK 0xffffffc0U
#define AARCH32_HPRLAR_LIMIT_GET( _reg ) \
  ( ( ( _reg ) >> 6 ) & 0x3ffffffU )

static inline uint32_t _AArch32_Read_hprlar( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c3, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c3, 1" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_0, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c8, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_0( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c8, 1" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_1, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c8, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_1( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c8, 5" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_2, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c9, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_2( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c9, 1" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_3, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_3( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c9, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_3( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c9, 5" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_4, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_4( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c10, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_4( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c10, 1" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_5, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_5( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c10, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_5( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c10, 5" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_6, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_6( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c11, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_6( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c11, 1" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_7, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_7( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c11, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_7( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c11, 5" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_8, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_8( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c12, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_8( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c12, 1" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_9, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_9( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c12, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_9( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c12, 5" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_10, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_10( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c13, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_10( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c13, 1" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_11, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_11( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c13, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_11( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c13, 5" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_12, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_12( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c14, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_12( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c14, 1" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_13, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_13( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c14, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_13( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c14, 5" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_14, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_14( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c15, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_14( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c15, 1" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_15, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_15( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c15, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_15( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c15, 5" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_16, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_16( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c8, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_16( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c8, 1" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_17, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_17( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c8, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_17( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c8, 5" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_18, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_18( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c9, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_18( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c9, 1" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_19, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_19( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c9, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_19( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c9, 5" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_20, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_20( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c10, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_20( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c10, 1" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_21, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_21( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c10, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_21( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c10, 5" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_22, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_22( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c11, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_22( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c11, 1" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_23, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_23( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c11, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_23( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c11, 5" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_24, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_24( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c12, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_24( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c12, 1" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_25, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_25( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c12, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_25( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c12, 5" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_26, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_26( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c13, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_26( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c13, 1" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_27, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_27( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c13, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_27( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c13, 5" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_28, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_28( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c14, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_28( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c14, 1" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_29, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_29( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c14, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_29( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c14, 5" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_30, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_30( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c15, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_30( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c15, 1" : : "r" ( value ) : "memory"
  );
}

/* HPRLAR_31, Hypervisor Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_hprlar_31( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 5, %0, c6, c15, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprlar_31( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 5, %0, c6, c15, 5" : : "r" ( value ) : "memory"
  );
}

/* HPRSELR, Hypervisor Protection Region Selector Register */

#define AARCH32_HPRSELR_REGION( _val ) ( ( _val ) << 0 )
#define AARCH32_HPRSELR_REGION_SHIFT 0
#define AARCH32_HPRSELR_REGION_MASK 0xffU
#define AARCH32_HPRSELR_REGION_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

static inline uint32_t _AArch32_Read_hprselr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 4, %0, c6, c2, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_hprselr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 4, %0, c6, c2, 1" : : "r" ( value ) : "memory"
  );
}

/* MPUIR, MPU Type Register */

#define AARCH32_MPUIR_REGION( _val ) ( ( _val ) << 8 )
#define AARCH32_MPUIR_REGION_SHIFT 8
#define AARCH32_MPUIR_REGION_MASK 0xff00U
#define AARCH32_MPUIR_REGION_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xffU )

static inline uint32_t _AArch32_Read_mpuir( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c0, c0, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_mpuir( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c0, c0, 4" : : "r" ( value ) : "memory"
  );
}

/* PRBAR, Protection Region Base Address Register */

#define AARCH32_PRBAR_XN 0x1U

#define AARCH32_PRBAR_AP_2_1( _val ) ( ( _val ) << 1 )
#define AARCH32_PRBAR_AP_2_1_SHIFT 1
#define AARCH32_PRBAR_AP_2_1_MASK 0x6U
#define AARCH32_PRBAR_AP_2_1_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x3U )

#define AARCH32_PRBAR_SH_1_0( _val ) ( ( _val ) << 3 )
#define AARCH32_PRBAR_SH_1_0_SHIFT 3
#define AARCH32_PRBAR_SH_1_0_MASK 0x18U
#define AARCH32_PRBAR_SH_1_0_GET( _reg ) \
  ( ( ( _reg ) >> 3 ) & 0x3U )

#define AARCH32_PRBAR_BASE( _val ) ( ( _val ) << 6 )
#define AARCH32_PRBAR_BASE_SHIFT 6
#define AARCH32_PRBAR_BASE_MASK 0xffffffc0U
#define AARCH32_PRBAR_BASE_GET( _reg ) \
  ( ( ( _reg ) >> 6 ) & 0x3ffffffU )

static inline uint32_t _AArch32_Read_prbar( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c3, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c3, 0" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_0, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c8, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_0( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c8, 0" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_1, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c8, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_1( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c8, 4" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_2, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c9, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_2( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c9, 0" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_3, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_3( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c9, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_3( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c9, 4" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_4, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_4( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c10, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_4( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c10, 0" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_5, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_5( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c10, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_5( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c10, 4" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_6, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_6( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c11, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_6( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c11, 0" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_7, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_7( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c11, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_7( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c11, 4" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_8, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_8( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c12, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_8( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c12, 0" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_9, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_9( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c12, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_9( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c12, 4" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_10, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_10( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c13, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_10( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c13, 0" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_11, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_11( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c13, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_11( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c13, 4" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_12, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_12( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c14, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_12( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c14, 0" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_13, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_13( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c14, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_13( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c14, 4" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_14, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_14( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c15, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_14( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c15, 0" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_15, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_15( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c15, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_15( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c15, 4" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_16, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_16( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c8, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_16( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c8, 0" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_17, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_17( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c8, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_17( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c8, 4" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_18, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_18( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c9, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_18( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c9, 0" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_19, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_19( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c9, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_19( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c9, 4" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_20, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_20( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c10, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_20( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c10, 0" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_21, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_21( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c10, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_21( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c10, 4" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_22, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_22( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c11, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_22( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c11, 0" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_23, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_23( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c11, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_23( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c11, 4" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_24, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_24( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c12, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_24( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c12, 0" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_25, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_25( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c12, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_25( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c12, 4" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_26, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_26( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c13, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_26( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c13, 0" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_27, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_27( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c13, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_27( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c13, 4" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_28, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_28( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c14, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_28( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c14, 0" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_29, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_29( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c14, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_29( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c14, 4" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_30, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_30( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c15, 0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_30( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c15, 0" : : "r" ( value ) : "memory"
  );
}

/* PRBAR_31, Protection Region Base Address Registers */

static inline uint32_t _AArch32_Read_prbar_31( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c15, 4" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prbar_31( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c15, 4" : : "r" ( value ) : "memory"
  );
}

/* PRLAR, Protection Region Limit Address Register */

#define AARCH32_PRLAR_EN 0x1U

#define AARCH32_PRLAR_ATTRINDX_2_0( _val ) ( ( _val ) << 1 )
#define AARCH32_PRLAR_ATTRINDX_2_0_SHIFT 1
#define AARCH32_PRLAR_ATTRINDX_2_0_MASK 0xeU
#define AARCH32_PRLAR_ATTRINDX_2_0_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x7U )

#define AARCH32_PRLAR_LIMIT( _val ) ( ( _val ) << 6 )
#define AARCH32_PRLAR_LIMIT_SHIFT 6
#define AARCH32_PRLAR_LIMIT_MASK 0xffffffc0U
#define AARCH32_PRLAR_LIMIT_GET( _reg ) \
  ( ( ( _reg ) >> 6 ) & 0x3ffffffU )

static inline uint32_t _AArch32_Read_prlar( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c3, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c3, 1" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_0, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_0( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c8, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_0( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c8, 1" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_1, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_1( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c8, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_1( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c8, 5" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_2, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_2( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c9, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_2( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c9, 1" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_3, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_3( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c9, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_3( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c9, 5" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_4, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_4( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c10, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_4( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c10, 1" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_5, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_5( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c10, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_5( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c10, 5" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_6, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_6( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c11, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_6( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c11, 1" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_7, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_7( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c11, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_7( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c11, 5" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_8, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_8( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c12, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_8( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c12, 1" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_9, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_9( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c12, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_9( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c12, 5" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_10, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_10( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c13, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_10( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c13, 1" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_11, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_11( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c13, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_11( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c13, 5" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_12, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_12( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c14, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_12( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c14, 1" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_13, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_13( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c14, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_13( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c14, 5" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_14, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_14( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c15, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_14( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c15, 1" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_15, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_15( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c15, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_15( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c15, 5" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_16, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_16( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c8, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_16( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c8, 1" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_17, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_17( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c8, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_17( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c8, 5" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_18, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_18( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c9, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_18( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c9, 1" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_19, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_19( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c9, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_19( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c9, 5" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_20, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_20( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c10, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_20( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c10, 1" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_21, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_21( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c10, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_21( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c10, 5" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_22, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_22( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c11, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_22( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c11, 1" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_23, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_23( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c11, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_23( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c11, 5" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_24, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_24( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c12, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_24( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c12, 1" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_25, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_25( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c12, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_25( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c12, 5" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_26, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_26( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c13, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_26( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c13, 1" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_27, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_27( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c13, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_27( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c13, 5" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_28, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_28( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c14, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_28( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c14, 1" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_29, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_29( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c14, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_29( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c14, 5" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_30, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_30( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c15, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_30( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c15, 1" : : "r" ( value ) : "memory"
  );
}

/* PRLAR_31, Protection Region Limit Address Registers */

static inline uint32_t _AArch32_Read_prlar_31( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 1, %0, c6, c15, 5" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prlar_31( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 1, %0, c6, c15, 5" : : "r" ( value ) : "memory"
  );
}

/* PRSELR, Protection Region Selector Register */

#define AARCH32_PRSELR_REGION( _val ) ( ( _val ) << 0 )
#define AARCH32_PRSELR_REGION_SHIFT 0
#define AARCH32_PRSELR_REGION_MASK 0xffU
#define AARCH32_PRSELR_REGION_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

static inline uint32_t _AArch32_Read_prselr( void )
{
  uint32_t value;

  __asm__ volatile (
    "mrc p15, 0, %0, c6, c2, 1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch32_Write_prselr( uint32_t value )
{
  __asm__ volatile (
    "mcr p15, 0, %0, c6, c2, 1" : : "r" ( value ) : "memory"
  );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_AARCH32_SYSTEM_REGISTERS_H */
