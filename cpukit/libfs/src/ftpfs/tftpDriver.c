/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplTFTPFS
 *
 * @brief This source file contains the implementation of
 *   a Trivial File Transfer Protocol (TFTP) client library.
 *
 * The code in this file provides the ability to read files from and
 * to write files to remote servers using the Trivial File Transfer
 * Protocol (TFTP). It is used by the @ref tftpfs.c "TFTP file system" and
 * tested through its test suite. The
 * following RFCs are implemented:
 *
 *   + RFC 1350 "The TFTP Protocol (Revision 2)"
 *   + RFC 2347 "TFTP Option Extension"
 *   + RFC 2348 "TFTP Blocksize Option"
 *   + RFC 7440 "TFTP Windowsize Option"
 */

/*
 * Copyright (C) 1998 W. Eric Norum <eric@norum.ca>
 * Copyright (C) 2012, 2022 embedded brains GmbH & Co. KG
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

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <rtems.h>
#include <rtems/tftp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "tftp_driver.h"

/*
 * Range of UDP ports to try
 */
#define UDP_PORT_BASE        3180

/*
 * Default limits
 */
#define PACKET_FIRST_TIMEOUT_MILLISECONDS  400L
#define TFTP_WINDOW_SIZE_MIN               1
#define TFTP_BLOCK_SIZE_MIN                8
#define TFTP_BLOCK_SIZE_MAX                65464

#define TFTP_BLOCK_SIZE_OPTION             "blksize"
#define TFTP_WINDOW_SIZE_OPTION            "windowsize"
#define TFTP_DECIMAL_BASE                  10

#define TFTP_DEFAULT_SERVER_PORT           69

/*
 * These values are suggested by RFC 7440.
 */
#define TFTP_RFC7440_DATA_RETRANSMISSIONS  6
#define TFTP_RFC7440_TIMEOUT_MILLISECONDS  1000

/*
 * TFTP opcodes
 */
#define TFTP_OPCODE_RRQ     1
#define TFTP_OPCODE_WRQ     2
#define TFTP_OPCODE_DATA    3
#define TFTP_OPCODE_ACK     4
#define TFTP_OPCODE_ERROR   5
#define TFTP_OPCODE_OACK    6

/*
 * TFTP error codes
 */
#define TFTP_ERROR_CODE_NOT_DEFINED 0
#define TFTP_ERROR_CODE_NOT_FOUND   1
#define TFTP_ERROR_CODE_NO_ACCESS   2
#define TFTP_ERROR_CODE_DISK_FULL   3
#define TFTP_ERROR_CODE_ILLEGAL     4
#define TFTP_ERROR_CODE_UNKNOWN_ID  5
#define TFTP_ERROR_CODE_FILE_EXISTS 6
#define TFTP_ERROR_CODE_NO_USER     7
#define TFTP_ERROR_CODE_OPTION_NEGO 8

/*
 * Special return values for process_*_packet() functions
 * (other return values are POSIX errors)
 */
#define GOT_EXPECTED_PACKET 0
#define GOT_DUPLICATE_OF_CURRENT_PACKET -1
#define GOT_OLD_PACKET -2
#define GOT_FIRST_OUT_OF_ORDER_PACKET -3

/*
 * Special argument value for getPacket()
 */
#define GET_PACKET_DONT_WAIT -1

/*
 * Special return value for prepare_*_packet_for_sending() functions
 * (other return values are the length of the packet to be send)
 */
#define DO_NOT_SEND_PACKET 0

#define PKT_SIZE_FROM_BLK_SIZE(_blksize) ((_blksize) + 2 * sizeof (uint16_t))
#define BLK_SIZE_FROM_PKT_SIZE(_pktsize) ((_pktsize) - 2 * sizeof (uint16_t))
#define MUST_SEND_OPTIONS(_options) (\
    (_options).block_size  != TFTP_RFC1350_BLOCK_SIZE || \
    (_options).window_size != TFTP_RFC1350_WINDOW_SIZE )

/*
 * Packets transferred between machines
 */
union tftpPacket {
    /*
     * RRQ/WRQ packet
     */
    struct tftpRWRQ {
        uint16_t      opcode;
        char          filename_mode[];
    } tftpRWRQ;

    /*
     * DATA packet
     */
    struct tftpDATA {
        uint16_t      opcode;
        uint16_t      blocknum;
        uint8_t       data[];
    } tftpDATA;

    /*
     * ACK packet
     */
    struct tftpACK {
        uint16_t      opcode;
        uint16_t      blocknum;
    } tftpACK;

    /*
     * OACK packet
     */
    struct tftpOACK {
        uint16_t      opcode;
        char          options[];
    } tftpOACK;

    /*
     * ERROR packet
     */
    struct tftpERROR {
        uint16_t      opcode;
        uint16_t      errorCode;
        char          errorMessage[];
    } tftpERROR;
};

/*
 * State of each TFTP stream
 */
struct tftpStream {
    /*
     * Buffer for storing packets for sending and receiving.  Can point
     * to the same address when only one buffer is needed for reading.
     */
    union tftpPacket *receive_buf;
    union tftpPacket *send_buf;

    /*
     * Current block number - i.e. the block currently send or received
     */
    uint16_t      blocknum;

    /*
     * Size of the data area in a DATA single packet.
     */
    size_t block_size;

    /*
     * The maximum size of a packet.  It depends linearly on the block_size.
     * The receive_buf and (the packets in) the send_buf are of this size.
     */
    size_t packet_size;

    /*
     * The number of packets which can be stored in the send buffer.
     * During option negotiation and for reading a file from the server
     * only a buffer for a single packet is needed.  In those cases, this
     * value is always one.  When a file is written to the server,
     * the value equals the window size:
     *     send_buf_size_in_pkts == server_options.window_size
     *
     * Packet N is stored in
     *     send_buf + packet_size * (N % send_buf_size_in_pkts)
     */
    uint16_t send_buf_size_in_pkts;

