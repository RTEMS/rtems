/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief This header file provides the API to read and write the AArch64
 *   system registers.
 */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_SCORE_AARCH64_SYSTEM_REGISTERS_H
#define _RTEMS_SCORE_AARCH64_SYSTEM_REGISTERS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ACTLR_EL1, Auxiliary Control Register (EL1) */

static inline uint64_t _AArch64_Read_actlr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ACTLR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_actlr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr ACTLR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* ACTLR_EL2, Auxiliary Control Register (EL2) */

static inline uint64_t _AArch64_Read_actlr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ACTLR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_actlr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr ACTLR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* ACTLR_EL3, Auxiliary Control Register (EL3) */

static inline uint64_t _AArch64_Read_actlr_el3( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ACTLR_EL3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_actlr_el3( uint64_t value )
{
  __asm__ volatile (
    "msr ACTLR_EL3, %0" : : "r" ( value ) : "memory"
  );
}

/* AFSR0_EL1, Auxiliary Fault Status Register 0 (EL1) */

static inline uint64_t _AArch64_Read_afsr0_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AFSR0_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_afsr0_el1( uint64_t value )
{
  __asm__ volatile (
    "msr AFSR0_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* AFSR0_EL2, Auxiliary Fault Status Register 0 (EL2) */

static inline uint64_t _AArch64_Read_afsr0_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AFSR0_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_afsr0_el2( uint64_t value )
{
  __asm__ volatile (
    "msr AFSR0_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* AFSR0_EL3, Auxiliary Fault Status Register 0 (EL3) */

static inline uint64_t _AArch64_Read_afsr0_el3( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AFSR0_EL3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_afsr0_el3( uint64_t value )
{
  __asm__ volatile (
    "msr AFSR0_EL3, %0" : : "r" ( value ) : "memory"
  );
}

/* AFSR1_EL1, Auxiliary Fault Status Register 1 (EL1) */

static inline uint64_t _AArch64_Read_afsr1_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AFSR1_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_afsr1_el1( uint64_t value )
{
  __asm__ volatile (
    "msr AFSR1_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* AFSR1_EL2, Auxiliary Fault Status Register 1 (EL2) */

static inline uint64_t _AArch64_Read_afsr1_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AFSR1_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_afsr1_el2( uint64_t value )
{
  __asm__ volatile (
    "msr AFSR1_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* AFSR1_EL3, Auxiliary Fault Status Register 1 (EL3) */

static inline uint64_t _AArch64_Read_afsr1_el3( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AFSR1_EL3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_afsr1_el3( uint64_t value )
{
  __asm__ volatile (
    "msr AFSR1_EL3, %0" : : "r" ( value ) : "memory"
  );
}

/* AIDR_EL1, Auxiliary ID Register */

static inline uint64_t _AArch64_Read_aidr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AIDR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AMAIR_EL1, Auxiliary Memory Attribute Indirection Register (EL1) */

static inline uint64_t _AArch64_Read_amair_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AMAIR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_amair_el1( uint64_t value )
{
  __asm__ volatile (
    "msr AMAIR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* AMAIR_EL2, Auxiliary Memory Attribute Indirection Register (EL2) */

static inline uint64_t _AArch64_Read_amair_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AMAIR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_amair_el2( uint64_t value )
{
  __asm__ volatile (
    "msr AMAIR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* AMAIR_EL3, Auxiliary Memory Attribute Indirection Register (EL3) */

static inline uint64_t _AArch64_Read_amair_el3( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AMAIR_EL3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_amair_el3( uint64_t value )
{
  __asm__ volatile (
    "msr AMAIR_EL3, %0" : : "r" ( value ) : "memory"
  );
}

/* APDAKEYHI_EL1, Pointer Authentication Key A for Data (bits[127:64]) */

static inline uint64_t _AArch64_Read_apdakeyhi_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, APDAKEYHI_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_apdakeyhi_el1( uint64_t value )
{
  __asm__ volatile (
    "msr APDAKEYHI_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* APDAKEYLO_EL1, Pointer Authentication Key A for Data (bits[63:0]) */

static inline uint64_t _AArch64_Read_apdakeylo_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, APDAKEYLO_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_apdakeylo_el1( uint64_t value )
{
  __asm__ volatile (
    "msr APDAKEYLO_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* APDBKEYHI_EL1, Pointer Authentication Key B for Data (bits[127:64]) */

static inline uint64_t _AArch64_Read_apdbkeyhi_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, APDBKEYHI_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_apdbkeyhi_el1( uint64_t value )
{
  __asm__ volatile (
    "msr APDBKEYHI_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* APDBKEYLO_EL1, Pointer Authentication Key B for Data (bits[63:0]) */

static inline uint64_t _AArch64_Read_apdbkeylo_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, APDBKEYLO_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_apdbkeylo_el1( uint64_t value )
{
  __asm__ volatile (
    "msr APDBKEYLO_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* APGAKEYHI_EL1, Pointer Authentication Key A for Code (bits[127:64]) */

static inline uint64_t _AArch64_Read_apgakeyhi_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, APGAKEYHI_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_apgakeyhi_el1( uint64_t value )
{
  __asm__ volatile (
    "msr APGAKEYHI_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* APGAKEYLO_EL1, Pointer Authentication Key A for Code (bits[63:0]) */

static inline uint64_t _AArch64_Read_apgakeylo_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, APGAKEYLO_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_apgakeylo_el1( uint64_t value )
{
  __asm__ volatile (
    "msr APGAKEYLO_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* APIAKEYHI_EL1, Pointer Authentication Key A for Instruction (bits[127:64]) */

static inline uint64_t _AArch64_Read_apiakeyhi_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, APIAKEYHI_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_apiakeyhi_el1( uint64_t value )
{
  __asm__ volatile (
    "msr APIAKEYHI_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* APIAKEYLO_EL1, Pointer Authentication Key A for Instruction (bits[63:0]) */

static inline uint64_t _AArch64_Read_apiakeylo_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, APIAKEYLO_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_apiakeylo_el1( uint64_t value )
{
  __asm__ volatile (
    "msr APIAKEYLO_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* APIBKEYHI_EL1, Pointer Authentication Key B for Instruction (bits[127:64]) */

static inline uint64_t _AArch64_Read_apibkeyhi_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, APIBKEYHI_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_apibkeyhi_el1( uint64_t value )
{
  __asm__ volatile (
    "msr APIBKEYHI_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* APIBKEYLO_EL1, Pointer Authentication Key B for Instruction (bits[63:0]) */

static inline uint64_t _AArch64_Read_apibkeylo_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, APIBKEYLO_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_apibkeylo_el1( uint64_t value )
{
  __asm__ volatile (
    "msr APIBKEYLO_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* CCSIDR2_EL1, Current Cache Size ID Register 2 */

#define AARCH64_CCSIDR2_EL1_NUMSETS( _val ) ( ( _val ) << 0 )
#define AARCH64_CCSIDR2_EL1_NUMSETS_SHIFT 0
#define AARCH64_CCSIDR2_EL1_NUMSETS_MASK 0xffffffU
#define AARCH64_CCSIDR2_EL1_NUMSETS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffU )

static inline uint64_t _AArch64_Read_ccsidr2_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CCSIDR2_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* CCSIDR_EL1, Current Cache Size ID Register */

#define AARCH64_CCSIDR_EL1_LINESIZE( _val ) ( ( _val ) << 0 )
#define AARCH64_CCSIDR_EL1_LINESIZE_SHIFT 0
#define AARCH64_CCSIDR_EL1_LINESIZE_MASK 0x7U
#define AARCH64_CCSIDR_EL1_LINESIZE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x7U )

#define AARCH64_CCSIDR_EL1_ASSOCIATIVITY_0( _val ) ( ( _val ) << 3 )
#define AARCH64_CCSIDR_EL1_ASSOCIATIVITY_SHIFT_0 3
#define AARCH64_CCSIDR_EL1_ASSOCIATIVITY_MASK_0 0x1ff8U
#define AARCH64_CCSIDR_EL1_ASSOCIATIVITY_GET_0( _reg ) \
  ( ( ( _reg ) >> 3 ) & 0x3ffU )

#define AARCH64_CCSIDR_EL1_ASSOCIATIVITY_1( _val ) ( ( _val ) << 3 )
#define AARCH64_CCSIDR_EL1_ASSOCIATIVITY_SHIFT_1 3
#define AARCH64_CCSIDR_EL1_ASSOCIATIVITY_MASK_1 0xfffff8U
#define AARCH64_CCSIDR_EL1_ASSOCIATIVITY_GET_1( _reg ) \
  ( ( ( _reg ) >> 3 ) & 0x1fffffU )

#define AARCH64_CCSIDR_EL1_NUMSETS_0( _val ) ( ( _val ) << 13 )
#define AARCH64_CCSIDR_EL1_NUMSETS_SHIFT_0 13
#define AARCH64_CCSIDR_EL1_NUMSETS_MASK_0 0xfffe000U
#define AARCH64_CCSIDR_EL1_NUMSETS_GET_0( _reg ) \
  ( ( ( _reg ) >> 13 ) & 0x7fffU )

#define AARCH64_CCSIDR_EL1_NUMSETS_1( _val ) ( ( _val ) << 32 )
#define AARCH64_CCSIDR_EL1_NUMSETS_SHIFT_1 32
#define AARCH64_CCSIDR_EL1_NUMSETS_MASK_1 0xffffff00000000ULL
#define AARCH64_CCSIDR_EL1_NUMSETS_GET_1( _reg ) \
  ( ( ( _reg ) >> 32 ) & 0xffffffULL )

static inline uint64_t _AArch64_Read_ccsidr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CCSIDR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* CLIDR_EL1, Cache Level ID Register */

#define AARCH64_CLIDR_EL1_CTYPE1( _val ) ( ( _val ) << 0 )
#define AARCH64_CLIDR_EL1_CTYPE1_SHIFT 0
#define AARCH64_CLIDR_EL1_CTYPE1_MASK ( 0x7U << 0 )
#define AARCH64_CLIDR_EL1_CTYPE1_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x7U )

#define AARCH64_CLIDR_EL1_CTYPE2( _val ) ( ( _val ) << 3 )
#define AARCH64_CLIDR_EL1_CTYPE2_SHIFT 3
#define AARCH64_CLIDR_EL1_CTYPE2_MASK ( 0x7U << 3 )
#define AARCH64_CLIDR_EL1_CTYPE2_GET( _reg ) \
  ( ( ( _reg ) >> 3 ) & 0x7U )

#define AARCH64_CLIDR_EL1_CTYPE3( _val ) ( ( _val ) << 6 )
#define AARCH64_CLIDR_EL1_CTYPE3_SHIFT 6
#define AARCH64_CLIDR_EL1_CTYPE3_MASK ( 0x7U << 6 )
#define AARCH64_CLIDR_EL1_CTYPE3_GET( _reg ) \
  ( ( ( _reg ) >> 6 ) & 0x7U )

#define AARCH64_CLIDR_EL1_CTYPE4( _val ) ( ( _val ) << 9 )
#define AARCH64_CLIDR_EL1_CTYPE4_SHIFT 9
#define AARCH64_CLIDR_EL1_CTYPE4_MASK ( 0x7U << 9 )
#define AARCH64_CLIDR_EL1_CTYPE4_GET( _reg ) \
  ( ( ( _reg ) >> 9 ) & 0x7U )

#define AARCH64_CLIDR_EL1_CTYPE5( _val ) ( ( _val ) << 12 )
#define AARCH64_CLIDR_EL1_CTYPE5_SHIFT 12
#define AARCH64_CLIDR_EL1_CTYPE5_MASK ( 0x7U << 12 )
#define AARCH64_CLIDR_EL1_CTYPE5_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0x7U )

#define AARCH64_CLIDR_EL1_CTYPE6( _val ) ( ( _val ) << 15 )
#define AARCH64_CLIDR_EL1_CTYPE6_SHIFT 15
#define AARCH64_CLIDR_EL1_CTYPE6_MASK ( 0x7U << 15 )
#define AARCH64_CLIDR_EL1_CTYPE6_GET( _reg ) \
  ( ( ( _reg ) >> 15 ) & 0x7U )

#define AARCH64_CLIDR_EL1_CTYPE7( _val ) ( ( _val ) << 18 )
#define AARCH64_CLIDR_EL1_CTYPE7_SHIFT 18
#define AARCH64_CLIDR_EL1_CTYPE7_MASK ( 0x7U << 18 )
#define AARCH64_CLIDR_EL1_CTYPE7_GET( _reg ) \
  ( ( ( _reg ) >> 18 ) & 0x7U )

#define AARCH64_CLIDR_EL1_LOUIS( _val ) ( ( _val ) << 21 )
#define AARCH64_CLIDR_EL1_LOUIS_SHIFT 21
#define AARCH64_CLIDR_EL1_LOUIS_MASK 0xe00000U
#define AARCH64_CLIDR_EL1_LOUIS_GET( _reg ) \
  ( ( ( _reg ) >> 21 ) & 0x7U )

#define AARCH64_CLIDR_EL1_LOC( _val ) ( ( _val ) << 24 )
#define AARCH64_CLIDR_EL1_LOC_SHIFT 24
#define AARCH64_CLIDR_EL1_LOC_MASK 0x7000000U
#define AARCH64_CLIDR_EL1_LOC_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0x7U )

#define AARCH64_CLIDR_EL1_LOUU( _val ) ( ( _val ) << 27 )
#define AARCH64_CLIDR_EL1_LOUU_SHIFT 27
#define AARCH64_CLIDR_EL1_LOUU_MASK 0x38000000U
#define AARCH64_CLIDR_EL1_LOUU_GET( _reg ) \
  ( ( ( _reg ) >> 27 ) & 0x7U )

#define AARCH64_CLIDR_EL1_ICB( _val ) ( ( _val ) << 30 )
#define AARCH64_CLIDR_EL1_ICB_SHIFT 30
#define AARCH64_CLIDR_EL1_ICB_MASK 0x1c0000000ULL
#define AARCH64_CLIDR_EL1_ICB_GET( _reg ) \
  ( ( ( _reg ) >> 30 ) & 0x7ULL )

static inline uint64_t _AArch64_Read_clidr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CLIDR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* CONTEXTIDR_EL1, Context ID Register (EL1) */

#define AARCH64_CONTEXTIDR_EL1_PROCID( _val ) ( ( _val ) << 0 )
#define AARCH64_CONTEXTIDR_EL1_PROCID_SHIFT 0
#define AARCH64_CONTEXTIDR_EL1_PROCID_MASK 0xffffffffU
#define AARCH64_CONTEXTIDR_EL1_PROCID_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )

static inline uint64_t _AArch64_Read_contextidr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CONTEXTIDR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_contextidr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr CONTEXTIDR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* CONTEXTIDR_EL2, Context ID Register (EL2) */

#define AARCH64_CONTEXTIDR_EL2_PROCID( _val ) ( ( _val ) << 0 )
#define AARCH64_CONTEXTIDR_EL2_PROCID_SHIFT 0
#define AARCH64_CONTEXTIDR_EL2_PROCID_MASK 0xffffffffU
#define AARCH64_CONTEXTIDR_EL2_PROCID_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )

static inline uint64_t _AArch64_Read_contextidr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CONTEXTIDR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_contextidr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr CONTEXTIDR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* CPACR_EL1, Architectural Feature Access Control Register */

#define AARCH64_CPACR_EL1_ZEN( _val ) ( ( _val ) << 16 )
#define AARCH64_CPACR_EL1_ZEN_SHIFT 16
#define AARCH64_CPACR_EL1_ZEN_MASK 0x30000U
#define AARCH64_CPACR_EL1_ZEN_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0x3U )

#define AARCH64_CPACR_EL1_FPEN( _val ) ( ( _val ) << 20 )
#define AARCH64_CPACR_EL1_FPEN_SHIFT 20
#define AARCH64_CPACR_EL1_FPEN_MASK 0x300000U
#define AARCH64_CPACR_EL1_FPEN_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0x3U )

#define AARCH64_CPACR_EL1_TTA 0x10000000U

static inline uint64_t _AArch64_Read_cpacr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CPACR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cpacr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr CPACR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* CPTR_EL2, Architectural Feature Trap Register (EL2) */

#define AARCH64_CPTR_EL2_TZ 0x100U

#define AARCH64_CPTR_EL2_TFP 0x400U

#define AARCH64_CPTR_EL2_ZEN( _val ) ( ( _val ) << 16 )
#define AARCH64_CPTR_EL2_ZEN_SHIFT 16
#define AARCH64_CPTR_EL2_ZEN_MASK 0x30000U
#define AARCH64_CPTR_EL2_ZEN_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0x3U )

#define AARCH64_CPTR_EL2_TTA_0 0x100000U

#define AARCH64_CPTR_EL2_FPEN( _val ) ( ( _val ) << 20 )
#define AARCH64_CPTR_EL2_FPEN_SHIFT 20
#define AARCH64_CPTR_EL2_FPEN_MASK 0x300000U
#define AARCH64_CPTR_EL2_FPEN_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0x3U )

#define AARCH64_CPTR_EL2_TTA_1 0x10000000U

#define AARCH64_CPTR_EL2_TAM 0x40000000U

#define AARCH64_CPTR_EL2_TCPAC 0x80000000U

static inline uint64_t _AArch64_Read_cptr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CPTR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cptr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr CPTR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* CPTR_EL3, Architectural Feature Trap Register (EL3) */

#define AARCH64_CPTR_EL3_EZ 0x100U

#define AARCH64_CPTR_EL3_TFP 0x400U

#define AARCH64_CPTR_EL3_TTA 0x100000U

#define AARCH64_CPTR_EL3_TAM 0x40000000U

#define AARCH64_CPTR_EL3_TCPAC 0x80000000U

static inline uint64_t _AArch64_Read_cptr_el3( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CPTR_EL3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cptr_el3( uint64_t value )
{
  __asm__ volatile (
    "msr CPTR_EL3, %0" : : "r" ( value ) : "memory"
  );
}

/* CSSELR_EL1, Cache Size Selection Register */

#define AARCH64_CSSELR_EL1_IND 0x1U

#define AARCH64_CSSELR_EL1_LEVEL( _val ) ( ( _val ) << 1 )
#define AARCH64_CSSELR_EL1_LEVEL_SHIFT 1
#define AARCH64_CSSELR_EL1_LEVEL_MASK 0xeU
#define AARCH64_CSSELR_EL1_LEVEL_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x7U )

#define AARCH64_CSSELR_EL1_TND 0x10U

static inline uint64_t _AArch64_Read_csselr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CSSELR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_csselr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr CSSELR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* CTR_EL0, Cache Type Register */

#define AARCH64_CTR_EL0_IMINLINE( _val ) ( ( _val ) << 0 )
#define AARCH64_CTR_EL0_IMINLINE_SHIFT 0
#define AARCH64_CTR_EL0_IMINLINE_MASK 0xfU
#define AARCH64_CTR_EL0_IMINLINE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_CTR_EL0_L1IP( _val ) ( ( _val ) << 14 )
#define AARCH64_CTR_EL0_L1IP_SHIFT 14
#define AARCH64_CTR_EL0_L1IP_MASK 0xc000U
#define AARCH64_CTR_EL0_L1IP_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x3U )

#define AARCH64_CTR_EL0_DMINLINE( _val ) ( ( _val ) << 16 )
#define AARCH64_CTR_EL0_DMINLINE_SHIFT 16
#define AARCH64_CTR_EL0_DMINLINE_MASK 0xf0000U
#define AARCH64_CTR_EL0_DMINLINE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_CTR_EL0_ERG( _val ) ( ( _val ) << 20 )
#define AARCH64_CTR_EL0_ERG_SHIFT 20
#define AARCH64_CTR_EL0_ERG_MASK 0xf00000U
#define AARCH64_CTR_EL0_ERG_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_CTR_EL0_CWG( _val ) ( ( _val ) << 24 )
#define AARCH64_CTR_EL0_CWG_SHIFT 24
#define AARCH64_CTR_EL0_CWG_MASK 0xf000000U
#define AARCH64_CTR_EL0_CWG_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH64_CTR_EL0_IDC 0x10000000U

#define AARCH64_CTR_EL0_DIC 0x20000000U

#define AARCH64_CTR_EL0_TMINLINE( _val ) ( ( _val ) << 32 )
#define AARCH64_CTR_EL0_TMINLINE_SHIFT 32
#define AARCH64_CTR_EL0_TMINLINE_MASK 0x3f00000000ULL
#define AARCH64_CTR_EL0_TMINLINE_GET( _reg ) \
  ( ( ( _reg ) >> 32 ) & 0x3fULL )

static inline uint64_t _AArch64_Read_ctr_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CTR_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* DACR32_EL2, Domain Access Control Register */

static inline uint64_t _AArch64_Read_dacr32_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DACR32_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dacr32_el2( uint64_t value )
{
  __asm__ volatile (
    "msr DACR32_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* DCZID_EL0, Data Cache Zero ID Register */

#define AARCH64_DCZID_EL0_BS( _val ) ( ( _val ) << 0 )
#define AARCH64_DCZID_EL0_BS_SHIFT 0
#define AARCH64_DCZID_EL0_BS_MASK 0xfU
#define AARCH64_DCZID_EL0_BS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_DCZID_EL0_DZP 0x10U

static inline uint64_t _AArch64_Read_dczid_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DCZID_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ESR_EL1, Exception Syndrome Register (EL1) */

#define AARCH64_ESR_EL1_DIRECTION 0x1U

#define AARCH64_ESR_EL1_ERETA 0x1U

#define AARCH64_ESR_EL1_IOF 0x1U

#define AARCH64_ESR_EL1_TI 0x1U

#define AARCH64_ESR_EL1_BTYPE( _val ) ( ( _val ) << 0 )
#define AARCH64_ESR_EL1_BTYPE_SHIFT 0
#define AARCH64_ESR_EL1_BTYPE_MASK 0x3U
#define AARCH64_ESR_EL1_BTYPE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3U )

#define AARCH64_ESR_EL1_DFSC( _val ) ( ( _val ) << 0 )
#define AARCH64_ESR_EL1_DFSC_SHIFT 0
#define AARCH64_ESR_EL1_DFSC_MASK 0x3fU
#define AARCH64_ESR_EL1_DFSC_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )

#define AARCH64_ESR_EL1_IFSC( _val ) ( ( _val ) << 0 )
#define AARCH64_ESR_EL1_IFSC_SHIFT 0
#define AARCH64_ESR_EL1_IFSC_MASK 0x3fU
#define AARCH64_ESR_EL1_IFSC_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )

#define AARCH64_ESR_EL1_COMMENT( _val ) ( ( _val ) << 0 )
#define AARCH64_ESR_EL1_COMMENT_SHIFT 0
#define AARCH64_ESR_EL1_COMMENT_MASK 0xffffU
#define AARCH64_ESR_EL1_COMMENT_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )

#define AARCH64_ESR_EL1_IMM16( _val ) ( ( _val ) << 0 )
#define AARCH64_ESR_EL1_IMM16_SHIFT 0
#define AARCH64_ESR_EL1_IMM16_MASK 0xffffU
#define AARCH64_ESR_EL1_IMM16_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )

#define AARCH64_ESR_EL1_ISS( _val ) ( ( _val ) << 0 )
#define AARCH64_ESR_EL1_ISS_SHIFT 0
#define AARCH64_ESR_EL1_ISS_MASK 0x1ffffffU
#define AARCH64_ESR_EL1_ISS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1ffffffU )

#define AARCH64_ESR_EL1_DZF 0x2U

#define AARCH64_ESR_EL1_ERET 0x2U

#define AARCH64_ESR_EL1_AM( _val ) ( ( _val ) << 1 )
#define AARCH64_ESR_EL1_AM_SHIFT 1
#define AARCH64_ESR_EL1_AM_MASK 0xeU
#define AARCH64_ESR_EL1_AM_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x7U )

#define AARCH64_ESR_EL1_CRM( _val ) ( ( _val ) << 1 )
#define AARCH64_ESR_EL1_CRM_SHIFT 1
#define AARCH64_ESR_EL1_CRM_MASK 0x1eU
#define AARCH64_ESR_EL1_CRM_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0xfU )

#define AARCH64_ESR_EL1_OFF 0x4U

#define AARCH64_ESR_EL1_UFF 0x8U

#define AARCH64_ESR_EL1_IXF 0x10U

#define AARCH64_ESR_EL1_OFFSET 0x10U

#define AARCH64_ESR_EL1_RN( _val ) ( ( _val ) << 5 )
#define AARCH64_ESR_EL1_RN_SHIFT 5
#define AARCH64_ESR_EL1_RN_MASK 0x3e0U
#define AARCH64_ESR_EL1_RN_GET( _reg ) \
  ( ( ( _reg ) >> 5 ) & 0x1fU )

#define AARCH64_ESR_EL1_RT( _val ) ( ( _val ) << 5 )
#define AARCH64_ESR_EL1_RT_SHIFT 5
#define AARCH64_ESR_EL1_RT_MASK 0x3e0U
#define AARCH64_ESR_EL1_RT_GET( _reg ) \
  ( ( ( _reg ) >> 5 ) & 0x1fU )

#define AARCH64_ESR_EL1_EX 0x40U

#define AARCH64_ESR_EL1_WNR 0x40U

#define AARCH64_ESR_EL1_IDF 0x80U

#define AARCH64_ESR_EL1_S1PTW 0x80U

#define AARCH64_ESR_EL1_CM 0x100U

#define AARCH64_ESR_EL1_VECITR( _val ) ( ( _val ) << 8 )
#define AARCH64_ESR_EL1_VECITR_SHIFT 8
#define AARCH64_ESR_EL1_VECITR_MASK 0x700U
#define AARCH64_ESR_EL1_VECITR_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0x7U )

#define AARCH64_ESR_EL1_EA 0x200U

#define AARCH64_ESR_EL1_FNV 0x400U

#define AARCH64_ESR_EL1_AET( _val ) ( ( _val ) << 10 )
#define AARCH64_ESR_EL1_AET_SHIFT 10
#define AARCH64_ESR_EL1_AET_MASK 0x1c00U
#define AARCH64_ESR_EL1_AET_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x7U )

#define AARCH64_ESR_EL1_CRN( _val ) ( ( _val ) << 10 )
#define AARCH64_ESR_EL1_CRN_SHIFT 10
#define AARCH64_ESR_EL1_CRN_MASK 0x3c00U
#define AARCH64_ESR_EL1_CRN_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0xfU )

#define AARCH64_ESR_EL1_RT2( _val ) ( ( _val ) << 10 )
#define AARCH64_ESR_EL1_RT2_SHIFT 10
#define AARCH64_ESR_EL1_RT2_MASK 0x7c00U
#define AARCH64_ESR_EL1_RT2_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x1fU )

#define AARCH64_ESR_EL1_SET( _val ) ( ( _val ) << 11 )
#define AARCH64_ESR_EL1_SET_SHIFT 11
#define AARCH64_ESR_EL1_SET_MASK 0x1800U
#define AARCH64_ESR_EL1_SET_GET( _reg ) \
  ( ( ( _reg ) >> 11 ) & 0x3U )

#define AARCH64_ESR_EL1_IMM8( _val ) ( ( _val ) << 12 )
#define AARCH64_ESR_EL1_IMM8_SHIFT 12
#define AARCH64_ESR_EL1_IMM8_MASK 0xff000U
#define AARCH64_ESR_EL1_IMM8_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xffU )

#define AARCH64_ESR_EL1_IESB 0x2000U

#define AARCH64_ESR_EL1_VNCR 0x2000U

#define AARCH64_ESR_EL1_AR 0x4000U

#define AARCH64_ESR_EL1_OP1( _val ) ( ( _val ) << 14 )
#define AARCH64_ESR_EL1_OP1_SHIFT 14
#define AARCH64_ESR_EL1_OP1_MASK 0x1c000U
#define AARCH64_ESR_EL1_OP1_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x7U )

#define AARCH64_ESR_EL1_OPC1_0( _val ) ( ( _val ) << 14 )
#define AARCH64_ESR_EL1_OPC1_SHIFT_0 14
#define AARCH64_ESR_EL1_OPC1_MASK_0 0x1c000U
#define AARCH64_ESR_EL1_OPC1_GET_0( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x7U )

#define AARCH64_ESR_EL1_SF 0x8000U

#define AARCH64_ESR_EL1_OPC1_1( _val ) ( ( _val ) << 16 )
#define AARCH64_ESR_EL1_OPC1_SHIFT_1 16
#define AARCH64_ESR_EL1_OPC1_MASK_1 0xf0000U
#define AARCH64_ESR_EL1_OPC1_GET_1( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ESR_EL1_SRT( _val ) ( ( _val ) << 16 )
#define AARCH64_ESR_EL1_SRT_SHIFT 16
#define AARCH64_ESR_EL1_SRT_MASK 0x1f0000U
#define AARCH64_ESR_EL1_SRT_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0x1fU )

#define AARCH64_ESR_EL1_OP2( _val ) ( ( _val ) << 17 )
#define AARCH64_ESR_EL1_OP2_SHIFT 17
#define AARCH64_ESR_EL1_OP2_MASK 0xe0000U
#define AARCH64_ESR_EL1_OP2_GET( _reg ) \
  ( ( ( _reg ) >> 17 ) & 0x7U )

#define AARCH64_ESR_EL1_OPC2( _val ) ( ( _val ) << 17 )
#define AARCH64_ESR_EL1_OPC2_SHIFT 17
#define AARCH64_ESR_EL1_OPC2_MASK 0xe0000U
#define AARCH64_ESR_EL1_OPC2_GET( _reg ) \
  ( ( ( _reg ) >> 17 ) & 0x7U )

#define AARCH64_ESR_EL1_CCKNOWNPASS 0x80000U

#define AARCH64_ESR_EL1_OP0( _val ) ( ( _val ) << 20 )
#define AARCH64_ESR_EL1_OP0_SHIFT 20
#define AARCH64_ESR_EL1_OP0_MASK 0x300000U
#define AARCH64_ESR_EL1_OP0_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0x3U )

#define AARCH64_ESR_EL1_COND( _val ) ( ( _val ) << 20 )
#define AARCH64_ESR_EL1_COND_SHIFT 20
#define AARCH64_ESR_EL1_COND_MASK 0xf00000U
#define AARCH64_ESR_EL1_COND_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ESR_EL1_SSE 0x200000U

#define AARCH64_ESR_EL1_SAS( _val ) ( ( _val ) << 22 )
#define AARCH64_ESR_EL1_SAS_SHIFT 22
#define AARCH64_ESR_EL1_SAS_MASK 0xc00000U
#define AARCH64_ESR_EL1_SAS_GET( _reg ) \
  ( ( ( _reg ) >> 22 ) & 0x3U )

#define AARCH64_ESR_EL1_TFV 0x800000U

#define AARCH64_ESR_EL1_CV 0x1000000U

#define AARCH64_ESR_EL1_IDS 0x1000000U

#define AARCH64_ESR_EL1_ISV 0x1000000U

#define AARCH64_ESR_EL1_IL 0x2000000U

#define AARCH64_ESR_EL1_EC( _val ) ( ( _val ) << 26 )
#define AARCH64_ESR_EL1_EC_SHIFT 26
#define AARCH64_ESR_EL1_EC_MASK 0xfc000000U
#define AARCH64_ESR_EL1_EC_GET( _reg ) \
  ( ( ( _reg ) >> 26 ) & 0x3fU )

static inline uint64_t _AArch64_Read_esr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ESR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_esr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr ESR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* ESR_EL2, Exception Syndrome Register (EL2) */

#define AARCH64_ESR_EL2_DIRECTION 0x1U

#define AARCH64_ESR_EL2_ERETA 0x1U

#define AARCH64_ESR_EL2_IOF 0x1U

#define AARCH64_ESR_EL2_TI 0x1U

#define AARCH64_ESR_EL2_BTYPE( _val ) ( ( _val ) << 0 )
#define AARCH64_ESR_EL2_BTYPE_SHIFT 0
#define AARCH64_ESR_EL2_BTYPE_MASK 0x3U
#define AARCH64_ESR_EL2_BTYPE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3U )

#define AARCH64_ESR_EL2_DFSC( _val ) ( ( _val ) << 0 )
#define AARCH64_ESR_EL2_DFSC_SHIFT 0
#define AARCH64_ESR_EL2_DFSC_MASK 0x3fU
#define AARCH64_ESR_EL2_DFSC_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )

#define AARCH64_ESR_EL2_IFSC( _val ) ( ( _val ) << 0 )
#define AARCH64_ESR_EL2_IFSC_SHIFT 0
#define AARCH64_ESR_EL2_IFSC_MASK 0x3fU
#define AARCH64_ESR_EL2_IFSC_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )

#define AARCH64_ESR_EL2_COMMENT( _val ) ( ( _val ) << 0 )
#define AARCH64_ESR_EL2_COMMENT_SHIFT 0
#define AARCH64_ESR_EL2_COMMENT_MASK 0xffffU
#define AARCH64_ESR_EL2_COMMENT_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )

#define AARCH64_ESR_EL2_IMM16( _val ) ( ( _val ) << 0 )
#define AARCH64_ESR_EL2_IMM16_SHIFT 0
#define AARCH64_ESR_EL2_IMM16_MASK 0xffffU
#define AARCH64_ESR_EL2_IMM16_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )

#define AARCH64_ESR_EL2_ISS( _val ) ( ( _val ) << 0 )
#define AARCH64_ESR_EL2_ISS_SHIFT 0
#define AARCH64_ESR_EL2_ISS_MASK 0x1ffffffU
#define AARCH64_ESR_EL2_ISS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1ffffffU )

#define AARCH64_ESR_EL2_DZF 0x2U

#define AARCH64_ESR_EL2_ERET 0x2U

#define AARCH64_ESR_EL2_AM( _val ) ( ( _val ) << 1 )
#define AARCH64_ESR_EL2_AM_SHIFT 1
#define AARCH64_ESR_EL2_AM_MASK 0xeU
#define AARCH64_ESR_EL2_AM_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x7U )

#define AARCH64_ESR_EL2_CRM( _val ) ( ( _val ) << 1 )
#define AARCH64_ESR_EL2_CRM_SHIFT 1
#define AARCH64_ESR_EL2_CRM_MASK 0x1eU
#define AARCH64_ESR_EL2_CRM_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0xfU )

#define AARCH64_ESR_EL2_OFF 0x4U

#define AARCH64_ESR_EL2_UFF 0x8U

#define AARCH64_ESR_EL2_IXF 0x10U

#define AARCH64_ESR_EL2_OFFSET 0x10U

#define AARCH64_ESR_EL2_RN( _val ) ( ( _val ) << 5 )
#define AARCH64_ESR_EL2_RN_SHIFT 5
#define AARCH64_ESR_EL2_RN_MASK 0x3e0U
#define AARCH64_ESR_EL2_RN_GET( _reg ) \
  ( ( ( _reg ) >> 5 ) & 0x1fU )

#define AARCH64_ESR_EL2_RT( _val ) ( ( _val ) << 5 )
#define AARCH64_ESR_EL2_RT_SHIFT 5
#define AARCH64_ESR_EL2_RT_MASK 0x3e0U
#define AARCH64_ESR_EL2_RT_GET( _reg ) \
  ( ( ( _reg ) >> 5 ) & 0x1fU )

#define AARCH64_ESR_EL2_EX 0x40U

#define AARCH64_ESR_EL2_WNR 0x40U

#define AARCH64_ESR_EL2_IDF 0x80U

#define AARCH64_ESR_EL2_S1PTW 0x80U

#define AARCH64_ESR_EL2_CM 0x100U

#define AARCH64_ESR_EL2_VECITR( _val ) ( ( _val ) << 8 )
#define AARCH64_ESR_EL2_VECITR_SHIFT 8
#define AARCH64_ESR_EL2_VECITR_MASK 0x700U
#define AARCH64_ESR_EL2_VECITR_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0x7U )

#define AARCH64_ESR_EL2_EA 0x200U

#define AARCH64_ESR_EL2_FNV 0x400U

#define AARCH64_ESR_EL2_AET( _val ) ( ( _val ) << 10 )
#define AARCH64_ESR_EL2_AET_SHIFT 10
#define AARCH64_ESR_EL2_AET_MASK 0x1c00U
#define AARCH64_ESR_EL2_AET_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x7U )

#define AARCH64_ESR_EL2_CRN( _val ) ( ( _val ) << 10 )
#define AARCH64_ESR_EL2_CRN_SHIFT 10
#define AARCH64_ESR_EL2_CRN_MASK 0x3c00U
#define AARCH64_ESR_EL2_CRN_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0xfU )

#define AARCH64_ESR_EL2_RT2( _val ) ( ( _val ) << 10 )
#define AARCH64_ESR_EL2_RT2_SHIFT 10
#define AARCH64_ESR_EL2_RT2_MASK 0x7c00U
#define AARCH64_ESR_EL2_RT2_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x1fU )

#define AARCH64_ESR_EL2_SET( _val ) ( ( _val ) << 11 )
#define AARCH64_ESR_EL2_SET_SHIFT 11
#define AARCH64_ESR_EL2_SET_MASK 0x1800U
#define AARCH64_ESR_EL2_SET_GET( _reg ) \
  ( ( ( _reg ) >> 11 ) & 0x3U )

#define AARCH64_ESR_EL2_IMM8( _val ) ( ( _val ) << 12 )
#define AARCH64_ESR_EL2_IMM8_SHIFT 12
#define AARCH64_ESR_EL2_IMM8_MASK 0xff000U
#define AARCH64_ESR_EL2_IMM8_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xffU )

#define AARCH64_ESR_EL2_IESB 0x2000U

#define AARCH64_ESR_EL2_VNCR 0x2000U

#define AARCH64_ESR_EL2_AR 0x4000U

#define AARCH64_ESR_EL2_OP1( _val ) ( ( _val ) << 14 )
#define AARCH64_ESR_EL2_OP1_SHIFT 14
#define AARCH64_ESR_EL2_OP1_MASK 0x1c000U
#define AARCH64_ESR_EL2_OP1_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x7U )

#define AARCH64_ESR_EL2_OPC1_0( _val ) ( ( _val ) << 14 )
#define AARCH64_ESR_EL2_OPC1_SHIFT_0 14
#define AARCH64_ESR_EL2_OPC1_MASK_0 0x1c000U
#define AARCH64_ESR_EL2_OPC1_GET_0( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x7U )

#define AARCH64_ESR_EL2_SF 0x8000U

#define AARCH64_ESR_EL2_OPC1_1( _val ) ( ( _val ) << 16 )
#define AARCH64_ESR_EL2_OPC1_SHIFT_1 16
#define AARCH64_ESR_EL2_OPC1_MASK_1 0xf0000U
#define AARCH64_ESR_EL2_OPC1_GET_1( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ESR_EL2_SRT( _val ) ( ( _val ) << 16 )
#define AARCH64_ESR_EL2_SRT_SHIFT 16
#define AARCH64_ESR_EL2_SRT_MASK 0x1f0000U
#define AARCH64_ESR_EL2_SRT_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0x1fU )

#define AARCH64_ESR_EL2_OP2( _val ) ( ( _val ) << 17 )
#define AARCH64_ESR_EL2_OP2_SHIFT 17
#define AARCH64_ESR_EL2_OP2_MASK 0xe0000U
#define AARCH64_ESR_EL2_OP2_GET( _reg ) \
  ( ( ( _reg ) >> 17 ) & 0x7U )

#define AARCH64_ESR_EL2_OPC2( _val ) ( ( _val ) << 17 )
#define AARCH64_ESR_EL2_OPC2_SHIFT 17
#define AARCH64_ESR_EL2_OPC2_MASK 0xe0000U
#define AARCH64_ESR_EL2_OPC2_GET( _reg ) \
  ( ( ( _reg ) >> 17 ) & 0x7U )

#define AARCH64_ESR_EL2_CCKNOWNPASS 0x80000U

#define AARCH64_ESR_EL2_OP0( _val ) ( ( _val ) << 20 )
#define AARCH64_ESR_EL2_OP0_SHIFT 20
#define AARCH64_ESR_EL2_OP0_MASK 0x300000U
#define AARCH64_ESR_EL2_OP0_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0x3U )

#define AARCH64_ESR_EL2_COND( _val ) ( ( _val ) << 20 )
#define AARCH64_ESR_EL2_COND_SHIFT 20
#define AARCH64_ESR_EL2_COND_MASK 0xf00000U
#define AARCH64_ESR_EL2_COND_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ESR_EL2_SSE 0x200000U

#define AARCH64_ESR_EL2_SAS( _val ) ( ( _val ) << 22 )
#define AARCH64_ESR_EL2_SAS_SHIFT 22
#define AARCH64_ESR_EL2_SAS_MASK 0xc00000U
#define AARCH64_ESR_EL2_SAS_GET( _reg ) \
  ( ( ( _reg ) >> 22 ) & 0x3U )

#define AARCH64_ESR_EL2_TFV 0x800000U

#define AARCH64_ESR_EL2_CV 0x1000000U

#define AARCH64_ESR_EL2_IDS 0x1000000U

#define AARCH64_ESR_EL2_ISV 0x1000000U

#define AARCH64_ESR_EL2_IL 0x2000000U

#define AARCH64_ESR_EL2_EC( _val ) ( ( _val ) << 26 )
#define AARCH64_ESR_EL2_EC_SHIFT 26
#define AARCH64_ESR_EL2_EC_MASK 0xfc000000U
#define AARCH64_ESR_EL2_EC_GET( _reg ) \
  ( ( ( _reg ) >> 26 ) & 0x3fU )

static inline uint64_t _AArch64_Read_esr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ESR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_esr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr ESR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* ESR_EL3, Exception Syndrome Register (EL3) */

#define AARCH64_ESR_EL3_DIRECTION 0x1U

#define AARCH64_ESR_EL3_ERETA 0x1U

#define AARCH64_ESR_EL3_IOF 0x1U

#define AARCH64_ESR_EL3_TI 0x1U

#define AARCH64_ESR_EL3_BTYPE( _val ) ( ( _val ) << 0 )
#define AARCH64_ESR_EL3_BTYPE_SHIFT 0
#define AARCH64_ESR_EL3_BTYPE_MASK 0x3U
#define AARCH64_ESR_EL3_BTYPE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3U )

#define AARCH64_ESR_EL3_DFSC( _val ) ( ( _val ) << 0 )
#define AARCH64_ESR_EL3_DFSC_SHIFT 0
#define AARCH64_ESR_EL3_DFSC_MASK 0x3fU
#define AARCH64_ESR_EL3_DFSC_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )

#define AARCH64_ESR_EL3_IFSC( _val ) ( ( _val ) << 0 )
#define AARCH64_ESR_EL3_IFSC_SHIFT 0
#define AARCH64_ESR_EL3_IFSC_MASK 0x3fU
#define AARCH64_ESR_EL3_IFSC_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )

#define AARCH64_ESR_EL3_COMMENT( _val ) ( ( _val ) << 0 )
#define AARCH64_ESR_EL3_COMMENT_SHIFT 0
#define AARCH64_ESR_EL3_COMMENT_MASK 0xffffU
#define AARCH64_ESR_EL3_COMMENT_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )

#define AARCH64_ESR_EL3_IMM16( _val ) ( ( _val ) << 0 )
#define AARCH64_ESR_EL3_IMM16_SHIFT 0
#define AARCH64_ESR_EL3_IMM16_MASK 0xffffU
#define AARCH64_ESR_EL3_IMM16_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )

#define AARCH64_ESR_EL3_ISS( _val ) ( ( _val ) << 0 )
#define AARCH64_ESR_EL3_ISS_SHIFT 0
#define AARCH64_ESR_EL3_ISS_MASK 0x1ffffffU
#define AARCH64_ESR_EL3_ISS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1ffffffU )

#define AARCH64_ESR_EL3_DZF 0x2U

#define AARCH64_ESR_EL3_ERET 0x2U

#define AARCH64_ESR_EL3_AM( _val ) ( ( _val ) << 1 )
#define AARCH64_ESR_EL3_AM_SHIFT 1
#define AARCH64_ESR_EL3_AM_MASK 0xeU
#define AARCH64_ESR_EL3_AM_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x7U )

#define AARCH64_ESR_EL3_CRM( _val ) ( ( _val ) << 1 )
#define AARCH64_ESR_EL3_CRM_SHIFT 1
#define AARCH64_ESR_EL3_CRM_MASK 0x1eU
#define AARCH64_ESR_EL3_CRM_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0xfU )

#define AARCH64_ESR_EL3_OFF 0x4U

#define AARCH64_ESR_EL3_UFF 0x8U

#define AARCH64_ESR_EL3_IXF 0x10U

#define AARCH64_ESR_EL3_OFFSET 0x10U

#define AARCH64_ESR_EL3_RN( _val ) ( ( _val ) << 5 )
#define AARCH64_ESR_EL3_RN_SHIFT 5
#define AARCH64_ESR_EL3_RN_MASK 0x3e0U
#define AARCH64_ESR_EL3_RN_GET( _reg ) \
  ( ( ( _reg ) >> 5 ) & 0x1fU )

#define AARCH64_ESR_EL3_RT( _val ) ( ( _val ) << 5 )
#define AARCH64_ESR_EL3_RT_SHIFT 5
#define AARCH64_ESR_EL3_RT_MASK 0x3e0U
#define AARCH64_ESR_EL3_RT_GET( _reg ) \
  ( ( ( _reg ) >> 5 ) & 0x1fU )

#define AARCH64_ESR_EL3_EX 0x40U

#define AARCH64_ESR_EL3_WNR 0x40U

#define AARCH64_ESR_EL3_IDF 0x80U

#define AARCH64_ESR_EL3_S1PTW 0x80U

#define AARCH64_ESR_EL3_CM 0x100U

#define AARCH64_ESR_EL3_VECITR( _val ) ( ( _val ) << 8 )
#define AARCH64_ESR_EL3_VECITR_SHIFT 8
#define AARCH64_ESR_EL3_VECITR_MASK 0x700U
#define AARCH64_ESR_EL3_VECITR_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0x7U )

#define AARCH64_ESR_EL3_EA 0x200U

#define AARCH64_ESR_EL3_FNV 0x400U

#define AARCH64_ESR_EL3_AET( _val ) ( ( _val ) << 10 )
#define AARCH64_ESR_EL3_AET_SHIFT 10
#define AARCH64_ESR_EL3_AET_MASK 0x1c00U
#define AARCH64_ESR_EL3_AET_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x7U )

#define AARCH64_ESR_EL3_CRN( _val ) ( ( _val ) << 10 )
#define AARCH64_ESR_EL3_CRN_SHIFT 10
#define AARCH64_ESR_EL3_CRN_MASK 0x3c00U
#define AARCH64_ESR_EL3_CRN_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0xfU )

#define AARCH64_ESR_EL3_RT2( _val ) ( ( _val ) << 10 )
#define AARCH64_ESR_EL3_RT2_SHIFT 10
#define AARCH64_ESR_EL3_RT2_MASK 0x7c00U
#define AARCH64_ESR_EL3_RT2_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x1fU )

#define AARCH64_ESR_EL3_SET( _val ) ( ( _val ) << 11 )
#define AARCH64_ESR_EL3_SET_SHIFT 11
#define AARCH64_ESR_EL3_SET_MASK 0x1800U
#define AARCH64_ESR_EL3_SET_GET( _reg ) \
  ( ( ( _reg ) >> 11 ) & 0x3U )

#define AARCH64_ESR_EL3_IMM8( _val ) ( ( _val ) << 12 )
#define AARCH64_ESR_EL3_IMM8_SHIFT 12
#define AARCH64_ESR_EL3_IMM8_MASK 0xff000U
#define AARCH64_ESR_EL3_IMM8_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xffU )

#define AARCH64_ESR_EL3_IESB 0x2000U

#define AARCH64_ESR_EL3_VNCR 0x2000U

#define AARCH64_ESR_EL3_AR 0x4000U

#define AARCH64_ESR_EL3_OP1( _val ) ( ( _val ) << 14 )
#define AARCH64_ESR_EL3_OP1_SHIFT 14
#define AARCH64_ESR_EL3_OP1_MASK 0x1c000U
#define AARCH64_ESR_EL3_OP1_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x7U )

#define AARCH64_ESR_EL3_OPC1_0( _val ) ( ( _val ) << 14 )
#define AARCH64_ESR_EL3_OPC1_SHIFT_0 14
#define AARCH64_ESR_EL3_OPC1_MASK_0 0x1c000U
#define AARCH64_ESR_EL3_OPC1_GET_0( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x7U )

#define AARCH64_ESR_EL3_SF 0x8000U

#define AARCH64_ESR_EL3_OPC1_1( _val ) ( ( _val ) << 16 )
#define AARCH64_ESR_EL3_OPC1_SHIFT_1 16
#define AARCH64_ESR_EL3_OPC1_MASK_1 0xf0000U
#define AARCH64_ESR_EL3_OPC1_GET_1( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ESR_EL3_SRT( _val ) ( ( _val ) << 16 )
#define AARCH64_ESR_EL3_SRT_SHIFT 16
#define AARCH64_ESR_EL3_SRT_MASK 0x1f0000U
#define AARCH64_ESR_EL3_SRT_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0x1fU )

#define AARCH64_ESR_EL3_OP2( _val ) ( ( _val ) << 17 )
#define AARCH64_ESR_EL3_OP2_SHIFT 17
#define AARCH64_ESR_EL3_OP2_MASK 0xe0000U
#define AARCH64_ESR_EL3_OP2_GET( _reg ) \
  ( ( ( _reg ) >> 17 ) & 0x7U )

#define AARCH64_ESR_EL3_OPC2( _val ) ( ( _val ) << 17 )
#define AARCH64_ESR_EL3_OPC2_SHIFT 17
#define AARCH64_ESR_EL3_OPC2_MASK 0xe0000U
#define AARCH64_ESR_EL3_OPC2_GET( _reg ) \
  ( ( ( _reg ) >> 17 ) & 0x7U )

#define AARCH64_ESR_EL3_CCKNOWNPASS 0x80000U

#define AARCH64_ESR_EL3_OP0( _val ) ( ( _val ) << 20 )
#define AARCH64_ESR_EL3_OP0_SHIFT 20
#define AARCH64_ESR_EL3_OP0_MASK 0x300000U
#define AARCH64_ESR_EL3_OP0_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0x3U )

#define AARCH64_ESR_EL3_COND( _val ) ( ( _val ) << 20 )
#define AARCH64_ESR_EL3_COND_SHIFT 20
#define AARCH64_ESR_EL3_COND_MASK 0xf00000U
#define AARCH64_ESR_EL3_COND_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ESR_EL3_SSE 0x200000U

#define AARCH64_ESR_EL3_SAS( _val ) ( ( _val ) << 22 )
#define AARCH64_ESR_EL3_SAS_SHIFT 22
#define AARCH64_ESR_EL3_SAS_MASK 0xc00000U
#define AARCH64_ESR_EL3_SAS_GET( _reg ) \
  ( ( ( _reg ) >> 22 ) & 0x3U )

#define AARCH64_ESR_EL3_TFV 0x800000U

#define AARCH64_ESR_EL3_CV 0x1000000U

#define AARCH64_ESR_EL3_IDS 0x1000000U

#define AARCH64_ESR_EL3_ISV 0x1000000U

#define AARCH64_ESR_EL3_IL 0x2000000U

#define AARCH64_ESR_EL3_EC( _val ) ( ( _val ) << 26 )
#define AARCH64_ESR_EL3_EC_SHIFT 26
#define AARCH64_ESR_EL3_EC_MASK 0xfc000000U
#define AARCH64_ESR_EL3_EC_GET( _reg ) \
  ( ( ( _reg ) >> 26 ) & 0x3fU )

static inline uint64_t _AArch64_Read_esr_el3( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ESR_EL3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_esr_el3( uint64_t value )
{
  __asm__ volatile (
    "msr ESR_EL3, %0" : : "r" ( value ) : "memory"
  );
}

/* FAR_EL1, Fault Address Register (EL1) */

static inline uint64_t _AArch64_Read_far_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, FAR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_far_el1( uint64_t value )
{
  __asm__ volatile (
    "msr FAR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* FAR_EL2, Fault Address Register (EL2) */

static inline uint64_t _AArch64_Read_far_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, FAR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_far_el2( uint64_t value )
{
  __asm__ volatile (
    "msr FAR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* FAR_EL3, Fault Address Register (EL3) */

static inline uint64_t _AArch64_Read_far_el3( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, FAR_EL3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_far_el3( uint64_t value )
{
  __asm__ volatile (
    "msr FAR_EL3, %0" : : "r" ( value ) : "memory"
  );
}

/* FPEXC32_EL2, Floating-Point Exception Control Register */

#define AARCH64_FPEXC32_EL2_IOF 0x1U

#define AARCH64_FPEXC32_EL2_DZF 0x2U

#define AARCH64_FPEXC32_EL2_OFF 0x4U

#define AARCH64_FPEXC32_EL2_UFF 0x8U

#define AARCH64_FPEXC32_EL2_IXF 0x10U

#define AARCH64_FPEXC32_EL2_IDF 0x80U

#define AARCH64_FPEXC32_EL2_VECITR( _val ) ( ( _val ) << 8 )
#define AARCH64_FPEXC32_EL2_VECITR_SHIFT 8
#define AARCH64_FPEXC32_EL2_VECITR_MASK 0x700U
#define AARCH64_FPEXC32_EL2_VECITR_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0x7U )

#define AARCH64_FPEXC32_EL2_TFV 0x4000000U

#define AARCH64_FPEXC32_EL2_VV 0x8000000U

#define AARCH64_FPEXC32_EL2_FP2V 0x10000000U

#define AARCH64_FPEXC32_EL2_DEX 0x20000000U

#define AARCH64_FPEXC32_EL2_EN 0x40000000U

#define AARCH64_FPEXC32_EL2_EX 0x80000000U

static inline uint64_t _AArch64_Read_fpexc32_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, FPEXC32_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_fpexc32_el2( uint64_t value )
{
  __asm__ volatile (
    "msr FPEXC32_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* GCR_EL1, Tag Control Register. */

#define AARCH64_GCR_EL1_EXCLUDE( _val ) ( ( _val ) << 0 )
#define AARCH64_GCR_EL1_EXCLUDE_SHIFT 0
#define AARCH64_GCR_EL1_EXCLUDE_MASK 0xffffU
#define AARCH64_GCR_EL1_EXCLUDE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )

#define AARCH64_GCR_EL1_RRND 0x10000U

static inline uint64_t _AArch64_Read_gcr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, GCR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_gcr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr GCR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* GMID_EL1, Multiple tag transfer ID Register */

#define AARCH64_GMID_EL1_BS( _val ) ( ( _val ) << 0 )
#define AARCH64_GMID_EL1_BS_SHIFT 0
#define AARCH64_GMID_EL1_BS_MASK 0xfU
#define AARCH64_GMID_EL1_BS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

static inline uint64_t _AArch64_Read_gmid_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, GMID_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* HACR_EL2, Hypervisor Auxiliary Control Register */

static inline uint64_t _AArch64_Read_hacr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, HACR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_hacr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr HACR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* HAFGRTR_EL2, Hypervisor Activity Monitors Fine-Grained Read Trap Register */

#define AARCH64_HAFGRTR_EL2_AMCNTEN0 0x1U

#define AARCH64_HAFGRTR_EL2_AMCNTEN1 0x20000U

#define AARCH64_HAFGRTR_EL2_AMEVCNTR10_EL0 0x40000U

#define AARCH64_HAFGRTR_EL2_AMEVTYPER10_EL0 0x80000U

#define AARCH64_HAFGRTR_EL2_AMEVCNTR11_EL0 0x100000U

#define AARCH64_HAFGRTR_EL2_AMEVTYPER11_EL0 0x200000U

#define AARCH64_HAFGRTR_EL2_AMEVCNTR12_EL0 0x400000U

#define AARCH64_HAFGRTR_EL2_AMEVTYPER12_EL0 0x800000U

#define AARCH64_HAFGRTR_EL2_AMEVCNTR13_EL0 0x1000000U

#define AARCH64_HAFGRTR_EL2_AMEVTYPER13_EL0 0x2000000U

#define AARCH64_HAFGRTR_EL2_AMEVCNTR14_EL0 0x4000000U

#define AARCH64_HAFGRTR_EL2_AMEVTYPER14_EL0 0x8000000U

#define AARCH64_HAFGRTR_EL2_AMEVCNTR15_EL0 0x10000000U

#define AARCH64_HAFGRTR_EL2_AMEVTYPER15_EL0 0x20000000U

#define AARCH64_HAFGRTR_EL2_AMEVCNTR16_EL0 0x40000000U

#define AARCH64_HAFGRTR_EL2_AMEVTYPER16_EL0 0x80000000U

#define AARCH64_HAFGRTR_EL2_AMEVCNTR17_EL0 0x100000000ULL

#define AARCH64_HAFGRTR_EL2_AMEVTYPER17_EL0 0x200000000ULL

#define AARCH64_HAFGRTR_EL2_AMEVCNTR18_EL0 0x400000000ULL

#define AARCH64_HAFGRTR_EL2_AMEVTYPER18_EL0 0x800000000ULL

#define AARCH64_HAFGRTR_EL2_AMEVCNTR19_EL0 0x1000000000ULL

#define AARCH64_HAFGRTR_EL2_AMEVTYPER19_EL0 0x2000000000ULL

#define AARCH64_HAFGRTR_EL2_AMEVCNTR110_EL0 0x4000000000ULL

#define AARCH64_HAFGRTR_EL2_AMEVTYPER110_EL0 0x8000000000ULL

#define AARCH64_HAFGRTR_EL2_AMEVCNTR111_EL0 0x10000000000ULL

#define AARCH64_HAFGRTR_EL2_AMEVTYPER111_EL0 0x20000000000ULL

#define AARCH64_HAFGRTR_EL2_AMEVCNTR112_EL0 0x40000000000ULL

#define AARCH64_HAFGRTR_EL2_AMEVTYPER112_EL0 0x80000000000ULL

#define AARCH64_HAFGRTR_EL2_AMEVCNTR113_EL0 0x100000000000ULL

#define AARCH64_HAFGRTR_EL2_AMEVTYPER113_EL0 0x200000000000ULL

#define AARCH64_HAFGRTR_EL2_AMEVCNTR114_EL0 0x400000000000ULL

#define AARCH64_HAFGRTR_EL2_AMEVTYPER114_EL0 0x800000000000ULL

#define AARCH64_HAFGRTR_EL2_AMEVCNTR115_EL0 0x1000000000000ULL

#define AARCH64_HAFGRTR_EL2_AMEVTYPER115_EL0 0x2000000000000ULL

static inline uint64_t _AArch64_Read_hafgrtr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, HAFGRTR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_hafgrtr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr HAFGRTR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* HCR_EL2, Hypervisor Configuration Register */

#define AARCH64_HCR_EL2_VM 0x1U

#define AARCH64_HCR_EL2_SWIO 0x2U

#define AARCH64_HCR_EL2_PTW 0x4U

#define AARCH64_HCR_EL2_FMO 0x8U

#define AARCH64_HCR_EL2_IMO 0x10U

#define AARCH64_HCR_EL2_AMO 0x20U

#define AARCH64_HCR_EL2_VF 0x40U

#define AARCH64_HCR_EL2_VI 0x80U

#define AARCH64_HCR_EL2_VSE 0x100U

#define AARCH64_HCR_EL2_FB 0x200U

#define AARCH64_HCR_EL2_BSU( _val ) ( ( _val ) << 10 )
#define AARCH64_HCR_EL2_BSU_SHIFT 10
#define AARCH64_HCR_EL2_BSU_MASK 0xc00U
#define AARCH64_HCR_EL2_BSU_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3U )

#define AARCH64_HCR_EL2_DC 0x1000U

#define AARCH64_HCR_EL2_TWI 0x2000U

#define AARCH64_HCR_EL2_TWE 0x4000U

#define AARCH64_HCR_EL2_TID0 0x8000U

#define AARCH64_HCR_EL2_TID1 0x10000U

#define AARCH64_HCR_EL2_TID2 0x20000U

#define AARCH64_HCR_EL2_TID3 0x40000U

#define AARCH64_HCR_EL2_TSC 0x80000U

#define AARCH64_HCR_EL2_TIDCP 0x100000U

#define AARCH64_HCR_EL2_TACR 0x200000U

#define AARCH64_HCR_EL2_TSW 0x400000U

#define AARCH64_HCR_EL2_TPCP 0x800000U

#define AARCH64_HCR_EL2_TPU 0x1000000U

#define AARCH64_HCR_EL2_TTLB 0x2000000U

#define AARCH64_HCR_EL2_TVM 0x4000000U

#define AARCH64_HCR_EL2_TGE 0x8000000U

#define AARCH64_HCR_EL2_TDZ 0x10000000U

#define AARCH64_HCR_EL2_HCD 0x20000000U

#define AARCH64_HCR_EL2_TRVM 0x40000000U

#define AARCH64_HCR_EL2_RW 0x80000000U

#define AARCH64_HCR_EL2_CD 0x100000000ULL

#define AARCH64_HCR_EL2_ID 0x200000000ULL

#define AARCH64_HCR_EL2_E2H 0x400000000ULL

#define AARCH64_HCR_EL2_TLOR 0x800000000ULL

#define AARCH64_HCR_EL2_TERR 0x1000000000ULL

#define AARCH64_HCR_EL2_TEA 0x2000000000ULL

#define AARCH64_HCR_EL2_MIOCNCE 0x4000000000ULL

#define AARCH64_HCR_EL2_APK 0x10000000000ULL

#define AARCH64_HCR_EL2_API 0x20000000000ULL

#define AARCH64_HCR_EL2_NV 0x40000000000ULL

#define AARCH64_HCR_EL2_NV1 0x80000000000ULL

#define AARCH64_HCR_EL2_AT 0x100000000000ULL

#define AARCH64_HCR_EL2_NV2 0x200000000000ULL

#define AARCH64_HCR_EL2_FWB 0x400000000000ULL

#define AARCH64_HCR_EL2_FIEN 0x800000000000ULL

#define AARCH64_HCR_EL2_TID4 0x2000000000000ULL

#define AARCH64_HCR_EL2_TICAB 0x4000000000000ULL

#define AARCH64_HCR_EL2_AMVOFFEN 0x8000000000000ULL

#define AARCH64_HCR_EL2_TOCU 0x10000000000000ULL

#define AARCH64_HCR_EL2_ENSCXT 0x20000000000000ULL

#define AARCH64_HCR_EL2_TTLBIS 0x40000000000000ULL

#define AARCH64_HCR_EL2_TTLBOS 0x80000000000000ULL

#define AARCH64_HCR_EL2_ATA 0x100000000000000ULL

#define AARCH64_HCR_EL2_DCT 0x200000000000000ULL

#define AARCH64_HCR_EL2_TID5 0x400000000000000ULL

#define AARCH64_HCR_EL2_TWEDEN 0x800000000000000ULL

#define AARCH64_HCR_EL2_TWEDEL( _val ) ( ( _val ) << 60 )
#define AARCH64_HCR_EL2_TWEDEL_SHIFT 60
#define AARCH64_HCR_EL2_TWEDEL_MASK 0xf000000000000000ULL
#define AARCH64_HCR_EL2_TWEDEL_GET( _reg ) \
  ( ( ( _reg ) >> 60 ) & 0xfULL )

static inline uint64_t _AArch64_Read_hcr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, HCR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_hcr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr HCR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* HDFGRTR_EL2, Hypervisor Debug Fine-Grained Read Trap Register */

#define AARCH64_HDFGRTR_EL2_DBGBCRN_EL1 0x1U

#define AARCH64_HDFGRTR_EL2_DBGBVRN_EL1 0x2U

#define AARCH64_HDFGRTR_EL2_DBGWCRN_EL1 0x4U

#define AARCH64_HDFGRTR_EL2_DBGWVRN_EL1 0x8U

#define AARCH64_HDFGRTR_EL2_MDSCR_EL1 0x10U

#define AARCH64_HDFGRTR_EL2_DBGCLAIM 0x20U

#define AARCH64_HDFGRTR_EL2_DBGAUTHSTATUS_EL1 0x40U

#define AARCH64_HDFGRTR_EL2_DBGPRCR_EL1 0x80U

#define AARCH64_HDFGRTR_EL2_OSLSR_EL1 0x200U

#define AARCH64_HDFGRTR_EL2_OSECCR_EL1 0x400U

#define AARCH64_HDFGRTR_EL2_OSDLR_EL1 0x800U

#define AARCH64_HDFGRTR_EL2_PMEVCNTRN_EL0 0x1000U

#define AARCH64_HDFGRTR_EL2_PMEVTYPERN_EL0 0x2000U

#define AARCH64_HDFGRTR_EL2_PMCCFILTR_EL0 0x4000U

#define AARCH64_HDFGRTR_EL2_PMCCNTR_EL0 0x8000U

#define AARCH64_HDFGRTR_EL2_PMCNTEN 0x10000U

#define AARCH64_HDFGRTR_EL2_PMINTEN 0x20000U

#define AARCH64_HDFGRTR_EL2_PMOVS 0x40000U

#define AARCH64_HDFGRTR_EL2_PMSELR_EL0 0x80000U

#define AARCH64_HDFGRTR_EL2_PMMIR_EL1 0x400000U

#define AARCH64_HDFGRTR_EL2_PMBLIMITR_EL1 0x800000U

#define AARCH64_HDFGRTR_EL2_PMBPTR_EL1 0x1000000U

#define AARCH64_HDFGRTR_EL2_PMBSR_EL1 0x2000000U

#define AARCH64_HDFGRTR_EL2_PMSCR_EL1 0x4000000U

#define AARCH64_HDFGRTR_EL2_PMSEVFR_EL1 0x8000000U

#define AARCH64_HDFGRTR_EL2_PMSFCR_EL1 0x10000000U

#define AARCH64_HDFGRTR_EL2_PMSICR_EL1 0x20000000U

#define AARCH64_HDFGRTR_EL2_PMSIDR_EL1 0x40000000U

#define AARCH64_HDFGRTR_EL2_PMSIRR_EL1 0x80000000U

#define AARCH64_HDFGRTR_EL2_PMSLATFR_EL1 0x100000000ULL

#define AARCH64_HDFGRTR_EL2_TRC 0x200000000ULL

#define AARCH64_HDFGRTR_EL2_TRCAUTHSTATUS 0x400000000ULL

#define AARCH64_HDFGRTR_EL2_TRCAUXCTLR 0x800000000ULL

#define AARCH64_HDFGRTR_EL2_TRCCLAIM 0x1000000000ULL

#define AARCH64_HDFGRTR_EL2_TRCCNTVRN 0x2000000000ULL

#define AARCH64_HDFGRTR_EL2_TRCID 0x10000000000ULL

#define AARCH64_HDFGRTR_EL2_TRCIMSPECN 0x20000000000ULL

#define AARCH64_HDFGRTR_EL2_TRCOSLSR 0x80000000000ULL

#define AARCH64_HDFGRTR_EL2_TRCPRGCTLR 0x100000000000ULL

#define AARCH64_HDFGRTR_EL2_TRCSEQSTR 0x200000000000ULL

#define AARCH64_HDFGRTR_EL2_TRCSSCSRN 0x400000000000ULL

#define AARCH64_HDFGRTR_EL2_TRCSTATR 0x800000000000ULL

#define AARCH64_HDFGRTR_EL2_TRCVICTLR 0x1000000000000ULL

#define AARCH64_HDFGRTR_EL2_PMUSERENR_EL0 0x200000000000000ULL

#define AARCH64_HDFGRTR_EL2_PMCEIDN_EL0 0x400000000000000ULL

static inline uint64_t _AArch64_Read_hdfgrtr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, HDFGRTR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_hdfgrtr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr HDFGRTR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* HDFGWTR_EL2, Hypervisor Debug Fine-Grained Write Trap Register */

#define AARCH64_HDFGWTR_EL2_DBGBCRN_EL1 0x1U

#define AARCH64_HDFGWTR_EL2_DBGBVRN_EL1 0x2U

#define AARCH64_HDFGWTR_EL2_DBGWCRN_EL1 0x4U

#define AARCH64_HDFGWTR_EL2_DBGWVRN_EL1 0x8U

#define AARCH64_HDFGWTR_EL2_MDSCR_EL1 0x10U

#define AARCH64_HDFGWTR_EL2_DBGCLAIM 0x20U

#define AARCH64_HDFGWTR_EL2_DBGPRCR_EL1 0x80U

#define AARCH64_HDFGWTR_EL2_OSLAR_EL1 0x100U

#define AARCH64_HDFGWTR_EL2_OSECCR_EL1 0x400U

#define AARCH64_HDFGWTR_EL2_OSDLR_EL1 0x800U

#define AARCH64_HDFGWTR_EL2_PMEVCNTRN_EL0 0x1000U

#define AARCH64_HDFGWTR_EL2_PMEVTYPERN_EL0 0x2000U

#define AARCH64_HDFGWTR_EL2_PMCCFILTR_EL0 0x4000U

#define AARCH64_HDFGWTR_EL2_PMCCNTR_EL0 0x8000U

#define AARCH64_HDFGWTR_EL2_PMCNTEN 0x10000U

#define AARCH64_HDFGWTR_EL2_PMINTEN 0x20000U

#define AARCH64_HDFGWTR_EL2_PMOVS 0x40000U

#define AARCH64_HDFGWTR_EL2_PMSELR_EL0 0x80000U

#define AARCH64_HDFGWTR_EL2_PMSWINC_EL0 0x100000U

#define AARCH64_HDFGWTR_EL2_PMCR_EL0 0x200000U

#define AARCH64_HDFGWTR_EL2_PMBLIMITR_EL1 0x800000U

#define AARCH64_HDFGWTR_EL2_PMBPTR_EL1 0x1000000U

#define AARCH64_HDFGWTR_EL2_PMBSR_EL1 0x2000000U

#define AARCH64_HDFGWTR_EL2_PMSCR_EL1 0x4000000U

#define AARCH64_HDFGWTR_EL2_PMSEVFR_EL1 0x8000000U

#define AARCH64_HDFGWTR_EL2_PMSFCR_EL1 0x10000000U

#define AARCH64_HDFGWTR_EL2_PMSICR_EL1 0x20000000U

#define AARCH64_HDFGWTR_EL2_PMSIRR_EL1 0x80000000U

#define AARCH64_HDFGWTR_EL2_PMSLATFR_EL1 0x100000000ULL

#define AARCH64_HDFGWTR_EL2_TRC 0x200000000ULL

#define AARCH64_HDFGWTR_EL2_TRCAUXCTLR 0x800000000ULL

#define AARCH64_HDFGWTR_EL2_TRCCLAIM 0x1000000000ULL

#define AARCH64_HDFGWTR_EL2_TRCCNTVRN 0x2000000000ULL

#define AARCH64_HDFGWTR_EL2_TRCIMSPECN 0x20000000000ULL

#define AARCH64_HDFGWTR_EL2_TRCOSLAR 0x40000000000ULL

#define AARCH64_HDFGWTR_EL2_TRCPRGCTLR 0x100000000000ULL

#define AARCH64_HDFGWTR_EL2_TRCSEQSTR 0x200000000000ULL

#define AARCH64_HDFGWTR_EL2_TRCSSCSRN 0x400000000000ULL

#define AARCH64_HDFGWTR_EL2_TRCVICTLR 0x1000000000000ULL

#define AARCH64_HDFGWTR_EL2_TRFCR_EL1 0x2000000000000ULL

#define AARCH64_HDFGWTR_EL2_PMUSERENR_EL0 0x200000000000000ULL

static inline uint64_t _AArch64_Read_hdfgwtr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, HDFGWTR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_hdfgwtr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr HDFGWTR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* HFGITR_EL2, Hypervisor Fine-Grained Instruction Trap Register */

#define AARCH64_HFGITR_EL2_ICIALLUIS 0x1U

#define AARCH64_HFGITR_EL2_ICIALLU 0x2U

#define AARCH64_HFGITR_EL2_ICIVAU 0x4U

#define AARCH64_HFGITR_EL2_DCIVAC 0x8U

#define AARCH64_HFGITR_EL2_DCISW 0x10U

#define AARCH64_HFGITR_EL2_DCCSW 0x20U

#define AARCH64_HFGITR_EL2_DCCISW 0x40U

#define AARCH64_HFGITR_EL2_DCCVAU 0x80U

#define AARCH64_HFGITR_EL2_DCCVAP 0x100U

#define AARCH64_HFGITR_EL2_DCCVADP 0x200U

#define AARCH64_HFGITR_EL2_DCCIVAC 0x400U

#define AARCH64_HFGITR_EL2_DCZVA 0x800U

#define AARCH64_HFGITR_EL2_ATS1E1R 0x1000U

#define AARCH64_HFGITR_EL2_ATS1E1W 0x2000U

#define AARCH64_HFGITR_EL2_ATS1E0R 0x4000U

#define AARCH64_HFGITR_EL2_ATS1E0W 0x8000U

#define AARCH64_HFGITR_EL2_ATS1E1RP 0x10000U

#define AARCH64_HFGITR_EL2_ATS1E1WP 0x20000U

#define AARCH64_HFGITR_EL2_TLBIVMALLE1OS 0x40000U

#define AARCH64_HFGITR_EL2_TLBIVAE1OS 0x80000U

#define AARCH64_HFGITR_EL2_TLBIASIDE1OS 0x100000U

#define AARCH64_HFGITR_EL2_TLBIVAAE1OS 0x200000U

#define AARCH64_HFGITR_EL2_TLBIVALE1OS 0x400000U

#define AARCH64_HFGITR_EL2_TLBIVAALE1OS 0x800000U

#define AARCH64_HFGITR_EL2_TLBIRVAE1OS 0x1000000U

#define AARCH64_HFGITR_EL2_TLBIRVAAE1OS 0x2000000U

#define AARCH64_HFGITR_EL2_TLBIRVALE1OS 0x4000000U

#define AARCH64_HFGITR_EL2_TLBIRVAALE1OS 0x8000000U

#define AARCH64_HFGITR_EL2_TLBIVMALLE1IS 0x10000000U

#define AARCH64_HFGITR_EL2_TLBIVAE1IS 0x20000000U

#define AARCH64_HFGITR_EL2_TLBIASIDE1IS 0x40000000U

#define AARCH64_HFGITR_EL2_TLBIVAAE1IS 0x80000000U

#define AARCH64_HFGITR_EL2_TLBIVALE1IS 0x100000000ULL

#define AARCH64_HFGITR_EL2_TLBIVAALE1IS 0x200000000ULL

#define AARCH64_HFGITR_EL2_TLBIRVAE1IS 0x400000000ULL

#define AARCH64_HFGITR_EL2_TLBIRVAAE1IS 0x800000000ULL

#define AARCH64_HFGITR_EL2_TLBIRVALE1IS 0x1000000000ULL

#define AARCH64_HFGITR_EL2_TLBIRVAALE1IS 0x2000000000ULL

#define AARCH64_HFGITR_EL2_TLBIRVAE1 0x4000000000ULL

#define AARCH64_HFGITR_EL2_TLBIRVAAE1 0x8000000000ULL

#define AARCH64_HFGITR_EL2_TLBIRVALE1 0x10000000000ULL

#define AARCH64_HFGITR_EL2_TLBIRVAALE1 0x20000000000ULL

#define AARCH64_HFGITR_EL2_TLBIVMALLE1 0x40000000000ULL

#define AARCH64_HFGITR_EL2_TLBIVAE1 0x80000000000ULL

#define AARCH64_HFGITR_EL2_TLBIASIDE1 0x100000000000ULL

#define AARCH64_HFGITR_EL2_TLBIVAAE1 0x200000000000ULL

#define AARCH64_HFGITR_EL2_TLBIVALE1 0x400000000000ULL

#define AARCH64_HFGITR_EL2_TLBIVAALE1 0x800000000000ULL

#define AARCH64_HFGITR_EL2_CFPRCTX 0x1000000000000ULL

#define AARCH64_HFGITR_EL2_DVPRCTX 0x2000000000000ULL

#define AARCH64_HFGITR_EL2_CPPRCTX 0x4000000000000ULL

#define AARCH64_HFGITR_EL2_ERET 0x8000000000000ULL

#define AARCH64_HFGITR_EL2_SVC_EL0 0x10000000000000ULL

#define AARCH64_HFGITR_EL2_SVC_EL1 0x20000000000000ULL

#define AARCH64_HFGITR_EL2_DCCVAC 0x40000000000000ULL

static inline uint64_t _AArch64_Read_hfgitr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, HFGITR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_hfgitr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr HFGITR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* HFGRTR_EL2, Hypervisor Fine-Grained Read Trap Register */

#define AARCH64_HFGRTR_EL2_AFSR0_EL1 0x1U

#define AARCH64_HFGRTR_EL2_AFSR1_EL1 0x2U

#define AARCH64_HFGRTR_EL2_AIDR_EL1 0x4U

#define AARCH64_HFGRTR_EL2_AMAIR_EL1 0x8U

#define AARCH64_HFGRTR_EL2_APDAKEY 0x10U

#define AARCH64_HFGRTR_EL2_APDBKEY 0x20U

#define AARCH64_HFGRTR_EL2_APGAKEY 0x40U

#define AARCH64_HFGRTR_EL2_APIAKEY 0x80U

#define AARCH64_HFGRTR_EL2_APIBKEY 0x100U

#define AARCH64_HFGRTR_EL2_CCSIDR_EL1 0x200U

#define AARCH64_HFGRTR_EL2_CLIDR_EL1 0x400U

#define AARCH64_HFGRTR_EL2_CONTEXTIDR_EL1 0x800U

#define AARCH64_HFGRTR_EL2_CPACR_EL1 0x1000U

#define AARCH64_HFGRTR_EL2_CSSELR_EL1 0x2000U

#define AARCH64_HFGRTR_EL2_CTR_EL0 0x4000U

#define AARCH64_HFGRTR_EL2_DCZID_EL0 0x8000U

#define AARCH64_HFGRTR_EL2_ESR_EL1 0x10000U

#define AARCH64_HFGRTR_EL2_FAR_EL1 0x20000U

#define AARCH64_HFGRTR_EL2_ISR_EL1 0x40000U

#define AARCH64_HFGRTR_EL2_LORC_EL1 0x80000U

#define AARCH64_HFGRTR_EL2_LOREA_EL1 0x100000U

#define AARCH64_HFGRTR_EL2_LORID_EL1 0x200000U

#define AARCH64_HFGRTR_EL2_LORN_EL1 0x400000U

#define AARCH64_HFGRTR_EL2_LORSA_EL1 0x800000U

#define AARCH64_HFGRTR_EL2_MAIR_EL1 0x1000000U

#define AARCH64_HFGRTR_EL2_MIDR_EL1 0x2000000U

#define AARCH64_HFGRTR_EL2_MPIDR_EL1 0x4000000U

#define AARCH64_HFGRTR_EL2_PAR_EL1 0x8000000U

#define AARCH64_HFGRTR_EL2_REVIDR_EL1 0x10000000U

#define AARCH64_HFGRTR_EL2_SCTLR_EL1 0x20000000U

#define AARCH64_HFGRTR_EL2_SCXTNUM_EL1 0x40000000U

#define AARCH64_HFGRTR_EL2_SCXTNUM_EL0 0x80000000U

#define AARCH64_HFGRTR_EL2_TCR_EL1 0x100000000ULL

#define AARCH64_HFGRTR_EL2_TPIDR_EL1 0x200000000ULL

#define AARCH64_HFGRTR_EL2_TPIDRRO_EL0 0x400000000ULL

#define AARCH64_HFGRTR_EL2_TPIDR_EL0 0x800000000ULL

#define AARCH64_HFGRTR_EL2_TTBR0_EL1 0x1000000000ULL

#define AARCH64_HFGRTR_EL2_TTBR1_EL1 0x2000000000ULL

#define AARCH64_HFGRTR_EL2_VBAR_EL1 0x4000000000ULL

#define AARCH64_HFGRTR_EL2_ICC_IGRPENN_EL1 0x8000000000ULL

#define AARCH64_HFGRTR_EL2_ERRIDR_EL1 0x10000000000ULL

#define AARCH64_HFGRTR_EL2_ERRSELR_EL1 0x20000000000ULL

#define AARCH64_HFGRTR_EL2_ERXFR_EL1 0x40000000000ULL

#define AARCH64_HFGRTR_EL2_ERXCTLR_EL1 0x80000000000ULL

#define AARCH64_HFGRTR_EL2_ERXSTATUS_EL1 0x100000000000ULL

#define AARCH64_HFGRTR_EL2_ERXMISCN_EL1 0x200000000000ULL

#define AARCH64_HFGRTR_EL2_ERXPFGF_EL1 0x400000000000ULL

#define AARCH64_HFGRTR_EL2_ERXPFGCTL_EL1 0x800000000000ULL

#define AARCH64_HFGRTR_EL2_ERXPFGCDN_EL1 0x1000000000000ULL

#define AARCH64_HFGRTR_EL2_ERXADDR_EL1 0x2000000000000ULL

static inline uint64_t _AArch64_Read_hfgrtr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, HFGRTR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_hfgrtr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr HFGRTR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* HFGWTR_EL2, Hypervisor Fine-Grained Write Trap Register */

#define AARCH64_HFGWTR_EL2_AFSR0_EL1 0x1U

#define AARCH64_HFGWTR_EL2_AFSR1_EL1 0x2U

#define AARCH64_HFGWTR_EL2_AMAIR_EL1 0x8U

#define AARCH64_HFGWTR_EL2_APDAKEY 0x10U

#define AARCH64_HFGWTR_EL2_APDBKEY 0x20U

#define AARCH64_HFGWTR_EL2_APGAKEY 0x40U

#define AARCH64_HFGWTR_EL2_APIAKEY 0x80U

#define AARCH64_HFGWTR_EL2_APIBKEY 0x100U

#define AARCH64_HFGWTR_EL2_CONTEXTIDR_EL1 0x800U

#define AARCH64_HFGWTR_EL2_CPACR_EL1 0x1000U

#define AARCH64_HFGWTR_EL2_CSSELR_EL1 0x2000U

#define AARCH64_HFGWTR_EL2_ESR_EL1 0x10000U

#define AARCH64_HFGWTR_EL2_FAR_EL1 0x20000U

#define AARCH64_HFGWTR_EL2_LORC_EL1 0x80000U

#define AARCH64_HFGWTR_EL2_LOREA_EL1 0x100000U

#define AARCH64_HFGWTR_EL2_LORN_EL1 0x400000U

#define AARCH64_HFGWTR_EL2_LORSA_EL1 0x800000U

#define AARCH64_HFGWTR_EL2_MAIR_EL1 0x1000000U

#define AARCH64_HFGWTR_EL2_PAR_EL1 0x8000000U

#define AARCH64_HFGWTR_EL2_SCTLR_EL1 0x20000000U

#define AARCH64_HFGWTR_EL2_SCXTNUM_EL1 0x40000000U

#define AARCH64_HFGWTR_EL2_SCXTNUM_EL0 0x80000000U

#define AARCH64_HFGWTR_EL2_TCR_EL1 0x100000000ULL

#define AARCH64_HFGWTR_EL2_TPIDR_EL1 0x200000000ULL

#define AARCH64_HFGWTR_EL2_TPIDRRO_EL0 0x400000000ULL

#define AARCH64_HFGWTR_EL2_TPIDR_EL0 0x800000000ULL

#define AARCH64_HFGWTR_EL2_TTBR0_EL1 0x1000000000ULL

#define AARCH64_HFGWTR_EL2_TTBR1_EL1 0x2000000000ULL

#define AARCH64_HFGWTR_EL2_VBAR_EL1 0x4000000000ULL

#define AARCH64_HFGWTR_EL2_ICC_IGRPENN_EL1 0x8000000000ULL

#define AARCH64_HFGWTR_EL2_ERRSELR_EL1 0x20000000000ULL

#define AARCH64_HFGWTR_EL2_ERXCTLR_EL1 0x80000000000ULL

#define AARCH64_HFGWTR_EL2_ERXSTATUS_EL1 0x100000000000ULL

#define AARCH64_HFGWTR_EL2_ERXMISCN_EL1 0x200000000000ULL

#define AARCH64_HFGWTR_EL2_ERXPFGCTL_EL1 0x800000000000ULL

#define AARCH64_HFGWTR_EL2_ERXPFGCDN_EL1 0x1000000000000ULL

#define AARCH64_HFGWTR_EL2_ERXADDR_EL1 0x2000000000000ULL

static inline uint64_t _AArch64_Read_hfgwtr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, HFGWTR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_hfgwtr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr HFGWTR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* HPFAR_EL2, Hypervisor IPA Fault Address Register */

#define AARCH64_HPFAR_EL2_FIPA_47_12( _val ) ( ( _val ) << 4 )
#define AARCH64_HPFAR_EL2_FIPA_47_12_SHIFT 4
#define AARCH64_HPFAR_EL2_FIPA_47_12_MASK 0xfffffffff0ULL
#define AARCH64_HPFAR_EL2_FIPA_47_12_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfffffffffULL )

#define AARCH64_HPFAR_EL2_FIPA_51_48( _val ) ( ( _val ) << 40 )
#define AARCH64_HPFAR_EL2_FIPA_51_48_SHIFT 40
#define AARCH64_HPFAR_EL2_FIPA_51_48_MASK 0xf0000000000ULL
#define AARCH64_HPFAR_EL2_FIPA_51_48_GET( _reg ) \
  ( ( ( _reg ) >> 40 ) & 0xfULL )

#define AARCH64_HPFAR_EL2_NS 0x8000000000000000ULL

static inline uint64_t _AArch64_Read_hpfar_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, HPFAR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_hpfar_el2( uint64_t value )
{
  __asm__ volatile (
    "msr HPFAR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* HSTR_EL2, Hypervisor System Trap Register */

static inline uint64_t _AArch64_Read_hstr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, HSTR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_hstr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr HSTR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* ID_AA64AFR0_EL1, AArch64 Auxiliary Feature Register 0 */

static inline uint64_t _AArch64_Read_id_aa64afr0_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_AA64AFR0_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_AA64AFR1_EL1, AArch64 Auxiliary Feature Register 1 */

static inline uint64_t _AArch64_Read_id_aa64afr1_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_AA64AFR1_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_AA64DFR0_EL1, AArch64 Debug Feature Register 0 */

#define AARCH64_ID_AA64DFR0_EL1_DEBUGVER( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_AA64DFR0_EL1_DEBUGVER_SHIFT 0
#define AARCH64_ID_AA64DFR0_EL1_DEBUGVER_MASK 0xfU
#define AARCH64_ID_AA64DFR0_EL1_DEBUGVER_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_ID_AA64DFR0_EL1_TRACEVER( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_AA64DFR0_EL1_TRACEVER_SHIFT 4
#define AARCH64_ID_AA64DFR0_EL1_TRACEVER_MASK 0xf0U
#define AARCH64_ID_AA64DFR0_EL1_TRACEVER_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_AA64DFR0_EL1_PMUVER( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_AA64DFR0_EL1_PMUVER_SHIFT 8
#define AARCH64_ID_AA64DFR0_EL1_PMUVER_MASK 0xf00U
#define AARCH64_ID_AA64DFR0_EL1_PMUVER_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_ID_AA64DFR0_EL1_BRPS( _val ) ( ( _val ) << 12 )
#define AARCH64_ID_AA64DFR0_EL1_BRPS_SHIFT 12
#define AARCH64_ID_AA64DFR0_EL1_BRPS_MASK 0xf000U
#define AARCH64_ID_AA64DFR0_EL1_BRPS_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_ID_AA64DFR0_EL1_WRPS( _val ) ( ( _val ) << 20 )
#define AARCH64_ID_AA64DFR0_EL1_WRPS_SHIFT 20
#define AARCH64_ID_AA64DFR0_EL1_WRPS_MASK 0xf00000U
#define AARCH64_ID_AA64DFR0_EL1_WRPS_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ID_AA64DFR0_EL1_CTX_CMPS( _val ) ( ( _val ) << 28 )
#define AARCH64_ID_AA64DFR0_EL1_CTX_CMPS_SHIFT 28
#define AARCH64_ID_AA64DFR0_EL1_CTX_CMPS_MASK 0xf0000000U
#define AARCH64_ID_AA64DFR0_EL1_CTX_CMPS_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

#define AARCH64_ID_AA64DFR0_EL1_PMSVER( _val ) ( ( _val ) << 32 )
#define AARCH64_ID_AA64DFR0_EL1_PMSVER_SHIFT 32
#define AARCH64_ID_AA64DFR0_EL1_PMSVER_MASK 0xf00000000ULL
#define AARCH64_ID_AA64DFR0_EL1_PMSVER_GET( _reg ) \
  ( ( ( _reg ) >> 32 ) & 0xfULL )

#define AARCH64_ID_AA64DFR0_EL1_DOUBLELOCK( _val ) ( ( _val ) << 36 )
#define AARCH64_ID_AA64DFR0_EL1_DOUBLELOCK_SHIFT 36
#define AARCH64_ID_AA64DFR0_EL1_DOUBLELOCK_MASK 0xf000000000ULL
#define AARCH64_ID_AA64DFR0_EL1_DOUBLELOCK_GET( _reg ) \
  ( ( ( _reg ) >> 36 ) & 0xfULL )

#define AARCH64_ID_AA64DFR0_EL1_TRACEFILT( _val ) ( ( _val ) << 40 )
#define AARCH64_ID_AA64DFR0_EL1_TRACEFILT_SHIFT 40
#define AARCH64_ID_AA64DFR0_EL1_TRACEFILT_MASK 0xf0000000000ULL
#define AARCH64_ID_AA64DFR0_EL1_TRACEFILT_GET( _reg ) \
  ( ( ( _reg ) >> 40 ) & 0xfULL )

#define AARCH64_ID_AA64DFR0_EL1_MTPMU( _val ) ( ( _val ) << 48 )
#define AARCH64_ID_AA64DFR0_EL1_MTPMU_SHIFT 48
#define AARCH64_ID_AA64DFR0_EL1_MTPMU_MASK 0xf000000000000ULL
#define AARCH64_ID_AA64DFR0_EL1_MTPMU_GET( _reg ) \
  ( ( ( _reg ) >> 48 ) & 0xfULL )

static inline uint64_t _AArch64_Read_id_aa64dfr0_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_AA64DFR0_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_AA64DFR1_EL1, AArch64 Debug Feature Register 1 */

static inline uint64_t _AArch64_Read_id_aa64dfr1_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_AA64DFR1_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_AA64ISAR0_EL1, AArch64 Instruction Set Attribute Register 0 */

#define AARCH64_ID_AA64ISAR0_EL1_AES( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_AA64ISAR0_EL1_AES_SHIFT 4
#define AARCH64_ID_AA64ISAR0_EL1_AES_MASK 0xf0U
#define AARCH64_ID_AA64ISAR0_EL1_AES_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_AA64ISAR0_EL1_SHA1( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_AA64ISAR0_EL1_SHA1_SHIFT 8
#define AARCH64_ID_AA64ISAR0_EL1_SHA1_MASK 0xf00U
#define AARCH64_ID_AA64ISAR0_EL1_SHA1_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_ID_AA64ISAR0_EL1_SHA2( _val ) ( ( _val ) << 12 )
#define AARCH64_ID_AA64ISAR0_EL1_SHA2_SHIFT 12
#define AARCH64_ID_AA64ISAR0_EL1_SHA2_MASK 0xf000U
#define AARCH64_ID_AA64ISAR0_EL1_SHA2_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_ID_AA64ISAR0_EL1_CRC32( _val ) ( ( _val ) << 16 )
#define AARCH64_ID_AA64ISAR0_EL1_CRC32_SHIFT 16
#define AARCH64_ID_AA64ISAR0_EL1_CRC32_MASK 0xf0000U
#define AARCH64_ID_AA64ISAR0_EL1_CRC32_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ID_AA64ISAR0_EL1_ATOMIC( _val ) ( ( _val ) << 20 )
#define AARCH64_ID_AA64ISAR0_EL1_ATOMIC_SHIFT 20
#define AARCH64_ID_AA64ISAR0_EL1_ATOMIC_MASK 0xf00000U
#define AARCH64_ID_AA64ISAR0_EL1_ATOMIC_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ID_AA64ISAR0_EL1_RDM( _val ) ( ( _val ) << 28 )
#define AARCH64_ID_AA64ISAR0_EL1_RDM_SHIFT 28
#define AARCH64_ID_AA64ISAR0_EL1_RDM_MASK 0xf0000000U
#define AARCH64_ID_AA64ISAR0_EL1_RDM_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

#define AARCH64_ID_AA64ISAR0_EL1_SHA3( _val ) ( ( _val ) << 32 )
#define AARCH64_ID_AA64ISAR0_EL1_SHA3_SHIFT 32
#define AARCH64_ID_AA64ISAR0_EL1_SHA3_MASK 0xf00000000ULL
#define AARCH64_ID_AA64ISAR0_EL1_SHA3_GET( _reg ) \
  ( ( ( _reg ) >> 32 ) & 0xfULL )

#define AARCH64_ID_AA64ISAR0_EL1_SM3( _val ) ( ( _val ) << 36 )
#define AARCH64_ID_AA64ISAR0_EL1_SM3_SHIFT 36
#define AARCH64_ID_AA64ISAR0_EL1_SM3_MASK 0xf000000000ULL
#define AARCH64_ID_AA64ISAR0_EL1_SM3_GET( _reg ) \
  ( ( ( _reg ) >> 36 ) & 0xfULL )

#define AARCH64_ID_AA64ISAR0_EL1_SM4( _val ) ( ( _val ) << 40 )
#define AARCH64_ID_AA64ISAR0_EL1_SM4_SHIFT 40
#define AARCH64_ID_AA64ISAR0_EL1_SM4_MASK 0xf0000000000ULL
#define AARCH64_ID_AA64ISAR0_EL1_SM4_GET( _reg ) \
  ( ( ( _reg ) >> 40 ) & 0xfULL )

#define AARCH64_ID_AA64ISAR0_EL1_DP( _val ) ( ( _val ) << 44 )
#define AARCH64_ID_AA64ISAR0_EL1_DP_SHIFT 44
#define AARCH64_ID_AA64ISAR0_EL1_DP_MASK 0xf00000000000ULL
#define AARCH64_ID_AA64ISAR0_EL1_DP_GET( _reg ) \
  ( ( ( _reg ) >> 44 ) & 0xfULL )

#define AARCH64_ID_AA64ISAR0_EL1_FHM( _val ) ( ( _val ) << 48 )
#define AARCH64_ID_AA64ISAR0_EL1_FHM_SHIFT 48
#define AARCH64_ID_AA64ISAR0_EL1_FHM_MASK 0xf000000000000ULL
#define AARCH64_ID_AA64ISAR0_EL1_FHM_GET( _reg ) \
  ( ( ( _reg ) >> 48 ) & 0xfULL )

#define AARCH64_ID_AA64ISAR0_EL1_TS( _val ) ( ( _val ) << 52 )
#define AARCH64_ID_AA64ISAR0_EL1_TS_SHIFT 52
#define AARCH64_ID_AA64ISAR0_EL1_TS_MASK 0xf0000000000000ULL
#define AARCH64_ID_AA64ISAR0_EL1_TS_GET( _reg ) \
  ( ( ( _reg ) >> 52 ) & 0xfULL )

#define AARCH64_ID_AA64ISAR0_EL1_TLB( _val ) ( ( _val ) << 56 )
#define AARCH64_ID_AA64ISAR0_EL1_TLB_SHIFT 56
#define AARCH64_ID_AA64ISAR0_EL1_TLB_MASK 0xf00000000000000ULL
#define AARCH64_ID_AA64ISAR0_EL1_TLB_GET( _reg ) \
  ( ( ( _reg ) >> 56 ) & 0xfULL )

#define AARCH64_ID_AA64ISAR0_EL1_RNDR( _val ) ( ( _val ) << 60 )
#define AARCH64_ID_AA64ISAR0_EL1_RNDR_SHIFT 60
#define AARCH64_ID_AA64ISAR0_EL1_RNDR_MASK 0xf000000000000000ULL
#define AARCH64_ID_AA64ISAR0_EL1_RNDR_GET( _reg ) \
  ( ( ( _reg ) >> 60 ) & 0xfULL )

static inline uint64_t _AArch64_Read_id_aa64isar0_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_AA64ISAR0_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_AA64ISAR1_EL1, AArch64 Instruction Set Attribute Register 1 */

#define AARCH64_ID_AA64ISAR1_EL1_DPB( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_AA64ISAR1_EL1_DPB_SHIFT 0
#define AARCH64_ID_AA64ISAR1_EL1_DPB_MASK 0xfU
#define AARCH64_ID_AA64ISAR1_EL1_DPB_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_ID_AA64ISAR1_EL1_APA( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_AA64ISAR1_EL1_APA_SHIFT 4
#define AARCH64_ID_AA64ISAR1_EL1_APA_MASK 0xf0U
#define AARCH64_ID_AA64ISAR1_EL1_APA_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_AA64ISAR1_EL1_API( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_AA64ISAR1_EL1_API_SHIFT 8
#define AARCH64_ID_AA64ISAR1_EL1_API_MASK 0xf00U
#define AARCH64_ID_AA64ISAR1_EL1_API_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_ID_AA64ISAR1_EL1_JSCVT( _val ) ( ( _val ) << 12 )
#define AARCH64_ID_AA64ISAR1_EL1_JSCVT_SHIFT 12
#define AARCH64_ID_AA64ISAR1_EL1_JSCVT_MASK 0xf000U
#define AARCH64_ID_AA64ISAR1_EL1_JSCVT_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_ID_AA64ISAR1_EL1_FCMA( _val ) ( ( _val ) << 16 )
#define AARCH64_ID_AA64ISAR1_EL1_FCMA_SHIFT 16
#define AARCH64_ID_AA64ISAR1_EL1_FCMA_MASK 0xf0000U
#define AARCH64_ID_AA64ISAR1_EL1_FCMA_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ID_AA64ISAR1_EL1_LRCPC( _val ) ( ( _val ) << 20 )
#define AARCH64_ID_AA64ISAR1_EL1_LRCPC_SHIFT 20
#define AARCH64_ID_AA64ISAR1_EL1_LRCPC_MASK 0xf00000U
#define AARCH64_ID_AA64ISAR1_EL1_LRCPC_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ID_AA64ISAR1_EL1_GPA( _val ) ( ( _val ) << 24 )
#define AARCH64_ID_AA64ISAR1_EL1_GPA_SHIFT 24
#define AARCH64_ID_AA64ISAR1_EL1_GPA_MASK 0xf000000U
#define AARCH64_ID_AA64ISAR1_EL1_GPA_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH64_ID_AA64ISAR1_EL1_GPI( _val ) ( ( _val ) << 28 )
#define AARCH64_ID_AA64ISAR1_EL1_GPI_SHIFT 28
#define AARCH64_ID_AA64ISAR1_EL1_GPI_MASK 0xf0000000U
#define AARCH64_ID_AA64ISAR1_EL1_GPI_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

#define AARCH64_ID_AA64ISAR1_EL1_FRINTTS( _val ) ( ( _val ) << 32 )
#define AARCH64_ID_AA64ISAR1_EL1_FRINTTS_SHIFT 32
#define AARCH64_ID_AA64ISAR1_EL1_FRINTTS_MASK 0xf00000000ULL
#define AARCH64_ID_AA64ISAR1_EL1_FRINTTS_GET( _reg ) \
  ( ( ( _reg ) >> 32 ) & 0xfULL )

#define AARCH64_ID_AA64ISAR1_EL1_SB( _val ) ( ( _val ) << 36 )
#define AARCH64_ID_AA64ISAR1_EL1_SB_SHIFT 36
#define AARCH64_ID_AA64ISAR1_EL1_SB_MASK 0xf000000000ULL
#define AARCH64_ID_AA64ISAR1_EL1_SB_GET( _reg ) \
  ( ( ( _reg ) >> 36 ) & 0xfULL )

#define AARCH64_ID_AA64ISAR1_EL1_SPECRES( _val ) ( ( _val ) << 40 )
#define AARCH64_ID_AA64ISAR1_EL1_SPECRES_SHIFT 40
#define AARCH64_ID_AA64ISAR1_EL1_SPECRES_MASK 0xf0000000000ULL
#define AARCH64_ID_AA64ISAR1_EL1_SPECRES_GET( _reg ) \
  ( ( ( _reg ) >> 40 ) & 0xfULL )

#define AARCH64_ID_AA64ISAR1_EL1_BF16( _val ) ( ( _val ) << 44 )
#define AARCH64_ID_AA64ISAR1_EL1_BF16_SHIFT 44
#define AARCH64_ID_AA64ISAR1_EL1_BF16_MASK 0xf00000000000ULL
#define AARCH64_ID_AA64ISAR1_EL1_BF16_GET( _reg ) \
  ( ( ( _reg ) >> 44 ) & 0xfULL )

#define AARCH64_ID_AA64ISAR1_EL1_DGH( _val ) ( ( _val ) << 48 )
#define AARCH64_ID_AA64ISAR1_EL1_DGH_SHIFT 48
#define AARCH64_ID_AA64ISAR1_EL1_DGH_MASK 0xf000000000000ULL
#define AARCH64_ID_AA64ISAR1_EL1_DGH_GET( _reg ) \
  ( ( ( _reg ) >> 48 ) & 0xfULL )

#define AARCH64_ID_AA64ISAR1_EL1_I8MM( _val ) ( ( _val ) << 52 )
#define AARCH64_ID_AA64ISAR1_EL1_I8MM_SHIFT 52
#define AARCH64_ID_AA64ISAR1_EL1_I8MM_MASK 0xf0000000000000ULL
#define AARCH64_ID_AA64ISAR1_EL1_I8MM_GET( _reg ) \
  ( ( ( _reg ) >> 52 ) & 0xfULL )

static inline uint64_t _AArch64_Read_id_aa64isar1_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_AA64ISAR1_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_AA64MMFR0_EL1, AArch64 Memory Model Feature Register 0 */

#define AARCH64_ID_AA64MMFR0_EL1_PARANGE( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_AA64MMFR0_EL1_PARANGE_SHIFT 0
#define AARCH64_ID_AA64MMFR0_EL1_PARANGE_MASK 0xfU
#define AARCH64_ID_AA64MMFR0_EL1_PARANGE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_ID_AA64MMFR0_EL1_ASIDBITS( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_AA64MMFR0_EL1_ASIDBITS_SHIFT 4
#define AARCH64_ID_AA64MMFR0_EL1_ASIDBITS_MASK 0xf0U
#define AARCH64_ID_AA64MMFR0_EL1_ASIDBITS_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_AA64MMFR0_EL1_BIGEND( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_AA64MMFR0_EL1_BIGEND_SHIFT 8
#define AARCH64_ID_AA64MMFR0_EL1_BIGEND_MASK 0xf00U
#define AARCH64_ID_AA64MMFR0_EL1_BIGEND_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_ID_AA64MMFR0_EL1_SNSMEM( _val ) ( ( _val ) << 12 )
#define AARCH64_ID_AA64MMFR0_EL1_SNSMEM_SHIFT 12
#define AARCH64_ID_AA64MMFR0_EL1_SNSMEM_MASK 0xf000U
#define AARCH64_ID_AA64MMFR0_EL1_SNSMEM_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_ID_AA64MMFR0_EL1_BIGENDEL0( _val ) ( ( _val ) << 16 )
#define AARCH64_ID_AA64MMFR0_EL1_BIGENDEL0_SHIFT 16
#define AARCH64_ID_AA64MMFR0_EL1_BIGENDEL0_MASK 0xf0000U
#define AARCH64_ID_AA64MMFR0_EL1_BIGENDEL0_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ID_AA64MMFR0_EL1_TGRAN16( _val ) ( ( _val ) << 20 )
#define AARCH64_ID_AA64MMFR0_EL1_TGRAN16_SHIFT 20
#define AARCH64_ID_AA64MMFR0_EL1_TGRAN16_MASK 0xf00000U
#define AARCH64_ID_AA64MMFR0_EL1_TGRAN16_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ID_AA64MMFR0_EL1_TGRAN64( _val ) ( ( _val ) << 24 )
#define AARCH64_ID_AA64MMFR0_EL1_TGRAN64_SHIFT 24
#define AARCH64_ID_AA64MMFR0_EL1_TGRAN64_MASK 0xf000000U
#define AARCH64_ID_AA64MMFR0_EL1_TGRAN64_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH64_ID_AA64MMFR0_EL1_TGRAN4( _val ) ( ( _val ) << 28 )
#define AARCH64_ID_AA64MMFR0_EL1_TGRAN4_SHIFT 28
#define AARCH64_ID_AA64MMFR0_EL1_TGRAN4_MASK 0xf0000000U
#define AARCH64_ID_AA64MMFR0_EL1_TGRAN4_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

#define AARCH64_ID_AA64MMFR0_EL1_TGRAN16_2( _val ) ( ( _val ) << 32 )
#define AARCH64_ID_AA64MMFR0_EL1_TGRAN16_2_SHIFT 32
#define AARCH64_ID_AA64MMFR0_EL1_TGRAN16_2_MASK 0xf00000000ULL
#define AARCH64_ID_AA64MMFR0_EL1_TGRAN16_2_GET( _reg ) \
  ( ( ( _reg ) >> 32 ) & 0xfULL )

#define AARCH64_ID_AA64MMFR0_EL1_TGRAN64_2( _val ) ( ( _val ) << 36 )
#define AARCH64_ID_AA64MMFR0_EL1_TGRAN64_2_SHIFT 36
#define AARCH64_ID_AA64MMFR0_EL1_TGRAN64_2_MASK 0xf000000000ULL
#define AARCH64_ID_AA64MMFR0_EL1_TGRAN64_2_GET( _reg ) \
  ( ( ( _reg ) >> 36 ) & 0xfULL )

#define AARCH64_ID_AA64MMFR0_EL1_TGRAN4_2( _val ) ( ( _val ) << 40 )
#define AARCH64_ID_AA64MMFR0_EL1_TGRAN4_2_SHIFT 40
#define AARCH64_ID_AA64MMFR0_EL1_TGRAN4_2_MASK 0xf0000000000ULL
#define AARCH64_ID_AA64MMFR0_EL1_TGRAN4_2_GET( _reg ) \
  ( ( ( _reg ) >> 40 ) & 0xfULL )

#define AARCH64_ID_AA64MMFR0_EL1_EXS( _val ) ( ( _val ) << 44 )
#define AARCH64_ID_AA64MMFR0_EL1_EXS_SHIFT 44
#define AARCH64_ID_AA64MMFR0_EL1_EXS_MASK 0xf00000000000ULL
#define AARCH64_ID_AA64MMFR0_EL1_EXS_GET( _reg ) \
  ( ( ( _reg ) >> 44 ) & 0xfULL )

#define AARCH64_ID_AA64MMFR0_EL1_FGT( _val ) ( ( _val ) << 56 )
#define AARCH64_ID_AA64MMFR0_EL1_FGT_SHIFT 56
#define AARCH64_ID_AA64MMFR0_EL1_FGT_MASK 0xf00000000000000ULL
#define AARCH64_ID_AA64MMFR0_EL1_FGT_GET( _reg ) \
  ( ( ( _reg ) >> 56 ) & 0xfULL )

#define AARCH64_ID_AA64MMFR0_EL1_ECV( _val ) ( ( _val ) << 60 )
#define AARCH64_ID_AA64MMFR0_EL1_ECV_SHIFT 60
#define AARCH64_ID_AA64MMFR0_EL1_ECV_MASK 0xf000000000000000ULL
#define AARCH64_ID_AA64MMFR0_EL1_ECV_GET( _reg ) \
  ( ( ( _reg ) >> 60 ) & 0xfULL )

static inline uint64_t _AArch64_Read_id_aa64mmfr0_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_AA64MMFR0_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_AA64MMFR1_EL1, AArch64 Memory Model Feature Register 1 */

#define AARCH64_ID_AA64MMFR1_EL1_HAFDBS( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_AA64MMFR1_EL1_HAFDBS_SHIFT 0
#define AARCH64_ID_AA64MMFR1_EL1_HAFDBS_MASK 0xfU
#define AARCH64_ID_AA64MMFR1_EL1_HAFDBS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_ID_AA64MMFR1_EL1_VMIDBITS( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_AA64MMFR1_EL1_VMIDBITS_SHIFT 4
#define AARCH64_ID_AA64MMFR1_EL1_VMIDBITS_MASK 0xf0U
#define AARCH64_ID_AA64MMFR1_EL1_VMIDBITS_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_AA64MMFR1_EL1_VH( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_AA64MMFR1_EL1_VH_SHIFT 8
#define AARCH64_ID_AA64MMFR1_EL1_VH_MASK 0xf00U
#define AARCH64_ID_AA64MMFR1_EL1_VH_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_ID_AA64MMFR1_EL1_HPDS( _val ) ( ( _val ) << 12 )
#define AARCH64_ID_AA64MMFR1_EL1_HPDS_SHIFT 12
#define AARCH64_ID_AA64MMFR1_EL1_HPDS_MASK 0xf000U
#define AARCH64_ID_AA64MMFR1_EL1_HPDS_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_ID_AA64MMFR1_EL1_LO( _val ) ( ( _val ) << 16 )
#define AARCH64_ID_AA64MMFR1_EL1_LO_SHIFT 16
#define AARCH64_ID_AA64MMFR1_EL1_LO_MASK 0xf0000U
#define AARCH64_ID_AA64MMFR1_EL1_LO_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ID_AA64MMFR1_EL1_PAN( _val ) ( ( _val ) << 20 )
#define AARCH64_ID_AA64MMFR1_EL1_PAN_SHIFT 20
#define AARCH64_ID_AA64MMFR1_EL1_PAN_MASK 0xf00000U
#define AARCH64_ID_AA64MMFR1_EL1_PAN_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ID_AA64MMFR1_EL1_SPECSEI( _val ) ( ( _val ) << 24 )
#define AARCH64_ID_AA64MMFR1_EL1_SPECSEI_SHIFT 24
#define AARCH64_ID_AA64MMFR1_EL1_SPECSEI_MASK 0xf000000U
#define AARCH64_ID_AA64MMFR1_EL1_SPECSEI_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH64_ID_AA64MMFR1_EL1_XNX( _val ) ( ( _val ) << 28 )
#define AARCH64_ID_AA64MMFR1_EL1_XNX_SHIFT 28
#define AARCH64_ID_AA64MMFR1_EL1_XNX_MASK 0xf0000000U
#define AARCH64_ID_AA64MMFR1_EL1_XNX_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

#define AARCH64_ID_AA64MMFR1_EL1_TWED( _val ) ( ( _val ) << 32 )
#define AARCH64_ID_AA64MMFR1_EL1_TWED_SHIFT 32
#define AARCH64_ID_AA64MMFR1_EL1_TWED_MASK 0xf00000000ULL
#define AARCH64_ID_AA64MMFR1_EL1_TWED_GET( _reg ) \
  ( ( ( _reg ) >> 32 ) & 0xfULL )

#define AARCH64_ID_AA64MMFR1_EL1_ETS( _val ) ( ( _val ) << 36 )
#define AARCH64_ID_AA64MMFR1_EL1_ETS_SHIFT 36
#define AARCH64_ID_AA64MMFR1_EL1_ETS_MASK 0xf000000000ULL
#define AARCH64_ID_AA64MMFR1_EL1_ETS_GET( _reg ) \
  ( ( ( _reg ) >> 36 ) & 0xfULL )

static inline uint64_t _AArch64_Read_id_aa64mmfr1_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_AA64MMFR1_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_AA64MMFR2_EL1, AArch64 Memory Model Feature Register 2 */

#define AARCH64_ID_AA64MMFR2_EL1_CNP( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_AA64MMFR2_EL1_CNP_SHIFT 0
#define AARCH64_ID_AA64MMFR2_EL1_CNP_MASK 0xfU
#define AARCH64_ID_AA64MMFR2_EL1_CNP_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_ID_AA64MMFR2_EL1_UAO( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_AA64MMFR2_EL1_UAO_SHIFT 4
#define AARCH64_ID_AA64MMFR2_EL1_UAO_MASK 0xf0U
#define AARCH64_ID_AA64MMFR2_EL1_UAO_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_AA64MMFR2_EL1_LSM( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_AA64MMFR2_EL1_LSM_SHIFT 8
#define AARCH64_ID_AA64MMFR2_EL1_LSM_MASK 0xf00U
#define AARCH64_ID_AA64MMFR2_EL1_LSM_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_ID_AA64MMFR2_EL1_IESB( _val ) ( ( _val ) << 12 )
#define AARCH64_ID_AA64MMFR2_EL1_IESB_SHIFT 12
#define AARCH64_ID_AA64MMFR2_EL1_IESB_MASK 0xf000U
#define AARCH64_ID_AA64MMFR2_EL1_IESB_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_ID_AA64MMFR2_EL1_VARANGE( _val ) ( ( _val ) << 16 )
#define AARCH64_ID_AA64MMFR2_EL1_VARANGE_SHIFT 16
#define AARCH64_ID_AA64MMFR2_EL1_VARANGE_MASK 0xf0000U
#define AARCH64_ID_AA64MMFR2_EL1_VARANGE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ID_AA64MMFR2_EL1_CCIDX( _val ) ( ( _val ) << 20 )
#define AARCH64_ID_AA64MMFR2_EL1_CCIDX_SHIFT 20
#define AARCH64_ID_AA64MMFR2_EL1_CCIDX_MASK 0xf00000U
#define AARCH64_ID_AA64MMFR2_EL1_CCIDX_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ID_AA64MMFR2_EL1_NV( _val ) ( ( _val ) << 24 )
#define AARCH64_ID_AA64MMFR2_EL1_NV_SHIFT 24
#define AARCH64_ID_AA64MMFR2_EL1_NV_MASK 0xf000000U
#define AARCH64_ID_AA64MMFR2_EL1_NV_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH64_ID_AA64MMFR2_EL1_ST( _val ) ( ( _val ) << 28 )
#define AARCH64_ID_AA64MMFR2_EL1_ST_SHIFT 28
#define AARCH64_ID_AA64MMFR2_EL1_ST_MASK 0xf0000000U
#define AARCH64_ID_AA64MMFR2_EL1_ST_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

#define AARCH64_ID_AA64MMFR2_EL1_AT( _val ) ( ( _val ) << 32 )
#define AARCH64_ID_AA64MMFR2_EL1_AT_SHIFT 32
#define AARCH64_ID_AA64MMFR2_EL1_AT_MASK 0xf00000000ULL
#define AARCH64_ID_AA64MMFR2_EL1_AT_GET( _reg ) \
  ( ( ( _reg ) >> 32 ) & 0xfULL )

#define AARCH64_ID_AA64MMFR2_EL1_IDS( _val ) ( ( _val ) << 36 )
#define AARCH64_ID_AA64MMFR2_EL1_IDS_SHIFT 36
#define AARCH64_ID_AA64MMFR2_EL1_IDS_MASK 0xf000000000ULL
#define AARCH64_ID_AA64MMFR2_EL1_IDS_GET( _reg ) \
  ( ( ( _reg ) >> 36 ) & 0xfULL )

#define AARCH64_ID_AA64MMFR2_EL1_FWB( _val ) ( ( _val ) << 40 )
#define AARCH64_ID_AA64MMFR2_EL1_FWB_SHIFT 40
#define AARCH64_ID_AA64MMFR2_EL1_FWB_MASK 0xf0000000000ULL
#define AARCH64_ID_AA64MMFR2_EL1_FWB_GET( _reg ) \
  ( ( ( _reg ) >> 40 ) & 0xfULL )

#define AARCH64_ID_AA64MMFR2_EL1_TTL( _val ) ( ( _val ) << 48 )
#define AARCH64_ID_AA64MMFR2_EL1_TTL_SHIFT 48
#define AARCH64_ID_AA64MMFR2_EL1_TTL_MASK 0xf000000000000ULL
#define AARCH64_ID_AA64MMFR2_EL1_TTL_GET( _reg ) \
  ( ( ( _reg ) >> 48 ) & 0xfULL )

#define AARCH64_ID_AA64MMFR2_EL1_BBM( _val ) ( ( _val ) << 52 )
#define AARCH64_ID_AA64MMFR2_EL1_BBM_SHIFT 52
#define AARCH64_ID_AA64MMFR2_EL1_BBM_MASK 0xf0000000000000ULL
#define AARCH64_ID_AA64MMFR2_EL1_BBM_GET( _reg ) \
  ( ( ( _reg ) >> 52 ) & 0xfULL )

#define AARCH64_ID_AA64MMFR2_EL1_EVT( _val ) ( ( _val ) << 56 )
#define AARCH64_ID_AA64MMFR2_EL1_EVT_SHIFT 56
#define AARCH64_ID_AA64MMFR2_EL1_EVT_MASK 0xf00000000000000ULL
#define AARCH64_ID_AA64MMFR2_EL1_EVT_GET( _reg ) \
  ( ( ( _reg ) >> 56 ) & 0xfULL )

#define AARCH64_ID_AA64MMFR2_EL1_E0PD( _val ) ( ( _val ) << 60 )
#define AARCH64_ID_AA64MMFR2_EL1_E0PD_SHIFT 60
#define AARCH64_ID_AA64MMFR2_EL1_E0PD_MASK 0xf000000000000000ULL
#define AARCH64_ID_AA64MMFR2_EL1_E0PD_GET( _reg ) \
  ( ( ( _reg ) >> 60 ) & 0xfULL )

static inline uint64_t _AArch64_Read_id_aa64mmfr2_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_AA64MMFR2_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_AA64PFR0_EL1, AArch64 Processor Feature Register 0 */

#define AARCH64_ID_AA64PFR0_EL1_EL0( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_AA64PFR0_EL1_EL0_SHIFT 0
#define AARCH64_ID_AA64PFR0_EL1_EL0_MASK 0xfU
#define AARCH64_ID_AA64PFR0_EL1_EL0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_ID_AA64PFR0_EL1_EL1( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_AA64PFR0_EL1_EL1_SHIFT 4
#define AARCH64_ID_AA64PFR0_EL1_EL1_MASK 0xf0U
#define AARCH64_ID_AA64PFR0_EL1_EL1_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_AA64PFR0_EL1_EL2( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_AA64PFR0_EL1_EL2_SHIFT 8
#define AARCH64_ID_AA64PFR0_EL1_EL2_MASK 0xf00U
#define AARCH64_ID_AA64PFR0_EL1_EL2_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_ID_AA64PFR0_EL1_EL3( _val ) ( ( _val ) << 12 )
#define AARCH64_ID_AA64PFR0_EL1_EL3_SHIFT 12
#define AARCH64_ID_AA64PFR0_EL1_EL3_MASK 0xf000U
#define AARCH64_ID_AA64PFR0_EL1_EL3_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_ID_AA64PFR0_EL1_FP( _val ) ( ( _val ) << 16 )
#define AARCH64_ID_AA64PFR0_EL1_FP_SHIFT 16
#define AARCH64_ID_AA64PFR0_EL1_FP_MASK 0xf0000U
#define AARCH64_ID_AA64PFR0_EL1_FP_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ID_AA64PFR0_EL1_ADVSIMD( _val ) ( ( _val ) << 20 )
#define AARCH64_ID_AA64PFR0_EL1_ADVSIMD_SHIFT 20
#define AARCH64_ID_AA64PFR0_EL1_ADVSIMD_MASK 0xf00000U
#define AARCH64_ID_AA64PFR0_EL1_ADVSIMD_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ID_AA64PFR0_EL1_GIC( _val ) ( ( _val ) << 24 )
#define AARCH64_ID_AA64PFR0_EL1_GIC_SHIFT 24
#define AARCH64_ID_AA64PFR0_EL1_GIC_MASK 0xf000000U
#define AARCH64_ID_AA64PFR0_EL1_GIC_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH64_ID_AA64PFR0_EL1_RAS( _val ) ( ( _val ) << 28 )
#define AARCH64_ID_AA64PFR0_EL1_RAS_SHIFT 28
#define AARCH64_ID_AA64PFR0_EL1_RAS_MASK 0xf0000000U
#define AARCH64_ID_AA64PFR0_EL1_RAS_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

#define AARCH64_ID_AA64PFR0_EL1_SVE( _val ) ( ( _val ) << 32 )
#define AARCH64_ID_AA64PFR0_EL1_SVE_SHIFT 32
#define AARCH64_ID_AA64PFR0_EL1_SVE_MASK 0xf00000000ULL
#define AARCH64_ID_AA64PFR0_EL1_SVE_GET( _reg ) \
  ( ( ( _reg ) >> 32 ) & 0xfULL )

#define AARCH64_ID_AA64PFR0_EL1_SEL2( _val ) ( ( _val ) << 36 )
#define AARCH64_ID_AA64PFR0_EL1_SEL2_SHIFT 36
#define AARCH64_ID_AA64PFR0_EL1_SEL2_MASK 0xf000000000ULL
#define AARCH64_ID_AA64PFR0_EL1_SEL2_GET( _reg ) \
  ( ( ( _reg ) >> 36 ) & 0xfULL )

#define AARCH64_ID_AA64PFR0_EL1_MPAM( _val ) ( ( _val ) << 40 )
#define AARCH64_ID_AA64PFR0_EL1_MPAM_SHIFT 40
#define AARCH64_ID_AA64PFR0_EL1_MPAM_MASK 0xf0000000000ULL
#define AARCH64_ID_AA64PFR0_EL1_MPAM_GET( _reg ) \
  ( ( ( _reg ) >> 40 ) & 0xfULL )

#define AARCH64_ID_AA64PFR0_EL1_AMU( _val ) ( ( _val ) << 44 )
#define AARCH64_ID_AA64PFR0_EL1_AMU_SHIFT 44
#define AARCH64_ID_AA64PFR0_EL1_AMU_MASK 0xf00000000000ULL
#define AARCH64_ID_AA64PFR0_EL1_AMU_GET( _reg ) \
  ( ( ( _reg ) >> 44 ) & 0xfULL )

#define AARCH64_ID_AA64PFR0_EL1_DIT( _val ) ( ( _val ) << 48 )
#define AARCH64_ID_AA64PFR0_EL1_DIT_SHIFT 48
#define AARCH64_ID_AA64PFR0_EL1_DIT_MASK 0xf000000000000ULL
#define AARCH64_ID_AA64PFR0_EL1_DIT_GET( _reg ) \
  ( ( ( _reg ) >> 48 ) & 0xfULL )

#define AARCH64_ID_AA64PFR0_EL1_CSV2( _val ) ( ( _val ) << 56 )
#define AARCH64_ID_AA64PFR0_EL1_CSV2_SHIFT 56
#define AARCH64_ID_AA64PFR0_EL1_CSV2_MASK 0xf00000000000000ULL
#define AARCH64_ID_AA64PFR0_EL1_CSV2_GET( _reg ) \
  ( ( ( _reg ) >> 56 ) & 0xfULL )

#define AARCH64_ID_AA64PFR0_EL1_CSV3( _val ) ( ( _val ) << 60 )
#define AARCH64_ID_AA64PFR0_EL1_CSV3_SHIFT 60
#define AARCH64_ID_AA64PFR0_EL1_CSV3_MASK 0xf000000000000000ULL
#define AARCH64_ID_AA64PFR0_EL1_CSV3_GET( _reg ) \
  ( ( ( _reg ) >> 60 ) & 0xfULL )

static inline uint64_t _AArch64_Read_id_aa64pfr0_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_AA64PFR0_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_AA64PFR1_EL1, AArch64 Processor Feature Register 1 */

#define AARCH64_ID_AA64PFR1_EL1_BT( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_AA64PFR1_EL1_BT_SHIFT 0
#define AARCH64_ID_AA64PFR1_EL1_BT_MASK 0xfU
#define AARCH64_ID_AA64PFR1_EL1_BT_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_ID_AA64PFR1_EL1_SSBS( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_AA64PFR1_EL1_SSBS_SHIFT 4
#define AARCH64_ID_AA64PFR1_EL1_SSBS_MASK 0xf0U
#define AARCH64_ID_AA64PFR1_EL1_SSBS_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_AA64PFR1_EL1_MTE( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_AA64PFR1_EL1_MTE_SHIFT 8
#define AARCH64_ID_AA64PFR1_EL1_MTE_MASK 0xf00U
#define AARCH64_ID_AA64PFR1_EL1_MTE_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_ID_AA64PFR1_EL1_RAS_FRAC( _val ) ( ( _val ) << 12 )
#define AARCH64_ID_AA64PFR1_EL1_RAS_FRAC_SHIFT 12
#define AARCH64_ID_AA64PFR1_EL1_RAS_FRAC_MASK 0xf000U
#define AARCH64_ID_AA64PFR1_EL1_RAS_FRAC_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_ID_AA64PFR1_EL1_MPAM_FRAC( _val ) ( ( _val ) << 16 )
#define AARCH64_ID_AA64PFR1_EL1_MPAM_FRAC_SHIFT 16
#define AARCH64_ID_AA64PFR1_EL1_MPAM_FRAC_MASK 0xf0000U
#define AARCH64_ID_AA64PFR1_EL1_MPAM_FRAC_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

static inline uint64_t _AArch64_Read_id_aa64pfr1_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_AA64PFR1_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_AFR0_EL1, AArch64 Auxiliary Feature Register 0 */

static inline uint64_t _AArch64_Read_id_afr0_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_AFR0_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_DFR0_EL1, AArch64 Debug Feature Register 0 */

#define AARCH64_ID_DFR0_EL1_COPDBG( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_DFR0_EL1_COPDBG_SHIFT 0
#define AARCH64_ID_DFR0_EL1_COPDBG_MASK 0xfU
#define AARCH64_ID_DFR0_EL1_COPDBG_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_ID_DFR0_EL1_COPSDBG( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_DFR0_EL1_COPSDBG_SHIFT 4
#define AARCH64_ID_DFR0_EL1_COPSDBG_MASK 0xf0U
#define AARCH64_ID_DFR0_EL1_COPSDBG_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_DFR0_EL1_MMAPDBG( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_DFR0_EL1_MMAPDBG_SHIFT 8
#define AARCH64_ID_DFR0_EL1_MMAPDBG_MASK 0xf00U
#define AARCH64_ID_DFR0_EL1_MMAPDBG_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_ID_DFR0_EL1_COPTRC( _val ) ( ( _val ) << 12 )
#define AARCH64_ID_DFR0_EL1_COPTRC_SHIFT 12
#define AARCH64_ID_DFR0_EL1_COPTRC_MASK 0xf000U
#define AARCH64_ID_DFR0_EL1_COPTRC_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_ID_DFR0_EL1_MMAPTRC( _val ) ( ( _val ) << 16 )
#define AARCH64_ID_DFR0_EL1_MMAPTRC_SHIFT 16
#define AARCH64_ID_DFR0_EL1_MMAPTRC_MASK 0xf0000U
#define AARCH64_ID_DFR0_EL1_MMAPTRC_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ID_DFR0_EL1_MPROFDBG( _val ) ( ( _val ) << 20 )
#define AARCH64_ID_DFR0_EL1_MPROFDBG_SHIFT 20
#define AARCH64_ID_DFR0_EL1_MPROFDBG_MASK 0xf00000U
#define AARCH64_ID_DFR0_EL1_MPROFDBG_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ID_DFR0_EL1_PERFMON( _val ) ( ( _val ) << 24 )
#define AARCH64_ID_DFR0_EL1_PERFMON_SHIFT 24
#define AARCH64_ID_DFR0_EL1_PERFMON_MASK 0xf000000U
#define AARCH64_ID_DFR0_EL1_PERFMON_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH64_ID_DFR0_EL1_TRACEFILT( _val ) ( ( _val ) << 28 )
#define AARCH64_ID_DFR0_EL1_TRACEFILT_SHIFT 28
#define AARCH64_ID_DFR0_EL1_TRACEFILT_MASK 0xf0000000U
#define AARCH64_ID_DFR0_EL1_TRACEFILT_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint64_t _AArch64_Read_id_dfr0_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_DFR0_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_DFR1_EL1, Debug Feature Register 1 */

#define AARCH64_ID_DFR1_EL1_MTPMU( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_DFR1_EL1_MTPMU_SHIFT 0
#define AARCH64_ID_DFR1_EL1_MTPMU_MASK 0xfU
#define AARCH64_ID_DFR1_EL1_MTPMU_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

static inline uint64_t _AArch64_Read_id_dfr1_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_DFR1_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_ISAR0_EL1, AArch64 Instruction Set Attribute Register 0 */

#define AARCH64_ID_ISAR0_EL1_SWAP( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_ISAR0_EL1_SWAP_SHIFT 0
#define AARCH64_ID_ISAR0_EL1_SWAP_MASK 0xfU
#define AARCH64_ID_ISAR0_EL1_SWAP_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_ID_ISAR0_EL1_BITCOUNT( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_ISAR0_EL1_BITCOUNT_SHIFT 4
#define AARCH64_ID_ISAR0_EL1_BITCOUNT_MASK 0xf0U
#define AARCH64_ID_ISAR0_EL1_BITCOUNT_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_ISAR0_EL1_BITFIELD( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_ISAR0_EL1_BITFIELD_SHIFT 8
#define AARCH64_ID_ISAR0_EL1_BITFIELD_MASK 0xf00U
#define AARCH64_ID_ISAR0_EL1_BITFIELD_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_ID_ISAR0_EL1_CMPBRANCH( _val ) ( ( _val ) << 12 )
#define AARCH64_ID_ISAR0_EL1_CMPBRANCH_SHIFT 12
#define AARCH64_ID_ISAR0_EL1_CMPBRANCH_MASK 0xf000U
#define AARCH64_ID_ISAR0_EL1_CMPBRANCH_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_ID_ISAR0_EL1_COPROC( _val ) ( ( _val ) << 16 )
#define AARCH64_ID_ISAR0_EL1_COPROC_SHIFT 16
#define AARCH64_ID_ISAR0_EL1_COPROC_MASK 0xf0000U
#define AARCH64_ID_ISAR0_EL1_COPROC_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ID_ISAR0_EL1_DEBUG( _val ) ( ( _val ) << 20 )
#define AARCH64_ID_ISAR0_EL1_DEBUG_SHIFT 20
#define AARCH64_ID_ISAR0_EL1_DEBUG_MASK 0xf00000U
#define AARCH64_ID_ISAR0_EL1_DEBUG_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ID_ISAR0_EL1_DIVIDE( _val ) ( ( _val ) << 24 )
#define AARCH64_ID_ISAR0_EL1_DIVIDE_SHIFT 24
#define AARCH64_ID_ISAR0_EL1_DIVIDE_MASK 0xf000000U
#define AARCH64_ID_ISAR0_EL1_DIVIDE_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

static inline uint64_t _AArch64_Read_id_isar0_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_ISAR0_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_ISAR1_EL1, AArch64 Instruction Set Attribute Register 1 */

#define AARCH64_ID_ISAR1_EL1_ENDIAN( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_ISAR1_EL1_ENDIAN_SHIFT 0
#define AARCH64_ID_ISAR1_EL1_ENDIAN_MASK 0xfU
#define AARCH64_ID_ISAR1_EL1_ENDIAN_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_ID_ISAR1_EL1_EXCEPT( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_ISAR1_EL1_EXCEPT_SHIFT 4
#define AARCH64_ID_ISAR1_EL1_EXCEPT_MASK 0xf0U
#define AARCH64_ID_ISAR1_EL1_EXCEPT_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_ISAR1_EL1_EXCEPT_AR( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_ISAR1_EL1_EXCEPT_AR_SHIFT 8
#define AARCH64_ID_ISAR1_EL1_EXCEPT_AR_MASK 0xf00U
#define AARCH64_ID_ISAR1_EL1_EXCEPT_AR_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_ID_ISAR1_EL1_EXTEND( _val ) ( ( _val ) << 12 )
#define AARCH64_ID_ISAR1_EL1_EXTEND_SHIFT 12
#define AARCH64_ID_ISAR1_EL1_EXTEND_MASK 0xf000U
#define AARCH64_ID_ISAR1_EL1_EXTEND_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_ID_ISAR1_EL1_IFTHEN( _val ) ( ( _val ) << 16 )
#define AARCH64_ID_ISAR1_EL1_IFTHEN_SHIFT 16
#define AARCH64_ID_ISAR1_EL1_IFTHEN_MASK 0xf0000U
#define AARCH64_ID_ISAR1_EL1_IFTHEN_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ID_ISAR1_EL1_IMMEDIATE( _val ) ( ( _val ) << 20 )
#define AARCH64_ID_ISAR1_EL1_IMMEDIATE_SHIFT 20
#define AARCH64_ID_ISAR1_EL1_IMMEDIATE_MASK 0xf00000U
#define AARCH64_ID_ISAR1_EL1_IMMEDIATE_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ID_ISAR1_EL1_INTERWORK( _val ) ( ( _val ) << 24 )
#define AARCH64_ID_ISAR1_EL1_INTERWORK_SHIFT 24
#define AARCH64_ID_ISAR1_EL1_INTERWORK_MASK 0xf000000U
#define AARCH64_ID_ISAR1_EL1_INTERWORK_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH64_ID_ISAR1_EL1_JAZELLE( _val ) ( ( _val ) << 28 )
#define AARCH64_ID_ISAR1_EL1_JAZELLE_SHIFT 28
#define AARCH64_ID_ISAR1_EL1_JAZELLE_MASK 0xf0000000U
#define AARCH64_ID_ISAR1_EL1_JAZELLE_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint64_t _AArch64_Read_id_isar1_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_ISAR1_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_ISAR2_EL1, AArch64 Instruction Set Attribute Register 2 */

#define AARCH64_ID_ISAR2_EL1_LOADSTORE( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_ISAR2_EL1_LOADSTORE_SHIFT 0
#define AARCH64_ID_ISAR2_EL1_LOADSTORE_MASK 0xfU
#define AARCH64_ID_ISAR2_EL1_LOADSTORE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_ID_ISAR2_EL1_MEMHINT( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_ISAR2_EL1_MEMHINT_SHIFT 4
#define AARCH64_ID_ISAR2_EL1_MEMHINT_MASK 0xf0U
#define AARCH64_ID_ISAR2_EL1_MEMHINT_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_ISAR2_EL1_MULTIACCESSINT( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_ISAR2_EL1_MULTIACCESSINT_SHIFT 8
#define AARCH64_ID_ISAR2_EL1_MULTIACCESSINT_MASK 0xf00U
#define AARCH64_ID_ISAR2_EL1_MULTIACCESSINT_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_ID_ISAR2_EL1_MULT( _val ) ( ( _val ) << 12 )
#define AARCH64_ID_ISAR2_EL1_MULT_SHIFT 12
#define AARCH64_ID_ISAR2_EL1_MULT_MASK 0xf000U
#define AARCH64_ID_ISAR2_EL1_MULT_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_ID_ISAR2_EL1_MULTS( _val ) ( ( _val ) << 16 )
#define AARCH64_ID_ISAR2_EL1_MULTS_SHIFT 16
#define AARCH64_ID_ISAR2_EL1_MULTS_MASK 0xf0000U
#define AARCH64_ID_ISAR2_EL1_MULTS_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ID_ISAR2_EL1_MULTU( _val ) ( ( _val ) << 20 )
#define AARCH64_ID_ISAR2_EL1_MULTU_SHIFT 20
#define AARCH64_ID_ISAR2_EL1_MULTU_MASK 0xf00000U
#define AARCH64_ID_ISAR2_EL1_MULTU_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ID_ISAR2_EL1_PSR_AR( _val ) ( ( _val ) << 24 )
#define AARCH64_ID_ISAR2_EL1_PSR_AR_SHIFT 24
#define AARCH64_ID_ISAR2_EL1_PSR_AR_MASK 0xf000000U
#define AARCH64_ID_ISAR2_EL1_PSR_AR_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH64_ID_ISAR2_EL1_REVERSAL( _val ) ( ( _val ) << 28 )
#define AARCH64_ID_ISAR2_EL1_REVERSAL_SHIFT 28
#define AARCH64_ID_ISAR2_EL1_REVERSAL_MASK 0xf0000000U
#define AARCH64_ID_ISAR2_EL1_REVERSAL_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint64_t _AArch64_Read_id_isar2_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_ISAR2_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_ISAR3_EL1, AArch64 Instruction Set Attribute Register 3 */

#define AARCH64_ID_ISAR3_EL1_SATURATE( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_ISAR3_EL1_SATURATE_SHIFT 0
#define AARCH64_ID_ISAR3_EL1_SATURATE_MASK 0xfU
#define AARCH64_ID_ISAR3_EL1_SATURATE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_ID_ISAR3_EL1_SIMD( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_ISAR3_EL1_SIMD_SHIFT 4
#define AARCH64_ID_ISAR3_EL1_SIMD_MASK 0xf0U
#define AARCH64_ID_ISAR3_EL1_SIMD_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_ISAR3_EL1_SVC( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_ISAR3_EL1_SVC_SHIFT 8
#define AARCH64_ID_ISAR3_EL1_SVC_MASK 0xf00U
#define AARCH64_ID_ISAR3_EL1_SVC_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_ID_ISAR3_EL1_SYNCHPRIM( _val ) ( ( _val ) << 12 )
#define AARCH64_ID_ISAR3_EL1_SYNCHPRIM_SHIFT 12
#define AARCH64_ID_ISAR3_EL1_SYNCHPRIM_MASK 0xf000U
#define AARCH64_ID_ISAR3_EL1_SYNCHPRIM_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_ID_ISAR3_EL1_TABBRANCH( _val ) ( ( _val ) << 16 )
#define AARCH64_ID_ISAR3_EL1_TABBRANCH_SHIFT 16
#define AARCH64_ID_ISAR3_EL1_TABBRANCH_MASK 0xf0000U
#define AARCH64_ID_ISAR3_EL1_TABBRANCH_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ID_ISAR3_EL1_T32COPY( _val ) ( ( _val ) << 20 )
#define AARCH64_ID_ISAR3_EL1_T32COPY_SHIFT 20
#define AARCH64_ID_ISAR3_EL1_T32COPY_MASK 0xf00000U
#define AARCH64_ID_ISAR3_EL1_T32COPY_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ID_ISAR3_EL1_TRUENOP( _val ) ( ( _val ) << 24 )
#define AARCH64_ID_ISAR3_EL1_TRUENOP_SHIFT 24
#define AARCH64_ID_ISAR3_EL1_TRUENOP_MASK 0xf000000U
#define AARCH64_ID_ISAR3_EL1_TRUENOP_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH64_ID_ISAR3_EL1_T32EE( _val ) ( ( _val ) << 28 )
#define AARCH64_ID_ISAR3_EL1_T32EE_SHIFT 28
#define AARCH64_ID_ISAR3_EL1_T32EE_MASK 0xf0000000U
#define AARCH64_ID_ISAR3_EL1_T32EE_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint64_t _AArch64_Read_id_isar3_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_ISAR3_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_ISAR4_EL1, AArch64 Instruction Set Attribute Register 4 */

#define AARCH64_ID_ISAR4_EL1_UNPRIV( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_ISAR4_EL1_UNPRIV_SHIFT 0
#define AARCH64_ID_ISAR4_EL1_UNPRIV_MASK 0xfU
#define AARCH64_ID_ISAR4_EL1_UNPRIV_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_ID_ISAR4_EL1_WITHSHIFTS( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_ISAR4_EL1_WITHSHIFTS_SHIFT 4
#define AARCH64_ID_ISAR4_EL1_WITHSHIFTS_MASK 0xf0U
#define AARCH64_ID_ISAR4_EL1_WITHSHIFTS_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_ISAR4_EL1_WRITEBACK( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_ISAR4_EL1_WRITEBACK_SHIFT 8
#define AARCH64_ID_ISAR4_EL1_WRITEBACK_MASK 0xf00U
#define AARCH64_ID_ISAR4_EL1_WRITEBACK_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_ID_ISAR4_EL1_SMC( _val ) ( ( _val ) << 12 )
#define AARCH64_ID_ISAR4_EL1_SMC_SHIFT 12
#define AARCH64_ID_ISAR4_EL1_SMC_MASK 0xf000U
#define AARCH64_ID_ISAR4_EL1_SMC_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_ID_ISAR4_EL1_BARRIER( _val ) ( ( _val ) << 16 )
#define AARCH64_ID_ISAR4_EL1_BARRIER_SHIFT 16
#define AARCH64_ID_ISAR4_EL1_BARRIER_MASK 0xf0000U
#define AARCH64_ID_ISAR4_EL1_BARRIER_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ID_ISAR4_EL1_SYNCHPRIM_FRAC( _val ) ( ( _val ) << 20 )
#define AARCH64_ID_ISAR4_EL1_SYNCHPRIM_FRAC_SHIFT 20
#define AARCH64_ID_ISAR4_EL1_SYNCHPRIM_FRAC_MASK 0xf00000U
#define AARCH64_ID_ISAR4_EL1_SYNCHPRIM_FRAC_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ID_ISAR4_EL1_PSR_M( _val ) ( ( _val ) << 24 )
#define AARCH64_ID_ISAR4_EL1_PSR_M_SHIFT 24
#define AARCH64_ID_ISAR4_EL1_PSR_M_MASK 0xf000000U
#define AARCH64_ID_ISAR4_EL1_PSR_M_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH64_ID_ISAR4_EL1_SWP_FRAC( _val ) ( ( _val ) << 28 )
#define AARCH64_ID_ISAR4_EL1_SWP_FRAC_SHIFT 28
#define AARCH64_ID_ISAR4_EL1_SWP_FRAC_MASK 0xf0000000U
#define AARCH64_ID_ISAR4_EL1_SWP_FRAC_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint64_t _AArch64_Read_id_isar4_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_ISAR4_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_ISAR5_EL1, AArch64 Instruction Set Attribute Register 5 */

#define AARCH64_ID_ISAR5_EL1_SEVL( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_ISAR5_EL1_SEVL_SHIFT 0
#define AARCH64_ID_ISAR5_EL1_SEVL_MASK 0xfU
#define AARCH64_ID_ISAR5_EL1_SEVL_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_ID_ISAR5_EL1_AES( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_ISAR5_EL1_AES_SHIFT 4
#define AARCH64_ID_ISAR5_EL1_AES_MASK 0xf0U
#define AARCH64_ID_ISAR5_EL1_AES_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_ISAR5_EL1_SHA1( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_ISAR5_EL1_SHA1_SHIFT 8
#define AARCH64_ID_ISAR5_EL1_SHA1_MASK 0xf00U
#define AARCH64_ID_ISAR5_EL1_SHA1_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_ID_ISAR5_EL1_SHA2( _val ) ( ( _val ) << 12 )
#define AARCH64_ID_ISAR5_EL1_SHA2_SHIFT 12
#define AARCH64_ID_ISAR5_EL1_SHA2_MASK 0xf000U
#define AARCH64_ID_ISAR5_EL1_SHA2_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_ID_ISAR5_EL1_CRC32( _val ) ( ( _val ) << 16 )
#define AARCH64_ID_ISAR5_EL1_CRC32_SHIFT 16
#define AARCH64_ID_ISAR5_EL1_CRC32_MASK 0xf0000U
#define AARCH64_ID_ISAR5_EL1_CRC32_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ID_ISAR5_EL1_RDM( _val ) ( ( _val ) << 24 )
#define AARCH64_ID_ISAR5_EL1_RDM_SHIFT 24
#define AARCH64_ID_ISAR5_EL1_RDM_MASK 0xf000000U
#define AARCH64_ID_ISAR5_EL1_RDM_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH64_ID_ISAR5_EL1_VCMA( _val ) ( ( _val ) << 28 )
#define AARCH64_ID_ISAR5_EL1_VCMA_SHIFT 28
#define AARCH64_ID_ISAR5_EL1_VCMA_MASK 0xf0000000U
#define AARCH64_ID_ISAR5_EL1_VCMA_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint64_t _AArch64_Read_id_isar5_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_ISAR5_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_ISAR6_EL1, AArch64 Instruction Set Attribute Register 6 */

#define AARCH64_ID_ISAR6_EL1_JSCVT( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_ISAR6_EL1_JSCVT_SHIFT 0
#define AARCH64_ID_ISAR6_EL1_JSCVT_MASK 0xfU
#define AARCH64_ID_ISAR6_EL1_JSCVT_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_ID_ISAR6_EL1_DP( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_ISAR6_EL1_DP_SHIFT 4
#define AARCH64_ID_ISAR6_EL1_DP_MASK 0xf0U
#define AARCH64_ID_ISAR6_EL1_DP_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_ISAR6_EL1_FHM( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_ISAR6_EL1_FHM_SHIFT 8
#define AARCH64_ID_ISAR6_EL1_FHM_MASK 0xf00U
#define AARCH64_ID_ISAR6_EL1_FHM_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_ID_ISAR6_EL1_SB( _val ) ( ( _val ) << 12 )
#define AARCH64_ID_ISAR6_EL1_SB_SHIFT 12
#define AARCH64_ID_ISAR6_EL1_SB_MASK 0xf000U
#define AARCH64_ID_ISAR6_EL1_SB_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_ID_ISAR6_EL1_SPECRES( _val ) ( ( _val ) << 16 )
#define AARCH64_ID_ISAR6_EL1_SPECRES_SHIFT 16
#define AARCH64_ID_ISAR6_EL1_SPECRES_MASK 0xf0000U
#define AARCH64_ID_ISAR6_EL1_SPECRES_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ID_ISAR6_EL1_BF16( _val ) ( ( _val ) << 20 )
#define AARCH64_ID_ISAR6_EL1_BF16_SHIFT 20
#define AARCH64_ID_ISAR6_EL1_BF16_MASK 0xf00000U
#define AARCH64_ID_ISAR6_EL1_BF16_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ID_ISAR6_EL1_I8MM( _val ) ( ( _val ) << 24 )
#define AARCH64_ID_ISAR6_EL1_I8MM_SHIFT 24
#define AARCH64_ID_ISAR6_EL1_I8MM_MASK 0xf000000U
#define AARCH64_ID_ISAR6_EL1_I8MM_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

static inline uint64_t _AArch64_Read_id_isar6_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_ISAR6_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_MMFR0_EL1, AArch64 Memory Model Feature Register 0 */

#define AARCH64_ID_MMFR0_EL1_VMSA( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_MMFR0_EL1_VMSA_SHIFT 0
#define AARCH64_ID_MMFR0_EL1_VMSA_MASK 0xfU
#define AARCH64_ID_MMFR0_EL1_VMSA_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_ID_MMFR0_EL1_PMSA( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_MMFR0_EL1_PMSA_SHIFT 4
#define AARCH64_ID_MMFR0_EL1_PMSA_MASK 0xf0U
#define AARCH64_ID_MMFR0_EL1_PMSA_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_MMFR0_EL1_OUTERSHR( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_MMFR0_EL1_OUTERSHR_SHIFT 8
#define AARCH64_ID_MMFR0_EL1_OUTERSHR_MASK 0xf00U
#define AARCH64_ID_MMFR0_EL1_OUTERSHR_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_ID_MMFR0_EL1_SHARELVL( _val ) ( ( _val ) << 12 )
#define AARCH64_ID_MMFR0_EL1_SHARELVL_SHIFT 12
#define AARCH64_ID_MMFR0_EL1_SHARELVL_MASK 0xf000U
#define AARCH64_ID_MMFR0_EL1_SHARELVL_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_ID_MMFR0_EL1_TCM( _val ) ( ( _val ) << 16 )
#define AARCH64_ID_MMFR0_EL1_TCM_SHIFT 16
#define AARCH64_ID_MMFR0_EL1_TCM_MASK 0xf0000U
#define AARCH64_ID_MMFR0_EL1_TCM_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ID_MMFR0_EL1_AUXREG( _val ) ( ( _val ) << 20 )
#define AARCH64_ID_MMFR0_EL1_AUXREG_SHIFT 20
#define AARCH64_ID_MMFR0_EL1_AUXREG_MASK 0xf00000U
#define AARCH64_ID_MMFR0_EL1_AUXREG_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ID_MMFR0_EL1_FCSE( _val ) ( ( _val ) << 24 )
#define AARCH64_ID_MMFR0_EL1_FCSE_SHIFT 24
#define AARCH64_ID_MMFR0_EL1_FCSE_MASK 0xf000000U
#define AARCH64_ID_MMFR0_EL1_FCSE_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH64_ID_MMFR0_EL1_INNERSHR( _val ) ( ( _val ) << 28 )
#define AARCH64_ID_MMFR0_EL1_INNERSHR_SHIFT 28
#define AARCH64_ID_MMFR0_EL1_INNERSHR_MASK 0xf0000000U
#define AARCH64_ID_MMFR0_EL1_INNERSHR_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint64_t _AArch64_Read_id_mmfr0_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_MMFR0_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_MMFR1_EL1, AArch64 Memory Model Feature Register 1 */

#define AARCH64_ID_MMFR1_EL1_L1HVDVA( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_MMFR1_EL1_L1HVDVA_SHIFT 0
#define AARCH64_ID_MMFR1_EL1_L1HVDVA_MASK 0xfU
#define AARCH64_ID_MMFR1_EL1_L1HVDVA_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_ID_MMFR1_EL1_L1UNIVA( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_MMFR1_EL1_L1UNIVA_SHIFT 4
#define AARCH64_ID_MMFR1_EL1_L1UNIVA_MASK 0xf0U
#define AARCH64_ID_MMFR1_EL1_L1UNIVA_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_MMFR1_EL1_L1HVDSW( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_MMFR1_EL1_L1HVDSW_SHIFT 8
#define AARCH64_ID_MMFR1_EL1_L1HVDSW_MASK 0xf00U
#define AARCH64_ID_MMFR1_EL1_L1HVDSW_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_ID_MMFR1_EL1_L1UNISW( _val ) ( ( _val ) << 12 )
#define AARCH64_ID_MMFR1_EL1_L1UNISW_SHIFT 12
#define AARCH64_ID_MMFR1_EL1_L1UNISW_MASK 0xf000U
#define AARCH64_ID_MMFR1_EL1_L1UNISW_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_ID_MMFR1_EL1_L1HVD( _val ) ( ( _val ) << 16 )
#define AARCH64_ID_MMFR1_EL1_L1HVD_SHIFT 16
#define AARCH64_ID_MMFR1_EL1_L1HVD_MASK 0xf0000U
#define AARCH64_ID_MMFR1_EL1_L1HVD_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ID_MMFR1_EL1_L1UNI( _val ) ( ( _val ) << 20 )
#define AARCH64_ID_MMFR1_EL1_L1UNI_SHIFT 20
#define AARCH64_ID_MMFR1_EL1_L1UNI_MASK 0xf00000U
#define AARCH64_ID_MMFR1_EL1_L1UNI_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ID_MMFR1_EL1_L1TSTCLN( _val ) ( ( _val ) << 24 )
#define AARCH64_ID_MMFR1_EL1_L1TSTCLN_SHIFT 24
#define AARCH64_ID_MMFR1_EL1_L1TSTCLN_MASK 0xf000000U
#define AARCH64_ID_MMFR1_EL1_L1TSTCLN_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH64_ID_MMFR1_EL1_BPRED( _val ) ( ( _val ) << 28 )
#define AARCH64_ID_MMFR1_EL1_BPRED_SHIFT 28
#define AARCH64_ID_MMFR1_EL1_BPRED_MASK 0xf0000000U
#define AARCH64_ID_MMFR1_EL1_BPRED_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint64_t _AArch64_Read_id_mmfr1_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_MMFR1_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_MMFR2_EL1, AArch64 Memory Model Feature Register 2 */

#define AARCH64_ID_MMFR2_EL1_L1HVDFG( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_MMFR2_EL1_L1HVDFG_SHIFT 0
#define AARCH64_ID_MMFR2_EL1_L1HVDFG_MASK 0xfU
#define AARCH64_ID_MMFR2_EL1_L1HVDFG_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_ID_MMFR2_EL1_L1HVDBG( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_MMFR2_EL1_L1HVDBG_SHIFT 4
#define AARCH64_ID_MMFR2_EL1_L1HVDBG_MASK 0xf0U
#define AARCH64_ID_MMFR2_EL1_L1HVDBG_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_MMFR2_EL1_L1HVDRNG( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_MMFR2_EL1_L1HVDRNG_SHIFT 8
#define AARCH64_ID_MMFR2_EL1_L1HVDRNG_MASK 0xf00U
#define AARCH64_ID_MMFR2_EL1_L1HVDRNG_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_ID_MMFR2_EL1_HVDTLB( _val ) ( ( _val ) << 12 )
#define AARCH64_ID_MMFR2_EL1_HVDTLB_SHIFT 12
#define AARCH64_ID_MMFR2_EL1_HVDTLB_MASK 0xf000U
#define AARCH64_ID_MMFR2_EL1_HVDTLB_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_ID_MMFR2_EL1_UNITLB( _val ) ( ( _val ) << 16 )
#define AARCH64_ID_MMFR2_EL1_UNITLB_SHIFT 16
#define AARCH64_ID_MMFR2_EL1_UNITLB_MASK 0xf0000U
#define AARCH64_ID_MMFR2_EL1_UNITLB_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ID_MMFR2_EL1_MEMBARR( _val ) ( ( _val ) << 20 )
#define AARCH64_ID_MMFR2_EL1_MEMBARR_SHIFT 20
#define AARCH64_ID_MMFR2_EL1_MEMBARR_MASK 0xf00000U
#define AARCH64_ID_MMFR2_EL1_MEMBARR_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ID_MMFR2_EL1_WFISTALL( _val ) ( ( _val ) << 24 )
#define AARCH64_ID_MMFR2_EL1_WFISTALL_SHIFT 24
#define AARCH64_ID_MMFR2_EL1_WFISTALL_MASK 0xf000000U
#define AARCH64_ID_MMFR2_EL1_WFISTALL_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH64_ID_MMFR2_EL1_HWACCFLG( _val ) ( ( _val ) << 28 )
#define AARCH64_ID_MMFR2_EL1_HWACCFLG_SHIFT 28
#define AARCH64_ID_MMFR2_EL1_HWACCFLG_MASK 0xf0000000U
#define AARCH64_ID_MMFR2_EL1_HWACCFLG_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint64_t _AArch64_Read_id_mmfr2_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_MMFR2_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_MMFR3_EL1, AArch64 Memory Model Feature Register 3 */

#define AARCH64_ID_MMFR3_EL1_CMAINTVA( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_MMFR3_EL1_CMAINTVA_SHIFT 0
#define AARCH64_ID_MMFR3_EL1_CMAINTVA_MASK 0xfU
#define AARCH64_ID_MMFR3_EL1_CMAINTVA_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_ID_MMFR3_EL1_CMAINTSW( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_MMFR3_EL1_CMAINTSW_SHIFT 4
#define AARCH64_ID_MMFR3_EL1_CMAINTSW_MASK 0xf0U
#define AARCH64_ID_MMFR3_EL1_CMAINTSW_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_MMFR3_EL1_BPMAINT( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_MMFR3_EL1_BPMAINT_SHIFT 8
#define AARCH64_ID_MMFR3_EL1_BPMAINT_MASK 0xf00U
#define AARCH64_ID_MMFR3_EL1_BPMAINT_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_ID_MMFR3_EL1_MAINTBCST( _val ) ( ( _val ) << 12 )
#define AARCH64_ID_MMFR3_EL1_MAINTBCST_SHIFT 12
#define AARCH64_ID_MMFR3_EL1_MAINTBCST_MASK 0xf000U
#define AARCH64_ID_MMFR3_EL1_MAINTBCST_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_ID_MMFR3_EL1_PAN( _val ) ( ( _val ) << 16 )
#define AARCH64_ID_MMFR3_EL1_PAN_SHIFT 16
#define AARCH64_ID_MMFR3_EL1_PAN_MASK 0xf0000U
#define AARCH64_ID_MMFR3_EL1_PAN_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ID_MMFR3_EL1_COHWALK( _val ) ( ( _val ) << 20 )
#define AARCH64_ID_MMFR3_EL1_COHWALK_SHIFT 20
#define AARCH64_ID_MMFR3_EL1_COHWALK_MASK 0xf00000U
#define AARCH64_ID_MMFR3_EL1_COHWALK_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ID_MMFR3_EL1_CMEMSZ( _val ) ( ( _val ) << 24 )
#define AARCH64_ID_MMFR3_EL1_CMEMSZ_SHIFT 24
#define AARCH64_ID_MMFR3_EL1_CMEMSZ_MASK 0xf000000U
#define AARCH64_ID_MMFR3_EL1_CMEMSZ_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH64_ID_MMFR3_EL1_SUPERSEC( _val ) ( ( _val ) << 28 )
#define AARCH64_ID_MMFR3_EL1_SUPERSEC_SHIFT 28
#define AARCH64_ID_MMFR3_EL1_SUPERSEC_MASK 0xf0000000U
#define AARCH64_ID_MMFR3_EL1_SUPERSEC_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint64_t _AArch64_Read_id_mmfr3_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_MMFR3_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_MMFR4_EL1, AArch64 Memory Model Feature Register 4 */

#define AARCH64_ID_MMFR4_EL1_SPECSEI( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_MMFR4_EL1_SPECSEI_SHIFT 0
#define AARCH64_ID_MMFR4_EL1_SPECSEI_MASK 0xfU
#define AARCH64_ID_MMFR4_EL1_SPECSEI_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_ID_MMFR4_EL1_AC2( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_MMFR4_EL1_AC2_SHIFT 4
#define AARCH64_ID_MMFR4_EL1_AC2_MASK 0xf0U
#define AARCH64_ID_MMFR4_EL1_AC2_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_MMFR4_EL1_XNX( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_MMFR4_EL1_XNX_SHIFT 8
#define AARCH64_ID_MMFR4_EL1_XNX_MASK 0xf00U
#define AARCH64_ID_MMFR4_EL1_XNX_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_ID_MMFR4_EL1_CNP( _val ) ( ( _val ) << 12 )
#define AARCH64_ID_MMFR4_EL1_CNP_SHIFT 12
#define AARCH64_ID_MMFR4_EL1_CNP_MASK 0xf000U
#define AARCH64_ID_MMFR4_EL1_CNP_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_ID_MMFR4_EL1_HPDS( _val ) ( ( _val ) << 16 )
#define AARCH64_ID_MMFR4_EL1_HPDS_SHIFT 16
#define AARCH64_ID_MMFR4_EL1_HPDS_MASK 0xf0000U
#define AARCH64_ID_MMFR4_EL1_HPDS_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ID_MMFR4_EL1_LSM( _val ) ( ( _val ) << 20 )
#define AARCH64_ID_MMFR4_EL1_LSM_SHIFT 20
#define AARCH64_ID_MMFR4_EL1_LSM_MASK 0xf00000U
#define AARCH64_ID_MMFR4_EL1_LSM_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ID_MMFR4_EL1_CCIDX( _val ) ( ( _val ) << 24 )
#define AARCH64_ID_MMFR4_EL1_CCIDX_SHIFT 24
#define AARCH64_ID_MMFR4_EL1_CCIDX_MASK 0xf000000U
#define AARCH64_ID_MMFR4_EL1_CCIDX_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH64_ID_MMFR4_EL1_EVT( _val ) ( ( _val ) << 28 )
#define AARCH64_ID_MMFR4_EL1_EVT_SHIFT 28
#define AARCH64_ID_MMFR4_EL1_EVT_MASK 0xf0000000U
#define AARCH64_ID_MMFR4_EL1_EVT_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint64_t _AArch64_Read_id_mmfr4_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_MMFR4_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_MMFR5_EL1, AArch64 Memory Model Feature Register 5 */

#define AARCH64_ID_MMFR5_EL1_ETS( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_MMFR5_EL1_ETS_SHIFT 0
#define AARCH64_ID_MMFR5_EL1_ETS_MASK 0xfU
#define AARCH64_ID_MMFR5_EL1_ETS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

static inline uint64_t _AArch64_Read_id_mmfr5_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_MMFR5_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_PFR0_EL1, AArch64 Processor Feature Register 0 */

#define AARCH64_ID_PFR0_EL1_STATE0( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_PFR0_EL1_STATE0_SHIFT 0
#define AARCH64_ID_PFR0_EL1_STATE0_MASK 0xfU
#define AARCH64_ID_PFR0_EL1_STATE0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_ID_PFR0_EL1_STATE1( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_PFR0_EL1_STATE1_SHIFT 4
#define AARCH64_ID_PFR0_EL1_STATE1_MASK 0xf0U
#define AARCH64_ID_PFR0_EL1_STATE1_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_PFR0_EL1_STATE2( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_PFR0_EL1_STATE2_SHIFT 8
#define AARCH64_ID_PFR0_EL1_STATE2_MASK 0xf00U
#define AARCH64_ID_PFR0_EL1_STATE2_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_ID_PFR0_EL1_STATE3( _val ) ( ( _val ) << 12 )
#define AARCH64_ID_PFR0_EL1_STATE3_SHIFT 12
#define AARCH64_ID_PFR0_EL1_STATE3_MASK 0xf000U
#define AARCH64_ID_PFR0_EL1_STATE3_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_ID_PFR0_EL1_CSV2( _val ) ( ( _val ) << 16 )
#define AARCH64_ID_PFR0_EL1_CSV2_SHIFT 16
#define AARCH64_ID_PFR0_EL1_CSV2_MASK 0xf0000U
#define AARCH64_ID_PFR0_EL1_CSV2_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ID_PFR0_EL1_AMU( _val ) ( ( _val ) << 20 )
#define AARCH64_ID_PFR0_EL1_AMU_SHIFT 20
#define AARCH64_ID_PFR0_EL1_AMU_MASK 0xf00000U
#define AARCH64_ID_PFR0_EL1_AMU_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ID_PFR0_EL1_DIT( _val ) ( ( _val ) << 24 )
#define AARCH64_ID_PFR0_EL1_DIT_SHIFT 24
#define AARCH64_ID_PFR0_EL1_DIT_MASK 0xf000000U
#define AARCH64_ID_PFR0_EL1_DIT_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH64_ID_PFR0_EL1_RAS( _val ) ( ( _val ) << 28 )
#define AARCH64_ID_PFR0_EL1_RAS_SHIFT 28
#define AARCH64_ID_PFR0_EL1_RAS_MASK 0xf0000000U
#define AARCH64_ID_PFR0_EL1_RAS_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint64_t _AArch64_Read_id_pfr0_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_PFR0_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_PFR1_EL1, AArch64 Processor Feature Register 1 */

#define AARCH64_ID_PFR1_EL1_PROGMOD( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_PFR1_EL1_PROGMOD_SHIFT 0
#define AARCH64_ID_PFR1_EL1_PROGMOD_MASK 0xfU
#define AARCH64_ID_PFR1_EL1_PROGMOD_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_ID_PFR1_EL1_SECURITY( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_PFR1_EL1_SECURITY_SHIFT 4
#define AARCH64_ID_PFR1_EL1_SECURITY_MASK 0xf0U
#define AARCH64_ID_PFR1_EL1_SECURITY_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_PFR1_EL1_MPROGMOD( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_PFR1_EL1_MPROGMOD_SHIFT 8
#define AARCH64_ID_PFR1_EL1_MPROGMOD_MASK 0xf00U
#define AARCH64_ID_PFR1_EL1_MPROGMOD_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_ID_PFR1_EL1_VIRTUALIZATION( _val ) ( ( _val ) << 12 )
#define AARCH64_ID_PFR1_EL1_VIRTUALIZATION_SHIFT 12
#define AARCH64_ID_PFR1_EL1_VIRTUALIZATION_MASK 0xf000U
#define AARCH64_ID_PFR1_EL1_VIRTUALIZATION_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_ID_PFR1_EL1_GENTIMER( _val ) ( ( _val ) << 16 )
#define AARCH64_ID_PFR1_EL1_GENTIMER_SHIFT 16
#define AARCH64_ID_PFR1_EL1_GENTIMER_MASK 0xf0000U
#define AARCH64_ID_PFR1_EL1_GENTIMER_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_ID_PFR1_EL1_SEC_FRAC( _val ) ( ( _val ) << 20 )
#define AARCH64_ID_PFR1_EL1_SEC_FRAC_SHIFT 20
#define AARCH64_ID_PFR1_EL1_SEC_FRAC_MASK 0xf00000U
#define AARCH64_ID_PFR1_EL1_SEC_FRAC_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_ID_PFR1_EL1_VIRT_FRAC( _val ) ( ( _val ) << 24 )
#define AARCH64_ID_PFR1_EL1_VIRT_FRAC_SHIFT 24
#define AARCH64_ID_PFR1_EL1_VIRT_FRAC_MASK 0xf000000U
#define AARCH64_ID_PFR1_EL1_VIRT_FRAC_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH64_ID_PFR1_EL1_GIC( _val ) ( ( _val ) << 28 )
#define AARCH64_ID_PFR1_EL1_GIC_SHIFT 28
#define AARCH64_ID_PFR1_EL1_GIC_MASK 0xf0000000U
#define AARCH64_ID_PFR1_EL1_GIC_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint64_t _AArch64_Read_id_pfr1_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_PFR1_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ID_PFR2_EL1, AArch64 Processor Feature Register 2 */

#define AARCH64_ID_PFR2_EL1_CSV3( _val ) ( ( _val ) << 0 )
#define AARCH64_ID_PFR2_EL1_CSV3_SHIFT 0
#define AARCH64_ID_PFR2_EL1_CSV3_MASK 0xfU
#define AARCH64_ID_PFR2_EL1_CSV3_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_ID_PFR2_EL1_SSBS( _val ) ( ( _val ) << 4 )
#define AARCH64_ID_PFR2_EL1_SSBS_SHIFT 4
#define AARCH64_ID_PFR2_EL1_SSBS_MASK 0xf0U
#define AARCH64_ID_PFR2_EL1_SSBS_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_ID_PFR2_EL1_RAS_FRAC( _val ) ( ( _val ) << 8 )
#define AARCH64_ID_PFR2_EL1_RAS_FRAC_SHIFT 8
#define AARCH64_ID_PFR2_EL1_RAS_FRAC_MASK 0xf00U
#define AARCH64_ID_PFR2_EL1_RAS_FRAC_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

static inline uint64_t _AArch64_Read_id_pfr2_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ID_PFR2_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* IFSR32_EL2, Instruction Fault Status Register (EL2) */

#define AARCH64_IFSR32_EL2_FS_3_0( _val ) ( ( _val ) << 0 )
#define AARCH64_IFSR32_EL2_FS_3_0_SHIFT 0
#define AARCH64_IFSR32_EL2_FS_3_0_MASK 0xfU
#define AARCH64_IFSR32_EL2_FS_3_0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_IFSR32_EL2_STATUS( _val ) ( ( _val ) << 0 )
#define AARCH64_IFSR32_EL2_STATUS_SHIFT 0
#define AARCH64_IFSR32_EL2_STATUS_MASK 0x3fU
#define AARCH64_IFSR32_EL2_STATUS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )

#define AARCH64_IFSR32_EL2_LPAE 0x200U

#define AARCH64_IFSR32_EL2_FS_4 0x400U

#define AARCH64_IFSR32_EL2_EXT 0x1000U

#define AARCH64_IFSR32_EL2_FNV 0x10000U

static inline uint64_t _AArch64_Read_ifsr32_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, IFSR32_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_ifsr32_el2( uint64_t value )
{
  __asm__ volatile (
    "msr IFSR32_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* ISR_EL1, Interrupt Status Register */

#define AARCH64_ISR_EL1_F 0x40U

#define AARCH64_ISR_EL1_I 0x80U

#define AARCH64_ISR_EL1_A 0x100U

static inline uint64_t _AArch64_Read_isr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ISR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* LORC_EL1, LORegion Control (EL1) */

#define AARCH64_LORC_EL1_EN 0x1U

#define AARCH64_LORC_EL1_DS( _val ) ( ( _val ) << 2 )
#define AARCH64_LORC_EL1_DS_SHIFT 2
#define AARCH64_LORC_EL1_DS_MASK 0x3fcU
#define AARCH64_LORC_EL1_DS_GET( _reg ) \
  ( ( ( _reg ) >> 2 ) & 0xffU )

static inline uint64_t _AArch64_Read_lorc_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, LORC_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_lorc_el1( uint64_t value )
{
  __asm__ volatile (
    "msr LORC_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* LOREA_EL1, LORegion End Address (EL1) */

#define AARCH64_LOREA_EL1_EA_47_16( _val ) ( ( _val ) << 16 )
#define AARCH64_LOREA_EL1_EA_47_16_SHIFT 16
#define AARCH64_LOREA_EL1_EA_47_16_MASK 0xffffffff0000ULL
#define AARCH64_LOREA_EL1_EA_47_16_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xffffffffULL )

#define AARCH64_LOREA_EL1_EA_51_48( _val ) ( ( _val ) << 48 )
#define AARCH64_LOREA_EL1_EA_51_48_SHIFT 48
#define AARCH64_LOREA_EL1_EA_51_48_MASK 0xf000000000000ULL
#define AARCH64_LOREA_EL1_EA_51_48_GET( _reg ) \
  ( ( ( _reg ) >> 48 ) & 0xfULL )

static inline uint64_t _AArch64_Read_lorea_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, LOREA_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_lorea_el1( uint64_t value )
{
  __asm__ volatile (
    "msr LOREA_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* LORID_EL1, LORegionID (EL1) */

#define AARCH64_LORID_EL1_LR( _val ) ( ( _val ) << 0 )
#define AARCH64_LORID_EL1_LR_SHIFT 0
#define AARCH64_LORID_EL1_LR_MASK 0xffU
#define AARCH64_LORID_EL1_LR_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

#define AARCH64_LORID_EL1_LD( _val ) ( ( _val ) << 16 )
#define AARCH64_LORID_EL1_LD_SHIFT 16
#define AARCH64_LORID_EL1_LD_MASK 0xff0000U
#define AARCH64_LORID_EL1_LD_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xffU )

static inline uint64_t _AArch64_Read_lorid_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, LORID_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* LORN_EL1, LORegion Number (EL1) */

#define AARCH64_LORN_EL1_NUM( _val ) ( ( _val ) << 0 )
#define AARCH64_LORN_EL1_NUM_SHIFT 0
#define AARCH64_LORN_EL1_NUM_MASK 0xffU
#define AARCH64_LORN_EL1_NUM_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

static inline uint64_t _AArch64_Read_lorn_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, LORN_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_lorn_el1( uint64_t value )
{
  __asm__ volatile (
    "msr LORN_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* LORSA_EL1, LORegion Start Address (EL1) */

#define AARCH64_LORSA_EL1_VALID 0x1U

#define AARCH64_LORSA_EL1_SA_47_16( _val ) ( ( _val ) << 16 )
#define AARCH64_LORSA_EL1_SA_47_16_SHIFT 16
#define AARCH64_LORSA_EL1_SA_47_16_MASK 0xffffffff0000ULL
#define AARCH64_LORSA_EL1_SA_47_16_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xffffffffULL )

#define AARCH64_LORSA_EL1_SA_51_48( _val ) ( ( _val ) << 48 )
#define AARCH64_LORSA_EL1_SA_51_48_SHIFT 48
#define AARCH64_LORSA_EL1_SA_51_48_MASK 0xf000000000000ULL
#define AARCH64_LORSA_EL1_SA_51_48_GET( _reg ) \
  ( ( ( _reg ) >> 48 ) & 0xfULL )

static inline uint64_t _AArch64_Read_lorsa_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, LORSA_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_lorsa_el1( uint64_t value )
{
  __asm__ volatile (
    "msr LORSA_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* MAIR_EL1, Memory Attribute Indirection Register (EL1) */

#define AARCH64_MAIR_EL1_ATTR0( _val ) ( ( _val ) << 0 )
#define AARCH64_MAIR_EL1_ATTR1( _val ) ( ( _val ) << 8 )
#define AARCH64_MAIR_EL1_ATTR2( _val ) ( ( _val ) << 16 )
#define AARCH64_MAIR_EL1_ATTR3( _val ) ( ( _val ) << 24 )
#define AARCH64_MAIR_EL1_ATTR4( _val ) ( ( _val ) << 32 )
#define AARCH64_MAIR_EL1_ATTR5( _val ) ( ( _val ) << 40 )
#define AARCH64_MAIR_EL1_ATTR6( _val ) ( ( _val ) << 48 )
#define AARCH64_MAIR_EL1_ATTR7( _val ) ( ( _val ) << 56 )

static inline uint64_t _AArch64_Read_mair_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, MAIR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_mair_el1( uint64_t value )
{
  __asm__ volatile (
    "msr MAIR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* MAIR_EL2, Memory Attribute Indirection Register (EL2) */

static inline uint64_t _AArch64_Read_mair_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, MAIR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_mair_el2( uint64_t value )
{
  __asm__ volatile (
    "msr MAIR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* MAIR_EL3, Memory Attribute Indirection Register (EL3) */

static inline uint64_t _AArch64_Read_mair_el3( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, MAIR_EL3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_mair_el3( uint64_t value )
{
  __asm__ volatile (
    "msr MAIR_EL3, %0" : : "r" ( value ) : "memory"
  );
}

/* MIDR_EL1, Main ID Register */

#define AARCH64_MIDR_EL1_REVISION( _val ) ( ( _val ) << 0 )
#define AARCH64_MIDR_EL1_REVISION_SHIFT 0
#define AARCH64_MIDR_EL1_REVISION_MASK 0xfU
#define AARCH64_MIDR_EL1_REVISION_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_MIDR_EL1_PARTNUM( _val ) ( ( _val ) << 4 )
#define AARCH64_MIDR_EL1_PARTNUM_SHIFT 4
#define AARCH64_MIDR_EL1_PARTNUM_MASK 0xfff0U
#define AARCH64_MIDR_EL1_PARTNUM_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfffU )

#define AARCH64_MIDR_EL1_ARCHITECTURE( _val ) ( ( _val ) << 16 )
#define AARCH64_MIDR_EL1_ARCHITECTURE_SHIFT 16
#define AARCH64_MIDR_EL1_ARCHITECTURE_MASK 0xf0000U
#define AARCH64_MIDR_EL1_ARCHITECTURE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_MIDR_EL1_VARIANT( _val ) ( ( _val ) << 20 )
#define AARCH64_MIDR_EL1_VARIANT_SHIFT 20
#define AARCH64_MIDR_EL1_VARIANT_MASK 0xf00000U
#define AARCH64_MIDR_EL1_VARIANT_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_MIDR_EL1_IMPLEMENTER( _val ) ( ( _val ) << 24 )
#define AARCH64_MIDR_EL1_IMPLEMENTER_SHIFT 24
#define AARCH64_MIDR_EL1_IMPLEMENTER_MASK 0xff000000U
#define AARCH64_MIDR_EL1_IMPLEMENTER_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xffU )

static inline uint64_t _AArch64_Read_midr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, MIDR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* MPIDR_EL1, Multiprocessor Affinity Register */

#define AARCH64_MPIDR_EL1_AFF0( _val ) ( ( _val ) << 0 )
#define AARCH64_MPIDR_EL1_AFF0_SHIFT 0
#define AARCH64_MPIDR_EL1_AFF0_MASK 0xffU
#define AARCH64_MPIDR_EL1_AFF0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

#define AARCH64_MPIDR_EL1_AFF1( _val ) ( ( _val ) << 8 )
#define AARCH64_MPIDR_EL1_AFF1_SHIFT 8
#define AARCH64_MPIDR_EL1_AFF1_MASK 0xff00U
#define AARCH64_MPIDR_EL1_AFF1_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xffU )

#define AARCH64_MPIDR_EL1_AFF2( _val ) ( ( _val ) << 16 )
#define AARCH64_MPIDR_EL1_AFF2_SHIFT 16
#define AARCH64_MPIDR_EL1_AFF2_MASK 0xff0000U
#define AARCH64_MPIDR_EL1_AFF2_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xffU )

#define AARCH64_MPIDR_EL1_MT 0x1000000U

#define AARCH64_MPIDR_EL1_U 0x40000000U

#define AARCH64_MPIDR_EL1_AFF3( _val ) ( ( _val ) << 32 )
#define AARCH64_MPIDR_EL1_AFF3_SHIFT 32
#define AARCH64_MPIDR_EL1_AFF3_MASK 0xff00000000ULL
#define AARCH64_MPIDR_EL1_AFF3_GET( _reg ) \
  ( ( ( _reg ) >> 32 ) & 0xffULL )

static inline uint64_t _AArch64_Read_mpidr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, MPIDR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* MVFR0_EL1, AArch64 Media and VFP Feature Register 0 */

#define AARCH64_MVFR0_EL1_SIMDREG( _val ) ( ( _val ) << 0 )
#define AARCH64_MVFR0_EL1_SIMDREG_SHIFT 0
#define AARCH64_MVFR0_EL1_SIMDREG_MASK 0xfU
#define AARCH64_MVFR0_EL1_SIMDREG_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_MVFR0_EL1_FPSP( _val ) ( ( _val ) << 4 )
#define AARCH64_MVFR0_EL1_FPSP_SHIFT 4
#define AARCH64_MVFR0_EL1_FPSP_MASK 0xf0U
#define AARCH64_MVFR0_EL1_FPSP_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_MVFR0_EL1_FPDP( _val ) ( ( _val ) << 8 )
#define AARCH64_MVFR0_EL1_FPDP_SHIFT 8
#define AARCH64_MVFR0_EL1_FPDP_MASK 0xf00U
#define AARCH64_MVFR0_EL1_FPDP_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_MVFR0_EL1_FPTRAP( _val ) ( ( _val ) << 12 )
#define AARCH64_MVFR0_EL1_FPTRAP_SHIFT 12
#define AARCH64_MVFR0_EL1_FPTRAP_MASK 0xf000U
#define AARCH64_MVFR0_EL1_FPTRAP_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_MVFR0_EL1_FPDIVIDE( _val ) ( ( _val ) << 16 )
#define AARCH64_MVFR0_EL1_FPDIVIDE_SHIFT 16
#define AARCH64_MVFR0_EL1_FPDIVIDE_MASK 0xf0000U
#define AARCH64_MVFR0_EL1_FPDIVIDE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_MVFR0_EL1_FPSQRT( _val ) ( ( _val ) << 20 )
#define AARCH64_MVFR0_EL1_FPSQRT_SHIFT 20
#define AARCH64_MVFR0_EL1_FPSQRT_MASK 0xf00000U
#define AARCH64_MVFR0_EL1_FPSQRT_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_MVFR0_EL1_FPSHVEC( _val ) ( ( _val ) << 24 )
#define AARCH64_MVFR0_EL1_FPSHVEC_SHIFT 24
#define AARCH64_MVFR0_EL1_FPSHVEC_MASK 0xf000000U
#define AARCH64_MVFR0_EL1_FPSHVEC_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH64_MVFR0_EL1_FPROUND( _val ) ( ( _val ) << 28 )
#define AARCH64_MVFR0_EL1_FPROUND_SHIFT 28
#define AARCH64_MVFR0_EL1_FPROUND_MASK 0xf0000000U
#define AARCH64_MVFR0_EL1_FPROUND_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint64_t _AArch64_Read_mvfr0_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, MVFR0_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* MVFR1_EL1, AArch64 Media and VFP Feature Register 1 */

#define AARCH64_MVFR1_EL1_FPFTZ( _val ) ( ( _val ) << 0 )
#define AARCH64_MVFR1_EL1_FPFTZ_SHIFT 0
#define AARCH64_MVFR1_EL1_FPFTZ_MASK 0xfU
#define AARCH64_MVFR1_EL1_FPFTZ_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_MVFR1_EL1_FPDNAN( _val ) ( ( _val ) << 4 )
#define AARCH64_MVFR1_EL1_FPDNAN_SHIFT 4
#define AARCH64_MVFR1_EL1_FPDNAN_MASK 0xf0U
#define AARCH64_MVFR1_EL1_FPDNAN_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_MVFR1_EL1_SIMDLS( _val ) ( ( _val ) << 8 )
#define AARCH64_MVFR1_EL1_SIMDLS_SHIFT 8
#define AARCH64_MVFR1_EL1_SIMDLS_MASK 0xf00U
#define AARCH64_MVFR1_EL1_SIMDLS_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_MVFR1_EL1_SIMDINT( _val ) ( ( _val ) << 12 )
#define AARCH64_MVFR1_EL1_SIMDINT_SHIFT 12
#define AARCH64_MVFR1_EL1_SIMDINT_MASK 0xf000U
#define AARCH64_MVFR1_EL1_SIMDINT_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_MVFR1_EL1_SIMDSP( _val ) ( ( _val ) << 16 )
#define AARCH64_MVFR1_EL1_SIMDSP_SHIFT 16
#define AARCH64_MVFR1_EL1_SIMDSP_MASK 0xf0000U
#define AARCH64_MVFR1_EL1_SIMDSP_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_MVFR1_EL1_SIMDHP( _val ) ( ( _val ) << 20 )
#define AARCH64_MVFR1_EL1_SIMDHP_SHIFT 20
#define AARCH64_MVFR1_EL1_SIMDHP_MASK 0xf00000U
#define AARCH64_MVFR1_EL1_SIMDHP_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_MVFR1_EL1_FPHP( _val ) ( ( _val ) << 24 )
#define AARCH64_MVFR1_EL1_FPHP_SHIFT 24
#define AARCH64_MVFR1_EL1_FPHP_MASK 0xf000000U
#define AARCH64_MVFR1_EL1_FPHP_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )

#define AARCH64_MVFR1_EL1_SIMDFMAC( _val ) ( ( _val ) << 28 )
#define AARCH64_MVFR1_EL1_SIMDFMAC_SHIFT 28
#define AARCH64_MVFR1_EL1_SIMDFMAC_MASK 0xf0000000U
#define AARCH64_MVFR1_EL1_SIMDFMAC_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint64_t _AArch64_Read_mvfr1_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, MVFR1_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* MVFR2_EL1, AArch64 Media and VFP Feature Register 2 */

#define AARCH64_MVFR2_EL1_SIMDMISC( _val ) ( ( _val ) << 0 )
#define AARCH64_MVFR2_EL1_SIMDMISC_SHIFT 0
#define AARCH64_MVFR2_EL1_SIMDMISC_MASK 0xfU
#define AARCH64_MVFR2_EL1_SIMDMISC_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_MVFR2_EL1_FPMISC( _val ) ( ( _val ) << 4 )
#define AARCH64_MVFR2_EL1_FPMISC_SHIFT 4
#define AARCH64_MVFR2_EL1_FPMISC_MASK 0xf0U
#define AARCH64_MVFR2_EL1_FPMISC_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

static inline uint64_t _AArch64_Read_mvfr2_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, MVFR2_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* PAR_EL1, Physical Address Register */

#define AARCH64_PAR_EL1_F 0x1U

#define AARCH64_PAR_EL1_FST( _val ) ( ( _val ) << 1 )
#define AARCH64_PAR_EL1_FST_SHIFT 1
#define AARCH64_PAR_EL1_FST_MASK 0x7eU
#define AARCH64_PAR_EL1_FST_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x3fU )

#define AARCH64_PAR_EL1_SH( _val ) ( ( _val ) << 7 )
#define AARCH64_PAR_EL1_SH_SHIFT 7
#define AARCH64_PAR_EL1_SH_MASK 0x180U
#define AARCH64_PAR_EL1_SH_GET( _reg ) \
  ( ( ( _reg ) >> 7 ) & 0x3U )

#define AARCH64_PAR_EL1_PTW 0x100U

#define AARCH64_PAR_EL1_NS 0x200U

#define AARCH64_PAR_EL1_S 0x200U

#define AARCH64_PAR_EL1_PA_47_12( _val ) ( ( _val ) << 12 )
#define AARCH64_PAR_EL1_PA_47_12_SHIFT 12
#define AARCH64_PAR_EL1_PA_47_12_MASK 0xfffffffff000ULL
#define AARCH64_PAR_EL1_PA_47_12_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfffffffffULL )

#define AARCH64_PAR_EL1_PA_51_48( _val ) ( ( _val ) << 48 )
#define AARCH64_PAR_EL1_PA_51_48_SHIFT 48
#define AARCH64_PAR_EL1_PA_51_48_MASK 0xf000000000000ULL
#define AARCH64_PAR_EL1_PA_51_48_GET( _reg ) \
  ( ( ( _reg ) >> 48 ) & 0xfULL )

#define AARCH64_PAR_EL1_ATTR( _val ) ( ( _val ) << 56 )
#define AARCH64_PAR_EL1_ATTR_SHIFT 56
#define AARCH64_PAR_EL1_ATTR_MASK 0xff00000000000000ULL
#define AARCH64_PAR_EL1_ATTR_GET( _reg ) \
  ( ( ( _reg ) >> 56 ) & 0xffULL )

static inline uint64_t _AArch64_Read_par_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PAR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_par_el1( uint64_t value )
{
  __asm__ volatile (
    "msr PAR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* REVIDR_EL1, Revision ID Register */

static inline uint64_t _AArch64_Read_revidr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, REVIDR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* RGSR_EL1, Random Allocation Tag Seed Register. */

#define AARCH64_RGSR_EL1_TAG( _val ) ( ( _val ) << 0 )
#define AARCH64_RGSR_EL1_TAG_SHIFT 0
#define AARCH64_RGSR_EL1_TAG_MASK 0xfU
#define AARCH64_RGSR_EL1_TAG_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_RGSR_EL1_SEED( _val ) ( ( _val ) << 8 )
#define AARCH64_RGSR_EL1_SEED_SHIFT 8
#define AARCH64_RGSR_EL1_SEED_MASK 0xffff00U
#define AARCH64_RGSR_EL1_SEED_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xffffU )

static inline uint64_t _AArch64_Read_rgsr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, RGSR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_rgsr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr RGSR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* RMR_EL1, Reset Management Register (EL1) */

#define AARCH64_RMR_EL1_AA64 0x1U

#define AARCH64_RMR_EL1_RR 0x2U

static inline uint64_t _AArch64_Read_rmr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, RMR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_rmr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr RMR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* RMR_EL2, Reset Management Register (EL2) */

#define AARCH64_RMR_EL2_AA64 0x1U

#define AARCH64_RMR_EL2_RR 0x2U

static inline uint64_t _AArch64_Read_rmr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, RMR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_rmr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr RMR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* RMR_EL3, Reset Management Register (EL3) */

#define AARCH64_RMR_EL3_AA64 0x1U

#define AARCH64_RMR_EL3_RR 0x2U

static inline uint64_t _AArch64_Read_rmr_el3( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, RMR_EL3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_rmr_el3( uint64_t value )
{
  __asm__ volatile (
    "msr RMR_EL3, %0" : : "r" ( value ) : "memory"
  );
}

/* RNDR, Random Number */

static inline uint64_t _AArch64_Read_rndr( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, RNDR" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* RNDRRS, Reseeded Random Number */

static inline uint64_t _AArch64_Read_rndrrs( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, RNDRRS" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* RVBAR_EL1, Reset Vector Base Address Register (if EL2 and EL3 not implemented) */

static inline uint64_t _AArch64_Read_rvbar_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, RVBAR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* RVBAR_EL2, Reset Vector Base Address Register (if EL3 not implemented) */

static inline uint64_t _AArch64_Read_rvbar_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, RVBAR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* RVBAR_EL3, Reset Vector Base Address Register (if EL3 implemented) */

static inline uint64_t _AArch64_Read_rvbar_el3( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, RVBAR_EL3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* SCR_EL3, Secure Configuration Register */

#define AARCH64_SCR_EL3_NS 0x1U

#define AARCH64_SCR_EL3_IRQ 0x2U

#define AARCH64_SCR_EL3_FIQ 0x4U

#define AARCH64_SCR_EL3_EA 0x8U

#define AARCH64_SCR_EL3_SMD 0x80U

#define AARCH64_SCR_EL3_HCE 0x100U

#define AARCH64_SCR_EL3_SIF 0x200U

#define AARCH64_SCR_EL3_RW 0x400U

#define AARCH64_SCR_EL3_ST 0x800U

#define AARCH64_SCR_EL3_TWI 0x1000U

#define AARCH64_SCR_EL3_TWE 0x2000U

#define AARCH64_SCR_EL3_TLOR 0x4000U

#define AARCH64_SCR_EL3_TERR 0x8000U

#define AARCH64_SCR_EL3_APK 0x10000U

#define AARCH64_SCR_EL3_API 0x20000U

#define AARCH64_SCR_EL3_EEL2 0x40000U

#define AARCH64_SCR_EL3_EASE 0x80000U

#define AARCH64_SCR_EL3_NMEA 0x100000U

#define AARCH64_SCR_EL3_FIEN 0x200000U

#define AARCH64_SCR_EL3_ENSCXT 0x2000000U

#define AARCH64_SCR_EL3_ATA 0x4000000U

#define AARCH64_SCR_EL3_FGTEN 0x8000000U

#define AARCH64_SCR_EL3_ECVEN 0x10000000U

#define AARCH64_SCR_EL3_TWEDEN 0x20000000U

#define AARCH64_SCR_EL3_TWEDEL( _val ) ( ( _val ) << 30 )
#define AARCH64_SCR_EL3_TWEDEL_SHIFT 30
#define AARCH64_SCR_EL3_TWEDEL_MASK 0x3c0000000ULL
#define AARCH64_SCR_EL3_TWEDEL_GET( _reg ) \
  ( ( ( _reg ) >> 30 ) & 0xfULL )

#define AARCH64_SCR_EL3_AMVOFFEN 0x800000000ULL

static inline uint64_t _AArch64_Read_scr_el3( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, SCR_EL3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_scr_el3( uint64_t value )
{
  __asm__ volatile (
    "msr SCR_EL3, %0" : : "r" ( value ) : "memory"
  );
}

/* SCTLR_EL1, System Control Register (EL1) */

#define AARCH64_SCTLR_EL1_M 0x1U

#define AARCH64_SCTLR_EL1_A 0x2U

#define AARCH64_SCTLR_EL1_C 0x4U

#define AARCH64_SCTLR_EL1_SA 0x8U

#define AARCH64_SCTLR_EL1_SA0 0x10U

#define AARCH64_SCTLR_EL1_CP15BEN 0x20U

#define AARCH64_SCTLR_EL1_NAA 0x40U

#define AARCH64_SCTLR_EL1_ITD 0x80U

#define AARCH64_SCTLR_EL1_SED 0x100U

#define AARCH64_SCTLR_EL1_UMA 0x200U

#define AARCH64_SCTLR_EL1_ENRCTX 0x400U

#define AARCH64_SCTLR_EL1_EOS 0x800U

#define AARCH64_SCTLR_EL1_I 0x1000U

#define AARCH64_SCTLR_EL1_ENDB 0x2000U

#define AARCH64_SCTLR_EL1_DZE 0x4000U

#define AARCH64_SCTLR_EL1_UCT 0x8000U

#define AARCH64_SCTLR_EL1_NTWI 0x10000U

#define AARCH64_SCTLR_EL1_NTWE 0x40000U

#define AARCH64_SCTLR_EL1_WXN 0x80000U

#define AARCH64_SCTLR_EL1_TSCXT 0x100000U

#define AARCH64_SCTLR_EL1_IESB 0x200000U

#define AARCH64_SCTLR_EL1_EIS 0x400000U

#define AARCH64_SCTLR_EL1_SPAN 0x800000U

#define AARCH64_SCTLR_EL1_E0E 0x1000000U

#define AARCH64_SCTLR_EL1_EE 0x2000000U

#define AARCH64_SCTLR_EL1_UCI 0x4000000U

#define AARCH64_SCTLR_EL1_ENDA 0x8000000U

#define AARCH64_SCTLR_EL1_NTLSMD 0x10000000U

#define AARCH64_SCTLR_EL1_LSMAOE 0x20000000U

#define AARCH64_SCTLR_EL1_ENIB 0x40000000U

#define AARCH64_SCTLR_EL1_ENIA 0x80000000U

#define AARCH64_SCTLR_EL1_BT0 0x800000000ULL

#define AARCH64_SCTLR_EL1_BT1 0x1000000000ULL

#define AARCH64_SCTLR_EL1_ITFSB 0x2000000000ULL

#define AARCH64_SCTLR_EL1_TCF0( _val ) ( ( _val ) << 38 )
#define AARCH64_SCTLR_EL1_TCF0_SHIFT 38
#define AARCH64_SCTLR_EL1_TCF0_MASK 0xc000000000ULL
#define AARCH64_SCTLR_EL1_TCF0_GET( _reg ) \
  ( ( ( _reg ) >> 38 ) & 0x3ULL )

#define AARCH64_SCTLR_EL1_TCF( _val ) ( ( _val ) << 40 )
#define AARCH64_SCTLR_EL1_TCF_SHIFT 40
#define AARCH64_SCTLR_EL1_TCF_MASK 0x30000000000ULL
#define AARCH64_SCTLR_EL1_TCF_GET( _reg ) \
  ( ( ( _reg ) >> 40 ) & 0x3ULL )

#define AARCH64_SCTLR_EL1_ATA0 0x40000000000ULL

#define AARCH64_SCTLR_EL1_ATA 0x80000000000ULL

#define AARCH64_SCTLR_EL1_DSSBS 0x100000000000ULL

#define AARCH64_SCTLR_EL1_TWEDEN 0x200000000000ULL

#define AARCH64_SCTLR_EL1_TWEDEL( _val ) ( ( _val ) << 46 )
#define AARCH64_SCTLR_EL1_TWEDEL_SHIFT 46
#define AARCH64_SCTLR_EL1_TWEDEL_MASK 0x3c00000000000ULL
#define AARCH64_SCTLR_EL1_TWEDEL_GET( _reg ) \
  ( ( ( _reg ) >> 46 ) & 0xfULL )

static inline uint64_t _AArch64_Read_sctlr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, SCTLR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_sctlr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr SCTLR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* SCTLR_EL2, System Control Register (EL2) */

#define AARCH64_SCTLR_EL2_M 0x1U

#define AARCH64_SCTLR_EL2_A 0x2U

#define AARCH64_SCTLR_EL2_C 0x4U

#define AARCH64_SCTLR_EL2_SA 0x8U

#define AARCH64_SCTLR_EL2_SA0 0x10U

#define AARCH64_SCTLR_EL2_CP15BEN 0x20U

#define AARCH64_SCTLR_EL2_NAA 0x40U

#define AARCH64_SCTLR_EL2_ITD 0x80U

#define AARCH64_SCTLR_EL2_SED 0x100U

#define AARCH64_SCTLR_EL2_ENRCTX 0x400U

#define AARCH64_SCTLR_EL2_EOS 0x800U

#define AARCH64_SCTLR_EL2_I 0x1000U

#define AARCH64_SCTLR_EL2_ENDB 0x2000U

#define AARCH64_SCTLR_EL2_DZE 0x4000U

#define AARCH64_SCTLR_EL2_UCT 0x8000U

#define AARCH64_SCTLR_EL2_NTWI 0x10000U

#define AARCH64_SCTLR_EL2_NTWE 0x40000U

#define AARCH64_SCTLR_EL2_WXN 0x80000U

#define AARCH64_SCTLR_EL2_TSCXT 0x100000U

#define AARCH64_SCTLR_EL2_IESB 0x200000U

#define AARCH64_SCTLR_EL2_EIS 0x400000U

#define AARCH64_SCTLR_EL2_SPAN 0x800000U

#define AARCH64_SCTLR_EL2_E0E 0x1000000U

#define AARCH64_SCTLR_EL2_EE 0x2000000U

#define AARCH64_SCTLR_EL2_UCI 0x4000000U

#define AARCH64_SCTLR_EL2_ENDA 0x8000000U

#define AARCH64_SCTLR_EL2_NTLSMD 0x10000000U

#define AARCH64_SCTLR_EL2_LSMAOE 0x20000000U

#define AARCH64_SCTLR_EL2_ENIB 0x40000000U

#define AARCH64_SCTLR_EL2_ENIA 0x80000000U

#define AARCH64_SCTLR_EL2_BT0 0x800000000ULL

#define AARCH64_SCTLR_EL2_BT 0x1000000000ULL

#define AARCH64_SCTLR_EL2_BT1 0x1000000000ULL

#define AARCH64_SCTLR_EL2_ITFSB 0x2000000000ULL

#define AARCH64_SCTLR_EL2_TCF0( _val ) ( ( _val ) << 38 )
#define AARCH64_SCTLR_EL2_TCF0_SHIFT 38
#define AARCH64_SCTLR_EL2_TCF0_MASK 0xc000000000ULL
#define AARCH64_SCTLR_EL2_TCF0_GET( _reg ) \
  ( ( ( _reg ) >> 38 ) & 0x3ULL )

#define AARCH64_SCTLR_EL2_TCF( _val ) ( ( _val ) << 40 )
#define AARCH64_SCTLR_EL2_TCF_SHIFT 40
#define AARCH64_SCTLR_EL2_TCF_MASK 0x30000000000ULL
#define AARCH64_SCTLR_EL2_TCF_GET( _reg ) \
  ( ( ( _reg ) >> 40 ) & 0x3ULL )

#define AARCH64_SCTLR_EL2_ATA0 0x40000000000ULL

#define AARCH64_SCTLR_EL2_ATA 0x80000000000ULL

#define AARCH64_SCTLR_EL2_DSSBS 0x100000000000ULL

#define AARCH64_SCTLR_EL2_TWEDEN 0x200000000000ULL

#define AARCH64_SCTLR_EL2_TWEDEL( _val ) ( ( _val ) << 46 )
#define AARCH64_SCTLR_EL2_TWEDEL_SHIFT 46
#define AARCH64_SCTLR_EL2_TWEDEL_MASK 0x3c00000000000ULL
#define AARCH64_SCTLR_EL2_TWEDEL_GET( _reg ) \
  ( ( ( _reg ) >> 46 ) & 0xfULL )

static inline uint64_t _AArch64_Read_sctlr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, SCTLR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_sctlr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr SCTLR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* SCTLR_EL3, System Control Register (EL3) */

#define AARCH64_SCTLR_EL3_M 0x1U

#define AARCH64_SCTLR_EL3_A 0x2U

#define AARCH64_SCTLR_EL3_C 0x4U

#define AARCH64_SCTLR_EL3_SA 0x8U

#define AARCH64_SCTLR_EL3_NAA 0x40U

#define AARCH64_SCTLR_EL3_EOS 0x800U

#define AARCH64_SCTLR_EL3_I 0x1000U

#define AARCH64_SCTLR_EL3_ENDB 0x2000U

#define AARCH64_SCTLR_EL3_WXN 0x80000U

#define AARCH64_SCTLR_EL3_IESB 0x200000U

#define AARCH64_SCTLR_EL3_EIS 0x400000U

#define AARCH64_SCTLR_EL3_EE 0x2000000U

#define AARCH64_SCTLR_EL3_ENDA 0x8000000U

#define AARCH64_SCTLR_EL3_ENIB 0x40000000U

#define AARCH64_SCTLR_EL3_ENIA 0x80000000U

#define AARCH64_SCTLR_EL3_BT 0x1000000000ULL

#define AARCH64_SCTLR_EL3_ITFSB 0x2000000000ULL

#define AARCH64_SCTLR_EL3_TCF( _val ) ( ( _val ) << 40 )
#define AARCH64_SCTLR_EL3_TCF_SHIFT 40
#define AARCH64_SCTLR_EL3_TCF_MASK 0x30000000000ULL
#define AARCH64_SCTLR_EL3_TCF_GET( _reg ) \
  ( ( ( _reg ) >> 40 ) & 0x3ULL )

#define AARCH64_SCTLR_EL3_ATA 0x80000000000ULL

#define AARCH64_SCTLR_EL3_DSSBS 0x100000000000ULL

static inline uint64_t _AArch64_Read_sctlr_el3( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, SCTLR_EL3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_sctlr_el3( uint64_t value )
{
  __asm__ volatile (
    "msr SCTLR_EL3, %0" : : "r" ( value ) : "memory"
  );
}

/* SCXTNUM_EL0, EL0 Read/Write Software Context Number */

static inline uint64_t _AArch64_Read_scxtnum_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, SCXTNUM_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_scxtnum_el0( uint64_t value )
{
  __asm__ volatile (
    "msr SCXTNUM_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* SCXTNUM_EL1, EL1 Read/Write Software Context Number */

static inline uint64_t _AArch64_Read_scxtnum_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, SCXTNUM_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_scxtnum_el1( uint64_t value )
{
  __asm__ volatile (
    "msr SCXTNUM_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* SCXTNUM_EL2, EL2 Read/Write Software Context Number */

static inline uint64_t _AArch64_Read_scxtnum_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, SCXTNUM_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_scxtnum_el2( uint64_t value )
{
  __asm__ volatile (
    "msr SCXTNUM_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* SCXTNUM_EL3, EL3 Read/Write Software Context Number */

static inline uint64_t _AArch64_Read_scxtnum_el3( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, SCXTNUM_EL3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_scxtnum_el3( uint64_t value )
{
  __asm__ volatile (
    "msr SCXTNUM_EL3, %0" : : "r" ( value ) : "memory"
  );
}

/* TCR_EL1, Translation Control Register (EL1) */

#define AARCH64_TCR_EL1_T0SZ( _val ) ( ( _val ) << 0 )
#define AARCH64_TCR_EL1_T0SZ_SHIFT 0
#define AARCH64_TCR_EL1_T0SZ_MASK 0x3fU
#define AARCH64_TCR_EL1_T0SZ_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )

#define AARCH64_TCR_EL1_EPD0 0x80U

#define AARCH64_TCR_EL1_IRGN0( _val ) ( ( _val ) << 8 )
#define AARCH64_TCR_EL1_IRGN0_SHIFT 8
#define AARCH64_TCR_EL1_IRGN0_MASK 0x300U
#define AARCH64_TCR_EL1_IRGN0_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0x3U )

#define AARCH64_TCR_EL1_ORGN0( _val ) ( ( _val ) << 10 )
#define AARCH64_TCR_EL1_ORGN0_SHIFT 10
#define AARCH64_TCR_EL1_ORGN0_MASK 0xc00U
#define AARCH64_TCR_EL1_ORGN0_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3U )

#define AARCH64_TCR_EL1_SH0( _val ) ( ( _val ) << 12 )
#define AARCH64_TCR_EL1_SH0_SHIFT 12
#define AARCH64_TCR_EL1_SH0_MASK 0x3000U
#define AARCH64_TCR_EL1_SH0_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0x3U )

#define AARCH64_TCR_EL1_TG0( _val ) ( ( _val ) << 14 )
#define AARCH64_TCR_EL1_TG0_SHIFT 14
#define AARCH64_TCR_EL1_TG0_MASK 0xc000U
#define AARCH64_TCR_EL1_TG0_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x3U )

#define AARCH64_TCR_EL1_T1SZ( _val ) ( ( _val ) << 16 )
#define AARCH64_TCR_EL1_T1SZ_SHIFT 16
#define AARCH64_TCR_EL1_T1SZ_MASK 0x3f0000U
#define AARCH64_TCR_EL1_T1SZ_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0x3fU )

#define AARCH64_TCR_EL1_A1 0x400000U

#define AARCH64_TCR_EL1_EPD1 0x800000U

#define AARCH64_TCR_EL1_IRGN1( _val ) ( ( _val ) << 24 )
#define AARCH64_TCR_EL1_IRGN1_SHIFT 24
#define AARCH64_TCR_EL1_IRGN1_MASK 0x3000000U
#define AARCH64_TCR_EL1_IRGN1_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0x3U )

#define AARCH64_TCR_EL1_ORGN1( _val ) ( ( _val ) << 26 )
#define AARCH64_TCR_EL1_ORGN1_SHIFT 26
#define AARCH64_TCR_EL1_ORGN1_MASK 0xc000000U
#define AARCH64_TCR_EL1_ORGN1_GET( _reg ) \
  ( ( ( _reg ) >> 26 ) & 0x3U )

#define AARCH64_TCR_EL1_SH1( _val ) ( ( _val ) << 28 )
#define AARCH64_TCR_EL1_SH1_SHIFT 28
#define AARCH64_TCR_EL1_SH1_MASK 0x30000000U
#define AARCH64_TCR_EL1_SH1_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0x3U )

#define AARCH64_TCR_EL1_TG1( _val ) ( ( _val ) << 30 )
#define AARCH64_TCR_EL1_TG1_SHIFT 30
#define AARCH64_TCR_EL1_TG1_MASK 0xc0000000U
#define AARCH64_TCR_EL1_TG1_GET( _reg ) \
  ( ( ( _reg ) >> 30 ) & 0x3U )

#define AARCH64_TCR_EL1_IPS( _val ) ( ( _val ) << 32 )
#define AARCH64_TCR_EL1_IPS_SHIFT 32
#define AARCH64_TCR_EL1_IPS_MASK 0x700000000ULL
#define AARCH64_TCR_EL1_IPS_GET( _reg ) \
  ( ( ( _reg ) >> 32 ) & 0x7ULL )

#define AARCH64_TCR_EL1_AS 0x1000000000ULL

#define AARCH64_TCR_EL1_TBI0 0x2000000000ULL

#define AARCH64_TCR_EL1_TBI1 0x4000000000ULL

#define AARCH64_TCR_EL1_HA 0x8000000000ULL

#define AARCH64_TCR_EL1_HD 0x10000000000ULL

#define AARCH64_TCR_EL1_HPD0 0x20000000000ULL

#define AARCH64_TCR_EL1_HPD1 0x40000000000ULL

#define AARCH64_TCR_EL1_HWU059 0x80000000000ULL

#define AARCH64_TCR_EL1_HWU060 0x100000000000ULL

#define AARCH64_TCR_EL1_HWU061 0x200000000000ULL

#define AARCH64_TCR_EL1_HWU062 0x400000000000ULL

#define AARCH64_TCR_EL1_HWU159 0x800000000000ULL

#define AARCH64_TCR_EL1_HWU160 0x1000000000000ULL

#define AARCH64_TCR_EL1_HWU161 0x2000000000000ULL

#define AARCH64_TCR_EL1_HWU162 0x4000000000000ULL

#define AARCH64_TCR_EL1_TBID0 0x8000000000000ULL

#define AARCH64_TCR_EL1_TBID1 0x10000000000000ULL

#define AARCH64_TCR_EL1_NFD0 0x20000000000000ULL

#define AARCH64_TCR_EL1_NFD1 0x40000000000000ULL

#define AARCH64_TCR_EL1_E0PD0 0x80000000000000ULL

#define AARCH64_TCR_EL1_E0PD1 0x100000000000000ULL

#define AARCH64_TCR_EL1_TCMA0 0x200000000000000ULL

#define AARCH64_TCR_EL1_TCMA1 0x400000000000000ULL

static inline uint64_t _AArch64_Read_tcr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, TCR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_tcr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr TCR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* TCR_EL2, Translation Control Register (EL2) */

#define AARCH64_TCR_EL2_T0SZ( _val ) ( ( _val ) << 0 )
#define AARCH64_TCR_EL2_T0SZ_SHIFT 0
#define AARCH64_TCR_EL2_T0SZ_MASK 0x3fU
#define AARCH64_TCR_EL2_T0SZ_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )

#define AARCH64_TCR_EL2_EPD0 0x80U

#define AARCH64_TCR_EL2_IRGN0( _val ) ( ( _val ) << 8 )
#define AARCH64_TCR_EL2_IRGN0_SHIFT 8
#define AARCH64_TCR_EL2_IRGN0_MASK 0x300U
#define AARCH64_TCR_EL2_IRGN0_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0x3U )

#define AARCH64_TCR_EL2_ORGN0( _val ) ( ( _val ) << 10 )
#define AARCH64_TCR_EL2_ORGN0_SHIFT 10
#define AARCH64_TCR_EL2_ORGN0_MASK 0xc00U
#define AARCH64_TCR_EL2_ORGN0_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3U )

#define AARCH64_TCR_EL2_SH0( _val ) ( ( _val ) << 12 )
#define AARCH64_TCR_EL2_SH0_SHIFT 12
#define AARCH64_TCR_EL2_SH0_MASK 0x3000U
#define AARCH64_TCR_EL2_SH0_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0x3U )

#define AARCH64_TCR_EL2_TG0( _val ) ( ( _val ) << 14 )
#define AARCH64_TCR_EL2_TG0_SHIFT 14
#define AARCH64_TCR_EL2_TG0_MASK 0xc000U
#define AARCH64_TCR_EL2_TG0_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x3U )

#define AARCH64_TCR_EL2_PS( _val ) ( ( _val ) << 16 )
#define AARCH64_TCR_EL2_PS_SHIFT 16
#define AARCH64_TCR_EL2_PS_MASK 0x70000U
#define AARCH64_TCR_EL2_PS_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0x7U )

#define AARCH64_TCR_EL2_T1SZ( _val ) ( ( _val ) << 16 )
#define AARCH64_TCR_EL2_T1SZ_SHIFT 16
#define AARCH64_TCR_EL2_T1SZ_MASK 0x3f0000U
#define AARCH64_TCR_EL2_T1SZ_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0x3fU )

#define AARCH64_TCR_EL2_TBI 0x100000U

#define AARCH64_TCR_EL2_HA_0 0x200000U

#define AARCH64_TCR_EL2_A1 0x400000U

#define AARCH64_TCR_EL2_HD_0 0x400000U

#define AARCH64_TCR_EL2_EPD1 0x800000U

#define AARCH64_TCR_EL2_HPD 0x1000000U

#define AARCH64_TCR_EL2_IRGN1( _val ) ( ( _val ) << 24 )
#define AARCH64_TCR_EL2_IRGN1_SHIFT 24
#define AARCH64_TCR_EL2_IRGN1_MASK 0x3000000U
#define AARCH64_TCR_EL2_IRGN1_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0x3U )

#define AARCH64_TCR_EL2_HWU59 0x2000000U

#define AARCH64_TCR_EL2_HWU60 0x4000000U

#define AARCH64_TCR_EL2_ORGN1( _val ) ( ( _val ) << 26 )
#define AARCH64_TCR_EL2_ORGN1_SHIFT 26
#define AARCH64_TCR_EL2_ORGN1_MASK 0xc000000U
#define AARCH64_TCR_EL2_ORGN1_GET( _reg ) \
  ( ( ( _reg ) >> 26 ) & 0x3U )

#define AARCH64_TCR_EL2_HWU61 0x8000000U

#define AARCH64_TCR_EL2_HWU62 0x10000000U

#define AARCH64_TCR_EL2_SH1( _val ) ( ( _val ) << 28 )
#define AARCH64_TCR_EL2_SH1_SHIFT 28
#define AARCH64_TCR_EL2_SH1_MASK 0x30000000U
#define AARCH64_TCR_EL2_SH1_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0x3U )

#define AARCH64_TCR_EL2_TBID 0x20000000U

#define AARCH64_TCR_EL2_TCMA 0x40000000U

#define AARCH64_TCR_EL2_TG1( _val ) ( ( _val ) << 30 )
#define AARCH64_TCR_EL2_TG1_SHIFT 30
#define AARCH64_TCR_EL2_TG1_MASK 0xc0000000U
#define AARCH64_TCR_EL2_TG1_GET( _reg ) \
  ( ( ( _reg ) >> 30 ) & 0x3U )

#define AARCH64_TCR_EL2_IPS( _val ) ( ( _val ) << 32 )
#define AARCH64_TCR_EL2_IPS_SHIFT 32
#define AARCH64_TCR_EL2_IPS_MASK 0x700000000ULL
#define AARCH64_TCR_EL2_IPS_GET( _reg ) \
  ( ( ( _reg ) >> 32 ) & 0x7ULL )

#define AARCH64_TCR_EL2_AS 0x1000000000ULL

#define AARCH64_TCR_EL2_TBI0 0x2000000000ULL

#define AARCH64_TCR_EL2_TBI1 0x4000000000ULL

#define AARCH64_TCR_EL2_HA_1 0x8000000000ULL

#define AARCH64_TCR_EL2_HD_1 0x10000000000ULL

#define AARCH64_TCR_EL2_HPD0 0x20000000000ULL

#define AARCH64_TCR_EL2_HPD1 0x40000000000ULL

#define AARCH64_TCR_EL2_HWU059 0x80000000000ULL

#define AARCH64_TCR_EL2_HWU060 0x100000000000ULL

#define AARCH64_TCR_EL2_HWU061 0x200000000000ULL

#define AARCH64_TCR_EL2_HWU062 0x400000000000ULL

#define AARCH64_TCR_EL2_HWU159 0x800000000000ULL

#define AARCH64_TCR_EL2_HWU160 0x1000000000000ULL

#define AARCH64_TCR_EL2_HWU161 0x2000000000000ULL

#define AARCH64_TCR_EL2_HWU162 0x4000000000000ULL

#define AARCH64_TCR_EL2_TBID0 0x8000000000000ULL

#define AARCH64_TCR_EL2_TBID1 0x10000000000000ULL

#define AARCH64_TCR_EL2_NFD0 0x20000000000000ULL

#define AARCH64_TCR_EL2_NFD1 0x40000000000000ULL

#define AARCH64_TCR_EL2_E0PD0 0x80000000000000ULL

#define AARCH64_TCR_EL2_E0PD1 0x100000000000000ULL

#define AARCH64_TCR_EL2_TCMA0 0x200000000000000ULL

#define AARCH64_TCR_EL2_TCMA1 0x400000000000000ULL

static inline uint64_t _AArch64_Read_tcr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, TCR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_tcr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr TCR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* TCR_EL3, Translation Control Register (EL3) */

#define AARCH64_TCR_EL3_T0SZ( _val ) ( ( _val ) << 0 )
#define AARCH64_TCR_EL3_T0SZ_SHIFT 0
#define AARCH64_TCR_EL3_T0SZ_MASK 0x3fU
#define AARCH64_TCR_EL3_T0SZ_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )

#define AARCH64_TCR_EL3_IRGN0( _val ) ( ( _val ) << 8 )
#define AARCH64_TCR_EL3_IRGN0_SHIFT 8
#define AARCH64_TCR_EL3_IRGN0_MASK 0x300U
#define AARCH64_TCR_EL3_IRGN0_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0x3U )

#define AARCH64_TCR_EL3_ORGN0( _val ) ( ( _val ) << 10 )
#define AARCH64_TCR_EL3_ORGN0_SHIFT 10
#define AARCH64_TCR_EL3_ORGN0_MASK 0xc00U
#define AARCH64_TCR_EL3_ORGN0_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3U )

#define AARCH64_TCR_EL3_SH0( _val ) ( ( _val ) << 12 )
#define AARCH64_TCR_EL3_SH0_SHIFT 12
#define AARCH64_TCR_EL3_SH0_MASK 0x3000U
#define AARCH64_TCR_EL3_SH0_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0x3U )

#define AARCH64_TCR_EL3_TG0( _val ) ( ( _val ) << 14 )
#define AARCH64_TCR_EL3_TG0_SHIFT 14
#define AARCH64_TCR_EL3_TG0_MASK 0xc000U
#define AARCH64_TCR_EL3_TG0_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x3U )

#define AARCH64_TCR_EL3_PS( _val ) ( ( _val ) << 16 )
#define AARCH64_TCR_EL3_PS_SHIFT 16
#define AARCH64_TCR_EL3_PS_MASK 0x70000U
#define AARCH64_TCR_EL3_PS_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0x7U )

#define AARCH64_TCR_EL3_TBI 0x100000U

#define AARCH64_TCR_EL3_HA 0x200000U

#define AARCH64_TCR_EL3_HD 0x400000U

#define AARCH64_TCR_EL3_HPD 0x1000000U

#define AARCH64_TCR_EL3_HWU59 0x2000000U

#define AARCH64_TCR_EL3_HWU60 0x4000000U

#define AARCH64_TCR_EL3_HWU61 0x8000000U

#define AARCH64_TCR_EL3_HWU62 0x10000000U

#define AARCH64_TCR_EL3_TBID 0x20000000U

#define AARCH64_TCR_EL3_TCMA 0x40000000U

static inline uint64_t _AArch64_Read_tcr_el3( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, TCR_EL3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_tcr_el3( uint64_t value )
{
  __asm__ volatile (
    "msr TCR_EL3, %0" : : "r" ( value ) : "memory"
  );
}

/* TFSRE0_EL1, Tag Fault Status Register (EL0). */

#define AARCH64_TFSRE0_EL1_TF0 0x1U

#define AARCH64_TFSRE0_EL1_TF1 0x2U

static inline uint64_t _AArch64_Read_tfsre0_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, TFSRE0_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_tfsre0_el1( uint64_t value )
{
  __asm__ volatile (
    "msr TFSRE0_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* TFSR_EL1, Tag Fault Status Register (EL1) */

#define AARCH64_TFSR_EL1_TF0 0x1U

#define AARCH64_TFSR_EL1_TF1 0x2U

static inline uint64_t _AArch64_Read_tfsr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, TFSR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_tfsr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr TFSR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* TFSR_EL2, Tag Fault Status Register (EL2) */

#define AARCH64_TFSR_EL2_TF0 0x1U

#define AARCH64_TFSR_EL2_TF1 0x2U

static inline uint64_t _AArch64_Read_tfsr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, TFSR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_tfsr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr TFSR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* TFSR_EL3, Tag Fault Status Register (EL3) */

#define AARCH64_TFSR_EL3_TF0 0x1U

static inline uint64_t _AArch64_Read_tfsr_el3( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, TFSR_EL3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_tfsr_el3( uint64_t value )
{
  __asm__ volatile (
    "msr TFSR_EL3, %0" : : "r" ( value ) : "memory"
  );
}

/* TPIDR_EL0, EL0 Read/Write Software Thread ID Register */

static inline uint64_t _AArch64_Read_tpidr_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, TPIDR_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_tpidr_el0( uint64_t value )
{
  __asm__ volatile (
    "msr TPIDR_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* TPIDR_EL1, EL1 Software Thread ID Register */

static inline uint64_t _AArch64_Read_tpidr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, TPIDR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_tpidr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr TPIDR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* TPIDR_EL2, EL2 Software Thread ID Register */

static inline uint64_t _AArch64_Read_tpidr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, TPIDR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_tpidr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr TPIDR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* TPIDR_EL3, EL3 Software Thread ID Register */

static inline uint64_t _AArch64_Read_tpidr_el3( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, TPIDR_EL3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_tpidr_el3( uint64_t value )
{
  __asm__ volatile (
    "msr TPIDR_EL3, %0" : : "r" ( value ) : "memory"
  );
}

/* TPIDRRO_EL0, EL0 Read-Only Software Thread ID Register */

static inline uint64_t _AArch64_Read_tpidrro_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, TPIDRRO_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_tpidrro_el0( uint64_t value )
{
  __asm__ volatile (
    "msr TPIDRRO_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* TTBR0_EL1, Translation Table Base Register 0 (EL1) */

#define AARCH64_TTBR0_EL1_CNP 0x1U

#define AARCH64_TTBR0_EL1_BADDR( _val ) ( ( _val ) << 1 )
#define AARCH64_TTBR0_EL1_BADDR_SHIFT 1
#define AARCH64_TTBR0_EL1_BADDR_MASK 0xfffffffffffeULL
#define AARCH64_TTBR0_EL1_BADDR_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x7fffffffffffULL )

#define AARCH64_TTBR0_EL1_ASID( _val ) ( ( _val ) << 48 )
#define AARCH64_TTBR0_EL1_ASID_SHIFT 48
#define AARCH64_TTBR0_EL1_ASID_MASK 0xffff000000000000ULL
#define AARCH64_TTBR0_EL1_ASID_GET( _reg ) \
  ( ( ( _reg ) >> 48 ) & 0xffffULL )

static inline uint64_t _AArch64_Read_ttbr0_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, TTBR0_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_ttbr0_el1( uint64_t value )
{
  __asm__ volatile (
    "msr TTBR0_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* TTBR0_EL2, Translation Table Base Register 0 (EL2) */

#define AARCH64_TTBR0_EL2_CNP 0x1U

#define AARCH64_TTBR0_EL2_BADDR( _val ) ( ( _val ) << 1 )
#define AARCH64_TTBR0_EL2_BADDR_SHIFT 1
#define AARCH64_TTBR0_EL2_BADDR_MASK 0xfffffffffffeULL
#define AARCH64_TTBR0_EL2_BADDR_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x7fffffffffffULL )

#define AARCH64_TTBR0_EL2_ASID( _val ) ( ( _val ) << 48 )
#define AARCH64_TTBR0_EL2_ASID_SHIFT 48
#define AARCH64_TTBR0_EL2_ASID_MASK 0xffff000000000000ULL
#define AARCH64_TTBR0_EL2_ASID_GET( _reg ) \
  ( ( ( _reg ) >> 48 ) & 0xffffULL )

static inline uint64_t _AArch64_Read_ttbr0_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, TTBR0_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_ttbr0_el2( uint64_t value )
{
  __asm__ volatile (
    "msr TTBR0_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* TTBR0_EL3, Translation Table Base Register 0 (EL3) */

#define AARCH64_TTBR0_EL3_CNP 0x1U

#define AARCH64_TTBR0_EL3_BADDR( _val ) ( ( _val ) << 1 )
#define AARCH64_TTBR0_EL3_BADDR_SHIFT 1
#define AARCH64_TTBR0_EL3_BADDR_MASK 0xfffffffffffeULL
#define AARCH64_TTBR0_EL3_BADDR_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x7fffffffffffULL )

static inline uint64_t _AArch64_Read_ttbr0_el3( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, TTBR0_EL3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_ttbr0_el3( uint64_t value )
{
  __asm__ volatile (
    "msr TTBR0_EL3, %0" : : "r" ( value ) : "memory"
  );
}

/* TTBR1_EL1, Translation Table Base Register 1 (EL1) */

#define AARCH64_TTBR1_EL1_CNP 0x1U

#define AARCH64_TTBR1_EL1_BADDR( _val ) ( ( _val ) << 1 )
#define AARCH64_TTBR1_EL1_BADDR_SHIFT 1
#define AARCH64_TTBR1_EL1_BADDR_MASK 0xfffffffffffeULL
#define AARCH64_TTBR1_EL1_BADDR_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x7fffffffffffULL )

#define AARCH64_TTBR1_EL1_ASID( _val ) ( ( _val ) << 48 )
#define AARCH64_TTBR1_EL1_ASID_SHIFT 48
#define AARCH64_TTBR1_EL1_ASID_MASK 0xffff000000000000ULL
#define AARCH64_TTBR1_EL1_ASID_GET( _reg ) \
  ( ( ( _reg ) >> 48 ) & 0xffffULL )

static inline uint64_t _AArch64_Read_ttbr1_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, TTBR1_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_ttbr1_el1( uint64_t value )
{
  __asm__ volatile (
    "msr TTBR1_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* TTBR1_EL2, Translation Table Base Register 1 (EL2) */

#define AARCH64_TTBR1_EL2_CNP 0x1U

#define AARCH64_TTBR1_EL2_BADDR( _val ) ( ( _val ) << 1 )
#define AARCH64_TTBR1_EL2_BADDR_SHIFT 1
#define AARCH64_TTBR1_EL2_BADDR_MASK 0xfffffffffffeULL
#define AARCH64_TTBR1_EL2_BADDR_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x7fffffffffffULL )

#define AARCH64_TTBR1_EL2_ASID( _val ) ( ( _val ) << 48 )
#define AARCH64_TTBR1_EL2_ASID_SHIFT 48
#define AARCH64_TTBR1_EL2_ASID_MASK 0xffff000000000000ULL
#define AARCH64_TTBR1_EL2_ASID_GET( _reg ) \
  ( ( ( _reg ) >> 48 ) & 0xffffULL )

static inline uint64_t _AArch64_Read_ttbr1_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, TTBR1_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_ttbr1_el2( uint64_t value )
{
  __asm__ volatile (
    "msr TTBR1_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* VBAR_EL1, Vector Base Address Register (EL1) */

static inline uint64_t _AArch64_Read_vbar_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, VBAR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_vbar_el1( uint64_t value )
{
  __asm__ volatile (
    "msr VBAR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* VBAR_EL2, Vector Base Address Register (EL2) */

static inline uint64_t _AArch64_Read_vbar_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, VBAR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_vbar_el2( uint64_t value )
{
  __asm__ volatile (
    "msr VBAR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* VBAR_EL3, Vector Base Address Register (EL3) */

static inline uint64_t _AArch64_Read_vbar_el3( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, VBAR_EL3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_vbar_el3( uint64_t value )
{
  __asm__ volatile (
    "msr VBAR_EL3, %0" : : "r" ( value ) : "memory"
  );
}

/* VMPIDR_EL2, Virtualization Multiprocessor ID Register */

#define AARCH64_VMPIDR_EL2_AFF0( _val ) ( ( _val ) << 0 )
#define AARCH64_VMPIDR_EL2_AFF0_SHIFT 0
#define AARCH64_VMPIDR_EL2_AFF0_MASK 0xffU
#define AARCH64_VMPIDR_EL2_AFF0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

#define AARCH64_VMPIDR_EL2_AFF1( _val ) ( ( _val ) << 8 )
#define AARCH64_VMPIDR_EL2_AFF1_SHIFT 8
#define AARCH64_VMPIDR_EL2_AFF1_MASK 0xff00U
#define AARCH64_VMPIDR_EL2_AFF1_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xffU )

#define AARCH64_VMPIDR_EL2_AFF2( _val ) ( ( _val ) << 16 )
#define AARCH64_VMPIDR_EL2_AFF2_SHIFT 16
#define AARCH64_VMPIDR_EL2_AFF2_MASK 0xff0000U
#define AARCH64_VMPIDR_EL2_AFF2_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xffU )

#define AARCH64_VMPIDR_EL2_MT 0x1000000U

#define AARCH64_VMPIDR_EL2_U 0x40000000U

#define AARCH64_VMPIDR_EL2_AFF3( _val ) ( ( _val ) << 32 )
#define AARCH64_VMPIDR_EL2_AFF3_SHIFT 32
#define AARCH64_VMPIDR_EL2_AFF3_MASK 0xff00000000ULL
#define AARCH64_VMPIDR_EL2_AFF3_GET( _reg ) \
  ( ( ( _reg ) >> 32 ) & 0xffULL )

static inline uint64_t _AArch64_Read_vmpidr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, VMPIDR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_vmpidr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr VMPIDR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* VNCR_EL2, Virtual Nested Control Register */

#define AARCH64_VNCR_EL2_BADDR( _val ) ( ( _val ) << 12 )
#define AARCH64_VNCR_EL2_BADDR_SHIFT 12
#define AARCH64_VNCR_EL2_BADDR_MASK 0x1ffffffffff000ULL
#define AARCH64_VNCR_EL2_BADDR_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0x1ffffffffffULL )

#define AARCH64_VNCR_EL2_RESS( _val ) ( ( _val ) << 53 )
#define AARCH64_VNCR_EL2_RESS_SHIFT 53
#define AARCH64_VNCR_EL2_RESS_MASK 0xffe0000000000000ULL
#define AARCH64_VNCR_EL2_RESS_GET( _reg ) \
  ( ( ( _reg ) >> 53 ) & 0x7ffULL )

static inline uint64_t _AArch64_Read_vncr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, VNCR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_vncr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr VNCR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* VPIDR_EL2, Virtualization Processor ID Register */

#define AARCH64_VPIDR_EL2_REVISION( _val ) ( ( _val ) << 0 )
#define AARCH64_VPIDR_EL2_REVISION_SHIFT 0
#define AARCH64_VPIDR_EL2_REVISION_MASK 0xfU
#define AARCH64_VPIDR_EL2_REVISION_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_VPIDR_EL2_PARTNUM( _val ) ( ( _val ) << 4 )
#define AARCH64_VPIDR_EL2_PARTNUM_SHIFT 4
#define AARCH64_VPIDR_EL2_PARTNUM_MASK 0xfff0U
#define AARCH64_VPIDR_EL2_PARTNUM_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfffU )

#define AARCH64_VPIDR_EL2_ARCHITECTURE( _val ) ( ( _val ) << 16 )
#define AARCH64_VPIDR_EL2_ARCHITECTURE_SHIFT 16
#define AARCH64_VPIDR_EL2_ARCHITECTURE_MASK 0xf0000U
#define AARCH64_VPIDR_EL2_ARCHITECTURE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_VPIDR_EL2_VARIANT( _val ) ( ( _val ) << 20 )
#define AARCH64_VPIDR_EL2_VARIANT_SHIFT 20
#define AARCH64_VPIDR_EL2_VARIANT_MASK 0xf00000U
#define AARCH64_VPIDR_EL2_VARIANT_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

#define AARCH64_VPIDR_EL2_IMPLEMENTER( _val ) ( ( _val ) << 24 )
#define AARCH64_VPIDR_EL2_IMPLEMENTER_SHIFT 24
#define AARCH64_VPIDR_EL2_IMPLEMENTER_MASK 0xff000000U
#define AARCH64_VPIDR_EL2_IMPLEMENTER_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xffU )

static inline uint64_t _AArch64_Read_vpidr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, VPIDR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_vpidr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr VPIDR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* VSTCR_EL2, Virtualization Secure Translation Control Register */

#define AARCH64_VSTCR_EL2_T0SZ( _val ) ( ( _val ) << 0 )
#define AARCH64_VSTCR_EL2_T0SZ_SHIFT 0
#define AARCH64_VSTCR_EL2_T0SZ_MASK 0x3fU
#define AARCH64_VSTCR_EL2_T0SZ_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )

#define AARCH64_VSTCR_EL2_SL0( _val ) ( ( _val ) << 6 )
#define AARCH64_VSTCR_EL2_SL0_SHIFT 6
#define AARCH64_VSTCR_EL2_SL0_MASK 0xc0U
#define AARCH64_VSTCR_EL2_SL0_GET( _reg ) \
  ( ( ( _reg ) >> 6 ) & 0x3U )

#define AARCH64_VSTCR_EL2_TG0( _val ) ( ( _val ) << 14 )
#define AARCH64_VSTCR_EL2_TG0_SHIFT 14
#define AARCH64_VSTCR_EL2_TG0_MASK 0xc000U
#define AARCH64_VSTCR_EL2_TG0_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x3U )

#define AARCH64_VSTCR_EL2_SW 0x20000000U

#define AARCH64_VSTCR_EL2_SA 0x40000000U

static inline uint64_t _AArch64_Read_vstcr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, VSTCR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_vstcr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr VSTCR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* VSTTBR_EL2, Virtualization Secure Translation Table Base Register */

#define AARCH64_VSTTBR_EL2_CNP 0x1U

#define AARCH64_VSTTBR_EL2_BADDR( _val ) ( ( _val ) << 1 )
#define AARCH64_VSTTBR_EL2_BADDR_SHIFT 1
#define AARCH64_VSTTBR_EL2_BADDR_MASK 0xfffffffffffeULL
#define AARCH64_VSTTBR_EL2_BADDR_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x7fffffffffffULL )

static inline uint64_t _AArch64_Read_vsttbr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, VSTTBR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_vsttbr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr VSTTBR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* VTCR_EL2, Virtualization Translation Control Register */

#define AARCH64_VTCR_EL2_T0SZ( _val ) ( ( _val ) << 0 )
#define AARCH64_VTCR_EL2_T0SZ_SHIFT 0
#define AARCH64_VTCR_EL2_T0SZ_MASK 0x3fU
#define AARCH64_VTCR_EL2_T0SZ_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )

#define AARCH64_VTCR_EL2_SL0( _val ) ( ( _val ) << 6 )
#define AARCH64_VTCR_EL2_SL0_SHIFT 6
#define AARCH64_VTCR_EL2_SL0_MASK 0xc0U
#define AARCH64_VTCR_EL2_SL0_GET( _reg ) \
  ( ( ( _reg ) >> 6 ) & 0x3U )

#define AARCH64_VTCR_EL2_IRGN0( _val ) ( ( _val ) << 8 )
#define AARCH64_VTCR_EL2_IRGN0_SHIFT 8
#define AARCH64_VTCR_EL2_IRGN0_MASK 0x300U
#define AARCH64_VTCR_EL2_IRGN0_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0x3U )

#define AARCH64_VTCR_EL2_ORGN0( _val ) ( ( _val ) << 10 )
#define AARCH64_VTCR_EL2_ORGN0_SHIFT 10
#define AARCH64_VTCR_EL2_ORGN0_MASK 0xc00U
#define AARCH64_VTCR_EL2_ORGN0_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3U )

#define AARCH64_VTCR_EL2_SH0( _val ) ( ( _val ) << 12 )
#define AARCH64_VTCR_EL2_SH0_SHIFT 12
#define AARCH64_VTCR_EL2_SH0_MASK 0x3000U
#define AARCH64_VTCR_EL2_SH0_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0x3U )

#define AARCH64_VTCR_EL2_TG0( _val ) ( ( _val ) << 14 )
#define AARCH64_VTCR_EL2_TG0_SHIFT 14
#define AARCH64_VTCR_EL2_TG0_MASK 0xc000U
#define AARCH64_VTCR_EL2_TG0_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x3U )

#define AARCH64_VTCR_EL2_PS( _val ) ( ( _val ) << 16 )
#define AARCH64_VTCR_EL2_PS_SHIFT 16
#define AARCH64_VTCR_EL2_PS_MASK 0x70000U
#define AARCH64_VTCR_EL2_PS_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0x7U )

#define AARCH64_VTCR_EL2_VS 0x80000U

#define AARCH64_VTCR_EL2_HA 0x200000U

#define AARCH64_VTCR_EL2_HD 0x400000U

#define AARCH64_VTCR_EL2_HWU59 0x2000000U

#define AARCH64_VTCR_EL2_HWU60 0x4000000U

#define AARCH64_VTCR_EL2_HWU61 0x8000000U

#define AARCH64_VTCR_EL2_HWU62 0x10000000U

#define AARCH64_VTCR_EL2_NSW 0x20000000U

#define AARCH64_VTCR_EL2_NSA 0x40000000U

static inline uint64_t _AArch64_Read_vtcr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, VTCR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_vtcr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr VTCR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* VTTBR_EL2, Virtualization Translation Table Base Register */

#define AARCH64_VTTBR_EL2_CNP 0x1U

#define AARCH64_VTTBR_EL2_BADDR( _val ) ( ( _val ) << 1 )
#define AARCH64_VTTBR_EL2_BADDR_SHIFT 1
#define AARCH64_VTTBR_EL2_BADDR_MASK 0xfffffffffffeULL
#define AARCH64_VTTBR_EL2_BADDR_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x7fffffffffffULL )

#define AARCH64_VTTBR_EL2_VMID_7_0( _val ) ( ( _val ) << 48 )
#define AARCH64_VTTBR_EL2_VMID_7_0_SHIFT 48
#define AARCH64_VTTBR_EL2_VMID_7_0_MASK 0xff000000000000ULL
#define AARCH64_VTTBR_EL2_VMID_7_0_GET( _reg ) \
  ( ( ( _reg ) >> 48 ) & 0xffULL )

#define AARCH64_VTTBR_EL2_VMID_15_8( _val ) ( ( _val ) << 56 )
#define AARCH64_VTTBR_EL2_VMID_15_8_SHIFT 56
#define AARCH64_VTTBR_EL2_VMID_15_8_MASK 0xff00000000000000ULL
#define AARCH64_VTTBR_EL2_VMID_15_8_GET( _reg ) \
  ( ( ( _reg ) >> 56 ) & 0xffULL )

static inline uint64_t _AArch64_Read_vttbr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, VTTBR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_vttbr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr VTTBR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* DBGAUTHSTATUS_EL1, Debug Authentication Status Register */

#define AARCH64_DBGAUTHSTATUS_EL1_NSID( _val ) ( ( _val ) << 0 )
#define AARCH64_DBGAUTHSTATUS_EL1_NSID_SHIFT 0
#define AARCH64_DBGAUTHSTATUS_EL1_NSID_MASK 0x3U
#define AARCH64_DBGAUTHSTATUS_EL1_NSID_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3U )

#define AARCH64_DBGAUTHSTATUS_EL1_NSNID( _val ) ( ( _val ) << 2 )
#define AARCH64_DBGAUTHSTATUS_EL1_NSNID_SHIFT 2
#define AARCH64_DBGAUTHSTATUS_EL1_NSNID_MASK 0xcU
#define AARCH64_DBGAUTHSTATUS_EL1_NSNID_GET( _reg ) \
  ( ( ( _reg ) >> 2 ) & 0x3U )

#define AARCH64_DBGAUTHSTATUS_EL1_SID( _val ) ( ( _val ) << 4 )
#define AARCH64_DBGAUTHSTATUS_EL1_SID_SHIFT 4
#define AARCH64_DBGAUTHSTATUS_EL1_SID_MASK 0x30U
#define AARCH64_DBGAUTHSTATUS_EL1_SID_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0x3U )

#define AARCH64_DBGAUTHSTATUS_EL1_SNID( _val ) ( ( _val ) << 6 )
#define AARCH64_DBGAUTHSTATUS_EL1_SNID_SHIFT 6
#define AARCH64_DBGAUTHSTATUS_EL1_SNID_MASK 0xc0U
#define AARCH64_DBGAUTHSTATUS_EL1_SNID_GET( _reg ) \
  ( ( ( _reg ) >> 6 ) & 0x3U )

static inline uint64_t _AArch64_Read_dbgauthstatus_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGAUTHSTATUS_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* DBGBCR_N_EL1, Debug Breakpoint Control Registers, n = 0 - 15 */

#define AARCH64_DBGBCR_N_EL1_E 0x1U

#define AARCH64_DBGBCR_N_EL1_PMC( _val ) ( ( _val ) << 1 )
#define AARCH64_DBGBCR_N_EL1_PMC_SHIFT 1
#define AARCH64_DBGBCR_N_EL1_PMC_MASK 0x6U
#define AARCH64_DBGBCR_N_EL1_PMC_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x3U )

#define AARCH64_DBGBCR_N_EL1_BAS( _val ) ( ( _val ) << 5 )
#define AARCH64_DBGBCR_N_EL1_BAS_SHIFT 5
#define AARCH64_DBGBCR_N_EL1_BAS_MASK 0x1e0U
#define AARCH64_DBGBCR_N_EL1_BAS_GET( _reg ) \
  ( ( ( _reg ) >> 5 ) & 0xfU )

#define AARCH64_DBGBCR_N_EL1_HMC 0x2000U

#define AARCH64_DBGBCR_N_EL1_SSC( _val ) ( ( _val ) << 14 )
#define AARCH64_DBGBCR_N_EL1_SSC_SHIFT 14
#define AARCH64_DBGBCR_N_EL1_SSC_MASK 0xc000U
#define AARCH64_DBGBCR_N_EL1_SSC_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x3U )

#define AARCH64_DBGBCR_N_EL1_LBN( _val ) ( ( _val ) << 16 )
#define AARCH64_DBGBCR_N_EL1_LBN_SHIFT 16
#define AARCH64_DBGBCR_N_EL1_LBN_MASK 0xf0000U
#define AARCH64_DBGBCR_N_EL1_LBN_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_DBGBCR_N_EL1_BT( _val ) ( ( _val ) << 20 )
#define AARCH64_DBGBCR_N_EL1_BT_SHIFT 20
#define AARCH64_DBGBCR_N_EL1_BT_MASK 0xf00000U
#define AARCH64_DBGBCR_N_EL1_BT_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )

static inline uint64_t _AArch64_Read_dbgbcr0_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBCR0_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbcr0_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBCR0_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbcr1_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBCR1_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbcr1_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBCR1_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbcr2_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBCR2_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbcr2_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBCR2_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbcr3_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBCR3_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbcr3_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBCR3_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbcr4_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBCR4_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbcr4_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBCR4_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbcr5_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBCR5_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbcr5_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBCR5_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbcr6_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBCR6_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbcr6_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBCR6_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbcr7_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBCR7_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbcr7_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBCR7_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbcr8_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBCR8_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbcr8_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBCR8_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbcr9_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBCR9_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbcr9_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBCR9_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbcr10_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBCR10_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbcr10_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBCR10_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbcr11_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBCR11_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbcr11_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBCR11_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbcr12_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBCR12_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbcr12_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBCR12_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbcr13_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBCR13_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbcr13_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBCR13_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbcr14_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBCR14_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbcr14_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBCR14_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbcr15_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBCR15_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbcr15_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBCR15_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* DBGBVR_N_EL1, Debug Breakpoint Value Registers, n = 0 - 15 */

#define AARCH64_DBGBVR_N_EL1_CONTEXTID( _val ) ( ( _val ) << 0 )
#define AARCH64_DBGBVR_N_EL1_CONTEXTID_SHIFT 0
#define AARCH64_DBGBVR_N_EL1_CONTEXTID_MASK 0xffffffffU
#define AARCH64_DBGBVR_N_EL1_CONTEXTID_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )

#define AARCH64_DBGBVR_N_EL1_VA_48_2( _val ) ( ( _val ) << 2 )
#define AARCH64_DBGBVR_N_EL1_VA_48_2_SHIFT 2
#define AARCH64_DBGBVR_N_EL1_VA_48_2_MASK 0x1fffffffffffcULL
#define AARCH64_DBGBVR_N_EL1_VA_48_2_GET( _reg ) \
  ( ( ( _reg ) >> 2 ) & 0x7fffffffffffULL )

#define AARCH64_DBGBVR_N_EL1_VMID_7_0( _val ) ( ( _val ) << 32 )
#define AARCH64_DBGBVR_N_EL1_VMID_7_0_SHIFT 32
#define AARCH64_DBGBVR_N_EL1_VMID_7_0_MASK 0xff00000000ULL
#define AARCH64_DBGBVR_N_EL1_VMID_7_0_GET( _reg ) \
  ( ( ( _reg ) >> 32 ) & 0xffULL )

#define AARCH64_DBGBVR_N_EL1_CONTEXTID2( _val ) ( ( _val ) << 32 )
#define AARCH64_DBGBVR_N_EL1_CONTEXTID2_SHIFT 32
#define AARCH64_DBGBVR_N_EL1_CONTEXTID2_MASK 0xffffffff00000000ULL
#define AARCH64_DBGBVR_N_EL1_CONTEXTID2_GET( _reg ) \
  ( ( ( _reg ) >> 32 ) & 0xffffffffULL )

#define AARCH64_DBGBVR_N_EL1_VMID_15_8( _val ) ( ( _val ) << 40 )
#define AARCH64_DBGBVR_N_EL1_VMID_15_8_SHIFT 40
#define AARCH64_DBGBVR_N_EL1_VMID_15_8_MASK 0xff0000000000ULL
#define AARCH64_DBGBVR_N_EL1_VMID_15_8_GET( _reg ) \
  ( ( ( _reg ) >> 40 ) & 0xffULL )

#define AARCH64_DBGBVR_N_EL1_VA_52_49( _val ) ( ( _val ) << 49 )
#define AARCH64_DBGBVR_N_EL1_VA_52_49_SHIFT 49
#define AARCH64_DBGBVR_N_EL1_VA_52_49_MASK 0x1e000000000000ULL
#define AARCH64_DBGBVR_N_EL1_VA_52_49_GET( _reg ) \
  ( ( ( _reg ) >> 49 ) & 0xfULL )

#define AARCH64_DBGBVR_N_EL1_RESS_14_4( _val ) ( ( _val ) << 53 )
#define AARCH64_DBGBVR_N_EL1_RESS_14_4_SHIFT 53
#define AARCH64_DBGBVR_N_EL1_RESS_14_4_MASK 0xffe0000000000000ULL
#define AARCH64_DBGBVR_N_EL1_RESS_14_4_GET( _reg ) \
  ( ( ( _reg ) >> 53 ) & 0x7ffULL )

static inline uint64_t _AArch64_Read_dbgbvr0_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBVR0_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbvr0_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBVR0_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbvr1_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBVR1_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbvr1_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBVR1_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbvr2_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBVR2_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbvr2_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBVR2_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbvr3_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBVR3_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbvr3_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBVR3_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbvr4_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBVR4_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbvr4_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBVR4_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbvr5_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBVR5_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbvr5_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBVR5_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbvr6_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBVR6_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbvr6_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBVR6_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbvr7_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBVR7_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbvr7_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBVR7_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbvr8_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBVR8_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbvr8_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBVR8_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbvr9_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBVR9_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbvr9_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBVR9_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbvr10_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBVR10_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbvr10_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBVR10_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbvr11_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBVR11_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbvr11_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBVR11_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbvr12_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBVR12_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbvr12_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBVR12_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbvr13_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBVR13_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbvr13_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBVR13_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbvr14_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBVR14_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbvr14_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBVR14_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgbvr15_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGBVR15_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgbvr15_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGBVR15_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* DBGCLAIMCLR_EL1, Debug CLAIM Tag Clear Register */

#define AARCH64_DBGCLAIMCLR_EL1_CLAIM( _val ) ( ( _val ) << 0 )
#define AARCH64_DBGCLAIMCLR_EL1_CLAIM_SHIFT 0
#define AARCH64_DBGCLAIMCLR_EL1_CLAIM_MASK 0xffU
#define AARCH64_DBGCLAIMCLR_EL1_CLAIM_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

static inline uint64_t _AArch64_Read_dbgclaimclr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGCLAIMCLR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgclaimclr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGCLAIMCLR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* DBGCLAIMSET_EL1, Debug CLAIM Tag Set Register */

#define AARCH64_DBGCLAIMSET_EL1_CLAIM( _val ) ( ( _val ) << 0 )
#define AARCH64_DBGCLAIMSET_EL1_CLAIM_SHIFT 0
#define AARCH64_DBGCLAIMSET_EL1_CLAIM_MASK 0xffU
#define AARCH64_DBGCLAIMSET_EL1_CLAIM_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

static inline uint64_t _AArch64_Read_dbgclaimset_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGCLAIMSET_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgclaimset_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGCLAIMSET_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* DBGDTR_EL0, Debug Data Transfer Register, half-duplex */

#define AARCH64_DBGDTR_EL0_LOWWORD( _val ) ( ( _val ) << 0 )
#define AARCH64_DBGDTR_EL0_LOWWORD_SHIFT 0
#define AARCH64_DBGDTR_EL0_LOWWORD_MASK 0xffffffffU
#define AARCH64_DBGDTR_EL0_LOWWORD_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )

#define AARCH64_DBGDTR_EL0_HIGHWORD( _val ) ( ( _val ) << 32 )
#define AARCH64_DBGDTR_EL0_HIGHWORD_SHIFT 32
#define AARCH64_DBGDTR_EL0_HIGHWORD_MASK 0xffffffff00000000ULL
#define AARCH64_DBGDTR_EL0_HIGHWORD_GET( _reg ) \
  ( ( ( _reg ) >> 32 ) & 0xffffffffULL )

static inline uint64_t _AArch64_Read_dbgdtr_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGDTR_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgdtr_el0( uint64_t value )
{
  __asm__ volatile (
    "msr DBGDTR_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* DBGDTRRX_EL0, Debug Data Transfer Register, Receive */

static inline uint64_t _AArch64_Read_dbgdtrrx_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGDTRRX_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* DBGDTRTX_EL0, Debug Data Transfer Register, Transmit */

static inline void _AArch64_Write_dbgdtrtx_el0( uint64_t value )
{
  __asm__ volatile (
    "msr DBGDTRTX_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* DBGPRCR_EL1, Debug Power Control Register */

#define AARCH64_DBGPRCR_EL1_CORENPDRQ 0x1U

static inline uint64_t _AArch64_Read_dbgprcr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGPRCR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgprcr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGPRCR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* DBGVCR32_EL2, Debug Vector Catch Register */

#define AARCH64_DBGVCR32_EL2_SU 0x2U

#define AARCH64_DBGVCR32_EL2_U 0x2U

#define AARCH64_DBGVCR32_EL2_S 0x4U

#define AARCH64_DBGVCR32_EL2_SS 0x4U

#define AARCH64_DBGVCR32_EL2_P 0x8U

#define AARCH64_DBGVCR32_EL2_SP 0x8U

#define AARCH64_DBGVCR32_EL2_D 0x10U

#define AARCH64_DBGVCR32_EL2_SD 0x10U

#define AARCH64_DBGVCR32_EL2_I 0x40U

#define AARCH64_DBGVCR32_EL2_SI 0x40U

#define AARCH64_DBGVCR32_EL2_F 0x80U

#define AARCH64_DBGVCR32_EL2_SF 0x80U

#define AARCH64_DBGVCR32_EL2_NSU 0x2000000U

#define AARCH64_DBGVCR32_EL2_NSS 0x4000000U

#define AARCH64_DBGVCR32_EL2_NSP 0x8000000U

#define AARCH64_DBGVCR32_EL2_NSD 0x10000000U

#define AARCH64_DBGVCR32_EL2_NSI 0x40000000U

#define AARCH64_DBGVCR32_EL2_NSF 0x80000000U

static inline uint64_t _AArch64_Read_dbgvcr32_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGVCR32_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgvcr32_el2( uint64_t value )
{
  __asm__ volatile (
    "msr DBGVCR32_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* DBGWCR_N_EL1, Debug Watchpoint Control Registers, n = 0 - 15 */

#define AARCH64_DBGWCR_N_EL1_E 0x1U

#define AARCH64_DBGWCR_N_EL1_PAC( _val ) ( ( _val ) << 1 )
#define AARCH64_DBGWCR_N_EL1_PAC_SHIFT 1
#define AARCH64_DBGWCR_N_EL1_PAC_MASK 0x6U
#define AARCH64_DBGWCR_N_EL1_PAC_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x3U )

#define AARCH64_DBGWCR_N_EL1_LSC( _val ) ( ( _val ) << 3 )
#define AARCH64_DBGWCR_N_EL1_LSC_SHIFT 3
#define AARCH64_DBGWCR_N_EL1_LSC_MASK 0x18U
#define AARCH64_DBGWCR_N_EL1_LSC_GET( _reg ) \
  ( ( ( _reg ) >> 3 ) & 0x3U )

#define AARCH64_DBGWCR_N_EL1_BAS( _val ) ( ( _val ) << 5 )
#define AARCH64_DBGWCR_N_EL1_BAS_SHIFT 5
#define AARCH64_DBGWCR_N_EL1_BAS_MASK 0x1fe0U
#define AARCH64_DBGWCR_N_EL1_BAS_GET( _reg ) \
  ( ( ( _reg ) >> 5 ) & 0xffU )

#define AARCH64_DBGWCR_N_EL1_HMC 0x2000U

#define AARCH64_DBGWCR_N_EL1_SSC( _val ) ( ( _val ) << 14 )
#define AARCH64_DBGWCR_N_EL1_SSC_SHIFT 14
#define AARCH64_DBGWCR_N_EL1_SSC_MASK 0xc000U
#define AARCH64_DBGWCR_N_EL1_SSC_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x3U )

#define AARCH64_DBGWCR_N_EL1_LBN( _val ) ( ( _val ) << 16 )
#define AARCH64_DBGWCR_N_EL1_LBN_SHIFT 16
#define AARCH64_DBGWCR_N_EL1_LBN_MASK 0xf0000U
#define AARCH64_DBGWCR_N_EL1_LBN_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_DBGWCR_N_EL1_WT 0x100000U

#define AARCH64_DBGWCR_N_EL1_MASK( _val ) ( ( _val ) << 24 )
#define AARCH64_DBGWCR_N_EL1_MASK_SHIFT 24
#define AARCH64_DBGWCR_N_EL1_MASK_MASK 0x1f000000U
#define AARCH64_DBGWCR_N_EL1_MASK_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0x1fU )

static inline uint64_t _AArch64_Read_dbgwcr0_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWCR0_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwcr0_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWCR0_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwcr1_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWCR1_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwcr1_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWCR1_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwcr2_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWCR2_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwcr2_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWCR2_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwcr3_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWCR3_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwcr3_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWCR3_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwcr4_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWCR4_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwcr4_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWCR4_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwcr5_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWCR5_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwcr5_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWCR5_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwcr6_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWCR6_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwcr6_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWCR6_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwcr7_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWCR7_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwcr7_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWCR7_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwcr8_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWCR8_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwcr8_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWCR8_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwcr9_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWCR9_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwcr9_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWCR9_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwcr10_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWCR10_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwcr10_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWCR10_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwcr11_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWCR11_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwcr11_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWCR11_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwcr12_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWCR12_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwcr12_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWCR12_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwcr13_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWCR13_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwcr13_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWCR13_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwcr14_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWCR14_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwcr14_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWCR14_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwcr15_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWCR15_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwcr15_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWCR15_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* DBGWVR_N_EL1, Debug Watchpoint Value Registers, n = 0 - 15 */

#define AARCH64_DBGWVR_N_EL1_VA_48_2( _val ) ( ( _val ) << 2 )
#define AARCH64_DBGWVR_N_EL1_VA_48_2_SHIFT 2
#define AARCH64_DBGWVR_N_EL1_VA_48_2_MASK 0x1fffffffffffcULL
#define AARCH64_DBGWVR_N_EL1_VA_48_2_GET( _reg ) \
  ( ( ( _reg ) >> 2 ) & 0x7fffffffffffULL )

#define AARCH64_DBGWVR_N_EL1_VA_52_49( _val ) ( ( _val ) << 49 )
#define AARCH64_DBGWVR_N_EL1_VA_52_49_SHIFT 49
#define AARCH64_DBGWVR_N_EL1_VA_52_49_MASK 0x1e000000000000ULL
#define AARCH64_DBGWVR_N_EL1_VA_52_49_GET( _reg ) \
  ( ( ( _reg ) >> 49 ) & 0xfULL )

#define AARCH64_DBGWVR_N_EL1_RESS_14_4( _val ) ( ( _val ) << 53 )
#define AARCH64_DBGWVR_N_EL1_RESS_14_4_SHIFT 53
#define AARCH64_DBGWVR_N_EL1_RESS_14_4_MASK 0xffe0000000000000ULL
#define AARCH64_DBGWVR_N_EL1_RESS_14_4_GET( _reg ) \
  ( ( ( _reg ) >> 53 ) & 0x7ffULL )

static inline uint64_t _AArch64_Read_dbgwvr0_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWVR0_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwvr0_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWVR0_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwvr1_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWVR1_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwvr1_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWVR1_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwvr2_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWVR2_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwvr2_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWVR2_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwvr3_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWVR3_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwvr3_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWVR3_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwvr4_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWVR4_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwvr4_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWVR4_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwvr5_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWVR5_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwvr5_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWVR5_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwvr6_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWVR6_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwvr6_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWVR6_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwvr7_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWVR7_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwvr7_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWVR7_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwvr8_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWVR8_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwvr8_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWVR8_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwvr9_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWVR9_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwvr9_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWVR9_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwvr10_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWVR10_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwvr10_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWVR10_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwvr11_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWVR11_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwvr11_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWVR11_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwvr12_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWVR12_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwvr12_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWVR12_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwvr13_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWVR13_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwvr13_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWVR13_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwvr14_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWVR14_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwvr14_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWVR14_EL1, %0" : : "r" ( value ) : "memory"
  );
}

static inline uint64_t _AArch64_Read_dbgwvr15_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DBGWVR15_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dbgwvr15_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DBGWVR15_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* DLR_EL0, Debug Link Register */

static inline uint64_t _AArch64_Read_dlr_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DLR_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dlr_el0( uint64_t value )
{
  __asm__ volatile (
    "msr DLR_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* DSPSR_EL0, Debug Saved Program Status Register */

#define AARCH64_DSPSR_EL0_M_3_0( _val ) ( ( _val ) << 0 )
#define AARCH64_DSPSR_EL0_M_3_0_SHIFT 0
#define AARCH64_DSPSR_EL0_M_3_0_MASK 0xfU
#define AARCH64_DSPSR_EL0_M_3_0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_DSPSR_EL0_M_4 0x10U

#define AARCH64_DSPSR_EL0_T 0x20U

#define AARCH64_DSPSR_EL0_F 0x40U

#define AARCH64_DSPSR_EL0_I 0x80U

#define AARCH64_DSPSR_EL0_A 0x100U

#define AARCH64_DSPSR_EL0_D 0x200U

#define AARCH64_DSPSR_EL0_E 0x200U

#define AARCH64_DSPSR_EL0_BTYPE( _val ) ( ( _val ) << 10 )
#define AARCH64_DSPSR_EL0_BTYPE_SHIFT 10
#define AARCH64_DSPSR_EL0_BTYPE_MASK 0xc00U
#define AARCH64_DSPSR_EL0_BTYPE_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3U )

#define AARCH64_DSPSR_EL0_IT_7_2( _val ) ( ( _val ) << 10 )
#define AARCH64_DSPSR_EL0_IT_7_2_SHIFT 10
#define AARCH64_DSPSR_EL0_IT_7_2_MASK 0xfc00U
#define AARCH64_DSPSR_EL0_IT_7_2_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3fU )

#define AARCH64_DSPSR_EL0_SSBS_0 0x1000U

#define AARCH64_DSPSR_EL0_GE( _val ) ( ( _val ) << 16 )
#define AARCH64_DSPSR_EL0_GE_SHIFT 16
#define AARCH64_DSPSR_EL0_GE_MASK 0xf0000U
#define AARCH64_DSPSR_EL0_GE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

#define AARCH64_DSPSR_EL0_IL 0x100000U

#define AARCH64_DSPSR_EL0_SS 0x200000U

#define AARCH64_DSPSR_EL0_PAN 0x400000U

#define AARCH64_DSPSR_EL0_SSBS_1 0x800000U

#define AARCH64_DSPSR_EL0_UAO 0x800000U

#define AARCH64_DSPSR_EL0_DIT 0x1000000U

#define AARCH64_DSPSR_EL0_TCO 0x2000000U

#define AARCH64_DSPSR_EL0_IT_1_0( _val ) ( ( _val ) << 25 )
#define AARCH64_DSPSR_EL0_IT_1_0_SHIFT 25
#define AARCH64_DSPSR_EL0_IT_1_0_MASK 0x6000000U
#define AARCH64_DSPSR_EL0_IT_1_0_GET( _reg ) \
  ( ( ( _reg ) >> 25 ) & 0x3U )

#define AARCH64_DSPSR_EL0_Q 0x8000000U

#define AARCH64_DSPSR_EL0_V 0x10000000U

#define AARCH64_DSPSR_EL0_C 0x20000000U

#define AARCH64_DSPSR_EL0_Z 0x40000000U

#define AARCH64_DSPSR_EL0_N 0x80000000U

static inline uint64_t _AArch64_Read_dspsr_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DSPSR_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_dspsr_el0( uint64_t value )
{
  __asm__ volatile (
    "msr DSPSR_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* MDCCINT_EL1, Monitor DCC Interrupt Enable Register */

#define AARCH64_MDCCINT_EL1_TX 0x20000000U

#define AARCH64_MDCCINT_EL1_RX 0x40000000U

static inline uint64_t _AArch64_Read_mdccint_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, MDCCINT_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_mdccint_el1( uint64_t value )
{
  __asm__ volatile (
    "msr MDCCINT_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* MDCCSR_EL0, Monitor DCC Status Register */

#define AARCH64_MDCCSR_EL0_TXFULL 0x20000000U

#define AARCH64_MDCCSR_EL0_RXFULL 0x40000000U

static inline uint64_t _AArch64_Read_mdccsr_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, MDCCSR_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* MDCR_EL2, Monitor Debug Configuration Register (EL2) */

#define AARCH64_MDCR_EL2_HPMN( _val ) ( ( _val ) << 0 )
#define AARCH64_MDCR_EL2_HPMN_SHIFT 0
#define AARCH64_MDCR_EL2_HPMN_MASK 0x1fU
#define AARCH64_MDCR_EL2_HPMN_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1fU )

#define AARCH64_MDCR_EL2_TPMCR 0x20U

#define AARCH64_MDCR_EL2_TPM 0x40U

#define AARCH64_MDCR_EL2_HPME 0x80U

#define AARCH64_MDCR_EL2_TDE 0x100U

#define AARCH64_MDCR_EL2_TDA 0x200U

#define AARCH64_MDCR_EL2_TDOSA 0x400U

#define AARCH64_MDCR_EL2_TDRA 0x800U

#define AARCH64_MDCR_EL2_E2PB( _val ) ( ( _val ) << 12 )
#define AARCH64_MDCR_EL2_E2PB_SHIFT 12
#define AARCH64_MDCR_EL2_E2PB_MASK 0x3000U
#define AARCH64_MDCR_EL2_E2PB_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0x3U )

#define AARCH64_MDCR_EL2_TPMS 0x4000U

#define AARCH64_MDCR_EL2_HPMD 0x20000U

#define AARCH64_MDCR_EL2_TTRF 0x80000U

#define AARCH64_MDCR_EL2_HCCD 0x800000U

#define AARCH64_MDCR_EL2_HLP 0x4000000U

#define AARCH64_MDCR_EL2_TDCC 0x8000000U

#define AARCH64_MDCR_EL2_MTPME 0x10000000U

static inline uint64_t _AArch64_Read_mdcr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, MDCR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_mdcr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr MDCR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* MDCR_EL3, Monitor Debug Configuration Register (EL3) */

#define AARCH64_MDCR_EL3_TPM 0x40U

#define AARCH64_MDCR_EL3_TDA 0x200U

#define AARCH64_MDCR_EL3_TDOSA 0x400U

#define AARCH64_MDCR_EL3_NSPB( _val ) ( ( _val ) << 12 )
#define AARCH64_MDCR_EL3_NSPB_SHIFT 12
#define AARCH64_MDCR_EL3_NSPB_MASK 0x3000U
#define AARCH64_MDCR_EL3_NSPB_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0x3U )

#define AARCH64_MDCR_EL3_SPD32( _val ) ( ( _val ) << 14 )
#define AARCH64_MDCR_EL3_SPD32_SHIFT 14
#define AARCH64_MDCR_EL3_SPD32_MASK 0xc000U
#define AARCH64_MDCR_EL3_SPD32_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x3U )

#define AARCH64_MDCR_EL3_SDD 0x10000U

#define AARCH64_MDCR_EL3_SPME 0x20000U

#define AARCH64_MDCR_EL3_STE 0x40000U

#define AARCH64_MDCR_EL3_TTRF 0x80000U

#define AARCH64_MDCR_EL3_EDAD 0x100000U

#define AARCH64_MDCR_EL3_EPMAD 0x200000U

#define AARCH64_MDCR_EL3_SCCD 0x800000U

#define AARCH64_MDCR_EL3_TDCC 0x8000000U

#define AARCH64_MDCR_EL3_MTPME 0x10000000U

static inline uint64_t _AArch64_Read_mdcr_el3( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, MDCR_EL3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_mdcr_el3( uint64_t value )
{
  __asm__ volatile (
    "msr MDCR_EL3, %0" : : "r" ( value ) : "memory"
  );
}

/* MDRAR_EL1, Monitor Debug ROM Address Register */

#define AARCH64_MDRAR_EL1_VALID( _val ) ( ( _val ) << 0 )
#define AARCH64_MDRAR_EL1_VALID_SHIFT 0
#define AARCH64_MDRAR_EL1_VALID_MASK 0x3U
#define AARCH64_MDRAR_EL1_VALID_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3U )

#define AARCH64_MDRAR_EL1_ROMADDR_47_12( _val ) ( ( _val ) << 12 )
#define AARCH64_MDRAR_EL1_ROMADDR_47_12_SHIFT 12
#define AARCH64_MDRAR_EL1_ROMADDR_47_12_MASK 0xfffffffff000ULL
#define AARCH64_MDRAR_EL1_ROMADDR_47_12_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfffffffffULL )

#define AARCH64_MDRAR_EL1_ROMADDR_51_48( _val ) ( ( _val ) << 48 )
#define AARCH64_MDRAR_EL1_ROMADDR_51_48_SHIFT 48
#define AARCH64_MDRAR_EL1_ROMADDR_51_48_MASK 0xf000000000000ULL
#define AARCH64_MDRAR_EL1_ROMADDR_51_48_GET( _reg ) \
  ( ( ( _reg ) >> 48 ) & 0xfULL )

static inline uint64_t _AArch64_Read_mdrar_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, MDRAR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* MDSCR_EL1, Monitor Debug System Control Register */

#define AARCH64_MDSCR_EL1_SS 0x1U

#define AARCH64_MDSCR_EL1_ERR 0x40U

#define AARCH64_MDSCR_EL1_TDCC 0x1000U

#define AARCH64_MDSCR_EL1_KDE 0x2000U

#define AARCH64_MDSCR_EL1_HDE 0x4000U

#define AARCH64_MDSCR_EL1_MDE 0x8000U

#define AARCH64_MDSCR_EL1_SC2 0x80000U

#define AARCH64_MDSCR_EL1_TDA 0x200000U

#define AARCH64_MDSCR_EL1_INTDIS( _val ) ( ( _val ) << 22 )
#define AARCH64_MDSCR_EL1_INTDIS_SHIFT 22
#define AARCH64_MDSCR_EL1_INTDIS_MASK 0xc00000U
#define AARCH64_MDSCR_EL1_INTDIS_GET( _reg ) \
  ( ( ( _reg ) >> 22 ) & 0x3U )

#define AARCH64_MDSCR_EL1_TXU 0x4000000U

#define AARCH64_MDSCR_EL1_RXO 0x8000000U

#define AARCH64_MDSCR_EL1_TXFULL 0x20000000U

#define AARCH64_MDSCR_EL1_RXFULL 0x40000000U

#define AARCH64_MDSCR_EL1_TFO 0x80000000U

static inline uint64_t _AArch64_Read_mdscr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, MDSCR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_mdscr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr MDSCR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* OSDLR_EL1, OS Double Lock Register */

#define AARCH64_OSDLR_EL1_DLK 0x1U

static inline uint64_t _AArch64_Read_osdlr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, OSDLR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_osdlr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr OSDLR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* OSDTRRX_EL1, OS Lock Data Transfer Register, Receive */

static inline uint64_t _AArch64_Read_osdtrrx_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, OSDTRRX_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_osdtrrx_el1( uint64_t value )
{
  __asm__ volatile (
    "msr OSDTRRX_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* OSDTRTX_EL1, OS Lock Data Transfer Register, Transmit */

static inline uint64_t _AArch64_Read_osdtrtx_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, OSDTRTX_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_osdtrtx_el1( uint64_t value )
{
  __asm__ volatile (
    "msr OSDTRTX_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* OSECCR_EL1, OS Lock Exception Catch Control Register */

#define AARCH64_OSECCR_EL1_EDECCR( _val ) ( ( _val ) << 0 )
#define AARCH64_OSECCR_EL1_EDECCR_SHIFT 0
#define AARCH64_OSECCR_EL1_EDECCR_MASK 0xffffffffU
#define AARCH64_OSECCR_EL1_EDECCR_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )

static inline uint64_t _AArch64_Read_oseccr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, OSECCR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_oseccr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr OSECCR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* OSLAR_EL1, OS Lock Access Register */

#define AARCH64_OSLAR_EL1_OSLK 0x1U

static inline void _AArch64_Write_oslar_el1( uint64_t value )
{
  __asm__ volatile (
    "msr OSLAR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* OSLSR_EL1, OS Lock Status Register */

#define AARCH64_OSLSR_EL1_OSLM_0 0x1U

#define AARCH64_OSLSR_EL1_OSLK 0x2U

#define AARCH64_OSLSR_EL1_NTT 0x4U

#define AARCH64_OSLSR_EL1_OSLM_1 0x8U

static inline uint64_t _AArch64_Read_oslsr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, OSLSR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* SDER32_EL2, AArch64 Secure Debug Enable Register */

#define AARCH64_SDER32_EL2_SUIDEN 0x1U

#define AARCH64_SDER32_EL2_SUNIDEN 0x2U

static inline uint64_t _AArch64_Read_sder32_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, SDER32_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_sder32_el2( uint64_t value )
{
  __asm__ volatile (
    "msr SDER32_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* SDER32_EL3, AArch64 Secure Debug Enable Register */

#define AARCH64_SDER32_EL3_SUIDEN 0x1U

#define AARCH64_SDER32_EL3_SUNIDEN 0x2U

static inline uint64_t _AArch64_Read_sder32_el3( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, SDER32_EL3" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_sder32_el3( uint64_t value )
{
  __asm__ volatile (
    "msr SDER32_EL3, %0" : : "r" ( value ) : "memory"
  );
}

/* TRFCR_EL1, Trace Filter Control Register (EL1) */

#define AARCH64_TRFCR_EL1_E0TRE 0x1U

#define AARCH64_TRFCR_EL1_E1TRE 0x2U

#define AARCH64_TRFCR_EL1_TS( _val ) ( ( _val ) << 5 )
#define AARCH64_TRFCR_EL1_TS_SHIFT 5
#define AARCH64_TRFCR_EL1_TS_MASK 0x60U
#define AARCH64_TRFCR_EL1_TS_GET( _reg ) \
  ( ( ( _reg ) >> 5 ) & 0x3U )

static inline uint64_t _AArch64_Read_trfcr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, TRFCR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_trfcr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr TRFCR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* TRFCR_EL2, Trace Filter Control Register (EL2) */

#define AARCH64_TRFCR_EL2_E0HTRE 0x1U

#define AARCH64_TRFCR_EL2_E2TRE 0x2U

#define AARCH64_TRFCR_EL2_CX 0x8U

#define AARCH64_TRFCR_EL2_TS( _val ) ( ( _val ) << 5 )
#define AARCH64_TRFCR_EL2_TS_SHIFT 5
#define AARCH64_TRFCR_EL2_TS_MASK 0x60U
#define AARCH64_TRFCR_EL2_TS_GET( _reg ) \
  ( ( ( _reg ) >> 5 ) & 0x3U )

static inline uint64_t _AArch64_Read_trfcr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, TRFCR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_trfcr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr TRFCR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* PMCCFILTR_EL0, Performance Monitors Cycle Count Filter Register */

#define AARCH64_PMCCFILTR_EL0_SH 0x1000000U

#define AARCH64_PMCCFILTR_EL0_M 0x4000000U

#define AARCH64_PMCCFILTR_EL0_NSH 0x8000000U

#define AARCH64_PMCCFILTR_EL0_NSU 0x10000000U

#define AARCH64_PMCCFILTR_EL0_NSK 0x20000000U

#define AARCH64_PMCCFILTR_EL0_U 0x40000000U

#define AARCH64_PMCCFILTR_EL0_P 0x80000000U

static inline uint64_t _AArch64_Read_pmccfiltr_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMCCFILTR_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmccfiltr_el0( uint64_t value )
{
  __asm__ volatile (
    "msr PMCCFILTR_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* PMCCNTR_EL0, Performance Monitors Cycle Count Register */

static inline uint64_t _AArch64_Read_pmccntr_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMCCNTR_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmccntr_el0( uint64_t value )
{
  __asm__ volatile (
    "msr PMCCNTR_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* PMCEID0_EL0, Performance Monitors Common Event Identification Register 0 */

static inline uint64_t _AArch64_Read_pmceid0_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMCEID0_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* PMCEID1_EL0, Performance Monitors Common Event Identification Register 1 */

static inline uint64_t _AArch64_Read_pmceid1_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMCEID1_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* PMCNTENCLR_EL0, Performance Monitors Count Enable Clear Register */

#define AARCH64_PMCNTENCLR_EL0_C 0x80000000U

static inline uint64_t _AArch64_Read_pmcntenclr_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMCNTENCLR_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmcntenclr_el0( uint64_t value )
{
  __asm__ volatile (
    "msr PMCNTENCLR_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* PMCNTENSET_EL0, Performance Monitors Count Enable Set Register */

#define AARCH64_PMCNTENSET_EL0_C 0x80000000U

static inline uint64_t _AArch64_Read_pmcntenset_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMCNTENSET_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmcntenset_el0( uint64_t value )
{
  __asm__ volatile (
    "msr PMCNTENSET_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* PMCR_EL0, Performance Monitors Control Register */

#define AARCH64_PMCR_EL0_E 0x1U

#define AARCH64_PMCR_EL0_P 0x2U

#define AARCH64_PMCR_EL0_C 0x4U

#define AARCH64_PMCR_EL0_D 0x8U

#define AARCH64_PMCR_EL0_X 0x10U

#define AARCH64_PMCR_EL0_DP 0x20U

#define AARCH64_PMCR_EL0_LC 0x40U

#define AARCH64_PMCR_EL0_LP 0x80U

#define AARCH64_PMCR_EL0_N( _val ) ( ( _val ) << 11 )
#define AARCH64_PMCR_EL0_N_SHIFT 11
#define AARCH64_PMCR_EL0_N_MASK 0xf800U
#define AARCH64_PMCR_EL0_N_GET( _reg ) \
  ( ( ( _reg ) >> 11 ) & 0x1fU )

#define AARCH64_PMCR_EL0_IDCODE( _val ) ( ( _val ) << 16 )
#define AARCH64_PMCR_EL0_IDCODE_SHIFT 16
#define AARCH64_PMCR_EL0_IDCODE_MASK 0xff0000U
#define AARCH64_PMCR_EL0_IDCODE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xffU )

#define AARCH64_PMCR_EL0_IMP( _val ) ( ( _val ) << 24 )
#define AARCH64_PMCR_EL0_IMP_SHIFT 24
#define AARCH64_PMCR_EL0_IMP_MASK 0xff000000U
#define AARCH64_PMCR_EL0_IMP_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xffU )

static inline uint64_t _AArch64_Read_pmcr_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMCR_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmcr_el0( uint64_t value )
{
  __asm__ volatile (
    "msr PMCR_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* PMEVCNTR_N_EL0, Performance Monitors Event Count Registers, n = 0 - 30 */

static inline uint64_t _AArch64_Read_pmevcntr_n_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMEVCNTR_N_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmevcntr_n_el0( uint64_t value )
{
  __asm__ volatile (
    "msr PMEVCNTR_N_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* PMEVTYPER_N_EL0, Performance Monitors Event Type Registers, n = 0 - 30 */

#define AARCH64_PMEVTYPER_N_EL0_EVTCOUNT_9_0( _val ) ( ( _val ) << 0 )
#define AARCH64_PMEVTYPER_N_EL0_EVTCOUNT_9_0_SHIFT 0
#define AARCH64_PMEVTYPER_N_EL0_EVTCOUNT_9_0_MASK 0x3ffU
#define AARCH64_PMEVTYPER_N_EL0_EVTCOUNT_9_0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3ffU )

#define AARCH64_PMEVTYPER_N_EL0_EVTCOUNT_15_10( _val ) ( ( _val ) << 10 )
#define AARCH64_PMEVTYPER_N_EL0_EVTCOUNT_15_10_SHIFT 10
#define AARCH64_PMEVTYPER_N_EL0_EVTCOUNT_15_10_MASK 0xfc00U
#define AARCH64_PMEVTYPER_N_EL0_EVTCOUNT_15_10_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3fU )

#define AARCH64_PMEVTYPER_N_EL0_SH 0x1000000U

#define AARCH64_PMEVTYPER_N_EL0_MT 0x2000000U

#define AARCH64_PMEVTYPER_N_EL0_M 0x4000000U

#define AARCH64_PMEVTYPER_N_EL0_NSH 0x8000000U

#define AARCH64_PMEVTYPER_N_EL0_NSU 0x10000000U

#define AARCH64_PMEVTYPER_N_EL0_NSK 0x20000000U

#define AARCH64_PMEVTYPER_N_EL0_U 0x40000000U

#define AARCH64_PMEVTYPER_N_EL0_P 0x80000000U

static inline uint64_t _AArch64_Read_pmevtyper_n_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMEVTYPER_N_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmevtyper_n_el0( uint64_t value )
{
  __asm__ volatile (
    "msr PMEVTYPER_N_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* PMINTENCLR_EL1, Performance Monitors Interrupt Enable Clear Register */

#define AARCH64_PMINTENCLR_EL1_C 0x80000000U

static inline uint64_t _AArch64_Read_pmintenclr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMINTENCLR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmintenclr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr PMINTENCLR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* PMINTENSET_EL1, Performance Monitors Interrupt Enable Set Register */

#define AARCH64_PMINTENSET_EL1_C 0x80000000U

static inline uint64_t _AArch64_Read_pmintenset_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMINTENSET_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmintenset_el1( uint64_t value )
{
  __asm__ volatile (
    "msr PMINTENSET_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* PMMIR_EL1, Performance Monitors Machine Identification Register */

#define AARCH64_PMMIR_EL1_SLOTS( _val ) ( ( _val ) << 0 )
#define AARCH64_PMMIR_EL1_SLOTS_SHIFT 0
#define AARCH64_PMMIR_EL1_SLOTS_MASK 0xffU
#define AARCH64_PMMIR_EL1_SLOTS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

static inline uint64_t _AArch64_Read_pmmir_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMMIR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* PMOVSCLR_EL0, Performance Monitors Overflow Flag Status Clear Register */

#define AARCH64_PMOVSCLR_EL0_C 0x80000000U

static inline uint64_t _AArch64_Read_pmovsclr_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMOVSCLR_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmovsclr_el0( uint64_t value )
{
  __asm__ volatile (
    "msr PMOVSCLR_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* PMOVSSET_EL0, Performance Monitors Overflow Flag Status Set Register */

#define AARCH64_PMOVSSET_EL0_C 0x80000000U

static inline uint64_t _AArch64_Read_pmovsset_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMOVSSET_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmovsset_el0( uint64_t value )
{
  __asm__ volatile (
    "msr PMOVSSET_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* PMSELR_EL0, Performance Monitors Event Counter Selection Register */

#define AARCH64_PMSELR_EL0_SEL( _val ) ( ( _val ) << 0 )
#define AARCH64_PMSELR_EL0_SEL_SHIFT 0
#define AARCH64_PMSELR_EL0_SEL_MASK 0x1fU
#define AARCH64_PMSELR_EL0_SEL_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1fU )

static inline uint64_t _AArch64_Read_pmselr_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMSELR_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmselr_el0( uint64_t value )
{
  __asm__ volatile (
    "msr PMSELR_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* PMSWINC_EL0, Performance Monitors Software Increment Register */

static inline void _AArch64_Write_pmswinc_el0( uint64_t value )
{
  __asm__ volatile (
    "msr PMSWINC_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* PMUSERENR_EL0, Performance Monitors User Enable Register */

#define AARCH64_PMUSERENR_EL0_EN 0x1U

#define AARCH64_PMUSERENR_EL0_SW 0x2U

#define AARCH64_PMUSERENR_EL0_CR 0x4U

#define AARCH64_PMUSERENR_EL0_ER 0x8U

static inline uint64_t _AArch64_Read_pmuserenr_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMUSERENR_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmuserenr_el0( uint64_t value )
{
  __asm__ volatile (
    "msr PMUSERENR_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* PMXEVCNTR_EL0, Performance Monitors Selected Event Count Register */

#define AARCH64_PMXEVCNTR_EL0_PMEVCNTR_N( _val ) ( ( _val ) << 0 )
#define AARCH64_PMXEVCNTR_EL0_PMEVCNTR_N_SHIFT 0
#define AARCH64_PMXEVCNTR_EL0_PMEVCNTR_N_MASK 0xffffffffU
#define AARCH64_PMXEVCNTR_EL0_PMEVCNTR_N_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )

static inline uint64_t _AArch64_Read_pmxevcntr_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMXEVCNTR_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmxevcntr_el0( uint64_t value )
{
  __asm__ volatile (
    "msr PMXEVCNTR_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* PMXEVTYPER_EL0, Performance Monitors Selected Event Type Register */

static inline uint64_t _AArch64_Read_pmxevtyper_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMXEVTYPER_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmxevtyper_el0( uint64_t value )
{
  __asm__ volatile (
    "msr PMXEVTYPER_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* AMCFGR_EL0, Activity Monitors Configuration Register */

#define AARCH64_AMCFGR_EL0_N( _val ) ( ( _val ) << 0 )
#define AARCH64_AMCFGR_EL0_N_SHIFT 0
#define AARCH64_AMCFGR_EL0_N_MASK 0xffU
#define AARCH64_AMCFGR_EL0_N_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

#define AARCH64_AMCFGR_EL0_SIZE( _val ) ( ( _val ) << 8 )
#define AARCH64_AMCFGR_EL0_SIZE_SHIFT 8
#define AARCH64_AMCFGR_EL0_SIZE_MASK 0x3f00U
#define AARCH64_AMCFGR_EL0_SIZE_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0x3fU )

#define AARCH64_AMCFGR_EL0_HDBG 0x1000000U

#define AARCH64_AMCFGR_EL0_NCG( _val ) ( ( _val ) << 28 )
#define AARCH64_AMCFGR_EL0_NCG_SHIFT 28
#define AARCH64_AMCFGR_EL0_NCG_MASK 0xf0000000U
#define AARCH64_AMCFGR_EL0_NCG_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )

static inline uint64_t _AArch64_Read_amcfgr_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AMCFGR_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AMCG1IDR_EL0, Activity Monitors Counter Group 1 Identification Register */

static inline uint64_t _AArch64_Read_amcg1idr_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AMCG1IDR_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AMCGCR_EL0, Activity Monitors Counter Group Configuration Register */

#define AARCH64_AMCGCR_EL0_CG0NC( _val ) ( ( _val ) << 0 )
#define AARCH64_AMCGCR_EL0_CG0NC_SHIFT 0
#define AARCH64_AMCGCR_EL0_CG0NC_MASK 0xffU
#define AARCH64_AMCGCR_EL0_CG0NC_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )

#define AARCH64_AMCGCR_EL0_CG1NC( _val ) ( ( _val ) << 8 )
#define AARCH64_AMCGCR_EL0_CG1NC_SHIFT 8
#define AARCH64_AMCGCR_EL0_CG1NC_MASK 0xff00U
#define AARCH64_AMCGCR_EL0_CG1NC_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xffU )

static inline uint64_t _AArch64_Read_amcgcr_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AMCGCR_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AMCNTENCLR0_EL0, Activity Monitors Count Enable Clear Register 0 */

static inline uint64_t _AArch64_Read_amcntenclr0_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AMCNTENCLR0_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_amcntenclr0_el0( uint64_t value )
{
  __asm__ volatile (
    "msr AMCNTENCLR0_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* AMCNTENCLR1_EL0, Activity Monitors Count Enable Clear Register 1 */

static inline uint64_t _AArch64_Read_amcntenclr1_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AMCNTENCLR1_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_amcntenclr1_el0( uint64_t value )
{
  __asm__ volatile (
    "msr AMCNTENCLR1_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* AMCNTENSET0_EL0, Activity Monitors Count Enable Set Register 0 */

static inline uint64_t _AArch64_Read_amcntenset0_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AMCNTENSET0_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_amcntenset0_el0( uint64_t value )
{
  __asm__ volatile (
    "msr AMCNTENSET0_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* AMCNTENSET1_EL0, Activity Monitors Count Enable Set Register 1 */

static inline uint64_t _AArch64_Read_amcntenset1_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AMCNTENSET1_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_amcntenset1_el0( uint64_t value )
{
  __asm__ volatile (
    "msr AMCNTENSET1_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* AMCR_EL0, Activity Monitors Control Register */

#define AARCH64_AMCR_EL0_HDBG 0x400U

#define AARCH64_AMCR_EL0_CG1RZ 0x20000U

static inline uint64_t _AArch64_Read_amcr_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AMCR_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_amcr_el0( uint64_t value )
{
  __asm__ volatile (
    "msr AMCR_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR0_N_EL0, Activity Monitors Event Counter Registers 0, n = 0 - 15 */

static inline uint64_t _AArch64_Read_amevcntr0_n_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AMEVCNTR0_N_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_amevcntr0_n_el0( uint64_t value )
{
  __asm__ volatile (
    "msr AMEVCNTR0_N_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTR1_N_EL0, Activity Monitors Event Counter Registers 1, n = 0 - 15 */

static inline uint64_t _AArch64_Read_amevcntr1_n_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AMEVCNTR1_N_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_amevcntr1_n_el0( uint64_t value )
{
  __asm__ volatile (
    "msr AMEVCNTR1_N_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTVOFF0_N_EL2, Activity Monitors Event Counter Virtual Offset Registers 0, n = 0 - */

static inline uint64_t _AArch64_Read_amevcntvoff0_n_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AMEVCNTVOFF0_N_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_amevcntvoff0_n_el2( uint64_t value )
{
  __asm__ volatile (
    "msr AMEVCNTVOFF0_N_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* AMEVCNTVOFF1_N_EL2, Activity Monitors Event Counter Virtual Offset Registers 1, n = 0 - */

static inline uint64_t _AArch64_Read_amevcntvoff1_n_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AMEVCNTVOFF1_N_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_amevcntvoff1_n_el2( uint64_t value )
{
  __asm__ volatile (
    "msr AMEVCNTVOFF1_N_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* AMEVTYPER0_N_EL0, Activity Monitors Event Type Registers 0, n = 0 - 15 */

#define AARCH64_AMEVTYPER0_N_EL0_EVTCOUNT( _val ) ( ( _val ) << 0 )
#define AARCH64_AMEVTYPER0_N_EL0_EVTCOUNT_SHIFT 0
#define AARCH64_AMEVTYPER0_N_EL0_EVTCOUNT_MASK 0xffffU
#define AARCH64_AMEVTYPER0_N_EL0_EVTCOUNT_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )

static inline uint64_t _AArch64_Read_amevtyper0_n_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AMEVTYPER0_N_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* AMEVTYPER1_N_EL0, Activity Monitors Event Type Registers 1, n = 0 - 15 */

#define AARCH64_AMEVTYPER1_N_EL0_EVTCOUNT( _val ) ( ( _val ) << 0 )
#define AARCH64_AMEVTYPER1_N_EL0_EVTCOUNT_SHIFT 0
#define AARCH64_AMEVTYPER1_N_EL0_EVTCOUNT_MASK 0xffffU
#define AARCH64_AMEVTYPER1_N_EL0_EVTCOUNT_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )

static inline uint64_t _AArch64_Read_amevtyper1_n_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AMEVTYPER1_N_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_amevtyper1_n_el0( uint64_t value )
{
  __asm__ volatile (
    "msr AMEVTYPER1_N_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* AMUSERENR_EL0, Activity Monitors User Enable Register */

#define AARCH64_AMUSERENR_EL0_EN 0x1U

static inline uint64_t _AArch64_Read_amuserenr_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, AMUSERENR_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_amuserenr_el0( uint64_t value )
{
  __asm__ volatile (
    "msr AMUSERENR_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* PMBIDR_EL1, Profiling Buffer ID Register */

#define AARCH64_PMBIDR_EL1_ALIGN( _val ) ( ( _val ) << 0 )
#define AARCH64_PMBIDR_EL1_ALIGN_SHIFT 0
#define AARCH64_PMBIDR_EL1_ALIGN_MASK 0xfU
#define AARCH64_PMBIDR_EL1_ALIGN_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_PMBIDR_EL1_P 0x10U

#define AARCH64_PMBIDR_EL1_F 0x20U

static inline uint64_t _AArch64_Read_pmbidr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMBIDR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* PMBLIMITR_EL1, Profiling Buffer Limit Address Register */

#define AARCH64_PMBLIMITR_EL1_E 0x1U

#define AARCH64_PMBLIMITR_EL1_FM( _val ) ( ( _val ) << 1 )
#define AARCH64_PMBLIMITR_EL1_FM_SHIFT 1
#define AARCH64_PMBLIMITR_EL1_FM_MASK 0x6U
#define AARCH64_PMBLIMITR_EL1_FM_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0x3U )

#define AARCH64_PMBLIMITR_EL1_LIMIT( _val ) ( ( _val ) << 12 )
#define AARCH64_PMBLIMITR_EL1_LIMIT_SHIFT 12
#define AARCH64_PMBLIMITR_EL1_LIMIT_MASK 0xfffffffffffff000ULL
#define AARCH64_PMBLIMITR_EL1_LIMIT_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfffffffffffffULL )

static inline uint64_t _AArch64_Read_pmblimitr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMBLIMITR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmblimitr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr PMBLIMITR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* PMBPTR_EL1, Profiling Buffer Write Pointer Register */

static inline uint64_t _AArch64_Read_pmbptr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMBPTR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmbptr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr PMBPTR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* PMBSR_EL1, Profiling Buffer Status/syndrome Register */

#define AARCH64_PMBSR_EL1_BSC( _val ) ( ( _val ) << 0 )
#define AARCH64_PMBSR_EL1_BSC_SHIFT 0
#define AARCH64_PMBSR_EL1_BSC_MASK 0x3fU
#define AARCH64_PMBSR_EL1_BSC_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )

#define AARCH64_PMBSR_EL1_FSC( _val ) ( ( _val ) << 0 )
#define AARCH64_PMBSR_EL1_FSC_SHIFT 0
#define AARCH64_PMBSR_EL1_FSC_MASK 0x3fU
#define AARCH64_PMBSR_EL1_FSC_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )

#define AARCH64_PMBSR_EL1_MSS( _val ) ( ( _val ) << 0 )
#define AARCH64_PMBSR_EL1_MSS_SHIFT 0
#define AARCH64_PMBSR_EL1_MSS_MASK 0xffffU
#define AARCH64_PMBSR_EL1_MSS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )

#define AARCH64_PMBSR_EL1_COLL 0x10000U

#define AARCH64_PMBSR_EL1_S 0x20000U

#define AARCH64_PMBSR_EL1_EA 0x40000U

#define AARCH64_PMBSR_EL1_DL 0x80000U

#define AARCH64_PMBSR_EL1_EC( _val ) ( ( _val ) << 26 )
#define AARCH64_PMBSR_EL1_EC_SHIFT 26
#define AARCH64_PMBSR_EL1_EC_MASK 0xfc000000U
#define AARCH64_PMBSR_EL1_EC_GET( _reg ) \
  ( ( ( _reg ) >> 26 ) & 0x3fU )

static inline uint64_t _AArch64_Read_pmbsr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMBSR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmbsr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr PMBSR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* PMSCR_EL1, Statistical Profiling Control Register (EL1) */

#define AARCH64_PMSCR_EL1_E0SPE 0x1U

#define AARCH64_PMSCR_EL1_E1SPE 0x2U

#define AARCH64_PMSCR_EL1_CX 0x8U

#define AARCH64_PMSCR_EL1_PA 0x10U

#define AARCH64_PMSCR_EL1_TS 0x20U

#define AARCH64_PMSCR_EL1_PCT( _val ) ( ( _val ) << 6 )
#define AARCH64_PMSCR_EL1_PCT_SHIFT 6
#define AARCH64_PMSCR_EL1_PCT_MASK 0xc0U
#define AARCH64_PMSCR_EL1_PCT_GET( _reg ) \
  ( ( ( _reg ) >> 6 ) & 0x3U )

static inline uint64_t _AArch64_Read_pmscr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMSCR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmscr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr PMSCR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* PMSCR_EL2, Statistical Profiling Control Register (EL2) */

#define AARCH64_PMSCR_EL2_E0HSPE 0x1U

#define AARCH64_PMSCR_EL2_E2SPE 0x2U

#define AARCH64_PMSCR_EL2_CX 0x8U

#define AARCH64_PMSCR_EL2_PA 0x10U

#define AARCH64_PMSCR_EL2_TS 0x20U

#define AARCH64_PMSCR_EL2_PCT( _val ) ( ( _val ) << 6 )
#define AARCH64_PMSCR_EL2_PCT_SHIFT 6
#define AARCH64_PMSCR_EL2_PCT_MASK 0xc0U
#define AARCH64_PMSCR_EL2_PCT_GET( _reg ) \
  ( ( ( _reg ) >> 6 ) & 0x3U )

static inline uint64_t _AArch64_Read_pmscr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMSCR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmscr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr PMSCR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* PMSEVFR_EL1, Sampling Event Filter Register */

#define AARCH64_PMSEVFR_EL1_E_1 0x2U

#define AARCH64_PMSEVFR_EL1_E_3 0x8U

#define AARCH64_PMSEVFR_EL1_E_5 0x20U

#define AARCH64_PMSEVFR_EL1_E_7 0x80U

#define AARCH64_PMSEVFR_EL1_E_11 0x800U

#define AARCH64_PMSEVFR_EL1_E_12 0x1000U

#define AARCH64_PMSEVFR_EL1_E_13 0x2000U

#define AARCH64_PMSEVFR_EL1_E_14 0x4000U

#define AARCH64_PMSEVFR_EL1_E_15 0x8000U

#define AARCH64_PMSEVFR_EL1_E_17 0x20000U

#define AARCH64_PMSEVFR_EL1_E_18 0x40000U

#define AARCH64_PMSEVFR_EL1_E_24 0x1000000U

#define AARCH64_PMSEVFR_EL1_E_25 0x2000000U

#define AARCH64_PMSEVFR_EL1_E_26 0x4000000U

#define AARCH64_PMSEVFR_EL1_E_27 0x8000000U

#define AARCH64_PMSEVFR_EL1_E_28 0x10000000U

#define AARCH64_PMSEVFR_EL1_E_29 0x20000000U

#define AARCH64_PMSEVFR_EL1_E_30 0x40000000U

#define AARCH64_PMSEVFR_EL1_E_31 0x80000000U

#define AARCH64_PMSEVFR_EL1_E_48 0x1000000000000ULL

#define AARCH64_PMSEVFR_EL1_E_49 0x2000000000000ULL

#define AARCH64_PMSEVFR_EL1_E_50 0x4000000000000ULL

#define AARCH64_PMSEVFR_EL1_E_51 0x8000000000000ULL

#define AARCH64_PMSEVFR_EL1_E_52 0x10000000000000ULL

#define AARCH64_PMSEVFR_EL1_E_53 0x20000000000000ULL

#define AARCH64_PMSEVFR_EL1_E_54 0x40000000000000ULL

#define AARCH64_PMSEVFR_EL1_E_55 0x80000000000000ULL

#define AARCH64_PMSEVFR_EL1_E_56 0x100000000000000ULL

#define AARCH64_PMSEVFR_EL1_E_57 0x200000000000000ULL

#define AARCH64_PMSEVFR_EL1_E_58 0x400000000000000ULL

#define AARCH64_PMSEVFR_EL1_E_59 0x800000000000000ULL

#define AARCH64_PMSEVFR_EL1_E_60 0x1000000000000000ULL

#define AARCH64_PMSEVFR_EL1_E_61 0x2000000000000000ULL

#define AARCH64_PMSEVFR_EL1_E_62 0x4000000000000000ULL

#define AARCH64_PMSEVFR_EL1_E_63 0x8000000000000000ULL

static inline uint64_t _AArch64_Read_pmsevfr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMSEVFR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmsevfr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr PMSEVFR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* PMSFCR_EL1, Sampling Filter Control Register */

#define AARCH64_PMSFCR_EL1_FE 0x1U

#define AARCH64_PMSFCR_EL1_FT 0x2U

#define AARCH64_PMSFCR_EL1_FL 0x4U

#define AARCH64_PMSFCR_EL1_B 0x10000U

#define AARCH64_PMSFCR_EL1_LD 0x20000U

#define AARCH64_PMSFCR_EL1_ST 0x40000U

static inline uint64_t _AArch64_Read_pmsfcr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMSFCR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmsfcr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr PMSFCR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* PMSICR_EL1, Sampling Interval Counter Register */

#define AARCH64_PMSICR_EL1_COUNT( _val ) ( ( _val ) << 0 )
#define AARCH64_PMSICR_EL1_COUNT_SHIFT 0
#define AARCH64_PMSICR_EL1_COUNT_MASK 0xffffffffU
#define AARCH64_PMSICR_EL1_COUNT_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )

#define AARCH64_PMSICR_EL1_ECOUNT( _val ) ( ( _val ) << 56 )
#define AARCH64_PMSICR_EL1_ECOUNT_SHIFT 56
#define AARCH64_PMSICR_EL1_ECOUNT_MASK 0xff00000000000000ULL
#define AARCH64_PMSICR_EL1_ECOUNT_GET( _reg ) \
  ( ( ( _reg ) >> 56 ) & 0xffULL )

static inline uint64_t _AArch64_Read_pmsicr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMSICR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmsicr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr PMSICR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* PMSIDR_EL1, Sampling Profiling ID Register */

#define AARCH64_PMSIDR_EL1_FE 0x1U

#define AARCH64_PMSIDR_EL1_FT 0x2U

#define AARCH64_PMSIDR_EL1_FL 0x4U

#define AARCH64_PMSIDR_EL1_ARCHINST 0x8U

#define AARCH64_PMSIDR_EL1_LDS 0x10U

#define AARCH64_PMSIDR_EL1_ERND 0x20U

#define AARCH64_PMSIDR_EL1_INTERVAL( _val ) ( ( _val ) << 8 )
#define AARCH64_PMSIDR_EL1_INTERVAL_SHIFT 8
#define AARCH64_PMSIDR_EL1_INTERVAL_MASK 0xf00U
#define AARCH64_PMSIDR_EL1_INTERVAL_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xfU )

#define AARCH64_PMSIDR_EL1_MAXSIZE( _val ) ( ( _val ) << 12 )
#define AARCH64_PMSIDR_EL1_MAXSIZE_SHIFT 12
#define AARCH64_PMSIDR_EL1_MAXSIZE_MASK 0xf000U
#define AARCH64_PMSIDR_EL1_MAXSIZE_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )

#define AARCH64_PMSIDR_EL1_COUNTSIZE( _val ) ( ( _val ) << 16 )
#define AARCH64_PMSIDR_EL1_COUNTSIZE_SHIFT 16
#define AARCH64_PMSIDR_EL1_COUNTSIZE_MASK 0xf0000U
#define AARCH64_PMSIDR_EL1_COUNTSIZE_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )

static inline uint64_t _AArch64_Read_pmsidr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMSIDR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* PMSIRR_EL1, Sampling Interval Reload Register */

#define AARCH64_PMSIRR_EL1_RND 0x1U

#define AARCH64_PMSIRR_EL1_INTERVAL( _val ) ( ( _val ) << 8 )
#define AARCH64_PMSIRR_EL1_INTERVAL_SHIFT 8
#define AARCH64_PMSIRR_EL1_INTERVAL_MASK 0xffffff00U
#define AARCH64_PMSIRR_EL1_INTERVAL_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xffffffU )

static inline uint64_t _AArch64_Read_pmsirr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMSIRR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmsirr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr PMSIRR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* PMSLATFR_EL1, Sampling Latency Filter Register */

#define AARCH64_PMSLATFR_EL1_MINLAT( _val ) ( ( _val ) << 0 )
#define AARCH64_PMSLATFR_EL1_MINLAT_SHIFT 0
#define AARCH64_PMSLATFR_EL1_MINLAT_MASK 0xfffU
#define AARCH64_PMSLATFR_EL1_MINLAT_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfffU )

static inline uint64_t _AArch64_Read_pmslatfr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, PMSLATFR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_pmslatfr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr PMSLATFR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* DISR_EL1, Deferred Interrupt Status Register */

#define AARCH64_DISR_EL1_DFSC( _val ) ( ( _val ) << 0 )
#define AARCH64_DISR_EL1_DFSC_SHIFT 0
#define AARCH64_DISR_EL1_DFSC_MASK 0x3fU
#define AARCH64_DISR_EL1_DFSC_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )

#define AARCH64_DISR_EL1_ISS( _val ) ( ( _val ) << 0 )
#define AARCH64_DISR_EL1_ISS_SHIFT 0
#define AARCH64_DISR_EL1_ISS_MASK 0xffffffU
#define AARCH64_DISR_EL1_ISS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffU )

#define AARCH64_DISR_EL1_EA 0x200U

#define AARCH64_DISR_EL1_AET( _val ) ( ( _val ) << 10 )
#define AARCH64_DISR_EL1_AET_SHIFT 10
#define AARCH64_DISR_EL1_AET_MASK 0x1c00U
#define AARCH64_DISR_EL1_AET_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x7U )

#define AARCH64_DISR_EL1_IDS 0x1000000U

#define AARCH64_DISR_EL1_A 0x80000000U

static inline uint64_t _AArch64_Read_disr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, DISR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_disr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr DISR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* ERRIDR_EL1, Error Record ID Register */

#define AARCH64_ERRIDR_EL1_NUM( _val ) ( ( _val ) << 0 )
#define AARCH64_ERRIDR_EL1_NUM_SHIFT 0
#define AARCH64_ERRIDR_EL1_NUM_MASK 0xffffU
#define AARCH64_ERRIDR_EL1_NUM_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )

static inline uint64_t _AArch64_Read_erridr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ERRIDR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ERRSELR_EL1, Error Record Select Register */

#define AARCH64_ERRSELR_EL1_SEL( _val ) ( ( _val ) << 0 )
#define AARCH64_ERRSELR_EL1_SEL_SHIFT 0
#define AARCH64_ERRSELR_EL1_SEL_MASK 0xffffU
#define AARCH64_ERRSELR_EL1_SEL_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )

static inline uint64_t _AArch64_Read_errselr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ERRSELR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_errselr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr ERRSELR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* ERXADDR_EL1, Selected Error Record Address Register */

static inline uint64_t _AArch64_Read_erxaddr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ERXADDR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_erxaddr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr ERXADDR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* ERXCTLR_EL1, Selected Error Record Control Register */

static inline uint64_t _AArch64_Read_erxctlr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ERXCTLR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_erxctlr_el1( uint64_t value )
{
  __asm__ volatile (
    "msr ERXCTLR_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* ERXFR_EL1, Selected Error Record Feature Register */

static inline uint64_t _AArch64_Read_erxfr_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ERXFR_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ERXMISC0_EL1, Selected Error Record Miscellaneous Register 0 */

static inline uint64_t _AArch64_Read_erxmisc0_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ERXMISC0_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_erxmisc0_el1( uint64_t value )
{
  __asm__ volatile (
    "msr ERXMISC0_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* ERXMISC1_EL1, Selected Error Record Miscellaneous Register 1 */

static inline uint64_t _AArch64_Read_erxmisc1_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ERXMISC1_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_erxmisc1_el1( uint64_t value )
{
  __asm__ volatile (
    "msr ERXMISC1_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* ERXMISC2_EL1, Selected Error Record Miscellaneous Register 2 */

static inline uint64_t _AArch64_Read_erxmisc2_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ERXMISC2_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_erxmisc2_el1( uint64_t value )
{
  __asm__ volatile (
    "msr ERXMISC2_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* ERXMISC3_EL1, Selected Error Record Miscellaneous Register 3 */

static inline uint64_t _AArch64_Read_erxmisc3_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ERXMISC3_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_erxmisc3_el1( uint64_t value )
{
  __asm__ volatile (
    "msr ERXMISC3_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* ERXPFGCDN_EL1, Selected Pseudo-fault Generation Countdown Register */

static inline uint64_t _AArch64_Read_erxpfgcdn_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ERXPFGCDN_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_erxpfgcdn_el1( uint64_t value )
{
  __asm__ volatile (
    "msr ERXPFGCDN_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* ERXPFGCTL_EL1, Selected Pseudo-fault Generation Control Register */

static inline uint64_t _AArch64_Read_erxpfgctl_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ERXPFGCTL_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_erxpfgctl_el1( uint64_t value )
{
  __asm__ volatile (
    "msr ERXPFGCTL_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* ERXPFGF_EL1, Selected Pseudo-fault Generation Feature Register */

static inline uint64_t _AArch64_Read_erxpfgf_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ERXPFGF_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* ERXSTATUS_EL1, Selected Error Record Primary Status Register */

static inline uint64_t _AArch64_Read_erxstatus_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, ERXSTATUS_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_erxstatus_el1( uint64_t value )
{
  __asm__ volatile (
    "msr ERXSTATUS_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* VDISR_EL2, Virtual Deferred Interrupt Status Register */

#define AARCH64_VDISR_EL2_FS_3_0( _val ) ( ( _val ) << 0 )
#define AARCH64_VDISR_EL2_FS_3_0_SHIFT 0
#define AARCH64_VDISR_EL2_FS_3_0_MASK 0xfU
#define AARCH64_VDISR_EL2_FS_3_0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xfU )

#define AARCH64_VDISR_EL2_STATUS( _val ) ( ( _val ) << 0 )
#define AARCH64_VDISR_EL2_STATUS_SHIFT 0
#define AARCH64_VDISR_EL2_STATUS_MASK 0x3fU
#define AARCH64_VDISR_EL2_STATUS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )

#define AARCH64_VDISR_EL2_ISS( _val ) ( ( _val ) << 0 )
#define AARCH64_VDISR_EL2_ISS_SHIFT 0
#define AARCH64_VDISR_EL2_ISS_MASK 0xffffffU
#define AARCH64_VDISR_EL2_ISS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffU )

#define AARCH64_VDISR_EL2_LPAE 0x200U

#define AARCH64_VDISR_EL2_FS_4 0x400U

#define AARCH64_VDISR_EL2_EXT 0x1000U

#define AARCH64_VDISR_EL2_AET( _val ) ( ( _val ) << 14 )
#define AARCH64_VDISR_EL2_AET_SHIFT 14
#define AARCH64_VDISR_EL2_AET_MASK 0xc000U
#define AARCH64_VDISR_EL2_AET_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x3U )

#define AARCH64_VDISR_EL2_IDS 0x1000000U

#define AARCH64_VDISR_EL2_A 0x80000000U

static inline uint64_t _AArch64_Read_vdisr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, VDISR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_vdisr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr VDISR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* VSESR_EL2, Virtual SError Exception Syndrome Register */

#define AARCH64_VSESR_EL2_ISS( _val ) ( ( _val ) << 0 )
#define AARCH64_VSESR_EL2_ISS_SHIFT 0
#define AARCH64_VSESR_EL2_ISS_MASK 0xffffffU
#define AARCH64_VSESR_EL2_ISS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffU )

#define AARCH64_VSESR_EL2_EXT 0x1000U

#define AARCH64_VSESR_EL2_AET( _val ) ( ( _val ) << 14 )
#define AARCH64_VSESR_EL2_AET_SHIFT 14
#define AARCH64_VSESR_EL2_AET_MASK 0xc000U
#define AARCH64_VSESR_EL2_AET_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x3U )

#define AARCH64_VSESR_EL2_IDS 0x1000000U

static inline uint64_t _AArch64_Read_vsesr_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, VSESR_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_vsesr_el2( uint64_t value )
{
  __asm__ volatile (
    "msr VSESR_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTFRQ_EL0, Counter-timer Frequency Register */

static inline uint64_t _AArch64_Read_cntfrq_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTFRQ_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cntfrq_el0( uint64_t value )
{
  __asm__ volatile (
    "msr CNTFRQ_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTHCTL_EL2, Counter-timer Hypervisor Control Register */

#define AARCH64_CNTHCTL_EL2_EL0PCTEN 0x1U

#define AARCH64_CNTHCTL_EL2_EL1PCTEN_0 0x1U

#define AARCH64_CNTHCTL_EL2_EL0VCTEN 0x2U

#define AARCH64_CNTHCTL_EL2_EL1PCEN 0x2U

#define AARCH64_CNTHCTL_EL2_EVNTEN 0x4U

#define AARCH64_CNTHCTL_EL2_EVNTDIR 0x8U

#define AARCH64_CNTHCTL_EL2_EVNTI( _val ) ( ( _val ) << 4 )
#define AARCH64_CNTHCTL_EL2_EVNTI_SHIFT 4
#define AARCH64_CNTHCTL_EL2_EVNTI_MASK 0xf0U
#define AARCH64_CNTHCTL_EL2_EVNTI_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_CNTHCTL_EL2_EL0VTEN 0x100U

#define AARCH64_CNTHCTL_EL2_EL0PTEN 0x200U

#define AARCH64_CNTHCTL_EL2_EL1PCTEN_1 0x400U

#define AARCH64_CNTHCTL_EL2_EL1PTEN 0x800U

#define AARCH64_CNTHCTL_EL2_ECV 0x1000U

#define AARCH64_CNTHCTL_EL2_EL1TVT 0x2000U

#define AARCH64_CNTHCTL_EL2_EL1TVCT 0x4000U

#define AARCH64_CNTHCTL_EL2_EL1NVPCT 0x8000U

#define AARCH64_CNTHCTL_EL2_EL1NVVCT 0x10000U

#define AARCH64_CNTHCTL_EL2_EVNTIS 0x20000U

static inline uint64_t _AArch64_Read_cnthctl_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTHCTL_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cnthctl_el2( uint64_t value )
{
  __asm__ volatile (
    "msr CNTHCTL_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTHP_CTL_EL2, Counter-timer Hypervisor Physical Timer Control Register */

#define AARCH64_CNTHP_CTL_EL2_ENABLE 0x1U

#define AARCH64_CNTHP_CTL_EL2_IMASK 0x2U

#define AARCH64_CNTHP_CTL_EL2_ISTATUS 0x4U

static inline uint64_t _AArch64_Read_cnthp_ctl_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTHP_CTL_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cnthp_ctl_el2( uint64_t value )
{
  __asm__ volatile (
    "msr CNTHP_CTL_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTHP_CVAL_EL2, Counter-timer Physical Timer CompareValue Register (EL2) */

static inline uint64_t _AArch64_Read_cnthp_cval_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTHP_CVAL_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cnthp_cval_el2( uint64_t value )
{
  __asm__ volatile (
    "msr CNTHP_CVAL_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTHP_TVAL_EL2, Counter-timer Physical Timer TimerValue Register (EL2) */

#define AARCH64_CNTHP_TVAL_EL2_TIMERVALUE( _val ) ( ( _val ) << 0 )
#define AARCH64_CNTHP_TVAL_EL2_TIMERVALUE_SHIFT 0
#define AARCH64_CNTHP_TVAL_EL2_TIMERVALUE_MASK 0xffffffffU
#define AARCH64_CNTHP_TVAL_EL2_TIMERVALUE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )

static inline uint64_t _AArch64_Read_cnthp_tval_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTHP_TVAL_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cnthp_tval_el2( uint64_t value )
{
  __asm__ volatile (
    "msr CNTHP_TVAL_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTHPS_CTL_EL2, Counter-timer Secure Physical Timer Control Register (EL2) */

#define AARCH64_CNTHPS_CTL_EL2_ENABLE 0x1U

#define AARCH64_CNTHPS_CTL_EL2_IMASK 0x2U

#define AARCH64_CNTHPS_CTL_EL2_ISTATUS 0x4U

static inline uint64_t _AArch64_Read_cnthps_ctl_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTHPS_CTL_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cnthps_ctl_el2( uint64_t value )
{
  __asm__ volatile (
    "msr CNTHPS_CTL_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTHPS_CVAL_EL2, Counter-timer Secure Physical Timer CompareValue Register (EL2) */

static inline uint64_t _AArch64_Read_cnthps_cval_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTHPS_CVAL_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cnthps_cval_el2( uint64_t value )
{
  __asm__ volatile (
    "msr CNTHPS_CVAL_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTHPS_TVAL_EL2, Counter-timer Secure Physical Timer TimerValue Register (EL2) */

#define AARCH64_CNTHPS_TVAL_EL2_TIMERVALUE( _val ) ( ( _val ) << 0 )
#define AARCH64_CNTHPS_TVAL_EL2_TIMERVALUE_SHIFT 0
#define AARCH64_CNTHPS_TVAL_EL2_TIMERVALUE_MASK 0xffffffffU
#define AARCH64_CNTHPS_TVAL_EL2_TIMERVALUE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )

static inline uint64_t _AArch64_Read_cnthps_tval_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTHPS_TVAL_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cnthps_tval_el2( uint64_t value )
{
  __asm__ volatile (
    "msr CNTHPS_TVAL_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTHV_CTL_EL2, Counter-timer Virtual Timer Control Register (EL2) */

#define AARCH64_CNTHV_CTL_EL2_ENABLE 0x1U

#define AARCH64_CNTHV_CTL_EL2_IMASK 0x2U

#define AARCH64_CNTHV_CTL_EL2_ISTATUS 0x4U

static inline uint64_t _AArch64_Read_cnthv_ctl_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTHV_CTL_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cnthv_ctl_el2( uint64_t value )
{
  __asm__ volatile (
    "msr CNTHV_CTL_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTHV_CVAL_EL2, Counter-timer Virtual Timer CompareValue Register (EL2) */

static inline uint64_t _AArch64_Read_cnthv_cval_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTHV_CVAL_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cnthv_cval_el2( uint64_t value )
{
  __asm__ volatile (
    "msr CNTHV_CVAL_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTHV_TVAL_EL2, Counter-timer Virtual Timer TimerValue Register (EL2) */

#define AARCH64_CNTHV_TVAL_EL2_TIMERVALUE( _val ) ( ( _val ) << 0 )
#define AARCH64_CNTHV_TVAL_EL2_TIMERVALUE_SHIFT 0
#define AARCH64_CNTHV_TVAL_EL2_TIMERVALUE_MASK 0xffffffffU
#define AARCH64_CNTHV_TVAL_EL2_TIMERVALUE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )

static inline uint64_t _AArch64_Read_cnthv_tval_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTHV_TVAL_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cnthv_tval_el2( uint64_t value )
{
  __asm__ volatile (
    "msr CNTHV_TVAL_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTHVS_CTL_EL2, Counter-timer Secure Virtual Timer Control Register (EL2) */

#define AARCH64_CNTHVS_CTL_EL2_ENABLE 0x1U

#define AARCH64_CNTHVS_CTL_EL2_IMASK 0x2U

#define AARCH64_CNTHVS_CTL_EL2_ISTATUS 0x4U

static inline uint64_t _AArch64_Read_cnthvs_ctl_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTHVS_CTL_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cnthvs_ctl_el2( uint64_t value )
{
  __asm__ volatile (
    "msr CNTHVS_CTL_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTHVS_CVAL_EL2, Counter-timer Secure Virtual Timer CompareValue Register (EL2) */

static inline uint64_t _AArch64_Read_cnthvs_cval_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTHVS_CVAL_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cnthvs_cval_el2( uint64_t value )
{
  __asm__ volatile (
    "msr CNTHVS_CVAL_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTHVS_TVAL_EL2, Counter-timer Secure Virtual Timer TimerValue Register (EL2) */

#define AARCH64_CNTHVS_TVAL_EL2_TIMERVALUE( _val ) ( ( _val ) << 0 )
#define AARCH64_CNTHVS_TVAL_EL2_TIMERVALUE_SHIFT 0
#define AARCH64_CNTHVS_TVAL_EL2_TIMERVALUE_MASK 0xffffffffU
#define AARCH64_CNTHVS_TVAL_EL2_TIMERVALUE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )

static inline uint64_t _AArch64_Read_cnthvs_tval_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTHVS_TVAL_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cnthvs_tval_el2( uint64_t value )
{
  __asm__ volatile (
    "msr CNTHVS_TVAL_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTKCTL_EL1, Counter-timer Kernel Control Register */

#define AARCH64_CNTKCTL_EL1_EL0PCTEN 0x1U

#define AARCH64_CNTKCTL_EL1_EL0VCTEN 0x2U

#define AARCH64_CNTKCTL_EL1_EVNTEN 0x4U

#define AARCH64_CNTKCTL_EL1_EVNTDIR 0x8U

#define AARCH64_CNTKCTL_EL1_EVNTI( _val ) ( ( _val ) << 4 )
#define AARCH64_CNTKCTL_EL1_EVNTI_SHIFT 4
#define AARCH64_CNTKCTL_EL1_EVNTI_MASK 0xf0U
#define AARCH64_CNTKCTL_EL1_EVNTI_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xfU )

#define AARCH64_CNTKCTL_EL1_EL0VTEN 0x100U

#define AARCH64_CNTKCTL_EL1_EL0PTEN 0x200U

#define AARCH64_CNTKCTL_EL1_EVNTIS 0x20000U

static inline uint64_t _AArch64_Read_cntkctl_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTKCTL_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cntkctl_el1( uint64_t value )
{
  __asm__ volatile (
    "msr CNTKCTL_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTP_CTL_EL0, Counter-timer Physical Timer Control Register */

#define AARCH64_CNTP_CTL_EL0_ENABLE 0x1U

#define AARCH64_CNTP_CTL_EL0_IMASK 0x2U

#define AARCH64_CNTP_CTL_EL0_ISTATUS 0x4U

static inline uint64_t _AArch64_Read_cntp_ctl_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTP_CTL_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cntp_ctl_el0( uint64_t value )
{
  __asm__ volatile (
    "msr CNTP_CTL_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTP_CVAL_EL0, Counter-timer Physical Timer CompareValue Register */

static inline uint64_t _AArch64_Read_cntp_cval_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTP_CVAL_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cntp_cval_el0( uint64_t value )
{
  __asm__ volatile (
    "msr CNTP_CVAL_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTP_TVAL_EL0, Counter-timer Physical Timer TimerValue Register */

#define AARCH64_CNTP_TVAL_EL0_TIMERVALUE( _val ) ( ( _val ) << 0 )
#define AARCH64_CNTP_TVAL_EL0_TIMERVALUE_SHIFT 0
#define AARCH64_CNTP_TVAL_EL0_TIMERVALUE_MASK 0xffffffffU
#define AARCH64_CNTP_TVAL_EL0_TIMERVALUE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )

static inline uint64_t _AArch64_Read_cntp_tval_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTP_TVAL_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cntp_tval_el0( uint64_t value )
{
  __asm__ volatile (
    "msr CNTP_TVAL_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTPCTSS_EL0, Counter-timer Self-Synchronized Physical Count Register */

static inline uint64_t _AArch64_Read_cntpctss_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTPCTSS_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* CNTPCT_EL0, Counter-timer Physical Count Register */

static inline uint64_t _AArch64_Read_cntpct_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTPCT_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* CNTPS_CTL_EL1, Counter-timer Physical Secure Timer Control Register */

#define AARCH64_CNTPS_CTL_EL1_ENABLE 0x1U

#define AARCH64_CNTPS_CTL_EL1_IMASK 0x2U

#define AARCH64_CNTPS_CTL_EL1_ISTATUS 0x4U

static inline uint64_t _AArch64_Read_cntps_ctl_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTPS_CTL_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cntps_ctl_el1( uint64_t value )
{
  __asm__ volatile (
    "msr CNTPS_CTL_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTPOFF_EL2, Counter-timer Physical Offset Register */

static inline uint64_t _AArch64_Read_cntpoff_el2( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTPOFF_EL2" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cntpoff_el2( uint64_t value )
{
  __asm__ volatile (
    "msr CNTPOFF_EL2, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTPS_CVAL_EL1, Counter-timer Physical Secure Timer CompareValue Register */

static inline uint64_t _AArch64_Read_cntps_cval_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTPS_CVAL_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cntps_cval_el1( uint64_t value )
{
  __asm__ volatile (
    "msr CNTPS_CVAL_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTPS_TVAL_EL1, Counter-timer Physical Secure Timer TimerValue Register */

#define AARCH64_CNTPS_TVAL_EL1_TIMERVALUE( _val ) ( ( _val ) << 0 )
#define AARCH64_CNTPS_TVAL_EL1_TIMERVALUE_SHIFT 0
#define AARCH64_CNTPS_TVAL_EL1_TIMERVALUE_MASK 0xffffffffU
#define AARCH64_CNTPS_TVAL_EL1_TIMERVALUE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )

static inline uint64_t _AArch64_Read_cntps_tval_el1( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTPS_TVAL_EL1" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cntps_tval_el1( uint64_t value )
{
  __asm__ volatile (
    "msr CNTPS_TVAL_EL1, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTV_CTL_EL0, Counter-timer Virtual Timer Control Register */

#define AARCH64_CNTV_CTL_EL0_ENABLE 0x1U

#define AARCH64_CNTV_CTL_EL0_IMASK 0x2U

#define AARCH64_CNTV_CTL_EL0_ISTATUS 0x4U

static inline uint64_t _AArch64_Read_cntv_ctl_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTV_CTL_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cntv_ctl_el0( uint64_t value )
{
  __asm__ volatile (
    "msr CNTV_CTL_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTV_CVAL_EL0, Counter-timer Virtual Timer CompareValue Register */

static inline uint64_t _AArch64_Read_cntv_cval_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTV_CVAL_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cntv_cval_el0( uint64_t value )
{
  __asm__ volatile (
    "msr CNTV_CVAL_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTV_TVAL_EL0, Counter-timer Virtual Timer TimerValue Register */

#define AARCH64_CNTV_TVAL_EL0_TIMERVALUE( _val ) ( ( _val ) << 0 )
#define AARCH64_CNTV_TVAL_EL0_TIMERVALUE_SHIFT 0
#define AARCH64_CNTV_TVAL_EL0_TIMERVALUE_MASK 0xffffffffU
#define AARCH64_CNTV_TVAL_EL0_TIMERVALUE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )

static inline uint64_t _AArch64_Read_cntv_tval_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTV_TVAL_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

static inline void _AArch64_Write_cntv_tval_el0( uint64_t value )
{
  __asm__ volatile (
    "msr CNTV_TVAL_EL0, %0" : : "r" ( value ) : "memory"
  );
}

/* CNTVCTSS_EL0, Counter-timer Self-Synchronized Virtual Count Register */

static inline uint64_t _AArch64_Read_cntvctss_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTVCTSS_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

/* CNTVCT_EL0, Counter-timer Virtual Count Register */

static inline uint64_t _AArch64_Read_cntvct_el0( void )
{
  uint64_t value;

  __asm__ volatile (
    "mrs %0, CNTVCT_EL0" : "=&r" ( value ) : : "memory"
  );

  return value;
}

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_AARCH64_SYSTEM_REGISTERS_H */
