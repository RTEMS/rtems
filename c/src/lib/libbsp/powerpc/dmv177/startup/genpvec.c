/*  genpvec.c
 *
 *  These routines handle the external exception.  Multiple ISRs occur off
 *  of this one interrupt.  This method will allow multiple ISRs to be
 *  called using the same IRQ index.  However, removing the ISR routines is
 *  presently not supported.
 *
 *  The external exception vector numbers begin with DMV170_IRQ_FIRST.
 *  DMV170_IRQ_FIRST is defined to be one greater than the last processor
 *  interrupt.  
 *
 *  COPYRIGHT (c) 1989-1999.
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

/*
 * Note:  The following will not work if we add a method to remove
 *        handlers at a later time.
 */
EE_ISR_Type       ISR_Nodes [NUM_LIRQ_HANDLERS];
rtems_unsigned16  Nodes_Used; 
Chain_Control     ISR_Array  [NUM_LIRQ];

/*PAGE
 *
 * external_exception_ISR
 *
 * This interrupt service routine is called for an External Exception.
 *
 *  Input parameters:
 *    vector - vector number representing the external exception vector.
 *
 *  Output parameters:  NONE
 *
 *  Return values: 
 */

rtems_isr external_exception_ISR (
  rtems_vector_number   vector             /* IN  */
)
{ 
  rtems_unsigned16      index;
  rtems_boolean         is_active=FALSE;
  rtems_unsigned32      scv64_status;
  Chain_Node           *node;
  EE_ISR_Type          *ee_isr;
  

  /*
   * Get all active interrupts. 
   */
  scv64_status = SCV64_Get_Interrupt();
  scv64_status &= SCV64_Get_Interrupt_Enable();

  /*
   * Process any set interrupts.
   */
  for (index = 0; index <= 5; index++) {
    switch(index) {
      case 0:
        is_active = SCV64_Is_IRQ0( scv64_status );
        break;
      case 1:
        is_active = SCV64_Is_IRQ1( scv64_status );
        break;
      case 2:
        is_active = SCV64_Is_IRQ2( scv64_status );
        break;
      case 3:
        is_active = SCV64_Is_IRQ3( scv64_status );
        break;
      case 4:
        is_active = SCV64_Is_IRQ4( scv64_status );
        break;
      case 5:
        is_active = SCV64_Is_IRQ5( scv64_status );
        break;
    }

    if (is_active) {
      /*
       * Read vector.
       */
      node = ISR_Array[ index ].first;
      while ( !_Chain_Is_tail( &ISR_Array[ index ], node ) ) {
        ee_isr = (EE_ISR_Type *) node;
        (*ee_isr->handler)( ee_isr->vector );
        node = node->next;
      }
    }
  }
}


/*PAGE
 *
 *  initialize_external_exception_vector
 *
 *  This routine initializes the external exception vector
 * 
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values: NONE
 */

void initialize_external_exception_vector ()
{
  int i;
  rtems_isr_entry previous_isr;
  rtems_status_code status;
  extern void SCV64_Initialize( void ); 

  Nodes_Used = 0;

  /*
   * Initialize the SCV64 chip
   */
  SCV64_Initialize(); 

  for (i=0; i <NUM_LIRQ; i++)
    Chain_Initialize_empty( &ISR_Array[i] );

  /*  
   * Install external_exception_ISR () as the handler for 
   *  the General Purpose Interrupt.
   */

  status = rtems_interrupt_catch( external_exception_ISR, 
           PPC_IRQ_EXTERNAL , (rtems_isr_entry *) &previous_isr );

}

/*PAGE
 *
 *  set_EE_vector
 *
 *  This routine installs one of multiple ISRs for the general purpose 
 *  inerrupt.
 *
 *  Input parameters:
 *    handler - handler to call at exception
 *    vector  - vector number associated with this handler.
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 */

rtems_isr_entry  set_EE_vector(
  rtems_isr_entry     handler,      /* isr routine        */
  rtems_vector_number vector        /* vector number      */
)
{
  rtems_unsigned16 vec_idx  = vector - DMV170_IRQ_FIRST;
  rtems_unsigned32 index;
  
  /* 
   *  Verify that all of the nodes have not been used.
   */
  assert  (Nodes_Used < NUM_LIRQ_HANDLERS);
   
  /*
   *  If we have already installed this handler for this vector, then
   *  just reset it.
   */

  for ( index=0 ; index <= Nodes_Used ; index++ ) {
    if ( ISR_Nodes[index].vector == vector &&
         ISR_Nodes[index].handler == handler )
      return 0;
  }

  /*
   * Increment the number of nedes used and set the index for the node
   * array.
   */
  
  Nodes_Used++; 
  index = Nodes_Used - 1;

  /*
   * Write the values of the handler and the vector to this node.
   */
  ISR_Nodes[index].handler = handler;
  ISR_Nodes[index].vector  = vector;

  /*
   * Connect this node to the chain at the location of the 
   * vector index.  
   */
  Chain_Append( &ISR_Array[vec_idx], &ISR_Nodes[index].Node );

  /*
   * Enable the LIRQ interrupt.
   */
  SCV64_Generate_DUART_Interrupts();

  /*
   * No interrupt service routine was removed so return 0
   */
  return 0;
}










