#
#  $Id$
#

#######################################
# asmstub.s                           #
# Last change : 20. 1.95              #
#######################################

########################################################################
#
# PURPOSE:	resets processor
#
# ARGUMENTS:	g0 = start point
#               g1 = prcb
#
# RETURNS:	none
########################################################################

	.text
	.globl	_asm_exit

	.text
_asm_exit:
	ldconst	0x300, g2
#	ldconst	_start, g1
#	ldconst	_ram_prcb, g2
	sysctl	g2, g0, g1
	ret

########################################################################
#
# PURPOSE:	execute sysctl instruction
#
# ARGUMENTS:	g0 = message type	(0x300 for reinitialize)
#		g1 = depends on type	(inst addr for reinitialize)
#		g2 = depends on type	(prcb addr for reinitialize)
#
# RETURNS:	none
########################################################################

	.text
	.globl	_asm_sysctl

	.text
_asm_sysctl:
	b	_asm_sysctl	
	sysctl	g0, g1, g2
	ret

########################################################################
#
# PURPOSE:	alter a bit in the interrupt pending register
#
# ARGUMENTS:	g0 = interrupt number
#		g1 = 1 to set, 0 to clear
#
# RETURNS:	none
########################################################################

	.text
	.globl		_asm_ipend

	.text
_asm_ipend:
	chkbit	 0, g1
	/* alterbit g0, sf0, sf0 XXX JRS */
	ret
########################################################################
#
# PURPOSE:	alter a bit in the interrupt mask register
#
# ARGUMENTS:	g0 = interrupt number
#		g1 = 1 to set, 0 to clear
#
# RETURNS:	none
########################################################################

	.text
	.globl		_asm_imask

	.text
_asm_imask:
	chkbit	 0, g1
	/* alterbit g0, sf1, sf1 XXX JRS */
	ret


########################################################################
#
# PURPOSE:	get the value of the interrupt mask register
#
# ARGUMENTS:	none
#
# RETURNS:	value of IMASK reg
########################################################################

	.text
	.globl		_asm_get_imask

	.text
_asm_get_imask:
	/* mov		sf1, g0 XXX JRS */
	ret

########################################################################
#
# PURPOSE:	modify process-controls register
#
# ARGUMENTS:	g0 = value masked/stored in PC reg
#		g1 = mask of bits to be modified
#
# RETURNS:	g0 = initial value of PC reg
########################################################################

	.text
	.globl	_asm_modpc

	.text
_asm_modpc:
	modpc	g1, g1, g0
	ret

########################################################################
#
# PURPOSE:	change a cached interrupt vector
#
# ARGUMENTS:	g0 = interrupt number
#		g1 = new interrupt vector
#
# RETURNS:	none
########################################################################

	.text
	.globl		_asm_ivector

	.text
_asm_ivector:
	addo	1, g0, g0
	st	g1, [g0 * 4]
	ret

###############
# End of file #
###############
