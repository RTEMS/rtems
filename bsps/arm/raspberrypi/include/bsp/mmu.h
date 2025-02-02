/**
 * @file
 *
 * @ingroup rapberrypi_mmu
 *
 * @brief MMU API.
 */

/*
 * Copyright (C) 2013 Alan Cudmore
 * Copyright (C) 2009 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_ARM_RASPBERRYPI_MMU_H
#define LIBBSP_ARM_RASPBERRYPI_MMU_H

#include <libcpu/arm-cp15.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup raspberrypi_mmu MMU Support
 *
 * @ingroup RTEMSBSPsARMRaspberryPi
 *
 * @brief MMU support.
 *
 * @{
 */

#define RASPBERRYPI_MMU_CLIENT_DOMAIN 15U

#define RASPBERRYPI_MMU_READ_ONLY \
  ((RASPBERRYPI_MMU_CLIENT_DOMAIN << ARM_MMU_SECT_DOMAIN_SHIFT) \
    | ARM_MMU_SECT_DEFAULT)

#define RASPBERRYPI_MMU_READ_ONLY_CACHED \
  (RASPBERRYPI_MMU_READ_ONLY | ARM_MMU_SECT_C | ARM_MMU_SECT_B)

#define RASPBERRYPI_MMU_READ_WRITE \
  ((RASPBERRYPI_MMU_CLIENT_DOMAIN << ARM_MMU_SECT_DOMAIN_SHIFT) \
    | ARM_MMU_SECT_AP_0 \
    | ARM_MMU_SECT_DEFAULT)

#define RASPBERRYPI_MMU_READ_WRITE_CACHED \
  (RASPBERRYPI_MMU_READ_WRITE | ARM_MMU_SECT_C | ARM_MMU_SECT_B)

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_RASPBERRYPI_MMU_H */
