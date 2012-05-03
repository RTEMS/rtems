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
 *
 *	Modified to reflect isp entries for sh7045 processor:
 *	John M. Mills (jmills@tga.com)
 *	TGA Technologies, Inc.
 *      100 Pinnacle Way, Suite 140
 *	Norcross, GA 30071 U.S.A.
 *	August, 1999
 *
 *	This modified file may be copied and distributed in accordance
 *	the above-referenced license. It is provided for critique and
 *	developmental purposes without any warranty nor representation
 *	by the authors or by TGA Technologies.
 */

#include <rtems/system.h>
#include <rtems/score/types.h>

/*
 * This is a exception vector table
 *
 * It has the same structure as the actual vector table (vectab)
 */


/* SH-2 ISR Table */
#include <rtems/score/ispsh7045.h>

proc_ptr _Hardware_isr_Table[256]={
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,		/* PWRon Reset, Maual Reset,...*/
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp,
_nmi_isp, _usb_isp,                               /* irq 11, 12*/
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
_irq0_isp, _irq1_isp, _irq2_isp, _irq3_isp,   /* external H/W: irq 64-71 */
_irq4_isp, _irq5_isp, _irq6_isp, _irq7_isp,
_dma0_isp, _dummy_isp, _dummy_isp, _dummy_isp, /* DMAC: irq 72-87*/
_dma1_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dma2_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dma3_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_mtua0_isp, _mtub0_isp, _mtuc0_isp, _mtud0_isp, /* MTUs: irq 88-127 */
_mtuv0_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_mtua1_isp, _mtub1_isp, _dummy_isp, _dummy_isp,
_mtuv1_isp, _mtuu1_isp, _dummy_isp, _dummy_isp,
_mtua2_isp, _mtub2_isp, _dummy_isp, _dummy_isp,
_mtuv2_isp, _mtuu2_isp, _dummy_isp, _dummy_isp,
_mtua3_isp, _mtub3_isp, _mtuc3_isp, _mtud3_isp,
_mtuv3_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_mtua4_isp, _mtub4_isp, _mtuc4_isp, _mtud4_isp,
_mtuv4_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_eri0_isp, _rxi0_isp, _txi0_isp, _tei0_isp, /* SCI0-1: irq 128-135*/
_eri1_isp, _rxi1_isp, _txi1_isp, _tei1_isp,
_adi0_isp, _adi1_isp, _dummy_isp, _dummy_isp, /* ADC0-1: irq 136-139*/
_dtci_isp, _dummy_isp, _dummy_isp, _dummy_isp, /* DTU: irq 140-143 */
_cmt0_isp, _dummy_isp, _dummy_isp, _dummy_isp, /* CMT0-1: irq 144-151 */
_cmt1_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_wdt_isp, /* WDT: irq 152*/
_bsc_isp, _dummy_isp, _dummy_isp, /* BSC: irq 153-155*/
_oei_isp, /* I/O Port: irq 156*/
};

#define Str(a)#a

/*
 * Some versions of gcc and all version of egcs at least until egcs-1.1b
 * are not able to handle #pragma interrupt correctly if more than 1 isr is
 * contained in a file and when optimizing.
 * We try to work around this problem by using the macro below.
 */
#define isp( name, number, func)\
__asm__ (".global _"Str(name)"\n\t"\
     "_"Str(name)":       \n\t"\
     "    mov.l r0,@-r15   \n\t"\
     "    mov.l r1,@-r15   \n\t"\
     "    mov.l r2,@-r15   \n\t"\
     "    mov.l r3,@-r15   \n\t"\
     "    mov.l r4,@-r15   \n\t"\
     "    mov.l r5,@-r15   \n\t"\
     "    mov.l r6,@-r15   \n\t"\
     "    mov.l r7,@-r15   \n\t"\
     "    mov.l r14,@-r15  \n\t"\
     "    sts.l pr,@-r15   \n\t"\
     "    sts.l mach,@-r15 \n\t"\
     "    sts.l macl,@-r15 \n\t"\
     "    mov r15,r14      \n\t"\
     "    mov.l "Str(name)"_v, r2 \n\t"\
     "    mov.l "Str(name)"_k, r1\n\t"\
     "    jsr @r1           \n\t"\
     "    mov   r2,r4      \n\t"\
     "    mov   r14,r15    \n\t"\
     "    lds.l @r15+,macl \n\t"\
     "    lds.l @r15+,mach \n\t"\
     "    lds.l @r15+,pr   \n\t"\
     "    mov.l @r15+,r14  \n\t"\
     "    mov.l @r15+,r7   \n\t"\
     "    mov.l @r15+,r6   \n\t"\
     "    mov.l @r15+,r5   \n\t"\
     "    mov.l @r15+,r4   \n\t"\
     "    mov.l @r15+,r3   \n\t"\
     "    mov.l @r15+,r2   \n\t"\
     "    mov.l @r15+,r1   \n\t"\
     "    mov.l @r15+,r0   \n\t"\
     "    rte              \n\t"\
     "    nop              \n\t"\
     "    .align 2         \n\t"\
     #name"_k: \n\t"\
     ".long "Str(func)"\n\t"\
     #name"_v: \n\t"\
     ".long "Str(number));

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

