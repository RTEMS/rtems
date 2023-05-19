/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQMMU
 *
 * @brief MMU API.
 */

/*
 * Copyright (C) 2011, 2015 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_POWERPC_QORIQ_MMU_H
#define LIBBSP_POWERPC_QORIQ_MMU_H

#include <stdint.h>
#include <stdbool.h>

#include <bspopts.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup QorIQMMU QorIQ - MMU Support
 *
 * @ingroup RTEMSBSPsPowerPCQorIQ
 *
 * @brief MMU support.
 *
 * @{
 */

#define QORIQ_MMU_MIN_POWER 12
#define QORIQ_MMU_MAX_POWER 30
#define QORIQ_MMU_POWER_STEP 2

typedef struct {
	uintptr_t begin;
	uintptr_t last;
	uint32_t mas1;
	uint32_t mas2;
	uint32_t mas3;
	uint32_t mas7;
} qoriq_mmu_entry;

typedef struct {
	int count;
	qoriq_mmu_entry entries [QORIQ_TLB1_ENTRY_COUNT];
} qoriq_mmu_context;

void qoriq_mmu_context_init(qoriq_mmu_context *self);

bool qoriq_mmu_add(
	qoriq_mmu_context *self,
	uintptr_t begin,
	uintptr_t last,
	uint32_t mas1,
	uint32_t mas2,
	uint32_t mas3,
	uint32_t mas7
);

void qoriq_mmu_partition(qoriq_mmu_context *self, int max_count);

void qoriq_mmu_write_to_tlb1(qoriq_mmu_context *self, int first_tlb);

void qoriq_mmu_change_perm(uint32_t test, uint32_t set, uint32_t clear);

int qoriq_mmu_find_free_tlb1_entry(void);

void qoriq_mmu_config(bool boot_processor, int first_tlb, int scratch_tlb);

void qoriq_tlb1_write(
	int esel,
	uint32_t mas1,
	uint32_t mas2,
	uint32_t mas3,
	uint32_t mas7,
	uintptr_t ea,
	int tsize
);

void qoriq_mmu_adjust_and_write_to_tlb1(
	int tlb,
	uintptr_t begin,
	uintptr_t last,
	uint32_t mas1,
	uint32_t mas2,
	uint32_t mas3,
	uint32_t mas7
);

void qoriq_tlb1_invalidate(int esel);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_POWERPC_QORIQ_MMU_H */
