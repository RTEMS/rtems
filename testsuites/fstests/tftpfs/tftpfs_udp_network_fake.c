/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuiteTestsTFTPFS
 *
 * @brief This source file contains the implementation of UDP network fake
 *   functions related to tftpfs testing.
 *
 * The libtftpfs source code is situated in the RTEMS repository.  For
 * testing it, either libbsd or RTEMS legacy networking would be required.
 * This implies that the tests for libtftpfs would need to be placed in
 * the libbsd repository - a different one than the libtftpfs source code.
 *
 * Yet, libtftpfs uses only a handful of networking functions.  This
 * file provides fake implementations of those functions.  These fake
 * functions permit to simulate the exchange of UDP packets
 * with the libtftpfs code and thus permits testing libtftpfs without
 * the need of a full network stack.
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

#include <stdio.h> /* snprintf() */
#include <stdlib.h> /* malloc(), free() */
#include <inttypes.h> /* printf() macros like PRId8 */
#include <string.h>
#include <ctype.h> /* isprint() */
#include <netdb.h> /* getnameinfo() */
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr_in6 */
#include <rtems/test.h>

#include "tftpfs_udp_network_fake.h"

#define IPV4_ADDR_SIZE 4
#define MAX_SOCKET_FD 4

int __wrap_close( int fd ); /* Prevent compiler warnings */
int __real_close( int fd ); /* Prevent compiler warnings */

/*
 * Control data
 */

/*
 * A singleton global data object is used to control the network interaction
 * with the TFTP client.
 *
 * A test can exchange UDP packets when the TFTP client calls functions
 * sendto() and recvfrom().  Simply put, when the client calls sendto()
 * the test must check that the client sends the expected UDP packet
 * and when the client calls recvfrom() the test must provide the UDP
 * packet it wants to send to the client.
 *
 * Defining the sequence of sendto() and recvfrom() function calls
 * including parameters and return values essentially represents a
 * test for the TFTP networking.  To be a bit more complete, a few more
 * functions such as socket() and bind() are included in the sequence.
 *
 * Each of these function calls defines a single *interaction* between
 * TFTP client and test (aka TFTP server).  The idea is that each test
 * defines a sequence of interactions.  In a successful test run all
 * interactions must be carried out one-by-one till the last (normally
 * "close()") interaction is reached.
 *
 * The control_data essentially stores the sequence of interactions
 * as well as the current state (e.g. which is the next interaction?).
 */
typedef struct control_data {
  Tftp_Interaction *interaction_head;
  Tftp_Interaction *interaction_tail;
  Tftp_Interaction *interaction_cur;
  int receive_timeout_socket_fd[MAX_SOCKET_FD];
  uint32_t receive_timeout_ms[MAX_SOCKET_FD];
} control_data;

static control_data *control = NULL;

void _Tftp_Reset( void )
{
  static control_data ctl;
  int i;
  Tftp_Interaction *iter;
  Tftp_Interaction *current;

  if ( control == NULL ) {
    control = &ctl;
  } else {
    for ( iter = control->interaction_head; iter != NULL; ) {
      current = iter;
      iter = iter->next;
      free( current );
    }
  }

  control->interaction_head = NULL;
  control->interaction_tail = (Tftp_Interaction *) &control->interaction_head;
  control->interaction_cur = (Tftp_Interaction *) &control->interaction_head;
  for ( i = 0; i < MAX_SOCKET_FD; ++i ) {
    control->receive_timeout_socket_fd[i] = 0;
    control->receive_timeout_ms[i] = 0;
  }
}

void *_Tftp_Append_interaction(
  Tftp_Action_kind kind,
  Tftp_Interaction_fn fn,
  size_t size
)
{
  Tftp_Interaction *ia;
  T_quiet_not_null( control );
  ia = malloc( sizeof( Tftp_Interaction ) + size );
  T_quiet_not_null( ia );
  ia->next = NULL;
  ia->kind = kind;
  ia->fn = fn;
  control->interaction_tail->next = ia;
  control->interaction_tail = ia;
  return ia->data;
}

bool _Tftp_Has_no_more_interactions( void )
{
  return control == NULL || control->interaction_cur != NULL;
}

