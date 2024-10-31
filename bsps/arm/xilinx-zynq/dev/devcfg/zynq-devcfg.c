/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Xilinx Zynq7000 Device Configuration Driver Implementation
 *
 * Notes:
 * - There will only ever be 1 of these controllers in the Zynq, so this driver
 *   is designed to be run as a single instance.
 * - Even if an interrupt bit is already asserted, unmasking it will lead to
 *   triggering the interrupt. In several areas operations are started before
 *   unmasking an interrupt which could be triggered by those operations; this
 *   interrupt behavior allows for such code to not be racy.
 * - Secure loading is not supported.
 *
 * Copyright (c) 2016
 *  NSF Center for High-Performance Reconfigurable Computing (CHREC),
 *  University of Florida.  All rights reserved.
 * Copyright (c) 2017
 *  NSF Center for High-Performance Reconfigurable Computing (CHREC),
 *  University of Pittsburgh.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing
 * official policies, either expressed or implied, of CHREC.
 *
 * Author: Patrick Gauvin <gauvin@hcs.ufl.edu>
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/rtems/sem.h>
#include <rtems/irq-extension.h>
#include <dev/slcr/zynq-slcr.h>
#include <dev/slcr/zynq-slcr-regs.h>
#include <dev/devcfg/zynq-devcfg.h>
#include <dev/devcfg/zynq-devcfg-regs.h>

#include <rtems/bspIo.h>

#define WARN( msg ) printk( "%s:%s: %s", __FILE__, __func__, msg )
/* Timeout for interrupt waits, 2 seconds should be enough for any operation.
 */
#define INT_TIMEOUT ( 2 * rtems_clock_get_ticks_per_second() )
#define ZYNQ_DEVCFG_EVENT_SET RTEMS_EVENT_0

typedef struct {
  volatile zynq_devcfg_regs *regs;
  /* Used to restrict the device to being opened once at a time. */
  rtems_mutex                open_lock;
  /* Used for mutual exclusion between read/write/ioctl. */
  rtems_mutex                internal_lock;
  /* Indicates if the PCAP will be used for a secure bitstream. Secure
   * bitstreams are untested with this driver. Defaults to false.
   */
  bool                       secure;
  /* If true, write data is not assumed to be a complete bitstream and no
   * checks are performed on it.
   */
  bool                       write_mode_restricted;
  rtems_id                   current_task;
} driver_data;

typedef struct {
  uint8_t *buf;
  uint8_t *buf_orig;
} dma_buf;

static driver_data data;

/**
 * @brief Check if bit is set in reg (and also not masked by mask), and if it
 * is, write that bit to reg.
 *
 * @retval true The bit was set and not masked, and was written to.
 * @retval false The bit was not written to.
 */
static inline bool check_and_set(
  volatile uint32_t *reg,
  uint32_t           mask,
  uint32_t           bit
)
{
  if ( *reg & bit & ~mask )
  {
    *reg = bit;
    return true;
  }
  return false;
}

/* Only one event is used since only one interrupt is unmasked at a time.  The
 * interrupt is cleared and masked after it is caught here.
 */
static void zynq_devcfg_isr(
  void *args
)
{
  const uint32_t intrs[] = {
    ZYNQ_DEVCFG_INT_DMA_DONE_INT,
    ZYNQ_DEVCFG_INT_PCFG_INIT_PE_INT,
    ZYNQ_DEVCFG_INT_PCFG_INIT_NE_INT,
    ZYNQ_DEVCFG_INT_PCFG_DONE_INT,
    ZYNQ_DEVCFG_INT_PSS_CFG_RESET_B_INT
  };
  volatile uint32_t *int_sts = &data.regs->int_sts;
  volatile uint32_t *int_mask = &data.regs->int_mask;

  (void) args;

  for ( size_t i = 0; i < RTEMS_ARRAY_SIZE( intrs ); ++i )
    if ( check_and_set( int_sts, *int_mask, intrs[i] ) )
    {
      *int_mask |= intrs[i];
      if ( RTEMS_INVALID_ID != data.current_task )
        rtems_event_system_send( data.current_task, ZYNQ_DEVCFG_EVENT_SET );
      return;
    }
}

