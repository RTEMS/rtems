/* ---------------------------------------------------------------------------- */
/*                  Atmel Microcontroller Software Support                      */
/*                       SAM Software Package License                           */
/* ---------------------------------------------------------------------------- */
/* Copyright (c) 2015, Atmel Corporation                                        */
/* Copyright (c) 2016, embedded brains GmbH                                     */
/*                                                                              */
/* All rights reserved.                                                         */
/*                                                                              */
/* Redistribution and use in source and binary forms, with or without           */
/* modification, are permitted provided that the following condition is met:    */
/*                                                                              */
/* - Redistributions of source code must retain the above copyright notice,     */
/* this list of conditions and the disclaimer below.                            */
/*                                                                              */
/* Atmel's name may not be used to endorse or promote products derived from     */
/* this software without specific prior written permission.                     */
/*                                                                              */
/* DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR   */
/* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE   */
/* DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,      */
/* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT */
/* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,  */
/* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    */
/* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING         */
/* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, */
/* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                           */
/* ---------------------------------------------------------------------------- */

#include <bsp/atsam-spi.h>

#include <rtems/irq-extension.h>

#include <dev/spi/spi.h>

#define MAX_SPI_FREQUENCY 50000000

typedef struct {
  spi_bus base;
  uint32_t msg_todo;
  const spi_ioc_transfer *msgs;
  rtems_id task_id;
  sXdmad Dma;
  Spid SpiDma;
  uint32_t dma_tx_channel;
  uint32_t dma_rx_channel;
  bool rx_transfer_done;
  bool tx_transfer_done;
  bool chip_select_active;
} atsam_spi_bus;

static void atsam_spi_interrupt(void *arg)
{
  atsam_spi_bus *bus = (atsam_spi_bus *)arg;
  sXdmad *Dma = &bus->Dma;
  Spid *spid = &bus->SpiDma;
  Xdmac *xdmac;
  sXdmadChannel *ch;
  uint32_t xdmaChannelIntStatus, xdmaGlobaIntStatus, xdmaGlobalChStatus;
  uint8_t channel;
  uint8_t bExec = 0;
  rtems_status_code sc;
  assert(Dma != NULL);

  xdmac = Dma->pXdmacs;
  xdmaGlobaIntStatus = XDMAC_GetGIsr(xdmac);

  if ((xdmaGlobaIntStatus & 0xFFFFFF) != 0) {
    xdmaGlobalChStatus = XDMAC_GetGlobalChStatus(xdmac);

    for (channel = 0; channel < Dma->numChannels; channel ++) {
      if (!(xdmaGlobaIntStatus & (1 << channel))) {
        continue;
      }

      ch = &Dma->XdmaChannels[channel];

      if (ch->state == XDMAD_STATE_FREE) {
        return;
      }

      if ((xdmaGlobalChStatus & (XDMAC_GS_ST0 << channel)) == 0) {
        bExec = 0;
        xdmaChannelIntStatus = XDMAC_GetMaskChannelIsr(xdmac, channel);

        if (xdmaChannelIntStatus & XDMAC_CIS_BIS) {
          if ((XDMAC_GetChannelItMask(xdmac, channel) & XDMAC_CIM_LIM) == 0) {
            ch->state = XDMAD_STATE_DONE;
            bExec = 1;
          }
        }

        if (xdmaChannelIntStatus & XDMAC_CIS_LIS) {
          ch->state = XDMAD_STATE_DONE;
          bExec = 1;
        }

        if (xdmaChannelIntStatus & XDMAC_CIS_DIS) {
          ch->state = XDMAD_STATE_DONE;
          bExec = 1;
        }

      } else {
        /* Block end interrupt for LLI dma mode */
        if (XDMAC_GetChannelIsr(xdmac, channel) & XDMAC_CIS_BIS) {
        }
      }

      if (bExec == 1 && (channel == bus->dma_rx_channel)) {
        bus->rx_transfer_done = true;
        XDMAC_DisableGIt(spid->pXdmad->pXdmacs, bus->dma_rx_channel);
      } else if (bExec == 1 && (channel == bus->dma_tx_channel)) {
        bus->tx_transfer_done = true;
        XDMAC_DisableGIt(spid->pXdmad->pXdmacs, bus->dma_tx_channel);
      }

      if (bus->rx_transfer_done && bus->tx_transfer_done) {
        sc = rtems_event_transient_send(bus->task_id);
        assert(sc == RTEMS_SUCCESSFUL);
      }
    }
  }
}