static Tftp_Interaction *get_next_interaction( control_data *control )
{
  if ( control == NULL ) {
    return NULL;
  }
  if ( control->interaction_cur != NULL ) {
    control->interaction_cur = control->interaction_cur->next;
  }
  return control->interaction_cur;
}

static const char *get_action_kind_as_string( Tftp_Action_kind kind )
{
  switch ( kind ) {
    case TFTP_IA_KIND_SOCKET:
      return "socket";
    case TFTP_IA_KIND_CLOSE:
      return "close";
    case TFTP_IA_KIND_BIND:
      return "bind";
    case TFTP_IA_KIND_SENDTO:
      return "sendto";
    case TFTP_IA_KIND_RECVFROM:
      return "recvfrom";
    default:
      break;
  }
  return "UNKNOWN";
}

/*
 * Parse and log UDP TFTP packet functions
 */

const char *_Tftp_Get_error_str( uint16_t error_code )
{
  static const char *unknown_str = "Unknown error code";
  static const char *error_str[] = {
    "Not defined, see error message (if any)",
    "File not found",
    "Access violation",
    "Disk full or allocation exceeded",
    "Illegal TFTP operation",
    "Unknown transfer ID",
    "File already exists",
    "No such user",
    "Option negociation failed",
  };
  const char *result = unknown_str;

  if ( error_code < RTEMS_ARRAY_SIZE( error_str ) ) {
    result = error_str[ error_code ];
  }

  return result;
}

static void log_hex_dump( const void *buf, size_t len )
{
  const size_t per_line = 16;
  size_t pos = 0;
  const uint8_t *pkt = buf;
  char hex[2 * per_line + 4];
  char chars[per_line + 1];
  char *hexpos;
  int i;

  chars[per_line] = '\0';
  do {
    for ( i = 0, hexpos = hex; i < per_line; ++i ) {
      if ( pos + i < len ) {
        hexpos += snprintf( hexpos, 3, "%02"PRIX8, pkt[ pos + i ] );
        chars[i] = ( isprint( pkt[ pos + i ] ) ) ? pkt[ pos + i ] : '.';
      } else {
        hexpos += snprintf( hexpos, 3, "  " );
        chars[i] = '\0';
      }
      if ( i < per_line - 1 && i % 4 == 3 ) {
        *(hexpos++) = ' ';
      }
    }

    T_log( T_VERBOSE, "  %04zX : %s |%s|", pos, hex, chars );
    pos += per_line;
  } while( len > pos );
}

static const char *get_next_str(
  const char **buf,
  size_t *max_len,
  bool *has_errors
)
{
  const char *result;
  size_t len = strnlen( *buf, *max_len );
  if ( len < *max_len ) {
    result = *buf;
    *buf += len + 1;
    *max_len -= len + 1;
  } else {
    result = "MAL_FORMED_STRING";
    *max_len = 0;
    *has_errors = true;
  }
  return result;
}

