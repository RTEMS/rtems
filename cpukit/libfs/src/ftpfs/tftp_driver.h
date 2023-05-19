/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplTFTPFS
 *
 * @brief This header file provides private interfaces of the
 *   TFTP client library.
 *
 * This file declares the private functions of the Trivial File
 * Transfer Protocol (TFTP) client library.
 */

/*
 * Copyright (C) 2022 embedded brains GmbH & Co. KG
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

#ifndef _TFTP_DRIVER_H
#define _TFTP_DRIVER_H

/* Remove for C++ code */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSImplTFTPFS Trivial File Transfer Protocol (TFTP) file system
 *
 * @ingroup FileSystemTypesAndMount
 *
 * @brief The TFTP file system provides the ability to read files from and
 *   to write files to remote servers using the Trivial File Transfer
 *   Protocol (TFTP).
 *
 * The file `spec/build/cpukit/libtftpfs.yml` specifies how the RTEMS
 * WAF build system has to compile, link and install `libtftpfs`.
 *
 * There also exists a @ref RTEMSTestSuiteTestsTFTPFS
 * "TFTP file system test suite".
 *
 * @{
 */

/**
 * @brief Free the resources associated with a TFTP client connection.
 *
 * This directive releases any resources allocated at the client side.
 * The connection is not closed which implies that the server will not
 * be informed and data is likely lost.  According to RFC 1350 the
 * server will recognize the defect connection by timeouts.
 * This directive is internally used when the TFTP file system is unmounted.
 *
 * @param tftp_handle is the reference returned by a call to tftp_open().
 *   If this parameter is @c NULL, the directive call is a no-op.
 */
void _Tftp_Destroy(
  void *tftp_handle
);

/* Only non-private to ease unit testing */
ssize_t _Tftpfs_Parse_options(
  const char *option_str,
  tftp_net_config *tftp_config,
  uint32_t *flags
);

/** @} */

/* Remove for C++ code */
#ifdef __cplusplus
}
#endif

#endif /* _TFTP_DRIVER_H */