static uint8_t atsam_calculate_dlybcs(uint16_t delay_in_us)
{
  return (
    (BOARD_MCK / delay_in_us) < 0xFF) ?
    (BOARD_MCK / delay_in_us) : 0xFF;
}

static void atsam_set_phase_and_polarity(uint32_t mode, uint32_t *csr)
{
  uint32_t mode_mask = mode & SPI_MODE_3;

  switch(mode_mask) {
    case SPI_MODE_0:
      *csr |= SPI_CSR_NCPHA;
      break;
    case SPI_MODE_1:
      break;
    case SPI_MODE_2:
      *csr |= SPI_CSR_NCPHA;
      *csr |= SPI_CSR_CPOL;
      break;
    case SPI_MODE_3:
      *csr |= SPI_CSR_CPOL;
      break;
  }
  *csr |= SPI_CSR_CSAAT;
}

static void atsam_configure_spi(atsam_spi_bus *bus)
{
  uint8_t delay_cs;
  uint32_t csr = 0;

  delay_cs = atsam_calculate_dlybcs(bus->base.delay_usecs);

  SPID_Configure(
    &bus->SpiDma,
    bus->SpiDma.pSpiHw,
    bus->SpiDma.spiId,
    (SPI_MR_DLYBCS(delay_cs) |
      SPI_MR_MSTR |
      SPI_MR_MODFDIS |
      SPI_PCS(bus->base.cs)),
    &bus->Dma
  );

  csr =
    SPI_DLYBCT(1000, BOARD_MCK) |
    SPI_DLYBS(1000, BOARD_MCK) |
    SPI_SCBR(bus->base.speed_hz, BOARD_MCK) |
    SPI_CSR_BITS(bus->base.bits_per_word - 8);

  atsam_set_phase_and_polarity(bus->base.mode, &csr);

  SPI_ConfigureNPCS(bus->SpiDma.pSpiHw, bus->base.cs, csr);
}

static uint8_t atsam_configure_dma_channels(
  Spid *pSpid,
  uint32_t *dma_tx_channel,
  uint32_t *dma_rx_channel
)
{
  /* Driver initialize */
  XDMAD_Initialize(pSpid->pXdmad, 0);

  /* Allocate a DMA channel for SPI0/1 TX. */
  *dma_tx_channel = XDMAD_AllocateChannel(
    pSpid->pXdmad,
    XDMAD_TRANSFER_MEMORY,
    pSpid->spiId
  );

  if (*dma_tx_channel == XDMAD_ALLOC_FAILED) {
    return SPID_ERROR;
  }

  /* Allocate a DMA channel for SPI0/1 RX. */
  *dma_rx_channel = XDMAD_AllocateChannel(
    pSpid->pXdmad,
    pSpid->spiId,
    XDMAD_TRANSFER_MEMORY
  );

  if (*dma_rx_channel == XDMAD_ALLOC_FAILED) {
    return SPID_ERROR;
  }

  if (XDMAD_PrepareChannel(pSpid->pXdmad, *dma_rx_channel))
    return SPID_ERROR;

  if (XDMAD_PrepareChannel(pSpid->pXdmad, *dma_tx_channel))
    return SPID_ERROR;

  return 0;
}

static uint32_t atsam_set_dmac(atsam_spi_bus *bus)
{
  Spid *spid = &bus->SpiDma;

  /* Initialize DMA controller using channel 0 for RX, 1 for TX. */
  if (
    atsam_configure_dma_channels(
      spid,
      &bus->dma_tx_channel,
      &bus->dma_rx_channel
    )
  ) {
    return SPID_ERROR_LOCK;
  }

  return 0;
}