    /*
     * When writing files with windowsize > 1, the number of the completely
     * filled packet with the highest block number in the send buffer.
     * When the user calls write(), the data will be written into
     * the block after this one.
     */
    uint16_t blocknum_last_filled;

    /*
     * When writing files with windowsize > 1, the number of the packet
     * which is the last one in the whole file (i.e. the user
     * called close()).  This block is never full (but maybe empty).
     */
    uint16_t blocknum_eof_block;

    /*
     * Data transfer socket
     */
    int                 socket;
    struct sockaddr_in  myAddress;
    struct sockaddr_in  farAddress;

    /*
     * Indices into buffer
     * In case of sending a file with windowsize > 1, these values apply
     * only to the packet with the highest number in the send buffer
     * (blocknum_last_filled + 1).
     */
    int     nleft;
    int     nused;

    /*
     * Flags
     */
    int     firstReply;
    bool    at_eof;
    bool    is_for_reading;

    /*
     * Function pointers and members for use by communicate_with_server().
     */
    ssize_t (*prepare_packet_for_sending) (
        struct tftpStream *tp,
        bool force_retransmission,
        union tftpPacket **send_buf,
        bool *wait_for_packet_reception,
        const void *create_packet_data
    );
    int (*process_data_packet) (struct tftpStream *tp, ssize_t len);
    int (*process_ack_packet) (struct tftpStream *tp, ssize_t len);
    int (*process_oack_packet) (struct tftpStream *tp, ssize_t len);
    int (*process_error_packet) (struct tftpStream *tp, ssize_t len);
    int retransmission_error_code;
    bool ignore_out_of_order_packets;
    int32_t blocknum_of_first_packet_of_window;
    int error;

    /*
     * Configuration and TFTP options
     *
     *     * config.options are options desired by the user (i.e. the values
     *       send to the server).
     *     * server_options are the options agreed by the server
     *       (the option values actually used for the transfer of data).
     */
    tftp_net_config config;
    tftp_options server_options;
};

/*
 * Forward declaration cannot be avoided.
 */
static ssize_t prepare_data_packet_for_sending (
    struct tftpStream *tp,
    bool force_retransmission,
    union tftpPacket **send_buf,
    bool *wait_for_packet_reception,
    const void *path_name
);
static ssize_t prepare_ack_packet_for_sending (
    struct tftpStream *tp,
    bool force_retransmission,
    union tftpPacket **send_buf,
    bool *wait_for_packet_reception,
    const void *path_name
);

/*
 * Calculate the address of packet N in the send buffer
 */
static union tftpPacket *get_send_buffer_packet (
    struct tftpStream *tp,
    uint16_t packet_num
)
{
    return (union tftpPacket *) ( ( (char *) tp->send_buf) + tp->packet_size *
        (packet_num % tp->send_buf_size_in_pkts) );
}

/*
 * Create read or write request
 */
static size_t create_request (
    union tftpPacket *send_buf,
    size_t data_size,
    bool is_for_reading,
    const char *path,
    const tftp_options *options
)
{
  size_t res_size;
  char *cur = send_buf->tftpRWRQ.filename_mode;

  send_buf->tftpRWRQ.opcode = htons (
      is_for_reading ? TFTP_OPCODE_RRQ : TFTP_OPCODE_WRQ
  );

  res_size = snprintf (cur, data_size, "%s%c%s", path, 0, "octet");
  if (res_size >= data_size) {
     return -1;
  }
  res_size++;
  data_size -= res_size;
  cur += res_size;

  if (options->block_size != TFTP_RFC1350_BLOCK_SIZE) {
      res_size = snprintf (
          cur,
          data_size,
          "%s%c%"PRIu16,
          TFTP_BLOCK_SIZE_OPTION,
          0,
          options->block_size
      );
      if (res_size >= data_size) {
          return -1;
      }
      res_size++;
      data_size -= res_size;
      cur += res_size;
  }

  if (options->window_size != TFTP_RFC1350_WINDOW_SIZE) {
      res_size = snprintf (
          cur,
          data_size,
          "%s%c%"PRIu16,
          TFTP_WINDOW_SIZE_OPTION,
          0,
          options->window_size
      );
      if (res_size >= data_size) {
          return -1;
      }
      res_size++;
      data_size -= res_size;
      cur += res_size;
  }

  return cur - (char *)send_buf;
}

static bool parse_decimal_number (
    char **pos,
    size_t *remain,
    long min,
    long max,
    uint16_t *variable
)
{
    long value;
    const char *start = *pos;
    if (*remain < 2) {
        return false;
    }
    value = strtoul(start, pos, TFTP_DECIMAL_BASE);
    if (value < min || value > max || **pos != 0) {
        return false;
    }
    *variable = (uint16_t) value;
    (*pos)++;
    *remain -= *pos - start;

    return true;
}

/*
 * Map error message
 */
static int tftpErrno (uint16_t error_code)
{
    unsigned int tftpError;
    static const int errorMap[] = {
        EINVAL,
        ENOENT,
        EPERM,
        ENOSPC,
        EINVAL,
        ENXIO,
        EEXIST,
        ESRCH,
        ENOTSUP, /* Error: Option negotiation failed (RFC 2347) */
    };

    tftpError = ntohs (error_code);
    if (tftpError < (sizeof errorMap / sizeof errorMap[0]))
        return errorMap[tftpError];
    else
        return 1000 + tftpError;
}

/*
 * Parse options from an OACK packet
 */
