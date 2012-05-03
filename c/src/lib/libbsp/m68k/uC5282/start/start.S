/*
 *  uC5282 startup code
 *
 *  This file contains the entry point for the application.
 *  The name of this entry point is compiler dependent.
 *  It jumps to the BSP which is responsible for performing
 *  all initialization.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/asm.h>

#define SRAM_SIZE (64*1024)
#define DEFAULT_IPSBAR  0x40000000

BEGIN_CODE

/***************************************************************************
   Function : Entry

   Description : Entry point to the system. In a raw system we would have
   put the initial stack pointer as the first 4 bytes.  Instead we have to
   provide a real instruction at the first location since we might be getting
   started by dBUG after downloading from TFTP or FLASH.   Hack in an
   'initial stack pointer' that actually is a jump to the start address!
 ***************************************************************************/
Entry:


    nop ; jmp SYM(start)    |   0: Initial 'SSP'    1: Initial PC
    .long   SYM(_uhoh)      |   2: Bus error
    .long   SYM(_uhoh)      |   3: Address error
    .long   SYM(_uhoh)      |   4: Illegal instruction
    .long   SYM(_uhoh)      |   5: Zero division
    .long   SYM(_uhoh)      |   6: CHK, CHK2 instruction
    .long   SYM(_uhoh)      |   7: TRAPcc, TRAPV instructions
    .long   SYM(_uhoh)      |   8: Privilege violation
    .long   SYM(_uhoh)      |   9: Trace
    .long   SYM(_uhoh)      |  10: Line 1010 emulator
    .long   SYM(_uhoh)      |  11: Line 1111 emulator
    .long   SYM(_uhoh)      |  12: Hardware breakpoint
    .long   SYM(_uhoh)      |  13: Reserved for coprocessor violation
    .long   SYM(_uhoh)      |  14: Format error
    .long   SYM(_uhoh)      |  15: Uninitialized interrupt
    .long   SYM(_uhoh)      |  16: Unassigned, reserved
    .long   SYM(_uhoh)      |  17:
    .long   SYM(_uhoh)      |  18:
    .long   SYM(_uhoh)      |  19:
    .long   SYM(_uhoh)      |  20:
    .long   SYM(_uhoh)      |  21:
    .long   SYM(_uhoh)      |  22:
    .long   SYM(_uhoh)      |  23:
    .long   SYM(_spuriousInterrupt) |  24: Spurious interrupt
    .long   SYM(_uhoh)      |  25: Level 1 interrupt autovector
    .long   SYM(_uhoh)      |  26: Level 2 interrupt autovector
    .long   SYM(_uhoh)      |  27: Level 3 interrupt autovector
    .long   SYM(_uhoh)      |  28: Level 4 interrupt autovector
    .long   SYM(_uhoh)      |  29: Level 5 interrupt autovector
    .long   SYM(_uhoh)      |  30: Level 6 interrupt autovector
    .long   SYM(_uhoh)      |  31: Level 7 interrupt autovector
    .long   SYM(_uhoh)      |  32: Trap instruction (0-15)
    .long   SYM(_uhoh)      |  33:
    .long   SYM(_uhoh)      |  34:
    .long   SYM(_uhoh)      |  35:
    .long   SYM(_uhoh)      |  36:
    .long   SYM(_uhoh)      |  37:
    .long   SYM(_uhoh)      |  38:
    .long   SYM(_uhoh)      |  39:
    .long   SYM(_uhoh)      |  40:
    .long   SYM(_uhoh)      |  41:
    .long   SYM(_uhoh)      |  42:
    .long   SYM(_uhoh)      |  43:
    .long   SYM(_uhoh)      |  44:
    .long   SYM(_uhoh)      |  45:
    .long   SYM(_uhoh)      |  46:
    .long   SYM(_uhoh)      |  47:
    .long   SYM(_uhoh)      |  48: Reserved for coprocessor
    .long   SYM(_uhoh)      |  49:
    .long   SYM(_uhoh)      |  50:
    .long   SYM(_uhoh)      |  51:
    .long   SYM(_uhoh)      |  52:
    .long   SYM(_uhoh)      |  53:
    .long   SYM(_uhoh)      |  54:
    .long   SYM(_uhoh)      |  55:
    .long   SYM(_uhoh)      |  56:
    .long   SYM(_uhoh)      |  57:
    .long   SYM(_uhoh)      |  58:
    .long   SYM(_uhoh)      |  59: Unassigned, reserved
    .long   SYM(_uhoh)      |  60:
    .long   SYM(_uhoh)      |  61:
    .long   SYM(_uhoh)      |  62:
    .long   SYM(_uhoh)      |  63:
    .long   SYM(_spuriousInterrupt) |  64: User spurious handler
    .long   SYM(_uhoh)      |  65:
    .long   SYM(_uhoh)      |  66:
    .long   SYM(_uhoh)      |  67:
    .long   SYM(_uhoh)      |  68:
    .long   SYM(_uhoh)      |  69:
    .long   SYM(_uhoh)      |  70:
    .long   SYM(_uhoh)      |  71:
    .long   SYM(_uhoh)      |  72:
    .long   SYM(_uhoh)      |  73:
    .long   SYM(_uhoh)      |  74:
    .long   SYM(_uhoh)      |  75:
    .long   SYM(_uhoh)      |  76:
    .long   SYM(_uhoh)      |  77:
    .long   SYM(_uhoh)      |  78:
    .long   SYM(_uhoh)      |  79:
    .long   SYM(_uhoh)      |  80:
    .long   SYM(_uhoh)      |  81:
    .long   SYM(_uhoh)      |  82:
    .long   SYM(_uhoh)      |  83:
    .long   SYM(_uhoh)      |  84:
    .long   SYM(_uhoh)      |  85:
    .long   SYM(_uhoh)      |  86:
    .long   SYM(_uhoh)      |  87:
    .long   SYM(_uhoh)      |  88:
    .long   SYM(_uhoh)      |  89:
    .long   SYM(_uhoh)      |  90:
    .long   SYM(_uhoh)      |  91:
    .long   SYM(_uhoh)      |  92:
    .long   SYM(_uhoh)      |  93:
    .long   SYM(_uhoh)      |  94:
    .long   SYM(_uhoh)      |  95:
    .long   SYM(_uhoh)      |  96:
    .long   SYM(_uhoh)      |  97:
    .long   SYM(_uhoh)      |  98:
    .long   SYM(_uhoh)      |  99:
    .long   SYM(_uhoh)      | 100:
    .long   SYM(_uhoh)      | 101:
    .long   SYM(_uhoh)      | 102:
    .long   SYM(_uhoh)      | 103:
    .long   SYM(_uhoh)      | 104:
    .long   SYM(_uhoh)      | 105:
    .long   SYM(_uhoh)      | 106:
    .long   SYM(_uhoh)      | 107:
    .long   SYM(_uhoh)      | 108:
    .long   SYM(_uhoh)      | 109:
    .long   SYM(_uhoh)      | 110:
    .long   SYM(_uhoh)      | 111:
    .long   SYM(_uhoh)      | 112:
    .long   SYM(_uhoh)      | 113:
    .long   SYM(_uhoh)      | 114:
    .long   SYM(_uhoh)      | 115:
    .long   SYM(_uhoh)      | 116:
    .long   SYM(_uhoh)      | 117:
    .long   SYM(_uhoh)      | 118:
    .long   SYM(_uhoh)      | 119:
    .long   SYM(_uhoh)      | 120:
    .long   SYM(_uhoh)      | 121:
    .long   SYM(_uhoh)      | 122:
    .long   SYM(_uhoh)      | 123:
    .long   SYM(_uhoh)      | 124:
    .long   SYM(_uhoh)      | 125:
    .long   SYM(_uhoh)      | 126:
    .long   SYM(_uhoh)      | 127:
    .long   SYM(_uhoh)      | 128:
    .long   SYM(_uhoh)      | 129:
    .long   SYM(_uhoh)      | 130:
    .long   SYM(_uhoh)      | 131:
    .long   SYM(_uhoh)      | 132:
    .long   SYM(_uhoh)      | 133:
    .long   SYM(_uhoh)      | 134:
    .long   SYM(_uhoh)      | 135:
    .long   SYM(_uhoh)      | 136:
    .long   SYM(_uhoh)      | 137:
    .long   SYM(_uhoh)      | 138:
    .long   SYM(_uhoh)      | 139:
    .long   SYM(_uhoh)      | 140:
    .long   SYM(_uhoh)      | 141:
    .long   SYM(_uhoh)      | 142:
    .long   SYM(_uhoh)      | 143:
    .long   SYM(_uhoh)      | 144:
    .long   SYM(_uhoh)      | 145:
    .long   SYM(_uhoh)      | 146:
    .long   SYM(_uhoh)      | 147:
    .long   SYM(_uhoh)      | 148:
    .long   SYM(_uhoh)      | 149:
    .long   SYM(_uhoh)      | 150:
    .long   SYM(_uhoh)      | 151:
    .long   SYM(_uhoh)      | 152:
    .long   SYM(_uhoh)      | 153:
    .long   SYM(_uhoh)      | 154:
    .long   SYM(_uhoh)      | 155:
    .long   SYM(_uhoh)      | 156:
    .long   SYM(_uhoh)      | 157:
    .long   SYM(_uhoh)      | 158:
    .long   SYM(_uhoh)      | 159:
    .long   SYM(_uhoh)      | 160:
    .long   SYM(_uhoh)      | 161:
    .long   SYM(_uhoh)      | 162:
    .long   SYM(_uhoh)      | 163:
    .long   SYM(_uhoh)      | 164:
    .long   SYM(_uhoh)      | 165:
    .long   SYM(_uhoh)      | 166:
    .long   SYM(_uhoh)      | 167:
    .long   SYM(_uhoh)      | 168:
    .long   SYM(_uhoh)      | 169:
    .long   SYM(_uhoh)      | 170:
    .long   SYM(_uhoh)      | 171:
    .long   SYM(_uhoh)      | 172:
    .long   SYM(_uhoh)      | 173:
    .long   SYM(_uhoh)      | 174:
    .long   SYM(_uhoh)      | 175:
    .long   SYM(_uhoh)      | 176:
    .long   SYM(_uhoh)      | 177:
    .long   SYM(_uhoh)      | 178:
    .long   SYM(_uhoh)      | 179:
    .long   SYM(_uhoh)      | 180:
    .long   SYM(_uhoh)      | 181:
    .long   SYM(_uhoh)      | 182:
    .long   SYM(_uhoh)      | 183:
    .long   SYM(_uhoh)      | 184:
    .long   SYM(_uhoh)      | 185:
    .long   SYM(_uhoh)      | 186:
    .long   SYM(_uhoh)      | 187:
    .long   SYM(_uhoh)      | 188:
    .long   SYM(_uhoh)      | 189:
    .long   SYM(_uhoh)      | 190:
    .long   SYM(_uhoh)      | 191:
    .long   SYM(_uhoh)      | 192:
    .long   SYM(_uhoh)      | 193:
    .long   SYM(_uhoh)      | 194:
    .long   SYM(_uhoh)      | 195:
    .long   SYM(_uhoh)      | 196:
    .long   SYM(_uhoh)      | 197:
    .long   SYM(_uhoh)      | 198:
    .long   SYM(_uhoh)      | 199:
    .long   SYM(_uhoh)      | 200:
    .long   SYM(_uhoh)      | 201:
    .long   SYM(_uhoh)      | 202:
    .long   SYM(_uhoh)      | 203:
    .long   SYM(_uhoh)      | 204:
    .long   SYM(_uhoh)      | 205:
    .long   SYM(_uhoh)      | 206:
    .long   SYM(_uhoh)      | 207:
    .long   SYM(_uhoh)      | 208:
    .long   SYM(_uhoh)      | 209:
    .long   SYM(_uhoh)      | 210:
    .long   SYM(_uhoh)      | 211:
    .long   SYM(_uhoh)      | 212:
    .long   SYM(_uhoh)      | 213:
    .long   SYM(_uhoh)      | 214:
    .long   SYM(_uhoh)      | 215:
    .long   SYM(_uhoh)      | 216:
    .long   SYM(_uhoh)      | 217:
    .long   SYM(_uhoh)      | 218:
    .long   SYM(_uhoh)      | 219:
    .long   SYM(_uhoh)      | 220:
    .long   SYM(_uhoh)      | 221:
    .long   SYM(_uhoh)      | 222:
    .long   SYM(_uhoh)      | 223:
    .long   SYM(_uhoh)      | 224:
    .long   SYM(_uhoh)      | 225:
    .long   SYM(_uhoh)      | 226:
    .long   SYM(_uhoh)      | 227:
    .long   SYM(_uhoh)      | 228:
    .long   SYM(_uhoh)      | 229:
    .long   SYM(_uhoh)      | 230:
    .long   SYM(_uhoh)      | 231:
    .long   SYM(_uhoh)      | 232:
    .long   SYM(_uhoh)      | 233:
    .long   SYM(_uhoh)      | 234:
    .long   SYM(_uhoh)      | 235:
    .long   SYM(_uhoh)      | 236:
    .long   SYM(_uhoh)      | 237:
    .long   SYM(_uhoh)      | 238:
    .long   SYM(_uhoh)      | 239:
    .long   SYM(_uhoh)      | 240:
    .long   SYM(_uhoh)      | 241:
    .long   SYM(_uhoh)      | 242:
    .long   SYM(_uhoh)      | 243:
    .long   SYM(_uhoh)      | 244:
    .long   SYM(_uhoh)      | 245:
    .long   SYM(_uhoh)      | 246:
    .long   SYM(_uhoh)      | 247:
    .long   SYM(_uhoh)      | 248:
    .long   SYM(_uhoh)      | 249:
    .long   SYM(_uhoh)      | 250:
    .long   SYM(_uhoh)      | 251:
    .long   SYM(_uhoh)      | 252:
    .long   SYM(_uhoh)      | 253:
    .long   SYM(_uhoh)      | 254:
    .long   SYM(_uhoh)      | 255:

/*
 * Default trap handler
 * With an oscilloscope you can see AS* stop
 */
.align 4
    PUBLIC (_uhoh)
SYM(_uhoh):
    nop                 | Leave spot for breakpoint
    stop    #0x2700             | Stop with interrupts disabled
    bra.w   SYM(_uhoh)          | Stuck forever

.align 4
    PUBLIC (_spuriousInterrupt)
SYM(_spuriousInterrupt):
    addql   #1,SYM(_M68kSpuriousInterruptCount)
    rte

.align 4
    PUBLIC (start)
SYM(start):
    move.w  #0x2700,sr                 | Disable interrupts

    /*
     * If we're being started by the debugger, and the debugger has
     * moved the IPSBAR, we're doomed........
     */
    move.l  #__IPSBAR+1,d0             | Enable the MCF5282 internal peripherals
    move.l  d0,DEFAULT_IPSBAR
    move.l  #__SRAMBASE+0x201,d0       | Enable the MCF5282 internal SRAM
    movec   d0,%rambar                 | CPU-space copy of RAMBAR
    move.l  d0,DEFAULT_IPSBAR+8        | Memory-space copy of RAMBAR
    move.l  #__SRAMBASE+SRAM_SIZE-4,sp | Overwrite the fake stack pointer

    /*
     * Copy the vector table to address 0 (VBR must be 0 mod 2^20)
     * Leave the dBUG vectors (0-63) alone
     */
    lea.l  (64*4)+Entry,a0
    lea.l  (64*4),a1
    move.l #(256-64)-1,d0
