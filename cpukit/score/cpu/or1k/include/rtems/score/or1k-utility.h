/**
 * @file
 *
 * @brief OR1K utility
 */
/*
 * COPYRIGHT (c) 2014 Hesham ALMatary <heshamelmatary@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_OR1K_UTILITY_H
#define _RTEMS_SCORE_OR1K_UTILITY_H

/* SPR groups definitions */
#define SPR_GRP_SHAMT 11
#define SPR_GRP0_SYS_CTRL  (0  << SPR_GRP_SHAMT)
#define SPR_GRP1_DMMU      (1  << SPR_GRP_SHAMT)
#define SPR_GRP2_IMMU      (2  << SPR_GRP_SHAMT)
#define SPR_GRP3_DC        (3  << SPR_GRP_SHAMT)
#define SPR_GRP4_IC        (4  << SPR_GRP_SHAMT)
#define SPR_GRP5_MAC       (5  << SPR_GRP_SHAMT)
#define SPR_GRP6_DEBUG     (6  << SPR_GRP_SHAMT)
#define SPR_GRP7_PERF_CTR  (7  << SPR_GRP_SHAMT)
#define SPR_GRP8_PWR_MNG   (8  << SPR_GRP_SHAMT)
#define SPR_GRP9_PIC       (9  << SPR_GRP_SHAMT)
#define SPR_GPR10_TICK_TMR (10 << SPR_GRP_SHAMT)
#define SPR_GPR11_FPU      (11 << SPR_GRP_SHAMT)

/* SPR registers definitions */

