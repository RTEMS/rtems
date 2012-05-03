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
#define DMA1_ISP_V 74
#define DMA2_ISP_V 76
#define DMA3_ISP_V 78

#define IMIA0_ISP_V 80
#define IMIB0_ISP_V 81
#define OVI0_ISP_V 82

#define IMIA1_ISP_V 84
#define IMIB1_ISP_V 85
#define OVI1_ISP_V 86

#define IMIA2_ISP_V 88
#define IMIB2_ISP_V 89
#define OVI2_ISP_V 90

#define IMIA3_ISP_V 92
#define IMIB3_ISP_V 93
#define OVI3_ISP_V 94

#define IMIA4_ISP_V 96
#define IMIB4_ISP_V 97
#define OVI4_ISP_V 98

#define ERI0_ISP_V 100
#define RXI0_ISP_V 101
#define TXI0_ISP_V 102
#define TEI0_ISP_V 103

#define ERI1_ISP_V 104
#define RXI1_ISP_V 105
#define TXI1_ISP_V 106
#define TEI1_ISP_V 107

#define PRT_ISP_V 108
#define ADU_ISP_V 109
#define WDT_ISP_V 112
#define DREF_ISP_V 113


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
extern void _imia0_isp( void );
extern void _imib0_isp( void );
extern void _ovi0_isp( void );
/* Timer 1 */
extern void _imia1_isp( void );
extern void _imib1_isp( void );
extern void _ovi1_isp( void );
/* Timer 2 */
extern void _imia2_isp( void );
extern void _imib2_isp( void );
extern void _ovi2_isp( void );
/* Timer 3 */
extern void _imia3_isp( void );
extern void _imib3_isp( void );
extern void _ovi3_isp( void );
/* Timer 4 */
extern void _imia4_isp( void );
extern void _imib4_isp( void );
extern void _ovi4_isp( void );

/* seriell interfaces */
extern void _eri0_isp( void );
extern void _rxi0_isp( void );
extern void _txi0_isp( void );
extern void _tei0_isp( void );
extern void _eri1_isp( void );
extern void _rxi1_isp( void );
extern void _txi1_isp( void );
extern void _tei1_isp( void );

/* Parity Control Unit of the Bus State Controllers */
extern void _prt_isp( void );

/* ADC */
extern void _adu_isp( void );

/* Watchdog Timer */
extern void _wdt_isp( void );

/* DRAM refresh control unit of bus state controller */
extern void _dref_isp( void );

#ifdef __cplusplus
}
#endif

#endif
