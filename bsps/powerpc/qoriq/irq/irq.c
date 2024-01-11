/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQ
 *
 * @brief Interrupt implementation.
 */

/*
 * Copyright (C) 2010, 2017 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/param.h>
#include <sys/bitset.h>

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

rtems_status_code bsp_interrupt_set_affinity(
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
	return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_get_affinity(
	rtems_vector_number vector,
	Processor_mask *affinity
)
{
	uint32_t config;
	unsigned int priority;
	uint32_t destination;

	ev_int_get_config(vector, &config, &priority, &destination);
	_Processor_mask_From_uint32_t(affinity, destination, 0);
	return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(pending != NULL);
  *pending = false;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_raise(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

#if defined(RTEMS_SMP)
rtems_status_code bsp_interrupt_raise_on(
  rtems_vector_number vector,
  uint32_t            cpu_index
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}
#endif

rtems_status_code bsp_interrupt_clear(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_is_enabled(
  rtems_vector_number vector,
  bool               *enabled
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(enabled != NULL);
  *enabled = false;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
	bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
	ev_int_set_mask(vector, 0);
	return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
	bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
	ev_int_set_mask(vector, 1);
	return RTEMS_SUCCESSFUL;
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
	PPC_SPECIAL_PURPOSE_REGISTER(FSL_EIS_EPR, vector);

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

void bsp_interrupt_facility_initialize(void)
{
	unsigned int i;

	for (i = 0; i < BSP_INTERRUPT_VECTOR_COUNT; ++i) {
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

static bool pic_is_ipi(rtems_vector_number vector)
{
	return (vector - QORIQ_IRQ_IPI_BASE) < 4;
}

rtems_status_code qoriq_pic_set_priority(
	rtems_vector_number vector,
	int new_priority,
	int *old_priority
)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	uint32_t old_vpr = 0;

	if (QORIQ_IRQ_IS_MSI(vector)) {
		return RTEMS_UNSATISFIED;
	}

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

rtems_status_code qoriq_pic_set_sense_and_polarity(
  rtems_vector_number vector,
  qoriq_eirq_sense_and_polarity new_sense_and_polarity,
  qoriq_eirq_sense_and_polarity *old_sense_and_polarity
)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	uint32_t old_vpr = 0;
	volatile qoriq_pic_src_cfg *src_cfg;
	rtems_interrupt_lock_context lock_context;
	uint32_t new_p_s = 0;

	if (!QORIQ_IRQ_IS_EXT(vector)) {
		return RTEMS_UNSATISFIED;
	}

	if (new_sense_and_polarity == QORIQ_EIRQ_TRIGGER_EDGE_RISING ||
	    new_sense_and_polarity == QORIQ_EIRQ_TRIGGER_LEVEL_HIGH) {
		new_p_s |= VPR_P;
	}

	if (new_sense_and_polarity == QORIQ_EIRQ_TRIGGER_LEVEL_HIGH ||
	    new_sense_and_polarity == QORIQ_EIRQ_TRIGGER_LEVEL_LOW) {
		new_p_s |= VPR_S;
	}

	src_cfg = get_src_cfg(vector);

	rtems_interrupt_lock_acquire(&lock, &lock_context);
	old_vpr = src_cfg->vpr;
	src_cfg->vpr = (old_vpr & ~(VPR_P | VPR_S)) | new_p_s;
	rtems_interrupt_lock_release(&lock, &lock_context);

	if (old_sense_and_polarity != NULL) {
		if ((old_vpr & VPR_P) == 0) {
			if ((old_vpr & VPR_S) == 0) {
				*old_sense_and_polarity =
					QORIQ_EIRQ_TRIGGER_EDGE_FALLING;
			} else {
				*old_sense_and_polarity =
					QORIQ_EIRQ_TRIGGER_LEVEL_LOW;
			}
		} else {
			if ((old_vpr & VPR_S) == 0) {
				*old_sense_and_polarity =
					QORIQ_EIRQ_TRIGGER_EDGE_RISING;
			} else {
				*old_sense_and_polarity =
					QORIQ_EIRQ_TRIGGER_LEVEL_HIGH;
			}
		}
	}

	return sc;
}

rtems_status_code bsp_interrupt_set_affinity(
	rtems_vector_number vector,
	const Processor_mask *affinity
)
{
	volatile qoriq_pic_src_cfg *src_cfg;

	if (pic_is_ipi(vector)) {
		return RTEMS_UNSATISFIED;
	}

	if (QORIQ_IRQ_IS_MSI(vector)) {
		return RTEMS_UNSATISFIED;
	}

	src_cfg = get_src_cfg(vector);
	src_cfg->dr = _Processor_mask_To_uint32_t(affinity, 0);
	return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_get_affinity(
	rtems_vector_number vector,
	Processor_mask *affinity
)
{
	volatile qoriq_pic_src_cfg *src_cfg;

	if (pic_is_ipi(vector)) {
		return RTEMS_UNSATISFIED;
	}

	if (QORIQ_IRQ_IS_MSI(vector)) {
		return RTEMS_UNSATISFIED;
	}

	src_cfg = get_src_cfg(vector);
	_Processor_mask_From_uint32_t(affinity, src_cfg->dr, 0);
	return RTEMS_SUCCESSFUL;
}

static rtems_status_code pic_vector_set_mask(
	rtems_vector_number vector,
	uint32_t msk
)
{
	volatile qoriq_pic_src_cfg *src_cfg;
	rtems_interrupt_lock_context lock_context;

	bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

	if (QORIQ_IRQ_IS_MSI(vector)) {
		return RTEMS_UNSATISFIED;
	}

	src_cfg = get_src_cfg(vector);
	rtems_interrupt_lock_acquire(&lock, &lock_context);
	src_cfg->vpr = (src_cfg->vpr & ~VPR_MSK) | msk;
	rtems_interrupt_lock_release(&lock, &lock_context);
	return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_get_attributes(
	rtems_vector_number vector,
	rtems_interrupt_attributes *attributes
)
{
	bool is_ipi = pic_is_ipi(vector);
	bool is_msi = QORIQ_IRQ_IS_MSI(vector);

	attributes->is_maskable = true;
	attributes->can_enable = !is_msi;
	attributes->maybe_enable = !is_msi;
	attributes->can_disable = !is_msi;
	attributes->maybe_disable = !is_msi;
	attributes->cleared_by_acknowledge = true;
	attributes->can_get_affinity = !(is_ipi || is_msi);
	attributes->can_set_affinity = !(is_ipi || is_msi);
	attributes->can_raise = is_ipi;
	attributes->can_raise_on = is_ipi;

	if (is_msi) {
		attributes->can_be_triggered_by_message = true;
		attributes->trigger_signal = RTEMS_INTERRUPT_NO_SIGNAL;
	}

	return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_is_pending(
	rtems_vector_number vector,
	bool *pending
)
{
	volatile qoriq_pic_src_cfg *src_cfg;

	bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
	bsp_interrupt_assert(pending != NULL);

	if (QORIQ_IRQ_IS_MSI(vector)) {
		*pending = false;
		return RTEMS_SUCCESSFUL;
	}

	src_cfg = get_src_cfg(vector);
	*pending = (src_cfg->vpr & VPR_A) != 0;
	return RTEMS_SUCCESSFUL;
}

static void raise_on(rtems_vector_number vector, uint32_t cpu_index)
{
	rtems_vector_number ipi_index = vector - QORIQ_IRQ_IPI_BASE;
	qoriq.pic.ipidr[ipi_index].reg = 1U << cpu_index;
}

rtems_status_code bsp_interrupt_raise(rtems_vector_number vector)
{
	bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

	if (pic_is_ipi(vector)) {
		raise_on(vector, rtems_scheduler_get_processor());
		return RTEMS_SUCCESSFUL;
	}

	return RTEMS_UNSATISFIED;
}

#if defined(RTEMS_SMP)
rtems_status_code bsp_interrupt_raise_on(
	rtems_vector_number vector,
	uint32_t cpu_index
)
{
	bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

	if (pic_is_ipi(vector)) {
		raise_on(vector, cpu_index);
		return RTEMS_SUCCESSFUL;
	}

	return RTEMS_UNSATISFIED;
}
#endif

rtems_status_code bsp_interrupt_clear(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_is_enabled(
	rtems_vector_number vector,
	bool *enabled
)
{
	volatile qoriq_pic_src_cfg *src_cfg;

	bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
	bsp_interrupt_assert(enabled != NULL);

	if (QORIQ_IRQ_IS_MSI(vector)) {
		vector = QORIQ_IRQ_MSI_0 + QORIQ_IRQ_MSI_INDEX(vector) / 32;
	}

	src_cfg = get_src_cfg(vector);
	*enabled = (src_cfg->vpr & VPR_MSK) == 0;
	return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
	return pic_vector_set_mask(vector, 0);
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
	return pic_vector_set_mask(vector, VPR_MSK);
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

void bsp_interrupt_facility_initialize(void)
{
	rtems_vector_number i = 0;
	uint32_t processor_id = ppc_processor_id();

	if (processor_id == 0) {
		/* Core 0 must do the basic initialization */

		pic_reset();

		for (i = 0; i < QORIQ_INTERRUPT_SOURCE_COUNT; ++i) {
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

		/* Clear shared message signaled interrupts */
		for (i = 0; i < RTEMS_ARRAY_SIZE(qoriq.pic.msir); ++i) {
			(void) qoriq.pic.msir[i].reg;
		}

		pic_global_timer_init();
	}

	qoriq.pic.ctpr = 0;

	for (i = 0; i < QORIQ_INTERRUPT_SOURCE_COUNT; ++i) {
		qoriq.pic.iack;
		qoriq.pic.eoi = 0;
		qoriq.pic.whoami;
	}
}

