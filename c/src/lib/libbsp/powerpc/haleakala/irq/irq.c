/*
 *
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Middleware support for PPC405 by M.Hamel ADInstruments Ltd 2008
 */

#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq_supp.h>
#include <bsp/vectors.h>
#include <libcpu/powerpc-utility.h>


/* PPC405EX UIC numbers */
#define UIC_DCR_BASE 0xc0
#define UIC0_SR      (UIC_DCR_BASE+0x0)  /* UIC status                       */
#define UIC0_SRS     (UIC_DCR_BASE+0x1)  /* UIC status set                   */
#define UIC0_ER      (UIC_DCR_BASE+0x2)  /* UIC enable                       */
#define UIC0_CR      (UIC_DCR_BASE+0x3)  /* UIC critical                     */
#define UIC0_PR      (UIC_DCR_BASE+0x4)  /* UIC polarity                     */
#define UIC0_TR      (UIC_DCR_BASE+0x5)  /* UIC triggering                   */
#define UIC0_MSR     (UIC_DCR_BASE+0x6)  /* UIC masked status                */
#define UIC0_VR      (UIC_DCR_BASE+0x7)  /* UIC vector                       */
#define UIC0_VCR     (UIC_DCR_BASE+0x8)  /* UIC vector configuration         */

#define UIC1_SR      (UIC_DCR_BASE+0x10)  /* UIC status                       */
#define UIC1_SRS     (UIC_DCR_BASE+0x11)  /* UIC status set                   */
#define UIC1_ER      (UIC_DCR_BASE+0x12)  /* UIC enable                       */
#define UIC1_CR      (UIC_DCR_BASE+0x13)  /* UIC critical                     */
#define UIC1_PR      (UIC_DCR_BASE+0x14)  /* UIC polarity                     */
#define UIC1_TR      (UIC_DCR_BASE+0x15)  /* UIC triggering                   */
#define UIC1_MSR     (UIC_DCR_BASE+0x16)  /* UIC masked status                */
#define UIC1_VR      (UIC_DCR_BASE+0x17)  /* UIC vector                       */
#define UIC1_VCR     (UIC_DCR_BASE+0x18)  /* UIC vector configuration         */

#define UIC2_SR      (UIC_DCR_BASE+0x20)  /* UIC status                       */
#define UIC2_SRS     (UIC_DCR_BASE+0x21)  /* UIC status set                   */
#define UIC2_ER      (UIC_DCR_BASE+0x22)  /* UIC enable                       */
#define UIC2_CR      (UIC_DCR_BASE+0x23)  /* UIC critical                     */
#define UIC2_PR      (UIC_DCR_BASE+0x24)  /* UIC polarity                     */
#define UIC2_TR      (UIC_DCR_BASE+0x25)  /* UIC triggering                   */
#define UIC2_MSR     (UIC_DCR_BASE+0x26)  /* UIC masked status                */
#define UIC2_VR      (UIC_DCR_BASE+0x27)  /* UIC vector                       */
#define UIC2_VCR     (UIC_DCR_BASE+0x28)  /* UIC vector configuration         */

enum { kUICWords = 3 };

static rtems_irq_connect_data* rtems_hdl_tblP;
static rtems_irq_connect_data  dflt_entry;

static uint32_t	gEnabledInts[kUICWords];		/* 1-bits mean enabled */
static uint32_t	gIntInhibited[kUICWords];		/* 1-bits disable, overriding gEnabledInts because the interrupt
												   is being processed in C_dispatch_irq_handler */

static inline int IsUICIRQ(const rtems_irq_number irqLine)
{
  return (((int) irqLine <= BSP_UIC_IRQ_MAX_OFFSET) &&
	  ((int) irqLine >= BSP_UIC_IRQ_LOWEST_OFFSET)
	 );
}

