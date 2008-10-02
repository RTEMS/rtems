/*
 *  Context switch for the Reneas M32C
 *
 *
 */

#define ARG_EXECUTING 8
#define ARG_HEIR      12

#define CTXT_SP  0
#define CTXT_FB  4

	.file	"context_switch.S"
	.text
	.global	__CPU_Context_switch
	.type	__CPU_Context_switch, @function
__CPU_Context_switch:
	enter	#0
	pushm 	a0,a1,r0,r1,r2,r3
	pushc	flg
	pushc	sb

	mov.l	ARG_EXECUTING[fb],a0	; a0 = executing
	stc  	fb,a1
	mov.l	a1,CTXT_FB[a0]		; save fb
	stc  	sp,a1
	mov.l	a1,CTXT_SP[a0]		; save sp

	mov.l	ARG_HEIR[fb],a0		; a0 = heir

	mov.l	CTXT_SP[a0],a1
	ldc  	a1,sp			; restore sp
	mov.l	CTXT_FB[a0],a1
	ldc  	a1,fb			; restore fb
	popc	sb
	popc	flg
	popm 	a0,a1,r0,r1,r2,r3
	exitd
	.size	__CPU_Context_switch, .-__CPU_Context_switch
