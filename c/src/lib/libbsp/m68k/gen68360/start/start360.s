/*  entry.s
 *
 *  This file contains the entry point for the application.
 *  The name of this entry point is compiler dependent.
 *  It jumps to the BSP which is responsible for performing
 *  all initialization.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 * Based on the `gen68302' board support package, and covered by the
 * original distribution terms.
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 *
 *  $Id$
 */

#include "asm.h"

BEGIN_CODE
	/*
	 * Step 1: Decide on Reset Stack Pointer and Initial Program Counter
	 */
Entry:
	.long	SYM(m360)+1024		|   0: Initial SSP
	.long	start			|   1: Initial PC
	.long	uhoh			|   2: Bus error
	.long	uhoh			|   3: Address error
	.long	uhoh			|   4: Illegal instruction
	.long	uhoh			|   5: Zero division
	.long	uhoh			|   6: CHK, CHK2 instruction
	.long	uhoh			|   7: TRAPcc, TRAPV instructions
	.long	uhoh			|   8: Privilege violation
	.long	uhoh			|   9: Trace
	.long	uhoh			|  10: Line 1010 emulator
	.long	uhoh			|  11: Line 1111 emulator
	.long	uhoh			|  12: Hardware breakpoint
	.long	uhoh			|  13: Reserved for coprocessor violation
	.long	uhoh			|  14: Format error
	.long	uhoh			|  15: Uninitialized interrupt
	.long	uhoh			|  16: Unassigned, reserved
	.long	uhoh			|  17:
	.long	uhoh			|  18:
	.long	uhoh			|  19:
	.long	uhoh			|  20:
	.long	uhoh			|  21:
	.long	uhoh			|  22:
	.long	uhoh			|  23:
	.long	uhoh			|  24: Spurious interrupt
	.long	uhoh			|  25: Level 1 interrupt autovector
	.long	uhoh			|  26: Level 2 interrupt autovector
	.long	uhoh			|  27: Level 3 interrupt autovector
	.long	uhoh			|  28: Level 4 interrupt autovector
	.long	uhoh			|  29: Level 5 interrupt autovector
	.long	uhoh			|  30: Level 6 interrupt autovector
	.long	uhoh			|  31: Level 7 interrupt autovector
	.long	uhoh			|  32: Trap instruction (0-15)
	.long	uhoh			|  33:
	.long	uhoh			|  34:
	.long	uhoh			|  35:
	.long	uhoh			|  36:
	.long	uhoh			|  37:
	.long	uhoh			|  38:
	.long	uhoh			|  39:
	.long	uhoh			|  40:
	.long	uhoh			|  41:
	.long	uhoh			|  42:
	.long	uhoh			|  43:
	.long	uhoh			|  44:
	.long	uhoh			|  45:
	.long	uhoh			|  46:
	.long	uhoh			|  47:
	.long	uhoh			|  48: Reserved for coprocessor
	.long	uhoh			|  49:
	.long	uhoh			|  50:
	.long	uhoh			|  51:
	.long	uhoh			|  52:
	.long	uhoh			|  53:
	.long	uhoh			|  54:
	.long	uhoh			|  55:
	.long	uhoh			|  56:
	.long	uhoh			|  57:
	.long	uhoh			|  58:
	.long	uhoh			|  59: Unassigned, reserved
	.long	uhoh			|  60:
	.long	uhoh			|  61:
	.long	uhoh			|  62:
	.long	uhoh			|  63:
	.long	uhoh			|  64: User defined vectors (192)
	.long	uhoh			|  65:
	.long	uhoh			|  66:
	.long	uhoh			|  67:
	.long	uhoh			|  68:
	.long	uhoh			|  69:
	.long	uhoh			|  70:
	.long	uhoh			|  71:
	.long	uhoh			|  72:
	.long	uhoh			|  73:
	.long	uhoh			|  74:
	.long	uhoh			|  75:
	.long	uhoh			|  76:
	.long	uhoh			|  77:
	.long	uhoh			|  78:
	.long	uhoh			|  79:
	.long	uhoh			|  80:
	.long	uhoh			|  81:
	.long	uhoh			|  82:
	.long	uhoh			|  83:
	.long	uhoh			|  84:
	.long	uhoh			|  85:
	.long	uhoh			|  86:
	.long	uhoh			|  87:
	.long	uhoh			|  88:
	.long	uhoh			|  89:
	.long	uhoh			|  90:
	.long	uhoh			|  91:
	.long	uhoh			|  92:
	.long	uhoh			|  93:
	.long	uhoh			|  94:
	.long	uhoh			|  95:
	.long	uhoh			|  96:
	.long	uhoh			|  97:
	.long	uhoh			|  98:
	.long	uhoh			|  99:
	.long	uhoh			| 100:
	.long	uhoh			| 101:
	.long	uhoh			| 102:
	.long	uhoh			| 103:
	.long	uhoh			| 104:
	.long	uhoh			| 105:
	.long	uhoh			| 106:
	.long	uhoh			| 107:
	.long	uhoh			| 108:
	.long	uhoh			| 109:
	.long	uhoh			| 110:
	.long	uhoh			| 111:
	.long	uhoh			| 112:
	.long	uhoh			| 113:
	.long	uhoh			| 114:
	.long	uhoh			| 115:
	.long	uhoh			| 116:
	.long	uhoh			| 117:
	.long	uhoh			| 118:
	.long	uhoh			| 119:
	.long	uhoh			| 120:
	.long	uhoh			| 121:
	.long	uhoh			| 122:
	.long	uhoh			| 123:
	.long	uhoh			| 124:
	.long	uhoh			| 125:
	.long	uhoh			| 126:
	.long	uhoh			| 127:
	.long	uhoh			| 128:
	.long	uhoh			| 129:
	.long	uhoh			| 130:
	.long	uhoh			| 131:
	.long	uhoh			| 132:
	.long	uhoh			| 133:
	.long	uhoh			| 134:
	.long	uhoh			| 135:
	.long	uhoh			| 136:
	.long	uhoh			| 137:
	.long	uhoh			| 138:
	.long	uhoh			| 139:
	.long	uhoh			| 140:
	.long	uhoh			| 141:
	.long	uhoh			| 142:
	.long	uhoh			| 143:
	.long	uhoh			| 144:
	.long	uhoh			| 145:
	.long	uhoh			| 146:
	.long	uhoh			| 147:
	.long	uhoh			| 148:
	.long	uhoh			| 149:
	.long	uhoh			| 150:
	.long	uhoh			| 151:
	.long	uhoh			| 152:
	.long	uhoh			| 153:
	.long	uhoh			| 154:
	.long	uhoh			| 155:
	.long	uhoh			| 156:
	.long	uhoh			| 157:
	.long	uhoh			| 158:
	.long	uhoh			| 159:
	.long	uhoh			| 160:
	.long	uhoh			| 161:
	.long	uhoh			| 162:
	.long	uhoh			| 163:
	.long	uhoh			| 164:
	.long	uhoh			| 165:
	.long	uhoh			| 166:
	.long	uhoh			| 167:
	.long	uhoh			| 168:
	.long	uhoh			| 169:
	.long	uhoh			| 170:
	.long	uhoh			| 171:
	.long	uhoh			| 172:
	.long	uhoh			| 173:
	.long	uhoh			| 174:
	.long	uhoh			| 175:
	.long	uhoh			| 176:
	.long	uhoh			| 177:
	.long	uhoh			| 178:
	.long	uhoh			| 179:
	.long	uhoh			| 180:
	.long	uhoh			| 181:
	.long	uhoh			| 182:
	.long	uhoh			| 183:
	.long	uhoh			| 184:
	.long	uhoh			| 185:
	.long	uhoh			| 186:
	.long	uhoh			| 187:
	.long	uhoh			| 188:
	.long	uhoh			| 189:
	.long	uhoh			| 190:
	.long	uhoh			| 191:
	.long	uhoh			| 192:
	.long	uhoh			| 193:
	.long	uhoh			| 194:
	.long	uhoh			| 195:
	.long	uhoh			| 196:
	.long	uhoh			| 197:
	.long	uhoh			| 198:
	.long	uhoh			| 199:
	.long	uhoh			| 200:
	.long	uhoh			| 201:
	.long	uhoh			| 202:
	.long	uhoh			| 203:
	.long	uhoh			| 204:
	.long	uhoh			| 205:
	.long	uhoh			| 206:
	.long	uhoh			| 207:
	.long	uhoh			| 208:
	.long	uhoh			| 209:
	.long	uhoh			| 210:
	.long	uhoh			| 211:
	.long	uhoh			| 212:
	.long	uhoh			| 213:
	.long	uhoh			| 214:
	.long	uhoh			| 215:
	.long	uhoh			| 216:
	.long	uhoh			| 217:
	.long	uhoh			| 218:
	.long	uhoh			| 219:
	.long	uhoh			| 220:
	.long	uhoh			| 221:
	.long	uhoh			| 222:
	.long	uhoh			| 223:
	.long	uhoh			| 224:
	.long	uhoh			| 225:
	.long	uhoh			| 226:
	.long	uhoh			| 227:
	.long	uhoh			| 228:
	.long	uhoh			| 229:
	.long	uhoh			| 230:
	.long	uhoh			| 231:
	.long	uhoh			| 232:
	.long	uhoh			| 233:
	.long	uhoh			| 234:
	.long	uhoh			| 235:
	.long	uhoh			| 236:
	.long	uhoh			| 237:
	.long	uhoh			| 238:
	.long	uhoh			| 239:
	.long	uhoh			| 240:
	.long	uhoh			| 241:
	.long	uhoh			| 242:
	.long	uhoh			| 243:
	.long	uhoh			| 244:
	.long	uhoh			| 245:
	.long	uhoh			| 246:
	.long	uhoh			| 247:
	.long	uhoh			| 248:
	.long	uhoh			| 249:
	.long	uhoh			| 250:
	.long	uhoh			| 251:
	.long	uhoh			| 252:
	.long	uhoh			| 253:
	.long	uhoh			| 254:
	.long	uhoh			| 255:

