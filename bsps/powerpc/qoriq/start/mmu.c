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

#include <bsp/mmu.h>
#include <libcpu/powerpc-utility.h>

#define TEXT __attribute__((section(".bsp_start_text")))

static uintptr_t TEXT power_of_two(uintptr_t val)
{
	uintptr_t test_power = QORIQ_MMU_MIN_POWER;
	uintptr_t power = test_power;
	uintptr_t alignment = 1U << test_power;

	while (test_power <= QORIQ_MMU_MAX_POWER && (val & (alignment - 1)) == 0) {
		power = test_power;
		alignment <<= QORIQ_MMU_POWER_STEP;
		test_power += QORIQ_MMU_POWER_STEP;
	}

	return power;
}

static uintptr_t TEXT max_power_of_two(uintptr_t val)
{
	uintptr_t test_power = QORIQ_MMU_MIN_POWER;
	uintptr_t power = test_power;
	uintptr_t max = 1U << test_power;

	do {
		power = test_power;
		max <<= QORIQ_MMU_POWER_STEP;
		test_power += QORIQ_MMU_POWER_STEP;
	} while (test_power <= QORIQ_MMU_MAX_POWER && max <= val);

	return power;
}

void TEXT qoriq_mmu_context_init(qoriq_mmu_context *self)
{
	int *cur = (int *) self;
	const int *end = cur + sizeof(*self) / sizeof(*cur);

	while (cur != end) {
		*cur = 0;
		++cur;
	}
}

static void TEXT sort(qoriq_mmu_context *self)
{
	qoriq_mmu_entry *entries = self->entries;
	int n = self->count;
	int i = 0;

	for (i = 1; i < n; ++i) {
		qoriq_mmu_entry key = entries [i];
		int j = 0;

		for (j = i - 1; j >= 0 && entries [j].begin > key.begin; --j) {
			entries [j + 1] = entries [j];
		}

		entries [j + 1] = key;
	}
}

static bool TEXT mas_compatible(const qoriq_mmu_entry *a, const qoriq_mmu_entry *b)
{
	uint32_t m = FSL_EIS_MAS2_M;

	return (a->mas2 & ~m) == (b->mas2 & ~m);
}

static bool TEXT can_merge(const qoriq_mmu_entry *prev, const qoriq_mmu_entry *cur)
{
	return mas_compatible(prev, cur)
		&& (prev->begin == cur->begin || prev->last >= cur->begin - 1);
}

static void TEXT merge(qoriq_mmu_context *self)
{
	qoriq_mmu_entry *entries = self->entries;
	int n = self->count;
	int i = 0;

	for (i = 1; i < n; ++i) {
		qoriq_mmu_entry *prev = &entries [i - 1];
		qoriq_mmu_entry *cur = &entries [i];

		if (can_merge(prev, cur)) {
			int j = 0;

			prev->mas1 |= cur->mas1;
			prev->mas2 |= cur->mas2;
			prev->mas3 |= cur->mas3;

			if (cur->last > prev->last) {
				prev->last = cur->last;
			}

			for (j = i + 1; j < n; ++j) {
				entries [j - 1] = entries [j];
			}

			--i;
			--n;
		}
	}

	self->count = n;
}

static void TEXT compact(qoriq_mmu_context *self)
{
	sort(self);
	merge(self);
}

static bool TEXT can_expand_down(
	const qoriq_mmu_context *self,
	const qoriq_mmu_entry *cur,
	int i,
	uintptr_t new_begin
)
{
	int j;

	for (j = 0; j < i; ++j) {
		const qoriq_mmu_entry *before = &self->entries[j];

		if (
			before->begin <= new_begin
				&& new_begin <= before->last
				&& !mas_compatible(before, cur)
		) {
			return false;
		}
	}

	return true;
}

static bool TEXT can_expand_up(
	const qoriq_mmu_context *self,
	const qoriq_mmu_entry *cur,
	int i,
	int n,
	uintptr_t new_last
)
{
	int j;

	for (j = i + 1; j < n; ++j) {
		const qoriq_mmu_entry *after = &self->entries[j];

		if (
			after->begin <= new_last
				&& new_last <= after->last
				&& !mas_compatible(after, cur)
		) {
			return false;
		}
	}

	return true;
}

static void TEXT align(qoriq_mmu_context *self, uintptr_t alignment)
{
	int n = self->count;
	int i;

	for (i = 0; i < n; ++i) {
		qoriq_mmu_entry *cur = &self->entries[i];
		uintptr_t new_begin = cur->begin & ~(alignment - 1);
		uintptr_t new_last = alignment + (cur->last & ~(alignment - 1)) - 1;

		if (
			can_expand_down(self, cur, i, new_begin)
				&& can_expand_up(self, cur, i, n, new_last)
		) {
			cur->begin = new_begin;
			cur->last = new_last;
		}
	}
}

