/*  set_vector
 *
 *  This routine attempts to perform all "generic" interrupt initialization
 *  for the specified XINT line.  It is specific to the Cyclone CVME961 in
 *  that it knows which interrupts are initialized by the monitor, the
 *  characteristics of XINT5 (VIC068 clock tick), and that it assumes the
 *  i960 is processing interrupts in dedicated mode.  It attempts to map
 *  XINTs to interrupt vectors in a fairly straght forward way.
 *
 *         XINT   USE          VECTOR INTR TBL INDEX TRIGGERED
 *         ==== =============  ====== ============== =========
 *          0   VMEbus ERROR    0x02       0x03        EDGE
 *          1   DRAM PARITY     0x12       0x13        EDGE
 *          2   Z8530           0x22       0x23        LEVEL
 *          3   SQUALL 0        0x52       0x53        ----
 *          4   Z8536 (SQSIO4)  0x72       0x73        LEVEL
 *          5   TICK            0x32       0x33        EDGE
 *          6   VIC068          0x62       0x63        LEVEL
 *          7   UNUSED          0x42       0x43        LEVEL
 *
 *  The interrupt handler is installed in both the cached and memory
 *  resident interrupt tables.  The appropriate IMAP register is updated to
 *  reflect the vector selected by this routine.  Global interrupts are
 *  enabled. If XINT5 is being installed, places it in trigger mode.
 *  Finally, set_vector_support() is invoked to install the new IMAP and
 *  ICON, unmask the XINT in IMASK, and lower the i960's interrupt
 *  level to 0.
 *
 *  INPUT:
 *    func  - interrupt handler entry point
 *    xint  - external interrupt line
 *    type  - 0 indicates raw hardware connect
 *            1 indicates RTEMS interrupt connect
 *
 *  RETURNS:
 *    address of previous interrupt handler
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems.h>
#include <bsp.h>

#include <stdio.h>

void print_prcb();
void print_intr_info();
void print_ipnd_imsk();

unsigned int Xint_2_Group_Map[8] = { 0, 1, 2, 5, 7, 3, 6, 4 };

i960_isr_entry set_vector(                 /* returns old vector */
  rtems_isr_entry func,                    /* isr routine */
  unsigned int    xint,                    /* XINT number */
  unsigned int    type                     /* RTEMS or RAW */
)
{
  i960_isr_entry  *intr_tbl, *cached_intr_tbl;
  i960_isr_entry   saved_intr;
  unsigned int     vector, group, nibble;
  unsigned int    *imap;

  if ( xint > 7 )
    exit( 0x80 );

  cached_intr_tbl = (i960_isr_entry *) 0;
  intr_tbl        = (i960_isr_entry *) Prcb->intr_tbl;
  group           = Xint_2_Group_Map[xint];  /* remap XINT to group */
  vector          = (group << 4) + 2;        /* direct vector num   */

  if ( type )
    rtems_interrupt_catch( func, vector, (rtems_isr_entry *) &saved_intr );
  else {
    saved_intr    = (i960_isr_entry) intr_tbl[ vector ];
                                                   /* return old vector   */
    intr_tbl[ vector + 1 ]   =                     /* normal vector table */
    cached_intr_tbl[ group ] = (i960_isr_entry) func;    /* cached vector */
  }

  if       ( xint <= 3 ) imap = &Ctl_tbl->imap0;  /* updating IMAP0 */
  else                   imap = &Ctl_tbl->imap1;  /* updating IMAP1 */
  nibble = (xint % 4) * 4;
  *imap &= ~(0xf << nibble);
  *imap |= group << nibble;

  Ctl_tbl->icon &= ~0x00000400;        /* enable global interrupts */
  Ctl_tbl->icon |=  0x00004000;        /* fast sampling mode       */
  switch ( xint ) {
    case 0:   Ctl_tbl->icon |=  0x00000004;  break;
    case 1:   Ctl_tbl->icon |=  0x00000008;  break;
    case 2:   Ctl_tbl->icon &= ~0x00000010;  break;
    case 4:   Ctl_tbl->icon &= ~0x00000040;  break;
    case 5:   Ctl_tbl->icon |=  0x00000080;  break;
    case 6:   Ctl_tbl->icon &= ~0x00000100;  break;
    default:  exit( 0x81 );                  break; /* unsupported  */
  }

  if ( xint == 4 ) {                   /* reprogram MCON for SQSIO4 */
    Ctl_tbl->mcon12 = 0x00002012;      /* MCON12 - 0xCxxxxxxx       */
    Ctl_tbl->mcon13 = 0x00000000;      /* MCON13 - 0xDxxxxxxx       */
    i960_reload_ctl_group( 5 );        /* update MCON12-MCON15      */
  }

  i960_unmask_intr( xint );            /* update IMSK               */
  i960_reload_ctl_group( 1 );          /* update IMAP?/ICON         */
  return( saved_intr );                /* return old vector         */
}

void print_prcb()
{
  printf( "fault_table  =0x%p\n", Prcb->fault_tbl );
  printf( "control_tbl  =0x%p\n", Prcb->control_tbl );
  printf( "AC mask ov   =0x%x\n", Prcb->initial_ac );
  printf( "fltconfig    =0x%x\n", Prcb->fault_config );
  printf( "intr tbl     =0x%p\n", Prcb->intr_tbl );
  printf( "systable     =0x%p\n", Prcb->sys_proc_tbl );
  printf( "reserved     =0x%x\n", Prcb->reserved );
  printf( "isr stk      =0x%p\n", Prcb->intr_stack );
  printf( "ins cache    =0x%x\n", Prcb->ins_cache_cfg );
  printf( "reg cache    =0x%x\n", Prcb->reg_cache_cfg );
}

void print_intr_info()
{
  printf( "prcb =0x%p\n",    Prcb );
  printf( "ctl_tbl =0x%p\n", Ctl_tbl );
  printf( "intr_tbl=0x%p\n", Prcb->intr_tbl );
  printf( "IMAP0 = 0x%x\n",  Ctl_tbl->imap0 );
  printf( "IMAP1 = 0x%x\n",  Ctl_tbl->imap1 );
  print_ipnd_imsk();
}

void print_ipnd_imsk()
{
  printf(" IPEND = 0x%x\n", i960_pend_intrs() );
  printf(" IMASK = 0x%x\n", i960_mask_intrs() );
}
