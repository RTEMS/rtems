/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMSTM32H7
 *
 * @brief This source file contains the shared SPI2 peripheral configuration.
 */

/*
 * Copyright (C) 2024 On-Line Applications Research (OAR) Corporation
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stm32h7/hal.h>

#ifdef SPI2

/*
 * On most stm32h7 CPUs (at least 743/747/750/753/755/757/7b3), SPI2 can occupy:
 * AF5:
 *   NSS A11
 *   NSS B9
 *   NSS B12
 *   NSS I0
 *   SCK A9
 *   SCK A12
 *   SCK B10
 *   SCK B13
 *   SCK D3
 *   SCK I1
 *   MISO B14
 *   MISO C2
 *   MISO I2
 *   MOSI B15
 *   MOSI C1
 *   MOSI C3
 *   MOSI I3
 * AF7:
 *   NSS B4
 */
stm32h7_spi_context stm32h7_spi2_instance = {
  .spi = {
    .Instance = SPI2,
    /* Configure full-duplex SPI master with 8 bit data size */
    .Init.Mode = SPI_MODE_MASTER,
    .Init.Direction = SPI_DIRECTION_2LINES,
    .Init.DataSize = SPI_DATASIZE_8BIT,
    /* Configure mode 0 */
    .Init.CLKPolarity = SPI_POLARITY_LOW,
    .Init.CLKPhase = SPI_PHASE_1EDGE,
    /* Assume software-controlled-chip-select */
    .Init.NSS = SPI_NSS_SOFT,
    .Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2,
    .Init.FirstBit = SPI_FIRSTBIT_MSB,
    .Init.TIMode = SPI_TIMODE_DISABLE,
    /* Disable CRC calculation */
    .Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE,
    .Init.NSSPMode = SPI_NSS_PULSE_DISABLE,
    .Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA,
    .Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN,
    .Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN,
    .Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE,
    .Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE,
    .Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE,
    .Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE,
    .Init.IOSwap = SPI_IO_SWAP_DISABLE

  },
  .config = &stm32h7_spi2_config,
  .irq = SPI2_IRQn
};

#endif /* SPI2 */