static uint8_t atsam_configure_link_list(
  Spi *pSpiHw,
  void *pXdmad,
  uint32_t dma_tx_channel,
  uint32_t dma_rx_channel,
  const spi_ioc_transfer *msg
)
{
  sXdmadCfg xdmadRxCfg, xdmadTxCfg;
  uint32_t xdmaCndc, xdmaInt;
  uint32_t spiId;

  if ((unsigned int)pSpiHw == (unsigned int)SPI0) spiId = ID_SPI0;

  if ((unsigned int)pSpiHw == (unsigned int)SPI1) spiId = ID_SPI1;

  /* Setup TX  */

  xdmadTxCfg.mbr_sa = (uint32_t)msg->tx_buf;

  xdmadTxCfg.mbr_da = (uint32_t)&pSpiHw->SPI_TDR;

  xdmadTxCfg.mbr_ubc =
    XDMA_UBC_NVIEW_NDV0 |
    XDMA_UBC_NDE_FETCH_DIS |
    XDMA_UBC_NSEN_UPDATED |
    msg->len;

  xdmadTxCfg.mbr_cfg =
    XDMAC_CC_TYPE_PER_TRAN |
    XDMAC_CC_MBSIZE_SINGLE |
    XDMAC_CC_DSYNC_MEM2PER |
    XDMAC_CC_CSIZE_CHK_1 |
    XDMAC_CC_DWIDTH_BYTE |
    XDMAC_CC_SIF_AHB_IF1 |
    XDMAC_CC_DIF_AHB_IF1 |
    XDMAC_CC_SAM_INCREMENTED_AM |
    XDMAC_CC_DAM_FIXED_AM |
    XDMAC_CC_PERID(XDMAIF_Get_ChannelNumber(spiId, XDMAD_TRANSFER_TX));

  xdmadTxCfg.mbr_bc = 0;
  xdmadTxCfg.mbr_sus = 0;
  xdmadTxCfg.mbr_dus = 0;

  /* Setup RX Link List */

  xdmadRxCfg.mbr_ubc =
    XDMA_UBC_NVIEW_NDV0 |
    XDMA_UBC_NDE_FETCH_DIS |
    XDMA_UBC_NDEN_UPDATED |
    msg->len;

  xdmadRxCfg.mbr_da = (uint32_t)msg->rx_buf;

  xdmadRxCfg.mbr_sa = (uint32_t)&pSpiHw->SPI_RDR;
  xdmadRxCfg.mbr_cfg =
    XDMAC_CC_TYPE_PER_TRAN |
    XDMAC_CC_MBSIZE_SINGLE |
    XDMAC_CC_DSYNC_PER2MEM |
    XDMAC_CC_CSIZE_CHK_1 |
    XDMAC_CC_DWIDTH_BYTE |
    XDMAC_CC_SIF_AHB_IF1 |
    XDMAC_CC_DIF_AHB_IF1 |
    XDMAC_CC_SAM_FIXED_AM |
    XDMAC_CC_DAM_INCREMENTED_AM |
    XDMAC_CC_PERID(XDMAIF_Get_ChannelNumber(spiId, XDMAD_TRANSFER_RX));

  xdmadRxCfg.mbr_bc = 0;
  xdmadRxCfg.mbr_sus = 0;
  xdmadRxCfg.mbr_dus = 0;

  xdmaCndc = 0;

  /* Put all interrupts on for non LLI list setup of DMA */
  xdmaInt =  (
    XDMAC_CIE_BIE |
    XDMAC_CIE_DIE |
    XDMAC_CIE_FIE |
    XDMAC_CIE_RBIE |
    XDMAC_CIE_WBIE |
    XDMAC_CIE_ROIE);

  if (
    XDMAD_ConfigureTransfer(
      pXdmad,
      dma_rx_channel,
      &xdmadRxCfg,
      xdmaCndc,
      0,
      xdmaInt
    )
  ) {
    return SPID_ERROR;
  }

  if (
    XDMAD_ConfigureTransfer(
      pXdmad,
      dma_tx_channel,
      &xdmadTxCfg,
      xdmaCndc,
      0,
      xdmaInt
    )
  ) {
    return SPID_ERROR;
  }

  return 0;
}

static uint32_t atsam_send_command(
  atsam_spi_bus *bus,
  const spi_ioc_transfer *msg
)
{
  Spid *spid = &bus->SpiDma;
  Spi *pSpiHw = spid->pSpiHw;
  Xdmac *pXdmac = bus->SpiDma.pXdmad->pXdmacs;

  if (
    atsam_configure_link_list(
      pSpiHw,
      spid->pXdmad,
      bus->dma_tx_channel,
      bus->dma_rx_channel,
      msg
    )
  ) {
    return SPID_ERROR_LOCK;
  }

  if (!bus->chip_select_active){
    bus->chip_select_active = true;

    SPI_ChipSelect(pSpiHw, 1 << msg->cs);
    SPI_Enable(pSpiHw);
  }

  /* Start DMA */
  XDMAC_StartTransfer(pXdmac, bus->dma_rx_channel);
  XDMAC_StartTransfer(pXdmac, bus->dma_tx_channel);

  return 0;
}

static int atsam_check_configure_spi(atsam_spi_bus *bus, const spi_ioc_transfer *msg)
{
  if (
    msg->mode != bus->base.mode
      || msg->speed_hz != bus->base.speed_hz
      || msg->bits_per_word != bus->base.bits_per_word
      || msg->cs != bus->base.cs
      || msg->delay_usecs != bus->base.delay_usecs
  ) {
    if (
      msg->bits_per_word < 8
        || msg->bits_per_word > 16
        || msg->mode > 3
        || msg->speed_hz > bus->base.max_speed_hz
    ) {
      return -EINVAL;
    }

    bus->base.mode = msg->mode;
    bus->base.speed_hz = msg->speed_hz;
    bus->base.bits_per_word = msg->bits_per_word;
    bus->base.cs = msg->cs;
    bus->base.delay_usecs = msg->delay_usecs;
    atsam_configure_spi(bus);
  }

  return 0;
}

