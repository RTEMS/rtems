/*
 *  Description:
 *		
 * Definitions for HP PA Risc
 * ref: PA RISC 1.1 Architecture and Instruction Set Reference Manual
 *	
 *  COPYRIGHT (c) 1994 by Division Incorporated
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 * Note:
 *      This file is included by both C and assembler code ( -DASM )
 *
 *  $Id$
 */

#ifndef _INCLUDE_HPPA_H
#define _INCLUDE_HPPA_H

#if defined(__cplusplus)
extern "C" {
#endif

/*
 *  This section contains the information required to build
 *  RTEMS for a particular member of the Hewlett Packard
 *  PA-RISC family.  It does this by setting variables to
 *  indicate which implementation dependent features are
 *  present in a particular member of the family.
 */

/*
 *  Hack to allow multlib effort to continue -- known to build.
 */

#define CPU_MODEL_NAME  "hppa 7xxx"
#if 0
#if defined(rtems_multilib)
/*
 *  Figure out all CPU Model Feature Flags based upon compiler 
 *  predefines. 
 */

#define CPU_MODEL_NAME  "rtems_multilib"

#elif defined(hppa7100)

#define CPU_MODEL_NAME  "hppa 7100"

#elif defined(hppa7200)

#define CPU_MODEL_NAME  "hppa 7200"

#else

#error "Unsupported CPU Model"

#endif
#endif
          
/*
 *  Define the name of the CPU family.
 */

#if !defined(CPU_NAME)
#define CPU_NAME "HP PA-RISC 1.1"
#endif

/*
 * Processor Status Word (PSW) Masks
 */


#define HPPA_PSW_Y      0x80000000    /* Data Debug Trap Disable */
#define HPPA_PSW_Z      0x40000000    /* Instruction Debug Trap Disable */
#define HPPA_PSW_r2     0x20000000    /* reserved */
#define HPPA_PSW_r3     0x10000000    /* reserved */
#define HPPA_PSW_r4     0x08000000    /* reserved */
#define HPPA_PSW_E      0x04000000    /* Little Endian on Memory References */
#define HPPA_PSW_S      0x02000000    /* Secure Interval Timer */
#define HPPA_PSW_T      0x01000000    /* Taken Branch Trap Enable */
#define HPPA_PSW_H      0x00800000    /* Higher-Privilege Transfer Trap Enable*/
#define HPPA_PSW_L      0x00400000    /* Lower-Privilege Transfer Trap Enable */
#define HPPA_PSW_N      0x00200000    /* PC Queue Front Instruction Nullified */
#define HPPA_PSW_X      0x00100000    /* Data Memory Break Disable */
#define HPPA_PSW_B      0x00080000    /* Taken Branch in Previous Cycle */
#define HPPA_PSW_C      0x00040000    /* Code Address Translation Enable */
#define HPPA_PSW_V      0x00020000    /* Divide Step Correction */
#define HPPA_PSW_M      0x00010000    /* High-Priority Machine Check Disable */
#define HPPA_PSW_CB     0x0000ff00    /* Carry/Borrow Bits */
#define HPPA_PSW_r24    0x00000080    /* reserved */
#define HPPA_PSW_G      0x00000040    /* Debug trap Enable */
#define HPPA_PSW_F      0x00000020    /* Performance monitor interrupt unmask */
#define HPPA_PSW_R      0x00000010    /* Recovery Counter Enable */
#define HPPA_PSW_Q      0x00000008    /* Interruption State Collection Enable */
#define HPPA_PSW_P      0x00000004    /* Protection ID Validation Enable */
#define HPPA_PSW_D      0x00000002    /* Data Address Translation Enable */
#define HPPA_PSW_I      0x00000001    /* External, Power Failure, */
                                      /*   Low-Priority Machine Check */
                                      /*   Interruption Enable */

/*
 * HPPA traps and interrupts
 * basic layout.  Note numbers do not denote priority
 *
 *      0-31    basic traps and interrupts defined by HPPA architecture
 *      0-31    32 external interrupts
 *     32-...   bsp defined
 */

#define HPPA_TRAP_NON_EXISTENT                     0
/* group 1 */
#define HPPA_TRAP_HIGH_PRIORITY_MACHINE_CHECK      1
/* group 2 */
#define HPPA_TRAP_POWER_FAIL                       2
#define HPPA_TRAP_RECOVERY_COUNTER                 3
#define HPPA_TRAP_EXTERNAL_INTERRUPT               4
#define HPPA_TRAP_LOW_PRIORITY_MACHINE_CHECK       5
#define HPPA_TRAP_PERFORMANCE_MONITOR             29
/* group 3 */
#define HPPA_TRAP_INSTRUCTION_TLB_MISS             6
#define HPPA_TRAP_INSTRUCTION_MEMORY_PROTECTION    7
#define HPPA_TRAP_INSTRUCTION_DEBUG               30
#define HPPA_TRAP_ILLEGAL_INSTRUCTION              8
#define HPPA_TRAP_BREAK_INSTRUCTION                9
#define HPPA_TRAP_PRIVILEGED_OPERATION            10
#define HPPA_TRAP_PRIVILEGED_REGISTER             11
#define HPPA_TRAP_OVERFLOW                        12
#define HPPA_TRAP_CONDITIONAL                     13
#define HPPA_TRAP_ASSIST_EXCEPTION                14
#define HPPA_TRAP_DATA_TLB_MISS                   15
#define HPPA_TRAP_NON_ACCESS_INSTRUCTION_TLB_MISS 16
#define HPPA_TRAP_NON_ACCESS_DATA_TLB_MISS        17
#define HPPA_TRAP_DATA_MEMORY_ACCESS_RIGHTS       26
#define HPPA_TRAP_DATA_MEMORY_PROTECTION_ID       27
#define HPPA_TRAP_UNALIGNED_DATA_REFERENCE        28
#define HPPA_TRAP_DATA_MEMORY_PROTECTION          18
#define HPPA_TRAP_DATA_MEMORY_BREAK               19
#define HPPA_TRAP_TLB_DIRTY_BIT                   20
#define HPPA_TRAP_PAGE_REFERENCE                  21
#define HPPA_TRAP_DATA_DEBUG                      31
#define HPPA_TRAP_ASSIST_EMULATION                22
/* group 4 */
#define HPPA_TRAP_HIGHER_PRIVILEGE_TRANSFER       23
#define HPPA_TRAP_LOWER_PRIVILEGE_TRANSFER        24
#define HPPA_TRAP_TAKEN_BRANCH                    25

#define HPPA_INTERNAL_TRAPS                       32

/* External Interrupts via interrupt 4 */

#define HPPA_INTERRUPT_EXTERNAL_0                  0
#define HPPA_INTERRUPT_EXTERNAL_1                  1
#define HPPA_INTERRUPT_EXTERNAL_2                  2
#define HPPA_INTERRUPT_EXTERNAL_3                  3
#define HPPA_INTERRUPT_EXTERNAL_4                  4
#define HPPA_INTERRUPT_EXTERNAL_5                  5
#define HPPA_INTERRUPT_EXTERNAL_6                  6
#define HPPA_INTERRUPT_EXTERNAL_7                  7
#define HPPA_INTERRUPT_EXTERNAL_8                  8
#define HPPA_INTERRUPT_EXTERNAL_9                  9
#define HPPA_INTERRUPT_EXTERNAL_10                10
#define HPPA_INTERRUPT_EXTERNAL_11                11
#define HPPA_INTERRUPT_EXTERNAL_12                12
#define HPPA_INTERRUPT_EXTERNAL_13                13
#define HPPA_INTERRUPT_EXTERNAL_14                14
#define HPPA_INTERRUPT_EXTERNAL_15                15
#define HPPA_INTERRUPT_EXTERNAL_16                16
#define HPPA_INTERRUPT_EXTERNAL_17                17
#define HPPA_INTERRUPT_EXTERNAL_18                18
#define HPPA_INTERRUPT_EXTERNAL_19                19
#define HPPA_INTERRUPT_EXTERNAL_20                20
#define HPPA_INTERRUPT_EXTERNAL_21                21
#define HPPA_INTERRUPT_EXTERNAL_22                22
#define HPPA_INTERRUPT_EXTERNAL_23                23
#define HPPA_INTERRUPT_EXTERNAL_24                24
#define HPPA_INTERRUPT_EXTERNAL_25                25
#define HPPA_INTERRUPT_EXTERNAL_26                26
#define HPPA_INTERRUPT_EXTERNAL_27                27
#define HPPA_INTERRUPT_EXTERNAL_28                28
#define HPPA_INTERRUPT_EXTERNAL_29                29
#define HPPA_INTERRUPT_EXTERNAL_30                30
#define HPPA_INTERRUPT_EXTERNAL_31                31

#define HPPA_INTERRUPT_EXTERNAL_INTERVAL_TIMER    HPPA_INTERRUPT_EXTERNAL_0
#define HPPA_EXTERNAL_INTERRUPTS                  32

/* BSP defined interrupts begin here */

#define HPPA_INTERRUPT_MAX  32

/*
 * Cache characteristics
 */
 
#define HPPA_CACHELINE_SIZE     32
#define HPPA_CACHELINE_MASK     (HPPA_CACHELINE_SIZE - 1)

/*
 * page size characteristics
 */

#define HPPA_PAGE_SIZE          4096
#define HPPA_PAGE_MASK          (0xfffff000)


/*
 * TLB characteristics
 *
 * Flags and Access Control layout for using TLB protection insertion
 *
 *                      1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 3 3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |?|?|T|D|B|type |PL1|Pl2|U|           access id               |?|
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

/*
 * Access rights (type + PL1 + PL2)
 */
#define HPPA_PROT_R    0x00c00000   /* Read Only, no Write, no Execute */
#define HPPA_PROT_RW   0x01c00000   /* Read & Write Only, no Execute */
#define HPPA_PROT_RX   0x02c00000   /* Read & Execute Only, no Write */
#define HPPA_PROT_RWX  0x03c00000   /* Read, Write, Execute */
#define HPPA_PROT_X0   0x04c00000   /* Execute Only, Promote to Level 0 */
#define HPPA_PROT_X1   0x05c00000   /* Execute Only, Promote to Level 1 */
#define HPPA_PROT_X2   0x06c00000   /* Execute Only, Promote to Level 2 */
#define HPPA_PROT_X3   0x07c00000   /* Execute Only, Promote to Level 3 */

/*
 * Floating point status register definitions
 */

#define HPPA_FPSTATUS_ENABLE_I	  0x00000001	/* inexact operation */
#define HPPA_FPSTATUS_ENABLE_U	  0x00000002    /* underflow */
#define HPPA_FPSTATUS_ENABLE_O	  0x00000004    /* overflow */
#define HPPA_FPSTATUS_ENABLE_Z    0x00000008    /* division by zero */
#define HPPA_FPSTATUS_ENABLE_V    0x00000010    /* invalid operation */
#define HPPA_FPSTATUS_D           0x00000020    /* denormalize as zero */
#define HPPA_FPSTATUS_T           0x00000040    /* delayed trap */
#define HPPA_FPSTATUS_RM_MASK     0x00000600    /* rounding mode */
#define HPPA_FPSTATUS_RM_SHIFT    9
#define HPPA_FPSTATUS_CQ_MASK     0x001FFC00	/* compare queue */
#define HPPA_FPSTATUS_CQ_SHIFT	  13
#define HPPA_FPSTATUS_C		  0x04000000	/* most recent ompare bit */
#define HPPA_FPSTATUS_FLAG_I      0x08000000	/* inexact */
#define HPPA_FPSTATUS_FLAG_U	  0x10000000	/* underflow */
#define HPPA_FPSTATUS_FLAG_O	  0x20000000	/* overflow */
#define HPPA_FPSTATUS_FLAG_Z	  0x40000000	/* division by zero */
#define HPPA_FPSTATUS_FLAG_V      0x80000000	/* invalid operation */


/*
 * Inline macros for misc. interesting opcodes
 */

/* generate a global label */
#define HPPA_ASM_LABEL(label) \
    asm(".export " label ", ! .label " label);

/* Return From Interrupt                RFI */
#define HPPA_ASM_RFI()                  asm volatile ("rfi")

/* Set System Mask                      SSM i,t */
#define HPPA_ASM_SSM(i,gr)              asm volatile ("ssm %1, %0" \
                                                      : "=r" (gr)   \
                                                      : "i"  (i))