static void log_tftp_packet( const void *buf, size_t len )
{
  int op_code;
  const char *buffer = ( (char *) buf ) + sizeof( uint16_t );
  size_t remaining_len = len - sizeof( uint16_t );
  bool has_errors = false;

  if ( len >= sizeof( uint16_t ) ) {
    op_code = ntohs( *((uint16_t *) buf ) );
    switch ( op_code ) {
    case TFTP_OPCODE_RRQ:
    case TFTP_OPCODE_WRQ:
      T_log(
        T_VERBOSE,
        "  %s File: \"%s\" Mode: \"%s\" %s",
        ( op_code == TFTP_OPCODE_RRQ ) ? "RRQ" : "WRQ",
        get_next_str( &buffer, &remaining_len, &has_errors ),
        get_next_str( &buffer, &remaining_len, &has_errors ),
        ( remaining_len > 0 ) ? "Options:" : "No options"
      );
      while ( remaining_len > 0 ) {
        T_log(
          T_VERBOSE,
          "    %s = \"%s\"",
          get_next_str( &buffer, &remaining_len, &has_errors ),
          get_next_str( &buffer, &remaining_len, &has_errors )
        );
      }
      break;

    case TFTP_OPCODE_DATA:
      if ( len >= 2 * sizeof( uint16_t ) ) {
        buffer += sizeof( uint16_t );
        remaining_len -= sizeof( uint16_t );
        T_log(
          T_VERBOSE,
          "  DATA Block: %"PRIu16" Data (%zu bytes):",
          ntohs( *( ( (uint16_t *) buf ) + 1 ) ),
          remaining_len
        );
        log_hex_dump( buffer, remaining_len );
      } else {
        T_log( T_VERBOSE, "  DATA packet ILLEGAL length" );
        has_errors = true;
      }
      break;

    case TFTP_OPCODE_ACK:
      if ( len == 2 * sizeof( uint16_t ) ) {
        T_log(
          T_VERBOSE,
          "  ACK Block: %"PRIu16,
          ntohs( *( ( (uint16_t *) buf ) + 1 ) )
        );
      } else {
        T_log( T_VERBOSE, "  ACK packet ILLEGAL length" );
        has_errors = true;
      }
      break;

    case TFTP_OPCODE_ERROR:
      if ( len > 2 * sizeof( uint16_t ) ) {
        uint16_t err_code = ntohs( *( ( (uint16_t *) buf ) + 1 ) );
        T_log(
          T_VERBOSE,
          "  ERROR Code: %"PRIu16" (%s) ErrMsg:",
          err_code,
          _Tftp_Get_error_str( err_code )
        );
        buffer += sizeof( uint16_t );
        remaining_len -= sizeof( uint16_t );
        T_log(
          T_VERBOSE,
          "    \"%s\"",
          get_next_str( &buffer, &remaining_len, &has_errors )
        );
      } else {
        T_log( T_VERBOSE, "  ERROR packet ILLEGAL length" );
        has_errors = true;
      }
      break;

    case TFTP_OPCODE_OACK:
      T_log(
        T_VERBOSE,
        "  OACK %s",
        ( remaining_len > 0 ) ? "Options:" : "No options"
      );
      while ( remaining_len > 0 ) {
        T_log(
          T_VERBOSE,
          "    %s = \"%s\"",
          get_next_str( &buffer, &remaining_len, &has_errors ),
          get_next_str( &buffer, &remaining_len, &has_errors )
        );
      }
      break;

    default:
      T_log( T_VERBOSE, "  TFTP ILLEGAL OpCode: %d", op_code );
      has_errors = true;
    }
  } else {
    has_errors = true;
  }

  if ( has_errors ) {
    log_hex_dump( buf, len );
  }
}

