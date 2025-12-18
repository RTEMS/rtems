/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMEFM32GG11
 *
 * @brief EFM32GG11 Processor Definitions
 */

/*
 * Copyright (C) 2025 Allan N. Hessenflow
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

#ifndef LIBBSP_ARM_EFM32GG11_PROCESSOR_H
#define LIBBSP_ARM_EFM32GG11_PROCESSOR_H

#include <bspopts.h>

#define EFM32GG11B110  0x11b110
#define EFM32GG11B120  0x11b120
#define EFM32GG11B310  0x11b310
#define EFM32GG11B320  0x11b320
#define EFM32GG11B420  0x11b420
#define EFM32GG11B510  0x11b510
#define EFM32GG11B520  0x11b520
#define EFM32GG11B820  0x11b820
#define EFM32GG11B840  0x11b840


#if defined(EFM32GG11_DEVICE)

#if EFM32GG11_DEVICE == EFM32GG11B110
#include "efm32gg11b110f2048gm64.h"
#elif EFM32GG11_DEVICE == EFM32GG11B120
#include "efm32gg11b120f2048gm64.h"
#elif EFM32GG11_DEVICE == EFM32GG11B310
#include "efm32gg11b310f2048gl112.h"
#elif EFM32GG11_DEVICE == EFM32GG11B320
#include "efm32gg11b320f2048gl112.h"
#elif EFM32GG11_DEVICE == EFM32GG11B420
#include "efm32gg11b420f2048gl120.h"
#elif EFM32GG11_DEVICE == EFM32GG11B510
#include "efm32gg11b510f2048gl120.h"
#elif EFM32GG11_DEVICE == EFM32GG11B520
#include "efm32gg11b520f2048gl120.h"
#elif EFM32GG11_DEVICE == EFM32GG11B820
#include "efm32gg11b820f2048gl192.h"
#elif EFM32GG11_DEVICE == EFM32GG11B840
#include "efm32gg11b840f1024gl192.h"
#else
#error unknown device
#endif

#else
#error EFM32GG11_DEVICE not defined
#endif

#endif /* LIBBSP_ARM_EFM32GG11_PROCESSOR_H */
