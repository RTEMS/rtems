/* macros.inc
 *
 * This file assists the board independent startup code
 * by initializing the chip-select channels to 
 * reflect the proper memory configuration.
 *
 * NOTE: No stack has been established when this routine	
 *       is invoked.  It returns to start.s through ldsegs
 *
 *  Submitted by:
 *
 *    Erik Ivanenko
 *    University of Toronto
 *    erik.ivanenko@utoronto.ca
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

.set GDT_CODE_PTR	, 0x08
.set GDT_DATA_PTR	, 0x10	

.set BOARD_SELECT  , 0x301
.set BOARD_DATA	   , 0x302
.set BOARD_LATCH   , 0x303


#define SetExRegByte(_register,_value) \
        movb       $ ## _value, al; \
	movw	  $ ## _register, dx; \
        outb       al, dx

#define SetExRegWord(_register,_value) \
        movw      $ ## _value, ax; \
	movw      $ ## _register, dx ; \
        outw      ax, dx


#define DESC2(lim_0_15, bas_0_15, bas_16_23, access, gran, bas_24_31) \
 .word lim_0_15 ; \
 .word bas_0_15 ; \
 .byte bas_16_23 ; \
 .byte access ; \
 .byte gran ; \
 .byte bas_24_31

#define DESC3( _base, _limit) \
.word _limit ; \
.long _base

/* THE GET_ACCESS IS CHANGED FROM 8E TO 9E */

#define INTERRUPT_GATE( _vector ) \
	.word   _vector ; \
	.word	GDT_CODE_PTR ; \
	.byte   0x0 ; \
	.byte	0x8E ; \
	.word	0x0

#define rLOAD_INDEX( _selector, _segment_register) \
        movl     _selector , _segment_register ; \
        addb	al, ( eax )

#define rLOAD_SEGMENT( _reg, _segment_register) \
        mov     _reg , _segment_register ; \

/* #define rLOAD_SEGMENT( _reg, _segment_register) \
	.code16 ; \
        mov     _reg , _segment_register ; \
        .code32 
*/
#define pLOAD_SEGMENT( _selector, _segment_register) \
        movl     $ ##  _selector , eax ; .code16 ; \
        mov      ax, _segment_register ; .code32 

/* #define MOVE_WORD( _immed32, _reg ) \
	data16 ; \
	movl    _immed32, _reg
*/

#define MOVE_WORD( _immed32, _reg ) \
	movw    _immed32, _reg

/* #define MOVE_IMMED_WORD( _immed32, _reg ) \
	data16 ; \
	mov    $ ## _immed32, _reg

#define CS_OFF( _cs_symbol, _reg ) \
	data16 ; \
	mov	_cs_symbol, _reg ; \
	data16 ;\
	andl	$0x000ffff, _reg

#define _16_NOPS \
	nop ; \
	nop ; \
	nop ; \
	nop ; \
	nop ; \
	nop ; \
	nop ; \
	nop ; \
	nop ; \
	nop ; \
	nop ; \
	nop ; \
	nop ; \
	nop ; \
	nop ; \
	nop ; 
*/