static void log_interaction(
  Tftp_Action *act,
  bool has_result,
  bool was_success
)
{
  char *begin = has_result ? "[" : "";
  const char *action_name;
  int result;
  char result_buffer[20];
  result_buffer[0] = '\0';

  if ( act == NULL ) { return; }
  action_name = get_action_kind_as_string( act->kind );

  if ( has_result && was_success ) {
    switch ( act->kind ) {
    case TFTP_IA_KIND_SOCKET:
      result = act->data.socket.result;
      break;

    case TFTP_IA_KIND_CLOSE:
      result = act->data.close.result;
      break;

    case TFTP_IA_KIND_BIND:
      result = act->data.bind.result;
      break;

    case TFTP_IA_KIND_SENDTO:
      result = (int) act->data.sendto.result;
      break;

    case TFTP_IA_KIND_RECVFROM:
      result = (int) act->data.recvfrom.result;
      break;

    default:
      result = 0;
    }
    snprintf( result_buffer, sizeof( result_buffer ), "] = %d", result );
  } else if ( ! was_success ) {
    snprintf( result_buffer, sizeof( result_buffer ), "] = FAILED!" );
  }

  switch ( act->kind ) {
  case TFTP_IA_KIND_SOCKET:
    T_log(
      T_VERBOSE,
      "%s%s(domain=%d, type=%d, protocol=%d)%s",
      begin,
      action_name,
      act->data.socket.domain,
      act->data.socket.type,
      act->data.socket.protocol,
      result_buffer
    );
    break;

  case TFTP_IA_KIND_CLOSE:
    T_log( T_VERBOSE, "%s%s(%d)%s",
      begin,
      action_name,
      act->data.close.fd,
      result_buffer
    );
    break;

  case TFTP_IA_KIND_BIND:
    T_log(
      T_VERBOSE,
      "%s%s(sockfd=%d, addr.family=%d, addr=%s:%"PRIu16")%s",
      begin,
      action_name,
      act->data.bind.fd,
      act->data.bind.family,
      act->data.bind.addr_str,
      act->data.bind.port,
      result_buffer
    );
    break;

  case TFTP_IA_KIND_SENDTO:
    T_log(
      T_VERBOSE,
      "%s%s(sockfd=%d, buf=%p, len=%zu, flags=%X, "
        "addr=%s:%"PRIu16", addrlen=%d)%s",
      begin,
      action_name,
      act->data.sendto.fd,
      act->data.sendto.buf,
      act->data.sendto.len,
      act->data.sendto.flags,
      act->data.sendto.dest_addr_str,
      act->data.sendto.dest_port,
      act->data.sendto.addrlen,
      result_buffer
    );
    if ( !has_result ) {
      log_tftp_packet( act->data.sendto.buf, act->data.sendto.len );
    }
    break;

  case TFTP_IA_KIND_RECVFROM:
    if ( !has_result ) {
      T_log(
        T_VERBOSE,
        "%s%s(sockfd=%d, buf=%p, len=%zu, flags=%X, "
          "timeout=%"PRIu32"ms, addr=?:?, addrlen=%d)%s",
        begin,
        action_name,
        act->data.recvfrom.fd,
        act->data.recvfrom.buf,
        act->data.recvfrom.len,
        act->data.recvfrom.flags,
        act->data.recvfrom.timeout_ms,
        act->data.recvfrom.addrlen,
        result_buffer
      );
    } else {
      T_log(
        T_VERBOSE,
        "%s%s(sockfd=%d, buf=%p, len=%zu, flags=%X, "
          "timeout=%"PRIu32"ms, addr=%s:%"PRIu16", addrlen=%d)%s",
        begin,
        action_name,
        act->data.recvfrom.fd,
        act->data.recvfrom.buf,
        act->data.recvfrom.len,
        act->data.recvfrom.flags,
        act->data.recvfrom.timeout_ms,
        act->data.recvfrom.src_addr_str,
        act->data.recvfrom.src_port,
        act->data.recvfrom.addrlen,
        result_buffer
      );
      if ( act->data.recvfrom.result > 0 && was_success ) {
        log_tftp_packet( act->data.recvfrom.buf, act->data.recvfrom.result );
      }
    }
    break;

  default:
    T_quiet_true( false, "Unknown interaction: %d", act->kind );
  }
}

/*
 * Note: This function *must* return.
 *   All the fake network functions (and any functions called from them)
 *   must return control to the TFTP client.  Hence, do not use T_assert_*()
 *   or similar functions.  Even if the test fails at some point,
 *   continue and return an error value to the client.
 *   Reason: T_assert_*() does stop the test and invokes teardown()
 *   without returning to the client.  teardown() then "hangs" when
 *   un-mounting while executing client code.
 */
static bool process_interaction( Tftp_Action *act )
{
  Tftp_Interaction *ia = get_next_interaction( control );
  bool result;

  T_quiet_not_null( act );
  if ( act == NULL ) {
    return false;
  } else {
    /* Logging this early helps debugging defect tests */
    log_interaction( act, false, true );
  }

  T_quiet_not_null( ia );
  if( ia == NULL ) {
    T_log(
    T_NORMAL,
      "ERROR: You have not registered any (more) 'interaction' but the TFTP "
      "client invoked interaction '%s()'. See 'tftpfs_interactions.h' and use "
      "'T_set_verbosity( T_VERBOSE )'.",
      get_action_kind_as_string( act->kind )
    );
    return false;
  }

  T_true(
    act->kind == ia->kind,
    "Expected %s() call but got %s()",
    get_action_kind_as_string( ia->kind ),
    get_action_kind_as_string( act->kind )
  );
  if ( act->kind != ia->kind ) {
    return false;
  }
  result = ia->fn( act, ia->data );
  log_interaction( act, true, result );

  return result;
}

