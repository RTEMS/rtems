/*
 *  boardinit.s
 *
 *  Initialise various ERC32 registers
 */

#include <rtems/asm.h>
#include <erc32.h>

        .global __bsp_board_init
__bsp_board_init:

/* Check if MEC is initialised. If not, this means that we are
   running on the simulator. Initiate some of the parameters
   that are done by the boot-prom otherwise.
*/

        set     SYM(ERC32_MEC), %g3  ! g3 = base address of peripherals
	ld	[%g3], %g2
	set	0xfe080000, %g1
	andcc	%g1, %g2, %g0
 	bne	2f

 /* Stop the watchdog */

	st	%g0, [%g3 + SYM(ERC32_MEC_WATCHDOG_TRAP_DOOR_SET_OFFSET)]

 /* Set zero waitstates */

	st	%g0, [%g3 + SYM(ERC32_MEC_WAIT_STATE_CONFIGURATION_OFFSET)]

 /* Set the correct memory size in MEC memory config register */

 	set	SYM(PROM_SIZE), %l0
 	set	0, %l1
 	srl	%l0, 18, %l0
1:
 	tst	%l0
 	srl	%l0, 1, %l0
 	bne,a	1b
 	inc	%l1
 	sll	%l1, 8, %l1

 	set	SYM(RAM_SIZE), %l0
 	srl	%l0, 19, %l0
1:
 	tst	%l0
 	srl	%l0, 1, %l0
 	bne,a	1b
 	inc	%l1
 	sll	%l1, 10, %l1

                                           ! set the Memory Configuration
        st     %l1, [ %g3 + ERC32_MEC_MEMORY_CONFIGURATION_OFFSET ]

 	set	SYM(RAM_START), %l1  ! Cannot use RAM_END due to bug in linker
 	set	SYM(RAM_SIZE), %l2
 	add	%l1, %l2, %sp

	set	SYM(CLOCK_SPEED), %g6	! Use 14 MHz in simulator
	set	14, %g1
	st	%g1, [%g6]

2:

 /* Initialise timer */

 	set	SYM(_ERC32_MEC_Timer_Control_Mirror), %l2
	st	%g0, [%l2]
	st	%g0, [%g3 + SYM(ERC32_MEC_TIMER_CONTROL_OFFSET)]

 /* Enable power-down */

	ld	[%g3 + SYM(ERC32_MEC_CONTROL_OFFSET)], %l2
	or	%l2, ERC32_CONFIGURATION_POWER_DOWN_ALLOWED, %l2
	st	%l2, [%g3 + SYM(ERC32_MEC_CONTROL_OFFSET)]

	retl
	nop

/* end of file */
