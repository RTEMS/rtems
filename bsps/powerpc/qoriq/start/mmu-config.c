/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQMMU
 *
 * @brief MMU implementation.
 */

/*
 * Copyright (c) 2011, 2018 embedded brains GmbH.  All rights reserved.
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
#include <rtems/sysinit.h>

#define TEXT __attribute__((section(".bsp_start_text")))
#define DATA __attribute__((section(".bsp_start_data")))

typedef struct {
	uintptr_t begin;
	uintptr_t size;
	uint32_t mas2;
	uint32_t mas3;
	uint32_t mas7;
} entry;

#define ENTRY_X(b, s) { \
	.begin = (uintptr_t) b, \
	.size = (uintptr_t) s, \
	.mas2 = 0, \
	.mas3 = FSL_EIS_MAS3_SX \
}

#define ENTRY_R(b, s) { \
	.begin = (uintptr_t) b, \
	.size = (uintptr_t) s, \
	.mas2 = 0, \
	.mas3 = FSL_EIS_MAS3_SR \
}

#ifdef RTEMS_SMP
  #define ENTRY_RW_MAS2 FSL_EIS_MAS2_M
#else
  #define ENTRY_RW_MAS2 0
#endif

#define ENTRY_RW(b, s) { \
	.begin = (uintptr_t) b, \
	.size = (uintptr_t) s, \
	.mas2 = ENTRY_RW_MAS2, \
	.mas3 = FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SW \
}

#define ENTRY_IO(b, s) { \
	.begin = (uintptr_t) b, \
	.size = (uintptr_t) s, \
	.mas2 = FSL_EIS_MAS2_I | FSL_EIS_MAS2_G, \
	.mas3 = FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SW \
}

#define ENTRY_DEV(b, s) { \
	.begin = (uintptr_t) b, \
	.size = (uintptr_t) s, \
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
	.begin = (uintptr_t) b, \
	.size = (uintptr_t) s, \
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
	ENTRY_RW(bsp_section_rtemsstack_begin, bsp_section_rtemsstack_size),
	ENTRY_RW(bsp_section_stack_begin, bsp_section_stack_size),
	ENTRY_IO(bsp_section_nocache_begin, bsp_section_nocache_size),
	ENTRY_IO(bsp_section_nocachenoload_begin, bsp_section_nocachenoload_size),
#ifndef QORIQ_IS_HYPERVISOR_GUEST
#if QORIQ_CHIP_IS_T_VARIANT(QORIQ_CHIP_VARIANT)
	/* BMan Portals */
	ENTRY_DEV_CACHED(&qoriq_bman_portal[0][0], sizeof(qoriq_bman_portal[0])),
	ENTRY_DEV(&qoriq_bman_portal[1][0], sizeof(qoriq_bman_portal[1])),
	/* QMan Portals */
	ENTRY_DEV_CACHED(&qoriq_qman_portal[0][0], sizeof(qoriq_qman_portal[0])),
	ENTRY_DEV(&qoriq_qman_portal[1][0], sizeof(qoriq_qman_portal[1])),
#endif
	ENTRY_DEV(&qoriq, sizeof(qoriq))
#endif
};

static DATA char memory_path[] = "/memory";

#ifdef QORIQ_IS_HYPERVISOR_GUEST
static void TEXT add_dpaa_bqman_portals(
	qoriq_mmu_context *context,
	const void *fdt,
	const char *compatible
)
{
	int node;

	node = -1;

	while (true) {
		const void *val;
		int len;
		uintptr_t paddr;
		uintptr_t size;

		node = fdt_node_offset_by_compatible(fdt, node, compatible);
		if (node < 0) {
			break;
		}

		val = fdt_getprop(fdt, node, "reg", &len);
		if (len != 32) {
			continue;
		}

		paddr = (uintptr_t) fdt64_to_cpu(((fdt64_t *) val)[0]);
		size = (uintptr_t) fdt64_to_cpu(((fdt64_t *) val)[1]);

		qoriq_mmu_add(
			context,
			paddr,
			paddr + size - 1,
			0,
			FSL_EIS_MAS2_M | FSL_EIS_MAS2_G,
			FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SW,
			QORIQ_MMU_DEVICE_MAS7
		);

		paddr = (uintptr_t) fdt64_to_cpu(((fdt64_t *) val)[2]);
		size = (uintptr_t) fdt64_to_cpu(((fdt64_t *) val)[3]);

		qoriq_mmu_add(
			context,
			paddr,
			paddr + size - 1,
			0,
			FSL_EIS_MAS2_I | FSL_EIS_MAS2_G,
			FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SW,
			QORIQ_MMU_DEVICE_MAS7
		);
	}
}

