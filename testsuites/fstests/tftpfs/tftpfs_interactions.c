/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuiteTestsTFTPFS
 *
 * @brief This source file contains the implementation of network interaction
 *   functions related to the UDP network fake for tftpfs testing.
 *
 * The UDP Network Fake requires *interactions* between TFTP client and test
 * (which emulates a TFTP server).  The idea is that each test defines a
 * sequence of interactions.  In a successful test run all interactions must
 * be carried out one-by-one till the *last* interaction is reached.
 *
 * Interactions appear when the TFTP client calls functions like
 * sendto(), recvfrom(), or socket().  Here functions are defined
 * which
 *
 *   * handle such interactions and
 *   * permit the tests to easily defined the sequence of interactions.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h> /* sprintf() */
#include <inttypes.h> /* printf() macros like PRId8 */
#include <arpa/inet.h> /* ntohs() */
#include <rtems/test.h>


#include "tftpfs_interactions.h"
#include "tftpfs_udp_network_fake.h"

/*
 * Interaction: socket()
 */

typedef struct interaction_data_socket {
  int domain;
  int type;
  int protocol;
  int result;
} interaction_data_socket;

static bool interact_socket( Tftp_Action *act, void *data )
{
  interaction_data_socket *d = data;
  T_eq_int( act->data.socket.domain,   d->domain );
  T_eq_int( act->data.socket.type,     d->type );
  T_eq_int( act->data.socket.protocol, d->protocol );
  if (
    act->data.socket.domain   != d->domain ||
    act->data.socket.type     != d->type ||
    act->data.socket.protocol != d->protocol
  ) {
    return false;
  }

  act->data.socket.result = d->result;
  return true;
}

void _Tftp_Add_interaction_socket(
  int domain,
  int type,
  int protocol,
  int result
)
{
  interaction_data_socket *d;

  d = _Tftp_Append_interaction(
    TFTP_IA_KIND_SOCKET,
    interact_socket,
    sizeof( interaction_data_socket )
  );

  d->domain   = domain;
  d->type     = type;
  d->protocol = protocol;
  d->result   = result;
}

/*
 * Interaction: close()
 */

typedef struct interaction_data_close {
  int fd;
  int result;
} interaction_data_close;

static bool interact_close( Tftp_Action *act, void *data )
{
  interaction_data_close *d = data;
  T_eq_int( act->data.close.fd, d->fd );
  if ( act->data.close.fd != d->fd ) {
    return false;
  }

  act->data.close.result = d->result;
  return true;
}

void _Tftp_Add_interaction_close( int fd, int result )
{
  interaction_data_close *d;

  d = _Tftp_Append_interaction(
    TFTP_IA_KIND_CLOSE,
    interact_close,
    sizeof( interaction_data_close )
  );

  d->fd     = fd;
  d->result = result;
}

/*
 * Interaction: bind()
 */

typedef struct interaction_data_bind {
  int fd;
  int family;
  int result;
} interaction_data_bind;

static bool interact_bind( Tftp_Action *act, void *data )
{
  interaction_data_bind *d = data;
  T_eq_int( act->data.bind.fd, d->fd );
  T_eq_int( act->data.bind.family, d->family );
  if (
    act->data.bind.fd     != d->fd     ||
    act->data.bind.family != d->family
  ) {
    return false;
  }

  act->data.bind.result = d->result;
  return true;
}

void _Tftp_Add_interaction_bind( int fd, int family, int result )
{
  interaction_data_bind *d;

  d = _Tftp_Append_interaction(
    TFTP_IA_KIND_BIND,
    interact_bind,
    sizeof( interaction_data_bind )
  );

  d->fd     = fd;
  d->family = family;
  d->result = result;
}

/*
 * Interaction: sendto()
 */

#define TFTP_MAX_FILENAME_STRLEN 12

