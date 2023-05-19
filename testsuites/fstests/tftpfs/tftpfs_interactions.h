/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuiteTestsTFTPFS
 *
 * @brief This header file provides functions used to
 *   implement network interactions of the UDP network fake for tftpfs tests.
 *
 * Definitions and declarations of data structures and functions.
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

#ifndef _TFTPFS_INTERACTIONS_H
#define _TFTPFS_INTERACTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#define NO_BLOCK_SIZE_OPTION 0
#define NO_WINDOW_SIZE_OPTION 0
#define DO_NOT_WAIT_FOR_ANY_TIMEOUT UINT32_MAX

/**
 * @addtogroup RTEMSTestSuiteTestsTFTPFS
 *
 * @{
 */

/*
 * These functions append an interaction to the list of expected interactions.
 * For example, _Tftp_Add_interaction_socket() "means":
 *
 *   * As next interaction, expect that the TFTP client calls function
 *     socket().
 *   * Expect (i.e. check) that this socket() call will get parameter values
 *     as provided by its parameters `domain`, `type` and `protocol`.
 *   * This call to socket() shall return value of parameter  `result`
 *     to the TFTP client.
 *
 * The interactions with functions sendto() and recvfrom() are a bit more
 * complicated because specific packets are expected to be received or sent.
 * For example, _Tftp_Add_interaction_send_rrq() appends an interaction
 * where the TFTP client is expected to call sendto() with an RRQ (Read
 * Request) packet.  _Tftp_Add_interaction_recv_data() appends an interaction
 * where the TFTP client is expected to call recvfrom() and as result it
 * receives a data packet (which the interaction writes into the buffer
 * which the TFTP client provides as parameter in its the recvfrom() call).
 */

void _Tftp_Add_interaction_socket(
  int domain,
  int type,
  int protocol,
  int result
);

void _Tftp_Add_interaction_close( int fd, int result );

void _Tftp_Add_interaction_bind( int fd, int family, int result );

void _Tftp_Add_interaction_send_rrq(
  int fd,
  const char *filename,
  uint16_t dest_port,
  const char *dest_addr_str,
  uint16_t block_size,
  uint16_t window_size,
  bool result
);

void _Tftp_Add_interaction_send_wrq(
  int fd,
  const char *filename,
  uint16_t dest_port,
  const char *dest_addr_str,
  uint16_t block_size,
  uint16_t window_size,
  bool result
);

void _Tftp_Add_interaction_send_ack(
  int fd,
  uint16_t block_num,
  uint16_t dest_port,
  const char *dest_addr_str,
  bool result
);

void _Tftp_Add_interaction_send_data(
  int fd,
  uint16_t block_num,
  size_t start,
  size_t len,
  uint8_t (*get_data)( size_t pos ),
  uint16_t dest_port,
  const char *dest_addr_str,
  bool result
);

void _Tftp_Add_interaction_send_error(
  int fd,
  uint16_t error_code,
  uint16_t dest_port,
  const char *dest_addr_str,
  bool result
);

/*
 * _Tftp_Add_interaction_recv_data() permits only a boolean result.
 * The TFTP client code does not check `errno` and always behaves as if
 * a return of -1 indicates a timeout.  Hence
 * _Tftp_Add_interaction_recv_data() permits only a boolean result
 * and no special value to distinct timeouts from other errors.
 */
void _Tftp_Add_interaction_recv_data(
  int fd,
  uint32_t timeout_ms,
  uint16_t src_port,
  const char *src_addr_str,
  uint16_t block_num,
  size_t start,
  size_t len,
  uint8_t (*get_data)( size_t pos ),
  bool result
);

void _Tftp_Add_interaction_recv_ack(
  int fd,
  uint32_t timeout_ms,
  uint16_t src_port,
  const char *src_addr_str,
  uint16_t block_num,
  bool result
);

void _Tftp_Add_interaction_recv_oack(
  int fd,
  uint32_t timeout_ms,
  uint16_t src_port,
  const char *src_addr_str,
  const char *options,
  size_t options_size,
  bool result
);

void _Tftp_Add_interaction_recv_error(
  int fd,
  uint32_t timeout_ms,
  uint16_t src_port,
  const char *src_addr_str,
  uint16_t error_code,
  const char *err_msg,
  bool result
);

/*
 * The TFTP client receives a "raw" packet.
 *
 * Test tests use this function to trigger packet format errors such as:
 *
 *   * Too short packets,
 *   * Strings without 0-byte at their end
 *   * Wrong op-codes
 */
void _Tftp_Add_interaction_recv_raw(
  int fd,
  uint32_t timeout_ms,
  uint16_t src_port,
  const char *src_addr_str,
  size_t len,
  const uint8_t *bytes,
  bool result
);

void _Tftp_Add_interaction_recv_nothing(
  int fd,
  uint32_t timeout_ms
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TFTPFS_INTERACTIONS_H */