static inline bool ptr_is_pcap_dma_aligned(
  void *ptr
)
{
  return 0 == (uintptr_t)ptr % ZYNQ_DEVCFG_PCAP_DMA_ALIGN;
}

/**
 * @brief Create an aligned buffer for the bitstream.
 *
 * @param len Desired length of the buffer in bytes.
 *
 * @return dma_buf members are NULL if malloc failed.
 */
static dma_buf dma_buf_get(
  size_t len
)
{
  dma_buf dbuf;

  dbuf.buf_orig = malloc( len + ZYNQ_DEVCFG_PCAP_DMA_ALIGN );
  if ( NULL == dbuf.buf_orig )
  {
    dbuf.buf = NULL;
    return dbuf;
  }

  if ( !ptr_is_pcap_dma_aligned( dbuf.buf_orig ) )
  {
    dbuf.buf = dbuf.buf_orig + ZYNQ_DEVCFG_PCAP_DMA_ALIGN
        - ( (size_t)dbuf.buf_orig % ZYNQ_DEVCFG_PCAP_DMA_ALIGN );
  }
  else
    dbuf.buf = dbuf.buf_orig;
  return dbuf;
}

/**
 * @brief Frees the originally allocated area for dbuf.
 */
static void dma_buf_release(
  dma_buf dbuf
)
{
  free( dbuf.buf_orig );
}

/**
 * @brief Initiates a PCAP DMA transfer.
 *
 * @param src[in] For programming the FPGA, this is the location of the
 * bitstream data. For readback, it is the location of the PL readback command
 * sequence.
 * @param src_len Typically the length of bitstream in dwords, or the number of
 * PL commands. The user must check this value for correctness.
 * @param dst[in,out] For programming the FPGA use ZYNQ_DEVCFG_BITSTREAM_ADDR,
 * for readback this is where the readback data is stored.
 * @param dst_len Typically the Length of bitstream in dwords, or the number of
 * readback words expected. The user must check this value for correctness.
 * @param pcap_wait If true, interrupt assertion waits for both the AXI
 * transfer and the PCAP to finish.
 *
 * @retval 0 Transfer was started.
 * @retval -1 src_len or dst_len invalid.
 * @retval -2 The DMA queue was full.
 */
static int pcap_dma_xfer(
  uint32_t                  *src,
  size_t                     src_len,
  uint32_t                  *dst,
  size_t                     dst_len,
  bool                       pcap_wait
)
{
  if ( ZYNQ_DEVCFG_DMA_SRC_LEN_LEN( src_len ) != src_len )
    return -1;
  if ( ZYNQ_DEVCFG_DMA_DEST_LEN_LEN( dst_len ) != dst_len )
    return -1;

  if ( pcap_wait )
  {
    src = (uint32_t *)
        ( (uintptr_t)src | ZYNQ_DEVCFG_DMA_SRC_ADDR_DMA_DONE_INT_WAIT_PCAP );
    dst = (uint32_t *)
        ( (uintptr_t)dst | ZYNQ_DEVCFG_DMA_DST_ADDR_DMA_DONE_INT_WAIT_PCAP );
  }

#ifdef ZYNQ_DEVCFG_DEBUG
  printk( "DMA TRANSFER REQUESTED:\n" );
  printk( "Source: %p\n", src );
  printk( "Source length: %zu\n", src_len );
  printk( "Destination: %p\n", dst );
  printk( "Destination length: %zu\n", dst_len );
#endif /* ZYNQ_DEVCFG_DEBUG */

  /* Check if the command queue is full */
  if ( ZYNQ_DEVCFG_STATUS_DMA_CMD_Q_F( data.regs->status ) )
  {
    WARN( "Zynq DMA queue full\n" );
    return -2;
  }

  /* Order is important */
  data.regs->dma_src_addr = (uint32_t)src;
  data.regs->dma_dst_addr = (uint32_t)dst;
  data.regs->dma_src_len = ZYNQ_DEVCFG_DMA_SRC_LEN_LEN( src_len );
  data.regs->dma_dest_len = ZYNQ_DEVCFG_DMA_DEST_LEN_LEN( dst_len );

  return 0;
}

