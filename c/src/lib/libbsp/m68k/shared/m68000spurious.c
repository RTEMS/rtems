/*
 *  C3X Spurious Trap Handler
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
 */

#include <bsp.h>
#include <rtems/bspIo.h>

/*
 *  bsp_spurious_handler
 *
 *  Print a message on the debug console and then die
 */

void bsp_spurious_handler_assistant(
  rtems_vector_number /*,
  CPU_Interrupt_frame * */
);

rtems_isr bsp_spurious_handler(
   rtems_vector_number  vector /*,
   CPU_Interrupt_frame *isf */
)
{
  char *s;

  printk( "Unexpected interrupt (0x%x)\n", vector );
/*
  printk( "It looks like we got the interrupt at 0x%x\n", isf->interrupted );
*/

  /*
   *  Can we print a name?
   */

  s = 0;
#if 0
  if ( vector <= 0x1f ) {
    switch ( vector ) {
      case 1:     s = "INT0";     break;
      case 2:     s = "INT1";     break;
      case 3:     s = "INT2";     break;
      case 4:     s = "INT3";     break;
      case 5:     s = "XINT0";    break;
      case 6:     s = "RINT0";    break;
      case 7:     s = "XINT1";    break;
      case 8:     s = "RINT1";    break;
      case 9:     s = "TINT0";    break;
      case 0x0a:  s = "TINT1";    break;
      case 0x0b:  s = "DINT0";    break;
      case 0x0c:  s = "DINT1";    break;
      default:    s = "Reserved"; break;
    }
    printk( "Looks like it was an %s\n", s );
  } else {
    printk( "Looks like it was a TRAP%d\n", vector - 0x20 );
  }
#endif

  /*
   *  Now call the BSP specific routine
   */

  bsp_spurious_handler_assistant( vector/* , 0 */ );
}