/* Reset System Mask                    RSM i,t */
#define HPPA_ASM_RSM(i,gr)              asm volatile ("rsm %1, %0" \
                                                      : "=r" (gr)   \
                                                      : "i"  (i))
/* Move To System Mask                  MTSM r */
#define HPPA_ASM_MTSM(gr)               asm volatile ("mtsm %0" \
                                                      : : "r" (gr))

/* Load Space Identifier                LDSID (s,b),t */
#define HPPA_ASM_LDSID(sr,grb,grt)      asm volatile ("ldsid (%1,%2),%0" \
                                                      : "=r" (grt)  \
                                                      : "i"  (sr), \
                                                        "r"  (grb))

/*
 * Gcc extended asm doesn't really allow for treatment of space registers
 * as "registers", so we have to use "i" format.
 * Unfortunately this means that the "=" constraint is not available.
 */

/* Move To Space Register               MTSP r,sr */
#define HPPA_ASM_MTSP(gr,sr)            asm volatile ("mtsp %1,%0" \
                                                      : : "i"  (sr),  \
                                                          "r"  (gr))

/* Move From Space Register             MFSP sr,t */
#define HPPA_ASM_MFSP(sr,gr)            asm volatile ("mfsp %1,%0" \
                                                      : "=r" (gr)  \
                                                      : "i"  (sr))

