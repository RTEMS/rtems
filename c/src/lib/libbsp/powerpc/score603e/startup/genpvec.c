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
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>
#include <chain.h>
#include <assert.h>

#include <stdio.h> /* for sprintf */


/* 
 * Proto types for this file                                          
 */

rtems_isr external_exception_ISR (
  rtems_vector_number   vector                                  /* IN  */
);

#define   NUM_LIRQ_HANDLERS   20
#define   NUM_LIRQ            ( MAX_BOARD_IRQS - PPC_IRQ_LAST )

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

/* XXX */
void init_irq_data_register();

void initialize_external_exception_vector ()
{
  int i;
  rtems_isr_entry previous_isr;
  rtems_status_code status;

  Nodes_Used = 0;

  /*
   * Mask out all interupts until they have a handler installed.
   */

  for (i=0; i <NUM_LIRQ; i++)
    Chain_Initialize_empty( &ISR_Array[i] );
  
  init_irq_data_register();
   
  /*  
   * Install external_exception_ISR () as the handler for 
   *  the General Purpose Interrupt.
   */
  status = rtems_interrupt_catch( external_exception_ISR, 
           PPC_IRQ_EXTERNAL, (rtems_isr_entry *) &previous_isr );
}

void Init_EE_mask_init() {
;
}

/*
 *  This routine installs one of multiple ISRs for the general purpose 
 *  inerrupt.
 */
rtems_isr_entry  set_EE_vector(
  rtems_isr_entry     handler,      /* isr routine        */
  rtems_vector_number vector        /* vector number      */
)
{
  rtems_unsigned16 vec_idx  = vector - Score_IRQ_First;
  rtems_unsigned32 index;
  
  assert  (Nodes_Used < NUM_LIRQ_HANDLERS);
   
  /*
   *  If we have already installed this handler for this vector, then
   *  just reset it.
   */

  for ( index=0 ; index <= Nodes_Used ; index++ ) {
    if ( ISR_Nodes[index].vector == vector &&
         ISR_Nodes[index].handler == handler )
      return NULL;
  }

  /*
   *  Doing things in this order makes them more atomic
   */
  
  Nodes_Used++; 

  index = Nodes_Used - 1;

  ISR_Nodes[index].handler = handler;
  ISR_Nodes[index].vector  = vector;

  /* printf( "Vector Index: %04x, Vector: %d (%x)\n", 
          vec_idx, vector, vector); */

  Chain_Append( &ISR_Array[vec_idx], &ISR_Nodes[index].Node );

  /*
   * Unmask the interrupt.
   */
  unmask_irq( vec_idx );

  return NULL;
}

/* 
 * This interrupt service routine is called for an External Exception.
 */
rtems_isr external_exception_ISR (
  rtems_vector_number   vector             /* IN  */
)
{ 
 rtems_unsigned16    index;
 EE_ISR_Type         *node;
 rtems_unsigned16    value;
 char                err_msg[100];
#if (HAS_PMC_PSC8)
 rtems_unsigned16    PMC_irq;
 rtems_unsigned16    check_irq;
 rtems_unsigned16    status_word;
#endif

 index = read_and_clear_irq();
 if ( index >= NUM_LIRQ ) {
   sprintf( err_msg, "ERROR:: Invalid interrupt number (%02x)\n", index );
   DEBUG_puts( err_msg );
   return;
 }

#if (HAS_PMC_PSC8)
  PMC_irq = SCORE603E_PCI_IRQ_0 - SCORE603E_IRQ00;

  if (index ==  PMC_irq) {
    status_word = read_and_clear_PMC_irq( index );

    for (check_irq=SCORE603E_IRQ16; check_irq<=SCORE603E_IRQ19; check_irq++) {
      if ( Is_PMC_IRQ( check_irq, status_word )) {
        index = check_irq - SCORE603E_IRQ00;
        node = (EE_ISR_Type *)(ISR_Array[ index ].first);

        if ( _Chain_Is_tail( &ISR_Array[ index ], (void *)node ) ) {
          sprintf(err_msg,"ERROR:: check %d interrupt %02d has no isr\n", 
                  check_irq, index);
          DEBUG_puts( err_msg);
          value = get_irq_mask();
          sprintf(err_msg,"        Mask = %02x\n", value);
          DEBUG_puts( err_msg);
	}
        while ( !_Chain_Is_tail( &ISR_Array[ index ], (void *)node ) ) {
          (*node->handler)( node->vector );
          node = (EE_ISR_Type *) node->Node.next;
        }
      }
    }
  }
  else
#endif
  { 
    node = (EE_ISR_Type *)(ISR_Array[ index ].first);
    if ( _Chain_Is_tail( &ISR_Array[ index ], (void *)node ) ) {
      sprintf(err_msg,"ERROR:: interrupt %02x has no isr\n", index);
      DEBUG_puts( err_msg);
      value = get_irq_mask();
      sprintf(err_msg,"        Mask = %02x\n", value);
      DEBUG_puts( err_msg);
      return;
    }
    while ( !_Chain_Is_tail( &ISR_Array[ index ], (void *)node ) ) {
     (*node->handler)( node->vector );
     node = (EE_ISR_Type *) node->Node.next;
    }
  }

}



