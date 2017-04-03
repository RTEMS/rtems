/**
 * @file
 *
 * @ingroup QorIQMMU
 *
 * @brief MMU implementation.
 */

/*
 * Copyright (c) 2011, 2017 embedded brains GmbH.  All rights reserved.
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

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/fdt.h>
#include <bsp/linker-symbols.h>
#include <bsp/mmu.h>
#include <bsp/qoriq.h>

#include <sys/param.h>

#include <libfdt.h>

#include <rtems/config.h>

#define TEXT __attribute__((section(".bsp_start_text")))
#define DATA __attribute__((section(".bsp_start_data")))

typedef struct {
	uint32_t begin;
	uint32_t size;
	uint32_t mas2;
	uint32_t mas3;
	uint32_t mas7;
} entry;

#define ENTRY_X(b, s) { \
	.begin = (uint32_t) b, \
	.size = (uint32_t) s, \
	.mas2 = 0, \
	.mas3 = FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SX \
}

#define ENTRY_R(b, s) { \
	.begin = (uint32_t) b, \
	.size = (uint32_t) s, \
	.mas2 = 0, \
	.mas3 = FSL_EIS_MAS3_SR \
}

#ifdef RTEMS_SMP
  #define ENTRY_RW_MAS2 FSL_EIS_MAS2_M
#else
  #define ENTRY_RW_MAS2 0
#endif

#define ENTRY_RW(b, s) { \
	.begin = (uint32_t) b, \
	.size = (uint32_t) s, \
	.mas2 = ENTRY_RW_MAS2, \
	.mas3 = FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SW \
}

#define ENTRY_IO(b, s) { \
	.begin = (uint32_t) b, \
	.size = (uint32_t) s, \
	.mas2 = FSL_EIS_MAS2_I | FSL_EIS_MAS2_G, \
	.mas3 = FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SW \
}

#define ENTRY_DEV(b, s) { \
	.begin = (uint32_t) b, \
	.size = (uint32_t) s, \
	.mas2 = FSL_EIS_MAS2_I | FSL_EIS_MAS2_G, \
	.mas3 = FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SW, \
	.mas7 = QORIQ_MMU_DEVICE_MAS7 \
}

/*
 * MMU entry for BMan and QMan software portals.
 *
 * The M bit must be set if stashing is used, see 3.3.8.6 DQRR Entry Stashing
 * and 3.3.8 Software Portals in T4240DPAARM.
 *
 * The G bit must be set, otherwise ECC errors in the QMan software portals
 * will occur.  No documentation reference for this is available.
 */
#define ENTRY_DEV_CACHED(b, s) { \
	.begin = (uint32_t) b, \
	.size = (uint32_t) s, \
	.mas2 = FSL_EIS_MAS2_M | FSL_EIS_MAS2_G, \
	.mas3 = FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SW, \
	.mas7 = QORIQ_MMU_DEVICE_MAS7 \
}

#define WORKSPACE_ENTRY_INDEX 0

