/*  set_vector
 *
 *  This routine attempts to perform all "generic" interrupt initialization
 *  for the specified XINT line.
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
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
/*
 * i960rp specific function added
 */

#include <rtems.h>
#include <bsp.h>

#include <stdio.h>

void print_prcb();
void print_intr_info();
void print_ipnd_imsk();

unsigned int Xint_2_Group_Map[8] = { 0, 1, 2, 5, 7, 3, 6, 4 };

i960_isr_entry old_set_vector(                 /* returns old vector */
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

#if defined (i960ca)
  if ( xint == 4 ) {                   /* reprogram MCON for SQSIO4 */
    Ctl_tbl->mcon12 = 0x00002012;      /* MCON12 - 0xCxxxxxxx       */
    Ctl_tbl->mcon13 = 0x00000000;      /* MCON13 - 0xDxxxxxxx       */
    i960_reload_ctl_group( 5 );        /* update MCON12-MCON15      */
  }
#endif;

  i960_unmask_intr( xint );            /* update IMSK               */
  i960_reload_ctl_group( 1 );          /* update IMAP?/ICON         */
  return( saved_intr );                /* return old vector         */
}

/* note:  this needs a little fix up work for XINTs */
i960_isr_entry set_vector(                 /* returns old vector */
  rtems_isr_entry func,                    /* isr routine */
  unsigned int    vector,                  /* vector number */
  unsigned int    type                     /* RTEMS or RAW */
)
{
  i960_isr_entry  *intr_tbl, *cached_intr_tbl;
  i960_isr_entry   saved_intr;
  unsigned int     vect_idx, group, nibble;
  unsigned int    *imap;
  unsigned int    imask;
  unsigned int    vec_idx;
  volatile unsigned int *ipnd = (unsigned int *) IPND_ADDR;
  volatile unsigned int *imsk = (unsigned int *) IMSK_ADDR;


  cached_intr_tbl = (i960_isr_entry *) 0;
  intr_tbl        = (i960_isr_entry *) Prcb->intr_tbl;

  vec_idx = vector >> 4;
  if ( type )
  {
    rtems_interrupt_catch( func, vector, (rtems_isr_entry *) &saved_intr );
    return (saved_intr);
  }
  else {
    saved_intr    = (i960_isr_entry) intr_tbl[ vect_idx ];
                                                   /* return old vector   */
    intr_tbl[ vector ]   =                     /* normal vector table */
    cached_intr_tbl[ vec_idx ] = (i960_isr_entry) func;    /* cached vector */
  }

  if( vec_idx > 8)
	imask = 0x1000 << (vec_idx - 9);
  else
	imask = 0x1 << (vec_idx - 1);
    *ipnd &= ~(imask);
    *imsk |= (imask);

  return( saved_intr );                /* return old vector         */
}

i960_isr_entry set_tmr_vector(             /* returns old vector */
  rtems_isr_entry func,                    /* isr routine */
  unsigned int    vector,                  /* vector number */
  unsigned int    tmrno                    /* which timer? */
)
{
#if defined(i960ca)
  saved_intr = NULL;
#else
  volatile i960_isr_entry  *intr_tbl;
  volatile i960_isr_entry   saved_intr;
  volatile unsigned int *imap2 = (unsigned int *) IMAP2_ADDR;
  volatile unsigned int *isr = (unsigned int *) (4*(vector >> 4));

  intr_tbl          = (i960_isr_entry *) Prcb->intr_tbl;
  saved_intr        = (i960_isr_entry) intr_tbl[ vector ];
  intr_tbl[vector]  = (((int) func) | 0x2); /* set IN_CACHE_IH flag */
  *isr              = (unsigned int) func | 0x2;

  if (tmrno) /* timer 1 */
  {
    *imap2 = (*imap2 & 0xff0fffff) | (((vector >> 4) & 0xf) << 20);
  }
  else /* timer 0 */
  {
    *imap2 = (*imap2 & 0xfff0ffff) | (((vector >> 4) & 0xf) << 16);
  }

#endif
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
