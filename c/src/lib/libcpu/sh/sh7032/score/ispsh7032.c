/*
 * This file contains the isp frames for the user interrupts.
 * From these procedures __ISR_Handler is called with the vector number
 * as argument.
 *
 * __ISR_Handler is kept in a separate file (cpu_asm.c), because a bug in
 * some releases of gcc doesn't properly handle #pragma interrupt, if a
 * file contains both isrs and normal functions.
 *
 *  Authors: Ralf Corsepius (corsepiu@faw.uni-ulm.de) and
 *           Bernd Becker (becker@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE
 *
 *
 *  COPYRIGHT (c) 1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/system.h>
#include <rtems/score/types.h>
#include <rtems/score/ispsh7032.h>

/*
 * This is an exception vector table
 *
 * It has the same structure like the actual vector table (vectab)
 */
proc_ptr _Hardware_isr_Table[256]={
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp,
_nmi_isp, _usb_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp,
/* trapa 0 -31 */
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
/* irq 64 ... */
_irq0_isp, _irq1_isp, _irq2_isp, _irq3_isp,
_irq4_isp, _irq5_isp, _irq6_isp, _irq7_isp,
_dma0_isp, _dummy_isp, _dma1_isp, _dummy_isp,
_dma2_isp, _dummy_isp, _dma3_isp, _dummy_isp,
_imia0_isp, _imib0_isp, _ovi0_isp, _dummy_isp,
_imia1_isp, _imib1_isp, _ovi1_isp, _dummy_isp,
_imia2_isp, _imib2_isp, _ovi2_isp, _dummy_isp,
_imia3_isp, _imib3_isp, _ovi3_isp, _dummy_isp,
_imia4_isp, _imib4_isp, _ovi4_isp, _dummy_isp,
_eri0_isp, _rxi0_isp, _txi0_isp, _tei0_isp,
_eri1_isp, _rxi1_isp, _txi1_isp, _tei1_isp,
_prt_isp, _adu_isp, _dummy_isp, _dummy_isp,
_wdt_isp,
/* 113 */ _dref_isp
};

#define Str(a)#a

/*
 * Some versions of gcc and all version of egcs at least until egcs-1.1b
 * are not able to handle #pragma interrupt correctly if more than 1 isr is
 * contained in a file and when optimizing.
 * We try to work around this problem by using the macro below.
 */
#define isp( name, number, func)\
__asm__ (".global _"Str(name)"\n\t" \
     "_"Str(name)":       \n\t" \
     "    mov.l r0,@-r15   \n\t" \
     "    mov.l r1,@-r15   \n\t" \
     "    mov.l r2,@-r15   \n\t" \
     "    mov.l r3,@-r15   \n\t" \
     "    mov.l r4,@-r15   \n\t" \
     "    mov.l r5,@-r15   \n\t" \
     "    mov.l r6,@-r15   \n\t" \
     "    mov.l r7,@-r15   \n\t" \
     "    mov.l r14,@-r15  \n\t" \
     "    sts.l pr,@-r15   \n\t" \
     "    sts.l mach,@-r15 \n\t" \
     "    sts.l macl,@-r15 \n\t" \
     "    mov r15,r14      \n\t" \
     "    mov.l "Str(name)"_k, r1\n\t" \
     "    jsr @r1           \n\t" \
     "    mov #"Str(number)", r4\n\t" \
     "    mov   r14,r15    \n\t" \
     "    lds.l @r15+,macl \n\t" \
     "    lds.l @r15+,mach \n\t" \
     "    lds.l @r15+,pr   \n\t" \
     "    mov.l @r15+,r14  \n\t" \
     "    mov.l @r15+,r7   \n\t" \
     "    mov.l @r15+,r6   \n\t" \
     "    mov.l @r15+,r5   \n\t" \
     "    mov.l @r15+,r4   \n\t" \
     "    mov.l @r15+,r3   \n\t" \
     "    mov.l @r15+,r2   \n\t" \
     "    mov.l @r15+,r1   \n\t" \
     "    mov.l @r15+,r0   \n\t" \
     "    rte              \n\t" \
     "    nop              \n\t" \
     "    .align 2         \n\t" \
     #name"_k: \n\t" \
     ".long "Str(func));

/************************************************
 * Dummy interrupt service procedure for
 * interrupts being not allowed --> Trap 34
 ************************************************/
__asm__ (" .section .text\n\
.global __dummy_isp\n\
__dummy_isp:\n\
      mov.l r14,@-r15\n\
      mov   r15, r14\n\
      trapa #34\n\
      mov.l @r15+,r14\n\
      rte\n\
      nop");