/* Move To Control register             MTCTL r,t */
#define HPPA_ASM_MTCTL(gr,cr)           asm volatile ("mtctl %1,%0" \
                                                      : : "i" (cr), \
                                                          "r" (gr))

/* Move From Control register           MFCTL r,t */
#define HPPA_ASM_MFCTL(cr,gr)           asm volatile ("mfctl %1,%0" \
                                                      : "=r" (gr) \
                                                      : "i" (cr))

/* Synchronize caches                   SYNC */
#define HPPA_ASM_SYNC()                 asm volatile ("sync")

/* Probe Read Access                    PROBER (s,b),r,t */
#define HPPA_ASM_PROBER(sr,groff,gracc,grt) \
                                        asm volatile ("prober (%1,%2),%3,%0" \
                                                      : "=r" (grt) \
                                                      : "i"  (sr), \
                                                        "r"  (groff), \
                                                        "r"  (gracc))

/* Probe Read Access Immediate          PROBERI (s,b),i,t*/
#define HPPA_ASM_PROBERI(sr,groff,iacc,grt) \
                                        asm volatile ("proberi (%1,%2),%3,%0" \
                                                      : "=r" (grt) \
                                                      : "i"  (sr), \
                                                        "r"  (groff), \
                                                        "i"  (iacc))