/* Group 0: System control registers */
#define CPU_OR1K_SPR_VR       (SPR_GRP0_SYS_CTRL + 0)
#define CPU_OR1K_SPR_UPR      (SPR_GRP0_SYS_CTRL + 1)
#define CPU_OR1K_SPR_CPUCFGR  (SPR_GRP0_SYS_CTRL + 2)
#define CPU_OR1K_SPR_DMMUCFGR (SPR_GRP0_SYS_CTRL + 3)
#define CPU_OR1K_SPR_IMMUCFGR (SPR_GRP0_SYS_CTRL + 4)
#define CPU_OR1K_SPR_DCCFGR   (SPR_GRP0_SYS_CTRL + 5)
#define CPU_OR1K_SPR_ICCFGR   (SPR_GRP0_SYS_CTRL + 6)
#define CPU_OR1K_SPR_DCFGR    (SPR_GRP0_SYS_CTRL + 7)
#define CPU_OR1K_SPR_PCCFGR   (SPR_GRP0_SYS_CTRL + 8)
#define CPU_OR1K_SPR_VR2      (SPR_GRP0_SYS_CTRL + 9)
#define CPU_OR1K_SPR_AVR      (SPR_GRP0_SYS_CTRL + 10)
#define CPU_OR1K_SPR_EVBAR    (SPR_GRP0_SYS_CTRL + 11)
#define CPU_OR1K_SPR_AECR     (SPR_GRP0_SYS_CTRL + 12)
#define CPU_OR1K_SPR_AESR     (SPR_GRP0_SYS_CTRL + 13)
#define CPU_OR1K_SPR_NPC      (SPR_GRP0_SYS_CTRL + 16)
#define CPU_OR1K_SPR_SR       (SPR_GRP0_SYS_CTRL + 17)
#define CPU_OR1K_SPR_PPC      (SPR_GRP0_SYS_CTRL + 18)
#define CPU_OR1K_SPR_FPCSR    (SPR_GRP0_SYS_CTRL + 20)
#define CPU_OR1K_SPR_EPCR0    (SPR_GRP0_SYS_CTRL + 32)
#define CPU_OR1K_SPR_EPCR1    (SPR_GRP0_SYS_CTRL + 33)
#define CPU_OR1K_SPR_EPCR2    (SPR_GRP0_SYS_CTRL + 34)
#define CPU_OR1K_SPR_EPCR3    (SPR_GRP0_SYS_CTRL + 35)
#define CPU_OR1K_SPR_EPCR4    (SPR_GRP0_SYS_CTRL + 36)
#define CPU_OR1K_SPR_EPCR5    (SPR_GRP0_SYS_CTRL + 37)
#define CPU_OR1K_SPR_EPCR6    (SPR_GRP0_SYS_CTRL + 38)
#define CPU_OR1K_SPR_EPCR7    (SPR_GRP0_SYS_CTRL + 39)
#define CPU_OR1K_SPR_EPCR8    (SPR_GRP0_SYS_CTRL + 40)
#define CPU_OR1K_SPR_EPCR9    (SPR_GRP0_SYS_CTRL + 41)
#define CPU_OR1K_SPR_EPCR10   (SPR_GRP0_SYS_CTRL + 42)
#define CPU_OR1K_SPR_EPCR11   (SPR_GRP0_SYS_CTRL + 43)
#define CPU_OR1K_SPR_EPCR12   (SPR_GRP0_SYS_CTRL + 44)
#define CPU_OR1K_SPR_EPCR13   (SPR_GRP0_SYS_CTRL + 45)
#define CPU_OR1K_SPR_EPCR14   (SPR_GRP0_SYS_CTRL + 46)
#define CPU_OR1K_SPR_EPCR15   (SPR_GRP0_SYS_CTRL + 47)
#define CPU_OR1K_SPR_EEAR0    (SPR_GRP0_SYS_CTRL + 48)
#define CPU_OR1K_SPR_EEAR1    (SPR_GRP0_SYS_CTRL + 49)
#define CPU_OR1K_SPR_EEAR2    (SPR_GRP0_SYS_CTRL + 50)
#define CPU_OR1K_SPR_EEAR3    (SPR_GRP0_SYS_CTRL + 51)
#define CPU_OR1K_SPR_EEAR4    (SPR_GRP0_SYS_CTRL + 52)
#define CPU_OR1K_SPR_EEAR5    (SPR_GRP0_SYS_CTRL + 53)
#define CPU_OR1K_SPR_EEAR6    (SPR_GRP0_SYS_CTRL + 54)
#define CPU_OR1K_SPR_EEAR7    (SPR_GRP0_SYS_CTRL + 55)
#define CPU_OR1K_SPR_EEAR8    (SPR_GRP0_SYS_CTRL + 56)
#define CPU_OR1K_SPR_EEAR9    (SPR_GRP0_SYS_CTRL + 57)
#define CPU_OR1K_SPR_EEAR10   (SPR_GRP0_SYS_CTRL + 58)
#define CPU_OR1K_SPR_EEAR11   (SPR_GRP0_SYS_CTRL + 59)
#define CPU_OR1K_SPR_EEAR12   (SPR_GRP0_SYS_CTRL + 60)
#define CPU_OR1K_SPR_EEAR13   (SPR_GRP0_SYS_CTRL + 61)
#define CPU_OR1K_SPR_EEAR14   (SPR_GRP0_SYS_CTRL + 62)
#define CPU_OR1K_SPR_EEAR15   (SPR_GRP0_SYS_CTRL + 63)
#define CPU_OR1K_SPR_ESR0     (SPR_GRP0_SYS_CTRL + 64)
#define CPU_OR1K_SPR_ESR1     (SPR_GRP0_SYS_CTRL + 65)
#define CPU_OR1K_SPR_ESR2     (SPR_GRP0_SYS_CTRL + 66)
#define CPU_OR1K_SPR_ESR3     (SPR_GRP0_SYS_CTRL + 67)
#define CPU_OR1K_SPR_ESR4     (SPR_GRP0_SYS_CTRL + 68)
#define CPU_OR1K_SPR_ESR5     (SPR_GRP0_SYS_CTRL + 69)
#define CPU_OR1K_SPR_ESR6     (SPR_GRP0_SYS_CTRL + 70)
#define CPU_OR1K_SPR_ESR7     (SPR_GRP0_SYS_CTRL + 71)
#define CPU_OR1K_SPR_ESR8     (SPR_GRP0_SYS_CTRL + 72)
#define CPU_OR1K_SPR_ESR9     (SPR_GRP0_SYS_CTRL + 73)
#define CPU_OR1K_SPR_ESR10    (SPR_GRP0_SYS_CTRL + 74)
#define CPU_OR1K_SPR_ESR11    (SPR_GRP0_SYS_CTRL + 75)
#define CPU_OR1K_SPR_ESR12    (SPR_GRP0_SYS_CTRL + 76)
#define CPU_OR1K_SPR_ESR13    (SPR_GRP0_SYS_CTRL + 77)
#define CPU_OR1K_SPR_ESR14    (SPR_GRP0_SYS_CTRL + 78)
#define CPU_OR1K_SPR_ESR15    (SPR_GRP0_SYS_CTRL + 79)

