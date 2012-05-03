/**
 * @file
 *
 * @ingroup QorIQ
 *
 * @brief Interrupt implementation.
 */

/*
 * Copyright (c) 2010, 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>

#include <libcpu/powerpc-utility.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/vectors.h>
#include <bsp/utility.h>
#include <bsp/qoriq.h>

#define VPR_MSK BSP_BBIT32(0)
#define VPR_A BSP_BBIT32(1)
#define VPR_P BSP_BBIT32(8)
#define VPR_S BSP_BBIT32(9)
#define VPR_PRIORITY(val) BSP_BFLD32(val, 12, 15)
#define VPR_PRIORITY_GET(reg) BSP_BFLD32GET(reg, 12, 15)
#define VPR_PRIORITY_SET(reg, val) BSP_BFLD32SET(reg, val, 12, 15)
#define VPR_VECTOR(val) BSP_BFLD32(val, 16, 31)
#define VPR_VECTOR_GET(reg) BSP_BFLD32GET(reg, 16, 31)
#define VPR_VECTOR_SET(reg, val) BSP_BFLD32SET(reg, val, 16, 31)

#define GCR_RST BSP_BBIT32(0)
#define GCR_M BSP_BBIT32(2)

#define SPURIOUS 0xffff

static const uint16_t vpr_and_dr_offsets [] = {
	[0] = 0x10200 >> 4,
	[1] = 0x10220 >> 4,
	[2] = 0x10240 >> 4,
	[3] = 0x10260 >> 4,
	[4] = 0x10280 >> 4,
	[5] = 0x102a0 >> 4,
	[6] = 0x102c0 >> 4,
	[7] = 0x102e0 >> 4,
	[8] = 0x10300 >> 4,
	[9] = 0x10320 >> 4,
	[10] = 0x10340 >> 4,
	[11] = 0x10360 >> 4,
	[12] = 0x10380 >> 4,
	[13] = 0x103a0 >> 4,
	[14] = 0x103c0 >> 4,
	[15] = 0x103e0 >> 4,
	[16] = 0x10400 >> 4,
	[17] = 0x10420 >> 4,
	[18] = 0x10440 >> 4,
	[19] = 0x10460 >> 4,
	[20] = 0x10480 >> 4,
	[21] = 0x104a0 >> 4,
	[22] = 0x104c0 >> 4,
	[23] = 0x104e0 >> 4,
	[24] = 0x10500 >> 4,
	[25] = 0x10520 >> 4,
	[26] = 0x10540 >> 4,
	[27] = 0x10560 >> 4,
	[28] = 0x10580 >> 4,
	[29] = 0x105a0 >> 4,
	[30] = 0x105c0 >> 4,
	[31] = 0x105e0 >> 4,
	[32] = 0x10600 >> 4,
	[33] = 0x10620 >> 4,
	[34] = 0x10640 >> 4,
	[35] = 0x10660 >> 4,
	[36] = 0x10680 >> 4,
	[37] = 0x106a0 >> 4,
	[38] = 0x106c0 >> 4,
	[39] = 0x106e0 >> 4,
	[40] = 0x10700 >> 4,
	[41] = 0x10720 >> 4,
	[42] = 0x10740 >> 4,
	[43] = 0x10760 >> 4,
	[44] = 0x10780 >> 4,
	[45] = 0x107a0 >> 4,
	[46] = 0x107c0 >> 4,
	[47] = 0x107e0 >> 4,
	[48] = 0x10800 >> 4,
	[49] = 0x10820 >> 4,
	[50] = 0x10840 >> 4,
	[51] = 0x10860 >> 4,
	[52] = 0x10880 >> 4,
	[53] = 0x108a0 >> 4,
	[54] = 0x108c0 >> 4,
	[55] = 0x108e0 >> 4,
	[56] = 0x10900 >> 4,
	[57] = 0x10920 >> 4,
	[58] = 0x10940 >> 4,
	[59] = 0x10960 >> 4,
	[60] = 0x10980 >> 4,
	[61] = 0x109a0 >> 4,
	[62] = 0x109c0 >> 4,
	[63] = 0x109e0 >> 4,
	[QORIQ_IRQ_EXT_0] = 0x10000 >> 4,
	[QORIQ_IRQ_EXT_1] = 0x10020 >> 4,
	[QORIQ_IRQ_EXT_2] = 0x10040 >> 4,
	[QORIQ_IRQ_EXT_3] = 0x10060 >> 4,
	[QORIQ_IRQ_EXT_4] = 0x10080 >> 4,
	[QORIQ_IRQ_EXT_5] = 0x100a0 >> 4,
	[QORIQ_IRQ_EXT_6] = 0x100c0 >> 4,
	[QORIQ_IRQ_EXT_7] = 0x100e0 >> 4,
	[QORIQ_IRQ_EXT_8] = 0x10100 >> 4,
	[QORIQ_IRQ_EXT_9] = 0x10120 >> 4,
	[QORIQ_IRQ_EXT_10] = 0x10140 >> 4,
	[QORIQ_IRQ_EXT_11] = 0x10160 >> 4,
	[QORIQ_IRQ_IPI_0] = 0x010a0 >> 4,
	[QORIQ_IRQ_IPI_1] = 0x010b0 >> 4,
	[QORIQ_IRQ_IPI_2] = 0x010c0 >> 4,
	[QORIQ_IRQ_IPI_3] = 0x010d0 >> 4,
	[QORIQ_IRQ_MI_0] = 0x11600 >> 4,
	[QORIQ_IRQ_MI_1] = 0x11620 >> 4,
	[QORIQ_IRQ_MI_2] = 0x11640 >> 4,
	[QORIQ_IRQ_MI_3] = 0x11660 >> 4,
	[QORIQ_IRQ_MI_4] = 0x11680 >> 4,
	[QORIQ_IRQ_MI_5] = 0x116a0 >> 4,
	[QORIQ_IRQ_MI_6] = 0x116c0 >> 4,
	[QORIQ_IRQ_MI_7] = 0x116e0 >> 4,
	[QORIQ_IRQ_MSI_0] = 0x11c00 >> 4,
	[QORIQ_IRQ_MSI_1] = 0x11c20 >> 4,
	[QORIQ_IRQ_MSI_2] = 0x11c40 >> 4,
	[QORIQ_IRQ_MSI_3] = 0x11c60 >> 4,
	[QORIQ_IRQ_MSI_4] = 0x11c80 >> 4,
	[QORIQ_IRQ_MSI_5] = 0x11ca0 >> 4,
	[QORIQ_IRQ_MSI_6] = 0x11cc0 >> 4,
	[QORIQ_IRQ_MSI_7] = 0x11ce0 >> 4,
	[QORIQ_IRQ_GT_A_0] = 0x01120 >> 4,
	[QORIQ_IRQ_GT_A_1] = 0x01160 >> 4,
	[QORIQ_IRQ_GT_A_2] = 0x011a0 >> 4,
	[QORIQ_IRQ_GT_A_3] = 0x011e0 >> 4,
	[QORIQ_IRQ_GT_B_0] = 0x02120 >> 4,
	[QORIQ_IRQ_GT_B_1] = 0x02160 >> 4,
	[QORIQ_IRQ_GT_B_2] = 0x021a0 >> 4,
	[QORIQ_IRQ_GT_B_3] = 0x021e0 >> 4
};

rtems_status_code qoriq_pic_set_priority(
	rtems_vector_number vector,
	int new_priority,
	int *old_priority
)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	uint32_t old_vpr = 0;

	if (bsp_interrupt_is_valid_vector(vector)) {
		int offs = vpr_and_dr_offsets [vector] << 2;
		volatile uint32_t *vpr = (volatile uint32_t *) &qoriq.pic + offs;

		if (QORIQ_PIC_PRIORITY_IS_VALID(new_priority)) {
			rtems_interrupt_level level;

			rtems_interrupt_disable(level);
			old_vpr = *vpr;
			*vpr = VPR_PRIORITY_SET(old_vpr, (uint32_t) new_priority);
			rtems_interrupt_enable(level);
		} else if (new_priority < 0) {
			old_vpr = *vpr;
		} else {
			sc = RTEMS_INVALID_PRIORITY;
		}
	} else {
		sc = RTEMS_INVALID_ID;
	}

	if (old_priority != NULL) {
		*old_priority = (int) VPR_PRIORITY_GET(old_vpr);
	}

	return sc;
}

rtems_status_code qoriq_pic_set_affinity(
	rtems_vector_number vector,
	uint32_t processor_index
)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	if (bsp_interrupt_is_valid_vector(vector)) {
		if (processor_index <= 1) {
			int offs = (vpr_and_dr_offsets [vector] << 2) + 4;
			volatile uint32_t *dr = (volatile uint32_t *) &qoriq.pic + offs;

			*dr = BSP_BIT32(processor_index);
		} else {
			sc = RTEMS_INVALID_NUMBER;
		}
	} else {
		sc = RTEMS_INVALID_ID;
	}

	return sc;
}

static rtems_status_code pic_vector_enable(rtems_vector_number vector, uint32_t msk)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	if (bsp_interrupt_is_valid_vector(vector)) {
		int offs = vpr_and_dr_offsets [vector] << 2;
		volatile uint32_t *vpr = (volatile uint32_t *) &qoriq.pic + offs;
		rtems_interrupt_level level;

		rtems_interrupt_disable(level);
		*vpr = (*vpr & ~VPR_MSK) | msk;
		rtems_interrupt_enable(level);
	}

	return sc;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
	return pic_vector_enable(vector, 0);
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
	return pic_vector_enable(vector, VPR_MSK);
}

static int qoriq_external_exception_handler(BSP_Exception_frame *frame, unsigned exception_number)
{
	rtems_vector_number vector = qoriq.pic.iack;

	if (vector != SPURIOUS) {
		uint32_t msr = ppc_external_exceptions_enable();

		bsp_interrupt_handler_dispatch(vector);

		ppc_external_exceptions_disable(msr);

		qoriq.pic.eoi = 0;
		qoriq.pic.whoami;
	} else {
		bsp_interrupt_handler_default(vector);
	}

	return 0;
}

static bool pic_is_ipi(rtems_vector_number vector)
{
	return QORIQ_IRQ_IPI_0 <= vector && vector <= QORIQ_IRQ_IPI_3;
}

static void pic_reset(void)
{
	qoriq.pic.gcr = GCR_RST;
	while ((qoriq.pic.gcr & GCR_RST) != 0) {
		/* Wait */
	}
}

