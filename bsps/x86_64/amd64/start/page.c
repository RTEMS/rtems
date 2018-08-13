/*
 * This file sets up page sizes to 1GiB (i.e. huge pages, using only the PML4
 * and PDPT, skipping the PDT, and PT).
 * We set up identity-page mapping for the 512 GiBs addressable by using static
 * PML4 and PDPT tables.
 *
 * Section 4.5 "4-Level Paging" of Volume 3 of the Intel Software Developer
 * Manual guides a lot of the code used in this file.
 */

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

#include <stdio.h>
#include <assert.h>
#include <bsp.h>
#include <rtems.h>
#include <libcpu/page.h>
#include <rtems/score/cpu.h>

uint64_t amd64_pml4[NUM_PAGE_TABLE_ENTRIES] RTEMS_ALIGNED(4096);
uint64_t amd64_pdpt[NUM_PAGE_TABLE_ENTRIES] RTEMS_ALIGNED(4096);

bool paging_1gib_pages_supported(void)
{
  /*
   * If CPUID.80000001H:EDX.Page1GB [bit 26] = 1, 1-GByte pages are supported
   * with 4-level paging.
   */
  uint32_t a, b, c, d;
  cpuid(0x80000001, &a, &b, &c, &d);
  return (d >> 26) & 1;
}

uint8_t get_maxphysaddr(void)
{
  /*
   * CPUID.80000008H:EAX[15:8] reports the linear-address width supported by the
   * processor. Generally, this value is 48 if CPUID.80000001H:EDX.LM [bit 29] =
   * 1 and 32 otherwise.
   */
  uint32_t a, b, c, d;
  cpuid(0x80000008, &a, &b, &c, &d);

  uint8_t maxphysaddr = (a >> 8) & 0xff;
  /* This width is referred to as MAXPHYADDR. MAXPHYADDR is at most 52. */
  assert(maxphysaddr <= 52);

  return maxphysaddr;
}

uint64_t get_mask_for_bits(uint8_t start, uint8_t end)
{
  /*
   * Create a mask that lets you select bits start:end when logically ANDed with
   * a value. For eg.
   *   get_mask_for_bits(48, 64) = 0xffff000000000000
   */
  uint64_t mask = (((uint64_t) 1 << (end - start)) - 1) << start;
  return mask;
}

RTEMS_INLINE_ROUTINE void assert_0s_from_bit(uint64_t entry, uint8_t bit_pos)
{
  /* Confirm that bit_pos:64 are all 0s */
  assert((entry & get_mask_for_bits(bit_pos, 64)) == 0);
}

uint64_t create_cr3_entry(
  uint64_t phys_addr, uint8_t maxphysaddr, uint64_t flags
)
{
  /* Confirm PML4 address is aligned on a 4KiB boundary */
  assert((phys_addr & 0xfff) == 0);
  uint64_t entry = (phys_addr & get_mask_for_bits(12, maxphysaddr)) | flags;

  /* Confirm that bits maxphysaddr:64 are 0s */
  assert_0s_from_bit(entry, maxphysaddr);
  return entry;
}

uint64_t create_pml4_entry(
  uint64_t phys_addr, uint8_t maxphysaddr, uint64_t flags
)
{
  /* Confirm address we're writing is aligned on a 4KiB boundary */
  assert((phys_addr & 0xfff) == 0);
  uint64_t entry = (phys_addr & get_mask_for_bits(12, maxphysaddr)) | flags;

  /*
   * Confirm that bits maxphysaddr:64 are 0s; there are other usable bits there
   * such as PAGE_FLAGS_NO_EXECUTE, but we're asserting that those aren't set
   * either.
   */
  assert_0s_from_bit(entry, maxphysaddr);
  return entry;
}

uint64_t create_pdpt_entry(
  uint64_t phys_addr, uint8_t maxphysaddr, uint64_t flags
)
{
  /* Confirm physical address is a 1GiB aligned page address */
  assert((phys_addr & 0x3fffffff) == 0);
  uint64_t entry = (phys_addr & get_mask_for_bits(30, maxphysaddr)) | flags;

  /*
   * Confirm that bits maxphysaddr:64 are 0s; there are other usable bits there
   * such as the protection key and PAGE_FLAGS_NO_EXECUTE, but we're asserting
   * that those aren't set either.
   */
  assert_0s_from_bit(entry, maxphysaddr);
  return entry;
}

void paging_init(void)
{
  if ( !paging_1gib_pages_supported() ) {
    printf("warning: 1 GiB pages aren't supported - trying anyway.\n");
  }
  const uint8_t maxphysaddr = get_maxphysaddr();
  DBG_PRINTF("maxphysaddr = %d\n", maxphysaddr);

  const uint64_t gib = (1 << 30);

  for (uint32_t i = 0; i < NUM_PAGE_TABLE_ENTRIES; i++) {
    amd64_pdpt[i] = create_pdpt_entry(
      /* This is the i-th GiB for identity-mapping */
      (uint64_t) i * gib,
      maxphysaddr,
      /* Setting huge page in the PDPTE gives us 1 GiB pages */
      PAGE_FLAGS_DEFAULTS | PAGE_FLAGS_HUGE_PAGE
    );

    amd64_pml4[i] = create_pml4_entry(
      (uint64_t) amd64_pdpt,
      maxphysaddr,
      PAGE_FLAGS_DEFAULTS
    );
  }

  amd64_set_cr3(
    create_cr3_entry(
      (uint64_t) &amd64_pml4,
      maxphysaddr,
      PAGE_FLAGS_WRITE_THROUGH
    )
  );
}