static entry DATA config[] = {
	/* Must be first entry, see WORKSPACE_ENTRY_INDEX */
	ENTRY_RW(bsp_section_work_begin, bsp_section_work_size),

	#if defined(RTEMS_MULTIPROCESSING) && \
	    defined(QORIQ_INTERCOM_AREA_BEGIN) && \
	    defined(QORIQ_INTERCOM_AREA_SIZE)
		{
			.begin = QORIQ_INTERCOM_AREA_BEGIN,
			.size = QORIQ_INTERCOM_AREA_SIZE,
			.mas2 = FSL_EIS_MAS2_M,
			.mas3 = FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SW
		},
	#endif
	ENTRY_X(bsp_section_start_begin, bsp_section_start_size),
	ENTRY_R(bsp_section_fast_text_load_begin, bsp_section_fast_text_size),
	ENTRY_X(bsp_section_fast_text_begin, bsp_section_fast_text_size),
	ENTRY_X(bsp_section_text_begin, bsp_section_text_size),
	ENTRY_R(bsp_section_rodata_load_begin, bsp_section_rodata_size),
	ENTRY_R(bsp_section_rodata_begin, bsp_section_rodata_size),
	ENTRY_R(bsp_section_fast_data_load_begin, bsp_section_fast_data_size),
	ENTRY_RW(bsp_section_fast_data_begin, bsp_section_fast_data_size),
	ENTRY_R(bsp_section_data_load_begin, bsp_section_data_size),
	ENTRY_RW(bsp_section_data_begin, bsp_section_data_size),
	ENTRY_RW(bsp_section_sbss_begin, bsp_section_sbss_size),
	ENTRY_RW(bsp_section_bss_begin, bsp_section_bss_size),
	ENTRY_RW(bsp_section_rwextra_begin, bsp_section_rwextra_size),
	ENTRY_RW(bsp_section_stack_begin, bsp_section_stack_size),
	ENTRY_IO(bsp_section_nocache_begin, bsp_section_nocache_size),
	ENTRY_IO(bsp_section_nocachenoload_begin, bsp_section_nocachenoload_size),
#if QORIQ_CHIP_IS_T_VARIANT(QORIQ_CHIP_VARIANT)
	/* BMan Portals */
	ENTRY_DEV_CACHED(&qoriq_bman_portal[0][0], sizeof(qoriq_bman_portal[0])),
	ENTRY_DEV(&qoriq_bman_portal[1][0], sizeof(qoriq_bman_portal[1])),
	/* QMan Portals */
	ENTRY_DEV_CACHED(&qoriq_qman_portal[0][0], sizeof(qoriq_qman_portal[0])),
	ENTRY_DEV(&qoriq_qman_portal[1][0], sizeof(qoriq_qman_portal[1])),
#endif
	ENTRY_DEV(&qoriq, sizeof(qoriq))
};

static DATA char memory_path[] = "/memory";

static void TEXT config_fdt_adjust(void)
{
	const void *fdt = bsp_fdt_get();
	int node;

	node = fdt_path_offset_namelen(
		fdt,
		memory_path,
		(int) sizeof(memory_path) - 1
	);

	if (node >= 0) {
		int len;
		const void *val;
		uint64_t begin;
		uint64_t size;

		val = fdt_getprop(fdt, node, "reg", &len);
		if (len == 8) {
			begin = fdt32_to_cpu(((fdt32_t *) val)[0]);
			size = fdt32_to_cpu(((fdt32_t *) val)[1]);
		} else if (len == 16) {
			begin = fdt64_to_cpu(((fdt64_t *) val)[0]);
			size = fdt64_to_cpu(((fdt64_t *) val)[1]);
		} else {
			begin = 0;
			size = 0;
		}

		size = MIN(size, 0x80000000U);

		if (
			begin == 0
				&& size > (uintptr_t) bsp_section_work_end
				&& (uintptr_t) bsp_section_nocache_end
					< (uintptr_t) bsp_section_work_end
		) {
			config[WORKSPACE_ENTRY_INDEX].size += (uintptr_t) size
				- (uintptr_t) bsp_section_work_end;
		}
	}
}

void TEXT qoriq_mmu_config(bool boot_processor, int first_tlb, int scratch_tlb)
{
	qoriq_mmu_context context;
	int i = 0;

	if (boot_processor) {
		config_fdt_adjust();
	}

	qoriq_mmu_context_init(&context);

	for (i = 0; i < QORIQ_TLB1_ENTRY_COUNT; ++i) {
		if (i != scratch_tlb) {
			qoriq_tlb1_invalidate(i);
		}
	}

	for (i = 0; i < (int) (sizeof(config) / sizeof(config [0])); ++i) {
		const entry *cur = &config [i];
		if (cur->size > 0) {
			qoriq_mmu_add(
				&context,
				cur->begin,
				cur->begin + cur->size - 1,
				0,
				cur->mas2,
				cur->mas3,
				cur->mas7
			);
		}
	}

	qoriq_mmu_partition(&context, (3 * QORIQ_TLB1_ENTRY_COUNT) / 4);
	qoriq_mmu_write_to_tlb1(&context, first_tlb);
}

void TEXT bsp_work_area_initialize(void)
{
	const entry *we = &config[WORKSPACE_ENTRY_INDEX];
	uintptr_t begin = we->begin;
	uintptr_t end = begin + we->size;

#ifdef BSP_INTERRUPT_STACK_AT_WORK_AREA_BEGIN
	begin += rtems_configuration_get_interrupt_stack_size();
#endif

	bsp_work_area_initialize_default((void *) begin, end - begin);
}
