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
 */

#ifndef RTEMS_SCORE_ARMV7M_H
#define RTEMS_SCORE_ARMV7M_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  uint32_t reserved_0;
  uint32_t ictr;
  uint32_t actlr;
  uint32_t reserved_1;
} ARMV7M_ICTAC;

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
#define ARMV7M_SCB_ICSR_VECTPENDING_GET(reg) (((reg) >> 12) & 0x1ffU)
#define ARMV7M_SCB_ICSR_RETTOBASE (1U << 11)
#define ARMV7M_SCB_ICSR_VECTACTIVE_GET(reg) ((reg) & 0x1ffU)
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

typedef struct {
#define ARMV7M_MPU_TYPE_IREGION_GET(reg) (((reg) >> 16) & 0xffU)
#define ARMV7M_MPU_TYPE_DREGION_GET(reg) (((reg) >> 8) & 0xffU)
#define ARMV7M_MPU_TYPE_SEPARATE (1U << 0)
  uint32_t type;

#define ARMV7M_MPU_CTRL_PRIVDEFENA (1U << 2)
#define ARMV7M_MPU_CTRL_HFNMIENA (1U << 1)
#define ARMV7M_MPU_CTRL_ENABLE (1U << 0)
  uint32_t ctrl;

  uint32_t rnr;

#define ARMV7M_MPU_RBAR_ADDR_SHIFT 5
#define ARMV7M_MPU_RBAR_ADDR_MASK \
  ((0x7ffffffU) << ARMV7M_MPU_RBAR_ADDR_SHIFT)
#define ARMV7M_MPU_RBAR_ADDR(val) \
  (((val) << ARMV7M_MPU_RBAR_ADDR_SHIFT) & ARMV7M_MPU_RBAR_ADDR_MASK)
#define ARMV7M_MPU_RBAR_ADDR_GET(reg) \
  (((val) & ARMV7M_MPU_RBAR_ADDR_MASK) >> ARMV7M_MPU_RBAR_ADDR_SHIFT)
#define ARMV7M_MPU_RBAR_ADDR_SET(reg, val) \
  (((reg) & ~ARMV7M_MPU_RBAR_ADDR_MASK) | ARMV7M_MPU_RBAR_ADDR(val))
#define ARMV7M_MPU_RBAR_VALID (1U << 4)
#define ARMV7M_MPU_RBAR_REGION_SHIFT 0
#define ARMV7M_MPU_RBAR_REGION_MASK \
  ((0xfU) << ARMV7M_MPU_RBAR_REGION_SHIFT)
#define ARMV7M_MPU_RBAR_REGION(val) \
  (((val) << ARMV7M_MPU_RBAR_REGION_SHIFT) & ARMV7M_MPU_RBAR_REGION_MASK)
#define ARMV7M_MPU_RBAR_REGION_GET(reg) \
  (((val) & ARMV7M_MPU_RBAR_REGION_MASK) >> ARMV7M_MPU_RBAR_REGION_SHIFT)
#define ARMV7M_MPU_RBAR_REGION_SET(reg, val) \
  (((reg) & ~ARMV7M_MPU_RBAR_REGION_MASK) | ARMV7M_MPU_RBAR_REGION(val))
  uint32_t rbar;

#define ARMV7M_MPU_RASR_XN (1U << 28)
#define ARMV7M_MPU_RASR_AP_SHIFT 24
#define ARMV7M_MPU_RASR_AP_MASK \
  ((0x7U) << ARMV7M_MPU_RASR_AP_SHIFT)
#define ARMV7M_MPU_RASR_AP(val) \
  (((val) << ARMV7M_MPU_RASR_AP_SHIFT) & ARMV7M_MPU_RASR_AP_MASK)
#define ARMV7M_MPU_RASR_AP_GET(reg) \
  (((val) & ARMV7M_MPU_RASR_AP_MASK) >> ARMV7M_MPU_RASR_AP_SHIFT)
#define ARMV7M_MPU_RASR_AP_SET(reg, val) \
  (((reg) & ~ARMV7M_MPU_RASR_AP_MASK) | ARMV7M_MPU_RASR_AP(val))
#define ARMV7M_MPU_RASR_TEX_SHIFT 19
#define ARMV7M_MPU_RASR_TEX_MASK \
  ((0x7U) << ARMV7M_MPU_RASR_TEX_SHIFT)
#define ARMV7M_MPU_RASR_TEX(val) \
  (((val) << ARMV7M_MPU_RASR_TEX_SHIFT) & ARMV7M_MPU_RASR_TEX_MASK)
#define ARMV7M_MPU_RASR_TEX_GET(reg) \
  (((val) & ARMV7M_MPU_RASR_TEX_MASK) >> ARMV7M_MPU_RASR_TEX_SHIFT)
#define ARMV7M_MPU_RASR_TEX_SET(reg, val) \
  (((reg) & ~ARMV7M_MPU_RASR_TEX_MASK) | ARMV7M_MPU_RASR_TEX(val))
#define ARMV7M_MPU_RASR_S (1U << 18)
#define ARMV7M_MPU_RASR_C (1U << 17)
#define ARMV7M_MPU_RASR_B (1U << 16)
#define ARMV7M_MPU_RASR_SRD_SHIFT 8
#define ARMV7M_MPU_RASR_SRD_MASK \
  ((0xffU) << ARMV7M_MPU_RASR_SRD_SHIFT)
#define ARMV7M_MPU_RASR_SRD(val) \
  (((val) << ARMV7M_MPU_RASR_SRD_SHIFT) & ARMV7M_MPU_RASR_SRD_MASK)
#define ARMV7M_MPU_RASR_SRD_GET(reg) \
  (((val) & ARMV7M_MPU_RASR_SRD_MASK) >> ARMV7M_MPU_RASR_SRD_SHIFT)
#define ARMV7M_MPU_RASR_SRD_SET(reg, val) \
  (((reg) & ~ARMV7M_MPU_RASR_SRD_MASK) | ARMV7M_MPU_RASR_SRD(val))
#define ARMV7M_MPU_RASR_SIZE_SHIFT 1
#define ARMV7M_MPU_RASR_SIZE_MASK \
  ((0x1fU) << ARMV7M_MPU_RASR_SIZE_SHIFT)
#define ARMV7M_MPU_RASR_SIZE(val) \
  (((val) << ARMV7M_MPU_RASR_SIZE_SHIFT) & ARMV7M_MPU_RASR_SIZE_MASK)
#define ARMV7M_MPU_RASR_SIZE_GET(reg) \
  (((val) & ARMV7M_MPU_RASR_SIZE_MASK) >> ARMV7M_MPU_RASR_SIZE_SHIFT)
#define ARMV7M_MPU_RASR_SIZE_SET(reg, val) \
  (((reg) & ~ARMV7M_MPU_RASR_SIZE_MASK) | ARMV7M_MPU_RASR_SIZE(val))
#define ARMV7M_MPU_RASR_ENABLE (1U << 0)
  uint32_t rasr;

  uint32_t rbar_a1;
  uint32_t rasr_a1;
  uint32_t rbar_a2;
  uint32_t rasr_a2;
  uint32_t rbar_a3;
  uint32_t rasr_a3;
} ARMV7M_MPU;

