/*
 *  This include file contains information pertaining to the Hitachi SH
 *  processor.
 *
 *  Authors: Ralf Corsepius (corsepiu@faw.uni-ulm.de) and
 *           Bernd Becker (becker@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *  COPYRIGHT (c) 1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Modified to reflect isp entries for sh7045 processor:
 *  John M. Mills (jmills@tga.com)
 *  TGA Technologies, Inc.
 *  100 Pinnacle Way, Suite 140
 *  Norcross, GA 30071 U.S.A.
 *
 *
 *  This modified file may be copied and distributed in accordance
 *  the above-referenced license. It is provided for critique and
 *  developmental purposes without any warranty nor representation
 *  by the authors or by TGA Technologies.
 */

#ifndef __CPU_ISPS_H
#define __CPU_ISPS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/types.h>

extern void __ISR_Handler( uint32_t   vector );


/*
 * interrupt vector table offsets
 */
#define NMI_ISP_V 11
#define USB_ISP_V 12
#define IRQ0_ISP_V 64
#define IRQ1_ISP_V 65
#define IRQ2_ISP_V 66
#define IRQ3_ISP_V 67
#define IRQ4_ISP_V 68
#define IRQ5_ISP_V 69
#define IRQ6_ISP_V 70
#define IRQ7_ISP_V 71
#define DMA0_ISP_V 72
#define DMA1_ISP_V 76
#define DMA2_ISP_V 80
#define DMA3_ISP_V 84

#define MTUA0_ISP_V 88
#define MTUB0_ISP_V 89
#define MTUC0_ISP_V 90
#define MTUD0_ISP_V 91
#define MTUV0_ISP_V 92

#define MTUA1_ISP_V 96
#define MTUB1_ISP_V 97
#define MTUV1_ISP_V 100
#define MTUU1_ISP_V 101

#define MTUA2_ISP_V 104
#define MTUB2_ISP_V 105
#define MTUV2_ISP_V 108
#define MTUU2_ISP_V 109

#define MTUA3_ISP_V 112
#define MTUB3_ISP_V 113
#define MTUC3_ISP_V 114
#define MTUD3_ISP_V 115
#define MTUV3_ISP_V 116

#define MTUA4_ISP_V 120
#define MTUB4_ISP_V 121
#define MTUC4_ISP_V 122
#define MTUD4_ISP_V 123
#define MTUV4_ISP_V 124

#define ERI0_ISP_V 128
#define RXI0_ISP_V 129
#define TXI0_ISP_V 130
#define TEI0_ISP_V 131

#define ERI1_ISP_V 132
#define RXI1_ISP_V 133
#define TXI1_ISP_V 134
#define TEI1_ISP_V 135

#define ADI0_ISP_V 136
#define ADI1_ISP_V 137
#define DTC_ISP_V 140  /* Data Transfer Controller */
#define CMT0_ISP_V 144 /* Compare Match Timer */
#define CMT1_ISP_V 148
#define WDT_ISP_V 152  /* Wtachdog Timer */
#define CMI_ISP_V 153  /* BSC RAS interrupt */
#define OEI_ISP_V 156  /* I/O Port */
#define DREF_ISP_V CMI_ISP_V /* DRAM Refresh from BSC */
#if 0
#define PRT_ISP_V /* parity error - no equivalent */
#endif

/* dummy ISP */
extern void _dummy_isp( void );

/* Non Maskable Interrupt */
extern void _nmi_isp( void );

/* User Break Controller */
extern void _usb_isp( void );

/* External interrupts 0-7 */
extern void _irq0_isp( void );
extern void _irq1_isp( void );
extern void _irq2_isp( void );
extern void _irq3_isp( void );
extern void _irq4_isp( void );
extern void _irq5_isp( void );
extern void _irq6_isp( void );
extern void _irq7_isp( void );

/* DMA - Controller */
extern void _dma0_isp( void );
extern void _dma1_isp( void );
extern void _dma2_isp( void );
extern void _dma3_isp( void );

/* Interrupt Timer Unit */
/* Timer 0 */
extern void _mtua0_isp( void );
extern void _mtub0_isp( void );
extern void _mtuc0_isp( void );
extern void _mtud0_isp( void );
extern void _mtuv0_isp( void );
/* Timer 1 */
extern void _mtua1_isp( void );
extern void _mtub1_isp( void );
extern void _mtuv1_isp( void );
extern void _mtuu1_isp( void );
/* Timer 2 */
extern void _mtua2_isp( void );
extern void _mtub2_isp( void );
extern void _mtuv2_isp( void );
extern void _mtuu2_isp( void );
/* Timer 3 */
extern void _mtua3_isp( void );
extern void _mtub3_isp( void );
extern void _mtuc3_isp( void );
extern void _mtud3_isp( void );
extern void _mtuv3_isp( void );
/* Timer 4 */
extern void _mtua4_isp( void );
extern void _mtub4_isp( void );
extern void _mtuc4_isp( void );
extern void _mtud4_isp( void );
extern void _mtuv4_isp( void );

/* serial interfaces */
extern void _eri0_isp( void );
extern void _rxi0_isp( void );
extern void _txi0_isp( void );
extern void _tei0_isp( void );
extern void _eri1_isp( void );
extern void _rxi1_isp( void );
extern void _txi1_isp( void );
extern void _tei1_isp( void );

/* ADC */
extern void _adi0_isp( void );
extern void _adi1_isp( void );

/* Data Transfer Controller */
extern void _dtci_isp( void );

/* Compare Match Timer */
extern void _cmt0_isp( void );
extern void _cmt1_isp( void );

/* Watchdog Timer */
extern void _wdt_isp( void );

/* DRAM refresh control unit of bus state controller */
extern void _bsc_isp( void );

/* I/O Port */
extern void _oei_isp( void );

/* Parity Control Unit of the Bus State Controllers */
/* extern void _prt_isp( void ); */

#ifdef __cplusplus
}
#endif

#endif
