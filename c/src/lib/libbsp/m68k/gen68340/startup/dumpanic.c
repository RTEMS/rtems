/*
 *  M68340/349 registers and stack dump if an exception is raised
 *
 *  Author:
 *  Pascal Cadic
 *  France Telecom - CNET/DSM/TAM/CAT
 *  4, rue du Clos Courtel
 *  35512 CESSON-SEVIGNE
 *  FRANCE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <stdio.h>

const char *exceptionName[] =
	{
	"INITIAL STACK POINTER",
	"INITIAL PROGRAM COUNTER",
	"BUS ERROR",
	"ADDRESS ERROR",
	"ILLEGAL INSTRUCTION",
	"DIVISION BY ZERO",
	"CHK, CHK2",
	"TRAPcc, TRAPv",
	"PRIVILEGE VIOLATION",
	"TRACE",
	"LINE A EMULATOR",
	"LINE F EMULATOR",
	"HARDWARE BREAK",
	"COPROCESSOR PROTOCOL VIOLATION",
	"FORMAT ERROR",
	"UNINITIALIZED INTERRUPT",
	"RESERVED 16",
	"RESERVED 17",
	"RESERVED 18",
	"RESERVED 19",
	"RESERVED 20",
	"RESERVED 21",
	"RESERVED 22",
	"RESERVED 23",
	"SPURIOUS INTERRUPT",
	"LEVEL 1 AUTOVECTOR",
	"LEVEL 2 AUTOVECTOR",
	"LEVEL 3 AUTOVECTOR",
	"LEVEL 4 AUTOVECTOR",
	"LEVEL 5 AUTOVECTOR",
	"LEVEL 6 AUTOVECTOR",
	"LEVEL 7 AUTOVECTOR",
	"TRAP 1",
	"TRAP 2",
	"TRAP 3",
	"TRAP 4",
	"TRAP 5",
	"TRAP 6",
	"TRAP 7",
	"TRAP 8",
	"TRAP 9",
	"TRAP 10",
	"TRAP 11",
	"TRAP 12",
	"TRAP 13",
	"TRAP 14",
	"TRAP 15",
	"VECTOR 48",
	"VECTOR 49",
	"VECTOR 50",
	"VECTOR 51",
	"VECTOR 52",
	"VECTOR 53",
	"VECTOR 54",
	"VECTOR 55",
	"VECTOR 56",
	"VECTOR 57",
	"VECTOR 58",
	"VECTOR 59",
	"VECTOR 60",
	"VECTOR 61",
	"VECTOR 62",
	"VECTOR 63",
	};

typedef struct {
    unsigned long	pc;
    unsigned short	sr;
    unsigned short	format_id;
    unsigned long	d0, d1, d2, d3, d4, d5, d6, d7;
    unsigned long	a0, a1, a2, a3, a4, a5, a6, a7;
    unsigned long	sfc, dfc, vbr;
} boot_panic_registers_t;

boot_panic_registers_t _boot_panic_registers;

extern void RAW_FMT( int minor, const char* fmt, ... );
extern char RAW_GETC(int minor);

/******************************************************
  Name: _dbug_dump
  Input parameters: sr, pc, stack pointer,
		    size to display
  Output parameters: -
  Description: display the supervisor stack
 *****************************************************/
void _dbug_dump(unsigned short sr, void* pc, unsigned short *stack, int size)
{
int i;

	RAW_FMT(0,"%x : %x \t%x",0,sr,(unsigned short)(((unsigned)pc)>>16));
	for (i=2; i<size; i++) {
		if ((i%8)==0) RAW_FMT(0,"\n%x :",i/8);
		RAW_FMT(0," %x\t",stack[i-2]);
	}
	RAW_FMT(0,"\n");
}

/******************************************************
  Name: _dbug_dump
  Input parameters: -
  Output parameters: -
  Description: display microcontroler state. Registers
	       values are stored in _boot_panic_registers
	       which is filled in _uhoh ASM routine
 *****************************************************/
void _dbug_dumpanic(void)
{
 int c;
 void *faultedAddr, *pc;
 unsigned short vector, status;
 unsigned char frametype, *stack;
 #define ESCAPE 27

	stack = (unsigned char*)(_boot_panic_registers.a7);
	do {
		status = _boot_panic_registers.sr;
		pc = (void*)_boot_panic_registers.pc;
		faultedAddr = *(void**)(stack+4);
		vector = (_boot_panic_registers.format_id&0x0FFF)>>2;
		frametype = (_boot_panic_registers.format_id&0xF000)>>12;

		RAW_FMT(0,"\n---------------------------------------------\n");
		if (vector<64)
			RAW_FMT(0,"%s",exceptionName[vector]);
		else {
			RAW_FMT(0,"RESERVED USER");
		}
		RAW_FMT(0," exception (vector %x, type %x)\n",vector,frametype);
		RAW_FMT(0,"---------------------------------------------\n");
		RAW_FMT(0,"PC : 0x%x  ",pc);
		RAW_FMT(0,"A7 : 0x%x  ",_boot_panic_registers.a7);
		RAW_FMT(0,"SR : 0x%x\n",status);
		if (frametype==0x0c) {
			RAW_FMT(0,"\nfaulted address = 0x%x\n",faultedAddr);
		}
		RAW_FMT(0,"---------------------------------------------\n");
		RAW_FMT(0,"               panic regs\n");
		RAW_FMT(0,"---------------------------------------------\n");
		RAW_FMT(0,"D[0..3] : %x \t%x \t%x \t%x\n",
				_boot_panic_registers.d0,_boot_panic_registers.d1,
				_boot_panic_registers.d2,_boot_panic_registers.d3);
		RAW_FMT(0,"D[4..7] : %x \t%x \t%x \t%x\n",
				_boot_panic_registers.d4,_boot_panic_registers.d5,
				_boot_panic_registers.d6,_boot_panic_registers.d7);
		RAW_FMT(0,"A[0..3] : %x \t%x \t%x \t%x\n",
				_boot_panic_registers.a0,_boot_panic_registers.a1,
				_boot_panic_registers.a2,_boot_panic_registers.a3);
		RAW_FMT(0,"A[4..7] : %x \t%x \t%x \t%x\n",
				_boot_panic_registers.a4,_boot_panic_registers.a5,
				_boot_panic_registers.a6,_boot_panic_registers.a7);

		RAW_FMT(0,"    SFC : %x",_boot_panic_registers.sfc);
		RAW_FMT(0,"    DFC : %x\n",_boot_panic_registers.dfc);
		RAW_FMT(0,"    VBR : %x\n",_boot_panic_registers.vbr);
		RAW_FMT(0,"---------------------------------------------\n");
		RAW_FMT(0,"               panic stack\n");
		RAW_FMT(0,"---------------------------------------------\n");
		_dbug_dump(status, pc, (unsigned short*)stack,64*2);

		RAW_FMT(0,"---------------------------------------------\n");
		RAW_FMT(0,"press escape to reboot\n");
	} while ((c=RAW_GETC(0))!=ESCAPE);	/* cgets ne marche pas si les IT sont bloquées */
}
