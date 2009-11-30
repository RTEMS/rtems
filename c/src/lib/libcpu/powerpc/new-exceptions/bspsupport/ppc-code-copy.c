/**
 * @file
 *
 * @ingroup powerpc_shared
 *
 * @brief Code copy implementation.
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <string.h>

#include <rtems.h>

#include <libcpu/powerpc-utility.h>

void ppc_code_copy(void *dest, const void *src, size_t n)
{
  if (memcmp(dest, src, n) != 0) {
    memcpy(dest, src, n);

    rtems_cache_flush_multiple_data_lines(dest, n);
    ppc_synchronize_data();

    rtems_cache_invalidate_multiple_instruction_lines(dest, n);
    ppc_synchronize_instructions();
  }
}
