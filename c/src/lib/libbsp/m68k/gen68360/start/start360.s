/*
 *
 *  This file contains the entry point for the application.
 *  The name of this entry point is compiler dependent.
 *  It jumps to the BSP which is responsible for performing
 *  all initialization.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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
	.long	SYM(_uhoh)		|   2: Bus error
	.long	SYM(_uhoh)		|   3: Address error
	.long	SYM(_uhoh)		|   4: Illegal instruction
	.long	SYM(_uhoh)		|   5: Zero division
	.long	SYM(_uhoh)		|   6: CHK, CHK2 instruction
	.long	SYM(_uhoh)		|   7: TRAPcc, TRAPV instructions
	.long	SYM(_uhoh)		|   8: Privilege violation
	.long	SYM(_uhoh)		|   9: Trace
	.long	SYM(_uhoh)		|  10: Line 1010 emulator
	.long	SYM(_uhoh)		|  11: Line 1111 emulator
	.long	SYM(_uhoh)		|  12: Hardware breakpoint
	.long	SYM(_uhoh)		|  13: Reserved for coprocessor violation
	.long	SYM(_uhoh)		|  14: Format error
	.long	SYM(_uhoh)		|  15: Uninitialized interrupt
	.long	SYM(_uhoh)		|  16: Unassigned, reserved
	.long	SYM(_uhoh)		|  17:
	.long	SYM(_uhoh)		|  18:
	.long	SYM(_uhoh)		|  19:
	.long	SYM(_uhoh)		|  20:
	.long	SYM(_uhoh)		|  21:
	.long	SYM(_uhoh)		|  22:
	.long	SYM(_uhoh)		|  23:
	.long	SYM(_spuriousInterrupt)	|  24: Spurious interrupt
	.long	SYM(_uhoh)		|  25: Level 1 interrupt autovector
	.long	SYM(_uhoh)		|  26: Level 2 interrupt autovector
	.long	SYM(_uhoh)		|  27: Level 3 interrupt autovector
	.long	SYM(_uhoh)		|  28: Level 4 interrupt autovector
	.long	SYM(_uhoh)		|  29: Level 5 interrupt autovector
	.long	SYM(_uhoh)		|  30: Level 6 interrupt autovector
	.long	SYM(_uhoh)		|  31: Level 7 interrupt autovector
	.long	SYM(_uhoh)		|  32: Trap instruction (0-15)
	.long	SYM(_uhoh)		|  33:
	.long	SYM(_uhoh)		|  34:
	.long	SYM(_uhoh)		|  35:
	.long	SYM(_uhoh)		|  36:
	.long	SYM(_uhoh)		|  37:
	.long	SYM(_uhoh)		|  38:
	.long	SYM(_uhoh)		|  39:
	.long	SYM(_uhoh)		|  40:
	.long	SYM(_uhoh)		|  41:
	.long	SYM(_uhoh)		|  42:
	.long	SYM(_uhoh)		|  43:
	.long	SYM(_uhoh)		|  44:
	.long	SYM(_uhoh)		|  45:
	.long	SYM(_uhoh)		|  46:
	.long	SYM(_uhoh)		|  47:
	.long	SYM(_uhoh)		|  48: Reserved for coprocessor
	.long	SYM(_uhoh)		|  49:
	.long	SYM(_uhoh)		|  50:
	.long	SYM(_uhoh)		|  51:
	.long	SYM(_uhoh)		|  52:
	.long	SYM(_uhoh)		|  53:
	.long	SYM(_uhoh)		|  54:
	.long	SYM(_uhoh)		|  55:
	.long	SYM(_uhoh)		|  56:
	.long	SYM(_uhoh)		|  57:
	.long	SYM(_uhoh)		|  58:
	.long	SYM(_uhoh)		|  59: Unassigned, reserved
	.long	SYM(_uhoh)		|  60:
	.long	SYM(_uhoh)		|  61:
	.long	SYM(_uhoh)		|  62:
	.long	SYM(_uhoh)		|  63:
	.long	SYM(_uhoh)		|  64: User defined vectors (192)
	.long	SYM(_uhoh)		|  65:
	.long	SYM(_uhoh)		|  66:
	.long	SYM(_uhoh)		|  67:
	.long	SYM(_uhoh)		|  68:
	.long	SYM(_uhoh)		|  69:
	.long	SYM(_uhoh)		|  70:
	.long	SYM(_uhoh)		|  71:
	.long	SYM(_uhoh)		|  72:
	.long	SYM(_uhoh)		|  73:
	.long	SYM(_uhoh)		|  74:
	.long	SYM(_uhoh)		|  75:
	.long	SYM(_uhoh)		|  76:
	.long	SYM(_uhoh)		|  77:
	.long	SYM(_uhoh)		|  78:
	.long	SYM(_uhoh)		|  79:
	.long	SYM(_uhoh)		|  80:
	.long	SYM(_uhoh)		|  81:
	.long	SYM(_uhoh)		|  82:
	.long	SYM(_uhoh)		|  83:
	.long	SYM(_uhoh)		|  84:
	.long	SYM(_uhoh)		|  85:
	.long	SYM(_uhoh)		|  86:
	.long	SYM(_uhoh)		|  87:
	.long	SYM(_uhoh)		|  88:
	.long	SYM(_uhoh)		|  89:
	.long	SYM(_uhoh)		|  90:
	.long	SYM(_uhoh)		|  91:
	.long	SYM(_uhoh)		|  92:
	.long	SYM(_uhoh)		|  93:
	.long	SYM(_uhoh)		|  94:
	.long	SYM(_uhoh)		|  95:
	.long	SYM(_uhoh)		|  96:
	.long	SYM(_uhoh)		|  97:
	.long	SYM(_uhoh)		|  98:
	.long	SYM(_uhoh)		|  99:
	.long	SYM(_uhoh)		| 100:
	.long	SYM(_uhoh)		| 101:
	.long	SYM(_uhoh)		| 102:
	.long	SYM(_uhoh)		| 103:
	.long	SYM(_uhoh)		| 104:
	.long	SYM(_uhoh)		| 105:
	.long	SYM(_uhoh)		| 106:
	.long	SYM(_uhoh)		| 107:
	.long	SYM(_uhoh)		| 108:
	.long	SYM(_uhoh)		| 109:
	.long	SYM(_uhoh)		| 110:
	.long	SYM(_uhoh)		| 111:
	.long	SYM(_uhoh)		| 112:
	.long	SYM(_uhoh)		| 113:
	.long	SYM(_uhoh)		| 114:
	.long	SYM(_uhoh)		| 115:
	.long	SYM(_uhoh)		| 116:
	.long	SYM(_uhoh)		| 117:
	.long	SYM(_uhoh)		| 118:
	.long	SYM(_uhoh)		| 119:
	.long	SYM(_uhoh)		| 120:
	.long	SYM(_uhoh)		| 121:
	.long	SYM(_uhoh)		| 122:
	.long	SYM(_uhoh)		| 123:
	.long	SYM(_uhoh)		| 124:
	.long	SYM(_uhoh)		| 125:
	.long	SYM(_uhoh)		| 126:
	.long	SYM(_uhoh)		| 127:
	.long	SYM(_uhoh)		| 128:
	.long	SYM(_uhoh)		| 129:
	.long	SYM(_uhoh)		| 130:
	.long	SYM(_uhoh)		| 131:
	.long	SYM(_uhoh)		| 132:
	.long	SYM(_uhoh)		| 133:
	.long	SYM(_uhoh)		| 134:
	.long	SYM(_uhoh)		| 135:
	.long	SYM(_uhoh)		| 136:
	.long	SYM(_uhoh)		| 137:
	.long	SYM(_uhoh)		| 138:
	.long	SYM(_uhoh)		| 139:
	.long	SYM(_uhoh)		| 140:
	.long	SYM(_uhoh)		| 141:
	.long	SYM(_uhoh)		| 142:
	.long	SYM(_uhoh)		| 143:
	.long	SYM(_uhoh)		| 144:
	.long	SYM(_uhoh)		| 145:
	.long	SYM(_uhoh)		| 146:
	.long	SYM(_uhoh)		| 147:
	.long	SYM(_uhoh)		| 148:
	.long	SYM(_uhoh)		| 149:
	.long	SYM(_uhoh)		| 150:
	.long	SYM(_uhoh)		| 151:
	.long	SYM(_uhoh)		| 152:
	.long	SYM(_uhoh)		| 153:
	.long	SYM(_uhoh)		| 154:
	.long	SYM(_uhoh)		| 155:
	.long	SYM(_uhoh)		| 156:
	.long	SYM(_uhoh)		| 157:
	.long	SYM(_uhoh)		| 158:
	.long	SYM(_uhoh)		| 159:
	.long	SYM(_uhoh)		| 160:
	.long	SYM(_uhoh)		| 161:
	.long	SYM(_uhoh)		| 162:
	.long	SYM(_uhoh)		| 163:
	.long	SYM(_uhoh)		| 164:
	.long	SYM(_uhoh)		| 165:
	.long	SYM(_uhoh)		| 166:
	.long	SYM(_uhoh)		| 167:
	.long	SYM(_uhoh)		| 168:
	.long	SYM(_uhoh)		| 169:
	.long	SYM(_uhoh)		| 170:
	.long	SYM(_uhoh)		| 171:
	.long	SYM(_uhoh)		| 172:
	.long	SYM(_uhoh)		| 173:
	.long	SYM(_uhoh)		| 174:
	.long	SYM(_uhoh)		| 175:
	.long	SYM(_uhoh)		| 176:
	.long	SYM(_uhoh)		| 177:
	.long	SYM(_uhoh)		| 178:
	.long	SYM(_uhoh)		| 179:
	.long	SYM(_uhoh)		| 180:
	.long	SYM(_uhoh)		| 181:
	.long	SYM(_uhoh)		| 182:
	.long	SYM(_uhoh)		| 183:
	.long	SYM(_uhoh)		| 184:
	.long	SYM(_uhoh)		| 185:
	.long	SYM(_uhoh)		| 186:
	.long	SYM(_uhoh)		| 187:
	.long	SYM(_uhoh)		| 188:
	.long	SYM(_uhoh)		| 189:
	.long	SYM(_uhoh)		| 190:
	.long	SYM(_uhoh)		| 191:
	.long	SYM(_uhoh)		| 192:
	.long	SYM(_uhoh)		| 193:
	.long	SYM(_uhoh)		| 194:
	.long	SYM(_uhoh)		| 195:
	.long	SYM(_uhoh)		| 196:
	.long	SYM(_uhoh)		| 197:
	.long	SYM(_uhoh)		| 198:
	.long	SYM(_uhoh)		| 199:
	.long	SYM(_uhoh)		| 200:
	.long	SYM(_uhoh)		| 201:
	.long	SYM(_uhoh)		| 202:
	.long	SYM(_uhoh)		| 203:
	.long	SYM(_uhoh)		| 204:
	.long	SYM(_uhoh)		| 205:
	.long	SYM(_uhoh)		| 206:
	.long	SYM(_uhoh)		| 207:
	.long	SYM(_uhoh)		| 208:
	.long	SYM(_uhoh)		| 209:
	.long	SYM(_uhoh)		| 210:
	.long	SYM(_uhoh)		| 211:
	.long	SYM(_uhoh)		| 212:
	.long	SYM(_uhoh)		| 213:
	.long	SYM(_uhoh)		| 214:
	.long	SYM(_uhoh)		| 215:
	.long	SYM(_uhoh)		| 216:
	.long	SYM(_uhoh)		| 217:
	.long	SYM(_uhoh)		| 218:
	.long	SYM(_uhoh)		| 219:
	.long	SYM(_uhoh)		| 220:
	.long	SYM(_uhoh)		| 221:
	.long	SYM(_uhoh)		| 222:
	.long	SYM(_uhoh)		| 223:
	.long	SYM(_uhoh)		| 224:
	.long	SYM(_uhoh)		| 225:
	.long	SYM(_uhoh)		| 226:
	.long	SYM(_uhoh)		| 227:
	.long	SYM(_uhoh)		| 228:
	.long	SYM(_uhoh)		| 229:
	.long	SYM(_uhoh)		| 230:
	.long	SYM(_uhoh)		| 231:
	.long	SYM(_uhoh)		| 232:
	.long	SYM(_uhoh)		| 233:
	.long	SYM(_uhoh)		| 234:
	.long	SYM(_uhoh)		| 235:
	.long	SYM(_uhoh)		| 236:
	.long	SYM(_uhoh)		| 237:
	.long	SYM(_uhoh)		| 238:
	.long	SYM(_uhoh)		| 239:
	.long	SYM(_uhoh)		| 240:
	.long	SYM(_uhoh)		| 241:
	.long	SYM(_uhoh)		| 242:
	.long	SYM(_uhoh)		| 243:
	.long	SYM(_uhoh)		| 244:
	.long	SYM(_uhoh)		| 245:
	.long	SYM(_uhoh)		| 246:
	.long	SYM(_uhoh)		| 247:
	.long	SYM(_uhoh)		| 248:
	.long	SYM(_uhoh)		| 249:
	.long	SYM(_uhoh)		| 250:
	.long	SYM(_uhoh)		| 251:
	.long	SYM(_uhoh)		| 252:
	.long	SYM(_uhoh)		| 253:
	.long	SYM(_uhoh)		| 254:
	.long	SYM(_uhoh)		| 255:

/*
 * Default trap handler
 * With an oscilloscope you can see AS* stop
 */
	PUBLIC (_uhoh)