/**
 * @brief Unmasks and waits for the interrupt in int_bit.
 *
 * @param int_bit The interrupt bit in int_sts (NOT a bit number).
 * @return The result of rtems_event_system_receive.
 */
static rtems_status_code int_enable_and_wait(
  uint32_t int_bit
)
{
  rtems_event_set   ev;
  rtems_status_code status;

  data.current_task = rtems_task_self();
  /* data.current_task must be updated before an interrupt is handled. */
  RTEMS_COMPILER_MEMORY_BARRIER();
  data.regs->int_mask &= ~int_bit;
  status = rtems_event_system_receive(
    ZYNQ_DEVCFG_EVENT_SET,
    RTEMS_WAIT,
    INT_TIMEOUT,
    &ev
  );
  /* Re-mask interrupt if not received. */
  if ( RTEMS_SUCCESSFUL != status )
    data.regs->int_mask |= int_bit;
  return status;
}

/**
 * @brief Wait for a DMA transfer to finish and check for failure.
 *
 * @retval 0 Success.
 * @retval -1 DMA timeout.
 * @retval -2 AXI transfer error.
 * @retval -3 Receive FIFO overflow.
 * @retval -4 DMA command error or command queue overflow.
 * @retval -5 Mismatch between PCAP output length and DMA transfer length.
 * @retval -6 HMAC error.
 */
static int pcap_dma_xfer_wait_and_check( void )
{
  uint32_t          int_sts;
  rtems_status_code status;

  /* NOTE: The ISR will handle acknowledging the transfer. */
  status = int_enable_and_wait( ZYNQ_DEVCFG_INT_DMA_DONE_INT );
  if ( RTEMS_SUCCESSFUL != status )
  {
    WARN( "DMA timed out\n" );
    return -1;
  }

  int_sts = data.regs->int_sts;
  if (
    ZYNQ_DEVCFG_INT_AXI_WERR_INT_GET( int_sts )
    || ZYNQ_DEVCFG_INT_AXI_RTO_INT_GET( int_sts )
    || ZYNQ_DEVCFG_INT_AXI_RERR_INT_GET( int_sts )
  )
    return -2;
  if ( ZYNQ_DEVCFG_INT_RX_FIFO_OV_INT_GET( int_sts ) )
    return -3;
  if (
    ZYNQ_DEVCFG_INT_DMA_CMD_ERR_INT_GET( int_sts )
    || ZYNQ_DEVCFG_INT_DMA_Q_OV_INT_GET( int_sts )
  )
    return -4;
  if ( ZYNQ_DEVCFG_INT_P2D_LEN_ERR_INT_GET( int_sts ) )
    return -5;
  if ( ZYNQ_DEVCFG_INT_PCFG_HMAC_ERR_INT_GET( int_sts ) )
    return -6;

  return 0;
}

/**
 * @brief Configure the PCAP controller.
 */
static void pl_init( void )
{
  data.regs->ctrl = ZYNQ_DEVCFG_CTRL_PCAP_MODE( 1 )
      | ZYNQ_DEVCFG_CTRL_PCAP_PR( ZYNQ_DEVCFG_CTRL_PCAP_PR_PCAP )
      | ZYNQ_DEVCFG_CTRL_RESERVED_BITS | data.regs->ctrl;
  /* Disable loopback */
  data.regs->mctrl = ZYNQ_DEVCFG_MCTRL_SET(
    data.regs->mctrl,
    ~ZYNQ_DEVCFG_MCTRL_INT_PCAP_LPBK( 1 ) & data.regs->mctrl
  );
  /* Clear all interrupts */
  data.regs->int_sts = ZYNQ_DEVCFG_INT_ALL;

  if ( !data.secure )
  {
    if ( ZYNQ_DEVCFG_CTRL_QUARTER_PCAP_RATE_EN( data.regs->ctrl ) )
      data.regs->ctrl =
          ( ~ZYNQ_DEVCFG_CTRL_QUARTER_PCAP_RATE_EN( 1 ) & data.regs->ctrl )
          | ZYNQ_DEVCFG_CTRL_RESERVED_BITS;
  }
  else
  {
    if ( !ZYNQ_DEVCFG_CTRL_QUARTER_PCAP_RATE_EN( data.regs->ctrl ) )
      data.regs->ctrl =
          ZYNQ_DEVCFG_CTRL_QUARTER_PCAP_RATE_EN( 1 ) | data.regs->ctrl
          | ZYNQ_DEVCFG_CTRL_RESERVED_BITS;
  }
}