typedef __BITSET_DEFINE(pic_msi_bitset, QORIQ_IRQ_MSI_COUNT) pic_msi_bitset;

static pic_msi_bitset pic_msi_available =
  __BITSET_T_INITIALIZER(__BITSET_FSET(__bitset_words(QORIQ_IRQ_MSI_COUNT)));


static uint32_t pic_msi_bitset_to_uint32_t(
	const pic_msi_bitset *bitset,
	uint32_t              index
)
{
	long bits = bitset->__bits[index / _BITSET_BITS];

	return (uint32_t) (bits >> (32 * ((index % _BITSET_BITS) / 32)));
}

static void pic_msi_dispatch(void *arg)
{
	uintptr_t reg = (uintptr_t) arg;
	uint32_t msir = qoriq.pic.msir[reg].reg;

	while (msir != 0) {
		uint32_t index = 31 - __builtin_clz(msir);
		const rtems_interrupt_entry *entry;

		msir &= ~(UINT32_C(1) << index);
		entry = bsp_interrupt_entry_load_first(
			QORIQ_IRQ_MSI_VECTOR(32 * reg + index)
		);

		if (entry != NULL) {
			bsp_interrupt_dispatch_entries(entry);
		}
	}
}

static rtems_status_code pic_msi_allocate(rtems_vector_number *vector)
{
	pic_msi_bitset *available = &pic_msi_available;
	long found = __BIT_FFS(QORIQ_IRQ_MSI_COUNT, available);
	rtems_vector_number index;
	uint32_t subset;

	if (found == 0) {
		return RTEMS_UNSATISFIED;
	}

	index = (rtems_vector_number) found - 1;
	subset = pic_msi_bitset_to_uint32_t(available, index);

	if (subset == 0xffffffff) {
		uintptr_t reg = index / 32;
		rtems_status_code sc;

		sc = rtems_interrupt_handler_install(
			QORIQ_IRQ_MSI_0 + reg,
			"MSI",
			RTEMS_INTERRUPT_UNIQUE,
			pic_msi_dispatch,
			(void *) reg
		);

		if (sc != RTEMS_SUCCESSFUL) {
			return sc;
		}
	}

	__BIT_CLR(QORIQ_IRQ_MSI_COUNT, index, available);
	*vector = QORIQ_IRQ_MSI_VECTOR(index);
	return RTEMS_SUCCESSFUL;
}