SYM(_uhoh):	nop				| Leave spot for breakpoint
	stop	#0x2700			| Stop with interrupts disabled
	bra.s	SYM(_uhoh)			| Stuck forever

/*
 * Log, but otherwise ignore, spurious interrupts
 */
	PUBLIC (_spuriousInterrupt)
SYM(_spuriousInterrupt):
	addql	#1,SYM(_M68kSpuriousInterruptCount)
	rte

/*
 * Place the low-order 3 octets of the board's ethernet address at
 * a `well-known' fixed location relative to the beginning of ROM.
 */
	.align 2
	.long	ETHERNET_ADDRESS	| Low-order 3 octets of ethernet address

/*
 *  For some reason, the symbol start must not be global.
 *
 *       .global start
 */

/*
 * Initial PC
 */
.globl start
start:
	/*
	 * Step 2: Stay in Supervisor Mode
	 */
#if ( M68K_HAS_SEPARATE_STACKS == 1 )
	oriw	#0x3000,sr		| Switch to Master Stack Pointer
	lea	SYM(m360)+1024-64,a7	| Load stack pointer with space
					|   for the Interrupt Stack
#endif

	/*
	 * Step 3: Write the VBR
	 */
	lea	Entry,a0		| Get base of vector table
	movec	a0,vbr			| Set up the VBR

	/*
	 * Step 4: Write the MBAR
	 */
	movec	dfc,d1			| Save destination register
	moveq	#7,d0			| CPU-space funcction code
	movec	d0,dfc			| Set destination function code register
	movel	#SYM(m360)+0x101,d0	| MBAR value (mask CPU space accesses)
	movesl	d0,0x3FF00		| Set MBAR
	movec	d1,dfc			| Restore destination register

	/*
	 * Step 5: Verify a dual-port RAM location
	 */
	lea	SYM(m360),a0		| Point a0 to first DPRAM location
	moveb	#0x33,d0		| Set the test value
	moveb	d0,a0@			| Set the memory location
	cmpb	a0@,d0			| Does it read back?
	bne	SYM(_uhoh)			| If not, bad news!
	notb	d0			| Flip bits
	moveb	d0,a0@			| Set the memory location
	cmpb	a0@,d0			| Does it read back?
	bne	SYM(_uhoh)			| If not, bad news!

	/*
	 * Remaining steps are handled by C code
	 */
	jmp	SYM(_Init68360)		| Start C code (which never returns)

