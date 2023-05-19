/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplTFTPFS
 *
 * @brief This header file provides interfaces and functions used to
 *   implement the TFTP file system.
 *
 * This file declares the public functions of the Trivial File
 * Transfer Protocol (TFTP) file system.
 */

/*
 * Copyright (C) 1998 W. Eric Norum <eric@norum.ca>
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

#ifndef _RTEMS_TFTP_H
#define _RTEMS_TFTP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <rtems/fs.h>

/**
 * @brief Do not call directly, use mount().
 *
 * @ingroup RTEMSImplTFTPFS
 *
 * Filesystem Mount table entry.
 */
int rtems_tftpfs_initialize(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void *data
);

/**
 * @defgroup RTEMSAPITFTPFS Trivial File Transfer Protocol (TFTP) API
 *
 * @ingroup RTEMSAPIIO
 *
 * @brief The TFTP client library provides an API to read files from and
 *   to write files to remote servers using the Trivial File Transfer
 *   Protocol (TFTP).
 *
 * See the _RTEMS Filesystem Design Guide_ Chapter _Trivial FTP Client
 * Filesystem_.
 *
 * Usage as TFTP File System
 * =========================
 *
 * To open `/bootfiles/image` on `hostname` for reading:
 *
 *     fd = open ("/TFTP/hostname:bootfiles/image", O_RDONLY);
 *
 * The `TFTP` is the mount path and the `hostname` must be
 *
 * + an IPv4 address (like `127.0.0.1`) or
 * + the (full-qualified) name of an IPv4 host (acceptable to
 *  `gethostbyname()`)
 *
 * IPv6 is currently not supported. `bootfiles/image` is a path on the
 * TFTP server `hostname` where the file (here `image`) can be found.
 *
 * Usage of TFTP Client
 * ====================
 *
 * The pseudo-code below shows the principal usage for reading a file.
 *
 * @code
 *   int res;
 *   ssize_t bytes;
 *   void *tftp_handle;
 *   tftp_net_config config;
 *   const size_t buffer_size = 4000;
 *   char data_buffer[buffer_size];
 *
 *   tftp_initialize_net_config( &config );
 *   config.options.window_size = 1; // Set desired config values
 *
 *   res = tftp_open(
 *     "127.0.0.1",
 *     "filename.txt",
 *     true, // is_for_reading
 *     &config,
 *     &tftp_handle
 *   );
 *
 *   if ( res != 0 || tftp_handle == NULL ) {
 *     // Error
 *   }
 *
 *   // Use tftp_read() (probably in a loop) ...
 *   bytes = tftp_read(
 *     tftp_handle,
 *     data_buffer,
 *     buffer_size
 *   );
 *   // ... or use tftp_write() instead when the file is open for writing.
 *
 *   res = tftp_close( tftp_handle );
 *
 *   if ( res != 0 ) {
 *     // Error ... check! Especially when writing!
 *   }
 * @endcode
 *
 * @{
 */

/*
 * The functions below use of the TFTP client library standalone
 * - i.e. without going through the file system.
 */

/**
 * @brief This block size meets RFC 1350 and avoids the sending of
 *   the `blksize` option to the TFTP server.
 */
#define TFTP_RFC1350_BLOCK_SIZE 512

/**
 * @brief This window size avoids the sending of the `windowsize`
 *   option to the TFTP server.
 *
 * This effectively mimics the operation defined in RFC 1350 which
 * does not know any window size.
 */
#define TFTP_RFC1350_WINDOW_SIZE 1

/**
 * @brief This block size is suggested in RFC 2348 and is used as
 *   default if no different block size is provided.
 */
#define TFTP_DEFAULT_BLOCK_SIZE 1456

/**
 * @brief This window size is suggested in RFC 2348 and is used as
 *   default if no different window size is provided.
 */
#define TFTP_DEFAULT_WINDOW_SIZE 8

/**
 * @brief This structure represents TFTP options negotiated between
 *   client and server.
 *
 * RFC 2347 is the basis for the TFTP option.
 */
