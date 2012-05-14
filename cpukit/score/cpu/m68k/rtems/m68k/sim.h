/*
 *-------------------------------------------------------------------
 *
 *   SIM -- System Integration Module
 *
 * The system integration module (SIM) is used on many Motorola 16-
 * and 32-bit MCUs for the following functions:
 *
 *  () System configuration and protection. Bus and software watchdog
 *  monitors are provided in addition to periodic interrupt generators.
 *
 *  () Clock signal generation for other intermodule bus (IMB) members
 *  and external devices.
 *
 *  () The generation of chip-select signals that simplify external
 *  circuitry interface.
 *
 *  () Data ports that are available for general purpose input and
 *  output.
 *
 *  () A system test block that is intended only for factory tests.
 *
 * For more information, refer to Motorola's "Modular Microcontroller
 * Family System Integration Module Reference Manual" (Motorola document
 * SIMRM/AD).
 *
 * This file was created by John S. Gwynne to support Motorola's 68332 MCU.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the following conditions are met:
 * 1. Redistribution of source code and documentation must retain
 *    the above authorship, this list of conditions and the
 *    following disclaimer.
 * 2. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * This software is provided "AS IS" without warranty of any kind,
 * either expressed or implied, including, but not limited to, the
 * implied warranties of merchantability, title and fitness for a
 * particular purpose.
 *
 *------------------------------------------------------------------
 */

#ifndef _RTEMS_M68K_SIM_H
#define _RTEMS_M68K_SIM_H


/* SAM-- shift and mask */
#undef  SAM
#define SAM(a,b,c) ((a << b) & c)

/*
 *  These macros make this file usable from assembly.
 */

#ifdef ASM
#define SIM_VOLATILE_USHORT_POINTER
#define SIM_VOLATILE_UCHAR_POINTER
#else
#define SIM_VOLATILE_USHORT_POINTER (volatile unsigned short int * const)
#define SIM_VOLATILE_UCHAR_POINTER  (volatile unsigned char * const)
#endif

/* SIM_CRB (SIM Control Register Block) base address of the SIM
   control registers */
#ifndef SIM_CRB
#if SIM_MM == 0
#define SIM_CRB 0x7ffa00
#else /* SIM_MM */
#undef SIM_MM
#define SIM_MM 1
#define SIM_CRB 0xfffa00
#endif /* SIM_MM */
#endif /* SIM_CRB */


#define SIMCR SIM_VOLATILE_USHORT_POINTER(0x00 + SIM_CRB)
				/* Module Configuration Register */
#define    EXOFF 0x8000		/*    External Clock Off */
#define    FRZSW 0x4000		/*    Freeze Software Enable */
#define    FRZBM 0x2000		/*    Freeze Bus Monitor Enable */
#define    SLVEN 0x0800		/*    Factory Test Model Enabled (ro)*/
#define    SHEN  0x0300		/*    Show Cycle Enable */
#define    SUPV  0x0080		/*    Supervisor/Unrestricted Data Space */
#define    MM    0x0040 	/*    Module Mapping */
#define    IARB  0x000f		/*    Interrupt Arbitration Field */



#define SIMTR SIM_VOLATILE_USHORT_POINTER(0x02 + SIM_CRB)
				/* SIM Test Register */
/* Used only for factor testing */



#define SYNCR SIM_VOLATILE_USHORT_POINTER(0x04 + SIM_CRB)
				/* Clock Synthesizer Control Register */
#define    VCO      0x8000	/*    Frequency Control (VCO) */
#define    PRESCALE 0x4000	/*    Frequency Control Bit (Prescale) */
#define    COUNTER  0x3f00	/*    Frequency Control Counter */
#define    EDIV     0x0080	/*    ECLK Divide Rate */
#define    SLIMP    0x0010	/*    Limp Mode Status */
#define	   SLOCK    0x0008	/*    Synthesizer Lock */
#define    RSTEN    0x0004	/*    Reset Enable */
#define    STSIM    0x0002	/*    Stop Mode SIM Clock */
#define    STEXT    0x0001	/*    Stop Mode External Clock */



#define RSR SIM_VOLATILE_UCHAR_POINTER(0x07 + SIM_CRB)
				/* Reset Status Register */
#define    EXT   0x0080		/*    External Reset */
#define    POW   0x0040		/*    Power-On Reset */
#define    SW    0x0020		/*    Software Watchdog Reset */
#define    DBF   0x0010		/*    Double Bus Fault Reset */
#define    LOC   0x0004		/*    Loss of Clock Reset */
#define    SYS   0x0002		/*    System Reset */
#define    TST   0x0001		/*    Test Submodule Reset */



#define SIMTRE SIM_VOLATILE_USHORT_POINTER(0x08 + SIM_CRB)
				/* System Integration Test Register */
/* Used only for factor testing */



#define PORTE0 SIM_VOLATILE_UCHAR_POINTER(0x11 + SIM_CRB)
#define PORTE1 SIM_VOLATILE_UCHAR_POINTER(0x13 + SIM_CRB)
				/* Port E Data Register */
