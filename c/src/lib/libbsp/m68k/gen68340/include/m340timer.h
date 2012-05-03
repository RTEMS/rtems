/*
 *  Header file for timer driver
 *  defines for accessing M68340 timer registers
 *
 *  Author:
 *  Geoffroy Montel
 *  France Telecom - CNET/DSM/TAM/CAT
 *  4, rue du Clos Courtel
 *  35512 CESSON-SEVIGNE
 *  FRANCE
 *
 *  e-mail: g_montel@yahoo.com
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __m340timer_H__
#define __m340timer_H__

extern void Fifo_Full_benchmark_timer_initialize (void);

extern void (*Restart_Fifo_Full_A_Timer)();
extern void (*Restart_Check_A_Timer)();
extern void (*Restart_Fifo_Full_B_Timer)();
extern void (*Restart_Check_B_Timer)();

/*  CR */
#define m340_SWR		(1<<15)
#define m340_Polling_Mode	(0<<12)
#define m340_TC_Enabled		(1<<12)
#define m340_TG_Enabled		(2<<12)
#define m340_TG_TC_Enabled	(3<<12)
#define m340_TO_Enabled		(4<<12)
#define m340_TO_TC_Enabled	(5<<12)
#define m340_TG_TG_Enabled	(6<<12)
#define m340_TO_TG_TG_Enabled	(7<<12)
#define m340_TGE		(1<<11)
#define m340_PSE		(1<<10)
#define m340_CPE		(1<<9)
#define m340_CLK		(1<<8)
#define m340_Divide_by_2	(1<<5)
#define m340_Divide_by_4	(2<<5)
#define m340_Divide_by_8	(3<<5)
#define m340_Divide_by_16	(4<<5)
#define m340_Divide_by_32	(5<<5)
#define m340_Divide_by_64	(6<<5)
#define m340_Divide_by_128	(7<<5)
#define m340_Divide_by_256	(0<<5)
#define m340_ICOC		(0<<2)
#define m340_SWG		(1<<2)
#define m340_VDCSWG		(2<<2)
#define m340_VWSSPG		(3<<2)
#define m340_PWM		(4<<2)
#define m340_PM			(5<<2)
#define m340_EC			(6<<2)
#define m340_TB			(7<<2)
#define m340_Disabled		0
#define m340_Toggle_Mode	1
#define m340_Zero_Mode		2
#define m340_One_Mode		3

/*  SR */
#define m340_IRQ		(1<<15)
#define m340_TO			(1<<14)
#define m340_TG			(1<<13)
#define m340_TC			(1<<12)
#define m340_TGL		(1<<11)
#define m340_ON			(1<<10)
#define m340_OUT		(1<<9)
#define m340_COM		(1<<8)

#endif