typedef enum {
  ARMV7M_MPU_AP_PRIV_NO_USER_NO,
  ARMV7M_MPU_AP_PRIV_RW_USER_NO,
  ARMV7M_MPU_AP_PRIV_RW_USER_RO,
  ARMV7M_MPU_AP_PRIV_RW_USER_RW,
  ARMV7M_MPU_AP_PRIV_RO_USER_NO = 0x5,
  ARMV7M_MPU_AP_PRIV_RO_USER_RO,
} ARMV7M_MPU_Access_permissions;

typedef enum {
  ARMV7M_MPU_ATTR_R = ARMV7M_MPU_RASR_AP(ARMV7M_MPU_AP_PRIV_RO_USER_NO)
    | ARMV7M_MPU_RASR_C | ARMV7M_MPU_RASR_XN,
  ARMV7M_MPU_ATTR_RW = ARMV7M_MPU_RASR_AP(ARMV7M_MPU_AP_PRIV_RW_USER_NO)
    | ARMV7M_MPU_RASR_C | ARMV7M_MPU_RASR_XN | ARMV7M_MPU_RASR_B,
  ARMV7M_MPU_ATTR_RWX = ARMV7M_MPU_RASR_AP(ARMV7M_MPU_AP_PRIV_RW_USER_NO)
    | ARMV7M_MPU_RASR_C | ARMV7M_MPU_RASR_B,
  ARMV7M_MPU_ATTR_X = ARMV7M_MPU_RASR_AP(ARMV7M_MPU_AP_PRIV_NO_USER_NO)
    | ARMV7M_MPU_RASR_C,
  ARMV7M_MPU_ATTR_RX = ARMV7M_MPU_RASR_AP(ARMV7M_MPU_AP_PRIV_RO_USER_NO)
    | ARMV7M_MPU_RASR_C,
  ARMV7M_MPU_ATTR_IO = ARMV7M_MPU_RASR_AP(ARMV7M_MPU_AP_PRIV_RW_USER_NO)
    | ARMV7M_MPU_RASR_XN,
} ARMV7M_MPU_Attributes;