#define DDRE SIM_VOLATILE_UCHAR_POINTER(0x15 + SIM_CRB)
				/* Port E Data Direction Register */
#define PEPAR SIM_VOLATILE_UCHAR_POINTER(0x17 + SIM_CRB)
				/* Port E Pin Assignment Register */
/* Any bit cleared (zero) defines the corresponding pin to be an I/O
   pin. Any bit set defines the corresponding pin to be a bus control
   signal. */



#define PORTF0 SIM_VOLATILE_UCHAR_POINTER(0x19 + SIM_CRB)
#define PORTF1 SIM_VOLATILE_UCHAR_POINTER(0x1b + SIM_CRB)
				/* Port F Data Register */
#define DDRF SIM_VOLATILE_UCHAR_POINTER(0x1d + SIM_CRB)
				/* Port E Data Direction Register */
#define PFPAR SIM_VOLATILE_UCHAR_POINTER(0x1f + SIM_CRB)
/* Any bit cleared (zero) defines the corresponding pin to be an I/O
   pin. Any bit set defines the corresponding pin to be a bus control
   signal. */



#define SYPCR SIM_VOLATILE_UCHAR_POINTER(0x21 + SIM_CRB)
/* !!! can write to only once after reset !!! */
				/* System Protection Control Register */
#define    SWE   0x80		/*    Software Watch Enable */
#define    SWP   0x40		/*    Software Watchdog Prescale */
#define    SWT   0x30		/*    Software Watchdog Timing */
#define    HME   0x08		/*    Halt Monitor Enable */
#define    BME   0x04		/*    Bus Monitor External Enable */
#define    BMT   0x03		/*    Bus Monitor Timing */



#define PICR SIM_VOLATILE_USHORT_POINTER(0x22 + SIM_CRB)
				/* Periodic Interrupt Control Reg. */
#define    PIRQL 0x0700		/*    Periodic Interrupt Request Level */
#define    PIV   0x00ff		/*    Periodic Interrupt Level */



#define PITR SIM_VOLATILE_USHORT_POINTER(0x24 + SIM_CRB)
				/* Periodic Interrupt Timer Register */
#define    PTP   0x0100		/*    Periodic Timer Prescaler Control */
#define    PITM  0x00ff		/*    Periodic Interrupt Timing Modulus */



#define SWSR SIM_VOLATILE_UCHAR_POINTER(0x27 + SIM_CRB)
				/* Software Service Register */
/* write 0x55 then 0xaa to service the software watchdog */



#define TSTMSRA SIM_VOLATILE_USHORT_POINTER(0x30 + SIM_CRB)
				/* Test Module Master Shift A */
#define TSTMSRB SIM_VOLATILE_USHORT_POINTER(0x32 + SIM_CRB)
				/* Test Module Master Shift A */
#define TSTSC SIM_VOLATILE_USHORT_POINTER(0x34 + SIM_CRB)
				/* Test Module Shift Count */
#define TSTRC SIM_VOLATILE_USHORT_POINTER(0x36 + SIM_CRB)
				/* Test Module Repetition Counter */
#define CREG SIM_VOLATILE_USHORT_POINTER(0x38 + SIM_CRB)
				/* Test Module Control */
#define DREG SIM_VOLATILE_USHORT_POINTER(0x3a + SIM_CRB)
				/* Test Module Distributed */
/* Used only for factor testing */



#define PORTC SIM_VOLATILE_UCHAR_POINTER(0x41 + SIM_CRB)
				/* Port C Data */



#define CSPAR0 SIM_VOLATILE_USHORT_POINTER(0x44 + SIM_CRB)
				/* Chip Select Pin Assignment
				   Resgister 0 */
/* CSPAR0 contains seven two-bit fields that determine the functions
   of corresponding chip-select pins. CSPAR0[15:14] are not
   used. These bits always read zero; write have no effect. CSPAR0 bit
   1 always reads one; writes to CSPAR0 bit 1 have no effect. */
#define CSPAR1 SIM_VOLATILE_USHORT_POINTER(0x46 + SIM_CRB)
				/* Chip Select Pin Assignment
				   Register 1 */
/* CSPAR1 contains five two-bit fields that determine the finctions of
   corresponding chip-select pins. CSPAR1[15:10] are not used. These
   bits always read zero; writes have no effect. */
/*
 *
 *                      Bit Field  |  Description
 *                     ------------+---------------
 *                         00      | Discrete Output
 *                         01      | Alternate Function
 *                         10      | Chip Select (8-bit port)
 *                         11      | Chip Select (16-bit port)
 */
#define DisOut 0x0
#define AltFun 0x1
#define CS8bit 0x2
#define CS16bit 0x3
/*
 *
 * CSPARx Field    |Chip Select Signal  |  Alternate Signal  |  Discrete Output
 *-----------------+--------------------+--------------------+---------------*/