/*****************************
 * Non maskable interrupt
 *****************************/
isp( _nmi_isp, NMI_ISP_V, ___ISR_Handler);

/*****************************
 * User break controller
 *****************************/
isp( _usb_isp, USB_ISP_V, ___ISR_Handler);

/*****************************
 *  External interrupts 0-7
 *****************************/
isp( _irq0_isp, IRQ0_ISP_V, ___ISR_Handler);
isp( _irq1_isp, IRQ1_ISP_V, ___ISR_Handler);
isp( _irq2_isp, IRQ2_ISP_V, ___ISR_Handler);
isp( _irq3_isp, IRQ3_ISP_V, ___ISR_Handler);
isp( _irq4_isp, IRQ4_ISP_V, ___ISR_Handler);
isp( _irq5_isp, IRQ5_ISP_V, ___ISR_Handler);
isp( _irq6_isp, IRQ6_ISP_V, ___ISR_Handler);
isp( _irq7_isp, IRQ7_ISP_V, ___ISR_Handler);

/*****************************
 * DMA - controller
 *****************************/
isp( _dma0_isp, DMA0_ISP_V, ___ISR_Handler);
isp( _dma1_isp, DMA1_ISP_V, ___ISR_Handler);
isp( _dma2_isp, DMA2_ISP_V, ___ISR_Handler);
isp( _dma3_isp, DMA3_ISP_V, ___ISR_Handler);


/*****************************
 * Interrupt timer unit
 *****************************/

/*****************************
 * Timer 0
 *****************************/
isp( _imia0_isp, IMIA0_ISP_V, ___ISR_Handler);
isp( _imib0_isp, IMIB0_ISP_V, ___ISR_Handler);
isp( _ovi0_isp,  OVI0_ISP_V, ___ISR_Handler);

/*****************************
 * Timer 1
 *****************************/
isp( _imia1_isp, IMIA1_ISP_V, ___ISR_Handler);
isp( _imib1_isp, IMIB1_ISP_V, ___ISR_Handler);
isp( _ovi1_isp,  OVI1_ISP_V, ___ISR_Handler);

/*****************************
 * Timer 2
 *****************************/
isp( _imia2_isp, IMIA2_ISP_V, ___ISR_Handler);
isp( _imib2_isp, IMIB2_ISP_V, ___ISR_Handler);
isp( _ovi2_isp,  OVI2_ISP_V, ___ISR_Handler);

/*****************************
 * Timer 3
 *****************************/
isp( _imia3_isp, IMIA3_ISP_V, ___ISR_Handler);
isp( _imib3_isp, IMIB3_ISP_V, ___ISR_Handler);
isp( _ovi3_isp,  OVI3_ISP_V, ___ISR_Handler);

/*****************************
 * Timer 4
 *****************************/
isp( _imia4_isp, IMIA4_ISP_V, ___ISR_Handler);
isp( _imib4_isp, IMIB4_ISP_V, ___ISR_Handler);
isp( _ovi4_isp,  OVI4_ISP_V, ___ISR_Handler);


/*****************************
 * Serial interfaces
 *****************************/

/*****************************
 * Serial interface 0
 *****************************/
isp( _eri0_isp,  ERI0_ISP_V, ___ISR_Handler);
isp( _rxi0_isp,  RXI0_ISP_V, ___ISR_Handler);
isp( _txi0_isp,  TXI0_ISP_V, ___ISR_Handler);
isp( _tei0_isp,  TEI0_ISP_V, ___ISR_Handler);

/*****************************
 * Serial interface 1
 *****************************/
isp( _eri1_isp,  ERI1_ISP_V, ___ISR_Handler);
isp( _rxi1_isp,  RXI1_ISP_V, ___ISR_Handler);
isp( _txi1_isp,  TXI1_ISP_V, ___ISR_Handler);
isp( _tei1_isp,  TEI1_ISP_V, ___ISR_Handler);


/*****************************
 * Parity control unit of
 * the bus state controller
 *****************************/
isp( _prt_isp,  PRT_ISP_V, ___ISR_Handler);


/******************************
 * Analog digital converter
 * ADC
 ******************************/
isp( _adu_isp,  ADU_ISP_V, ___ISR_Handler);


/******************************
 *  Watchdog timer
 ******************************/
isp( _wdt_isp,  WDT_ISP_V, ___ISR_Handler);


/******************************
 * DRAM refresh control unit
 * of bus state controller
 ******************************/
isp( _dref_isp,  DREF_ISP_V, ___ISR_Handler);
