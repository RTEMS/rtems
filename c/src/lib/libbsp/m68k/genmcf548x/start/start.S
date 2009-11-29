/*===============================================================*\
| Project: RTEMS generic mcf548x BSP                              |
+-----------------------------------------------------------------+
| File: start.S                                                   |
+-----------------------------------------------------------------+
| The file contains the assembly part of MCF548x init code        |
+-----------------------------------------------------------------+
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
|                                                                 |
| Parts of the code has been derived from the "dBUG source code"  |
| package Freescale is providing for M548X EVBs. The usage of     |
| the modified or unmodified code and it's integration into the   |
| generic mcf548x BSP has been done according to the Freescale    |
| license terms.                                                  |
|                                                                 |
| The Freescale license terms can be reviewed in the file         |
|                                                                 |
|    Freescale_license.txt                                        |
|                                                                 |
+-----------------------------------------------------------------+
|                                                                 |
| The generic mcf548x BSP has been developed on the basic         |
| structures and modules of the av5282 BSP.                       |
|                                                                 |
+-----------------------------------------------------------------+
|                                                                 |
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
|                                                                 |
|   date                      history                        ID   |
| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |
| 12.11.07                    1.0                            ras  |
|                                                                 |
\*===============================================================*/

/*===============================================================*\
| Includes                                                        |
\*===============================================================*/
#include <rtems/asm.h>

/*===============================================================*\
| External references                                             |
\*===============================================================*/
.extern __MBAR
.extern _CoreSramBase0
.extern _CoreSramBase1
.extern _CoreSramSize1
.extern mcf548x_init
.extern boot_card
.extern _SpInit

/*===============================================================*\
| Global symbols                                                  |
\*===============================================================*/

.global interrupt_vector_table
.global spurious_int_count
.global start


/*===============================================================*\
| Exception Table                                                 |
\*===============================================================*/