typedef struct interaction_data_sendto {
  int fd;
  uint16_t dest_port;
  char dest_addr_str[TFTP_MAX_IP_ADDR_STRLEN];
  bool result;
  union {
    struct {
      uint16_t opcode;
      char filename[TFTP_MAX_FILENAME_STRLEN];
      uint16_t block_size;
      uint16_t window_size;
    } rwrq;
    struct {
      uint16_t block_num;
    } ack;
    struct {
      uint16_t block_num;
      size_t start;
      size_t len;
      uint8_t (*get_data)( size_t pos );
    } data;
    struct {
      uint16_t error_code;
    } error;
  } content;
} interaction_data_sendto;

static bool check_filename_and_mode(
  const char **buf,
  size_t *max_len,
  const char *filename
)
{
  const char *str;
  size_t len;

  /* Make sure there is a 0 byte in the end before comparing strings */
  if ( *max_len <= 0 ) { return false; };
  T_quiet_eq_u8( *( *buf + *max_len - 1 ), '\0' );

  str = *buf;
  len = strlen( *buf ) + 1;
  *buf += len;
  *max_len -= len;
  if ( strcmp( str, filename ) != 0 ) {
    T_true( false, "Filename '%s' does not match '%s'", str, filename );
    return false;
  }
  if ( *max_len <= 0 ) {
    T_true( false, "Mode string missing." );
    return false;
  }

  str = *buf;
  len = strlen( *buf ) + 1;
  *buf += len;
  *max_len -= len;
  if ( strcasecmp( str, TFTP_MODE_OCTET ) != 0 ) {
    T_true( false, "Mode '%s' does not match '%s'", str, TFTP_MODE_OCTET );
    return false;
  }

  return true;
}

static bool check_for_option(
  const char **buf,
  size_t *max_len,
  const char *option_name,
  const char *option_value
)
{
  const char *str;
  size_t len;

  /* Make sure there is a 0 byte in the end before comparing strings */
  if ( *max_len <= 0 ) { return false; };
  T_quiet_eq_u8( *( *buf + *max_len - 1 ), '\0' );

  str = *buf;
  len = strlen( *buf ) + 1;
  if ( strcasecmp( str, option_name ) != 0 ) {
    return false;
  }
  *buf += len;
  *max_len -= len;

  if ( *max_len <= 0 ) {
    T_true( false, "Option value for '%s' missing.", option_name );
    return false;
  }

  str = *buf;
  len = strlen( *buf ) + 1;
  *buf += len;
  *max_len -= len;
  if ( strcmp( str, option_value ) != 0 ) {
    T_true(
      false,
      "Option '%s': Actual value '%s' does not match '%s'",
      option_name,
      str,
      option_value
    );
    return false;
  }

  return true;
}

static bool interact_sendto_common( Tftp_Action *act, void *data )
{
  interaction_data_sendto *d = data;
  const Tftp_Packet *pkt = act->data.sendto.buf;

  T_eq_int( act->data.sendto.fd,            d->fd );
  T_eq_int( act->data.sendto.flags,         0 );
  T_eq_u16( act->data.sendto.dest_port,     d->dest_port );
  T_eq_str( act->data.sendto.dest_addr_str, d->dest_addr_str );
  T_gt_sz(  act->data.sendto.len,           sizeof( pkt->opcode ) );
  if (
    act->data.sendto.fd        != d->fd ||
    act->data.sendto.flags     != 0 ||
    act->data.sendto.dest_port != d->dest_port ||
    strcmp( act->data.sendto.dest_addr_str, d->dest_addr_str ) != 0 ||
    act->data.sendto.len       <= sizeof( pkt->opcode )
  ) {
    return false;
  }

  act->data.sendto.result = d->result ? act->data.sendto.len : -1;
  return true;
}