static void TEXT add_dpaa_bpool(qoriq_mmu_context *context, const void *fdt)
{
	int node;

	node = -1;

	while (true) {
		const void *val;
		int len;
		uintptr_t config_count;
		uintptr_t size;
		uintptr_t paddr;

		node = fdt_node_offset_by_compatible(fdt, node, "fsl,bpool");
		if (node < 0) {
			break;
		}

		val = fdt_getprop(fdt, node, "fsl,bpool-ethernet-cfg", &len);
		if (len != 24) {
			continue;
		}

		config_count = (uintptr_t) fdt64_to_cpu(((fdt64_t *) val)[0]);
		size = (uintptr_t) fdt64_to_cpu(((fdt64_t *) val)[1]);
		paddr = (uintptr_t) fdt64_to_cpu(((fdt64_t *) val)[2]);

		qoriq_mmu_add(
			context,
			paddr,
			paddr + config_count * size - 1,
			0,
			FSL_EIS_MAS2_M,
			FSL_EIS_MAS3_SR | FSL_EIS_MAS3_SW,
			0
		);
	}
}
#endif

static void TEXT config_fdt_adjust(const void *fdt)
{
	int node;

	node = fdt_path_offset_namelen(
		fdt,
		memory_path,
		(int) sizeof(memory_path) - 1
	);

	if (node >= 0) {
		int len;
		const void *val;
		uint64_t mem_begin;
		uint64_t mem_size;

		val = fdt_getprop(fdt, node, "reg", &len);
		if (len == 8) {
			mem_begin = fdt32_to_cpu(((fdt32_t *) val)[0]);
			mem_size = fdt32_to_cpu(((fdt32_t *) val)[1]);
		} else if (len == 16) {
			mem_begin = fdt64_to_cpu(((fdt64_t *) val)[0]);
			mem_size = fdt64_to_cpu(((fdt64_t *) val)[1]);
		} else {
			mem_begin = 0;
			mem_size = 0;
		}

#ifndef __powerpc64__
		mem_size = MIN(mem_size, 0x80000000U);
#endif

		if (
			mem_begin == 0
				&& mem_size > (uintptr_t) bsp_section_work_end
				&& (uintptr_t) bsp_section_nocache_end
					< (uintptr_t) bsp_section_work_end
		) {
			/* Assign new value to allow a bsp_restart() */
			config[WORKSPACE_ENTRY_INDEX].size = (uintptr_t) mem_size
				- (uintptr_t) bsp_section_work_begin;
		}
	}
}

void TEXT qoriq_mmu_config(bool boot_processor, int first_tlb, int scratch_tlb)
{
	qoriq_mmu_context context;
	const void *fdt;
	int max_count;
	int i;

	for (i = 0; i < QORIQ_TLB1_ENTRY_COUNT; ++i) {
		if (i != scratch_tlb) {
			qoriq_tlb1_invalidate(i);
		}
	}

	fdt = bsp_fdt_get();
	qoriq_mmu_context_init(&context);

#ifdef QORIQ_IS_HYPERVISOR_GUEST
	add_dpaa_bqman_portals(&context, fdt, "fsl,bman-portal");
	add_dpaa_bqman_portals(&context, fdt, "fsl,qman-portal");
	add_dpaa_bpool(&context, fdt);
	max_count = QORIQ_TLB1_ENTRY_COUNT - 1;
#else
	max_count = (3 * QORIQ_TLB1_ENTRY_COUNT) / 4;
#endif

	if (boot_processor) {
		config_fdt_adjust(fdt);
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

	qoriq_mmu_partition(&context, max_count);
	qoriq_mmu_write_to_tlb1(&context, first_tlb);
}

static Memory_Area _Memory_Areas[1];

static const Memory_Information _Memory_Information =
	MEMORY_INFORMATION_INITIALIZER(_Memory_Areas);

static void bsp_memory_initialize(void)
{
	const entry *we = &config[WORKSPACE_ENTRY_INDEX];

	_Memory_Initialize_by_size(
		&_Memory_Areas[0],
		(void *) we->begin,
		we->size
	);
}

RTEMS_SYSINIT_ITEM(
	bsp_memory_initialize,
	RTEMS_SYSINIT_MEMORY,
	RTEMS_SYSINIT_ORDER_MIDDLE
);

const Memory_Information *_Memory_Get(void)
{
	return &_Memory_Information;
}