typedef struct tftp_options {
  /**
   * @brief This member represents the desired size of a data block.
   *
   * The TFTP blocksize option is introduced in RFC 2348.  It defines the
   * number of octets in the data packets transferred.  Valid values
   * range between 8 and 65464 octets, inclusive.  Values larger
   * than 1468 may cause packet fragmentation over standard Ethernet.
   * A value of 512 will prevent this option from being sent to
   * the server.
   *
   * The default value is 1456.
   */
  uint16_t block_size;

  /**
   * @brief This member represents the desired size of a window.
   *
   * The TFTP windowsize option is introduced in RFC 7440.  It defines the
   * number of data packets send before the receiver must send an
   * acknowledgment packet.  Valid values range between 1 and 65535
   * packets, inclusive.  Simple TFTP servers usually do not support this
   * option.  This option may negatively contribute to network
   * congestion.  This can be avoided by using a window size of 1.
   * A value of 1 will prevent this option from being sent to
   * the server.
   *
   * The default value is 8.
   */
  uint16_t window_size;
} tftp_options;

/**
 * @brief This structure represents configuration value used by the TFTP
 *   client.
 *
 * As defaults the values suggested in RFC 7440 are used.
 */
typedef struct tftp_net_config {
  /**
   * @brief This member defines how many attempts are made to send a
   *   network packet to the server.
   *
   * Repetitions occur when the server does not response to a packet
   * send by the client within a timeout period.  When the here defined
   * number of repetitions is reached and the server does still not
   * respond, the connection is considered broken and the file transfer
   * is ended with an error.
   *
   * The default value is 6.
   */
  uint16_t retransmissions;

  /**
   * @brief This member defines the port on which the server is listening
   *   for incoming connections.
   *
   * The default port number is 69.
   */
  uint16_t server_port;

  /**
   * @brief This member defines the maximum time in milliseconds the
   *   client waits for an answer packet from the server.
   *
   * If the time out is exceeded, the client will re-transmit the last
   * packet it send to the server.  In case @c window_size is larger one,
   * several packets may subject to re-transmission.
   *
   * Note that this timeout applies only after the first re-transmission
   * of a packet. The timeout till the first re-transmission is
   * @c first_timeout.
   *
   * The default value is 1000ms.
   */
  uint32_t timeout;

  /**
   * @brief This member defines the maximum time in milliseconds the
   *   client waits for the first answer packet from the server.
   *
   * The @c first_timeout is used instead of the regular @c timeout
   * for the first wait-period directly after the client sends a packet
   * for the first time to the server.  That is, this is the timeout
   * of the first re-transmission.  For any following re-transmissions
   * of the current packet the regular @c timeout is used.
   *
   * The default value is 400ms.
   */
  uint32_t first_timeout;

  /**
   * @brief This member represents the options to be sent to the server.
   *
   * These option values are sent to the server.  Yet, the server may
   *
   *   + ignore one or all options
   *   + ask the client to use a different value for an option
   *   + reject the whole request with an error
   *
   * If the server rejects a request with options, the current client
   * implementation will automatically send a second request without
   * options.  Hence, the user should be aware that the actual file
   * transfer may not use the option values specified here.
   */
  tftp_options options;
} tftp_net_config;

/**
 * @brief Set all members of a @c tftp_net_config structure to their
 *   default values.
 *
 * @param config references a @c tftp_net_config structure.
 *   The values are set to the defaults defined in
 *   @ref tftp_net_config "`type tftp_net_config`".
 */
void tftp_initialize_net_config(
  tftp_net_config *config
);

