#######################################
# ihandler.s                          #
# Last change :  8.11.94              #
#######################################

	.text
	.globl _nmiHandler
	.globl _intr5Handler
	.globl _intr6Handler
	.globl _clockHandler 

	.text 

# NMI Handler 
_nmiHandler :
	ldconst 64, r4
	addo 	sp, r4, sp

	stq 	g0, -64(sp)
	stq 	g4, -48(sp)
	stq 	g8, -32(sp)
	stt 	g12, -16(sp)

	callx	_nmi_isr

	ldq 	-64(sp), g0
	ldq 	-48(sp), g4
	ldq 	-32(sp), g8
	ldt 	-16(sp), g12

	ret
_clockHandler :
        ldconst 64, r4
        addo    sp, r4, sp

        stq     g0, -64(sp)
        stq     g4, -48(sp)
        stq     g8, -32(sp)
        stt     g12, -16(sp)

        callx   _Clock_isr

        ldq     -64(sp), g0
        ldq     -48(sp), g4
        ldq     -32(sp), g8
        ldt     -16(sp), g12
#
#
        ret