static bool parse_options (
    union tftpPacket *receive_buf,
    size_t packet_size,
    tftp_options *options_in,
    tftp_options *options_out
)
{
    char *pos = receive_buf->tftpOACK.options;
    size_t remain   = packet_size - sizeof (receive_buf->tftpOACK.opcode);

    /*
     * Make sure there is a 0 byte in the end before comparing strings
     */
    if (remain > 0 && pos[remain - 1] != 0) {
        return false;
    }

    while (remain > 0) {
      if (strcasecmp(pos, TFTP_BLOCK_SIZE_OPTION) == 0 &&
          options_in->block_size != TFTP_RFC1350_BLOCK_SIZE) {
          remain -= sizeof (TFTP_BLOCK_SIZE_OPTION);
          pos    += sizeof (TFTP_BLOCK_SIZE_OPTION);
          if (!parse_decimal_number (
              &pos,
              &remain,
              TFTP_BLOCK_SIZE_MIN,
              options_in->block_size,
              &options_out->block_size)) {
              return false;
          };

      } else if (strcasecmp(pos, TFTP_WINDOW_SIZE_OPTION) == 0 &&
          options_in->window_size != TFTP_RFC1350_WINDOW_SIZE) {
          remain -= sizeof (TFTP_WINDOW_SIZE_OPTION);
          pos    += sizeof (TFTP_WINDOW_SIZE_OPTION);
          if (!parse_decimal_number (
              &pos,
              &remain,
              TFTP_WINDOW_SIZE_MIN,
              options_in->window_size,
              &options_out->window_size)) {
              return false;
          };

      } else {
         return false; /* Unknown option */
      }
    }

    return true;
}

/*
 * Send an error message
 */
static void send_error (
    struct tftpStream *tp,
    struct sockaddr_in *to,
    uint8_t error_code,
    const char *error_message
)
{
    int len;
    struct {
        uint16_t      opcode;
        uint16_t      errorCode;
        char          errorMessage[80];
    } msg;

    /*
     * Create the error packet (Unknown transfer ID).
     */
    msg.opcode = htons (TFTP_OPCODE_ERROR);
    msg.errorCode = htons (error_code);
    len = snprintf (msg.errorMessage, sizeof (msg.errorMessage), error_message);
    if (len >= sizeof (msg.errorMessage)) {
      len = sizeof (msg.errorMessage) - 1;
    }
    len += sizeof (msg.opcode) + sizeof (msg.errorCode) + 1;

    /*
     * Send it
     *
     * Ignoring result because error packets are sent once and maybe lost.
     */
    (void) sendto (tp->socket, (char *)&msg, len, 0, (struct sockaddr *)to, sizeof *to);
}

/*
 * Send a message to make the other end shut up
 */
static void sendStifle (struct tftpStream *tp, struct sockaddr_in *to)
{
    send_error (tp, to, TFTP_ERROR_CODE_UNKNOWN_ID, "GO AWAY");
}

/*
 * Wait for a packet
 */
static ssize_t
getPacket (struct tftpStream *tp, int retryCount)
{
    ssize_t len;
    struct timeval tv;
    int flags = 0;

    if (retryCount == GET_PACKET_DONT_WAIT) {
        flags = MSG_DONTWAIT;
    } else if (retryCount == 0) {
        tv.tv_sec = tp->config.first_timeout / 1000L;
        tv.tv_usec = (tp->config.first_timeout % 1000L) * 1000L;
        /*
         * Ignoring result because all possible errors indicate wrong
         * arguments and these arguments are OK as tested by test suite.
         */
        (void) setsockopt (tp->socket, SOL_SOCKET, SO_RCVTIMEO, &tv,
            sizeof tv);
    } else {
        tv.tv_sec = tp->config.timeout / 1000L;
        tv.tv_usec = (tp->config.timeout % 1000L) * 1000L;
        /*
         * Ignoring result because all possible errors indicate wrong
         * arguments and these arguments are OK as tested by test suite.
         */
        (void) setsockopt (tp->socket, SOL_SOCKET, SO_RCVTIMEO, &tv,
            sizeof tv);
    }
    for (;;) {
        union {
            struct sockaddr s;
            struct sockaddr_in i;
        } from;
        socklen_t fromlen = sizeof from;
        len = recvfrom (tp->socket,
            tp->receive_buf,
            tp->packet_size,
            flags,
            &from.s,
            &fromlen
        );
        if (len < 0)
            break;
        if (from.i.sin_addr.s_addr == tp->farAddress.sin_addr.s_addr) {
            if (tp->firstReply) {
                tp->firstReply = 0;
                tp->farAddress.sin_port = from.i.sin_port;
            }
            if (tp->farAddress.sin_port == from.i.sin_port)
                break;
        }

        /*
         * Packet is from someone with whom we are
         * not interested.  Tell them to go away.
         */
        sendStifle (tp, &from.i);
    }
    if (retryCount != GET_PACKET_DONT_WAIT) {
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        /*
         * Ignoring result because all possible errors indicate wrong
         * arguments and these arguments are OK as tested by test suite.
         */
        (void) setsockopt (tp->socket, SOL_SOCKET, SO_RCVTIMEO, &tv,
            sizeof tv);
    }
    return len;
}

static int process_unexpected_packet (struct tftpStream *tp, ssize_t len)
{
    (void) len;
    send_error (
        tp,
        &tp->farAddress,
        TFTP_ERROR_CODE_ILLEGAL,
        "Got packet with unexpected opcode from server"
    );
    return EPROTO;
}

static int process_malformed_packet (struct tftpStream *tp, ssize_t len)
{
    (void) len;
    send_error (
        tp,
        &tp->farAddress,
        TFTP_ERROR_CODE_ILLEGAL,
        "Got malformed packet from server"
    );
    return EPROTO;
}

