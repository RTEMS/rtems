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

#include <bsp/mmu.h>
#include <libcpu/powerpc-utility.h>

#define TEXT __attribute__((section(".bsp_start_text")))

static uint32_t TEXT power_of_two(uint32_t val)
{
	uint32_t test_power = QORIQ_MMU_MIN_POWER;
	uint32_t power = test_power;
	uint32_t alignment = 1U << test_power;

	while (test_power <= QORIQ_MMU_MAX_POWER && (val & (alignment - 1)) == 0) {
		power = test_power;
		alignment <<= QORIQ_MMU_POWER_STEP;
		test_power += QORIQ_MMU_POWER_STEP;
	}

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

static bool TEXT mas_equal(const qoriq_mmu_entry *a, const qoriq_mmu_entry *b)
{
	return a->mas1 == b->mas1 && a->mas2 == b->mas2 && a->mas3 == b->mas3;
}

static bool TEXT can_merge(const qoriq_mmu_entry *prev, const qoriq_mmu_entry *cur)
{
	bool can = false;

	if (prev->begin == cur->begin || prev->last >= cur->begin - 1) {
		/*
		 * Here we can technically merge.  We need a heuristic to
		 * prevent merges in case the MAS values differ and the boarder
		 * is reasonably well aligned.
		 */
		if (
			mas_equal(prev, cur)
				|| prev->last != cur->begin - 1
				|| power_of_two(cur->begin) < 24
		) {
			can = true;
		}
	}

	return can;
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

static void TEXT align(qoriq_mmu_context *self, uint32_t alignment)
{
	qoriq_mmu_entry *entries = self->entries;
	int n = self->count;
	int i = 0;

	for (i = 0; i < n; ++i) {
		qoriq_mmu_entry *cur = &entries [i];
		cur->begin &= ~(alignment - 1);
		cur->last = alignment + (cur->last & ~(alignment - 1)) - 1;
	}
}

static bool TEXT is_full(qoriq_mmu_context *self)
{
	return self->count >= QORIQ_MMU_ENTRY_COUNT;
}

static void TEXT append(qoriq_mmu_context *self, const qoriq_mmu_entry *new_entry)
{
	self->entries [self->count] = *new_entry;
	++self->count;
}

bool TEXT qoriq_mmu_add(
	qoriq_mmu_context *self,
	uint32_t begin,
	uint32_t last,
	uint32_t mas1,
	uint32_t mas2,
	uint32_t mas3
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
				.mas3 = mas3
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

static uint32_t TEXT min(uint32_t a, uint32_t b)
{
	return a < b ? a : b;
}

static bool TEXT split(qoriq_mmu_context *self, qoriq_mmu_entry *cur)
{
	bool again = false;
	uint32_t begin = cur->begin;
	uint32_t end = cur->last + 1;
	uint32_t size = end - begin;
	uint32_t begin_power = power_of_two(begin);
	uint32_t end_power = power_of_two(end);
	uint32_t size_power = power_of_two(size);
	uint32_t power = min(begin_power, min(end_power, size_power));
	uint32_t split_size = power < 32 ? (1U << power) : 0;
	uint32_t split_pos = begin + split_size;

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
	uint32_t alignment = 4096;

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
		uint32_t ea = cur->begin;
		uint32_t size = cur->last - ea + 1;
		uint32_t tsize = (power_of_two(size) - 10) / 2;
		int tlb = first_tlb + i;

		qoriq_tlb1_write(tlb, cur->mas1, cur->mas2, cur->mas3, ea, tsize);
	}
}

void qoriq_mmu_change_perm(uint32_t test, uint32_t set, uint32_t clear)
{
	int i = 0;

	for (i = 0; i < 16; ++i) {
		int mas0 = FSL_EIS_MAS0_TLBSEL | FSL_EIS_MAS0_ESEL(i);
		int mas1 = 0;

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
				asm volatile ("tlbwe; msync; isync" : : : "memory");
			}
		}
	}
}