static void pic_global_timer_init(void)
{
	int i = 0;

	qoriq.pic.tcra = 0;
	qoriq.pic.tcrb = 0;

	for (i = 0; i < 4; ++i) {
		qoriq.pic.gta [0].bcr = GTBCR_CI;
		qoriq.pic.gtb [0].bcr = GTBCR_CI;
	}
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
	rtems_vector_number i = 0;
	uint32_t processor_id = ppc_processor_id();

	if (ppc_exc_set_handler(ASM_EXT_VECTOR, qoriq_external_exception_handler)) {
		return RTEMS_IO_ERROR;
	}

	if (processor_id == 0) {
		/* Core 0 must do the basic initialization */

		pic_reset();

		for (i = BSP_INTERRUPT_VECTOR_MIN; i <= BSP_INTERRUPT_VECTOR_MAX; ++i) {
			volatile uint32_t *base = (volatile uint32_t *) &qoriq.pic;
			int offs = vpr_and_dr_offsets [i] << 2;
			volatile uint32_t *vpr = base + offs;

			*vpr = VPR_MSK | VPR_P | VPR_PRIORITY(1) | VPR_VECTOR(i);

			if (!pic_is_ipi(i)) {
				volatile uint32_t *dr = base + offs + 4;

				*dr = 0x1;
			}
		}

		qoriq.pic.mer03 = 0xf;
		qoriq.pic.mer47 = 0xf;
		qoriq.pic.svr = SPURIOUS;
		qoriq.pic.gcr = GCR_M;

		pic_global_timer_init();
	}

	qoriq.pic.ctpr = 0;

	for (i = BSP_INTERRUPT_VECTOR_MIN; i <= BSP_INTERRUPT_VECTOR_MAX; ++i) {
		qoriq.pic.iack;
		qoriq.pic.eoi = 0;
		qoriq.pic.whoami;
	}

	return RTEMS_SUCCESSFUL;
}
