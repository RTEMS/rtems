/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X_dma
 *
 * @brief Direct memory access (DMA) support.
 */

/*
 * Copyright (c) 2008, 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp/lpc176x.h>
#include <bsp/dma.h>
#include <bsp/irq.h>

static rtems_id lpc176x_dma_sema_table[ GPDMA_CH_NUMBER ];
static bool     lpc176x_dma_status_table[ GPDMA_CH_NUMBER ];

static void lpc176x_dma_copy_handler( void *arg )
{
  /* Get interrupt status */
  uint32_t tc = GPDMA_INT_TCSTAT;
  uint32_t err = GPDMA_INT_ERR_STAT;

  /* Clear interrupt status */
  GPDMA_INT_TCCLR = tc;
  GPDMA_INT_ERR_CLR = err;

  if ( ( tc & GPDMA_STATUS_CH_0 ) != 0 ) {
    rtems_semaphore_release( lpc176x_dma_sema_table[ 0 ] );
  }

  /* else implies that the channel is not the 0. Also,
     there is nothing to do. */

  lpc176x_dma_status_table[ 0 ] = ( err & GPDMA_STATUS_CH_0 ) == 0;

  if ( ( tc & GPDMA_STATUS_CH_1 ) != 0 ) {
    rtems_semaphore_release( lpc176x_dma_sema_table[ 1 ] );
  }

  /* else implies that the channel is not the 1. Also,
     there is nothing to do. */

  lpc176x_dma_status_table[ 1 ] = ( err & GPDMA_STATUS_CH_1 ) == 0;
}

rtems_status_code lpc176x_dma_copy_initialize( void )
{
  rtems_status_code status_code = RTEMS_SUCCESSFUL;
  rtems_id          id0 = RTEMS_ID_NONE;
  rtems_id          id1 = RTEMS_ID_NONE;

  /* Create semaphore for channel 0 */
  status_code = rtems_semaphore_create( rtems_build_name( 'D', 'M', 'A', '0' ),
    0,
    RTEMS_LOCAL | RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE,
    0,
    &id0 );

  if ( status_code != RTEMS_SUCCESSFUL ) {
    return status_code;
  }

  /* else implies that the semaphore to the channel 0 was created succefully.
     Also, there is nothing to do. */

  /* Create semaphore for channel 1 */
  status_code = rtems_semaphore_create( rtems_build_name( 'D', 'M', 'A', '1' ),
    0,
    RTEMS_LOCAL | RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE,
    0,
    &id1 );

  if ( status_code != RTEMS_SUCCESSFUL ) {
    rtems_semaphore_delete( id0 );

    return status_code;
  }

  /* else implies that the semaphore to the channel 1 was created succefully.
     Also, there is nothing to do. */

  /* Install DMA interrupt handler */
  status_code = rtems_interrupt_handler_install( LPC176X_IRQ_DMA,
    "DMA copy",
    RTEMS_INTERRUPT_UNIQUE,
    lpc176x_dma_copy_handler,
    NULL );

  if ( status_code != RTEMS_SUCCESSFUL ) {
    rtems_semaphore_delete( id0 );
    rtems_semaphore_delete( id1 );

    return status_code;
  }

  /* else implies that the interrupt handler was installed succefully. Also,
     there is nothing to do. */

  /* Initialize global data */
  lpc176x_dma_sema_table[ 0 ] = id0;
  lpc176x_dma_sema_table[ 1 ] = id1;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code lpc176x_dma_copy_release( void )
{
  rtems_status_code status_code = RTEMS_SUCCESSFUL;
  rtems_status_code status_code_aux = RTEMS_SUCCESSFUL;

  status_code = rtems_interrupt_handler_remove( LPC176X_IRQ_DMA,
    lpc176x_dma_copy_handler,
    NULL );

  if ( status_code != RTEMS_SUCCESSFUL ) {
    status_code_aux = status_code;
  }

  /* else implies that the interrupt handler was removed succefully. Also,
     there is nothing to do. */

  status_code = rtems_semaphore_delete( lpc176x_dma_sema_table[ 0 ] );

  if ( status_code != RTEMS_SUCCESSFUL ) {
    status_code_aux = status_code;
  }

  /* else implies that the semaphore to the channel 0 was deleted succefully.
     Also, there is nothing to do. */

  status_code = rtems_semaphore_delete( lpc176x_dma_sema_table[ 1 ] );

  if ( status_code != RTEMS_SUCCESSFUL ) {
    status_code_aux = status_code;
  }

  /* else implies that the semaphore to the channel 1 was deleted succefully.
     Also, there is nothing to do. */

  return status_code_aux;
}

rtems_status_code lpc176x_dma_copy(
  unsigned          channel,
  const void *const dest,
  const void *const src,
  size_t            n,
  const size_t      width
)
{
  rtems_status_code             status_code = RTEMS_SUCCESSFUL;
  volatile lpc176x_dma_channel *e = GPDMA_CH_BASE_ADDR( channel );
  uint32_t                      w = GPDMA_CH_CTRL_W_8;

  switch ( width ) {
    case 4:
      w = GPDMA_CH_CTRL_W_32;
      break;
    case 2:
      w = GPDMA_CH_CTRL_W_16;
      break;
  }

  n = n >> w;

  if ( n > 0 && n < 4096 ) {
    e->desc.src = (uint32_t) src;
    e->desc.dest = (uint32_t) dest;
    e->desc.lli = 0;
    e->desc.ctrl = SET_GPDMA_CH_CTRL_TSZ( 0, n ) |
                   SET_GPDMA_CH_CTRL_SBSZ( 0, GPDMA_CH_CTRL_BSZ_1 ) |
                   SET_GPDMA_CH_CTRL_DBSZ( 0, GPDMA_CH_CTRL_BSZ_1 ) |
                   SET_GPDMA_CH_CTRL_SW( 0, w ) |
                   SET_GPDMA_CH_CTRL_DW( 0, w ) |
                   GPDMA_CH_CTRL_ITC |
                   GPDMA_CH_CTRL_SI |
                   GPDMA_CH_CTRL_DI;
    e->cfg = SET_GPDMA_CH_CFG_FLOW( 0, GPDMA_CH_CFG_FLOW_MEM_TO_MEM_DMA ) |
             GPDMA_CH_CFG_IE |
             GPDMA_CH_CFG_ITC |
             GPDMA_CH_CFG_EN;
  } else {
    status_code = RTEMS_INVALID_SIZE;
  }

  return status_code;
}

rtems_status_code lpc176x_dma_copy_wait( const unsigned channel )
{
  rtems_status_code status_code = RTEMS_SUCCESSFUL;

  status_code = rtems_semaphore_obtain( lpc176x_dma_sema_table[ channel ],
    RTEMS_WAIT,
    RTEMS_NO_TIMEOUT );

  if ( status_code != RTEMS_SUCCESSFUL ) {
    return status_code;
  }

  /* else implies that the semaphore was obtained succefully. Also,
     there is nothing to do. */

  status_code = lpc176x_dma_status_table[ channel ]
                ? RTEMS_SUCCESSFUL : RTEMS_IO_ERROR;

  return status_code;
}