vectcpy:
    move.l a0@+,a1@+       | Copy the vector table
    sub.l  #1,d0
    bne.s  vectcpy

    /*
     * Remainder of the startup code is handled by C code
     */
    jmp SYM(Init5282)       | Start C code (which never returns)

/***************************************************************************
   Function : CopyDataClearBSSAndStart

   Description : Copy DATA segment, Copy SRAM segment, clear BSS segment,
   start C program. Assume that DATA and BSS sizes are multiples of 4.
 ***************************************************************************/
.align 4

    PUBLIC (CopyDataClearBSSAndStart)
SYM(CopyDataClearBSSAndStart):
    lea SYM(_data_dest_start),a0        | Get start of DATA in RAM
    lea SYM(_data_src_start),a2     | Get start of DATA in ROM
        sub.l   #SYM(_header_offset),a2          | Change source by the amount of the header offset
    cmpl    a0,a2                   | Are they the same?
    beq.s   NODATACOPY              | Yes, no copy necessary
    lea SYM(_data_dest_end),a1      | Get end of DATA in RAM
    bra.s   DATACOPYLOOPTEST            | Branch into copy loop
DATACOPYLOOP:
    movel   a2@+,a0@+               | Copy word from ROM to RAM
DATACOPYLOOPTEST:
    cmpl    a1,a0                   | Done?
    bcs.s   DATACOPYLOOP                | No, skip
NODATACOPY:

/* Now, clear BSS */
        lea _clear_start,a0     | Get start of BSS
    lea _clear_end,a1       | Get end of BSS
    clrl    d0          | Value to set
    bra.s   ZEROLOOPTEST        | Branch into clear loop
ZEROLOOP:
    movel   d0,a0@+         | Clear a word
ZEROLOOPTEST:
    cmpl    a1,a0           | Done?
    bcs.s   ZEROLOOP        | No, skip


    /*
     * Right : Now we're ready to boot RTEMS
     */
    clrl    d0          | Pass in null to all boot_card() params
    movel   d0,a7@-         | command line
    jsr SYM(boot_card)      | Call C boot_card function to startup RTEMS
    movel   a7@+,d0
MULTI_TASK_EXIT:
    nop
    nop
    trap    #14
    bra     MULTI_TASK_EXIT


END_CODE

        .align 2
BEGIN_DATA_DCL
        .align 2
    PUBLIC (_M68kSpuriousInterruptCount)
SYM (_M68kSpuriousInterruptCount):
    .long   0
END_DATA_DCL

END