/*
 * Copy DATA segment, clear BSS segment, set up real stack,
 * initialize heap, start C program.
 * Assume that DATA and BSS sizes are multiples of 4.
 */
	PUBLIC (_CopyDataClearBSSAndStart)
SYM(_CopyDataClearBSSAndStart):
	lea	copy_start,a0		| Get start of DATA in RAM
	lea	SYM(etext),a2		| Get start of DATA in ROM
	cmpl	a0,a2			| Are they the same?
	beq.s	NOCOPY			| Yes, no copy necessary
	lea	copy_end,a1		| Get end of DATA in RAM
	bra.s	COPYLOOPTEST		| Branch into copy loop
COPYLOOP:
	movel	a2@+,a0@+		| Copy word from ROM to RAM
COPYLOOPTEST:
	cmpl	a1,a0			| Done?
	bcs.s	COPYLOOP		| No, skip
NOCOPY:

	lea	clear_start,a0		| Get start of BSS
	lea	clear_end,a1		| Get end of BSS
	clrl	d0			| Value to set
	bra.s	ZEROLOOPTEST		| Branch into clear loop
ZEROLOOP:
	movel	d0,a0@+			| Clear a word
ZEROLOOPTEST:
	cmpl	a1,a0			| Done?
	bcs.s	ZEROLOOP		| No, skip

	movel	#stack_init,a7		| set master stack pointer
	movel	d0,a7@-			| environp
	movel	d0,a7@-			| argv
	movel	d0,a7@-			| argc
	jsr	SYM(boot_card)		| Call C main

	PUBLIC (_mainDone)
SYM(_mainDone):
	nop				| Leave spot for breakpoint
	movew	#1,a7			| Force a double bus error
	movel	d0,a7@-			| This should cause a RESET
	stop	#0x2700			| Stop with interrupts disabled
	bra.s	SYM(_mainDone)		| Stuck forever

        .align 2
	PUBLIC (_HeapSize)
SYM (_HeapSize):
        .long  HeapSize
	PUBLIC (_StackSize)
SYM (_StackSize):
        .long  StackSize
END_CODE

BEGIN_DATA_DCL
        .align 2
	PUBLIC (environ)
SYM (environ):
	.long	0
	PUBLIC (_M68kSpuriousInterruptCount)
SYM (_M68kSpuriousInterruptCount):
	.long	0
END_DATA_DCL

END