/**
 * @brief Initialize the PCAP and clear the FPGA's configuration.
 *
 * @retval 0 Success.
 * @retval -1 PCAP intialization timeout.
 * @retval -2 PCAP deinitialization timeout.
 * @retval -3 PCAP reinitialization timeout.
 * @retval -4 Device reset timeout.
 */
static int pl_clear( void )
{
  /* TODO: Check that controller is available */
  rtems_status_code status;

  if ( !ZYNQ_DEVCFG_CTRL_PCFG_PROG_B_GET( data.regs->ctrl ) )
    data.regs->ctrl = ZYNQ_DEVCFG_CTRL_PCFG_PROG_B( 1 )
        | ZYNQ_DEVCFG_CTRL_RESERVED_BITS | data.regs->ctrl;
  if ( !ZYNQ_DEVCFG_STATUS_PCFG_INIT_GET( data.regs->status ) )
  {
    status = int_enable_and_wait( ZYNQ_DEVCFG_INT_PCFG_INIT_PE_INT );
    if ( RTEMS_SUCCESSFUL != status )
    {
      WARN( "PCAP init timed out\n" );
      return -1;
    }
  }

  data.regs->ctrl = ( ~ZYNQ_DEVCFG_CTRL_PCFG_PROG_B( 1 ) & data.regs->ctrl )
      | ZYNQ_DEVCFG_CTRL_RESERVED_BITS;
  status = int_enable_and_wait( ZYNQ_DEVCFG_INT_PCFG_INIT_NE_INT );
  if ( RTEMS_SUCCESSFUL != status )
  {
    WARN( "PCAP deinit timed out\n" );
    return -2;
  }

  data.regs->ctrl = ZYNQ_DEVCFG_CTRL_PCFG_PROG_B( 1 )
      | ZYNQ_DEVCFG_CTRL_RESERVED_BITS | data.regs->ctrl;
  status = int_enable_and_wait( ZYNQ_DEVCFG_INT_PCFG_INIT_PE_INT );
  if ( RTEMS_SUCCESSFUL != status )
  {
    WARN( "PCAP reinit timed out\n" );
    return -3;
  }

  status = int_enable_and_wait( ZYNQ_DEVCFG_INT_PSS_CFG_RESET_B_INT );
  if ( RTEMS_SUCCESSFUL != status )
  {
    WARN( "PSS_CFG_RESET_B_INT timed out\n" );
    return -4;
  }

  return 0;
}

static int pl_prog_pre( void )
{
  int status;

  pl_init();
  /* Hold FPGA clocks in reset */
  zynq_slcr_fpga_clk_rst( 0xf );
  /* Enable PS to PL level shifters */
  zynq_slcr_level_shifter_enable( ZYNQ_SLCR_LVL_SHFTR_EN_DISABLE );
  zynq_slcr_level_shifter_enable( ZYNQ_SLCR_LVL_SHFTR_EN_PS_TO_PL );
  status = pl_clear();
  return status;
}

static int pl_prog_post( void )
{
  /* Enable all PS-PL level shifters */
  zynq_slcr_level_shifter_enable( ZYNQ_SLCR_LVL_SHFTR_EN_ALL );
  /* Release FPGA clocks from reset */
  zynq_slcr_fpga_clk_rst( 0 );
  return 0;
}

static int pl_prog_done_wait( void )
{
  rtems_status_code status;

  status = int_enable_and_wait( ZYNQ_DEVCFG_INT_PCFG_DONE_INT );
  if ( RTEMS_SUCCESSFUL != status )
    return -1;
  return 0;
}