/* Probe Write Access                   PROBEW (s,b),r,t */
#define HPPA_ASM_PROBEW(sr,groff,gracc,grt) \
                                        asm volatile ("probew (%1,%2),%3,%0" \
                                                      : "=r" (grt) \
                                                      : "i"  (sr), \
                                                        "r"  (groff), \
                                                        "r"  (gracc))

/* Probe Write Access Immediate         PROBEWI (s,b),i,t */
#define HPPA_ASM_PROBEWI(sr,groff,iacc,grt) \
                                        asm volatile ("probewi (%1,%2),%3,%0" \
                                                      : "=r" (grt) \
                                                      : "i"  (sr), \
                                                        "r"  (groff), \
                                                        "i"  (iacc))

/* Load Physical Address                LPA x(s,b),t */
#define HPPA_ASM_LPA(sr,grb,grt)        asm volatile ("lpa %%r0(%1,%2),%0" \
                                                      : "=r" (grt) \
                                                      : "i"  (sr), \
                                                        "r"  (grb))

/* Load Coherence Index                 LCI x(s,b),t */
/* AKA: Load Hash Address               LHA x(s,b),t */
#define HPPA_ASM_LCI(grx,sr,grb,grt)    asm volatile ("lha %1(%2,%3),%0" \
                                                      : "=r" (grt) \
                                                      : "r"  (grx),\
                                                        "i"  (sr), \
                                                        "r"  (grb))
