/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreMemory
 *
 * @ingroup RTEMSBSPsPowerPCMPC55XX
 *
 * @brief This source file contains the powerpc/mpc55xxevb implementation of
 *   _Memory_Get().
 */

/*
 * Copyright (C) 2012, 2019 embedded brains GmbH (http://www.embedded-brains.de)
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

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/linker-symbols.h>

LINKER_SYMBOL(bsp_section_work_bonus_begin);
LINKER_SYMBOL(bsp_section_work_bonus_end);

static Memory_Area _Memory_Areas[] = {
  MEMORY_INITIALIZER(bsp_section_work_begin, bsp_section_work_end),
  MEMORY_INITIALIZER(bsp_section_work_bonus_begin, bsp_section_work_bonus_end)
};

static const Memory_Information _Memory_Information =
  MEMORY_INFORMATION_INITIALIZER(_Memory_Areas);

const Memory_Information *_Memory_Get(void)
{
  return &_Memory_Information;
}
