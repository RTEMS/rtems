/*
 *  C4X Spurious Trap Handler
 *
 *  This is just enough of a trap handler to let us know what 
 *  the likely source of the trap was.
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

#include <bsp.h>
#include <rtems/bspIo.h>

/*
 *  bsp_spurious_handler
 *
 *  Print a message on the debug console and then die
 */

void bsp_spurious_handler_assistant(
  rtems_vector_number,
  CPU_Interrupt_frame *
);

#if defined(_C4x)
rtems_isr bsp_spurious_handler(
   rtems_vector_number  vector,
   CPU_Interrupt_frame *isf
)
{
  char *s;

  printk( "Unexpected interrupt (0x%x)\n", vector );
  printk( "It looks like we got the interrupt at 0x%x\n", isf->interrupted );

  /*
   *  Can we print a name?
   */

  printk( "Looks like it was a " );
  if ( vector >= 0x0d && vector <= 0x24 ) {
    switch ( vector & 0x3 ) {
      case 1: s = "ICFULL"; break;
      case 2: s = "ICRDY"; break;
      case 3: s = "OCRDY"; break;
      case 0: s = "OCEMPTY"; break;
    }
    printk( "%s%d\n", s, (vector - 0x0d) / 4 );
  } else if ( vector >= 0x25 && vector <= 0x2a ) {
    printk( "DMA INT%d\n", (vector - 0x25) );
  } else /* if ( vector <= 0x0c || vector >= 0x2b ) */ {
    switch ( vector ) {
      case 0:     s = "RESET";     break;
      case 1:     s = "NMI";       break;
      case 2:     s = "TINT0";     break;
      case 3:     s = "IIOF0";     break;
      case 4:     s = "IIOF1";     break;
      case 5:     s = "IIOF2";     break;
      case 6:     s = "IIOF3";     break;
      case 0x2b:  s = "TINT1";     break;
      case 0x3f:  s = "Reserved";  break;
      default:    s = "Unused";    break;
    }
    printk( "%s\n", s );
  }

  /*
   *  Now call the BSP specific routine
   */

  bsp_spurious_handler_assistant( vector, isf );
}
#endif
