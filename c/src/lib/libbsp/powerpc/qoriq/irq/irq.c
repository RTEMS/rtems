/**
 * @file
 *
 * @ingroup QorIQ
 *
 * @brief Interrupt implementation.
 */

/*
 * Copyright (c) 2010-2015 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <sys/param.h>

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

RTEMS_INTERRUPT_LOCK_DEFINE(static, lock, "QorIQ IRQ")

#define SRC_CFG_IDX(i) ((i) - QORIQ_IRQ_EXT_BASE)

static const uint16_t src_cfg_offsets [] = {
	[SRC_CFG_IDX(QORIQ_IRQ_EXT_0)] = 0x10000 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_EXT_1)] = 0x10020 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_EXT_2)] = 0x10040 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_EXT_3)] = 0x10060 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_EXT_4)] = 0x10080 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_EXT_5)] = 0x100a0 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_EXT_6)] = 0x100c0 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_EXT_7)] = 0x100e0 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_EXT_8)] = 0x10100 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_EXT_9)] = 0x10120 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_EXT_10)] = 0x10140 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_EXT_11)] = 0x10160 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_IPI_0)] = 0x010a0 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_IPI_1)] = 0x010b0 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_IPI_2)] = 0x010c0 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_IPI_3)] = 0x010d0 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_MI_0)] = 0x11600 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_MI_1)] = 0x11620 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_MI_2)] = 0x11640 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_MI_3)] = 0x11660 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_MI_4)] = 0x11680 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_MI_5)] = 0x116a0 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_MI_6)] = 0x116c0 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_MI_7)] = 0x116e0 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_MSI_0)] = 0x11c00 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_MSI_1)] = 0x11c20 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_MSI_2)] = 0x11c40 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_MSI_3)] = 0x11c60 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_MSI_4)] = 0x11c80 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_MSI_5)] = 0x11ca0 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_MSI_6)] = 0x11cc0 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_MSI_7)] = 0x11ce0 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_GT_A_0)] = 0x01120 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_GT_A_1)] = 0x01160 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_GT_A_2)] = 0x011a0 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_GT_A_3)] = 0x011e0 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_GT_B_0)] = 0x02120 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_GT_B_1)] = 0x02160 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_GT_B_2)] = 0x021a0 >> 4,
	[SRC_CFG_IDX(QORIQ_IRQ_GT_B_3)] = 0x021e0 >> 4
};

static volatile qoriq_pic_src_cfg *get_src_cfg(rtems_vector_number vector)
{
	uint32_t n = MIN(RTEMS_ARRAY_SIZE(qoriq.pic.ii_0), QORIQ_IRQ_EXT_BASE);

	if (vector < n) {
		return &qoriq.pic.ii_0 [vector];
	} else if (vector < QORIQ_IRQ_EXT_BASE) {
		return &qoriq.pic.ii_1 [vector - n];
	} else {
		uint32_t offs = ((uint32_t)
			src_cfg_offsets [vector - QORIQ_IRQ_EXT_BASE]) << 4;

		return (volatile qoriq_pic_src_cfg *) ((uint32_t) &qoriq.pic + offs);
	}
}

rtems_status_code qoriq_pic_set_priority(
	rtems_vector_number vector,
	int new_priority,
	int *old_priority
)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	uint32_t old_vpr = 0;

	if (bsp_interrupt_is_valid_vector(vector)) {
		volatile qoriq_pic_src_cfg *src_cfg = get_src_cfg(vector);

		if (QORIQ_PIC_PRIORITY_IS_VALID(new_priority)) {
			rtems_interrupt_lock_context lock_context;

			rtems_interrupt_lock_acquire(&lock, &lock_context);
			old_vpr = src_cfg->vpr;
			src_cfg->vpr = VPR_PRIORITY_SET(old_vpr, (uint32_t) new_priority);
			rtems_interrupt_lock_release(&lock, &lock_context);
		} else if (new_priority < 0) {
			old_vpr = src_cfg->vpr;
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

rtems_status_code qoriq_pic_set_affinities(
	rtems_vector_number vector,
	uint32_t processor_affinities
)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	if (bsp_interrupt_is_valid_vector(vector)) {
		volatile qoriq_pic_src_cfg *src_cfg = get_src_cfg(vector);

		src_cfg->dr = processor_affinities;
	} else {
		sc = RTEMS_INVALID_ID;
	}

	return sc;
}

rtems_status_code qoriq_pic_set_affinity(
	rtems_vector_number vector,
	uint32_t processor_index
)
{
	return qoriq_pic_set_affinities(vector, BSP_BIT32(processor_index));
}

static rtems_status_code pic_vector_enable(rtems_vector_number vector, uint32_t msk)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	if (bsp_interrupt_is_valid_vector(vector)) {
		volatile qoriq_pic_src_cfg *src_cfg = get_src_cfg(vector);
		rtems_interrupt_lock_context lock_context;

		rtems_interrupt_lock_acquire(&lock, &lock_context);
		src_cfg->vpr = (src_cfg->vpr & ~VPR_MSK) | msk;
		rtems_interrupt_lock_release(&lock, &lock_context);
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

static void qoriq_interrupt_dispatch(void)
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
}

#ifndef PPC_EXC_CONFIG_USE_FIXED_HANDLER
static int qoriq_external_exception_handler(BSP_Exception_frame *frame, unsigned exception_number)
{
	qoriq_interrupt_dispatch();

	return 0;
}
#else
void bsp_interrupt_dispatch(void)
{
	qoriq_interrupt_dispatch();
}
#endif

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

#ifndef PPC_EXC_CONFIG_USE_FIXED_HANDLER
	if (ppc_exc_set_handler(ASM_EXT_VECTOR, qoriq_external_exception_handler)) {
		return RTEMS_IO_ERROR;
	}
#endif

	if (processor_id == 0) {
		/* Core 0 must do the basic initialization */

		pic_reset();

		for (i = BSP_INTERRUPT_VECTOR_MIN; i <= BSP_INTERRUPT_VECTOR_MAX; ++i) {
			volatile qoriq_pic_src_cfg *src_cfg = get_src_cfg(i);

			src_cfg->vpr = VPR_MSK | VPR_P | VPR_PRIORITY(1) | VPR_VECTOR(i);

			if (!pic_is_ipi(i)) {
				src_cfg->dr = 0x1;
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