static int atsam_spi_setup_transfer(atsam_spi_bus *bus)
{
  const spi_ioc_transfer *msgs = bus->msgs;
  uint32_t msg_todo = bus->msg_todo;
  uint32_t i;
  uint32_t rv_command;
  int error;

  for (i = 0; i < msg_todo; ++i) {
    error = atsam_check_configure_spi(bus, &msgs[i]);
    if (error < 0) {
      return error;
    }

    rv_command = atsam_send_command(bus, &msgs[i]);
    if (rv_command != 0) {
      return -EIO;
    }

    rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);

    bus->rx_transfer_done = false;
    bus->tx_transfer_done = false;

    if (msgs[i].cs_change > 0) {
      bus->chip_select_active = false;
      SPI_ReleaseCS(bus->SpiDma.pSpiHw);
      SPI_Disable(bus->SpiDma.pSpiHw);
    }
  }

  return 0;
}

static int atsam_spi_transfer(
  spi_bus *base,
  const spi_ioc_transfer *msgs,
  uint32_t msg_count
)
{
  int rv;
  atsam_spi_bus *bus = (atsam_spi_bus *)base;

  if (msg_count == 0) {
    return 0;
  }

  bus->msgs = &msgs[0];
  bus->msg_todo = msg_count;
  bus->task_id = rtems_task_self();

  rv = atsam_spi_setup_transfer(bus);

  return rv;
}


static void atsam_spi_destroy(spi_bus *base)
{
  atsam_spi_bus *bus = (atsam_spi_bus *)base;
  rtems_status_code sc;

  /* Free XDMAD Channels */
  XDMAD_FreeChannel(bus->SpiDma.pXdmad, 0);
  XDMAD_FreeChannel(bus->SpiDma.pXdmad, 1);

  sc = rtems_interrupt_handler_remove(ID_XDMAC, atsam_spi_interrupt, bus);
  assert(sc == RTEMS_SUCCESSFUL);

  SPI_Disable(bus->SpiDma.pSpiHw);
  PMC_DisablePeripheral(bus->SpiDma.spiId);

  spi_bus_destroy_and_free(&bus->base);
}

static int atsam_spi_setup(spi_bus *base)
{
  atsam_spi_bus *bus = (atsam_spi_bus *)base;

  if (
    bus->base.speed_hz > MAX_SPI_FREQUENCY ||
    bus->base.bits_per_word < 8 ||
    bus->base.bits_per_word > 16
  ) {
      return -EINVAL;
  }
  atsam_configure_spi(bus);
  return 0;
}

int spi_bus_register_atsam(
  const char *bus_path,
  uint8_t     spi_peripheral_id,
  Spi        *spi_regs,
  const Pin  *pins,
  size_t      pin_count
)
{
  atsam_spi_bus *bus;
  rtems_status_code sc;

  bus = (atsam_spi_bus *) spi_bus_alloc_and_init(sizeof(*bus));
  if (bus == NULL) {
    return -1;
  }

  bus->base.bits_per_word = 8;
  bus->base.speed_hz = bus->base.max_speed_hz;
  bus->base.delay_usecs = 1;
  bus->base.cs = 1;
  bus->SpiDma.spiId = spi_peripheral_id;
  bus->SpiDma.pSpiHw = spi_regs;

  PIO_Configure(pins, pin_count);
  PMC_EnablePeripheral(spi_peripheral_id);
  XDMAD_Initialize(&bus->Dma, 0);
  atsam_configure_spi(bus);
  atsam_set_dmac(bus);

  sc = rtems_interrupt_handler_install(
    ID_XDMAC,
    "SPI",
    RTEMS_INTERRUPT_UNIQUE,
    atsam_spi_interrupt,
    bus
  );
  if (sc != RTEMS_SUCCESSFUL) {
    (*bus->base.destroy)(&bus->base);

    rtems_set_errno_and_return_minus_one(EIO);
  }

  bus->base.transfer = atsam_spi_transfer;
  bus->base.destroy = atsam_spi_destroy;
  bus->base.setup = atsam_spi_setup;
  bus->base.max_speed_hz = MAX_SPI_FREQUENCY;

  return spi_bus_register(&bus->base, bus_path);
}