/*
 * Default trap handler
 * With an oscilloscope you can see AS* stop
 */
uhoh:	nop				| Leave spot for breakpoint
	stop	#0x2700			| Stop with interrupts disabled
	bra.b	uhoh			| Stuck forever

/*
 * Place the low-order 3 octets of the board's
 * ethernet address at a `well-known' location.
 */
	.align 2
|	.long	ETHERNET_ADDRESS	| 08: Low-order 3 octets

/*
 * Initial PC
 */
start:	
	/*
	 * Step 2: Stay in Supervisor Mode
	 * (i.e. just do nothing for this step)
	 */

	/*
	 * Step 3: Write the VBR
	 */
	lea	Entry,a0		| Get base of vector table
	movec	a0,VBR			| Set up the VBR

	/*
	 * Step 4: Write the MBAR
	 */
	movec	DFC,d1			| Save destination register
	moveq	#7,d0			| CPU-space funcction code
	movec	d0,DFC			| Set destination function code register
	movel	#SYM(m360)+0x101,d0	| MBAR value (mask CPU space accesses)
	movesl	d0,0x3FF00		| Set MBAR
	movec	d1,DFC			| Restore destination register

	/*
	 * Step 5: Verify a dual-port RAM location
	 */
	lea	SYM(m360),a0		| Point a0 to first DPRAM location
	moveb	#0x33,d0		| Set the test value
	moveb	d0,a0@			| Set the memory location
	cmpb	a0@,d0			| Does it read back?
	bne	uhoh			| If not, bad news!
	notb	d0			| Flip bits
	moveb	d0,a0@			| Set the memory location
	cmpb	a0@,d0			| Does it read back?
	bne	uhoh			| If not, bad news!

	/*
	 * Remaining steps are handled by C code
	 */
	jmp	SYM(_Init68360)		| Start C code (which never returns)


