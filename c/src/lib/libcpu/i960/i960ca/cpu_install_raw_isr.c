/*
 *  Install raw interrupt vector for i960ca
 *
 *  $Id$
 */

#include <rtems.h>
#include <libcpu/i960CA.h>

#define i960_vector_caching_enabled( _prcb ) \
   ((_prcb)->control_tbl->icon & 0x2000)

extern i960_PRCB *Prcb;

void _CPU_ISR_install_raw_handler(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  i960_PRCB   *prcb = Prcb;
  proc_ptr    *cached_intr_tbl = NULL;

  /*  The i80960CA does not support vectors 0-7.  The first 9 entries
   *  in the Interrupt Table are used to manage pending interrupts.
   *  Thus vector 8, the first valid vector number, is actually in
   *  slot 9 in the table.
   */

  *old_handler = prcb->intr_tbl[ vector + 1 ];

  prcb->intr_tbl[ vector + 1 ] = new_handler;

  if ( i960_vector_caching_enabled( prcb ) )
    if ( (vector & 0xf) == 0x2 )       /* cacheable? */
      cached_intr_tbl[ vector >> 4 ] = new_handler;
}