static int process_error_packet (struct tftpStream *tp, ssize_t len)
{
    (void) len;
    return tftpErrno (tp->receive_buf->tftpERROR.errorCode);
}

/*
 * When an RRQ or a WRQ with options is sent and the server responds with
 * an error, this function will trigger a re-sent of an RRQ or WRQ
 * without options (falling back to old RFC1350).
 *
 * If someone wants to change the implementation to force using options
 * (i.e. prevent fallback to RFC1350), at least these points must be
 * considered:
 *
 *     * Use `process_error_packet()` instead of
 *       `process_error_packet_option_negotiation()`
 *     * React to DATA and ACK packets, which are an immediate response to
 *       an RRQ or a WRQ with options, with an error packet.
 *     * Check the option values in the OACK whether they are in the
 *       desired range.
 */
static int process_error_packet_option_negotiation (
    struct tftpStream *tp, ssize_t len
)
{
    (void) len;
    /*
     * Setting tp->config.options causes an RRQ or a WRQ to be created without
     * options.
     * Setting tp->server_option is defensive programming as these fields
     * should already have these values.
     */
    tp->config.options.block_size  = TFTP_RFC1350_BLOCK_SIZE;
    tp->config.options.window_size = TFTP_RFC1350_WINDOW_SIZE;
    tp->server_options.block_size  = TFTP_RFC1350_BLOCK_SIZE;
    tp->server_options.window_size = TFTP_RFC1350_WINDOW_SIZE;

    tp->process_error_packet = process_error_packet;
    /*
     * GOT_FIRST_OUT_OF_ORDER_PACKET will trigger a re-send of the RRQ or WRQ.
     */
    return GOT_FIRST_OUT_OF_ORDER_PACKET;
}

static int process_data_packet (struct tftpStream *tp, ssize_t len)
{
    ssize_t plen;
    int32_t pkt_blocknum;
    union tftpPacket *send_buf;

    if (len < sizeof (tp->receive_buf->tftpACK)) {
        return process_malformed_packet (tp, len);
    }
    pkt_blocknum = (int32_t) ntohs (tp->receive_buf->tftpACK.blocknum);
    if (pkt_blocknum == 0) {
        return process_malformed_packet (tp, len);
    }

    /*
     * In case of reading a file from the server:
     * If the latest ACK packet(s) did not reach the server, the server
     * starts the window from the last ACK it received.  This if-clause
     * ensures, the client sends an ACK after having seen `windowsize`
     * packets.
     */
    if (pkt_blocknum < tp->blocknum_of_first_packet_of_window &&
        pkt_blocknum >= (int32_t) tp->blocknum + 1 -
            (int32_t) tp->server_options.window_size) {
        tp->blocknum_of_first_packet_of_window = pkt_blocknum;
    }
    if (!tp->ignore_out_of_order_packets &&
       pkt_blocknum > (int32_t) tp->blocknum + 1) {
        tp->ignore_out_of_order_packets = true;
        return GOT_FIRST_OUT_OF_ORDER_PACKET;
    } else if (pkt_blocknum == (int32_t) tp->blocknum) {
      /*
       * In case of reading a file from the server:
       * If the last ACK packet send by the client did not reach the
       * server, the server re-sends all packets of the window.  In this
       * case, the client must re-send the ACK packet after having
       * received the last packet of the window (even through it has
       * already received that packet before).
       * GOT_OLD_PACKET would wrongly suppress this.
       */
      return GOT_DUPLICATE_OF_CURRENT_PACKET;
    } else if (pkt_blocknum != (int32_t) tp->blocknum + 1) {
        return GOT_OLD_PACKET;
    }
    tp->ignore_out_of_order_packets = false;

    tp->blocknum++;
    tp->nused = 0; /* Only for 2nd, 3rd, 4th DATA packet received */
    tp->nleft = BLK_SIZE_FROM_PKT_SIZE (len);
    tp->at_eof = (tp->nleft < tp->server_options.block_size);
    /*
     * After the last DATA packet, the client must send a final ACK
     */
    if (tp->at_eof) {
        plen = prepare_ack_packet_for_sending (tp, true, &send_buf, NULL, NULL);

        /*
         * Send it. Errors during send will not matter for this last ACK.
         */
        (void) sendto (
            tp->socket,
            send_buf,
            plen,
            0,
            (struct sockaddr *) &tp->farAddress,
            sizeof (tp->farAddress)
        );
    }
    tp->prepare_packet_for_sending = prepare_ack_packet_for_sending;
    return GOT_EXPECTED_PACKET;
}

static int process_ack_packet (struct tftpStream *tp, ssize_t len)
{
    uint16_t blocknum_ack;
    if (len < sizeof (tp->receive_buf->tftpACK)) {
        return process_malformed_packet (tp, len);
    }
    blocknum_ack = ntohs (tp->receive_buf->tftpACK.blocknum);
    if ((int32_t) blocknum_ack == tp->blocknum_of_first_packet_of_window - 1 &&
        blocknum_ack != 0
    ) {
        tp->blocknum = tp->blocknum_of_first_packet_of_window;
        return GOT_DUPLICATE_OF_CURRENT_PACKET;
    }
    if ((int32_t) blocknum_ack < tp->blocknum_of_first_packet_of_window ||
        blocknum_ack > tp->blocknum_last_filled) {
        return GOT_OLD_PACKET;
    }
    tp->blocknum = blocknum_ack + 1;
    tp->blocknum_of_first_packet_of_window = (int32_t) tp->blocknum;
    tp->prepare_packet_for_sending = prepare_data_packet_for_sending;
    return GOT_EXPECTED_PACKET;
}