static bool interact_sendto_rwrq( Tftp_Action *act, void *data )
{
  interaction_data_sendto *d = data;
  const Tftp_Packet *pkt = act->data.sendto.buf;
  size_t len = act->data.sendto.len - sizeof( pkt->opcode );
  const char *buf = pkt->content.rrq.opts;
  const char *tmp;
  char block_size_buffer[6];
  char window_size_buffer[6];

  if ( !interact_sendto_common( act, data ) ) {
    return false;
  }

  T_eq_u16( ntohs( pkt->opcode ), d->content.rwrq.opcode );
  if ( ntohs( pkt->opcode ) != d->content.rwrq.opcode ) { return false; }
  if ( !check_filename_and_mode( &buf, &len, d->content.rwrq.filename ) ) {
    return false;
  }

  snprintf(
    block_size_buffer,
    sizeof( block_size_buffer ),
    "%"PRIu16,
    d->content.rwrq.block_size
  );
  snprintf(
    window_size_buffer,
    sizeof( window_size_buffer ),
    "%"PRIu16,
    d->content.rwrq.window_size
  );

  for ( tmp = buf; len > 0; ) {
    if ( d->content.rwrq.block_size != NO_BLOCK_SIZE_OPTION &&
      check_for_option(
        &buf,
        &len,
        TFTP_OPTION_BLKSIZE,
        block_size_buffer )) {
        d->content.rwrq.block_size = NO_BLOCK_SIZE_OPTION;
    } else if ( d->content.rwrq.window_size != NO_WINDOW_SIZE_OPTION &&
      check_for_option(
        &buf,
        &len,
        TFTP_OPTION_WINDOWSIZE,
        window_size_buffer )) {
        d->content.rwrq.window_size = NO_WINDOW_SIZE_OPTION;
    } else {
      T_true( false, "Error at option '%s'", tmp );
      return false;
    }
  }

  T_eq_sz( len, 0 ); /* Check that all data till the end has been read */

  return true;
}

static bool interact_sendto_ack( Tftp_Action *act, void *data )
{
  interaction_data_sendto *d = data;
  const Tftp_Packet *pkt = act->data.sendto.buf;
  size_t pkt_size = sizeof( pkt->opcode ) + sizeof( pkt->content.ack );

  if ( !interact_sendto_common( act, data ) ) {
    return false;
  }

  T_eq_u16( ntohs( pkt->opcode ), TFTP_OPCODE_ACK );
  T_eq_sz( act->data.sendto.len, pkt_size );
  if ( ntohs( pkt->opcode ) != TFTP_OPCODE_ACK ||
    act->data.sendto.len != pkt_size
  ) {
    return false;
  }

  T_eq_u16( ntohs( pkt->content.ack.block_num ), d->content.ack.block_num );
  if ( ntohs( pkt->content.ack.block_num ) != d->content.ack.block_num ) {
    return false;
  }

  return true;
}

static bool interact_sendto_data( Tftp_Action *act, void *data )
{
  interaction_data_sendto *d = data;
  const Tftp_Packet *pkt = act->data.sendto.buf;
  size_t pkt_size = sizeof( pkt->opcode ) +
    sizeof( pkt->content.data.block_num ) + d->content.data.len;
  size_t i;

  if ( !interact_sendto_common( act, data ) ) {
    return false;
  }

  T_eq_u16( ntohs( pkt->opcode ), TFTP_OPCODE_DATA );
  T_eq_sz( act->data.sendto.len, pkt_size );
  if ( ntohs( pkt->opcode ) != TFTP_OPCODE_DATA ||
    act->data.sendto.len != pkt_size
  ) {
    return false;
  }

  T_eq_u16( ntohs( pkt->content.ack.block_num ), d->content.ack.block_num );
  if ( ntohs( pkt->content.ack.block_num ) != d->content.ack.block_num ) {
    return false;
  }

  for ( i = 0; i < d->content.data.len; ++i ) {
    if ( pkt->content.data.bytes[i] !=
      d->content.data.get_data( d->content.data.start + i ) ) {
    T_true(
      false,
      "Expected byte %02"PRIx8" but TFTP client sent %02"PRIx8
        " at position %zu",
      d->content.data.get_data( d->content.data.start + i ),
      pkt->content.data.bytes[i],
      d->content.data.start + i
      );
      return false;
    }
  }

  return true;
}