typedef enum {
  ARMV7M_MPU_SIZE_32_B = 0x4,
  ARMV7M_MPU_SIZE_64_B,
  ARMV7M_MPU_SIZE_128_B,
  ARMV7M_MPU_SIZE_256_B,
  ARMV7M_MPU_SIZE_512_B,
  ARMV7M_MPU_SIZE_1_KB,
  ARMV7M_MPU_SIZE_2_KB,
  ARMV7M_MPU_SIZE_4_KB,
  ARMV7M_MPU_SIZE_8_KB,
  ARMV7M_MPU_SIZE_16_KB,
  ARMV7M_MPU_SIZE_32_KB,
  ARMV7M_MPU_SIZE_64_KB,
  ARMV7M_MPU_SIZE_128_KB,
  ARMV7M_MPU_SIZE_256_KB,
  ARMV7M_MPU_SIZE_512_KB,
  ARMV7M_MPU_SIZE_1_MB,
  ARMV7M_MPU_SIZE_2_MB,
  ARMV7M_MPU_SIZE_4_MB,
  ARMV7M_MPU_SIZE_8_MB,
  ARMV7M_MPU_SIZE_16_MB,
  ARMV7M_MPU_SIZE_32_MB,
  ARMV7M_MPU_SIZE_64_MB,
  ARMV7M_MPU_SIZE_128_MB,
  ARMV7M_MPU_SIZE_256_MB,
  ARMV7M_MPU_SIZE_512_MB,
  ARMV7M_MPU_SIZE_1_GB,
  ARMV7M_MPU_SIZE_2_GB,
  ARMV7M_MPU_SIZE_4_GB
} ARMV7M_MPU_Size;

typedef struct {
  uint32_t rbar;
  uint32_t rasr;
} ARMV7M_MPU_Region;

#define ARMV7M_MPU_REGION_INITIALIZER(idx, addr, size, attr) \
  { \
    ((addr) & ARMV7M_MPU_RBAR_ADDR_MASK) \
      | ARMV7M_MPU_RBAR_VALID \
      | ARMV7M_MPU_RBAR_REGION(idx), \
    ARMV7M_MPU_RASR_SIZE(size) | (attr) | ARMV7M_MPU_RASR_ENABLE \
  }

#define ARMV7M_MPU_REGION_DISABLED_INITIALIZER(idx) \
  { \
    ARMV7M_MPU_RBAR_VALID | ARMV7M_MPU_RBAR_REGION(idx), \
    0 \
  }