/**
 * @brief Opens and starts a TFTP client session to read or write a
 *   single file.
 *
 * This directive resolves the hostname or IP address, establishes a connection
 * to the TFTP server and initiates the data transfer.  It will not return
 * before an error is encountered or the TFTP server has responded to the
 * read or write request with a network packet.
 *
 * TFTP uses timeouts (of unspecified length).  It does not know keep-alive
 * messages.  If the client does not respond to the server in due time,
 * the server sets the connection faulty and drops it.  To avoid this
 * the user of this code must read or write enough data fast enough.
 *
 * "Enough data" means at least so much data which fills a single data
 * packet or all packets of a window if windows are used.  The data
 * can be read or written in anything from one single large chunk to
 * byte-by-byte pieces.  The point is, one cannot pause the reading
 * or writing for longer periods of time.
 *
 * @param hostname is the IPv4 address as string or the name of the TFTP
 *   server to connect to.
 * @param path is the pathname at the TFTP server side of the file to
 *   read or write.  According to RFC 1350 the path must be in
 *   NETASCII.  This is ASCII as defined in "USA Standard Code for
 *   Information Interchange" with the modifications specified in "Telnet
 *   Protocol Specification".
 * @param is_for_reading indicated whether the file is to be read or written.
 *   A value of @c true indicates that the file is intended to be read from
 *   the server.  A value of @c false indicates that the file is to be
 *   written to the server.
 * @param config either references a structure defining the configuration
 *   values for this file transfer or is @c NULL.  If it is @c NULL, default
 *   configuration values are used.  See @ref tftp_net_config
 *   "type tftp_net_config" for a description and the defaults values.
 *   This function copies the data so that the memory pointed to by
 *   @c config can be used for other purposes after the call returns.
 * @param[out] tftp_handle references a place where a handle of the connection
 *   can be stored.  On success a pointer to a handle is stored.  On failure
 *   (return value other than 0) a @c NULL pointer is stored.  This handle
 *   must be provided to all further calls to @c tftp_read(),
 *   @c tftp_write(), and @c tftp_close().
 *
 *   When this directive stores a non-NULL pointer in this place, a call
 *   to @c tftp_close() is mandatory to release allocated resources.
 *   This parameter cannot be @c NULL.
 *
 * @retval 0 When the client session was opened successfully.
 * @return Returns a POSIX @c errno value in case an error occurred.
 */
int tftp_open(
  const char  *hostname,
  const char  *path,
  bool   is_for_reading,
  const tftp_net_config *config,
  void **tftp_handle
);

/**
 * @brief Read data from a TFTP server.
 *
 * This directive attempts to read data from a TFTP connection open for
 * reading.
 *
 * Upon success, the buffer is always filled with @c count bytes of received
 * data with the exception when the end of the file has been reached.
 *
 * TFTP cannot recover from errors.  Once an error is reported, the
 * connection must be and can only be closed.
 *
 * @param tftp_handle is the reference returned by a call to tftp_open().
 *   The file must be opened for reading.
 * @param[out] buffer references a memory area into which the received
 *   data is written.
 * @param count defines the size of the @c buffer in bytes.
 *
 * @retval 0 The end of the file has been reached.  There is no more data.
 * @return If greater or equal to 0, returns the number of bytes written
 *   into the buffer.  If the return value is negative, an error occurred.
 *   In this case the negated value is a POSIX @c errno value.
 */
ssize_t tftp_read(
  void   *tftp_handle,
  void   *buffer,
  size_t  count
);

/**
 * @brief Write data to a TFTP server.
 *
 * This directive attempts to write data to a TFTP connection open for
 * writing.
 *
 * On a successful call, all data in the @c buffer will be used.  Yet, this
 * does not imply that all data is actually sent.  This depends on
 * whether a whole data packet or window can be filled.
 *
 * TFTP cannot recover from errors.  Once an error is reported, the connection
 * must be and can only be closed.
 *
 * @param tftp_handle is the reference returned by a call to tftp_open().
 *   The file must be opened for writing.
 * @param buffer references a memory area which contains the data to be
 *   sent.
 * @param count defines the size of the data in @c buffer in bytes.
 *
 * @return If greater or equal to 0, returns the number of bytes used
 *   from the buffer.  The value is always @c count on a successful call.
 *   If the return value is negative, an error occurred.  In this case
 *   the negated value is a POSIX @c errno value.
 */
ssize_t tftp_write(
  void       *tftp_handle,
  const void *buffer,
  size_t      count
);

/**
 * @brief Close a TFTP client connection.
 *
 * This directive sends all data which are still stored in a write buffer
 * to the server (if any), tells the server that the connection ends (if
 * required by RFC 1350) and releases any resources allocated at the
 * client side.
 *
 * @note Especially, when writing a file to the server, the return
 * code of `tftp_close()` should be checked.  Invoking
 * `tftp_close()` triggers the sending of the last -- not
 * completely filled -- data block.  This may fail the same way as any
 * `tftp_write()` may fail.  Therefore, an error returned by
 * `tftp_close()` likely indicates that the file was not
 * completely transferred.
 *
 * @param tftp_handle is the reference returned by a call to tftp_open().
 *   If this parameter is @c NULL, the directive call is a no-op.
 *
 * @retval 0 When the client session was closed successfully.
 * @return Returns a POSIX @c errno value in case an error occurred.
 */
int tftp_close(
  void *tftp_handle
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
