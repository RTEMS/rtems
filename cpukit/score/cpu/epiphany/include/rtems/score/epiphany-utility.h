/**
 * @file
 *
 * @addtogroup RTEMSScoreCPU
 *
 * @brief This include file contains macros pertaining to the
 *  Epiphany processor family.
 */

/*
 * Copyright (c) 2015 University of York.
 * Hesham ALMatary <hmka501@york.ac.uk>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _EPIPHANY_UTILITY_H
#define _EPIPHANY_UTILITY_H

/* eCore IRQs */
typedef enum
{
  START,
  SW_EXCEPTION,
  MEM_FAULT,
  TIMER0,
  TIMER1,
  SMP_MESSAGE,
  DMA0,
  DMA1,
  SER,
} EPIPHANY_IRQ_PER_CORE_T;

/*  Per-core IO mapped register addresses
 *  @see Epiphany architecture reference.
 */
#define EPIPHANY_PER_CORE_REG_CONFIG      0xF0400
#define EPIPHANY_PER_CORE_REG_STATUS      0xF0404
#define EPIPHANY_PER_CORE_REG_PC          0xF0408
#define EPIPHANY_PER_CORE_REG_DEBUGSTATUS 0xF040C
#define EPIPHANY_PER_CORE_REG_LC          0xF0414
#define EPIPHANY_PER_CORE_REG_LS          0xF0418
#define EPIPHANY_PER_CORE_REG_LE          0xF041C
#define EPIPHANY_PER_CORE_REG_IRET        0xF0420
#define EPIPHANY_PER_CORE_REG_IMASK       0xF0424
#define EPIPHANY_PER_CORE_REG_ILAT        0xF0428
#define EPIPHANY_PER_CORE_REG_ILATST      0xF042C
#define EPIPHANY_PER_CORE_REG_ILATCL      0xF0430
#define EPIPHANY_PER_CORE_REG_IPEND       0xF0434
#define EPIPHANY_PER_CORE_REG_FSTATUS     0xF0440
#define EPIPHANY_PER_CORE_REG_DEBUGCMD    0xF0448
#define EPIPHANY_PER_CORE_REG_RESETCORE   0xF070C

/* Event timer registers */
#define EPIPHANY_PER_CORE_REG_CTIMER0     0xF0438
#define EPIPHANY_PER_CORE_REG_CTIMER1     0xF043C

/* Processor control registers */
#define EPIPHANY_PER_CORE_REG_MEMSTATUS   0xF0604
#define EPIPHANY_PER_CORE_REG_MEMPROTECT  0xF0608

/* DMA Registers */
#define EPIPHANY_PER_CORE_REG_DMA0CONFIG  0xF0500
#define EPIPHANY_PER_CORE_REG_DMA0STRIDE  0xF0504
#define EPIPHANY_PER_CORE_REG_DMA0COUNT   0xF0508
#define EPIPHANY_PER_CORE_REG_DMA0SRCADDR 0xF050C
#define EPIPHANY_PER_CORE_REG_DMA0DSTADDR 0xF0510
#define EPIPHANY_PER_CORE_REG_DMA0AUTO0   0xF0514
#define EPIPHANY_PER_CORE_REG_DMA0AUTO1   0xF0518
#define EPIPHANY_PER_CORE_REG_DMA0STATUS  0xF051C
#define EPIPHANY_PER_CORE_REG_DMA1CONFIG  0xF0520
#define EPIPHANY_PER_CORE_REG_DMA1STRIDE  0xF0524
#define EPIPHANY_PER_CORE_REG_DMA1COUNT   0xF0528
#define EPIPHANY_PER_CORE_REG_DMA1SRCADDR 0xF052C
#define EPIPHANY_PER_CORE_REG_DMA1DSTADDR 0xF0530
#define EPIPHANY_PER_CORE_REG_DMA1AUTO0   0xF0534
#define EPIPHANY_PER_CORE_REG_DMA1AUTO1   0xF0538
#define EPIPHANY_PER_CORE_REG_DMA1STATUS  0xF053C

/* Mesh Node Control Registers */
#define EPIPHANY_PER_CORE_REG_MESHCONFIG  0xF0700
#define EPIPHANY_PER_CORE_REG_COREID      0xF0704
#define EPIPHANY_PER_CORE_REG_MULTICAST   0xF0708
#define EPIPHANY_PER_CORE_REG_CMESHROUTE  0xF0710
#define EPIPHANY_PER_CORE_REG_XMESHROUTE  0xF0714
#define EPIPHANY_PER_CORE_REG_RMESHROUTE  0xF0718

/*  This macros constructs an address space of epiphany cores
 *  from their IDs.
 */
#define EPIPHANY_COREID_TO_MSB_ADDR(id) (id) << 20

/*  Construct a complete/absolute IO mapped address register from
 *  core ID and register name
 */
#define EPIPHANY_GET_REG_ABSOLUTE_ADDR(coreid, reg) \
        (EPIPHANY_COREID_TO_MSB_ADDR(coreid) | (reg))

#define EPIPHANY_REG(reg) (uint32_t *) (reg)

/* Read register with its absolute address */
static inline uint32_t read_epiphany_reg(volatile uint32_t reg_addr)
{
  return *(EPIPHANY_REG(reg_addr));
}

/* Write register with its abolute address */
static inline void write_epiphany_reg(volatile uint32_t reg_addr, uint32_t val)
{
  *(EPIPHANY_REG(reg_addr)) = val;
}

/*  Epiphany uses 12 bits for defining core IDs, while RTEMS uses
 *  linear IDs. The following function converts RTEMS linear IDs to
 *  Epiphany corresponding ones
 */
static const uint32_t map[16] =
{
   0x808, 0x809, 0x80A, 0x80B,
   0x848, 0x849, 0x84A, 0x84B,
   0x888, 0x889, 0x88A, 0x88B,
   0x8C8, 0x8C9, 0x8CA, 0x8CB
};

static inline uint32_t rtems_coreid_to_epiphany_map(uint32_t rtems_id)
{
  return map[rtems_id];
}

/* Epiphany uses 12 bits for defining core IDs, while RTEMS uses
 * linear IDs. The following function is used to map Epiphany IDs to
 * RTEMS linear IDs.
 */
static inline uint32_t epiphany_coreid_to_rtems_map(uint32_t epiphany_id)
{
  register uint32_t coreid asm ("r17") = epiphany_id;

  /* Mapping from Epiphany IDs to 0-16 IDs macro */
  __asm__ __volatile__(" \
   movfs r17, coreid \
   mov r19, #0x003   \
   mov r20, #0x0F0   \
   and r19, r17, r19 \
   and r20, r17, r20 \
   lsr r20, r20, #4  \
   add r17, r19, r20 \
   ");

  /* coreid or r17 now holds the rtems core id */
  return coreid;
}

static inline uint32_t _Epiphany_Get_current_processor(void)
{
  uint32_t coreid;

  asm volatile ("movfs %0, coreid" : "=r" (coreid): );

  return epiphany_coreid_to_rtems_map(coreid);
}
#endif  /* _EPIPHANY_UTILITY_H */
