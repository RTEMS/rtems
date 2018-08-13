/*
 * Copyright (c) 2018.
 * Amaan Cheval <amaan.cheval@gmail.com>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _LIBCPU_AMD64_PAGE_H
#define _LIBCPU_AMD64_PAGE_H

#ifndef ASM

#define NUM_PAGE_TABLE_ENTRIES 512

extern uint64_t amd64_pml4[NUM_PAGE_TABLE_ENTRIES];
extern uint64_t amd64_pdpt[NUM_PAGE_TABLE_ENTRIES];

bool paging_1gib_pages_supported(void);
uint8_t get_maxphysaddr(void);
uint64_t get_mask_for_bits(uint8_t start, uint8_t end);
uint64_t create_cr3_entry(
  uint64_t phys_addr, uint8_t maxphysaddr, uint64_t flags
);
uint64_t create_pml4_entry(
  uint64_t phys_addr, uint8_t maxphysaddr, uint64_t flags
);
uint64_t create_pdpt_entry(
  uint64_t phys_addr, uint8_t maxphysaddr, uint64_t flags
);

void paging_init(void);

#define PAGE_FLAGS_PRESENT          (1 << 0)
#define PAGE_FLAGS_WRITABLE         (1 << 1)
#define PAGE_FLAGS_USER_ACCESSIBLE  (1 << 2)
#define PAGE_FLAGS_WRITE_THROUGH    (1 << 3)
#define PAGE_FLAGS_NO_CACHE         (1 << 4)
#define PAGE_FLAGS_ACCESSED         (1 << 5)
#define PAGE_FLAGS_DIRTY            (1 << 6)
#define PAGE_FLAGS_HUGE_PAGE        (1 << 7)
#define PAGE_FLAGS_GLOBAL           (1 << 8)
#define PAGE_FLAGS_NO_EXECUTE       (1 << 63)

#define PAGE_FLAGS_DEFAULTS                                              \
  (PAGE_FLAGS_PRESENT | PAGE_FLAGS_WRITABLE | PAGE_FLAGS_USER_ACCESSIBLE \
   | PAGE_FLAGS_WRITE_THROUGH | PAGE_FLAGS_NO_CACHE | PAGE_FLAGS_GLOBAL)

#endif /* !ASM */
#endif