static bool hdr_check_bin(
  const uint32_t *bitstream,
  size_t len
)
{
  const uint32_t valid_header[] = {
    ZYNQ_DEVCFG_CFG_DUMMY,
    ZYNQ_DEVCFG_CFG_DUMMY,
    ZYNQ_DEVCFG_CFG_DUMMY,
    ZYNQ_DEVCFG_CFG_DUMMY,
    ZYNQ_DEVCFG_CFG_DUMMY,
    ZYNQ_DEVCFG_CFG_DUMMY,
    ZYNQ_DEVCFG_CFG_DUMMY,
    ZYNQ_DEVCFG_CFG_DUMMY,
    ZYNQ_DEVCFG_CFG_BUS_WIDTH_SYNC,
    ZYNQ_DEVCFG_CFG_BUS_WIDTH_DETECT,
    ZYNQ_DEVCFG_CFG_DUMMY,
    ZYNQ_DEVCFG_CFG_DUMMY,
    ZYNQ_DEVCFG_CFG_SYNC
  };

  if ( len < RTEMS_ARRAY_SIZE( valid_header ) )
    return false;
  for ( size_t i = 0; i < RTEMS_ARRAY_SIZE( valid_header ); ++i )
    if ( valid_header[i] != bitstream[i] ) {
      return false;
    }
  return true;
}

/* TODO: Check that PL power is on.
 * TODO: Check for configuration differences between silicon revisions.
 */
rtems_device_driver zynq_devcfg_init(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *args
)
{
  rtems_status_code status;

  (void) args;

  data.regs = (zynq_devcfg_regs *)ZYNQ_DEVCFG_BASE_ADDR;
  data.secure = false;
  data.current_task = RTEMS_INVALID_ID;
  data.write_mode_restricted = true;

  rtems_mutex_init( &data.open_lock, "DevC" );
  rtems_mutex_init( &data.internal_lock, "DvCi" );

  /* Mask and clear all interrupts and install handler */
  data.regs->int_mask |= ZYNQ_DEVCFG_INT_ALL;
  data.regs->int_sts = ZYNQ_DEVCFG_INT_ALL;
  status = rtems_interrupt_handler_install(
    ZYNQ_DEVCFG_INTERRUPT_VECTOR,
    "DevC ISR",
    RTEMS_INTERRUPT_UNIQUE,
    zynq_devcfg_isr,
    NULL
  );
  if ( RTEMS_SUCCESSFUL != status )
  {
    WARN( "Failed to assign interrupt handler\n" );
    status = RTEMS_INTERNAL_ERROR;
    goto err;
  }

  status = rtems_io_register_name( ZYNQ_DEVCFG_NAME, major, minor );
  if ( RTEMS_SUCCESSFUL != status )
  {
    status = RTEMS_INVALID_NAME;
    goto err_register;
  }

  return RTEMS_SUCCESSFUL;
err_register:
  rtems_interrupt_handler_remove(
    ZYNQ_DEVCFG_INTERRUPT_VECTOR,
    zynq_devcfg_isr,
    NULL
  );
err:
  return status;
}