#define HPPA_ASM_LHA(grx,sr,grb,grt)    HPPA_ASM_LCI(grx,sr,grb,grt)

/* Purge Data Tlb                       PDTLB  x(s,b) */
#define HPPA_ASM_PDTLB(grx,sr,grb)      asm volatile ("pdtlb %0(%1,%2)" \
                                                      : : "r"  (grx), \
                                                          "i"  (sr),  \
                                                          "r"  (grb))

/* Purge Instruction Tlb                PITLB  x(s,b) */
#define HPPA_ASM_PITLB(grx,sr,grb)      asm volatile ("pitlb %0(%1,%2)" \
                                                      : : "r"  (grx), \
                                                          "i"  (sr),  \
                                                          "r"  (grb))

/* Purge Data Tlb Entry                 PDTLBE  x(s,b) */
#define HPPA_ASM_PDTLBE(grx,sr,grb)     asm volatile ("pdtlbe %0(%1,%2)" \
                                                      : : "r"  (grx), \
                                                          "i"  (sr),  \
                                                          "r"  (grb))

/* Purge Instruction Tlb Entry          PITLBE  x(s,b) */
#define HPPA_ASM_PITLBE(grx,sr,grb)     asm volatile ("pitlbe %0(%1,%2)" \
                                                      : : "r"  (grx), \
                                                          "i"  (sr),  \
                                                          "r"  (grb))


/* Insert Data TLB Address              IDTLBA r,(s,b) */
#define HPPA_ASM_IDTLBA(gr,sr,grb)      asm volatile ("idtlba %0,(%1,%2)" \
                                                      : : "r"  (gr),  \
                                                          "i"  (sr),  \
                                                          "r"  (grb))

/* Insert Instruction TLB Address       IITLBA r,(s,b) */
#define HPPA_ASM_IITLBA(gr,sr,grb)      asm volatile ("iitlba %0,(%1,%2)" \
                                                      : : "r"  (gr),  \
                                                          "i"  (sr),  \
                                                          "r"  (grb))

/* Insert Data TLB Protection           IDTLBP r,(s,b) */
#define HPPA_ASM_IDTLBP(gr,sr,grb)      asm volatile ("idtlbp %0,(%1,%2)" \
                                                      : : "r"  (gr),  \
                                                          "i"  (sr),  \
                                                          "r"  (grb))

/* Insert Instruction TLB Protection    IITLBP r,(s,b) */
#define HPPA_ASM_IITLBP(gr,sr,grb)      asm volatile ("iitlbp %0,(%1,%2)" \
                                                      : : "r"  (gr),  \
                                                          "i"  (sr),  \
                                                          "r"  (grb))

/* Purge Data Cache                     PDC x(s,b) */
#define HPPA_ASM_PDC(grx,sr,grb)        asm volatile ("pdc %0(%1,%2)" \
                                                      : : "r"  (grx), \
                                                          "i"  (sr),  \
                                                          "r"  (grb))

/* Flush Data Cache                     FDC x(s,b) */
#define HPPA_ASM_FDC(grx,sr,grb)        asm volatile ("fdc %0(%1,%2)" \
                                                      : : "r"  (grx), \
                                                          "i"  (sr),  \
                                                          "r"  (grb))

/* Flush Instruction Cache              FDC x(s,b) */
#define HPPA_ASM_FIC(grx,sr,grb)        asm volatile ("fic %0(%1,%2)" \
                                                      : : "r"  (grx), \
                                                          "i"  (sr),  \
                                                          "r"  (grb))