/* Shadow registers base */
#define CPU_OR1K_SPR_GPR32    (SPR_GRP0_SYS_CTRL + 1024)

/* Group1: Data MMU registers */
#define CPU_OR1K_SPR_DMMUCR   (SPR_GRP1_DMMU + 0)
#define CPU_OR1K_SPR_DMMUPR   (SPR_GRP1_DMMU + 1)
#define CPU_OR1K_SPR_DTLBEIR  (SPR_GRP1_DMMU + 2)
#define CPU_OR1K_SPR_DATBMR0  (SPR_GRP1_DMMU + 4)
#define CPU_OR1K_SPR_DATBMR1  (SPR_GRP1_DMMU + 5)
#define CPU_OR1K_SPR_DATBMR2  (SPR_GRP1_DMMU + 6)
#define CPU_OR1K_SPR_DATBMR3  (SPR_GRP1_DMMU + 7)
#define CPU_OR1K_SPR_DATBTR0  (SPR_GRP1_DMMU + 8)
#define CPU_OR1K_SPR_DATBTR1  (SPR_GRP1_DMMU + 9)
#define CPU_OR1K_SPR_DATBTR2  (SPR_GRP1_DMMU + 10)
#define CPU_OR1K_SPR_DATBTR3  (SPR_GRP1_DMMU + 11)

/* Group2: Instruction MMU registers */
#define CPU_OR1K_SPR_IMMUCR   (SPR_GRP2_IMMU + 0)
#define CPU_OR1K_SPR_IMMUPR   (SPR_GRP2_IMMU + 1)
#define CPU_OR1K_SPR_ITLBEIR  (SPR_GRP2_IMMU + 2)
#define CPU_OR1K_SPR_IATBMR0  (SPR_GRP2_IMMU + 4)
#define CPU_OR1K_SPR_IATBMR1  (SPR_GRP2_IMMU + 5)
#define CPU_OR1K_SPR_IATBMR2  (SPR_GRP2_IMMU + 6)
#define CPU_OR1K_SPR_IATBMR3  (SPR_GRP2_IMMU + 7)
#define CPU_OR1K_SPR_IATBTR0  (SPR_GRP2_IMMU + 8)
#define CPU_OR1K_SPR_IATBTR1  (SPR_GRP2_IMMU + 9)
#define CPU_OR1K_SPR_IATBTR2  (SPR_GRP2_IMMU + 10)
#define CPU_OR1K_SPR_IATBTR3  (SPR_GRP2_IMMU + 11)

/* Group3: Data Cache registers */
#define CPU_OR1K_SPR_DCCR   (SPR_GRP3_DC + 0)
#define CPU_OR1K_SPR_DCBPR  (SPR_GRP3_DC + 1)
#define CPU_OR1K_SPR_DCBFR  (SPR_GRP3_DC + 2)
#define CPU_OR1K_SPR_DCBIR  (SPR_GRP3_DC + 3)
#define CPU_OR1K_SPR_DCBWR  (SPR_GRP3_DC + 4)
#define CPU_OR1K_SPR_DCBLR  (SPR_GRP3_DC + 5)

