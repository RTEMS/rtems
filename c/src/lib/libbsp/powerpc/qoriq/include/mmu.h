/**
 * @file
 *
 * @ingroup QorIQMMU
 *
 * @brief MMU API.
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

#ifndef LIBBSP_POWERPC_QORIQ_MMU_H
#define LIBBSP_POWERPC_QORIQ_MMU_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup QorIQMMU QorIQ - MMU Support
 *
 * @ingroup QorIQ
 *
 * @brief MMU support.
 *
 * @{
 */

#define QORIQ_MMU_ENTRY_COUNT 32

#define QORIQ_MMU_MIN_POWER 12
#define QORIQ_MMU_MAX_POWER 32
#define QORIQ_MMU_POWER_STEP 2

typedef struct {
	uint32_t begin;
	uint32_t last;
	uint32_t mas1;
	uint32_t mas2;
	uint32_t mas3;
} qoriq_mmu_entry;

typedef struct {
	int count;
	qoriq_mmu_entry entries [QORIQ_MMU_ENTRY_COUNT];
} qoriq_mmu_context;

void qoriq_mmu_context_init(qoriq_mmu_context *self);

bool qoriq_mmu_add(
	qoriq_mmu_context *self,
	uint32_t begin,
	uint32_t last,
	uint32_t mas1,
	uint32_t mas2,
	uint32_t mas3
);

void qoriq_mmu_partition(qoriq_mmu_context *self, int max_count);

void qoriq_mmu_write_to_tlb1(qoriq_mmu_context *self, int first_tlb);

void qoriq_mmu_change_perm(uint32_t test, uint32_t set, uint32_t clear);

void qoriq_mmu_config(int first_tlb, int scratch_tlb);

void qoriq_tlb1_write(
	int esel,
	uint32_t mas1,
	uint32_t mas2,
	uint32_t mas3,
	uint32_t ea,
	uint32_t tsize
);

void qoriq_tlb1_invalidate(int esel);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_POWERPC_QORIQ_MMU_H */
