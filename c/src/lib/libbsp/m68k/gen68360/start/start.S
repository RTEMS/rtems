/*
 *
 *  This file contains the entry point for the application.
 *  It jumps to the BSP which is responsible for performing
 *  all initialization.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 * Based on the `gen68302' board support package, and covered by the
 * original distribution terms.
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 */

#include <rtems/asm.h>

BEGIN_CODE
	/*
	 * Step 1: Decide on Reset Stack Pointer and Initial Program Counter
	 */
Entry:
	.long	m360+1024		|   0: Initial SSP
	.long	start			|   1: Initial PC
	.long	_uhoh			|   2: Bus error
	.long	_uhoh			|   3: Address error
	.long	_uhoh			|   4: Illegal instruction
	.long	_uhoh			|   5: Zero division
	.long	_uhoh			|   6: CHK, CHK2 instruction
	.long	_uhoh			|   7: TRAPcc, TRAPV instructions
	.long	_uhoh			|   8: Privilege violation
	.long	_uhoh			|   9: Trace
	.long	_uhoh			|  10: Line 1010 emulator
	.long	_uhoh			|  11: Line 1111 emulator
	.long	_uhoh			|  12: Hardware breakpoint
	.long	_uhoh			|  13: Reserved for coprocessor violation
	.long	_uhoh			|  14: Format error
	.long	_uhoh			|  15: Uninitialized interrupt
	.long	_uhoh			|  16: Unassigned, reserved
	.long	_uhoh			|  17:
	.long	_uhoh			|  18:
	.long	_uhoh			|  19:
	.long	_uhoh			|  20:
	.long	_uhoh			|  21:
	.long	_uhoh			|  22:
	.long	_uhoh			|  23:
	.long	_spuriousInterrupt	|  24: Spurious interrupt
	.long	_uhoh			|  25: Level 1 interrupt autovector
	.long	_uhoh			|  26: Level 2 interrupt autovector
	.long	_uhoh			|  27: Level 3 interrupt autovector
	.long	_uhoh			|  28: Level 4 interrupt autovector
	.long	_uhoh			|  29: Level 5 interrupt autovector
	.long	_uhoh			|  30: Level 6 interrupt autovector
	.long	_uhoh			|  31: Level 7 interrupt autovector
	.long	_uhoh			|  32: Trap instruction (0-15)
	.long	_uhoh			|  33:
	.long	_uhoh			|  34:
	.long	_uhoh			|  35:
	.long	_uhoh			|  36:
	.long	_uhoh			|  37:
	.long	_uhoh			|  38:
	.long	_uhoh			|  39:
	.long	_uhoh			|  40:
	.long	_uhoh			|  41:
	.long	_uhoh			|  42:
	.long	_uhoh			|  43:
	.long	_uhoh			|  44:
	.long	_uhoh			|  45:
	.long	_uhoh			|  46:
	.long	_uhoh			|  47:
	.long	_uhoh			|  48: Reserved for coprocessor
	.long	_uhoh			|  49:
	.long	_uhoh			|  50:
	.long	_uhoh			|  51:
	.long	_uhoh			|  52:
	.long	_uhoh			|  53:
	.long	_uhoh			|  54:
	.long	_uhoh			|  55:
	.long	_uhoh			|  56:
	.long	_uhoh			|  57:
	.long	_uhoh			|  58:
	.long	_uhoh			|  59: Unassigned, reserved
	.long	_uhoh			|  60:
	.long	_uhoh			|  61:
	.long	_uhoh			|  62:
	.long	_uhoh			|  63:
	.long	_uhoh			|  64: User defined vectors (192)
	.long	_uhoh			|  65:
	.long	_uhoh			|  66:
	.long	_uhoh			|  67:
	.long	_uhoh			|  68:
	.long	_uhoh			|  69:
	.long	_uhoh			|  70:
	.long	_uhoh			|  71:
	.long	_uhoh			|  72:
	.long	_uhoh			|  73:
	.long	_uhoh			|  74:
	.long	_uhoh			|  75:
	.long	_uhoh			|  76:
	.long	_uhoh			|  77:
	.long	_uhoh			|  78:
	.long	_uhoh			|  79:
	.long	_uhoh			|  80:
	.long	_uhoh			|  81:
	.long	_uhoh			|  82:
	.long	_uhoh			|  83:
	.long	_uhoh			|  84:
	.long	_uhoh			|  85:
	.long	_uhoh			|  86:
	.long	_uhoh			|  87:
	.long	_uhoh			|  88:
	.long	_uhoh			|  89:
	.long	_uhoh			|  90:
	.long	_uhoh			|  91:
	.long	_uhoh			|  92:
	.long	_uhoh			|  93:
	.long	_uhoh			|  94:
	.long	_uhoh			|  95:
	.long	_uhoh			|  96:
	.long	_uhoh			|  97:
	.long	_uhoh			|  98:
	.long	_uhoh			|  99:
	.long	_uhoh			| 100:
	.long	_uhoh			| 101:
	.long	_uhoh			| 102:
	.long	_uhoh			| 103:
	.long	_uhoh			| 104:
	.long	_uhoh			| 105:
	.long	_uhoh			| 106:
	.long	_uhoh			| 107:
	.long	_uhoh			| 108:
	.long	_uhoh			| 109:
	.long	_uhoh			| 110:
	.long	_uhoh			| 111:
	.long	_uhoh			| 112:
	.long	_uhoh			| 113:
	.long	_uhoh			| 114:
	.long	_uhoh			| 115:
	.long	_uhoh			| 116:
	.long	_uhoh			| 117:
	.long	_uhoh			| 118:
	.long	_uhoh			| 119:
	.long	_uhoh			| 120:
	.long	_uhoh			| 121:
	.long	_uhoh			| 122:
	.long	_uhoh			| 123:
	.long	_uhoh			| 124:
	.long	_uhoh			| 125:
	.long	_uhoh			| 126:
	.long	_uhoh			| 127:
	.long	_uhoh			| 128:
	.long	_uhoh			| 129:
	.long	_uhoh			| 130:
	.long	_uhoh			| 131:
	.long	_uhoh			| 132:
	.long	_uhoh			| 133:
	.long	_uhoh			| 134:
	.long	_uhoh			| 135:
	.long	_uhoh			| 136:
	.long	_uhoh			| 137:
	.long	_uhoh			| 138:
	.long	_uhoh			| 139:
	.long	_uhoh			| 140:
	.long	_uhoh			| 141:
	.long	_uhoh			| 142:
	.long	_uhoh			| 143:
	.long	_uhoh			| 144:
	.long	_uhoh			| 145:
	.long	_uhoh			| 146:
	.long	_uhoh			| 147:
	.long	_uhoh			| 148:
	.long	_uhoh			| 149:
	.long	_uhoh			| 150:
	.long	_uhoh			| 151:
	.long	_uhoh			| 152:
	.long	_uhoh			| 153:
	.long	_uhoh			| 154:
	.long	_uhoh			| 155:
	.long	_uhoh			| 156:
	.long	_uhoh			| 157:
	.long	_uhoh			| 158:
	.long	_uhoh			| 159:
	.long	_uhoh			| 160:
	.long	_uhoh			| 161:
	.long	_uhoh			| 162:
	.long	_uhoh			| 163:
	.long	_uhoh			| 164:
	.long	_uhoh			| 165:
	.long	_uhoh			| 166:
	.long	_uhoh			| 167:
	.long	_uhoh			| 168:
	.long	_uhoh			| 169:
	.long	_uhoh			| 170:
	.long	_uhoh			| 171:
	.long	_uhoh			| 172:
	.long	_uhoh			| 173:
	.long	_uhoh			| 174:
	.long	_uhoh			| 175:
	.long	_uhoh			| 176:
	.long	_uhoh			| 177:
	.long	_uhoh			| 178:
	.long	_uhoh			| 179:
	.long	_uhoh			| 180:
	.long	_uhoh			| 181:
	.long	_uhoh			| 182:
	.long	_uhoh			| 183:
	.long	_uhoh			| 184:
	.long	_uhoh			| 185:
	.long	_uhoh			| 186:
	.long	_uhoh			| 187:
	.long	_uhoh			| 188:
	.long	_uhoh			| 189:
	.long	_uhoh			| 190:
	.long	_uhoh			| 191:
	.long	_uhoh			| 192:
	.long	_uhoh			| 193:
	.long	_uhoh			| 194:
	.long	_uhoh			| 195:
	.long	_uhoh			| 196:
	.long	_uhoh			| 197:
	.long	_uhoh			| 198:
	.long	_uhoh			| 199:
	.long	_uhoh			| 200:
	.long	_uhoh			| 201:
	.long	_uhoh			| 202:
	.long	_uhoh			| 203:
	.long	_uhoh			| 204:
	.long	_uhoh			| 205:
	.long	_uhoh			| 206:
	.long	_uhoh			| 207:
	.long	_uhoh			| 208:
	.long	_uhoh			| 209:
	.long	_uhoh			| 210:
	.long	_uhoh			| 211:
	.long	_uhoh			| 212:
	.long	_uhoh			| 213:
	.long	_uhoh			| 214:
	.long	_uhoh			| 215:
	.long	_uhoh			| 216:
	.long	_uhoh			| 217:
	.long	_uhoh			| 218:
	.long	_uhoh			| 219:
	.long	_uhoh			| 220:
	.long	_uhoh			| 221:
	.long	_uhoh			| 222:
	.long	_uhoh			| 223:
	.long	_uhoh			| 224:
	.long	_uhoh			| 225:
	.long	_uhoh			| 226:
	.long	_uhoh			| 227:
	.long	_uhoh			| 228:
	.long	_uhoh			| 229:
	.long	_uhoh			| 230:
	.long	_uhoh			| 231:
	.long	_uhoh			| 232:
	.long	_uhoh			| 233:
	.long	_uhoh			| 234:
	.long	_uhoh			| 235:
	.long	_uhoh			| 236:
	.long	_uhoh			| 237:
	.long	_uhoh			| 238:
	.long	_uhoh			| 239:
	.long	_uhoh			| 240:
	.long	_uhoh			| 241:
	.long	_uhoh			| 242:
	.long	_uhoh			| 243:
	.long	_uhoh			| 244:
	.long	_uhoh			| 245:
	.long	_uhoh			| 246:
	.long	_uhoh			| 247:
	.long	_uhoh			| 248:
	.long	_uhoh			| 249:
	.long	_uhoh			| 250:
	.long	_uhoh			| 251:
	.long	_uhoh			| 252:
	.long	_uhoh			| 253:
	.long	_uhoh			| 254:
	.long	_uhoh			| 255:

/*
 * Default trap handler
 * With an oscilloscope you can see AS* stop
 */
	PUBLIC (_uhoh)
_uhoh:	nop				| Leave spot for breakpoint
	stop	#0x2700			| Stop with interrupts disabled
	bra.l	_uhoh			| Stuck forever

/*
 * Log, but otherwise ignore, spurious interrupts
 */
	PUBLIC (_spuriousInterrupt)
_spuriousInterrupt:
	addql	#1,_M68kSpuriousInterruptCount
	rte

/*
 * Place the low-order 3 octets of the board's ethernet address at
 * a `well-known' fixed location relative to the startup location.
 */
	.align 2
	.word	0			| Padding
ethernet_address_buffer:
	.word	0x08F3			| Default address
	.word	0xDEAD
	.word	0xCAFE

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
	lea	SYM(m360)+1024-64,a7	| Put stack in dual-port ram
					| a little below the interrupt stack
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
	movel	#m360+0x101,d0		| MBAR value (mask CPU space accesses)
	movesl	d0,0x3FF00		| Set MBAR
	movec	d1,dfc			| Restore destination register

	/*
	 * Step 5: Verify a dual-port RAM location
	 */
	lea	m360,a0			| Point a0 to first DPRAM location
	moveb	#0x33,d0		| Set the test value
	moveb	d0,a0@			| Set the memory location
	cmpb	a0@,d0			| Does it read back?
	bne	_uhoh			| If not, bad news!
	notb	d0			| Flip bits
	moveb	d0,a0@			| Set the memory location
	cmpb	a0@,d0			| Does it read back?
	bne	_uhoh			| If not, bad news!

	/*
	 * Remaining steps are handled by C code
	 */
	jmp	_Init68360		| Start C code (which never returns)

