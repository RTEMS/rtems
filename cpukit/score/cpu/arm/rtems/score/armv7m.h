/*
 * Copyright (c) 2011 Sebastian Huber.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#ifndef RTEMS_SCORE_ARMV7M_H
#define RTEMS_SCORE_ARMV7M_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  uint32_t reserved_0;
  uint32_t ictr;
  uint32_t actlr;
  uint32_t reserved_1;
} ARMV7M_Interrupt_type;

typedef void (*ARMV7M_Exception_handler)(void);

typedef struct {
  uint32_t register_r0;
  uint32_t register_r1;
  uint32_t register_r2;
  uint32_t register_r3;
  uint32_t register_r12;
  void *register_lr;
  void *register_pc;
  uint32_t register_xpsr;
} ARMV7M_Exception_frame;

typedef struct {
  uint32_t cpuid;

#define ARMV7M_SCB_ICSR_NMIPENDSET (1U << 31)
#define ARMV7M_SCB_ICSR_PENDSVSET (1U << 28)
#define ARMV7M_SCB_ICSR_PENDSVCLR (1U << 27)
#define ARMV7M_SCB_ICSR_PENDSTSET (1U << 26)
#define ARMV7M_SCB_ICSR_PENDSTCLR (1U << 25)
#define ARMV7M_SCB_ICSR_ISRPREEMPT (1U << 23)
#define ARMV7M_SCB_ICSR_ISRPENDING (1U << 22)
#define ARMV7M_SCB_ICSR_VECTPENDING(reg) (((reg) >> 12) & 0x1ffU)
#define ARMV7M_SCB_ICSR_RETTOBASE (1U << 11)
#define ARMV7M_SCB_ICSR_VECTACTIVE(reg) ((reg) & 0x1ffU)
  uint32_t icsr;

  ARMV7M_Exception_handler *vtor;
  uint32_t aircr;
  uint32_t scr;
  uint32_t ccr;
  uint8_t shpr [12];
  uint32_t shcsr;
  uint32_t cfsr;
  uint32_t hfsr;
  uint32_t dfsr;
  uint32_t mmfar;
  uint32_t bfar;
  uint32_t afsr;
} ARMV7M_SCB;

typedef struct {
#define ARMV7M_SYSTICK_CSR_COUNTFLAG (1U << 16)
#define ARMV7M_SYSTICK_CSR_CLKSOURCE (1U << 2)
#define ARMV7M_SYSTICK_CSR_TICKINT (1U << 1)
#define ARMV7M_SYSTICK_CSR_ENABLE (1U << 0)
  uint32_t csr;

  uint32_t rvr;
  uint32_t cvr;

#define ARMV7M_SYSTICK_CALIB_NOREF (1U << 31)
#define ARMV7M_SYSTICK_CALIB_SKEW (1U << 30)
#define ARMV7M_SYSTICK_CALIB_TENMS_GET(reg) ((reg) & 0xffffffU)
  uint32_t calib;
} ARMV7M_Systick;

typedef struct {
  uint32_t iser [8];
  uint32_t reserved_0 [24];
  uint32_t icer [8];
  uint32_t reserved_1 [24];
  uint32_t ispr [8];
  uint32_t reserved_2 [24];
  uint32_t icpr [8];
  uint32_t reserved_3 [24];
  uint32_t iabr [8];
  uint32_t reserved_4 [56];
  uint8_t  ipr [240];
  uint32_t reserved_5 [644];
  uint32_t stir;
} ARMV7M_NVIC;

#define ARMV7M_SCS_BASE 0xe000e000
#define ARMV7M_SYSTICK_BASE (ARMV7M_SCS_BASE + 0x10)
#define ARMV7M_NVIC_BASE (ARMV7M_SCS_BASE + 0x100)
#define ARMV7M_SCB_BASE (ARMV7M_SCS_BASE + 0xd00)

#define _ARMV7M_Interrupt_type \
  ((volatile ARMV7M_Interrupt_type *) ARMV7M_SCS_BASE)
#define _ARMV7M_SCB \
  ((volatile ARMV7M_SCB *) ARMV7M_SCB_BASE)
#define _ARMV7M_Systick \
  ((volatile ARMV7M_Systick *) ARMV7M_SYSTICK_BASE)
#define _ARMV7M_NVIC \
  ((volatile ARMV7M_NVIC *) ARMV7M_NVIC_BASE)

#define ARMV7M_VECTOR_MSP 0
#define ARMV7M_VECTOR_RESET 1
#define ARMV7M_VECTOR_NMI 2
#define ARMV7M_VECTOR_HARD_FAULT 3
#define ARMV7M_VECTOR_MEM_MANAGE 4
#define ARMV7M_VECTOR_BUS_FAULT 5
#define ARMV7M_VECTOR_USAGE_FAULT 6
#define ARMV7M_VECTOR_SVC 11
#define ARMV7M_VECTOR_DEBUG_MONITOR 12
#define ARMV7M_VECTOR_PENDSV 14
#define ARMV7M_VECTOR_SYSTICK 15
#define ARMV7M_VECTOR_IRQ(n) (16 + (n))

static inline uint32_t _ARMV7M_Get_basepri(void)
{
  uint32_t val;
  __asm__ volatile ("mrs %[val], basepri\n" : [val] "=&r" (val));
  return val;
}

static inline void _ARMV7M_Set_basepri(uint32_t val)
{
  __asm__ volatile ("msr basepri, %[val]\n" : : [val] "r" (val));
}

static inline uint32_t _ARMV7M_Get_primask(void)
{
  uint32_t val;
  __asm__ volatile ("mrs %[val], primask\n" : [val] "=&r" (val));
  return val;
}

static inline void _ARMV7M_Set_primask(uint32_t val)
{
  __asm__ volatile ("msr primask, %[val]\n" : : [val] "r" (val));
}

static inline uint32_t _ARMV7M_Get_faultmask(void)
{
  uint32_t val;
  __asm__ volatile ("mrs %[val], faultmask\n" : [val] "=&r" (val));
  return val;
}

static inline void _ARMV7M_Set_faultmask(uint32_t val)
{
  __asm__ volatile ("msr faultmask, %[val]\n" : : [val] "r" (val));
}

static inline uint32_t _ARMV7M_Get_control(void)
{
  uint32_t val;
  __asm__ volatile ("mrs %[val], control\n" : [val] "=&r" (val));
  return val;
}

static inline void _ARMV7M_Set_control(uint32_t val)
{
  __asm__ volatile ("msr control, %[val]\n" : : [val] "r" (val));
}

static inline uint32_t _ARMV7M_Get_MSP(void)
{
  uint32_t val;
  __asm__ volatile ("mrs %[val], msp\n" : [val] "=&r" (val));
  return val;
}

static inline void _ARMV7M_Set_MSP(uint32_t val)
{
  __asm__ volatile ("msr msp, %[val]\n" : : [val] "r" (val));
}

static inline uint32_t _ARMV7M_Get_PSP(void)
{
  uint32_t val;
  __asm__ volatile ("mrs %[val], psp\n" : [val] "=&r" (val));
  return val;
}

static inline void _ARMV7M_Set_PSP(uint32_t val)
{
  __asm__ volatile ("msr psp, %[val]\n" : : [val] "r" (val));
}

static inline uint32_t _ARMV7M_Get_XPSR(void)
{
  uint32_t val;
  __asm__ volatile ("mrs %[val], xpsr\n" : [val] "=&r" (val));
  return val;
}

int _ARMV7M_Get_exception_priority( int vector );

void _ARMV7M_Set_exception_priority( int vector, int priority );

ARMV7M_Exception_handler _ARMV7M_Get_exception_handler( int index );

void _ARMV7M_Set_exception_handler(
  int index,
  ARMV7M_Exception_handler handler
);

void _ARMV7M_Interrupt_service_enter( void );

void _ARMV7M_Interrupt_service_leave( void );

void _ARMV7M_Pendable_service_call( void );

void _ARMV7M_Supervisor_call( void );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTEMS_SCORE_ARMV7M_H */
