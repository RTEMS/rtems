/*
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  MVME147 port for TNI - Telecom Bretagne
 *  by Dominique LE CAMPION (Dominique.LECAMPION@enst-bretagne.fr)
 *  May 1996
 */

#include <bsp.h>

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */
void bsp_start( void )
{
  rtems_isr_entry       *monitors_vector_table;
  int                   index;
  uint8_t               node_number;

  monitors_vector_table = (rtems_isr_entry *)0;   /* 147Bug Vectors are at 0 */
  m68k_set_vbr( monitors_vector_table );

  for ( index=2 ; index<=255 ; index++ )
    M68Kvec[ index ] = monitors_vector_table[ 32 ];

  M68Kvec[  2 ] = monitors_vector_table[  2 ];   /* bus error vector */
  M68Kvec[  4 ] = monitors_vector_table[  4 ];   /* breakpoints vector */
  M68Kvec[  9 ] = monitors_vector_table[  9 ];   /* trace vector */
  M68Kvec[ 47 ] = monitors_vector_table[ 47 ];   /* system call vector */

  m68k_set_vbr( &M68Kvec );

  pcc->int_base_vector = PCC_BASE_VECTOR & 0xF0;
  /* Set the PCC int vectors base */

  /* VME shared memory configuration */
  /* Only the first node shares its top 128k DRAM */

  vme_lcsr->utility_interrupt_vector = VME_BASE_VECTOR & 0xF8;
  /* Set VMEchip base interrupt vector */
  vme_lcsr->utility_interrupt_mask |= 0x02;
  /* Enable SIGLP interruption (see shm support) */
  pcc->general_purpose_control &= 0x10;
  /* Enable VME master interruptions */

  if (vme_lcsr->system_controller & 0x01) {
    /* the board is system controller */
    vme_lcsr->system_controller = 0x08;
    /* Make VME access round-robin */
  }

#if defined(RTEMS_MULTIPROCESSING)
  node_number = (uint8_t)
    (Configuration.User_multiprocessing_table->node - 1) & 0xF;
#else
   node_number = 1;
#endif
  /* Get and store node ID, first node_number = 0 */
  vme_gcsr->board_identification = node_number;

  vme_lcsr->gcsr_base_address = node_number;
  /* Setup the base address of this board's gcsr */
  vme_lcsr->timer_configuration = 0x6a;
  /* Enable VME time outs, maximum periods */

  if (node_number == 0) {
    pcc->slave_base_address = 0x01;
    /* Set local DRAM base address on the VME bus to the DRAM size */

    vme_lcsr->vme_bus_requester = 0x80;
    while (! (vme_lcsr->vme_bus_requester & 0x40));
    /* Get VMEbus mastership */
    vme_lcsr->slave_address_modifier = 0xfb;
    /* Share everything */
    vme_lcsr->slave_configuration = 0x80;
    /* Share local DRAM */
    vme_lcsr->vme_bus_requester = 0x0;
    /* release bus */
  } else {
    pcc->slave_base_address = 0;
    /* Set local DRAM base address on the VME bus to 0 */

    vme_lcsr->vme_bus_requester = 0x80;
    while (! (vme_lcsr->vme_bus_requester & 0x40));
    /* Get VMEbus mastership */
    vme_lcsr->slave_address_modifier = 0x08;
    /* Share only the short adress range */
    vme_lcsr->slave_configuration = 0;
    /* Don't share local DRAM */
    vme_lcsr->vme_bus_requester = 0x0;
    /* release bus */
  }

  vme_lcsr->master_address_modifier = 0;
  /* Automatically set the address modifier */
  vme_lcsr->master_configuration = 1;
  /* Disable D32 transfers : they don't work on my VMEbus rack */

  rtems_cache_enable_instruction();
  rtems_cache_enable_data();
}
