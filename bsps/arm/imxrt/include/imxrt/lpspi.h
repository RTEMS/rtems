/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

#ifndef LIBBSP_ARM_IMXRT_IMXRT_LPSPI_H
#define LIBBSP_ARM_IMXRT_IMXRT_LPSPI_H

#include <sys/ioccom.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define IMXRT_LPSPI_IOC_MAGIC 's'

/**
 * @brief Fetch the raw registers.
 *
 * This allows to do some special operations. Note that if you do something
 * fancy, you should lock the hardware by obtaining the bus with the
 * SPI_BUS_OBTAIN ioctl first. With that, the normal driver can't be used by
 * other tasks. Also note that you should leave the registers in a clean state
 * before releasing the bus again.
 */
#define IMXRT_LPSPI_GET_REGISTERS _IOR(IMXRT_LPSPI_IOC_MAGIC, 0, LPSPI_Type *)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_IMXRT_IMXRT_LPSPI_H */