/* Flush Data Cache Entry               FDCE x(s,b) */
#define HPPA_ASM_FDCE(grx,sr,grb)       asm volatile ("fdce %0(%1,%2)" \
                                                      : : "r"  (grx), \
                                                          "i"  (sr),  \
                                                          "r"  (grb))

/* Flush Instruction Cache Entry        FICE x(s,b) */
#define HPPA_ASM_FICE(grx,sr,grb)       asm volatile ("fice %0(%1,%2)" \
                                                      : : "r"  (grx), \
                                                          "i"  (sr),  \
                                                          "r"  (grb))

/* Break                                BREAK i5,i13 */
#define HPPA_ASM_BREAK(i5,i13)          asm volatile ("break %0,%1" \
                                                      : : "i" (i5), \
                                                          "i" (i13))

/* Load and Clear Word Short            LDCWS d(s,b),t */
#define HPPA_ASM_LDCWS(i,sr,grb,grt)    asm volatile ("ldcws %1(%2,%3),%0" \
                                                      : "=r" (grt) \
                                                      : "i"  (i),  \
                                                        "i"  (sr), \
                                                        "r"  (grb))

/* Load and Clear Word Indexed          LDCWX x(s,b),t */
#define HPPA_ASM_LDCWX(grx,sr,grb,grt)  asm volatile ("ldcwx %1(%2,%3),%0" \
                                                      : "=r" (grt)  \
                                                      : "r"  (grx), \
                                                        "i"  (sr),  \
                                                        "r"  (grb))

/* Load Word Absolute Short             LDWAS d(b),t */
/* NOTE: "short" here means "short displacement"     */
#define HPPA_ASM_LDWAS(disp,grbase,gr)  asm volatile("ldwas %1(%2),%0" \
                                                     : "=r" (gr)    \
                                                     : "i"  (disp), \
                                                       "r"  (grbase))

/* Store Word Absolute Short            STWAS r,d(b) */
/* NOTE: "short" here means "short displacement"     */
#define HPPA_ASM_STWAS(gr,disp,grbase)  asm volatile("stwas %0,%1(%2)" \
                                                     : : "r" (gr),   \
                                                         "i" (disp), \
                                                         "r" (grbase))

/*
 * Swap bytes
 * REFERENCE:  PA72000 TRM -- Appendix C
 */
#define HPPA_ASM_SWAPBYTES(value, swapped)  asm volatile( \
    " shd    %1,%1,16,%0  \n\
      dep    %0,15,8,%0   \n\
      shd    %1,%0,8,%0"    \
    : "=r" (swapped)        \
    : "r" (value)           \
  )


/* 72000 Diagnose instructions follow
 * These macros assume gas knows about these instructions.
 * gas2.2.u1 did not.
 * I added them to my copy and installed it locally.
 *
 * There are *very* special requirements for these guys
 *   ref: TRM 6.1.3 Programming Constraints
 *
 * The macros below handle the following rules
 *
 *   Except for WIT, WDT, WDD, WIDO, WIDE, all DIAGNOSE must be doubled.
 *   Must never be nullified (hence the leading nop)
 *   NOP must preced every RDD,RDT,WDD,WDT,RDTLB
 *   Instruction preceeding GR_SHDW must not set any of the GR's saved
 *
 * The macros do *NOT* deal with the following problems
 *   doubled DIAGNOSE instructions must not straddle a page boundary
 *       if code translation enabled.  (since 2nd could trap on ITLB)
 *   If you care about DHIT and DPE bits of DR0, then
 *       No store instruction in the 2 insn window before RDD
 */


/* Move To CPU/DIAG register            MTCPU r,t */
#define HPPA_ASM_MTCPU(gr,dr)           asm volatile (" nop \n"          \
                                                      " mtcpu %1,%0 \n"  \
                                                      " mtcpu %1,%0"     \
                                                      : : "i" (dr), \
                                                          "r" (gr))