.section ".vectors","ax"  	     /* begin of vectors section */
PUBLIC (InterruptVectorTable)
SYM(InterruptVectorTable):
INITSP:		.long	_SpInit		        /* Initial SP		  */
INITPC:		.long	start			/* Initial PC		  */
vector002:	.long	asm_default_interrupt	/* Access Error		  */
vector003:	.long	asm_default_interrupt	/* Address Error	  */
vector004:	.long	asm_default_interrupt	/* Illegal Instruction	  */
vector005:	.long	asm_default_interrupt	/* Reserved		  */
vector006:	.long	asm_default_interrupt	/* Reserved		  */
vector007:	.long	asm_default_interrupt	/* Reserved		  */
vector008:	.long	asm_default_interrupt	/* Privilege Violation	  */
vector009:	.long	asm_default_interrupt	/* Trace		  */
vector010:	.long	asm_default_interrupt	/* Unimplemented A-Line	  */
vector011:	.long	asm_default_interrupt	/* Unimplemented F-Line	  */
vector012:	.long	asm_default_interrupt	/* Debug Interrupt	  */
vector013:	.long	asm_default_interrupt	/* Reserved		  */
vector014:	.long	asm_default_interrupt	/* Format Error		  */
vector015:	.long	asm_default_interrupt	/* Unitialized Int.	  */
vector016:	.long	asm_default_interrupt	/* Reserved		  */
vector017:	.long	asm_default_interrupt	/* Reserved		  */
vector018:	.long	asm_default_interrupt	/* Reserved		  */
vector019:	.long	asm_default_interrupt	/* Reserved		  */
vector020:	.long	asm_default_interrupt	/* Reserved		  */
vector021:	.long	asm_default_interrupt	/* Reserved		  */
vector022:	.long	asm_default_interrupt	/* Reserved		  */
vector023:	.long	asm_default_interrupt	/* Reserved		  */
vector024:	.long	asm_spurious_interrupt	/* Spurious Interrupt	  */
vector025:	.long	asm_default_interrupt	/* Autovector Level 1	  */
vector026:	.long	asm_default_interrupt	/* Autovector Level 2	  */
vector027:	.long	asm_default_interrupt	/* Autovector Level 3	  */
vector028:	.long	asm_default_interrupt	/* Autovector Level 4	  */
vector029:	.long	asm_default_interrupt	/* Autovector Level 5	  */
vector030:	.long	asm_default_interrupt	/* Autovector Level 6	  */
vector031:	.long	asm_default_interrupt	/* Autovector Level 7	  */
vector032:	.long	asm_default_interrupt	/* TRAP #0		  */
vector033:	.long	asm_default_interrupt	/* TRAP #1		  */
vector034:	.long	asm_default_interrupt	/* TRAP #2		  */
vector035:	.long	asm_default_interrupt	/* TRAP #3		  */
vector036:	.long	asm_default_interrupt	/* TRAP #4		  */
vector037:	.long	asm_default_interrupt	/* TRAP #5		  */
vector038:	.long	asm_default_interrupt	/* TRAP #6		  */
vector039:	.long	asm_default_interrupt	/* TRAP #7		  */
vector040:	.long	asm_default_interrupt	/* TRAP #8		  */
vector041:	.long	asm_default_interrupt	/* TRAP #9		  */
vector042:	.long	asm_default_interrupt	/* TRAP #10		  */
vector043:	.long	asm_default_interrupt	/* TRAP #11		  */
vector044:	.long	asm_default_interrupt	/* TRAP #12		  */
vector045:	.long	asm_default_interrupt	/* TRAP #13		  */
vector046:	.long	asm_default_interrupt	/* TRAP #14		  */
vector047:	.long	asm_default_interrupt	/* TRAP #15		  */
vector048:	.long	asm_default_interrupt	/* Reserved		  */
vector049:	.long	asm_default_interrupt	/* Reserved		  */
vector050:	.long	asm_default_interrupt	/* Reserved		  */
vector051:	.long	asm_default_interrupt	/* Reserved		  */
vector052:	.long	asm_default_interrupt	/* Reserved		  */
vector053:	.long	asm_default_interrupt	/* Reserved		  */
vector054:	.long	asm_default_interrupt	/* Reserved		  */
vector055:	.long	asm_default_interrupt	/* Reserved		  */
vector056:	.long	asm_default_interrupt	/* Reserved		  */
vector057:	.long	asm_default_interrupt	/* Reserved		  */
vector058:	.long	asm_default_interrupt	/* Reserved		  */
vector059:	.long	asm_default_interrupt	/* Reserved		  */
vector060:	.long	asm_default_interrupt	/* Reserved		  */
vector061:	.long	asm_default_interrupt	/* Reserved		  */
vector062:	.long	asm_default_interrupt	/* Reserved		  */
vector063:	.long	asm_default_interrupt	/* Reserved		  */
vector064:	.long	asm_default_interrupt
vector065:	.long	asm_default_interrupt
vector066:	.long	asm_default_interrupt
vector067:	.long	asm_default_interrupt
vector068:	.long	asm_default_interrupt
vector069:	.long	asm_default_interrupt
vector070:	.long	asm_default_interrupt
vector071:	.long	asm_default_interrupt
vector072:	.long	asm_default_interrupt
vector073:	.long	asm_default_interrupt
vector074:	.long	asm_default_interrupt
vector075:	.long	asm_default_interrupt
vector076:	.long	asm_default_interrupt
vector077:	.long	asm_default_interrupt
vector078:	.long	asm_default_interrupt
vector079:	.long	asm_default_interrupt
vector080:	.long	asm_default_interrupt
vector081:	.long	asm_default_interrupt
vector082:	.long	asm_default_interrupt
vector083:	.long	asm_default_interrupt
vector084:	.long	asm_default_interrupt
vector085:	.long	asm_default_interrupt
vector086:	.long	asm_default_interrupt
vector087:	.long	asm_default_interrupt
vector088:	.long	asm_default_interrupt
vector089:	.long	asm_default_interrupt
vector090:	.long	asm_default_interrupt
vector091:	.long	asm_default_interrupt
vector092:	.long	asm_default_interrupt
vector093:	.long	asm_default_interrupt
vector094:	.long	asm_default_interrupt
vector095:	.long	asm_default_interrupt
vector096:	.long	asm_default_interrupt
vector097:	.long	asm_default_interrupt
vector098:	.long	asm_default_interrupt
vector099:	.long	asm_default_interrupt
vector100:	.long	asm_default_interrupt
vector101:	.long	asm_default_interrupt
vector102:	.long	asm_default_interrupt
vector103:	.long	asm_default_interrupt
vector104:	.long	asm_default_interrupt
vector105:	.long	asm_default_interrupt
vector106:	.long	asm_default_interrupt
vector107:	.long	asm_default_interrupt
vector108:	.long	asm_default_interrupt
vector109:	.long	asm_default_interrupt
vector110:	.long	asm_default_interrupt
vector111:	.long	asm_default_interrupt
vector112:	.long	asm_default_interrupt
vector113:	.long	asm_default_interrupt
vector114:	.long	asm_default_interrupt
vector115:	.long	asm_default_interrupt
vector116:	.long	asm_default_interrupt
vector117:	.long	asm_default_interrupt
vector118:	.long	asm_default_interrupt
vector119:	.long	asm_default_interrupt
vector120:	.long	asm_default_interrupt
vector121:	.long	asm_default_interrupt
vector122:	.long	asm_default_interrupt
vector123:	.long	asm_default_interrupt
vector124:	.long	asm_default_interrupt
vector125:	.long	asm_default_interrupt
vector126:	.long	asm_default_interrupt
vector127:	.long	asm_default_interrupt
vector128:	.long	asm_default_interrupt
vector129:	.long	asm_default_interrupt
vector130:	.long	asm_default_interrupt
vector131:	.long	asm_default_interrupt
vector132:	.long	asm_default_interrupt
vector133:	.long	asm_default_interrupt
vector134:	.long	asm_default_interrupt
vector135:	.long	asm_default_interrupt
vector136:	.long	asm_default_interrupt
vector137:	.long	asm_default_interrupt
vector138:	.long	asm_default_interrupt
vector139:	.long	asm_default_interrupt
vector140:	.long	asm_default_interrupt
vector141:	.long	asm_default_interrupt
vector142:	.long	asm_default_interrupt
vector143:	.long	asm_default_interrupt
vector144:	.long	asm_default_interrupt
vector145:	.long	asm_default_interrupt
vector146:	.long	asm_default_interrupt
vector147:	.long	asm_default_interrupt
vector148:	.long	asm_default_interrupt
vector149:	.long	asm_default_interrupt
vector150:	.long	asm_default_interrupt
vector151:	.long	asm_default_interrupt
vector152:	.long	asm_default_interrupt
vector153:	.long	asm_default_interrupt
vector154:	.long	asm_default_interrupt
vector155:	.long	asm_default_interrupt
vector156:	.long	asm_default_interrupt
vector157:	.long	asm_default_interrupt
vector158:	.long	asm_default_interrupt
vector159:	.long	asm_default_interrupt
vector160:	.long	asm_default_interrupt
vector161:	.long	asm_default_interrupt
vector162:	.long	asm_default_interrupt
vector163:	.long	asm_default_interrupt
vector164:	.long	asm_default_interrupt
vector165:	.long	asm_default_interrupt
vector166:	.long	asm_default_interrupt
vector167:	.long	asm_default_interrupt
vector168:	.long	asm_default_interrupt
vector169:	.long	asm_default_interrupt
vector170:	.long	asm_default_interrupt
vector171:	.long	asm_default_interrupt
vector172:	.long	asm_default_interrupt
vector173:	.long	asm_default_interrupt
vector174:	.long	asm_default_interrupt
vector175:	.long	asm_default_interrupt
vector176:	.long	asm_default_interrupt
vector177:	.long	asm_default_interrupt
vector178:	.long	asm_default_interrupt
vector179:	.long	asm_default_interrupt
vector180:	.long	asm_default_interrupt
vector181:	.long	asm_default_interrupt
vector182:	.long	asm_default_interrupt
vector183:	.long	asm_default_interrupt
vector184:	.long	asm_default_interrupt
vector185:	.long	asm_default_interrupt
vector186:	.long	asm_default_interrupt
vector187:	.long	asm_default_interrupt
vector188:	.long	asm_default_interrupt
vector189:	.long	asm_default_interrupt
vector190:	.long	asm_default_interrupt
vector191:	.long	asm_default_interrupt
vector192:	.long	asm_default_interrupt
vector193:	.long	asm_default_interrupt
vector194:	.long	asm_default_interrupt
vector195:	.long	asm_default_interrupt
vector196:	.long	asm_default_interrupt
vector197:	.long	asm_default_interrupt
vector198:	.long	asm_default_interrupt
vector199:	.long	asm_default_interrupt
vector200:	.long	asm_default_interrupt
vector201:	.long	asm_default_interrupt
vector202:	.long	asm_default_interrupt
vector203:	.long	asm_default_interrupt
vector204:	.long	asm_default_interrupt
vector205:	.long	asm_default_interrupt
vector206:	.long	asm_default_interrupt
vector207:	.long	asm_default_interrupt
vector208:	.long	asm_default_interrupt
vector209:	.long	asm_default_interrupt
vector210:	.long	asm_default_interrupt
vector211:	.long	asm_default_interrupt
vector212:	.long	asm_default_interrupt
vector213:	.long	asm_default_interrupt
vector214:	.long	asm_default_interrupt
vector215:	.long	asm_default_interrupt
vector216:	.long	asm_default_interrupt
vector217:	.long	asm_default_interrupt
vector218:	.long	asm_default_interrupt
vector219:	.long	asm_default_interrupt
vector220:	.long	asm_default_interrupt
vector221:	.long	asm_default_interrupt
vector222:	.long	asm_default_interrupt
vector223:	.long	asm_default_interrupt
vector224:	.long	asm_default_interrupt
vector225:	.long	asm_default_interrupt
vector226:	.long	asm_default_interrupt
vector227:	.long	asm_default_interrupt
vector228:	.long	asm_default_interrupt
vector229:	.long	asm_default_interrupt
vector230:	.long	asm_default_interrupt
vector231:	.long	asm_default_interrupt
vector232:	.long	asm_default_interrupt
vector233:	.long	asm_default_interrupt
vector234:	.long	asm_default_interrupt
vector235:	.long	asm_default_interrupt
vector236:	.long	asm_default_interrupt
vector237:	.long	asm_default_interrupt
vector238:	.long	asm_default_interrupt
vector239:	.long	asm_default_interrupt
vector240:	.long	asm_default_interrupt
vector241:	.long	asm_default_interrupt
vector242:	.long	asm_default_interrupt
vector243:	.long	asm_default_interrupt
vector244:	.long	asm_default_interrupt
vector245:	.long	asm_default_interrupt
vector246:	.long	asm_default_interrupt
vector247:	.long	asm_default_interrupt
vector248:	.long	asm_default_interrupt
vector249:	.long	asm_default_interrupt
vector250:	.long	asm_default_interrupt
vector251:	.long	asm_default_interrupt
vector252:	.long	asm_default_interrupt
vector253:	.long	asm_default_interrupt
vector254:	.long	asm_default_interrupt
vector255:	.long	asm_default_interrupt

