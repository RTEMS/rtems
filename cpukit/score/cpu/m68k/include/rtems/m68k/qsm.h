/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Motorola M68K Queued Serial Module
 *
 * The QSM contains two serial interfaces: (a) the queued serial
 * peripheral interface (QSPI) and the serial communication interface
 * (SCI). The QSPI provides peripheral expansion and/or interprocessor
 * communication through a full-duplex, synchronous, three-wire bus. A
 * self contained RAM queue permits serial data transfers without CPU
 * intervention and automatic continuous sampling. The SCI provides a
 * standard non-return to zero mark/space format with wakeup functions
 * to allow the CPU to run uninterrupted until woken
 *
 * For more information, refer to Motorola's "Modular Microcontroller
 * Family Queued Serial Module Reference Manual" (Motorola document
 * QSMRM/AD).
 */

/*
 * Copyright (c) 1995 John Gwynne <john.gwynne@matrixresearch.com>
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

#ifndef _RTEMS_M68K_QSM_H
#define _RTEMS_M68K_QSM_H

/* SAM-- shift and mask */
#undef  SAM
#define SAM(a,b,c) ((a << b) & c)


/* QSM_CRB (QSM Control Register Block) base address of the QSM
   control registers */
#if SIM_MM == 0
#define QSM_CRB 0x7ffc00
#else
#undef SIM_MM
#define SIM_MM 1
#define QSM_CRB 0xfffc00
#endif


#define QSMCR (volatile unsigned short int * const)(0x00 + QSM_CRB)
				/* QSM Configuration Register */
#define    STOP 0x8000		/*    Stop Enable */
#define    FRZ  0x6000		/*    Freeze Control */
#define    SUPV 0x0080		/*    Supervisor/Unrestricted */
#define	   IARB 0x000f		/*    Inerrupt Arbitration */


#define QTEST (volatile unsigned short int * const)(0x02 + QSM_CRB)
				/* QSM Test Register */
/* Used only for factor testing */


#define QILR (volatile unsigned char * const)(0x04 + QSM_CRB)
				/* QSM Interrupt Level Register */
#define    ILQSPI 0x38		/*    Interrupt Level for QSPI */
#define    ILSCI  0x07		/*    Interrupt Level for SCI */


#define QIVR (volatile unsigned char * const)(0x05 + QSM_CRB)
				/* QSM Interrupt Vector Register */
#define    INTV   0xff		/*    Interrupt Vector Number */


#define SCCR0 (volatile unsigned short int * const)(0x08 + QSM_CRB)
				/* SCI Control Register 0 */
#define    SCBR   0x1fff	/*    SCI Baud Rate */


#define SCCR1 (volatile unsigned short int * const)(0x0a + QSM_CRB)
				/* SCI Control Register 1 */
#define    LOOPS  0x4000	/*    Loop Mode */
#define    WOMS   0x2000	/*    Wired-OR Mode for SCI Pins */
#define    ILT    0x1000	/*    Idle-Line Detect Type */
#define    PT     0x0800	/*    Parity Type */
#define    PE     0x0400	/*    Parity Enable */
#define    M      0x0200	/*    Mode Select */
#define    WAKE   0x0100	/*    Wakeup by Address Mark */
#define    TIE    0x0080	/*    Transmit Complete Interrupt Enable */
#define    TCIE   0x0040	/*    Transmit Complete Interrupt Enable */
#define    RIE    0x0020	/*    Receiver Interrupt Enable */
#define    ILIE   0x0010	/*    Idle-Line Interrupt Enable */
#define    TE     0x0008	/*    Transmitter Enable */
#define    RE     0x0004	/*    Receiver Enable */
#define    RWU    0x0002	/*    Receiver Wakeup */
#define    SBK    0x0001	/*    Send Break */


#define SCSR (volatile unsigned short int * const)(0x0c + QSM_CRB)
				/* SCI Status Register */
#define    TDRE   0x0100	/*    Transmit Data Register Empty */
#define    TC     0x0080	/*    Transmit Complete */
#define    RDRF   0x0040	/*    Receive Data Register Full */
#define    RAF    0x0020	/*    Receiver Active */
#define    IDLE   0x0010	/*    Idle-Line Detected */
#define    OR     0x0008	/*    Overrun Error */
#define    NF     0x0004	/*    Noise Error Flag */
#define    FE     0x0002	/*    Framing Error */
#define    PF     0x0001	/*    Parity Error */