static bool interact_sendto_error( Tftp_Action *act, void *data )
{
  interaction_data_sendto *d = data;
  const Tftp_Packet *pkt = act->data.sendto.buf;
  size_t pkt_size = sizeof( pkt->opcode ) + sizeof( pkt->content.error );

  if ( !interact_sendto_common( act, data ) ) {
    return false;
  }

  T_eq_u16( ntohs( pkt->opcode ), TFTP_OPCODE_ERROR );
  T_gt_sz( act->data.sendto.len, pkt_size );
  if ( ntohs( pkt->opcode ) != TFTP_OPCODE_ERROR ||
    act->data.sendto.len <= pkt_size
  ) {
    return false;
  }

  T_eq_u16(
    ntohs( pkt->content.error.error_code ),
    d->content.error.error_code
  );
  if ( ntohs( pkt->content.error.error_code ) != d->content.error.error_code ) {
    return false;
  }

  return true;
}

static void add_interaction_send_rwrq(
  int fd,
  const char *filename,
  uint16_t dest_port,
  const char *dest_addr_str,
  uint16_t block_size,
  uint16_t window_size,
  bool result,
  uint16_t opcode
)
{
  interaction_data_sendto *d;

  d = _Tftp_Append_interaction(
    TFTP_IA_KIND_SENDTO,
    interact_sendto_rwrq,
    sizeof( interaction_data_sendto )
  );

  T_assert_lt_sz( strlen( filename ), TFTP_MAX_FILENAME_STRLEN );
  T_assert_lt_sz( strlen( dest_addr_str ), TFTP_MAX_IP_ADDR_STRLEN );
  strcpy( d->content.rwrq.filename, filename );
  strcpy( d->dest_addr_str, dest_addr_str );
  d->fd                       = fd;
  d->dest_port                = dest_port;
  d->content.rwrq.opcode      = opcode;
  d->content.rwrq.block_size  = block_size;
  d->content.rwrq.window_size = window_size;
  d->result                   = result;
}

void _Tftp_Add_interaction_send_rrq(
  int fd,
  const char *filename,
  uint16_t dest_port,
  const char *dest_addr_str,
  uint16_t block_size,
  uint16_t window_size,
  bool result
)
{
  add_interaction_send_rwrq(
    fd,
    filename,
    dest_port,
    dest_addr_str,
    block_size,
    window_size,
    result,
    TFTP_OPCODE_RRQ
  );
}

void _Tftp_Add_interaction_send_wrq(
  int fd,
  const char *filename,
  uint16_t dest_port,
  const char *dest_addr_str,
  uint16_t block_size,
  uint16_t window_size,
  bool result
)
{
  add_interaction_send_rwrq(
    fd,
    filename,
    dest_port,
    dest_addr_str,
    block_size,
    window_size,
    result,
    TFTP_OPCODE_WRQ
  );
}

void _Tftp_Add_interaction_send_ack(
  int fd,
  uint16_t block_num,
  uint16_t dest_port,
  const char *dest_addr_str,
  bool result
)
{
  interaction_data_sendto *d;

  d = _Tftp_Append_interaction(
    TFTP_IA_KIND_SENDTO,
    interact_sendto_ack,
    sizeof( interaction_data_sendto )
  );

  T_assert_lt_sz( strlen( dest_addr_str ), TFTP_MAX_IP_ADDR_STRLEN );
  strcpy( d->dest_addr_str, dest_addr_str );
  d->fd                    = fd;
  d->dest_port             = dest_port;
  d->result                = result;
  d->content.ack.block_num = block_num;
}