/* Group4: Instruction Cache registers */
#define CPU_OR1K_SPR_ICCR   (SPR_GRP4_IC + 0)
#define CPU_OR1K_SPR_ICBPR  (SPR_GRP4_IC + 1)
#define CPU_OR1K_SPR_ICBIR  (SPR_GRP4_IC + 2)
#define CPU_OR1K_SPR_ICBLR  (SPR_GRP4_IC + 3)

/* Group5: MAC registers */
#define CPU_OR1K_SPR_MACLO  (SPR_GRP5_MAC + 1)
#define CPU_OR1K_SPR_MACHI  (SPR_GRP5_MAC + 2)

/* Group6: Debug registers */
#define CPU_OR1K_SPR_DVR0   (SPR_GRP6_DEBUG + 0)
#define CPU_OR1K_SPR_DVR1   (SPR_GRP6_DEBUG + 1)
#define CPU_OR1K_SPR_DVR2   (SPR_GRP6_DEBUG + 2)
#define CPU_OR1K_SPR_DVR3   (SPR_GRP6_DEBUG + 3)
#define CPU_OR1K_SPR_DVR4   (SPR_GRP6_DEBUG + 4)
#define CPU_OR1K_SPR_DVR5   (SPR_GRP6_DEBUG + 5)
#define CPU_OR1K_SPR_DVR6   (SPR_GRP6_DEBUG + 6)
#define CPU_OR1K_SPR_DVR7   (SPR_GRP6_DEBUG + 7)
#define CPU_OR1K_SPR_DCR0   (SPR_GRP6_DEBUG + 8)
#define CPU_OR1K_SPR_DCR1   (SPR_GRP6_DEBUG + 9)
#define CPU_OR1K_SPR_DCR2   (SPR_GRP6_DEBUG + 10)
#define CPU_OR1K_SPR_DCR3   (SPR_GRP6_DEBUG + 11)
#define CPU_OR1K_SPR_DCR4   (SPR_GRP6_DEBUG + 12)
#define CPU_OR1K_SPR_DCR5   (SPR_GRP6_DEBUG + 13)
#define CPU_OR1K_SPR_DCR6   (SPR_GRP6_DEBUG + 14)
#define CPU_OR1K_SPR_DCR7   (SPR_GRP6_DEBUG + 15)
#define CPU_OR1K_SPR_DMR1   (SPR_GRP6_DEBUG + 16)
#define CPU_OR1K_SPR_DMR2   (SPR_GRP6_DEBUG + 17)
#define CPU_OR1K_SPR_DCWR0  (SPR_GRP6_DEBUG + 18)
#define CPU_OR1K_SPR_DCWR1  (SPR_GRP6_DEBUG + 19)
#define CPU_OR1K_SPR_DSR    (SPR_GRP6_DEBUG + 20)
#define CPU_OR1K_SPR_DRR    (SPR_GRP6_DEBUG + 21)

/* Group7: Performance counters registers */
#define CPU_OR1K_SPR_PCCR0  (SPR_GRP7_PERF_CTR + 0)
#define CPU_OR1K_SPR_PCCR1  (SPR_GRP7_PERF_CTR + 1)
#define CPU_OR1K_SPR_PCCR2  (SPR_GRP7_PERF_CTR + 2)
#define CPU_OR1K_SPR_PCCR3  (SPR_GRP7_PERF_CTR + 3)
#define CPU_OR1K_SPR_PCCR4  (SPR_GRP7_PERF_CTR + 4)
#define CPU_OR1K_SPR_PCCR5  (SPR_GRP7_PERF_CTR + 5)
#define CPU_OR1K_SPR_PCCR6  (SPR_GRP7_PERF_CTR + 6)
#define CPU_OR1K_SPR_PCCR7  (SPR_GRP7_PERF_CTR + 7)
#define CPU_OR1K_SPR_PCMR0  (SPR_GRP7_PERF_CTR + 8)
#define CPU_OR1K_SPR_PCMR1  (SPR_GRP7_PERF_CTR + 9)
#define CPU_OR1K_SPR_PCMR2  (SPR_GRP7_PERF_CTR + 10)
#define CPU_OR1K_SPR_PCMR3  (SPR_GRP7_PERF_CTR + 11)
#define CPU_OR1K_SPR_PCMR4  (SPR_GRP7_PERF_CTR + 12)
#define CPU_OR1K_SPR_PCMR5  (SPR_GRP7_PERF_CTR + 13)
#define CPU_OR1K_SPR_PCMR6  (SPR_GRP7_PERF_CTR + 14)
#define CPU_OR1K_SPR_PCMR7  (SPR_GRP7_PERF_CTR + 15)

