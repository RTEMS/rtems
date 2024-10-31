/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup zynq_devcfg
 * @brief Device configuration support.
 *
 * Provides support for the Zynq7000 series device configuration interface
 * controller. PCAP command sequences are written using the write interface,
 * and PCAP responses are retrieved with the read interface. The driver can be
 * used for reconfiguration of the FPGA, and also reading FPGA configuration
 * data for error checking.
 */

/*
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

/**
 * @defgroup zynq_devcfg Device Configuration Interface Support
 * @ingroup arm_zynq
 * @brief Device Configuration Interface Support
 */

#ifndef LIBBSP_ARM_XILINX_ZYNQ_DEVCFG_H
#define LIBBSP_ARM_XILINX_ZYNQ_DEVCFG_H

#include <rtems/libio.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define ZYNQ_DEVCFG_NAME "/dev/fpga"
/*
 * Add to CONFIGURE_APPLICATION_PREREQUISITE_DRIVERS
 */
#define ZYNQ_DEVCFG_DRIVER_TABLE_ENTRY \
  { zynq_devcfg_init, zynq_devcfg_open, zynq_devcfg_close, zynq_devcfg_read, \
    zynq_devcfg_write, zynq_devcfg_control }

/* PCAP DMA transfers must be 64-byte aligned.
   Use this to read or write an aligned buffer avoiding the
   use of the heap in the driver. */
#define ZYNQ_DEVCFG_PCAP_DMA_ALIGN 64

/* Configuration command words. */
#define ZYNQ_DEVCFG_CFG_DUMMY ( 0xffffffff )
#define ZYNQ_DEVCFG_CFG_BUS_WIDTH_SYNC ( 0x000000bb )
#define ZYNQ_DEVCFG_CFG_BUS_WIDTH_DETECT ( 0x11220044 )
#define ZYNQ_DEVCFG_CFG_SYNC ( 0xaa995566 )

/** @brief Zynq configuration frame length in bytes */
#define ZYNQ_DEVCFG_CONFIG_FRAME_LEN ( 101 * 4 )

#define ZYNQ_DEVCFG_IOCTL_VERSION_MAX_LEN 16

enum zynq_devcfg_ioctl {
  /** @brief Argument: Buffer for character string of at least
   * ZYNQ_DEVCFG_IOCTL_VERSION_MAX_LEN bytes.
   */
  ZYNQ_DEVCFG_IOCTL_VERSION,
  /** @brief Argument: None. */
  ZYNQ_DEVCFG_IOCTL_FPGA_PROGRAM_PRE,
  /** @brief Argument: None. */
  ZYNQ_DEVCFG_IOCTL_FPGA_PROGRAM_POST,
  /** @brief Argument: None. */
  ZYNQ_DEVCFG_IOCTL_FPGA_PROGRAM_WAIT_DONE,
  /** @brief Argument: bool. */
  ZYNQ_DEVCFG_IOCTL_SET_SECURE,
  /** @brief Argument: bool. */
  ZYNQ_DEVCFG_IOCTL_SET_WRITE_MODE_RESTRICTED
};

rtems_device_driver zynq_devcfg_init(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *args
);

rtems_device_driver zynq_devcfg_open(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *args
);

rtems_device_driver zynq_devcfg_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *args
);

/**
 * @brief Read from the PCAP controller.
 *
 * Readback reads cannot be split into multiple DMA reads, this may cause the
 * PCAP DMA to exhibit unexpected behavior. Therefore, the read length must
 * match the preceding command sequence's expected data output length.
 */
rtems_device_driver zynq_devcfg_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *args
);

/**
 * @brief Write to the PCAP controller.
 *
 * Data format: dword aligned bistream data or PCAP commands. Bitstream data is
 * expected to be formatted as Vivado 2016.4 outputs BIN-format bitstreams by
 * default (not bit-swapped) BUT with the byte order within each dword changed
 * to little endian. See UG470 for information on data ordering.
 */
rtems_device_driver zynq_devcfg_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *args
);

rtems_device_driver zynq_devcfg_control(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *args
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_XILINX_ZYNQ_DEVCFG_H */