void _Tftp_Add_interaction_send_data(
  int fd,
  uint16_t block_num,
  size_t start,
  size_t len,
  uint8_t (*get_data)( size_t pos ),
  uint16_t dest_port,
  const char *dest_addr_str,
  bool result
)
{
  interaction_data_sendto *d;

  d = _Tftp_Append_interaction(
    TFTP_IA_KIND_SENDTO,
    interact_sendto_data,
    sizeof( interaction_data_sendto )
  );

  T_assert_lt_sz( strlen( dest_addr_str ), TFTP_MAX_IP_ADDR_STRLEN );
  strcpy( d->dest_addr_str, dest_addr_str );
  d->fd                     = fd;
  d->dest_port              = dest_port;
  d->result                 = result;
  d->content.data.block_num = block_num;
  d->content.data.start     = start;
  d->content.data.len       = len;
  d->content.data.get_data  = get_data;
}

void _Tftp_Add_interaction_send_error(
  int fd,
  uint16_t error_code,
  uint16_t dest_port,
  const char *dest_addr_str,
  bool result
)
{
  interaction_data_sendto *d;

  d = _Tftp_Append_interaction(
    TFTP_IA_KIND_SENDTO,
    interact_sendto_error,
    sizeof( interaction_data_sendto )
  );

  T_assert_lt_sz( strlen( dest_addr_str ), TFTP_MAX_IP_ADDR_STRLEN );
  strcpy( d->dest_addr_str, dest_addr_str );
  d->fd                       = fd;
  d->dest_port                = dest_port;
  d->result                   = result;
  d->content.error.error_code = error_code;
}

/*
 * Interaction: recvfrom()
 */

typedef struct interaction_data_recvfrom {
  int fd;
  uint32_t timeout_ms;
  uint16_t src_port;
  char src_addr_str[TFTP_MAX_IP_ADDR_STRLEN];
  bool result;
  union {
    struct {
      uint16_t block_num;
    } ack;
    struct {
      size_t options_size;
      char options[TFTP_MAX_OPTIONS_SIZE];
    } oack;
    struct {
      uint16_t block_num;
      size_t start;
      size_t len;
      uint8_t (*get_data)( size_t pos );
    } data;
    struct {
      uint16_t error_code;
      char err_msg[TFTP_MAX_ERROR_STRLEN];
    } error;
    struct {
      size_t len;
      uint8_t bytes[0];
    } raw;
  } content;
} interaction_data_recvfrom;

static bool interact_recvfrom_common(
  Tftp_Action *act,
  void *data,
  size_t actual_size
)
{
  interaction_data_recvfrom *d = data;

  T_eq_int( act->data.recvfrom.fd, d->fd );
  T_quiet_ge_sz( act->data.recvfrom.len, actual_size );
  if (
    act->data.recvfrom.fd != d->fd ||
    act->data.recvfrom.len < actual_size
  ) {
    return false;
  }
  if ( d->timeout_ms == DO_NOT_WAIT_FOR_ANY_TIMEOUT ) {
    T_ne_int( act->data.recvfrom.flags, 0 );
    if ( act->data.recvfrom.flags == 0 ) {
      return false;
    }
  } else {
    T_eq_int( act->data.recvfrom.flags, 0 );
    T_eq_u32( act->data.recvfrom.timeout_ms, d->timeout_ms );
    if (
      act->data.recvfrom.flags != 0 ||
      act->data.recvfrom.timeout_ms != d->timeout_ms
    ) {
      return false;
    }
  }

  strncpy(
    act->data.recvfrom.src_addr_str,
    d->src_addr_str,
    sizeof( act->data.recvfrom.src_addr_str )
  );
  act->data.recvfrom.src_addr_str[
    sizeof( act->data.recvfrom.src_addr_str ) - 1] = '\0';
  act->data.recvfrom.src_port = d->src_port;
  act->data.recvfrom.result = d->result ? actual_size : -1;
  return true;
}