static void WriteIState(void)
/* Write the gEnabledInts state masked by gIntInhibited to the hardware */
{
	PPC_SET_DEVICE_CONTROL_REGISTER(UIC0_ER,
					gEnabledInts[0] & ~gIntInhibited[0]);
	PPC_SET_DEVICE_CONTROL_REGISTER(UIC1_ER,
					gEnabledInts[1] & ~gIntInhibited[1]);
	PPC_SET_DEVICE_CONTROL_REGISTER(UIC2_ER,
					gEnabledInts[2] & ~gIntInhibited[2]);
}

void
BSP_enable_irq_at_pic(const rtems_irq_number irq)
/* Enable an interrupt; this can be called from inside C_dispatch_irq_handler */
{
	if (IsUICIRQ(irq)) {
		/* Set relevant bit in the state, write state to the UIC */
		gEnabledInts[irq>>5] |= (0x80000000 >> (irq & 0x1F));
		WriteIState();
	}
}

int
BSP_disable_irq_at_pic(const rtems_irq_number irq)
/* Enable an interrupt; this can be called from inside C_dispatch_irq_handler */
{
	if (IsUICIRQ(irq)) {
		uint32_t oldState;
		int		 iword = irq>>5;
		uint32_t mask = (0x80000000 >> (irq & 0x1F));

		oldState = gEnabledInts[iword] & mask;
		gEnabledInts[iword] &= ~mask;
		WriteIState();
		return oldState ? 1 : 0;
	} else
		return -1;
}

int
BSP_setup_the_pic(rtems_irq_global_settings* config)
{
	int i;

	dflt_entry     = config->defaultEntry;
	rtems_hdl_tblP = config->irqHdlTbl;
	for (i=0; i<kUICWords; i++)
		gIntInhibited[i] = 0;

	/* disable all interrupts */
	PPC_SET_DEVICE_CONTROL_REGISTER (UIC2_ER, 0x00000000);
	/* Set Critical / Non Critical interrupts */
	PPC_SET_DEVICE_CONTROL_REGISTER (UIC2_CR, 0x00000000);
	/* Set Interrupt Polarities */
	PPC_SET_DEVICE_CONTROL_REGISTER (UIC2_PR, 0xf7ffffff);
	/* Set Interrupt Trigger Levels */
	PPC_SET_DEVICE_CONTROL_REGISTER (UIC2_TR, 0x01e1fff8);
	/* Set Vect base=0,INT31 Highest priority */
	PPC_SET_DEVICE_CONTROL_REGISTER (UIC2_VR, 0x00000001);
	/* clear all interrupts */
	PPC_SET_DEVICE_CONTROL_REGISTER (UIC2_SR, 0xffffffff);

	/* disable all interrupts */
	PPC_SET_DEVICE_CONTROL_REGISTER (UIC1_ER, 0x00000000);
	/* Set Critical / Non Critical interrupts */
	PPC_SET_DEVICE_CONTROL_REGISTER (UIC1_CR, 0x00000000);
	/* Set Interrupt Polarities */
	PPC_SET_DEVICE_CONTROL_REGISTER (UIC1_PR, 0xfffac785);
	/* Set Interrupt Trigger Levels */
	PPC_SET_DEVICE_CONTROL_REGISTER (UIC1_TR, 0x001d0040);
	/* Set Vect base=0,INT31 Highest priority */
	PPC_SET_DEVICE_CONTROL_REGISTER (UIC1_VR, 0x00000001);
	/* clear all interrupts */
	PPC_SET_DEVICE_CONTROL_REGISTER (UIC1_SR, 0xffffffff);

	/* Disable all interrupts except cascade UIC0 and UIC1 */
	PPC_SET_DEVICE_CONTROL_REGISTER (UIC0_ER, 0x0000000a);
	/* Set Critical / Non Critical interrupts */
	PPC_SET_DEVICE_CONTROL_REGISTER (UIC0_CR, 0x00000000);
	/* Set Interrupt Polarities */
	PPC_SET_DEVICE_CONTROL_REGISTER (UIC0_PR, 0xffbfefef);
	/* Set Interrupt Trigger Levels */
	PPC_SET_DEVICE_CONTROL_REGISTER (UIC0_TR, 0x00007000);
	/* Set Vect base=0,INT31 Highest priority */
	PPC_SET_DEVICE_CONTROL_REGISTER (UIC0_VR, 0x00000001);
	/* clear all interrupts */
	PPC_SET_DEVICE_CONTROL_REGISTER (UIC0_SR, 0xffffffff);

	return 1;
}


