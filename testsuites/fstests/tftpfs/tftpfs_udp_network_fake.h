/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuiteTestsTFTPFS
 *
 * @brief This header file provides interfaces and functions used to
 *   implement the UDP network fake for tftpfs tests.
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

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h> /* ssize_t */

#ifndef _TFTPFS_UDP_NETWORK_FAKE_H
#define _TFTPFS_UDP_NETWORK_FAKE_H

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @defgroup RTEMSTestSuiteTestsTFTPFS Test suite for libtftpsfs tests
 *
 * @ingroup RTEMSTestSuitesFilesystem
 *
 * @brief This test suite provides a tests for libtftpfs.
 *
 * There are some additional files relevant for the TFTP test suite:
 *
 * - `spec/build/testsuites/fstests/grp.yml`\n
 *   This file specifies how the RTEMS WAF build system has to compile, link
 *   and install all filesystem test suites.  The TFTP test suite must
 *   be mentioned in this file to be build.
 *
 * - `spec/build/testsuites/fstests/tftpfs.yml`\n
 *   This file specifies how the RTEMS WAF build system has to compile, link
 *   and install the TFTP test suite.
 *
 * - `Doxygen`\n
 *    At variable `INPUT` the test suite is included to be processed by the
 *    Doxygen documentation generator.
 *
 * See also the _RTEMS Filesystem Design Guide_ Chapter _Trivial FTP Client
 * Filesystem_.
 *
 * @{
 */

#define TFTP_STD_PORT               69
#define TFTP_MAX_IP_ADDR_STRLEN     (16 * 2 + 7 + 1)
#define TFTP_MAX_ERROR_STRLEN       20
#define TFTP_MAX_OPTIONS_SIZE       40

/**
 * @brief IP address strings and server names resolved by network fake
 *   functions like inet_aton() and gethostbyname().
 */
#define TFTP_KNOWN_IPV4_ADDR0_STR   "127.0.0.1"
#define TFTP_KNOWN_IPV4_ADDR0_ARRAY 127, 0, 0, 1
#define TFTP_KNOWN_SERVER0_NAME     "server.tftp"
#define TFTP_KNOWN_SERVER0_IPV4     "10.7.0.2"
#define TFTP_KNOWN_SERVER0_ARRAY    10, 7, 0, 2

/**
 * @brief The faked socket() function (i.e. socket interaction) must return
 *   a file descriptor equal or larger than @c TFTP_FIRST_FD
 *   or -1.
 */
#define TFTP_FIRST_FD 33333

typedef enum Tftp_Action_kind {
  TFTP_IA_KIND_SOCKET,
  TFTP_IA_KIND_CLOSE,
  TFTP_IA_KIND_BIND,
  TFTP_IA_KIND_SENDTO,
  TFTP_IA_KIND_RECVFROM
} Tftp_Action_kind;

typedef struct Tftp_Action {
  Tftp_Action_kind kind;
  union {
    struct {
      int domain;
      int type;
      int protocol;
      int result;
    } socket;
    struct {
      int fd;
      int result;
    } close;
    struct {
      int fd;
      int family;
      uint16_t port;
      const char *addr_str;
      int result;
    } bind;
    struct {
      int fd;
      const void *buf;
      size_t len;
      int flags;
      uint16_t dest_port;
      const char *dest_addr_str;
      int addrlen;
      ssize_t result;
    } sendto;
    struct {
      int fd;
      void *buf;
      size_t len;
      int flags;
      uint32_t timeout_ms;
      uint16_t src_port;
      char src_addr_str[TFTP_MAX_IP_ADDR_STRLEN];
      int addrlen;
      ssize_t result;
    } recvfrom;
  } data;
} Tftp_Action;

/**
 * @brief Carry out interactions with TFTP client.
 *
 * @c Tftp_Interaction_fn() is called to
 *
 *   * check that the fake network function has been called with the expected
 *     arguments (in @c act)
 *   * define values which shall be returned (to be stored in @c act)
 *
 * The function should not call @c T_assert_*() but use @c T_*().
 *  Otherwise, it is unlikely that the test can terminate the client in
 *  @c teardown().
 *
 * @param[in,out] act The actual arguments provided by the TFTP client
 *   to the network function.  Moreover, storage to store the results
 *   to be returned to the TFTP client.
 * @param data Arbitrary data area allocated when the interaction is created
 *   by @c _Tftp_Append_interaction()
 *
 * @retval true if the client behaved as expected.
 * @retval false if the test shall fail.
 */
typedef bool (*Tftp_Interaction_fn)( Tftp_Action *act, void *data );
typedef struct Tftp_Interaction Tftp_Interaction;
typedef struct Tftp_Interaction {
  Tftp_Interaction *next;
  Tftp_Action_kind kind;
  Tftp_Interaction_fn fn;
  void *data[0];
} Tftp_Interaction;

/**
 * @brief Initialize and free the singleton control object.
 *
 * Invoke @c _Tftp_Reset() in @c setup() and @c teardown() of the test.
 */
void _Tftp_Reset( void );

/**
 * @brief Create an interaction and append it to the sequence of expected
 *   interactions.
 *
 * This allocates memory for an interaction and additional specific data
 * for the function @c fn() parameter @c data.  The interaction is
 * initialized and appended at the end of the sequence of expected interactions.
 * If an error occurs a @c T_assert_*() macro is called. Hence, this function
 * never returns @c NULL.
 *
 * @param kind Defines which interaction is expected.  Note that it cannot
 *   happen that @c fn is called for a different network function.
 * @param fn A function which is called to handle the interaction.
 *   See @c Tftp_Interaction_fn()
 * @param size The size of a memory area which is given to @c fn() as
 *   @c data argument when it is invoked.  This can be used to provide
 *   private data to the function.
 *
 * @return A pointer to a memory area of size @c size.  The same pointer
 *   will be provided to @c fn as argument @c data when invoked.
 */
void *_Tftp_Append_interaction(
  Tftp_Action_kind kind,
  Tftp_Interaction_fn fn,
  size_t size
);


/**
 * @brief Have all queued interactions been processed?
 *
 * At the end of a test, it should be checked whether all queued interactions
 * have been consumed by the TFTP client.
 *
 * @retval true All queued interactions have been processed.
 * @retval false At least one queued interactions has not yet been processed.
 */
bool _Tftp_Has_no_more_interactions( void );

/*
 * TFTP details from RFC1350, RFC2347, RFC2348 and RFC7440
 *
 * Note: The RFCs require modes and options to be case in-sensitive.
 */

#define TFTP_MODE_NETASCII     "netascii"
#define TFTP_MODE_OCTET        "octet"
#define TFTP_OPTION_BLKSIZE    "blksize"
#define TFTP_OPTION_TIMEOUT    "timeout"
#define TFTP_OPTION_TSIZE      "tsize"
#define TFTP_OPTION_WINDOWSIZE "windowsize"

#define TFTP_WINDOW_SIZE_MIN   1
#define TFTP_BLOCK_SIZE_MIN    8
#define TFTP_BLOCK_SIZE_MAX    65464

typedef enum Tftp_Opcode {
  TFTP_OPCODE_RRQ   = 1,
  TFTP_OPCODE_WRQ   = 2,
  TFTP_OPCODE_DATA  = 3,
  TFTP_OPCODE_ACK   = 4,
  TFTP_OPCODE_ERROR = 5,
  TFTP_OPCODE_OACK  = 6,
} Tftp_Opcode;

typedef enum Tftp_Error_code {
  TFTP_ERROR_CODE_NOT_DEFINED = 0,
  TFTP_ERROR_CODE_NOT_FOUND   = 1,
  TFTP_ERROR_CODE_NO_ACCESS   = 2,
  TFTP_ERROR_CODE_DISK_FULL   = 3,
  TFTP_ERROR_CODE_ILLEGAL     = 4,
  TFTP_ERROR_CODE_UNKNOWN_ID  = 5,
  TFTP_ERROR_CODE_FILE_EXISTS = 6,
  TFTP_ERROR_CODE_NO_USER     = 7,
  TFTP_ERROR_CODE_OPTION_NEGO = 8,
} Tftp_Error_code;

typedef struct Tftp_Packet {
  uint16_t opcode;
  union {
    struct {
      char opts[0];
    } rrq;
    struct {
      char opts[0];
    } wrq;
    struct {
      uint16_t block_num;
      uint8_t bytes[0];
    } data;
    struct {
      uint16_t block_num;
    } ack;
    struct {
      uint16_t error_code;
      char err_msg[0];
    } error;
    struct {
      char opts[0];
    } oack;
  } content;
} Tftp_Packet;

/**
 * @brief Provides a human readable description for an error code from an TFTP
 *   error packet.
 *
 * @param error_code The error code from the TFTP error packet in host byte
 *   order.
 *
 * @return A pointer to a string describing the error.  If the error code is
 *   unknown, a pointer to "Unknown error code" is returned.  Do not change the
 *   the string as a pointer to the very same string will be returned by future
 *   calls.
 */
const char *_Tftp_Get_error_str( uint16_t error_code );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TFTPFS_UDP_NETWORK_FAKE_H */