static ssize_t prepare_data_packet_for_sending (
    struct tftpStream *tp,
    bool force_retransmission,
    union tftpPacket **send_buf,
    bool *wait_for_packet_reception,
    const void *not_used
)
{
    (void) not_used;
    ssize_t len;
    *send_buf = get_send_buffer_packet (tp, tp->blocknum);

    len = PKT_SIZE_FROM_BLK_SIZE (
        (tp->blocknum == tp->blocknum_eof_block) ? tp->nused : tp->block_size
    );
    (*send_buf)->tftpDATA.opcode   = htons (TFTP_OPCODE_DATA);
    (*send_buf)->tftpDATA.blocknum = htons (tp->blocknum);

    /*
     * If the client sends the last packet of a window,
     * it must wait for an ACK and - in case no ACK is received - begin
     * a retransmission with the first packet of the window.
     * Note that the last DATA block for the whole transfer is also
     * a "last packet of a window".
     */
    if ((int32_t) tp->blocknum + 1 >=
        tp->blocknum_of_first_packet_of_window + tp->send_buf_size_in_pkts ||
        tp->blocknum == tp->blocknum_eof_block) {
        tp->blocknum = (uint16_t) tp->blocknum_of_first_packet_of_window;
    } else {
        tp->blocknum++;
        *wait_for_packet_reception = false;
    }

    tp->process_data_packet  = process_unexpected_packet;
    tp->process_ack_packet   = process_ack_packet;
    tp->process_oack_packet  = process_unexpected_packet;
    tp->process_error_packet = process_error_packet;

    /*
     * Our last packet won't necessarily be acknowledged!
     */
    if (tp->blocknum == tp->blocknum_eof_block) {
        tp->retransmission_error_code = 0;
    }

    return len;
}

static ssize_t prepare_ack_packet_for_sending (
    struct tftpStream *tp,
    bool force_retransmission,
    union tftpPacket **send_buf,
    bool *wait_for_packet_reception,
    const void *path_name
)
{
    (void) wait_for_packet_reception;
    if (!force_retransmission &&
        tp->blocknum_of_first_packet_of_window - 1 +
        (int32_t) tp->server_options.window_size > (int32_t) tp->blocknum) {
      return DO_NOT_SEND_PACKET;
    }
    tp->blocknum_of_first_packet_of_window = (int32_t) tp->blocknum + 1;

    /*
     * Create the acknowledgement
     */
    *send_buf = tp->send_buf;
    (*send_buf)->tftpACK.opcode   = htons (TFTP_OPCODE_ACK);
    (*send_buf)->tftpACK.blocknum = htons (tp->blocknum);

    tp->process_data_packet  = process_data_packet;
    tp->process_ack_packet   = process_unexpected_packet;
    tp->process_oack_packet  = process_unexpected_packet;
    tp->process_error_packet = process_error_packet;

    return sizeof (tp->send_buf->tftpACK);
}

static int process_oack_packet (struct tftpStream *tp, ssize_t len)
{
    if (!parse_options(tp->receive_buf,
        len,
        &tp->config.options,
        &tp->server_options)) {
        send_error (
            tp,
            &tp->farAddress,
            TFTP_ERROR_CODE_OPTION_NEGO,
            "Bad options, option values or malformed OACK packet"
        );
        return EPROTO;
    }
    if (tp->is_for_reading) {
        /*
         * Since no DATA packet has been received yet, tell
         * tftp_read() there is no data left.
         */
        tp->nleft = 0;
        tp->prepare_packet_for_sending = prepare_ack_packet_for_sending;
    } else {
        tp->blocknum_of_first_packet_of_window = 1;
        tp->blocknum = (uint16_t) tp->blocknum_of_first_packet_of_window;
        tp->prepare_packet_for_sending = prepare_data_packet_for_sending;
    }
    return GOT_EXPECTED_PACKET;
}

static ssize_t prepare_request_packet_for_sending (
    struct tftpStream *tp,
    bool force_retransmission,
    union tftpPacket **send_buf,
    bool *wait_for_packet_reception,
    const void *path_name
)
{
    (void) wait_for_packet_reception;
    ssize_t len;
    *send_buf = tp->send_buf;
    len = create_request (
        *send_buf,
        tp->block_size,
        tp->is_for_reading,
        path_name,
        &tp->config.options
    );

    if (len < 0) {
        tp->error = ENAMETOOLONG;
    } else {
        tp->process_data_packet  = tp->is_for_reading ?
            process_data_packet : process_unexpected_packet;
        tp->process_ack_packet   = tp->is_for_reading ?
            process_unexpected_packet : process_ack_packet;
        tp->process_oack_packet  = MUST_SEND_OPTIONS(tp->config.options) ?
            process_oack_packet : process_unexpected_packet;
        tp->process_error_packet = MUST_SEND_OPTIONS(tp->config.options) ?
            process_error_packet_option_negotiation : process_error_packet;
    }

    /*
     * getPacket() will change these values when the first packet is
     * received.  Yet, this first packet may be an unexpected one
     * (e.g. an ERROR or having a wrong block number).
     * If a second, third, forth, ... RRQ/WRQ is to be sent, it should
     * be directed to the server port again and not to the port the
     * first unexpected packet came from.
     */
    tp->farAddress.sin_port = htons (tp->config.server_port);
    tp->firstReply          = 1;

    return len;
}