/*===============================================================*\
| Start of code                                                   |
\*===============================================================*/
PUBLIC (start)
SYM(start):
    move.w	#0x3700,sr              /* disable interrupts */
    jmp 	start_init

/*===============================================================*\
| Sspurious interrupt counter                                     |
\*===============================================================*/
.align 4
.data					/* begin of data section */
PUBLIC (spurious_int_count)
SYM(spurious_int_count):
    .long   0   			/* spurious interrupt counter */

/*===============================================================*\
| Function: Default exception handler                             |
+-----------------------------------------------------------------+
| - stop and disable all interrupts                               |
| - loop forever                                                  |
\*===============================================================*/
.text					/* start of text section */
.align 4
PUBLIC (asm_default_interrupt)
SYM(asm_default_interrupt):
    nop
    stop    #0x3700             	/* stop */
    bra.w   asm_default_interrupt      	/* loop forever */

/*===============================================================*\
| Function: Exception handler for spurious interrupts             |
+-----------------------------------------------------------------+
| - count spurious interrupts                                     |
\*===============================================================*/
.align 4
PUBLIC (asm_spurious_interrupt)
SYM(asm_spurious_interrupt):
    add.l   #1,spurious_int_count
    rte

/*===============================================================*\
| Function: start_init                                            |
+-----------------------------------------------------------------+
| - Disable all intterupts                                        |
| - Setup the internal SRAM                                       |
| - Initialize mcf548x peripherals                                |
| - Set initial stack pointer                                     |
| - Boot RTEMS
\*===============================================================*/
.align 4
PUBLIC (start_init)
SYM(start_init):

    move.l  #0x01040100,d0		/* invalidate instruction/data/branch cache, disable all caches */
    movec   d0,cacr

    move.l  #_CoreSramBase0,d0 		/* initialize RAMBAR0 */
    add.l   #0x21,d0                    /* for code & data    */
    movec   d0,rambar0

    move.l  #_CoreSramBase1,d0		/* initialize RAMBAR1 */
    add.l   #0x21,d0                    /* for code & data    */
    movec   d0,rambar1                  /* movec d0,RAMBAR1   */

    move.l  #__MBAR,d0			/* initialize MBAR */
    movec   d0,mbar

    move.l  #_CoreSramBase1,d0		/* set sp to end of Core SRAM temporarily */
    add.l   #_CoreSramSize1,d0
    move.l  d0,sp

    move.l  #0,d0			/* initialize frame pointer */
    movea.l d0,a6

    jsr	    mcf548x_init		/* Initialize mcf548x peripherals */

    move.l  #_SpInit,sp		        /* relocate sp */

    clrl    d0          		/* clear d0 */
    movel   d0,a7@-                     /* command line == 0 */

    jsr     boot_card                   /* boot rtems */

    movel   a7@+,d0

exit_multitasking:
    nop
    nop
    halt
    bra     exit_multitasking

.end					/* end of start.S module */



