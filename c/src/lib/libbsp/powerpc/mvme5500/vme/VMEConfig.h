#ifndef RTEMS_BSP_VME_CONFIG_H
#define RTEMS_BSP_VME_CONFIG_H
/* VMEConfig.h, S. Kate Feng modified it for MVME5500 3/04 
 * 
 * May 2011 : Use the VME shared IRQ handlers.
 *
 * It seems that the implementation of VMEUNIVERSE_IRQ_MGR_FLAG_PW_WORKAROUND
 * is not fully developed. The UNIV_REGOFF_VCSR_BS is defined for VME64
 * specification, which does not apply to a VME32 crate. In order to avoid
 * spurious VME interrupts, a better and more universal solution is
 * to flush the vmeUniverse FIFO by reading a register back within the
 * users' Interrupt Service Routine (ISR)  before returning.
 *
 * Some devices might require the ISR to issue an interrupt status READ
 * after its IRQ is cleared, but before its corresponding interrupt
 * is enabled again. 
 * 
 */
/* BSP specific address space configuration parameters */

/* 
 * The BSP maps VME address ranges into
 * one BAT.
 * NOTE: the BSP (startup/bspstart.c) uses
 * hardcoded window lengths that match this
 * layout:
 */
#define _VME_A32_WIN0_ON_PCI	        0x90000000
/* If _VME_CSR_ON_PCI is defined then the A32 window is reduced to accommodate
 * CSR for space.
 */
#define _VME_CSR_ON_PCI			0x9e000000
#define _VME_A24_ON_PCI			0x9f000000
#define _VME_A16_ON_PCI			0x9fff0000

/* Reuse BAT 0 for VME */
#define BSP_VME_BAT_IDX			0

/* start of the A32 window on the VME bus
 * TODO: this should perhaps be a configuration option
 */
#define _VME_A32_WIN0_ON_VME	0x20000000

/* if _VME_DRAM_OFFSET is defined, the BSP
 * will map our RAM onto the VME bus, starting
 * at _VME_DRAM_OFFSET
 */
#define _VME_DRAM_OFFSET		0x90000000

#define BSP_VME_UNIVERSE_INSTALL_IRQ_MGR(err)			\
	do {											\
	  err = vmeUniverseInstallIrqMgrAlt(VMEUNIVERSE_IRQ_MGR_FLAG_SHARED,\
             0, BSP_GPP_VME_VLINT0, \         
             1, BSP_GPP_VME_VLINT1, \          
             2, BSP_GPP_VME_VLINT2, \        
             3, BSP_GPP_VME_VLINT3, \
             -1 /* terminate list  */);  \             
	} while (0)

#endif
