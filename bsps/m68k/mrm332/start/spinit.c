/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  This routine installs spurious interrupt handlers for the mrm.
 */

/*
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <bsp.h>
#include <bsp/fatal.h>
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

static rtems_isr Spurious_Isr(
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
  bsp_fatal( MRM332_FATAL_SPURIOUS_INTERRUPT );
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
