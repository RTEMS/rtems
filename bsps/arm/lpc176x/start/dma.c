/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X_dma
 *
 * @brief Direct memory access (DMA) support.
 */

/*
 * Copyright (c) 2008-2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/endian.h>
#include <bsp/dma.h>
#include <bsp/io.h>

/**
 * @brief Table that indicates if a channel is currently occupied.
 */
static bool lpc176x_dma_channel_occupation[ GPDMA_CH_NUMBER ];

void lpc176x_dma_initialize( void )
{
  /* Enable module power */
  lpc176x_module_enable( LPC176X_MODULE_GPDMA, LPC176X_MODULE_PCLK_DEFAULT );

  /* Disable module */
  GPDMA_CONFIG = 0u;

  /* Reset registers */
  GPDMA_SOFT_SREQ = 0u;
  GPDMA_SOFT_BREQ = 0u;
  GPDMA_SOFT_LSREQ = 0u;
  GPDMA_SOFT_LBREQ = 0u;
  GPDMA_SYNC = 0u;
  GPDMA_CH0_CFG = 0u;
  GPDMA_CH1_CFG = 0u;

  /* Enable module */
#if BYTE_ORDER == LITTLE_ENDIAN
  GPDMA_CONFIG = GPDMA_CONFIG_EN;
#else
  GPDMA_CONFIG = GPDMA_CONFIG_EN | GPDMA_CONFIG_MODE;
#endif
}

rtems_status_code lpc176x_dma_channel_obtain( const unsigned channel )
{
  rtems_status_code status_code = RTEMS_INVALID_ID;

  if ( channel < GPDMA_CH_NUMBER ) {
    rtems_interrupt_level level = 0u;
    bool                  occupation = true;

    rtems_interrupt_disable( level );
    occupation = lpc176x_dma_channel_occupation[ channel ];
    lpc176x_dma_channel_occupation[ channel ] = true;
    rtems_interrupt_enable( level );

    status_code = occupation ? RTEMS_RESOURCE_IN_USE : RTEMS_SUCCESSFUL;
  }

  /* else implies that the channel is not valid. Also,
     there is nothing to do. */

  return status_code;
}

void lpc176x_dma_channel_release( const unsigned channel )
{
  if ( channel < GPDMA_CH_NUMBER ) {
    lpc176x_dma_channel_occupation[ channel ] = false;
  }

  /* else implies that the channel is not valid. Also,
     there is nothing to do. */
}

void lpc176x_dma_channel_disable(
  const unsigned channel,
  const bool     force
)
{
  if ( channel < GPDMA_CH_NUMBER ) {
    volatile lpc176x_dma_channel *ch = GPDMA_CH_BASE_ADDR( channel );
    uint32_t                      cfg = ch->cfg;

    if ( !force ) {
      /* Halt */
      ch->cfg |= GPDMA_CH_CFG_HALT;

      /* Wait for inactive */
      do {
        cfg = ch->cfg;
      } while ( ( cfg & GPDMA_CH_CFG_ACTIVE ) != 0u );
    }

    /* else implies that the channel is not to be forced. Also,
       there is nothing to do. */

    /* Disable */
    ch->cfg &= ~GPDMA_CH_CFG_EN;
  }

  /* else implies that the channel is not valid. Also,
     there is nothing to do. */
}