#define SCDR (volatile unsigned short int * const)(0x0e + QSM_CRB)
				/* SCI Data Register */


#define PORTQS (volatile unsigned char * const)(0x15 + QSM_CRB)
				/* Port QS Data Register */

#define PQSPAR (volatile unsigned char * const)(0x16 + QSM_CRB)
				/* PORT QS Pin Assignment Rgister */
/* Any bit cleared (zero) defines the corresponding pin to be an I/O
   pin. Any bit set defines the corresponding pin to be a QSPI
   signal. */
/* note: PQS2 is a digital I/O pin unless the SPI is enabled in which
   case it becomes the SPI serial clock SCK. */
/* note: PQS7 is a digital I/O pin unless the SCI transmitter is
   enabled in which case it becomes the SCI serial output TxD. */
#define QSMFun 0x0
#define QSMDis 0x1
/*
 * PQSPAR Field     | QSM Function | Discrete I/O pin
 *------------------+--------------+------------------   */
#define PQSPA0   0  /*   MISO      |      PQS0           */
#define PQSPA1   1  /*   MOSI      |      PQS1           */
#define PQSPA2   2  /*   SCK       |      PQS2 (see note)*/
#define PQSPA3   3  /*   PCSO/!SS  |      PQS3           */
#define PQSPA4   4  /*   PCS1      |      PQS4           */
#define PQSPA5   5  /*   PCS2      |      PQS5           */
#define PQSPA6   6  /*   PCS3      |      PQS6           */
#define PQSPA7   7  /*   TxD       |      PQS7 (see note)*/


#define DDRQS  (volatile unsigned char * const)(0x17 + QSM_CRB)
				/* PORT QS Data Direction Register */
/* Clearing a bit makes the corresponding pin an input; setting a bit
   makes the pin an output. */


#define SPCR0 (volatile unsigned short int * const)(0x18 + QSM_CRB)
				/* QSPI Control Register 0 */
#define    MSTR   0x8000	/*    Master/Slave Mode Select */
#define    WOMQ   0x4000	/*    Wired-OR Mode for QSPI Pins */
#define    BITS   0x3c00	/*    Bits Per Transfer */
#define    CPOL   0x0200	/*    Clock Polarity */
#define    CPHA   0x0100	/*    Clock Phase */
#define    SPBR   0x00ff	/*    Serial Clock Baud Rate */


#define SPCR1 (volatile unsigned short int * const)(0x1a + QSM_CRB)
				/* QSPI Control Register 1 */
#define    SPE    0x8000	/*    QSPI Enable */
#define    DSCKL  0x7f00	/*    Delay before SCK */
#define    DTL    0x00ff	/*    Length of Delay after Transfer */


#define SPCR2 (volatile unsigned short int * const)(0x1c + QSM_CRB)
				/* QSPI Control Register 2 */
#define    SPIFIE 0x8000	/*    SPI Finished Interrupt Enable */
#define    WREN   0x4000	/*    Wrap Enable */
#define    WRTO   0x2000	/*    Wrap To */
#define    ENDQP  0x0f00	/*    Ending Queue Pointer */
#define    NEWQP  0x000f	/*    New Queue Pointer Value */


#define SPCR3 (volatile unsigned char * const)(0x1e + QSM_CRB)
				/* QSPI Control Register 3 */
#define    LOOPQ  0x0400	/*    QSPI Loop Mode */
#define    HMIE   0x0200	/*    HALTA and MODF Interrupt Enable */
#define    HALT   0x0100	/*    Halt */


#define SPSR (volatile unsigned char * const)(0x1f + QSM_CRB)
				/* QSPI Status Register */
#define    SPIF   0x0080	/*    QSPI Finished Flag */
#define    MODF   0x0040	/*    Mode Fault Flag */
#define    HALTA  0x0020	/*    Halt Acknowlwdge Flag */
#define    CPTQP  x0000f	/*    Completed Queue Pointer */

#define QSPIRR (volatile unsigned char * const)(0x100 + QSM_CRB)
				/* QSPI Receive Data RAM */
#define QSPITR (volatile unsigned char * const)(0x120 + QSM_CRB)
				/* QSPI Transmit Data RAM */
#define QSPIcR (volatile unsigned char * const)(0x140 + QSM_CRB)
				/* QSPI Command RAM */

#endif /* _RTEMS_M68K_QSM_H */