rtems_device_driver zynq_devcfg_open(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *args
)
{
  int rstatus;

  (void) major;
  (void) minor;
  (void) args;

  rstatus = rtems_mutex_try_lock( &data.open_lock );
  if ( EBUSY == rstatus )
    return RTEMS_RESOURCE_IN_USE;

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver zynq_devcfg_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *args
)
{
  (void) major;
  (void) minor;
  (void) args;

  rtems_mutex_unlock( &data.open_lock );

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver zynq_devcfg_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *args
)
{
  rtems_libio_rw_args_t *rw_args;
  int                    status;
  rtems_status_code      final_status;
  dma_buf                data_buf;

  (void) major;
  (void) minor;
  rw_args = args;
  rw_args->bytes_moved = 0;

  status = rtems_mutex_try_lock( &data.internal_lock );
  if ( EBUSY == status )
  {
    final_status = RTEMS_RESOURCE_IN_USE;
    goto err_obtain;
  }

  if ( rw_args->count < 4 )
  {
    final_status = RTEMS_INVALID_SIZE;
    goto err_insane;
  }
  /* TODO: It might be valid to read configuration registers while the PL is
   * not programmed.
   */
  /* PCFG_DONE must be asserted before readback */
  if ( !ZYNQ_DEVCFG_INT_PCFG_DONE_INT_GET( data.regs->int_sts ) )
  {
    WARN( "read attempted when FPGA configuration not done\n" );
    final_status = RTEMS_IO_ERROR;
    goto err_insane;
  }

  if ( !ptr_is_pcap_dma_aligned ( rw_args->buffer ) )
  {
    data_buf = dma_buf_get( rw_args->count );
    if ( NULL == data_buf.buf )
    {
      final_status = RTEMS_NO_MEMORY;
      goto err_insane;
    }
  }
  else
    data_buf.buf = (uint8_t *)rw_args->buffer;

  status = pcap_dma_xfer(
    (uint32_t *)ZYNQ_DEVCFG_BITSTREAM_ADDR,
    rw_args->count / 4,
    (uint32_t *)data_buf.buf,
    rw_args->count / 4,
    true
  );
  if ( status )
  {
    WARN( "DMA setup FAILED\n" );
    final_status = RTEMS_IO_ERROR;
    goto err_dma;
  }
  else
  {
    status = pcap_dma_xfer_wait_and_check();
    if ( status )
    {
      WARN( "DMA FAILED\n" );
      final_status = RTEMS_IO_ERROR;
      goto err_dma;
    }
  }

  /* Ensure stale data is not read */
  rtems_cache_invalidate_multiple_data_lines( data_buf.buf, rw_args->count );

  final_status = RTEMS_SUCCESSFUL;
  rw_args->bytes_moved = rw_args->count;
  if ( data_buf.buf != (uint8_t *)rw_args->buffer )
    memcpy( rw_args->buffer, data_buf.buf, rw_args->count );
err_dma:
  if ( data_buf.buf != (uint8_t *)rw_args->buffer )
    dma_buf_release( data_buf );
  rtems_mutex_unlock( &data.internal_lock );
err_insane:
err_obtain:
  return final_status;
}

rtems_device_driver zynq_devcfg_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *args
)
{
  rtems_libio_rw_args_t *rw_args;
  int                    status;
  rtems_status_code      final_status;
  dma_buf                data_buf;

  (void) major;
  (void) minor;
  rw_args = args;
  rw_args->bytes_moved = 0;

  status = rtems_mutex_try_lock( &data.internal_lock );
  if ( EBUSY == status )
  {
    final_status = RTEMS_RESOURCE_IN_USE;
    goto err_obtain;
  }

  /* TODO: Check byte order. */
  if ( data.write_mode_restricted )
  {
    /* Only BIN files in restricted mode. */
    if ( !hdr_check_bin ( (uint32_t *)rw_args->buffer, rw_args->count / 4 ) )
    {
      /* Closest status to invalid argument I could find. */
      final_status = RTEMS_INVALID_NUMBER;
      goto err_obtain;
    }
    status = pl_prog_pre();
    if ( 0 != status )
    {
      final_status = RTEMS_IO_ERROR;
      goto err_obtain;
    }
  }

  if ( !ptr_is_pcap_dma_aligned( rw_args->buffer ) )
  {
    data_buf = dma_buf_get( rw_args->count );
    if ( NULL == data_buf.buf )
    {
      final_status = RTEMS_NO_MEMORY;
      goto err_obtain;
    }
    memcpy( data_buf.buf, rw_args->buffer, rw_args->count );
  }
  else
    data_buf.buf = (uint8_t *)rw_args->buffer;

  /* Ensure data is available to the DMA engine */
  rtems_cache_flush_multiple_data_lines( data_buf.buf, rw_args->count );

  status = pcap_dma_xfer(
    (uint32_t *)data_buf.buf,
    rw_args->count / 4,
    (uint32_t *)ZYNQ_DEVCFG_BITSTREAM_ADDR,
    rw_args->count / 4,
    true
  );
  if ( status )
  {
    final_status = RTEMS_IO_ERROR;
    goto err_dma;
  }
  else
  {
    status = pcap_dma_xfer_wait_and_check();
    if ( status )
    {
      final_status = RTEMS_IO_ERROR;
      goto err_dma;
    }
  }

  if ( data.write_mode_restricted )
  {
    status = pl_prog_post();
    if ( 0 != status )
    {
      final_status = RTEMS_IO_ERROR;
      goto err_dma;
    }
    status = pl_prog_done_wait();
    if ( 0 != status )
    {
      final_status = RTEMS_TIMEOUT;
      goto err_dma;
    }
  }

  final_status = RTEMS_SUCCESSFUL;
  rw_args->bytes_moved = rw_args->count;
err_dma:
  if ( data_buf.buf != (uint8_t *)rw_args->buffer )
    dma_buf_release( data_buf );
  rtems_mutex_unlock( &data.internal_lock );
err_obtain:
  return final_status;
}

