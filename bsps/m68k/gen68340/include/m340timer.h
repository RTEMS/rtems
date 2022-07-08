/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup m68k_gen68340
 *
 * @brief Time driver definitions.
 */

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

#ifndef __m340timer_H__
#define __m340timer_H__

extern void Fifo_Full_benchmark_timer_initialize (void);

extern void (*Restart_Fifo_Full_A_Timer)(void);
extern void (*Restart_Check_A_Timer)(void);
extern void (*Restart_Fifo_Full_B_Timer)(void);
extern void (*Restart_Check_B_Timer)(void);

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