/*
 * High level IRQ handler called from shared_raw_irq_code_entry; decode and
 * dispatch. Note that this routine needs to be re-entrant
 *
 * No support for critical interrupts here yet
 */

int
C_dispatch_irq_handler( BSP_Exception_frame* frame, unsigned int excNum )
{
	if (excNum == ASM_EXT_VECTOR) {
		uint32_t active[kUICWords];

		/* Fetch the masked flags that tell us what external ints are active.
		   Likely to be only one, but we need to handle more than one,
		   OR the flags into gIntInhibited */
		active[0] = PPC_DEVICE_CONTROL_REGISTER(UIC0_MSR);
		active[1] = PPC_DEVICE_CONTROL_REGISTER(UIC1_MSR);
		active[2] = PPC_DEVICE_CONTROL_REGISTER(UIC2_MSR);
		gIntInhibited[0] |= active[0];
		gIntInhibited[1] |= active[1];
		gIntInhibited[2] |= active[2];

		/*  ...and update the hardware so the active interrupts are disabled */
		WriteIState();

		/* Loop, calling bsp_irq_dispatch_list for each active interrupt */
		while ((active[0] | active[1] | active[2]) != 0) {
			uint32_t index = -1;
			uint32_t bit, bmask;

			/* Find an active interrupt, searching 0..2, bit 0..bit 31 (IBM order) */
			do {
				index++;
				asm volatile ("	cntlzw %0, %1":"=r" (bit):"r" (active[index]));
			} while (bit==32);

			/* Call the matching handler */
			bsp_irq_dispatch_list(rtems_hdl_tblP, (index*32)+bit, dflt_entry.hdl);

			/* Write a 1-bit to the appropriate status register to clear it */
			bmask = 0x80000000 >> bit;
			switch (index) {
			case 0:
			  PPC_SET_DEVICE_CONTROL_REGISTER(UIC0_SR, bmask);
			  break;
			case 1:
			  PPC_SET_DEVICE_CONTROL_REGISTER(UIC1_SR, bmask);
			  break;
			case 2:
			  PPC_SET_DEVICE_CONTROL_REGISTER(UIC2_SR, bmask);
			  break;
			}

			/* Clear in the active record and gIntInhibited */
			active[index] &= ~bmask;
			gIntInhibited[index] &= ~bmask;
		};

		/* Update the hardware again so the interrupts we have handled are unmasked */
		WriteIState();
		return 0;

	} else if (excNum == ASM_DEC_VECTOR) {		/* 0x1000 remapped by C_dispatch_dec_handler_bookE  */
		bsp_irq_dispatch_list(rtems_hdl_tblP, BSP_PIT, dflt_entry.hdl);
		return 0;

	} else if (excNum == ASM_BOOKE_FIT_VECTOR) {		/* 0x1010 mapped to 0x13 by ppc_get_vector_addr */
		bsp_irq_dispatch_list(rtems_hdl_tblP, BSP_FIT, dflt_entry.hdl);
		return 0;

	} else if (excNum == ASM_BOOKE_WDOG_VECTOR) {		/* 0x1020 mapped to 0x14 by ppc_get_vector_addr */
		bsp_irq_dispatch_list(rtems_hdl_tblP, BSP_WDOG, dflt_entry.hdl);
		return 0;

	} else
		return -1; /* unhandled interrupt, panic time */
}