/* Group8: Power management register */
#define CPU_OR1K_SPR_PMR    (SPR_GRP8_PWR_MNG + 0)

/* Group9: PIC registers */
#define CPU_OR1K_SPR_PICMR  (SPR_GRP9_PIC + 0)
#define CPU_OR1K_SPR_PICSR  (SPR_GRP9_PIC + 2)

/* Group10: Tick Timer registers */
#define CPU_OR1K_SPR_TTMR   (SPR_GPR10_TICK_TMR + 0)
#define CPU_OR1K_SPR_TTCR   (SPR_GPR10_TICK_TMR + 1)

 /* Shift amount macros for bits position in Supervision Register */
#define CPU_OR1K_SPR_SR_SHAMT_SM     (0)
#define CPU_OR1K_SPR_SR_SHAMT_TEE    (1)
#define CPU_OR1K_SPR_SR_SHAMT_IEE    (2)
#define CPU_OR1K_SPR_SR_SHAMT_DCE    (3)
#define CPU_OR1K_SPR_SR_SHAMT_ICE    (4)
#define CPU_OR1K_SPR_SR_SHAMT_DME    (5)
#define CPU_OR1K_SPR_SR_SHAMT_IME    (6)
#define CPU_OR1K_SPR_SR_SHAMT_LEE    (7)
#define CPU_OR1K_SPR_SR_SHAMT_CE     (8)
#define CPU_OR1K_SPR_SR_SHAMT_F      (9)
#define CPU_OR1K_SPR_SR_SHAMT_CY     (10)
#define CPU_OR1K_SPR_SR_SHAMT_OV     (11)
#define CPU_OR1K_SPR_SR_SHAMT_OVE    (12)
#define CPU_OR1K_SPR_SR_SHAMT_DSX    (13)
#define CPU_OR1K_SPR_SR_SHAMT_EPH    (14)
#define CPU_OR1K_SPR_SR_SHAMT_FO     (15)
#define CPU_OR1K_SPR_SR_SHAMT_SUMRA  (16)
#define CPU_OR1K_SPR_SR_SHAMT_CID    (28)

/* Supervision Mode Register. @see OpenRISC architecture manual*/

 /* Supervisor Mode */
