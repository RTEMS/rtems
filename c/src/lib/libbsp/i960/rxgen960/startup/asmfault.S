/*
 *  $Id$
 */

/*
 * asmfault.s
 * Last change : 31. 1.95 
 */

	.text
	.globl	_faultHndlEntry

	.text

_faultHndlEntry :
	  /* Raise priority. */
	ldconst	0x1F0000, r4
	ldconst	0xFFFFFFFF, r5
	modpc	r4, r4, r5
	  /* Where to keep state of the faulted code. */
	ldconst	_faultBuffer, r3
	  /* Save global registers. */
	stq	g0, 64+0(r3)
	stq	g4, 64+16(r3)	
	stq	g8, 64+32(r3)
	stt	g12, 64+48(r3)
	  /* Faulted code's fp (g15) is our pfp. */
	st	pfp, 64+60(r3)
	  /* Make sure locals are in stack. */
	flushreg
	  /* g3 = & previosFrame[0] */
	andnot	0xF, pfp, g3
	  /* Save local registers of faulted procedure. */
	ldq	0(g3), r4
	stq	r4, 0(r3)
	ldq	16(g3), r4
	stq	r4, 16(r3)
	ldq	32(g3), r4
	stq	r4, 32(r3)
	ldq	48(g3), r4
	stq	r4, 48(r3)  
	  /* To handling. */
	mov	fp, g0
	mov	r3, g1
	callx	_faultTblHandler
	  /* This point will never be reached ... */

/* End of file */