#define CS_5    12 /*     !CS5          |         FC2        |       PC2     */
#define CS_4    10 /*     !CS4          |         FC1        |       PC1     */
#define CS_3     8 /*     !CS3          |         FC0        |       PC0     */
#define CS_2     6 /*     !CS2          |       !BGACK       |               */
#define CS_1     4 /*     !CS1          |         !BG        |               */
#define CS_0     2 /*     !CS0          |         !BR        |               */
#define CSBOOT   0 /*     !CSBOOT       |                    |               */
/*                 |                    |                    |               */
#define CS_10    8 /*     !CS10         |       ADDR23       |      ECLK     */
#define CS_9     6 /*     !CS9          |       ADDR22       |       PC6     */
#define CS_8     4 /*     !CS8          |       ADDR21       |       PC5     */
#define CS_7     2 /*     !CS7          |       ADDR20       |       PC4     */
#define CS_6     0 /*     !CS6          |       ADDR19       |       PC3     */

#define BS_2K 0x0
#define BS_8K 0x1
#define BS_16K 0x2
#define BS_64K 0x3
#define BS_128K 0x4
#define BS_256K 0x5
#define BS_512K 0x6
#define BS_1M 0x7

#define CSBARBT SIM_VOLATILE_USHORT_POINTER(0x48 + SIM_CRB)
#define CSBAR0 SIM_VOLATILE_USHORT_POINTER(0x4c + SIM_CRB)
#define CSBAR1 SIM_VOLATILE_USHORT_POINTER(0x50 + SIM_CRB)
#define CSBAR2 SIM_VOLATILE_USHORT_POINTER(0x54 + SIM_CRB)
#define CSBAR3 SIM_VOLATILE_USHORT_POINTER(0x58 + SIM_CRB)
#define CSBAR4 SIM_VOLATILE_USHORT_POINTER(0x5c + SIM_CRB)
#define CSBAR5 SIM_VOLATILE_USHORT_POINTER(0x60 + SIM_CRB)
#define CSBAR6 SIM_VOLATILE_USHORT_POINTER(0x64 + SIM_CRB)
#define CSBAR7 SIM_VOLATILE_USHORT_POINTER(0x68 + SIM_CRB)
#define CSBAR8 SIM_VOLATILE_USHORT_POINTER(0x6c + SIM_CRB)
#define CSBAR9 SIM_VOLATILE_USHORT_POINTER(0x70 + SIM_CRB)
#define CSBAR10 SIM_VOLATILE_USHORT_POINTER(0x74 + SIM_CRB)

#define MODE 0x8000
#define Disable 0
#define LowerByte 0x2000
#define UpperByte 0x4000
#define BothBytes 0x6000
#define ReadOnly 0x0800
#define WriteOnly 0x1000
#define ReadWrite 0x1800
#define SyncAS 0x0
#define SyncDS 0x0400

#define WaitStates_0 (0x0 << 6)
#define WaitStates_1 (0x1 << 6)
#define WaitStates_2 (0x2 << 6)
#define WaitStates_3 (0x3 << 6)
#define WaitStates_4 (0x4 << 6)
#define WaitStates_5 (0x5 << 6)
#define WaitStates_6 (0x6 << 6)
#define WaitStates_7 (0x7 << 6)
#define WaitStates_8 (0x8 << 6)
#define WaitStates_9 (0x9 << 6)
#define WaitStates_10 (0xa << 6)
#define WaitStates_11 (0xb << 6)
#define WaitStates_12 (0xc << 6)
#define WaitStates_13 (0xd << 6)
#define FastTerm (0xe << 6)
#define External (0xf << 6)

#define CPUSpace (0x0 << 4)
#define UserSpace (0x1 << 4)
#define SupSpace (0x2 << 4)
#define UserSupSpace (0x3 << 4)

#define IPLevel_any 0x0
#define IPLevel_1 0x2
#define IPLevel_2 0x4
#define IPLevel_3 0x6
#define IPLevel_4 0x8
#define IPLevel_5 0xa
#define IPLevel_6 0xc
#define IPLevel_7 0xe

#define AVEC 1

#define CSORBT SIM_VOLATILE_USHORT_POINTER(0x4a + SIM_CRB)
#define CSOR0 SIM_VOLATILE_USHORT_POINTER(0x4e + SIM_CRB)
#define CSOR1 SIM_VOLATILE_USHORT_POINTER(0x52 + SIM_CRB)
#define CSOR2 SIM_VOLATILE_USHORT_POINTER(0x56 + SIM_CRB)
#define CSOR3 SIM_VOLATILE_USHORT_POINTER(0x5a + SIM_CRB)
#define CSOR4 SIM_VOLATILE_USHORT_POINTER(0x5e + SIM_CRB)
#define CSOR5 SIM_VOLATILE_USHORT_POINTER(0x62 + SIM_CRB)
#define CSOR6 SIM_VOLATILE_USHORT_POINTER(0x66 + SIM_CRB)
#define CSOR7 SIM_VOLATILE_USHORT_POINTER(0x6a + SIM_CRB)
#define CSOR8 SIM_VOLATILE_USHORT_POINTER(0x6e + SIM_CRB)
#define CSOR9 SIM_VOLATILE_USHORT_POINTER(0x72 + SIM_CRB)
#define CSOR10 SIM_VOLATILE_USHORT_POINTER(0x76 + SIM_CRB)

#endif /* _RTEMS_M68K_SIM_H */
