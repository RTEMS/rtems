/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
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

#include <bsp/flash-headers.h>
#include <bspopts.h>

const flexspi_nor_config_t imxrt_flexspi_config = {
  .memConfig = {
    .tag = FLEXSPI_CFG_BLK_TAG,
    .version = FLEXSPI_CFG_BLK_VERSION,
    .readSampleClkSrc = kFlexSPIReadSampleClk_ExternalInputFromDqsPad,
    .csHoldTime = 3u,
    .csSetupTime = 3u,
    .columnAddressWidth = 3u,
    .controllerMiscOption = (1 << kFlexSpiMiscOffset_DdrModeEnable) |
                            (1 << kFlexSpiMiscOffset_WordAddressableEnable) |
                            (1 << kFlexSpiMiscOffset_SafeConfigFreqEnable) |
                            (1 << kFlexSpiMiscOffset_DiffClkEnable),
    .deviceType = kFlexSpiDeviceType_SerialRAM,
    .sflashPadType = kSerialFlash_8Pads,
    .serialClkFreq = kFlexSpiSerialClk_133MHz,
    .sflashA1Size = IMXRT_MEMORY_FLASH_SIZE,
    .dataValidTime = {16u, 16u},
    .lookupTable = {
      FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0xA0, RADDR_DDR, FLEXSPI_8PAD, 0x18),
      FLEXSPI_LUT_SEQ(CADDR_DDR, FLEXSPI_8PAD, 0x10, DUMMY_DDR, FLEXSPI_8PAD, 0x06),
      FLEXSPI_LUT_SEQ(READ_DDR, FLEXSPI_8PAD, 0x04, STOP, FLEXSPI_1PAD, 0x0),
    },
    .lutCustomSeq = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
                     {0,0},{0,0},{0,0},{0,0},{0,0},{0,0}},
  },
  .pageSize = 0x200,
  .sectorSize = 0x40000,
  .blockSize = 0x40000,
  .isUniformBlockSize = 1,
};
