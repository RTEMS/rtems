/*
 *  These routines handle the external exception.  Multiple ISRs occur off
 *  of this one interrupt.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <rtems/chain.h>
#include <rtems/bspIo.h>
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
  rtems_chain_node    Node;
  rtems_isr_entry     handler;                  /* isr routine        */
  rtems_vector_number vector;                   /* vector number      */
} EE_ISR_Type;

/* Note:  The following will not work if we add a method to remove
 *        handlers at a later time.
 */
  EE_ISR_Type         ISR_Nodes [NUM_LIRQ_HANDLERS];
  uint16_t            Nodes_Used;
  rtems_chain_control ISR_Array  [NUM_LIRQ];

/* XXX */
void init_irq_data_register(void);

void Init_EE_mask_init(void)
{
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
  uint16_t         vec_idx  = vector - Score_IRQ_First;
  uint32_t         index;

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

  rtems_chain_append( &ISR_Array[vec_idx], &ISR_Nodes[index].Node );

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
 uint16_t            index;
 EE_ISR_Type         *node;
 uint16_t            value;
#if (HAS_PMC_PSC8)
 uint16_t            PMC_irq;
 uint16_t            check_irq;
 uint16_t            status_word;
#endif

 index = read_and_clear_irq();
 if ( index >= NUM_LIRQ ) {
   printk( "ERROR:: Invalid interrupt number (%02x)\n", index );
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

        if ( rtems_chain_is_tail( &ISR_Array[ index ], (void *)node ) ) {
          printk ("ERROR:: check %d interrupt %02d has no isr\n", check_irq, index);
          value = get_irq_mask();
          printk("        Mask = %02x\n", value);
	}
        while ( !rtems_chain_is_tail( &ISR_Array[ index ], (void *)node ) ) {
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
    if ( rtems_chain_is_tail( &ISR_Array[ index ], (void *)node ) ) {
      printk( "ERROR:: interrupt %02x has no isr\n", index);
      value = get_irq_mask();
      printk("        Mask = %02x\n", value);
      return;
    }
    while ( !rtems_chain_is_tail( &ISR_Array[ index ], (void *)node ) ) {
     (*node->handler)( node->vector );
     node = (EE_ISR_Type *) node->Node.next;
    }
  }

}