rtems_device_driver zynq_devcfg_control(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *args
)
{
  rtems_libio_ioctl_args_t *ioctl_args;
  char                     *str;
  int                       status;
  rtems_status_code         final_status;

  (void) major;
  (void) minor;
  ioctl_args = args;

  status = rtems_mutex_try_lock( &data.internal_lock );
  if ( EBUSY == status )
  {
    ioctl_args->ioctl_return = -1;
    return RTEMS_RESOURCE_IN_USE;
  }

  final_status = RTEMS_SUCCESSFUL;
  ioctl_args->ioctl_return = 0;
  switch ( ioctl_args->command ) {
  case ZYNQ_DEVCFG_IOCTL_VERSION:
    str = ioctl_args->buffer;
    switch( ZYNQ_DEVCFG_MCTRL_PS_VERSION_GET( data.regs->mctrl ) ) {
      case ZYNQ_DEVCFG_MCTRL_PS_VERSION_1_0:
        strncpy( str, "1.0", ZYNQ_DEVCFG_IOCTL_VERSION_MAX_LEN );
        break;
      case ZYNQ_DEVCFG_MCTRL_PS_VERSION_2_0:
        strncpy( str, "2.0", ZYNQ_DEVCFG_IOCTL_VERSION_MAX_LEN );
        break;
      case ZYNQ_DEVCFG_MCTRL_PS_VERSION_3_0:
        strncpy( str, "3.0", ZYNQ_DEVCFG_IOCTL_VERSION_MAX_LEN );
        break;
      case ZYNQ_DEVCFG_MCTRL_PS_VERSION_3_1:
        strncpy( str, "3.1", ZYNQ_DEVCFG_IOCTL_VERSION_MAX_LEN );
        break;
      default:
        strncpy( str, "???", ZYNQ_DEVCFG_IOCTL_VERSION_MAX_LEN );
        break;
    }
    break;
  case ZYNQ_DEVCFG_IOCTL_FPGA_PROGRAM_PRE:
    status = pl_prog_pre();
    if ( 0 != status )
    {
      ioctl_args->ioctl_return = -1;
      final_status = RTEMS_UNSATISFIED;
    }
    break;
  case ZYNQ_DEVCFG_IOCTL_FPGA_PROGRAM_POST:
    status = pl_prog_post();
    if ( 0 != status )
    {
      ioctl_args->ioctl_return = -1;
      final_status = RTEMS_UNSATISFIED;
    }
    break;
  case ZYNQ_DEVCFG_IOCTL_FPGA_PROGRAM_WAIT_DONE:
    status = pl_prog_done_wait();
    if ( 0 != status )
    {
      ioctl_args->ioctl_return = -1;
      final_status = RTEMS_TIMEOUT;
    }
    break;
  case ZYNQ_DEVCFG_IOCTL_SET_SECURE:
    data.secure = *(bool *)ioctl_args->buffer;
    break;
  case ZYNQ_DEVCFG_IOCTL_SET_WRITE_MODE_RESTRICTED:
    data.write_mode_restricted = *(bool *)ioctl_args->buffer;
    break;
  default:
    ioctl_args->ioctl_return = -1;
    final_status = RTEMS_INVALID_NAME; /* Maps to EINVAL */
    break;
  }

  rtems_mutex_unlock( &data.internal_lock );

  return final_status;
}