static bool TEXT is_full(qoriq_mmu_context *self)
{
	return self->count >= QORIQ_TLB1_ENTRY_COUNT;
}

static void TEXT append(qoriq_mmu_context *self, const qoriq_mmu_entry *new_entry)
{
	self->entries [self->count] = *new_entry;
	++self->count;
}

bool TEXT qoriq_mmu_add(
	qoriq_mmu_context *self,
	uintptr_t begin,
	uintptr_t last,
	uint32_t mas1,
	uint32_t mas2,
	uint32_t mas3,
	uint32_t mas7
)
{
	bool ok = true;

	if (is_full(self)) {
		compact(self);
	}

	if (!is_full(self)) {
		if (begin < last) {
			qoriq_mmu_entry new_entry = {
				.begin = begin,
				.last = last,
				.mas1 = mas1,
				.mas2 = mas2,
				.mas3 = mas3,
				.mas7 = mas7
			};
			append(self, &new_entry);
		} else {
			ok = false;
		}
	} else {
		ok = false;
	}

	return ok;
}

static uintptr_t TEXT min(uintptr_t a, uintptr_t b)
{
	return a < b ? a : b;
}

static bool TEXT split(qoriq_mmu_context *self, qoriq_mmu_entry *cur)
{
	bool again = false;
	uintptr_t begin = cur->begin;
	uintptr_t end = cur->last + 1;
	uintptr_t size = end - begin;
	uintptr_t begin_power = power_of_two(begin);
	uintptr_t size_power = max_power_of_two(size);
	uintptr_t power = min(begin_power, size_power);
	uintptr_t split_size = power < 32 ? (1U << power) : 0;
	uintptr_t split_pos = begin + split_size;

	if (split_pos != end && !is_full(self)) {
		qoriq_mmu_entry new_entry = *cur;
		cur->begin = split_pos;
		new_entry.last = split_pos - 1;
		append(self, &new_entry);
		again = true;
	}

	return again;
}

static void TEXT split_all(qoriq_mmu_context *self)
{
	qoriq_mmu_entry *entries = self->entries;
	int n = self->count;
	int i = 0;

	for (i = 0; i < n; ++i) {
		qoriq_mmu_entry *cur = &entries [i];

		while (split(self, cur)) {
			/* Repeat */
		}
	}
}

static TEXT void partition(qoriq_mmu_context *self)
{
	compact(self);
	split_all(self);
	sort(self);
}

void TEXT qoriq_mmu_partition(qoriq_mmu_context *self, int max_count)
{
	uintptr_t alignment = 4096;

	sort(self);

	do {
		align(self, alignment);
		partition(self);
		alignment *= 4;
	} while (self->count > max_count);
}

void TEXT qoriq_mmu_write_to_tlb1(qoriq_mmu_context *self, int first_tlb)
{
	qoriq_mmu_entry *entries = self->entries;
	int n = self->count;
	int i = 0;

	for (i = 0; i < n; ++i) {
		qoriq_mmu_entry *cur = &entries [i];
		uintptr_t ea = cur->begin;
		uintptr_t size = cur->last - ea + 1;
		uintptr_t tsize = (power_of_two(size) - 10) / 2;
		int tlb = first_tlb + i;

		qoriq_tlb1_write(
			tlb,
			cur->mas1,
			cur->mas2,
			cur->mas3,
			cur->mas7,
			ea,
			(int) tsize
		);
	}
}

void qoriq_mmu_change_perm(uint32_t test, uint32_t set, uint32_t clear)
{
	int i = 0;

	for (i = 0; i < 16; ++i) {
		uint32_t mas0 = FSL_EIS_MAS0_TLBSEL | FSL_EIS_MAS0_ESEL(i);
		uint32_t mas1 = 0;

		PPC_SET_SPECIAL_PURPOSE_REGISTER(FSL_EIS_MAS0, mas0);
		asm volatile ("tlbre");

		mas1 = PPC_SPECIAL_PURPOSE_REGISTER(FSL_EIS_MAS1);
		if ((mas1 & FSL_EIS_MAS1_V) != 0) {
			uint32_t mask = 0x3ff;
			uint32_t mas3 = PPC_SPECIAL_PURPOSE_REGISTER(FSL_EIS_MAS3);

			if ((mas3 & mask) == test) {
				mas3 &= ~(clear & mask);
				mas3 |= set & mask;
				PPC_SET_SPECIAL_PURPOSE_REGISTER(FSL_EIS_MAS3, mas3);
				asm volatile ("isync; msync; tlbwe; isync" : : : "memory");
			}
		}
	}
}
