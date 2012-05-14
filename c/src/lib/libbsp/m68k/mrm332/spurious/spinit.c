/*  Spurious_driver
 *
 *  This routine installs spurious interrupt handlers for the mrm.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <stdio.h>

const char * const _Spurious_Error_[] = {"Reset","Bus Error","Address Error",
   "Illegal Instruction","Zero Division","CHK, CHK2 Instruction",
   "TRAPcc, TRAPV Instruction","Privilege Violation","Trace",
   "Line 1010 Emulation","Line 1111 Emulation","Hardware Breakpoint",
   "Coprocessor Protocal Violation",
   "Format Error ans Uninitialized Interrupt","Unassigned",
   "Spurious Interrupt","AVec1","AVec2","AVec3","AVec4","AVec5","AVec6",
   "AVec7","Trap Instruction","Debug","Reboot","Reserved Coprocessor",
   "Reserved Unassigned","User Defined"};

rtems_isr Spurious_Isr(
  rtems_vector_number vector
)
{
  /*int sp = 0; */
#if 0
  const char * const VectDescrip[] = {
    _Spurious_Error_[0],   _Spurious_Error_[0],  _Spurious_Error_[1],
    _Spurious_Error_[2],   _Spurious_Error_[3],  _Spurious_Error_[4],
    _Spurious_Error_[5],   _Spurious_Error_[6],  _Spurious_Error_[7],
    _Spurious_Error_[8],   _Spurious_Error_[9], _Spurious_Error_[10],
    _Spurious_Error_[11], _Spurious_Error_[12], _Spurious_Error_[13],
    _Spurious_Error_[13], _Spurious_Error_[14], _Spurious_Error_[14],
    _Spurious_Error_[14], _Spurious_Error_[14], _Spurious_Error_[14],
    _Spurious_Error_[14], _Spurious_Error_[14], _Spurious_Error_[14],
    _Spurious_Error_[15], _Spurious_Error_[16], _Spurious_Error_[17],
    _Spurious_Error_[18], _Spurious_Error_[19], _Spurious_Error_[20],
    _Spurious_Error_[21], _Spurious_Error_[22], _Spurious_Error_[23],
    _Spurious_Error_[24], _Spurious_Error_[23], _Spurious_Error_[23],
    _Spurious_Error_[23], _Spurious_Error_[23], _Spurious_Error_[23],
    _Spurious_Error_[23], _Spurious_Error_[23], _Spurious_Error_[23],
    _Spurious_Error_[23], _Spurious_Error_[23], _Spurious_Error_[23],
    _Spurious_Error_[23], _Spurious_Error_[23], _Spurious_Error_[25],
    _Spurious_Error_[26], _Spurious_Error_[26], _Spurious_Error_[26],
    _Spurious_Error_[26], _Spurious_Error_[26], _Spurious_Error_[26],
    _Spurious_Error_[26], _Spurious_Error_[26], _Spurious_Error_[26],
    _Spurious_Error_[26], _Spurious_Error_[26], _Spurious_Error_[27],
    _Spurious_Error_[27], _Spurious_Error_[27], _Spurious_Error_[27],
    _Spurious_Error_[27], _Spurious_Error_[28]};
#endif

  /*asm volatile ( "movea.l   %%sp,%0 " : "=a" (sp) : "0" (sp) ); */

  _CPU_ISR_Set_level( 7 );
  /*_UART_flush(); */
#if 0
  RAW_PUTS("\n\rRTEMS: Spurious interrupt: ");
  RAW_PUTS((char *)VectDescrip[( (vector>64) ? 64 : vector )]);
  RAW_PUTS("\n\rRTEMS:    Vector: ");
  RAW_PUTI(vector);
  RAW_PUTS(" sp: ");
  RAW_PUTI(sp);
  RAW_PUTS("\n\r");
#endif
  bsp_cleanup(1);

  /* BDM SIGEMT */
  __asm__ ("  .word  0x4afa");

  for(;;);
}

void Spurious_Initialize(void)
{
  rtems_vector_number vector;

  for ( vector = 0x0 ; vector <= 0xFF ; vector++ )
    {
      switch (vector)
	{
	case 4:
	case 9:
	case 31:
	case 47:
	case 66:
	  /* These vectors used by CPU32bug - don't overwrite them. */
	  break;

	default:
	  (void) set_vector( Spurious_Isr, vector, 1 );
	  break;
	}
    }
}
