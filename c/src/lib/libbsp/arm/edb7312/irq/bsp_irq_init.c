/*
 * Cirrus EP7312 Intererrupt handler
 *
 * Copyright (c) 2002 by Jay Monkman <jtm@smoothsmoothie.com>
 *	
 * Copyright (c) 2002 by Charlie Steader <charlies@poliac.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *
 *  $Id$
*/
#include <irq.h>
#include <bsp.h>
#include <ep7312.h>

extern void default_int_handler();

void BSP_rtems_irq_mngt_init()
{
	long int_stat;

	/* mask all interrupts */
	*EP7312_INTMR1 = 0x0;
	*EP7312_INTMR2 = 0x0;
	*EP7312_INTMR3 = 0x0;

	/* clear all pending interrupt status' */
	int_stat = *EP7312_INTSR1;
	if(int_stat & EP7312_INTR1_EXTFIQ)
	{
	}
	if(int_stat & EP7312_INTR1_BLINT)
	{
		*EP7312_BLEOI = 0xFFFFFFFF;
	}
	if(int_stat & EP7312_INTR1_WEINT)
	{
            *EP7312_TEOI = 0xFFFFFFFF;
	}
	if(int_stat & EP7312_INTR1_MCINT)
	{
	}
	if(int_stat & EP7312_INTR1_CSINT)
	{
		*EP7312_COEOI = 0xFFFFFFFF;
	}
	if(int_stat & EP7312_INTR1_EINT1)
	{
	}
	if(int_stat & EP7312_INTR1_EINT2)
	{
	}
	if(int_stat & EP7312_INTR1_EINT3)
	{
	}
	if(int_stat & EP7312_INTR1_TC1OI)
	{
            *EP7312_TC1EOI = 0xFFFFFFFF;
	}
	if(int_stat & EP7312_INTR1_TC2OI)
	{
            *EP7312_TC2EOI = 0xFFFFFFFF;
	}
	if(int_stat & EP7312_INTR1_RTCMI)
	{
            *EP7312_RTCEOI = 0xFFFFFFFF;
	}
	if(int_stat & EP7312_INTR1_TINT)
	{
            *EP7312_TEOI = 0xFFFFFFFF;
	}
	if(int_stat & EP7312_INTR1_URXINT1)
	{
	}
	if(int_stat & EP7312_INTR1_UTXINT1)
	{
	}
	if(int_stat & EP7312_INTR1_UMSINT)
	{
            *EP7312_UMSEOI = 0xFFFFFFFF;
	}
	if(int_stat & EP7312_INTR1_SSEOTI)
	{
            *EP7312_SYNCIO;
	}
	int_stat = *EP7312_INTSR1;

	int_stat = *EP7312_INTSR2;
	if(int_stat & EP7312_INTR2_KBDINT)
	{
            *EP7312_KBDEOI = 0xFFFFFFFF;
	}
	if(int_stat & EP7312_INTR2_SS2RX)
	{
	}
	if(int_stat & EP7312_INTR2_SS2TX)
	{
	}
	if(int_stat & EP7312_INTR2_URXINT2)
	{
	}
	if(int_stat & EP7312_INTR2_UTXINT2)
	{
	}
	int_stat = *EP7312_INTSR2;

	int_stat = *EP7312_INTSR3;
	if(int_stat & EP7312_INTR2_DAIINT)
	{
	}
	int_stat = *EP7312_INTSR3;
}