static rtems_status_code pic_msi_free(rtems_vector_number vector)
{
	pic_msi_bitset *available = &pic_msi_available;
	rtems_vector_number index = QORIQ_IRQ_MSI_INDEX(vector);
	uint32_t subset;

	if (__BIT_ISSET(QORIQ_IRQ_MSI_COUNT, index, available)) {
		return RTEMS_NOT_DEFINED;
	}

	__BIT_SET(QORIQ_IRQ_MSI_COUNT, index, available);
	subset = pic_msi_bitset_to_uint32_t(available, index);

	if (subset == 0xffffffff) {
		uintptr_t reg = index / 32;

		return rtems_interrupt_handler_remove(
			QORIQ_IRQ_MSI_0 + reg,
			pic_msi_dispatch,
			(void *) reg
		);
	}

	return RTEMS_SUCCESSFUL;
}

rtems_status_code qoriq_pic_msi_allocate(rtems_vector_number *vector)
{
	rtems_status_code sc;

	if (!bsp_interrupt_is_initialized()) {
		return RTEMS_INCORRECT_STATE;
	}

	if (vector == NULL) {
		return RTEMS_INVALID_ADDRESS;
	}

	if (rtems_interrupt_is_in_progress()) {
		return RTEMS_CALLED_FROM_ISR;
	}

	bsp_interrupt_lock();
	sc = pic_msi_allocate(vector);
	bsp_interrupt_unlock();
	return sc;
}

rtems_status_code qoriq_pic_msi_free(rtems_vector_number vector)
{
	rtems_status_code sc;

	if (!bsp_interrupt_is_initialized()) {
		return RTEMS_INCORRECT_STATE;
	}

	if (!QORIQ_IRQ_IS_MSI(vector) ) {
		return RTEMS_INVALID_ID;
	}

	if (rtems_interrupt_is_in_progress()) {
		return RTEMS_CALLED_FROM_ISR;
	}

	bsp_interrupt_lock();
	sc = pic_msi_free(vector);
	bsp_interrupt_unlock();
	return sc;
}

rtems_status_code qoriq_pic_msi_map(
	rtems_vector_number vector,
	uint64_t *addr,
	uint32_t *data
)
{
	if (addr == NULL) {
		return RTEMS_INVALID_ADDRESS;
	}

	if (data == NULL) {
		return RTEMS_INVALID_ADDRESS;
	}

	if (!QORIQ_IRQ_IS_MSI(vector) ) {
		return RTEMS_INVALID_ID;
	}

	*addr = (uint64_t)(uintptr_t) &qoriq.pic.msiir;
	*data = QORIQ_IRQ_MSI_INDEX(vector) << 24;
	return RTEMS_SUCCESSFUL;
}

#endif /* QORIQ_IS_HYPERVISOR_GUEST */