/*
 * Conduct one communication step with the server. For windowsize == 1,
 * one step is:
 *   a) Send a packet to the server
 *   b) Receive a reply packet from the server
 *   c) Handle errors (if any)
 *   d) If no packet has been received from the server and the maximum
 *      retransmission count has not yet been reached, start over with a)
 * The flow of packets (i.e. which packet to send and which packet(s) to
 * expect from the server) is controlled by function pointers found in
 * struct tftpStream.
 *
 * Besides of handling errors and retransmissions, the essential data exchange
 * follows these patterns:
 *
 * Connection establishment and option negotiation:
 *   * Send RRQ/WRQ (read or write request packet)
 *   * Receive OACK (read and write) or ACK (write only) or DATA (read only)
 *
 * Read step with windowsize == 1:
 *   * Send ACK packet
 *   * Receive DATA packet
 * Sending the very last ACK packet for a "read" session is treated as a
 * special case.
 *
 * Write step with windowsize == 1:
 *   * Send DATA packet
 *   * Receive ACK packet
 *
 * A windowsize lager than one makes thinks more complicated.
 * In this case, a step normally only receives (read) or only sends (write)
 * a packet.  The sending or receiving of the ACK packets is skipped normally
 * and happens only at the last step of the window (in which case this last
 * step is similar to the windowsize == 1 case):
 *
 * Normal read step with windowsize > 1:
 *   * Receive DATA packet
 * Last read step of a window with windowsize > 1:
 *   * Send ACK packet
 *   * Receive DATA packet
 *
 * Normal write step with windowsize > 1:
 *   * Send DATA packet
 *   * Check for an ACK packet but do not wait for it
 * Last write step of a window with windowsize > 1:
 *   * Send DATA packet
 *   * Receive ACK packet
 *
 * The "normal write step for windowsize > 1" checks whether an ACK packet
 * has been received after each sending of a DATA packet.  Package lost and
 * exchanges in the network can give rise to situations in which the server
 * sends more than a single ACK packet during a window.  If these packets
 * are not reacted on immediately, the network would be flooded with
 * surplus packets.  (An example where two ACK packets appear in a window
 * appears in test case "read_file_windowsize_trouble" where the client/server
 * roles are exchanged.)
 */
static int communicate_with_server (
    struct tftpStream    *tp,
    const void *create_packet_data
)
{
    ssize_t len;
    uint16_t opcode;
    union tftpPacket *send_buf;
    bool received_duplicated_or_old_package = false;
    bool force_retransmission = false;
    bool wait_for_packet_reception;
    int retryCount = 0;
    while (tp->error == 0) {

        if (!received_duplicated_or_old_package) {
            wait_for_packet_reception = true;
            len = tp->prepare_packet_for_sending (
                tp,
                force_retransmission,
                &send_buf,
                &wait_for_packet_reception,
                create_packet_data
                );
            if (len < 0) {
                if (tp->error == 0) {
                    tp->error = EIO;
                }
                break;
            }

            if (len != DO_NOT_SEND_PACKET) {
                /*
                 * Send the packet
                 */
                if (sendto (tp->socket, send_buf, len, 0,
                            (struct sockaddr *)&tp->farAddress,
                            sizeof tp->farAddress) < 0) {
                    tp->error = EIO;
                    break;
                }
            }
        }
        received_duplicated_or_old_package = false;
        force_retransmission = false;

        /*
         * Get reply
         */
        len = getPacket (
            tp,
            wait_for_packet_reception ? retryCount : GET_PACKET_DONT_WAIT
        );
        if (len >= (int) sizeof (tp->receive_buf->tftpDATA.opcode)) {
            opcode = ntohs (tp->receive_buf->tftpDATA.opcode);
            switch (opcode) {
            case TFTP_OPCODE_DATA:
                tp->error = tp->process_data_packet (tp, len);
                break;
            case TFTP_OPCODE_ACK:
                tp->error = tp->process_ack_packet (tp, len);
                break;
            case TFTP_OPCODE_OACK:
                tp->error = tp->process_oack_packet (tp, len);
                break;
            case TFTP_OPCODE_ERROR:
                tp->error = tp->process_error_packet (tp, len);
                break;
            default:
                tp->error = process_unexpected_packet (tp, len);
                break;
            }
            if (tp->error == GOT_EXPECTED_PACKET) {
                break;
            } else if (tp->error == GOT_DUPLICATE_OF_CURRENT_PACKET) {
                tp->error = 0;
            } else if (tp->error == GOT_OLD_PACKET) {
                received_duplicated_or_old_package = true;
                tp->error = 0;
            } else if (tp->error <= GOT_FIRST_OUT_OF_ORDER_PACKET) {
                force_retransmission = true;
                tp->error = 0;
            } /* else ... tp->error > 0 means "exit this function with error" */
        } else if (len >= 0) {
            tp->error = process_malformed_packet (tp, len);
        } else if (len < 0 && !wait_for_packet_reception) {
            tp->error = 0;
            break;
        } else {
            /*
             * Timeout or other problems to receive packets
             * Attempt a retransmission
             */
            if (++retryCount >= (int) tp->config.retransmissions) {
                tp->error = tp->retransmission_error_code;
                break;
            }
            force_retransmission = true;
        }
    }

    return tp->error;
}

/*
 * Allocate and initialize an struct tftpStream object.
 *
 * This function does not check whether the config values are in valid ranges.
 */