/*******************************************************************
 *     ISP Vector Table for sh7045 family of processors            *
 *******************************************************************/


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
 * Match timer unit
 *****************************/

/*****************************
 * Timer 0
 *****************************/
isp( _mtua0_isp, MTUA0_ISP_V, ___ISR_Handler);
isp( _mtub0_isp, MTUB0_ISP_V, ___ISR_Handler);
isp( _mtuc0_isp, MTUC0_ISP_V, ___ISR_Handler);
isp( _mtud0_isp, MTUD0_ISP_V, ___ISR_Handler);
isp( _mtuv0_isp, MTUV0_ISP_V, ___ISR_Handler);

/*****************************
 * Timer 1
 *****************************/
isp( _mtua1_isp, MTUA1_ISP_V, ___ISR_Handler);
isp( _mtub1_isp, MTUB1_ISP_V, ___ISR_Handler);
isp( _mtuv1_isp, MTUV1_ISP_V, ___ISR_Handler);
isp( _mtuu1_isp, MTUU1_ISP_V, ___ISR_Handler);

/*****************************
 * Timer 2
 *****************************/
isp( _mtua2_isp, MTUA2_ISP_V, ___ISR_Handler);
isp( _mtub2_isp, MTUB2_ISP_V, ___ISR_Handler);
isp( _mtuv2_isp, MTUV2_ISP_V, ___ISR_Handler);
isp( _mtuu2_isp, MTUU2_ISP_V, ___ISR_Handler);

/*****************************
 * Timer 3
 *****************************/
isp( _mtua3_isp, MTUA3_ISP_V, ___ISR_Handler);
isp( _mtub3_isp, MTUB3_ISP_V, ___ISR_Handler);
isp( _mtuc3_isp, MTUC3_ISP_V, ___ISR_Handler);
isp( _mtud3_isp, MTUD3_ISP_V, ___ISR_Handler);
isp( _mtuv3_isp, MTUV3_ISP_V, ___ISR_Handler);

/*****************************
 * Timer 4
 *****************************/
isp( _mtua4_isp, MTUA4_ISP_V, ___ISR_Handler);
isp( _mtub4_isp, MTUB4_ISP_V, ___ISR_Handler);
isp( _mtuc4_isp, MTUC4_ISP_V, ___ISR_Handler);
isp( _mtud4_isp, MTUD4_ISP_V, ___ISR_Handler);
isp( _mtuv4_isp, MTUV4_ISP_V, ___ISR_Handler);


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


/******************************
 * A/D converters
 * ADC0-1
 ******************************/
isp( _adi0_isp,  ADI0_ISP_V, ___ISR_Handler);
isp( _adi1_isp,  ADI1_ISP_V, ___ISR_Handler);


/******************************
 *  Data transfer controller
 ******************************/
isp( _dtci_isp,  DTC_ISP_V, ___ISR_Handler);


/******************************
 *  Counter match timer
 ******************************/
isp( _cmt0_isp,  CMT0_ISP_V, ___ISR_Handler);
isp( _cmt1_isp,  CMT1_ISP_V, ___ISR_Handler);


/******************************
 *  Watchdog timer
 ******************************/
isp( _wdt_isp,  WDT_ISP_V, ___ISR_Handler);


/******************************
 * DRAM refresh control unit
 * of bus state controller
 ******************************/
isp( _bsc_isp,  CMI_ISP_V, ___ISR_Handler);

/******************************
 *  I/O port
 ******************************/
isp( _oei_isp,  OEI_ISP_V, ___ISR_Handler);


/*****************************
 * Parity control unit of
 * the bus state controller
 * NOT PROVIDED IN SH-2
 *****************************/
/* isp( _prt_isp,  PRT_ISP_V, ___ISR_Handler); */