/*
 * Copy DATA segment, clear BSS segment, set up real stack, start C program.
 * Assume that DATA and BSS sizes are multiples of 4.
 */
	PUBLIC (_CopyDataClearBSSAndStart)
_CopyDataClearBSSAndStart:
	lea	_copy_start,a0		| Get start of DATA in RAM
	lea	etext,a2		| Get start of DATA in ROM
	cmpl	a0,a2			| Are they the same?
	beq.s	NOCOPY			| Yes, no copy necessary
	lea	_copy_end,a1		| Get end of DATA in RAM
	bra.s	COPYLOOPTEST		| Branch into copy loop
COPYLOOP:
	movel	a2@+,a0@+		| Copy word from ROM to RAM
COPYLOOPTEST:
	cmpl	a1,a0			| Done?
	bcs.s	COPYLOOP		| No, skip
NOCOPY:

	lea	_clear_start,a0		| Get start of BSS
	lea	_clear_end,a1		| Get end of BSS
	clrl	d0			| Value to set
	bra.s	ZEROLOOPTEST		| Branch into clear loop
ZEROLOOP:
	movel	d0,a0@+			| Clear a word
ZEROLOOPTEST:
	cmpl	a1,a0			| Done?
	bcs.s	ZEROLOOP		| No, skip

	movel	#_stack_init,a7		| set master stack pointer
	movel	d0,a7@-			| command line
	jsr	boot_card		| Call C main

	PUBLIC (_mainDone)
_mainDone:
	nop				| Leave spot for breakpoint
	movew	#1,a7			| Force a double bus error
	movel	d0,a7@-			| This should cause a RESET
	stop	#0x2700			| Stop with interrupts disabled
	bra.l	_mainDone		| Stuck forever

        .align 2
END_CODE

BEGIN_DATA_DCL
        .align 2
	PUBLIC (environ)
environ:
	.long	0
	PUBLIC (_M68kSpuriousInterruptCount)
_M68kSpuriousInterruptCount:
	.long	0
END_DATA_DCL

END