#define CPU_OR1K_SPR_SR_SM    (1 << CPU_OR1K_SPR_SR_SHAMT_SM)
/* Tick Timer Exception Enabled */
#define CPU_OR1K_SPR_SR_TEE   (1 << CPU_OR1K_SPR_SR_SHAMT_TEE)
/* Interrupt Exception Enabled */
#define CPU_OR1K_SPR_SR_IEE   (1 << CPU_OR1K_SPR_SR_SHAMT_IEE)
/* Data Cache Enable */
#define CPU_OR1K_SPR_SR_DCE   (1 << CPU_OR1K_SPR_SR_SHAMT_DCE)
/* Instruction Cache Enable */
#define CPU_OR1K_SPR_SR_ICE   (1 << CPU_OR1K_SPR_SR_SHAMT_ICE)
/* Data MMU Enable */
#define CPU_OR1K_SPR_SR_DME   (1 << CPU_OR1K_SPR_SR_SHAMT_DME)
/* Instruction MMU Enable */
#define CPU_OR1K_SPR_SR_IME   (1 << CPU_OR1K_SPR_SR_SHAMT_IME)
/* Little Endian Enable */
#define CPU_OR1K_SPR_SR_LEE   (1 << CPU_OR1K_SPR_SR_SHAMT_LEE)
/* CID Enable */
#define CPU_OR1K_SPR_SR_CE    (1 << CPU_OR1K_SPR_SR_SHAMT_CE)
/* Conditional branch flag */
#define CPU_OR1K_SPR_SR_F     (1 << CPU_OR1K_SPR_SR_SHAMT_F)
/* Carry flag */
#define CPU_OR1K_SPR_SR_CY    (1 << CPU_OR1K_SPR_SR_SHAMT_CY)
/* Overflow flag */
#define CPU_OR1K_SPR_SR_OV    (1 << CPU_OR1K_SPR_SR_SHAMT_OV)
/* Overflow flag Exception */
#define CPU_OR1K_SPR_SR_OVE   (1 << CPU_OR1K_SPR_SR_SHAMT_OVE)
/* Delay Slot Exception */
#define CPU_OR1K_SPR_SR_DSX   (1 << CPU_OR1K_SPR_SR_SHAMT_DSX)
 /* Exception Prefix High */
#define CPU_OR1K_SPR_SR_EPH   (1 << CPU_OR1K_SPR_SR_SHAMT_EPH)
/* Fixed One */
#define CPU_OR1K_SPR_SR_FO    (1 << CPU_OR1K_SPR_SR_SHAMT_FO)
/* SPRs User Mode Read Access */
#define CPU_OR1K_SPR_SR_SUMRA (1 << CPU_OR1K_SPR_SR_SHAMT_SUMRA)
/*Context ID (Fast Context Switching) */
#define CPU_OR1K_SPR_SR_CID   (F << CPU_OR1K_SPR_SR_SHAMT_CID)

/* Tick timer configuration bits */
#define CPU_OR1K_SPR_TTMR_SHAMT_IP    28
#define CPU_OR1K_SPR_TTMR_SHAMT_IE    29
#define CPU_OR1K_SPR_TTMR_SHAMT_MODE  30

#define CPU_OR1K_SPR_TTMR_TP_MASK       (0x0FFFFFFF)
#define CPU_OR1K_SPR_TTMR_IP            (1 << CPU_OR1K_SPR_TTMR_SHAMT_IP)
#define CPU_OR1K_SPR_TTMR_IE            (1 << CPU_OR1K_SPR_TTMR_SHAMT_IE)
#define CPU_OR1K_SPR_TTMR_MODE_RESTART  (1 << CPU_OR1K_SPR_TTMR_SHAMT_MODE)
#define CPU_OR1K_SPR_TTMR_MODE_ONE_SHOT (2 << CPU_OR1K_SPR_TTMR_SHAMT_MODE)
#define CPU_OR1K_SPR_TTMR_MODE_CONT     (3 << CPU_OR1K_SPR_TTMR_SHAMT_MODE)

/* Power management register bits */

/* Shift amount macros for bit positions in Power Management register */
#define CPU_OR1K_SPR_PMR_SHAMT_SDF  0
#define CPU_OR1K_SPR_PMR_SHAMT_DME  4
#define CPU_OR1K_SPR_PMR_SHAMT_SME  5
#define CPU_OR1K_SPR_PMR_SHAMT_DCGE 6
#define CPU_OR1K_SPR_PMR_SHAMT_SUME 7

#define CPU_OR1K_SPR_PMR_SDF  (0xF << CPU_OR1K_SPR_PMR_SHAMT_SDF)
#define CPU_OR1K_SPR_PMR_DME  (1 << CPU_OR1K_SPR_PMR_SHAMT_DME)
#define CPU_OR1K_SPR_PMR_SME  (1 << CPU_OR1K_SPR_PMR_SHAMT_SME)
#define CPU_OR1K_SPR_PMR_DCGE (1 << CPU_OR1K_SPR_PMR_SHAMT_DCGE)
#define CPU_OR1K_SPR_PMR_SUME (1 << CPU_OR1K_SPR_PMR_SHAMT_SUME)