static uint16_t get_ip_addr_as_str(
  const struct sockaddr *addr,
  socklen_t addrlen,
  char *buf,
  size_t buf_size
)
{
  uint16_t port = 0xFFFF;
  *buf = '\0';

  switch ( addr->sa_family ) {
  case AF_INET:
    {
      const struct sockaddr_in *ipv4 = (const struct sockaddr_in *) addr;
      port = ntohs( ipv4->sin_port );
      const uint8_t *bytes = (const uint8_t *) &ipv4->sin_addr.s_addr;
      snprintf(
        buf,
        buf_size,
        "%"PRIu8".%"PRIu8".%"PRIu8".%"PRIu8,
        bytes[0],
        bytes[1],
        bytes[2],
        bytes[3]
      );
      break;
    }
  case AF_INET6:
    {
      const struct sockaddr_in6 *ipv6 = (const struct sockaddr_in6 *) addr;
      port = ntohs( ipv6->sin6_port );
      const uint16_t *words = (const uint16_t *) ipv6->sin6_addr.s6_addr;
      snprintf(
        buf,
        buf_size,
        "%"PRIx16":%"PRIx16":%"PRIx16":%"PRIx16":%"PRIx16":%"PRIx16
          ":%"PRIx16":%"PRIx16,
        ntohs( words[0] ),
        ntohs( words[1] ),
        ntohs( words[2] ),
        ntohs( words[3] ),
        ntohs( words[4] ),
        ntohs( words[5] ),
        ntohs( words[6] ),
        ntohs( words[7] )
      );
      break;
    }
  }

  return port;
}

static void set_ip_addr_from_str(
  const char *addr_str,
  uint16_t port,
  struct sockaddr *addr,
  socklen_t *addrlen
 )
{
  socklen_t addr_size;
  int res;
  int i;

  if ( addr == NULL || addrlen == NULL ) {
    return;
  }
  addr_size = *addrlen;

  if ( strchr( addr_str, ':' ) == NULL ) {
    /* IPv4 address */
    struct sockaddr_in *ipv4_addr = (struct sockaddr_in *) addr;
    uint8_t *bytes = (uint8_t *) &ipv4_addr->sin_addr.s_addr;

    *addrlen = sizeof( struct sockaddr_in );
    T_ge_sz( addr_size, *addrlen );
    if ( addr_size < *addrlen ) { return; }
    ipv4_addr->sin_family = AF_INET;
    ipv4_addr->sin_port = htons( port );
    res = sscanf(
      addr_str,
      "%"SCNu8".%"SCNu8".%"SCNu8".%"SCNu8,
      bytes,
      bytes + 1,
      bytes + 2,
      bytes + 3
    );
    T_quiet_true( res == 4, "Connot parse IPv4 address: \"%s\"", addr_str );
  } else {
    /* IPv6 address */
    struct sockaddr_in6 *ipv6_addr = (struct sockaddr_in6 *) addr;
    uint16_t *words = (uint16_t *) &ipv6_addr->sin6_addr.s6_addr;

    *addrlen = sizeof( struct sockaddr_in6 );
    T_gt_sz( addr_size, *addrlen );
    if ( addr_size < *addrlen ) { return; }
    ipv6_addr->sin6_family = AF_INET6;
    ipv6_addr->sin6_port = htons( port );
    ipv6_addr->sin6_flowinfo = 0;
    ipv6_addr->sin6_scope_id = 0;
    res = sscanf(
      addr_str,
      "%"SCNx16":%"SCNx16":%"SCNx16":%"SCNx16":%"SCNx16":%"SCNx16
        ":%"SCNx16":%"SCNx16,
      words,
      words + 1,
      words + 2,
      words + 3,
      words + 4,
      words + 5,
      words + 6,
      words + 7
    );
    T_quiet_true( res == 8, "Connot parse IPv6 address: \"%s\"", addr_str );
    for ( i = 0; i < 8; ++i ) {
      words[i] = htons( words[i] );
    }
  }
}

/*
 * Fake networking functions
 */

int inet_aton( const char *cp, struct in_addr *inp )
{
  int result = 0;
  uint8_t *ipv4_addr = (uint8_t *) inp;
  static const char addr0[] = TFTP_KNOWN_IPV4_ADDR0_STR;
  static const uint8_t addr0_data[] = { TFTP_KNOWN_IPV4_ADDR0_ARRAY };

  if ( strcmp( addr0, cp ) == 0 ) {
    memcpy( ipv4_addr, addr0_data, sizeof( addr0_data ) );
    result = 1;
  }

  T_log(
    T_VERBOSE,
    "inet_aton(cp=%s, addr=%"PRIu8".%"PRIu8".%"PRIu8".%"PRIu8") = %d",
    cp,
    ipv4_addr[0],
    ipv4_addr[1],
    ipv4_addr[2],
    ipv4_addr[3],
    result
  );
  return result;
}