/* Move From CPU/DIAG register          MFCPU r,t */
#define HPPA_ASM_MFCPU(dr,gr)           asm volatile (" nop \n"          \
                                                      " mfcpu %1,%0\n"   \
                                                      " mfcpu %1,%0"     \
                                                      : "=r" (gr) \
                                                      : "i" (dr))

/* Transfer of Control Enable           TOC_EN */
#define HPPA_ASM_TOC_EN()               asm volatile (" tocen \n" \
                                                      " tocen")

/* Transfer of Control Disable          TOC_DIS */
#define HPPA_ASM_TOC_DIS()              asm volatile (" tocdis \n" \
                                                      " tocdis")

/* Shadow Registers to General Register SHDW_GR */
#define HPPA_ASM_SHDW_GR()              asm volatile (" shdwgr \n" \
                                                      " shdwgr"    \
                                                ::: "r1" "r8" "r9" "r16" \
                                                    "r17" "r24" "r25")

/* General Registers to Shadow Register GR_SHDW */
#define HPPA_ASM_GR_SHDW()              asm volatile (" nop \n" \
                                                      " grshdw \n" \
                                                      " grshdw")

/*
 * Definitions of special registers for use by the above macros.
 */

/* Hardware Space Registers */
#define HPPA_SR0     0
#define HPPA_SR1     1
#define HPPA_SR2     2
#define HPPA_SR3     3
#define HPPA_SR4     4
#define HPPA_SR5     5
#define HPPA_SR6     6
#define HPPA_SR7     7

/* Hardware Control Registers */
#define HPPA_CR0     0
#define HPPA_RCTR    0               /* Recovery Counter Register */

#define HPPA_CR8     8               /* Protection ID 1 */
#define HPPA_PIDR1   8

#define HPPA_CR9     9               /* Protection ID 2 */
#define HPPA_PIDR2   9

#define HPPA_CR10    10
#define HPPA_CCR     10              /* Coprocessor Confiquration Register */

#define HPPA_CR11    11
#define HPPA_SAR     11              /* Shift Amount Register */

#define HPPA_CR12    12
#define HPPA_PIDR3   12              /* Protection ID 3 */

#define HPPA_CR13    13
#define HPPA_PIDR4   13              /* Protection ID 4 */

#define HPPA_CR14    14
#define HPPA_IVA     14              /* Interrupt Vector Address */

#define HPPA_CR15    15
#define HPPA_EIEM    15              /* External Interrupt Enable Mask */

#define HPPA_CR16    16
#define HPPA_ITMR    16              /* Interval Timer */

#define HPPA_CR17    17
#define HPPA_PCSQ    17              /* Program Counter Space queue */

#define HPPA_CR18    18
#define HPPA_PCOQ    18              /* Program Counter Offset queue */

#define HPPA_CR19    19
#define HPPA_IIR     19              /* Interruption Instruction Register */

#define HPPA_CR20    20
#define HPPA_ISR     20              /* Interruption Space Register */

#define HPPA_CR21    21
#define HPPA_IOR     21              /* Interruption Offset Register */

#define HPPA_CR22    22
#define HPPA_IPSW    22              /* Interrpution Processor Status Word */

#define HPPA_CR23    23
#define HPPA_EIRR    23              /* External Interrupt Request */

#define HPPA_CR24    24
#define HPPA_PPDA    24              /* Physcial Page Directory Address */
#define HPPA_TR0     24              /* Temporary register 0 */

#define HPPA_CR25    25
#define HPPA_HTA     25              /* Hash Table Address */
#define HPPA_TR1     25              /* Temporary register 1 */

#define HPPA_CR26    26
#define HPPA_TR2     26              /* Temporary register 2 */

#define HPPA_CR27    27
#define HPPA_TR3     27              /* Temporary register 3 */

#define HPPA_CR28    28
#define HPPA_TR4     28              /* Temporary register 4 */

#define HPPA_CR29    29
#define HPPA_TR5     29              /* Temporary register 5 */

#define HPPA_CR30    30
#define HPPA_TR6     30              /* Temporary register 6 */

#define HPPA_CR31    31
#define HPPA_CPUID   31              /* MP identifier */