static struct tftpStream *create_stream(
    const tftp_net_config *config,
    const struct in_addr *farAddress,
    bool is_for_reading
)
{
    struct tftpStream *tp = NULL;
    tp = malloc (sizeof (struct tftpStream));
    if (tp == NULL) {
        return NULL;
    }

    /*
     * Initialize fields accessed by _Tftp_Destroy().
     */
    tp->receive_buf = NULL;
    tp->send_buf    = NULL;
    tp->socket      = 0;

    /*
     * Allocate send and receive buffer for exchange of RRQ/WRQ and ACK/OACK.
     */
    tp->block_size  = TFTP_RFC1350_BLOCK_SIZE;
    tp->packet_size = PKT_SIZE_FROM_BLK_SIZE (tp->block_size);
    tp->receive_buf = malloc (tp->packet_size);
    if (tp->receive_buf == NULL) {
      _Tftp_Destroy (tp);
      return NULL;
    }
    tp->send_buf = tp->receive_buf;
    tp->send_buf_size_in_pkts = 1;

    /*
     * Create the socket
     */
    if ((tp->socket = socket (AF_INET, SOCK_DGRAM, 0)) < 0) {
        _Tftp_Destroy (tp);
        return NULL;
    }

    /*
     * Setup configuration and options
     */
    if ( config == NULL ) {
      tftp_initialize_net_config (&tp->config);
    } else {
      tp->config = *config;
    }

    /*
     * If the server does not confirm our option values later on,
     * use numbers from the original RFC 1350 for the actual transfer.
     */
    tp->server_options.block_size  = TFTP_RFC1350_BLOCK_SIZE;
    tp->server_options.window_size = TFTP_RFC1350_WINDOW_SIZE;

    /*
     * Set the UDP destination to the TFTP server
     * port on the remote machine.
     */
    tp->farAddress.sin_family              = AF_INET;
    tp->farAddress.sin_addr                = *farAddress;
    tp->farAddress.sin_port                = htons (tp->config.server_port);

    tp->nleft                              = 0;
    tp->nused                              = 0;
    tp->blocknum                           = 0;
    tp->blocknum_last_filled               = 0;
    tp->blocknum_eof_block                 = UINT16_MAX;
    tp->firstReply                         = 1;
    tp->at_eof                             = false;
    tp->is_for_reading                     = is_for_reading;

    tp->prepare_packet_for_sending         = prepare_request_packet_for_sending;
    tp->process_data_packet                = process_unexpected_packet;
    tp->process_ack_packet                 = process_unexpected_packet;
    tp->process_oack_packet                = process_unexpected_packet;
    tp->process_error_packet               = process_error_packet;
    tp->retransmission_error_code          = EIO;
    tp->ignore_out_of_order_packets        = false;
    tp->blocknum_of_first_packet_of_window = INT32_MIN;
    tp->error                              = 0;

    return tp;
}

/*
 * Change the size of the receive and send buffer to match the options
 * values acknowledged by the server.
 */
static struct tftpStream *reallocate_stream_buffer(struct tftpStream *tp)
{
    tp->block_size = tp->server_options.block_size;
    tp->packet_size = PKT_SIZE_FROM_BLK_SIZE (tp->block_size);
    /*
     * Defensive programming
     */
    if (tp->receive_buf == tp->send_buf) {
        tp->send_buf = NULL;
    } else {
       free (tp->send_buf);
    }

    tp->receive_buf = realloc (tp->receive_buf, tp->packet_size);
    if (tp->is_for_reading) {
        tp->send_buf = tp->receive_buf;
    } else {
        tp->send_buf_size_in_pkts = tp->server_options.window_size;
        tp->send_buf = malloc (
            tp->send_buf_size_in_pkts * tp->packet_size
        );
    }

    if (tp->receive_buf == NULL || tp->send_buf == NULL) {
      sendStifle (tp,  &tp->farAddress);
      _Tftp_Destroy (tp);
      return NULL;
    }
    return tp;
}

/*
 * Convert hostname to an Internet address
 */
static struct in_addr *get_ip_address(
    const char     *hostname,
    struct in_addr *farAddress
)
{
        struct hostent *he = gethostbyname(hostname);
        if (he == NULL) {
            return NULL;
        }
        memcpy (farAddress, he->h_addr, sizeof (*farAddress));
  return farAddress;
}

void tftp_initialize_net_config (tftp_net_config *config)
{
  static const tftp_net_config default_config = {
    .retransmissions = TFTP_RFC7440_DATA_RETRANSMISSIONS,
    .server_port     = TFTP_DEFAULT_SERVER_PORT,
    .timeout         = TFTP_RFC7440_TIMEOUT_MILLISECONDS,
    .first_timeout   = PACKET_FIRST_TIMEOUT_MILLISECONDS,
    .options = {
      .block_size    = TFTP_DEFAULT_BLOCK_SIZE,
      .window_size   = TFTP_DEFAULT_WINDOW_SIZE
    }
  };

  if (config != NULL) {
    memcpy (config, &default_config, sizeof (default_config));
  }
}

int tftp_open(
    const char               *hostname,
    const char               *path,
    bool                      is_for_reading,
    const tftp_net_config    *config,
    void                    **tftp_handle
)
{
    struct tftpStream    *tp;
    struct in_addr       farAddress;
    int                  err;

    /*
     * Check parameters
     */
    if (tftp_handle == NULL) {
      return EINVAL;
    }
    *tftp_handle = NULL;
    if (hostname == NULL || path == NULL) {
        return EINVAL;
    }
    if (config != NULL && (
        config->options.window_size < TFTP_WINDOW_SIZE_MIN ||
        config->options.block_size  < TFTP_BLOCK_SIZE_MIN  ||
        config->options.block_size  > TFTP_BLOCK_SIZE_MAX  ) ) {
        return EINVAL;
    }

    /*
     * Create tftpStream structure
     */
    if (get_ip_address( hostname, &farAddress ) == NULL) {
        return ENOENT;
    }
    tp = create_stream( config, &farAddress, is_for_reading );
    if (tp == NULL) {
        return ENOMEM;
    }

    /*
     * Send RRQ or WRQ and wait for reply
     */
    tp->prepare_packet_for_sending = prepare_request_packet_for_sending;
    err = communicate_with_server (tp, path);
    if ( err != 0 ) {
        _Tftp_Destroy (tp);
        return err;
    }

    *tftp_handle = reallocate_stream_buffer ( tp );
    if( *tftp_handle == NULL ) {
        return ENOMEM;
    }

    return 0;
}

/*
 * Read from a TFTP stream
 */