static bool interact_recvfrom_data( Tftp_Action *act, void *data )
{
  interaction_data_recvfrom *d = data;
  Tftp_Packet *pkt = act->data.recvfrom.buf;
  size_t actual_size;
  size_t i;

  actual_size = sizeof( pkt->opcode ) +
    sizeof( pkt->content.data.block_num ) + d->content.data.len;
  if ( !interact_recvfrom_common( act, data, actual_size ) ) {
    return false;
  }

  pkt->opcode = htons( TFTP_OPCODE_DATA );
  pkt->content.data.block_num = htons( d->content.data.block_num );
  for ( i = 0; i < d->content.data.len; ++i ) {
    pkt->content.data.bytes[i] =
      d->content.data.get_data( d->content.data.start + i );
  }

  return true;
}

static bool interact_recvfrom_ack( Tftp_Action *act, void *data )
{
  interaction_data_recvfrom *d = data;
  Tftp_Packet *pkt = act->data.recvfrom.buf;
  size_t actual_size;

  actual_size = sizeof( pkt->opcode ) + sizeof( pkt->content.ack.block_num );
  if ( !interact_recvfrom_common( act, data, actual_size ) ) {
    return false;
  }

  pkt->opcode = htons( TFTP_OPCODE_ACK );
  pkt->content.ack.block_num = htons( d->content.ack.block_num );

  return true;
}

static bool interact_recvfrom_oack( Tftp_Action *act, void *data )
{
  interaction_data_recvfrom *d = data;
  Tftp_Packet *pkt = act->data.recvfrom.buf;
  size_t actual_size;

  actual_size = sizeof( pkt->opcode ) + d->content.oack.options_size;
  if ( !interact_recvfrom_common( act, data, actual_size ) ) {
    return false;
  }

  pkt->opcode = htons( TFTP_OPCODE_OACK );
  memcpy(
    pkt->content.oack.opts,
    d->content.oack.options,
    d->content.oack.options_size
  );

  return true;
}

static bool interact_recvfrom_error( Tftp_Action *act, void *data )
{
  interaction_data_recvfrom *d = data;
  Tftp_Packet *pkt = act->data.recvfrom.buf;
  size_t actual_size;

  actual_size = sizeof( pkt->opcode ) +
    sizeof( pkt->content.error.error_code ) +
    strlen( d->content.error.err_msg ) + 1;
  if ( !interact_recvfrom_common( act, data, actual_size ) ) {
    return false;
  }

  pkt->opcode = htons( TFTP_OPCODE_ERROR );
  pkt->content.error.error_code = htons( d->content.error.error_code );
  strncpy(
    pkt->content.error.err_msg,
    d->content.error.err_msg,
    act->data.recvfrom.len - sizeof( pkt->opcode ) -
      sizeof( pkt->content.error.error_code ) - 1
  );

  return true;
}

static bool interact_recvfrom_raw( Tftp_Action *act, void *data )
{
  interaction_data_recvfrom *d = data;
  uint8_t *pkt = act->data.recvfrom.buf;
  size_t actual_size = d->content.raw.len;

  if ( !interact_recvfrom_common( act, data, actual_size ) ) {
    return false;
  }

  memcpy( pkt, d->content.raw.bytes, actual_size );

  return true;
}

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
)
{
  interaction_data_recvfrom *d;

  d = _Tftp_Append_interaction(
    TFTP_IA_KIND_RECVFROM,
    interact_recvfrom_data,
    sizeof( interaction_data_recvfrom )
  );

  T_assert_lt_sz( strlen( src_addr_str ), sizeof( d->src_addr_str ) - 1 );
  strcpy( d->src_addr_str, src_addr_str );
  d->fd                     = fd;
  d->timeout_ms             = timeout_ms;
  d->src_port               = src_port;
  d->content.data.block_num = block_num;
  d->content.data.start     = start;
  d->content.data.len       = len;
  d->content.data.get_data  = get_data;
  d->result                 = result;
}

