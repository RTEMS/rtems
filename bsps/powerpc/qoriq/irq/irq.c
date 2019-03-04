/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQ
 *
 * @brief Interrupt implementation.
 */

/*
 * Copyright (c) 2010, 2017 embedded brains GmbH.  All rights reserved.
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

#include <asm/epapr_hcalls.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/vectors.h>
#include <bsp/utility.h>
#include <bsp/qoriq.h>

#ifdef RTEMS_SMP
#include <rtems/score/smpimpl.h>
#endif

RTEMS_INTERRUPT_LOCK_DEFINE(static, lock, "QorIQ IRQ")

#define SPURIOUS 0xffff

#ifdef QORIQ_IS_HYPERVISOR_GUEST

void bsp_interrupt_set_affinity(
	rtems_vector_number vector,
	const Processor_mask *affinity
)
{
	uint32_t config;
	unsigned int priority;
	uint32_t destination;
	uint32_t new_destination;
	rtems_interrupt_lock_context lock_context;

	new_destination = _Processor_mask_Find_last_set(affinity) - 1;

	rtems_interrupt_lock_acquire(&lock, &lock_context);
	ev_int_get_config(vector, &config, &priority, &destination);
	ev_int_set_config(vector, config, priority, new_destination);
	rtems_interrupt_lock_release(&lock, &lock_context);
}

void bsp_interrupt_get_affinity(
	rtems_vector_number vector,
	Processor_mask *affinity
)
{
	uint32_t config;
	unsigned int priority;
	uint32_t destination;

	ev_int_get_config(vector, &config, &priority, &destination);
	_Processor_mask_From_uint32_t(affinity, destination, 0);
}

void bsp_interrupt_vector_enable(rtems_vector_number vector)
{
	bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
	ev_int_set_mask(vector, 0);
}

void bsp_interrupt_vector_disable(rtems_vector_number vector)
{
	bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
	ev_int_set_mask(vector, 1);
}

void bsp_interrupt_dispatch(uintptr_t exception_number)
{
	unsigned int vector;

	if (exception_number == 10) {
		qoriq_decrementer_dispatch();
		return;
	}

#ifdef RTEMS_SMP
	if (exception_number == 36) {
		_SMP_Inter_processor_interrupt_handler(_Per_CPU_Get());
		return;
	}
#endif

	/*
	 * This works only if the "has-external-proxy" property is present in the
	 * "epapr,hv-pic" device tree node.
	 */
	vector = PPC_SPECIAL_PURPOSE_REGISTER(FSL_EIS_EPR);

	if (vector != SPURIOUS) {
		uint32_t msr;

		msr = ppc_external_exceptions_enable();
		bsp_interrupt_handler_dispatch(vector);
		ppc_external_exceptions_disable(msr);

		ev_int_eoi(vector);
	} else {
		bsp_interrupt_handler_default(vector);
	}
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
	unsigned int i;

	for (i = BSP_INTERRUPT_VECTOR_MIN; i <= BSP_INTERRUPT_VECTOR_MAX; ++i) {
		uint32_t config;
		unsigned int priority;
		uint32_t destination;
		unsigned int err;

		err = ev_int_get_config(i, &config, &priority, &destination);
		if (err != EV_SUCCESS)
			continue;

		priority = QORIQ_PIC_PRIORITY_DEFAULT;

		ev_int_set_config(i, config, priority, destination);
	}

	return RTEMS_SUCCESSFUL;
}

#else /* !QORIQ_IS_HYPERVISOR_GUEST */

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
		uintptr_t offs = ((uintptr_t)
			src_cfg_offsets [vector - QORIQ_IRQ_EXT_BASE]) << 4;

		return (volatile qoriq_pic_src_cfg *) ((uintptr_t) &qoriq.pic + offs);
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

void bsp_interrupt_set_affinity(
	rtems_vector_number vector,
	const Processor_mask *affinity
)
{
	volatile qoriq_pic_src_cfg *src_cfg = get_src_cfg(vector);

	src_cfg->dr = _Processor_mask_To_uint32_t(affinity, 0);
}

void bsp_interrupt_get_affinity(
	rtems_vector_number vector,
	Processor_mask *affinity
)
{
	volatile qoriq_pic_src_cfg *src_cfg = get_src_cfg(vector);

	_Processor_mask_From_uint32_t(affinity, src_cfg->dr, 0);
}

static void pic_vector_enable(rtems_vector_number vector, uint32_t msk)
{
	volatile qoriq_pic_src_cfg *src_cfg = get_src_cfg(vector);
	rtems_interrupt_lock_context lock_context;

	bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

	rtems_interrupt_lock_acquire(&lock, &lock_context);
	src_cfg->vpr = (src_cfg->vpr & ~VPR_MSK) | msk;
	rtems_interrupt_lock_release(&lock, &lock_context);
}

void bsp_interrupt_vector_enable(rtems_vector_number vector)
{
	pic_vector_enable(vector, 0);
}

void bsp_interrupt_vector_disable(rtems_vector_number vector)
{
	pic_vector_enable(vector, VPR_MSK);
}

void bsp_interrupt_dispatch(uintptr_t exception_number)
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

	if (processor_id == 0) {
		/* Core 0 must do the basic initialization */

		pic_reset();

		for (i = BSP_INTERRUPT_VECTOR_MIN; i <= BSP_INTERRUPT_VECTOR_MAX; ++i) {
			volatile qoriq_pic_src_cfg *src_cfg = get_src_cfg(i);

			src_cfg->vpr = VPR_MSK | VPR_P
				| VPR_PRIORITY(QORIQ_PIC_PRIORITY_DEFAULT) | VPR_VECTOR(i);

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

#endif /* QORIQ_IS_HYPERVISOR_GUEST */