#ifndef ASM

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Supervision Mode registers definitions.
 *
 * @see OpenRISC architecture manual - revision 0.
 */
typedef enum {
  OR1K_EXCEPTION_RESET = 1,
  OR1K_EXCEPTION_BUS_ERR = 2,
  OR1K_EXCEPTION_D_PF = 3, /* Data Page Fault */
  OR1K_EXCEPTION_I_PF = 4, /* Instruction Page Fault */
  OR1K_EXCEPTION_TICK_TIMER = 5,
  OR1K_EXCEPTION_ALIGNMENT = 6,
  OR1K_EXCEPTION_I_UNDEF= 7, /* Undefiend instruction */
  OR1K_EXCEPTION_IRQ = 8, /* External interrupt */
  OR1K_EXCPETION_D_TLB = 9, /* Data TLB miss */
  OR1K_EXCPETION_I_TLB = 10, /* Instruction TLB miss */
  OR1K_EXCPETION_RANGE = 11, /* Range exception */
  OR1K_EXCPETION_SYS_CALL = 12,
  OR1K_EXCPETION_FP = 13, /* Floating point exception */
  OR1K_EXCPETION_TRAP = 14, /* Caused by l.trap instruction or by debug unit */
  OR1K_EXCPETION_RESERVED1 = 15,
  OR1K_EXCPETION_RESERVED2 = 16,
  OR1K_EXCPETION_RESERVED3 = 17,
  MAX_EXCEPTIONS = 17,
  OR1K_EXCEPTION_MAKE_ENUM_32_BIT = 0xffffffff
} OR1K_Symbolic_exception_name;

static inline uint32_t _OR1K_mfspr(uint32_t reg)
{
   uint32_t spr_value;

   asm volatile (
     "l.mfspr  %0, %1, 0;\n\t"
     : "=r" (spr_value) : "r" (reg));

   return spr_value;
}

static inline void _OR1K_mtspr(uint32_t reg, uint32_t value)
{
   asm volatile (
     "l.mtspr  %1, %0, 0;\n\t"
     :: "r" (value), "r" (reg)
   );
}

/**
 * @brief The slow down feature takes advantage of the low-power
 * dividers in external clock generation circuitry to enable full
 * functionality, but at a lower frequency so that power consumption
 * is reduced. @see OpenRISC architecture manual, power management section.
 *
 * @param[in] value is 4 bit value to be written in PMR[SDF].
 * A lower value specifies higher expected performance from the processor core.
 *
 */
#define _OR1K_CPU_SlowDown(value) \
   _OR1K_mtspr(CPU_OR1K_SPR_PMR, (value & CPU_OR1K_SPR_PMR_SDF))


#define _OR1K_CPU_Doze() \
  _OR1K_mtspr(CPU_OR1K_SPR_PMR, CPU_OR1K_SPR_PMR_DME)


#define _OR1K_CPU_Sleep() \
   _OR1K_mtspr(CPU_OR1K_SPR_PMR, CPU_OR1K_SPR_PMR_SME)

#define _OR1K_CPU_Suspend() \
   _OR1K_mtspr(CPU_OR1K_SPR_PMR, CPU_OR1K_SPR_PMR_SME)

static inline void _OR1K_Sync_mem( void )
{
  asm volatile("l.msync");
}

static inline void _OR1K_Sync_pipeline( void )
{
  asm volatile("l.psync");
}

/**
 * @brief or1ksim simulator can be sent a halt signal from RTEMS to tell
 * the running or1ksim process on the host machine to exit. The following
 * implementation has no effect on QEMU or hardware implementation and will
 * be treated as normal l.nop.
 *
 */
#define _OR1KSIM_CPU_Halt() \
	asm volatile ("l.nop 0xc")

#ifdef __cplusplus
}
#endif

#else /* ASM */

#endif /* ASM */

#endif /* _RTEMS_SCORE_OR1K_UTILITY_H */
