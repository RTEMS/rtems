/*
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *
 * THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 * AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 * You are hereby granted permission to use, copy, modify, and distribute
 * this file, provided that this notice, plus the above copyright notice
 * and disclaimer, appears in all copies. Radstone Technology will provide
 * no support for this code.
 *
 */
/*  genpvec.c
 *
 *  These routines handle the external exception.  Multiple ISRs occur off
 *  of this one interrupt.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <bsp.h>
#include <chain.h>
#include <assert.h>

/* 
 * Proto types for this file                                          
 */

rtems_isr external_exception_ISR (
  rtems_vector_number   vector                                  /* IN  */
);

#define   NUM_LIRQ_HANDLERS   20
#define   NUM_LIRQ            ( MAX_BOARD_IRQS - PPC_IRQ_LAST )

/*
 * Current 8259 masks
 */
unsigned8	ucMaster8259Mask;
unsigned8	ucSlave8259Mask;

/* 
 * Structure to for one of possible multiple interrupt handlers for 
 * a given interrupt.
 */
typedef struct
{
  Chain_Node          Node;
  rtems_isr_entry     handler;                  /* isr routine        */
  rtems_vector_number vector;                   /* vector number      */
} EE_ISR_Type;


/* Note:  The following will not work if we add a method to remove
 *        handlers at a later time.
 */
  EE_ISR_Type       ISR_Nodes [NUM_LIRQ_HANDLERS];
  rtems_unsigned16  Nodes_Used; 
  Chain_Control     ISR_Array  [NUM_LIRQ];

void initialize_external_exception_vector()
{
	rtems_isr_entry previous_isr;
	rtems_status_code status;
	int i;

	Nodes_Used = 0;

	for (i=0; i <NUM_LIRQ; i++)
	{
		Chain_Initialize_empty( &ISR_Array[i] );
	}

	/*
	 * Initialise the 8259s
	 */
	outport_byte(ISA8259_M_CTRL, 0x11); /* ICW1 */
	outport_byte(ISA8259_S_CTRL, 0x11); /* ICW1 */
	outport_byte(ISA8259_M_MASK, 0x00); /* ICW2 vectors 0-7 */
	outport_byte(ISA8259_S_MASK, 0x08); /* ICW2 vectors 8-15 */
	outport_byte(ISA8259_M_MASK, 0x04); /* ICW3 cascade on IRQ2 */
	outport_byte(ISA8259_S_MASK, 0x02); /* ICW3 cascade on IRQ2 */
	outport_byte(ISA8259_M_MASK, 0x01); /* ICW4 x86 normal EOI */
	outport_byte(ISA8259_S_MASK, 0x01); /* ICW4 x86 normal EOI */

	/*
	 * Enable IRQ2 cascade and disable all other interrupts
	 */
	ucMaster8259Mask=0xfb;
	ucSlave8259Mask=0xff;

	outport_byte(ISA8259_M_MASK, ucMaster8259Mask);
	outport_byte(ISA8259_S_MASK, ucSlave8259Mask);

	/*
	 * Set up edge/level
	 */
	switch(ucSystemType)
	{
		case SYS_TYPE_PPC1:
		{
			if(ucBoardRevMaj<5)
			{
				outport_byte(ISA8259_S_ELCR,
					     ELCRS_INT15_LVL);
			}
			else
			{
				outport_byte(ISA8259_S_ELCR,
					     ELCRS_INT9_LVL |
					     ELCRS_INT11_LVL |
					     ELCRS_INT14_LVL |
					     ELCRS_INT15_LVL);
			}
			outport_byte(ISA8259_M_ELCR,
				     ELCRM_INT5_LVL |
				     ELCRM_INT7_LVL);
			break;
		}

		case SYS_TYPE_PPC1a:
		{
			outport_byte(ISA8259_S_ELCR,
				     ELCRS_INT9_LVL |
				     ELCRS_INT11_LVL |
				     ELCRS_INT14_LVL |
				     ELCRS_INT15_LVL);
			outport_byte(ISA8259_M_ELCR,
				     ELCRM_INT5_LVL);
			break;
		}
		
		case SYS_TYPE_PPC2:
		case SYS_TYPE_PPC2a:
		case SYS_TYPE_PPC4:
		default:
		{
			outport_byte(ISA8259_S_ELCR,
				     ELCRS_INT9_LVL |
				     ELCRS_INT10_LVL |
				     ELCRS_INT11_LVL |
				     ELCRS_INT14_LVL |
				     ELCRS_INT15_LVL);
			outport_byte(ISA8259_M_ELCR,
				     ELCRM_INT5_LVL |
				     ELCRM_INT7_LVL);
			break;
		}
	}

	/*  
	 * Install external_exception_ISR () as the handler for 
	 *  the General Purpose Interrupt.
	 */

	status = rtems_interrupt_catch( external_exception_ISR, 
					PPC_IRQ_EXTERNAL,
					(rtems_isr_entry *) &previous_isr );
}

