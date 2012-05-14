/*  usc.S
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bspopts.h>
#include <rtems/asm.h>
#include <rtems/mips/iregdef.h>
#include <rtems/mips/idtcpu.h>
#if BSP_HAS_USC320
  #include <usc.h>
#endif


/***************************************************************************
**
**   The following code was added to support boards using V3 USC320
**     system controller chip.
**
****************************************************************************/

/*************************************************************
*  init_hbt()
*	Initialize the heartbeat timer
*/
FRAME(init_hbt,sp,0,ra)
	.set noreorder
	la	t0,SYSTEM	# Unlock USC registers
	li	t1,0xA5
	sb	t1,(t0)

	la	t0,WD_HBI	# Initialize heatbeat and watchdog timers

				# (1 / 64 MHz) * 4000 * (63 + 1) = 4000.0 microseconds
				# Watchdog period is heartbeat period times watchdog timer constant (bits 7 - 0)
				# Watchdog period = 4000 * 5 = 20000 microseconds
	li	t1,(WD_EN | HBI_4000_PS | 0x00003F00 | 0x5)

				# (1 / 64 MHz) * 4000 * (15 + 1) = 1000.0 microseconds
				# Watchdog period is heartbeat period times watchdog timer constant (bits 7 - 0)
				# Watchdog period = 1000 * 20 = 20000 microseconds
	li	t1,(WD_EN | HBI_4000_PS | 0x00000F00 | 0x14)

				# (1 / 64 MHz) * 40000 * (15 + 1) = 10000.0 microseconds
				# Watchdog period is heartbeat period times watchdog timer constant (bits 7 - 0)
				# Watchdog period = 10000 * 20 = 200000 microseconds
	li	t1,(WD_EN | HBI_4000_PS | 0x00009600 | 0x14)

	sw	t1,(t0)

	la	t0,SYSTEM	# Lock USC registers
	li	t1,0x60
	sb	t1,(t0)

	.set reorder
	j	ra
	nop
	.set reorder
ENDFRAME(init_hbt)

/*************************************************************
*  reset_wdt()
*	Reset the watchdog timer
*/
FRAME(reset_wdt,sp,0,ra)
	.set noreorder

	la	t0,WD_HBI+2	# Load address watchdog timer reset byte
	li	t1,WD_INIT
	sb	t1,(t0)

	.set reorder
	j	ra
	nop
	.set reorder
ENDFRAME(reset_wdt)

/*************************************************************
*  disable_wdt()
*	Disable watchdog timer
*/
FRAME(disable_wdt,sp,0,ra)
	.set noreorder
	la	t0,WD_HBI	# Clear watchdog enable bit in control register
	lw	t1,(t0)
	li	t2,~WD_EN
	and	t1,t1,t2
	sw	t1,(t0)

	.set reorder
	j	ra
	nop
	.set reorder
ENDFRAME(disable_wdt)

/*************************************************************
*  enable_hbi(ints)
*	Enable the heartbeat interrupt
*/
FRAME(enable_hbi,sp,0,ra)
	.set noreorder

	la	t0,INT_CFG3	# Enable heartbeat interrupt in USC320
	lw	t1,(t0)
	li	t2,(HBI_MASK | MODE_TOTEM_POLE)
	or	t1,t1,t2
	sw	t1,(t0)

	.set reorder
	j	ra
	nop
	.set reorder
ENDFRAME(enable_hbi)

/*************************************************************
*  disable_hbi(ints)
*	Disable the heartbeat interrupt
*/
FRAME(disable_hbi,sp,0,ra)
	.set noreorder
	la	t0,INT_CFG3	# Disable heartbeat interrupt in USC320
	lw	t1,(t0)
	li	t2,~HBI_MASK
	and	t1,t1,t2
	sw	t1,(t0)

	.set reorder
	j	ra
	nop
	.set reorder
ENDFRAME(disable_hbi)


/*************************************************************
*  enable_wdi()
*	Enable the watchdog interrupt
*/
FRAME(enable_wdi,sp,0,ra)
	.set noreorder

	la	t0,INT_CFG1	# Enable watchdog interrupt in USC320
	lw	t1,(t0)
	li	t2,(WDI_MASK | MODE_TOTEM_POLE)
	or	t1,t1,t2
	sw	t1,(t0)

	.set reorder
	j	ra
	nop
	.set reorder
ENDFRAME(enable_wdi)

/*************************************************************
*  disable_wdi(ints)
*	Disable the watchdog interrupt
*/
FRAME(disable_wdi,sp,0,ra)
	.set noreorder

	la	t0,INT_CFG1	# Disable watchdog interrupt in USC320
	lw	t1,(t0)
	li	t2,~(WDI_MASK | MODE_TOTEM_POLE)
	and	t1,t1,t2
	sw	t1,(t0)

	la	t0,INT_STAT	# Clear watchdog interrupt status bit
	li	t1,WDI_MASK
	sw	t1,(t0)

	.set reorder
	j	ra
	nop
	.set reorder
ENDFRAME(disable_wdi)