ssize_t tftp_read(
    void          *tftp_handle,
    void          *buffer,
    size_t         count
)
{
    char              *bp;
    struct tftpStream *tp = tftp_handle;
    int               nwant;
    int               err;

    if (tp == NULL || !tp->is_for_reading || buffer == NULL)
        return -EIO;

    /*
     * Read till user request is satisfied or EOF is reached
     */
    bp = buffer;
    nwant = count;
    while (nwant) {
        if (tp->nleft) {
            int ncopy;
            if (nwant < tp->nleft)
                ncopy = nwant;
            else
                ncopy = tp->nleft;
            memcpy (bp, &tp->receive_buf->tftpDATA.data[tp->nused], ncopy);
            tp->nused += ncopy;
            tp->nleft -= ncopy;
            bp += ncopy;
            nwant -= ncopy;
            if (nwant == 0)
                break;
        }
        if (tp->at_eof) {
            break;
        }

        /*
         * Wait for the next packet
         */
        tp->retransmission_error_code = -EIO;
        err = communicate_with_server(tp, NULL);
        if (err == tp->retransmission_error_code) {
          return -EIO;
        }
        /*
         * If communicate_with_server() returns an error, either
         *    * an error message from the server was received or
         *    * an error message was already sent to the server
         * Setting tp->at_eof true, prevents all further calls to
         * communicate_with_server() and suppresses the sending of
         * an error message to the server by tftp_close().
         */
        if (err != 0) {
            tp->at_eof = true;
            return -err;
        }
    }
    return count - nwant;
}

/*
 * Flush a write buffer and wait for acknowledgement
 *
 * This function returns only if there is at least one packet buffer free
 * in the tp->send_buf.  This ensures that tftp_write() can store
 * further data for sending in this free packet buffer.
 *
 * When the end of file has been reached (i.e. tftp_close() called this
 * function), this function returns only after all packets
 * in the write buffer have been send and acknowledged (or if an error
 * occurred).
 */
static int rtems_tftp_flush (struct tftpStream *tp)
{
    int err;

    if (tp->at_eof) {
        return 0;
    }

    do {
        err = communicate_with_server(tp, NULL);
        /*
         * If communicate_with_server() returns an error, either
         *    * an error message from the server was received or
         *    * an error message was already sent to the server
         * Setting tp->at_eof true, prevents all further calls to
         * communicate_with_server() and suppresses the sending of
         * an error message to the server by tftp_close().
         */
        if (err != 0) {
            tp->at_eof = true;
            return err;
        }
    } while(
         (int32_t) tp->blocknum_last_filled + 1 >=
         tp->blocknum_of_first_packet_of_window + tp->send_buf_size_in_pkts ||
         /*
          * tp->blocknum_eof_block == tp->blocknum_last_filled
          * holds only true when the user invoked tftp_close().
          */
         (tp->blocknum_eof_block == tp->blocknum_last_filled &&
         tp->blocknum_of_first_packet_of_window <=
         (int32_t) tp->blocknum_eof_block)
    );

    return 0;
}

/*
 * Close a TFTP stream
 */
int tftp_close(
    void *tftp_handle
)
{
    struct tftpStream *tp = tftp_handle;
    int                e = 0;

    if (tp == NULL) {
        return 0;
    }

    if (!tp->is_for_reading) {
        tp->blocknum_last_filled++;
        tp->blocknum_eof_block = tp->blocknum_last_filled;
        e = rtems_tftp_flush (tp);
        tp->at_eof = true;
    }
    if (!tp->at_eof && !tp->firstReply) {
        /*
         * Tell the other end to stop
         */
        rtems_interval ticksPerSecond;
        send_error (
            tp,
            &tp->farAddress,
            TFTP_ERROR_CODE_NO_USER,
            "User (client) stopped reading or "
            "server stopped sending packets (timeout)"
        );
        ticksPerSecond = rtems_clock_get_ticks_per_second();
        rtems_task_wake_after (1 + ticksPerSecond / 10);
    }
    _Tftp_Destroy (tp);
    return e;
}

ssize_t tftp_write(
    void            *tftp_handle,
    const void      *buffer,
    size_t           count
)
{
    const char        *bp;
    struct tftpStream *tp = tftp_handle;
    int               nleft, nfree, ncopy;
    int               err;
    union tftpPacket *send_buf;

    /*
     * Bail out if an error has occurred
     */
    if (tp == NULL || tp->is_for_reading || tp->at_eof || buffer == NULL) {
        return -EIO;
    }

    /*
     * Write till user request is satisfied
     * Notice that the buffer is flushed as soon as it is filled rather
     * than waiting for the next write or a close.  This ensures that
     * the flush in close writes a less than full buffer so the far
     * end can detect the end-of-file condition.
     */
    bp = buffer;
    nleft = count;
    while (nleft) {
        nfree = tp->block_size - tp->nused;
        if (nleft < nfree)
            ncopy = nleft;
        else
            ncopy = nfree;
        send_buf = get_send_buffer_packet (tp, tp->blocknum_last_filled + 1);
        memcpy (&send_buf->tftpDATA.data[tp->nused], bp, ncopy);
        tp->nused += ncopy;
        nleft -= ncopy;
        bp += ncopy;
        if (tp->nused == tp->block_size) {
            tp->blocknum_last_filled++;
            err = rtems_tftp_flush (tp);
            if (err) {
                return -err;
            }
            tp->nused = 0;
        }
    }
    return count;
}

void _Tftp_Destroy(
    void *tftp_handle
)
{
    struct tftpStream *tp = tftp_handle;
    if (tp == NULL) {
        return;
    }

    if (tp->socket >= 0) {
        close (tp->socket);
    }

    if (tp->receive_buf == tp->send_buf) {
        tp->send_buf = NULL;
    }
    free (tp->receive_buf);
    free (tp->send_buf);
    free (tp);
}