#define ARMV7M_SCS_BASE 0xe000e000
#define ARMV7M_ICTAC_BASE (ARMV7M_SCS_BASE + 0x0)
#define ARMV7M_SYSTICK_BASE (ARMV7M_SCS_BASE + 0x10)
#define ARMV7M_NVIC_BASE (ARMV7M_SCS_BASE + 0x100)
#define ARMV7M_SCB_BASE (ARMV7M_SCS_BASE + 0xd00)
#define ARMV7M_MPU_BASE (ARMV7M_SCS_BASE + 0xd90)

#define _ARMV7M_ICTAC \
  ((volatile ARMV7M_ICTAC *) ARMV7M_ICTAC_BASE)
#define _ARMV7M_SCB \
  ((volatile ARMV7M_SCB *) ARMV7M_SCB_BASE)
#define _ARMV7M_Systick \
  ((volatile ARMV7M_Systick *) ARMV7M_SYSTICK_BASE)
#define _ARMV7M_NVIC \
  ((volatile ARMV7M_NVIC *) ARMV7M_NVIC_BASE)
#define _ARMV7M_MPU \
  ((volatile ARMV7M_MPU *) ARMV7M_MPU_BASE)

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
#define ARMV7M_VECTOR_IRQ(n) ((n) + 16)
#define ARMV7M_IRQ_OF_VECTOR(n) ((n) - 16)

#define ARMV7M_EXCEPTION_PRIORITY_LOWEST 255

static inline bool _ARMV7M_Is_vector_an_irq( int vector )
{
  return vector >= 16;
}

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

static inline bool _ARMV7M_NVIC_Is_enabled( int irq )
{
  int index = irq >> 5;
  uint32_t bit = 1U << (irq & 0x1f);

  return (_ARMV7M_NVIC->iser [index] & bit) != 0;
}

static inline void _ARMV7M_NVIC_Set_enable( int irq )
{
  int index = irq >> 5;
  uint32_t bit = 1U << (irq & 0x1f);

  _ARMV7M_NVIC->iser [index] = bit;
}

static inline void _ARMV7M_NVIC_Clear_enable( int irq )
{
  int index = irq >> 5;
  uint32_t bit = 1U << (irq & 0x1f);

  _ARMV7M_NVIC->icer [index] = bit;
}

static inline bool _ARMV7M_NVIC_Is_pending( int irq )
{
  int index = irq >> 5;
  uint32_t bit = 1U << (irq & 0x1f);

  return (_ARMV7M_NVIC->ispr [index] & bit) != 0;
}

static inline void _ARMV7M_NVIC_Set_pending( int irq )
{
  int index = irq >> 5;
  uint32_t bit = 1U << (irq & 0x1f);

  _ARMV7M_NVIC->ispr [index] = bit;
}

static inline void _ARMV7M_NVIC_Clear_pending( int irq )
{
  int index = irq >> 5;
  uint32_t bit = 1U << (irq & 0x1f);

  _ARMV7M_NVIC->icpr [index] = bit;
}

static inline bool _ARMV7M_NVIC_Is_active( int irq )
{
  int index = irq >> 5;
  uint32_t bit = 1U << (irq & 0x1f);

  return (_ARMV7M_NVIC->iabr [index] & bit) != 0;
}

static inline void _ARMV7M_NVIC_Set_priority( int irq, int priority )
{
  _ARMV7M_NVIC->ipr [irq] = (uint8_t) priority;
}

static inline int _ARMV7M_NVIC_Get_priority( int irq )
{
  return _ARMV7M_NVIC->ipr [irq];
}

int _ARMV7M_Get_exception_priority( int vector );

void _ARMV7M_Set_exception_priority( int vector, int priority );

ARMV7M_Exception_handler _ARMV7M_Get_exception_handler( int index );

void _ARMV7M_Set_exception_handler(
  int index,
  ARMV7M_Exception_handler handler
);

void _ARMV7M_Set_exception_priority_and_handler(
  int index,
  int priority,
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