struct hostent *gethostbyname( const char *name )
{
  static char ip_addr_bytes[] = {
    TFTP_KNOWN_SERVER0_ARRAY,   /* IPv4: 10.7.0.2 "server.tftp" */
    TFTP_KNOWN_IPV4_ADDR0_ARRAY /* IPv4: 127.0.0.1 "127.0.0.1" */
  };
  static char *ip_addrs[] = {
    ip_addr_bytes + 0 * IPV4_ADDR_SIZE, NULL,
    ip_addr_bytes + 1 * IPV4_ADDR_SIZE, NULL
  };
  static struct hostent hosts[] = {
    {
    .h_name      = TFTP_KNOWN_SERVER0_NAME,
    .h_aliases   = NULL,
    .h_addrtype  = AF_INET,
    .h_length    = IPV4_ADDR_SIZE,
    .h_addr_list = ip_addrs + 0,
    },
    {
    .h_name      = TFTP_KNOWN_IPV4_ADDR0_STR,
    .h_aliases   = NULL,
    .h_addrtype  = AF_INET,
    .h_length    = IPV4_ADDR_SIZE,
    .h_addr_list = ip_addrs + 2,
    }
  };
  struct hostent *result = NULL;
  uint8_t *ipv4_addr;
  int i;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( hosts ); ++i ) {
    if ( strcmp( hosts[i].h_name, name ) == 0 ) {
      result = hosts + i;
    }
  }
  /* Note: `h_errno` not set due to linker issues */

  if ( result != NULL ) {
    ipv4_addr = (uint8_t *) result->h_addr_list[0];
    T_log(
      T_VERBOSE,
      "gethostbyname(%s) = %s, %"PRIu8".%"PRIu8".%"PRIu8".%"PRIu8,
      name,
      result->h_name,
      ipv4_addr[0],
      ipv4_addr[1],
      ipv4_addr[2],
      ipv4_addr[3]
    );
  } else {
    T_log( T_NORMAL, "gethostbyname(%s) = %p", name, result );
  }
  return result;
}

int socket( int domain, int type, int protocol )
{
  Tftp_Action act = {
    .kind                 = TFTP_IA_KIND_SOCKET,
    .data.socket.domain   = domain,
    .data.socket.type     = type,
    .data.socket.protocol = protocol
  };

  if( !process_interaction( &act ) ) {
    return -1;
  };

  /* Should never happen but check prevents programming mistakes */
  T_quiet_ge_int( act.data.socket.result, TFTP_FIRST_FD );
  if( act.data.socket.result < TFTP_FIRST_FD ) {
    return -1;
  };

  return act.data.socket.result;
}

int __wrap_close( int fd )
{
  if ( fd < TFTP_FIRST_FD ) {
    /* Normal file descriptors - i.e. not from fake socket() function above */
    return __real_close( fd );
  }
  Tftp_Action act = {
    .kind = TFTP_IA_KIND_CLOSE,
    .data.close.fd = fd,
  };

  if( !process_interaction( &act ) ) {
    return -1;
  };

  return act.data.close.result;
}

int bind( int sockfd, const struct sockaddr *addr, socklen_t addrlen )
{
  char addr_buf[INET6_ADDRSTRLEN];
  Tftp_Action act = {
    .kind                   = TFTP_IA_KIND_BIND,
    .data.bind.fd           = sockfd,
    .data.bind.family       = addr->sa_family,
    .data.bind.addr_str     = addr_buf
  };
  act.data.bind.port = get_ip_addr_as_str(
    addr,
    addrlen,
    addr_buf,
    sizeof( addr_buf )
  );

  if( !process_interaction( &act ) ) {
    return -1;
  };

  return act.data.bind.result;
}

