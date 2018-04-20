/* from libgloss/v850 */
	.text
	.global	___trap0
___trap0:
	trap 31
	tst r10,r10
	bz .L0
/*
 *  Libgloss cares about the errno from this. We don't
	movhi hi(_errno),r0,r6
	st.w r10,lo(_errno)[r6]
*/
.L0:
	jmp [r31]