/*
 *  This routine installs one of multiple ISRs for the general purpose 
 *  inerrupt.
 */
void set_EE_vector(
  rtems_isr_entry     handler,      /* isr routine        */
  rtems_vector_number vector        /* vector number      */
)
{
	rtems_unsigned16 vec_idx  = vector - PPCN_60X_8259_IRQ_BASE;
	rtems_unsigned32 index;

	assert  (Nodes_Used < NUM_LIRQ_HANDLERS);

	/*
	 *  If we have already installed this handler for this vector, then
	 *  just reset it.
	 */

	for ( index=0 ; index < Nodes_Used ; index++ )
	{
		if(ISR_Nodes[index].vector == vector &&
		   ISR_Nodes[index].handler == handler)
		{
			return;
		}
	}

	/*
	 *  Doing things in this order makes them more atomic
	 */

	Nodes_Used++; 

	index = Nodes_Used - 1;

	ISR_Nodes[index].handler = handler;
	ISR_Nodes[index].vector  = vector;

	Chain_Append( &ISR_Array[vec_idx], &ISR_Nodes[index].Node );

	/*
	 * Enable the interrupt
	 */
	En_Ext_Interrupt(vector);
}

/* 
 * This interrupt service routine is called for an External Exception.
 */
rtems_isr external_exception_ISR (
  rtems_vector_number   vector             /* IN  */
)
{ 
	unsigned16	index;
	unsigned8	ucISr;
	EE_ISR_Type	*node;

	index = *((volatile unsigned8 *)IRQ_VECTOR_BASE);

	/*
	 * check for spurious interrupt
	 */
	if(index==7)
	{
		/*
		 * OCW3 select IS register
		 */
		outport_byte(ISA8259_M_CTRL, 0x0b);
		/*
		 * Read IS register
		 */
		inport_byte(ISA8259_M_CTRL, ucISr);
		if(!(ucISr & 0x80))
		{
			/*
			 * Spurious interrupt
			 */
			return;
		}
	}

	node=(EE_ISR_Type *)ISR_Array[index].first;
	while(!_Chain_Is_tail(&ISR_Array[index], (Chain_Node *)node))
	{
		(*node->handler)( node->vector );
		node = (EE_ISR_Type *)node->Node.next;
	}

	/*
	 * Dismiss the interrupt
	 */
	if(index&8)
	{
		/*
		 * Dismiss the interrupt in Slave first as it
		 * is cascaded
		 */
		outport_byte(ISA8259_S_CTRL, NONSPECIFIC_EOI);
	}

	/*
	 * Dismiss the interrupt in Master
	 */
	outport_byte(ISA8259_M_CTRL, NONSPECIFIC_EOI);
}

void Dis_Ext_Interrupt(int level)
{
	ISR_Level Irql;

	level-=PPCN_60X_8259_IRQ_BASE;

	if(level==2)
	{
		/*
		 * Level 2 is for cascade and must not be fiddled with
		 */
		return;
	}

	/*
	 * Ensure that accesses to the mask are indivisible
	 */
	_ISR_Disable(Irql);

	if(level<8)
	{
		/*
		 * Interrupt is handled by Master
		 */
		ucMaster8259Mask|=1<<level;
		outport_byte(ISA8259_M_MASK, ucMaster8259Mask);
	}
	else
	{
		/*
		 * Interrupt is handled by Slave
		 */
		ucSlave8259Mask|=1<<(level-8);
		outport_byte(ISA8259_S_MASK, ucSlave8259Mask);
	}
	_ISR_Enable(Irql);
}

void En_Ext_Interrupt(int level)
{
	ISR_Level Irql;

	level-=PPCN_60X_8259_IRQ_BASE;

	if(level==2)
	{
		/*
		 * Level 2 is for cascade and must not be fiddled with
		 */
		return;
	}

	/*
	 * Ensure that accesses to the mask are indivisible
	 */
	_ISR_Disable(Irql);

	if(level<8)
	{
		/*
		 * Interrupt is handled by Master
		 */
		ucMaster8259Mask&=~(1<<level);
		outport_byte(ISA8259_M_MASK, ucMaster8259Mask);
	}
	else
	{
		/*
		 * Interrupt is handled by Slave
		 */
		ucSlave8259Mask&=~(1<<(level-8));
		outport_byte(ISA8259_S_MASK, ucSlave8259Mask);
	}

	_ISR_Enable(Irql);
}