int setsockopt(
  int sockfd,
  int level,
  int optname,
  const void *optval,
  socklen_t optlen
)
{
  int result = 0;
  int i;
  const struct timeval *tv = optval;

  T_log(
    T_VERBOSE,
    "setsockopt(sockfd=%d, level=%s, optname=%s, optval=%dms )",
    sockfd,
    ( level == SOL_SOCKET    ) ? "SOL_SOCKET"  : "UNKONWN",
    ( optname == SO_RCVTIMEO ) ? "SO_RCVTIMEO" : "UNKONWN",
    ( optname == SO_RCVTIMEO ) ?
      (int) ( tv->tv_sec * 1000 + tv->tv_usec / 1000 ) : -1
  );

  T_eq_int( level, SOL_SOCKET );
  T_eq_int( optname, SO_RCVTIMEO );
  T_eq_int( (int) optlen, sizeof( struct timeval ) );
  if (
    level != SOL_SOCKET ||
    optname != SO_RCVTIMEO ||
    optlen != sizeof( struct timeval )
  ) {
    result = -1;
  }

  for ( i = 0; result >= 0; ++i ) {
    if ( i >= MAX_SOCKET_FD ) {
      T_quiet_true(
        false,
        "Too few IP ports %d, increase MAX_SOCKET_FD",
        MAX_SOCKET_FD
      );
      result = -1;
      break;
    }
    if ( control->receive_timeout_socket_fd[i] == sockfd ||
      control->receive_timeout_socket_fd[i] == 0 ) {
      control->receive_timeout_socket_fd[i] = sockfd;
      control->receive_timeout_ms[i]   = tv->tv_sec * 1000 + tv->tv_usec / 1000;
      break;
    }
  }

  T_log(
    T_VERBOSE,
    "[setsockopt(sockfd=%d, level=%s, optname=%s, optval=%"PRIu32"ms )] = %d",
    sockfd,
    ( level == SOL_SOCKET    ) ? "SOL_SOCKET"  : "UNKONWN",
    ( optname == SO_RCVTIMEO ) ? "SO_RCVTIMEO" : "UNKONWN",
    ( i < MAX_SOCKET_FD ) ? control->receive_timeout_ms[i] : -1,
    result
  );

  return result;
}

ssize_t sendto(
  int sockfd,
  const void *buf,
  size_t len,
  int flags,
  const struct sockaddr *dest_addr,
  socklen_t addrlen
)
{
  char addr_buf[INET6_ADDRSTRLEN];
  Tftp_Action act = {
    .kind                      = TFTP_IA_KIND_SENDTO,
    .data.sendto.fd            = sockfd,
    .data.sendto.buf           = buf,
    .data.sendto.len           = len,
    .data.sendto.flags         = flags,
    .data.sendto.dest_addr_str = addr_buf,
    .data.sendto.addrlen       = (int) addrlen,
  };
  act.data.sendto.dest_port = get_ip_addr_as_str(
    dest_addr,
    addrlen,
    addr_buf,
    sizeof( addr_buf )
  );

  if( !process_interaction( &act ) ) {
    return -1;
  };

  return act.data.sendto.result;
}

ssize_t recvfrom(
  int sockfd,
  void *buf,
  size_t len,
  int flags,
  struct sockaddr *src_addr,
  socklen_t *addrlen
)
{
  int i;
  Tftp_Packet *pkt = buf;
  Tftp_Action act = {
    .kind                       = TFTP_IA_KIND_RECVFROM,
    .data.recvfrom.fd           = sockfd,
    .data.recvfrom.buf          = buf,
    .data.recvfrom.len          = len,
    .data.recvfrom.flags        = flags,
    .data.recvfrom.timeout_ms   = 0,
    .data.recvfrom.addrlen      = (int) *addrlen
  };

  for ( i = 0; i < MAX_SOCKET_FD; ++i ) {
    if ( control->receive_timeout_socket_fd[i] == sockfd ) {
      act.data.recvfrom.timeout_ms = control->receive_timeout_ms[i];
      break;
    }
  }

  /* Make log_tftp_packet() behave sane, if buf is not filled */
  if ( len >= sizeof( pkt->opcode ) ) {
    pkt->opcode = ntohs( 0xFFFF );
  }

  if( !process_interaction( &act ) ) {
    return -1;
  };

  set_ip_addr_from_str(
    act.data.recvfrom.src_addr_str,
    act.data.recvfrom.src_port,
    src_addr,
    addrlen
  );

  return act.data.recvfrom.result;
}