void _Tftp_Add_interaction_recv_ack(
  int fd,
  uint32_t timeout_ms,
  uint16_t src_port,
  const char *src_addr_str,
  uint16_t block_num,
  bool result
)
{
  interaction_data_recvfrom *d;

  d = _Tftp_Append_interaction(
    TFTP_IA_KIND_RECVFROM,
    interact_recvfrom_ack,
    sizeof( interaction_data_recvfrom )
  );

  T_assert_lt_sz( strlen( src_addr_str ), sizeof( d->src_addr_str ) - 1 );
  strcpy( d->src_addr_str, src_addr_str );
  d->fd                     = fd;
  d->timeout_ms             = timeout_ms;
  d->src_port               = src_port;
  d->content.ack.block_num  = block_num;
  d->result                 = result;
}

void _Tftp_Add_interaction_recv_oack(
  int fd,
  uint32_t timeout_ms,
  uint16_t src_port,
  const char *src_addr_str,
  const char *options,
  size_t options_size,
  bool result
)
{
  interaction_data_recvfrom *d;

  d = _Tftp_Append_interaction(
    TFTP_IA_KIND_RECVFROM,
    interact_recvfrom_oack,
    sizeof( interaction_data_recvfrom )
  );

  T_assert_lt_sz( strlen( src_addr_str ), sizeof( d->src_addr_str ) - 1 );
  strcpy( d->src_addr_str, src_addr_str );
  T_assert_lt_sz( options_size, sizeof( d->content.oack.options ) );
  memcpy( d->content.oack.options, options, options_size );
  d->fd                        = fd;
  d->timeout_ms                = timeout_ms;
  d->src_port                  = src_port;
  d->content.oack.options_size = options_size;
  d->result                    = result;
}

void _Tftp_Add_interaction_recv_error(
  int fd,
  uint32_t timeout_ms,
  uint16_t src_port,
  const char *src_addr_str,
  uint16_t error_code,
  const char *err_msg,
  bool result
)
{
  interaction_data_recvfrom *d;

  d = _Tftp_Append_interaction(
    TFTP_IA_KIND_RECVFROM,
    interact_recvfrom_error,
    sizeof( interaction_data_recvfrom )
  );

  T_assert_lt_sz( strlen( src_addr_str ), sizeof( d->src_addr_str ) - 1 );
  strcpy( d->src_addr_str, src_addr_str );
  T_assert_lt_sz( strlen( src_addr_str ), sizeof( d->src_addr_str ) - 1 );
  strcpy( d->content.error.err_msg, err_msg );
  d->fd                       = fd;
  d->timeout_ms               = timeout_ms;
  d->src_port                 = src_port;
  d->content.error.error_code = error_code;
  d->result                   = result;
}

void _Tftp_Add_interaction_recv_raw(
  int fd,
  uint32_t timeout_ms,
  uint16_t src_port,
  const char *src_addr_str,
  size_t len,
  const uint8_t *bytes,
  bool result
)
{
  interaction_data_recvfrom *d;

  d = _Tftp_Append_interaction(
    TFTP_IA_KIND_RECVFROM,
    interact_recvfrom_raw,
    sizeof( interaction_data_recvfrom ) + len
  );

  T_assert_lt_sz( strlen( src_addr_str ), sizeof( d->src_addr_str ) - 1 );
  strcpy( d->src_addr_str, src_addr_str );
  memcpy( d->content.raw.bytes, bytes, len );
  d->fd              = fd;
  d->timeout_ms      = timeout_ms;
  d->src_port        = src_port;
  d->content.raw.len = len;
  d->result          = result;
}

void _Tftp_Add_interaction_recv_nothing(
  int fd,
  uint32_t timeout_ms
)
{
  static const char *dummy_ip = "0.0.0.0";
  interaction_data_recvfrom *d;

  d = _Tftp_Append_interaction(
    TFTP_IA_KIND_RECVFROM,
    interact_recvfrom_ack,
    sizeof( interaction_data_recvfrom )
  );


  T_assert_lt_sz( strlen( dummy_ip ), sizeof( d->src_addr_str ) - 1 );
  strcpy( d->src_addr_str, dummy_ip );
  d->fd                     = fd;
  d->timeout_ms             = timeout_ms;
  d->src_port               = 0;
  d->content.ack.block_num  = 0;
  d->result                 = false;
}
