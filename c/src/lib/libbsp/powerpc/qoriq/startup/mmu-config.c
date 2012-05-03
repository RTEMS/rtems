/**
 * @file
 *
 * @ingroup QorIQMMU
 *
 * @brief MMU implementation.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
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

#include <bspopts.h>
#include <bsp/mmu.h>
#include <bsp/linker-symbols.h>
#include <bsp/qoriq.h>

#define TEXT __attribute__((section(".bsp_start_text")))
#define DATA __attribute__((section(".bsp_start_data")))

typedef struct {
	uint32_t begin;
	uint32_t size;
	uint32_t mas2;
	uint32_t mas3;
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

#define ENTRY_RW(b, s) { \
	.begin = (uint32_t) b, \
	.size = (uint32_t) s, \
	.mas2 = 0, \
	.mas3 = FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SW \
}

#define ENTRY_DEV(b, s) { \
	.begin = (uint32_t) b, \
	.size = (uint32_t) s, \
	.mas2 = FSL_EIS_MAS2_I | FSL_EIS_MAS2_G, \
	.mas3 = FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SW \
}

static const entry DATA config [] = {
	#if defined(QORIQ_INTERCOM_AREA_BEGIN) && defined(QORIQ_INTERCOM_AREA_SIZE)
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
	ENTRY_RW(bsp_section_work_begin, bsp_section_work_size),
	ENTRY_RW(bsp_section_stack_begin, bsp_section_stack_size),
	ENTRY_DEV(&qoriq, sizeof(qoriq))
};

void TEXT qoriq_mmu_config(int first_tlb, int scratch_tlb)
{
	qoriq_mmu_context context;
	int i = 0;

	qoriq_mmu_context_init(&context);

	for (i = 0; i < 16; ++i) {
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
				cur->mas3
			);
		}
	}

	qoriq_mmu_partition(&context, 8);
	qoriq_mmu_write_to_tlb1(&context, first_tlb);
}
