/*  Shm_Initialization
 *
 *  This routine is the shared memory communications initerface
 *  driver initialization routine.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters: NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#define _SHM_INIT

#include <rtems.h>
#include <shm_driver.h>

#include <string.h>    /* memset() */
#include <stdlib.h>    /* malloc() */
#include <assert.h>

/*
 * User extension to install MPCI_Fatal as a fatal error
 * handler extension
 */

rtems_extensions_table MPCI_Shm_extensions;

/*
 *  MP configuration table from confdefs.h
 */

rtems_mpci_entry Shm_Initialization( void )

{
  uint32_t                 i, all_initialized;
  uint32_t                 interrupt_cause, interrupt_value;
  void                    *interrupt_address;
  Shm_Node_status_control *nscb;
  uint32_t                 extension_id;    /* for installation of MPCI_Fatal */
  uint32_t                 remaining_memory;
  uint32_t                 local_node;

  local_node = _Configuration_MP_table->node;

  Shm_Get_configuration( local_node, &Shm_Configuration );

  Shm_Interrupt_table = (Shm_Interrupt_information *) malloc(
    sizeof(Shm_Interrupt_information) * (SHM_MAXIMUM_NODES + 1)
  );

  assert( Shm_Interrupt_table );

  Shm_Receive_message_count = 0;
  Shm_Null_message_count    = 0;
  Shm_Interrupt_count       = 0;

  /*
   *  Set the Node Status indicators
   */

  Shm_Pending_initialization =
    Shm_Convert(rtems_build_name( 'P', 'E', 'N', 'D' ));
  Shm_Initialization_complete =
    Shm_Convert(rtems_build_name( 'C', 'O', 'M', 'P' ));
  Shm_Active_node =
    Shm_Convert(rtems_build_name( 'A', 'C', 'T', 'V' ));

  /*
   *  Initialize the constants used by the Locked Queue code.
   */

  Shm_Locked_queue_End_of_list = Shm_Convert( 0xffffffff );
  Shm_Locked_queue_Not_on_list = Shm_Convert( 0xfffffffe );

  /*
   *  Set the base addresses for the:
   *     + Node Status Table
   *     + Free Pool and Receive Queues
   *     + Envelopes
   */

  Shm_Node_statuses  = (Shm_Node_status_control *) START_NS_CBS;
  Shm_Locked_queues  = (Shm_Locked_queue_Control *) START_LQ_CBS;
  Shm_Envelopes      = (Shm_Envelope_control *) START_ENVELOPES;

  /*
   *  Calculate the maximum number of envelopes which can be
   *  placed the remaining shared memory.
   */

  remaining_memory =
     ((void *)Shm_Configuration->base + Shm_Configuration->length) -
     ((void *)Shm_Envelopes);

  Shm_Maximum_envelopes = remaining_memory / sizeof( Shm_Envelope_control );
  Shm_Maximum_envelopes -= 1;

  /*
   *  Set the pointer to the receive queue for the local node.
   *  When we receive a node, we will get it from here before
   *  processing it.
   */

  Shm_Local_receive_queue  = &Shm_Locked_queues[ local_node ];
  Shm_Local_node_status    = &Shm_Node_statuses[ local_node ];

  /*
   *  Convert local interrupt cause information into the
   *  neutral format so other nodes will be able to
   *  understand it.
   */

  interrupt_address =
    (void *) Shm_Convert( (uint32_t)Shm_Configuration->Intr.address );
  interrupt_value   = Shm_Convert( Shm_Configuration->Intr.value );
  interrupt_cause   = Shm_Convert( Shm_Configuration->Intr.length );

  if ( Shm_Configuration->poll_intr == POLLED_MODE ) Shm_install_timer();
  else                                               Shm_setvec();

  if ( Shm_Is_master_node() ) {

    /*
     *  Zero out the shared memory area.
     */

    (void) memset(
      (void *) Shm_Configuration->base,
      0,
      Shm_Configuration->length
    );

    /*
     *  Initialize all of the locked queues (the free envelope
     *  pool and a receive queue per node) and set all of the
     *  node's status so they will be waiting to initialization
     *  to complete.
     */

    Shm_Locked_queue_Initialize( FREE_ENV_CB, FREE_ENV_POOL );

    for ( i=SHM_FIRST_NODE ; i<=SHM_MAXIMUM_NODES ; i++ ) {
      Shm_Initialize_receive_queue( i );

      Shm_Node_statuses[ i ].status = Shm_Pending_initialization;
      Shm_Node_statuses[ i ].error  = 0;
    }

    /*
     *  Initialize all of the envelopes and place them in the
     *  free pool.
     */

    for ( i=0 ; i<Shm_Maximum_envelopes ; i++ ) {
      Shm_Envelopes[ i ].index = Shm_Convert(i);
      Shm_Free_envelope( &Shm_Envelopes[ i ] );
    }

    /*
     *  Initialize this node's interrupt information in the
     *  shared area so other nodes can interrupt us.
     */

    Shm_Local_node_status->int_address = (uint32_t) interrupt_address;
    Shm_Local_node_status->int_value   = interrupt_value;
    Shm_Local_node_status->int_length  = interrupt_cause;

    Shm_Local_node_status->status      = Shm_Initialization_complete;

    /*
     *  Loop until all nodes have completed initialization.
     */

    do {
      all_initialized = 1;

      for ( i = SHM_FIRST_NODE ; i <= SHM_MAXIMUM_NODES ; i++ )
        if ( Shm_Node_statuses[ i ].status != Shm_Initialization_complete )
          all_initialized = 0;

    } while ( all_initialized == 0 );

    /*
     *  Tell the other nodes we think that the system is up.
     */

    for ( i = SHM_FIRST_NODE ; i <= SHM_MAXIMUM_NODES ; i++ )
      Shm_Node_statuses[ i ].status = Shm_Active_node;

  } else {   /* is not MASTER node */

    /*
     *  Initialize the node status for the non-master nodes.
     *  Because the master node zeroes out memory, it is
     *  necessary for them to keep putting their values in
     *  the node status area until the master says they
     *  should become active.
     */

    Shm_Local_node_status->status = Shm_Pending_initialization;

    do {

      if ( Shm_Local_node_status->status == Shm_Pending_initialization ) {

        /*
         *  Initialize this node's interrupt information in the
         *  shared area so other nodes can interrupt us.
         */

        Shm_Local_node_status->int_address =
          (uint32_t) interrupt_address;
        Shm_Local_node_status->int_value   = interrupt_value;
        Shm_Local_node_status->int_length  = interrupt_cause;

        Shm_Local_node_status->status      = Shm_Initialization_complete;
      }
    } while ( Shm_Local_node_status->status != Shm_Active_node ) ;
  }

  /*
   *  Initialize the Interrupt Information Table
   */

  for ( i = SHM_FIRST_NODE ; i <= SHM_MAXIMUM_NODES ; i++ ) {
    nscb = &Shm_Node_statuses[ i ];

    Shm_Interrupt_table[i].address = Shm_Convert_address(
      (void *)Shm_Convert(((vol_u32) nscb->int_address))
    );
    Shm_Interrupt_table[i].value = Shm_Convert( nscb->int_value );
    Shm_Interrupt_table[i].length = Shm_Convert( nscb->int_length );
  }

  MPCI_Shm_extensions.fatal = MPCI_Fatal;

  (void) rtems_extension_create(
    rtems_build_name( 'M', 'P', 'E', 'X' ),
    &MPCI_Shm_extensions,
    &extension_id
  );
}