/*
 * Diagnose registers
 */

#define HPPA_DR0      0
#define HPPA_DR1      1
#define HPPA_DR8      8
#define HPPA_DR24    24
#define HPPA_DR25    25

/*
 * Tear apart a break instruction to find its type.
 */
#define HPPA_BREAK5(x)          ((x) & 0x1F)
#define HPPA_BREAK13(x)         (((x) >> 13) & 0x1FFF)

/* assemble a break instruction */
#define HPPA_BREAK(i5,i13)      (((i5) & 0x1F) | (((i13) & 0x1FFF) << 13))


/*
 * this won't work in ASM or non-GNU compilers
 */

#if !defined(ASM) && defined(__GNUC__)

/*
 * static inline utility functions to get at control registers
 */

#define EMIT_GET_CONTROL(name, reg)            \
static __inline__ unsigned int                 \
get_ ## name (void)                            \
{                                              \
    unsigned int value;                        \
    HPPA_ASM_MFCTL(reg, value);                \
    return value;                              \
}

#define EMIT_SET_CONTROL(name, reg)            \
static __inline__ void                         \
set_ ## name (unsigned int new_value)          \
{                                              \
    HPPA_ASM_MTCTL(new_value, reg);            \
}

#define EMIT_CONTROLS(name, reg)               \
    EMIT_GET_CONTROL(name, reg)                \
    EMIT_SET_CONTROL(name, reg)

EMIT_CONTROLS(recovery, HPPA_RCTR);          /* CR0  */
EMIT_CONTROLS(pid1, HPPA_PIDR1);             /* CR8  */
EMIT_CONTROLS(pid2, HPPA_PIDR2);             /* CR9  */
EMIT_CONTROLS(ccr, HPPA_CCR);                /* CR10; CCR and SCR share CR10 */
EMIT_CONTROLS(scr, HPPA_CCR);                /* CR10; CCR and SCR share CR10 */
EMIT_CONTROLS(sar, HPPA_SAR);                /* CR11 */
EMIT_CONTROLS(pid3, HPPA_PIDR3);             /* CR12 */
EMIT_CONTROLS(pid4, HPPA_PIDR4);             /* CR13 */
EMIT_CONTROLS(iva, HPPA_IVA);                /* CR14 */
EMIT_CONTROLS(eiem, HPPA_EIEM);              /* CR15 */
EMIT_CONTROLS(itimer, HPPA_ITMR);            /* CR16 */
EMIT_CONTROLS(pcsq, HPPA_PCSQ);              /* CR17 */
EMIT_CONTROLS(pcoq, HPPA_PCOQ);              /* CR18 */
EMIT_CONTROLS(iir, HPPA_IIR);                /* CR19 */
EMIT_CONTROLS(isr, HPPA_ISR);                /* CR20 */
EMIT_CONTROLS(ior, HPPA_IOR);                /* CR21 */
EMIT_CONTROLS(ipsw, HPPA_IPSW);              /* CR22 */
EMIT_CONTROLS(eirr, HPPA_EIRR);              /* CR23 */
EMIT_CONTROLS(tr0, HPPA_TR0);                /* CR24 */
EMIT_CONTROLS(tr1, HPPA_TR1);                /* CR25 */
EMIT_CONTROLS(tr2, HPPA_TR2);                /* CR26 */
EMIT_CONTROLS(tr3, HPPA_TR3);                /* CR27 */
EMIT_CONTROLS(tr4, HPPA_TR4);                /* CR28 */
EMIT_CONTROLS(tr5, HPPA_TR5);                /* CR29 */
EMIT_CONTROLS(tr6, HPPA_TR6);                /* CR30 */
EMIT_CONTROLS(tr7, HPPA_CR31);               /* CR31 */

#endif /* ASM and GNU */

/*
 * If and How to invoke the debugger (a ROM debugger generally)
 */
#define CPU_INVOKE_DEBUGGER \
    do { \
        HPPA_ASM_BREAK(1,1); \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif /* ! _INCLUDE_HPPA_H */