/*
 * Clear BSS, set up real stack, initialize heap, start C program
 * Assume that BSS size is a multiple of 4.
 * FIXME: The zero-loop should be changed to put the CPU into loop-mode.
 * FIXME: PROM-based systems will have to copy data segment to RAM here
 */
	PUBLIC (_ClearBSSAndStart)
SYM(_ClearBSSAndStart):
	movel	#clear_start,a0
	movel	#clear_end,a1
	clrl	d0
	brab	ZEROLOOPTEST
ZEROLOOP:
	movel	d0,a0@+
ZEROLOOPTEST:
	cmpl	a1,a0
	bcsb	ZEROLOOP
	movel	#stack_init,a7		| set master stack pointer
	movel	d0,a7@-			| environp
	movel	d0,a7@-			| argv
	movel	d0,a7@-			| argc
	jsr	SYM(main)		| Call C main

					| Should this just force a reset?
mainDone:	nop			| Leave spot for breakpoint
	stop	#0x2700			| Stop with interrupts disabled
	bra.b	mainDone		| Stuck forever

        .align 2
	PUBLIC (_HeapSize)
SYM (_HeapSize):
        .long  HeapSize
	PUBLIC (_StackSize)
SYM (_StackSize):
        .long  StackSize
END_CODE

BEGIN_BSS
        .align 2
	PUBLIC (environ)
SYM (environ):
	.long	0
END
