/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2018, 2019 embedded brains GmbH
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

/*
 * This file must be compatible to general purpose POSIX system, e.g. Linux,
 * FreeBSD.  It may be used for utility programs.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/recordclient.h>

#include <string.h>

static void set_to_bt_scaler(
  rtems_record_client_context *ctx,
  uint32_t                     frequency
)
{
  uint64_t bin_per_s;

  bin_per_s = UINT64_C( 1 ) << 32;
  ctx->to_bt_scaler = ( ( bin_per_s << 31 ) + frequency - 1 ) / frequency;
}

static rtems_record_client_status call_handler(
  const rtems_record_client_context *ctx,
  uint64_t                           bt,
  rtems_record_event                 event,
  uint64_t                           data
)
{
  uint32_t seconds;
  uint32_t nanosec;

  seconds = (uint32_t) ( bt >> 32 );
  nanosec = (uint32_t) ( ( UINT64_C( 1000000000 ) * (uint32_t) bt ) >> 32 );

  return ( *ctx->handler )(
    seconds,
    nanosec,
    ctx->cpu,
    event,
    data,
    ctx->handler_arg
  );
}

static void check_overflow(
  const rtems_record_client_context *ctx,
  const rtems_record_client_per_cpu *per_cpu,
  uint32_t                           new_head
)
{
  uint32_t last_tail;
  uint32_t last_head;
  uint32_t capacity;
  uint32_t new_content;
  uint64_t bt;

  last_tail = per_cpu->tail[ per_cpu->index ];
  last_head = per_cpu->head[ per_cpu->index ];

  if ( last_tail == last_head ) {
    return;
  }

  capacity = ( last_tail - last_head - 1 ) & ( ctx->count - 1 );
  new_content = new_head - last_head;

  if ( new_content <= capacity ) {
    return;
  }

  bt = ( per_cpu->uptime.time_accumulated * ctx->to_bt_scaler ) >> 31;
  bt += per_cpu->uptime.bt;

  call_handler(
    ctx,
    bt,
    RTEMS_RECORD_PER_CPU_OVERFLOW,
    new_content - capacity
  );
}

static rtems_record_client_status visit( rtems_record_client_context *ctx )
{
  rtems_record_client_per_cpu *per_cpu;
  uint32_t                     time;
  rtems_record_event           event;
  uint64_t                     data;
  uint64_t                     bt;

  per_cpu = &ctx->per_cpu[ ctx->cpu ];
  time = RTEMS_RECORD_GET_TIME( ctx->event );
  event = RTEMS_RECORD_GET_EVENT( ctx->event );
  data = ctx->data;

  switch ( event ) {
    case RTEMS_RECORD_PROCESSOR:
      if ( data >= RTEMS_RECORD_CLIENT_MAXIMUM_CPU_COUNT ) {
        return RTEMS_RECORD_CLIENT_ERROR_UNSUPPORTED_CPU;
      }

      ctx->cpu = (uint32_t) data;
      per_cpu = &ctx->per_cpu[ ctx->cpu ];
      break;
    case RTEMS_RECORD_UPTIME_LOW:
      per_cpu->uptime.bt = (uint32_t) data;
      per_cpu->uptime.time_at_bt = time;
      per_cpu->uptime.time_last = time;
      per_cpu->uptime.time_accumulated = 0;
      time = 0;
      break;
    case RTEMS_RECORD_UPTIME_HIGH:
      per_cpu->uptime.bt += (int64_t) data << 32;
      time = 0;
      break;
    case RTEMS_RECORD_PER_CPU_TAIL:
      per_cpu->tail[ per_cpu->index ] = (uint32_t) data;
      break;
    case RTEMS_RECORD_PER_CPU_HEAD:
      per_cpu->head[ per_cpu->index ]= (uint32_t) data;
      per_cpu->index ^= 1;
      check_overflow( ctx, per_cpu, (uint32_t) data );
      break;
    case RTEMS_RECORD_PER_CPU_COUNT:
      ctx->count = (uint32_t) data;
      break;
    case RTEMS_RECORD_FREQUENCY:
      set_to_bt_scaler( ctx, (uint32_t) data );
      break;
    case RTEMS_RECORD_VERSION:
      if ( data != RTEMS_RECORD_THE_VERSION ) {
        return RTEMS_RECORD_CLIENT_ERROR_UNSUPPORTED_VERSION;
      }

      break;
    default:
      break;
  }

  if ( time != 0 ) {
    uint32_t delta;

    delta = ( time - per_cpu->uptime.time_last )
      & ( ( UINT32_C( 1 ) << RTEMS_RECORD_TIME_BITS ) - 1 );
    per_cpu->uptime.time_last = time;
    per_cpu->uptime.time_accumulated += delta;
    bt = ( per_cpu->uptime.time_accumulated * ctx->to_bt_scaler ) >> 31;
    bt += per_cpu->uptime.bt;
  } else {
    bt = 0;
  }

  return call_handler( ctx, bt, event, data );
}

static rtems_record_client_status consume_32(
  rtems_record_client_context *ctx,
  const void                  *buf,
  size_t                       n
)
{
  while ( n > 0 ) {
    size_t m;
    char *pos;

    m = ctx->todo < n ? ctx->todo : n;
    pos = ctx->pos;
    pos = memcpy( pos, buf, m );
    n -= m;
    buf = (char *) buf + m;

    if ( m == ctx->todo ) {
      rtems_record_client_status status;

      ctx->todo = sizeof( ctx->item.format_32 );
      ctx->pos = &ctx->item.format_32;
      ctx->event = ctx->item.format_32.event;
      ctx->data = ctx->item.format_32.data;

      status = visit( ctx );

      if ( status != RTEMS_RECORD_CLIENT_SUCCESS ) {
        return status;
      }
    } else {
      ctx->todo -= m;
      ctx->pos = pos + m;
    }
  }

  return RTEMS_RECORD_CLIENT_SUCCESS;
}

static rtems_record_client_status consume_64(
  rtems_record_client_context *ctx,
  const void                  *buf,
  size_t                       n
)
{
  while ( n > 0 ) {
    size_t m;
    char *pos;

    m = ctx->todo < n ? ctx->todo : n;
    pos = ctx->pos;
    pos = memcpy( pos, buf, m );
    n -= m;
    buf = (char *) buf + m;

    if ( m == ctx->todo ) {
      rtems_record_client_status status;

      ctx->todo = sizeof( ctx->item.format_64 );
      ctx->pos = &ctx->item.format_64;
      ctx->event = ctx->item.format_64.event;
      ctx->data = ctx->item.format_64.data;

      status = visit( ctx );

      if ( status != RTEMS_RECORD_CLIENT_SUCCESS ) {
        return status;
      }
    } else {
      ctx->todo -= m;
      ctx->pos = pos + m;
    }
  }

  return RTEMS_RECORD_CLIENT_SUCCESS;
}

static rtems_record_client_status consume_swap_32(
  rtems_record_client_context *ctx,
  const void                  *buf,
  size_t                       n
)
{
  while ( n > 0 ) {
    size_t m;
    char *pos;

    m = ctx->todo < n ? ctx->todo : n;
    pos = ctx->pos;
    pos = memcpy( pos, buf, m );
    n -= m;
    buf = (char *) buf + m;

    if ( m == ctx->todo ) {
      rtems_record_client_status status;

      ctx->todo = sizeof( ctx->item.format_32 );
      ctx->pos = &ctx->item.format_32;
      ctx->event = __builtin_bswap32( ctx->item.format_32.event );
      ctx->data = __builtin_bswap32( ctx->item.format_32.data );

      status = visit( ctx );

      if ( status != RTEMS_RECORD_CLIENT_SUCCESS ) {
        return status;
      }
    } else {
      ctx->todo -= m;
      ctx->pos = pos + m;
    }
  }

  return RTEMS_RECORD_CLIENT_SUCCESS;
}

static rtems_record_client_status consume_swap_64(
  rtems_record_client_context *ctx,
  const void                  *buf,
  size_t                       n
)
{
  while ( n > 0 ) {
    size_t m;
    char *pos;

    m = ctx->todo < n ? ctx->todo : n;
    pos = ctx->pos;
    pos = memcpy( pos, buf, m );
    n -= m;
    buf = (char *) buf + m;

    if ( m == ctx->todo ) {
      rtems_record_client_status status;

      ctx->todo = sizeof( ctx->item.format_64 );
      ctx->pos = &ctx->item.format_64;
      ctx->event = __builtin_bswap32( ctx->item.format_64.event );
      ctx->data = __builtin_bswap64( ctx->item.format_64.data );

      status = visit( ctx );

      if ( status != RTEMS_RECORD_CLIENT_SUCCESS ) {
        return status;
      }
    } else {
      ctx->todo -= m;
      ctx->pos = pos + m;
    }
  }

  return RTEMS_RECORD_CLIENT_SUCCESS;
}

static rtems_record_client_status consume_init(
  rtems_record_client_context *ctx,
  const void                  *buf,
  size_t                       n
)
{
  while ( n > 0 ) {
    size_t m;
    char *pos;

    m = ctx->todo < n ? ctx->todo : n;
    pos = ctx->pos;
    pos = memcpy( pos, buf, m );
    n -= m;
    buf = (char *) buf + m;

    if ( m == ctx->todo ) {
      uint32_t magic;

      magic = ctx->header[ 1 ];

      switch ( ctx->header[ 0 ] ) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        case RTEMS_RECORD_FORMAT_LE_32:
          ctx->todo = sizeof( ctx->item.format_32 );
          ctx->pos = &ctx->item.format_32;
          ctx->consume = consume_32;
          break;
        case RTEMS_RECORD_FORMAT_LE_64:
          ctx->todo = sizeof( ctx->item.format_64 );
          ctx->pos = &ctx->item.format_64;
          ctx->consume = consume_64;
          break;
        case RTEMS_RECORD_FORMAT_BE_32:
          ctx->todo = sizeof( ctx->item.format_32 );
          ctx->pos = &ctx->item.format_32;
          ctx->consume = consume_swap_32;
          magic = __builtin_bswap32( magic );
          break;
        case RTEMS_RECORD_FORMAT_BE_64:
          ctx->todo = sizeof( ctx->item.format_64 );
          ctx->pos = &ctx->item.format_64;
          ctx->consume = consume_swap_64;
          magic = __builtin_bswap32( magic );
          break;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        case RTEMS_RECORD_FORMAT_LE_32:
          ctx->todo = sizeof( ctx->item.format_32 );
          ctx->pos = &ctx->item.format_32;
          ctx->consume = consume_swap_32;
          magic = __builtin_bswap32( magic );
          break;
        case RTEMS_RECORD_FORMAT_LE_64:
          ctx->todo = sizeof( ctx->item.format_64 );
          ctx->pos = &ctx->item.format_64;
          ctx->consume = consume_swap_64;
          magic = __builtin_bswap32( magic );
          break;
        case RTEMS_RECORD_FORMAT_BE_32:
          ctx->todo = sizeof( ctx->item.format_32 );
          ctx->pos = &ctx->item.format_32;
          ctx->consume = consume_32;
          break;
        case RTEMS_RECORD_FORMAT_BE_64:
          ctx->todo = sizeof( ctx->item.format_64 );
          ctx->pos = &ctx->item.format_64;
          ctx->consume = consume_64;
          break;
#else
#error "unexpected __BYTE_ORDER__"
#endif
        default:
          return RTEMS_RECORD_CLIENT_ERROR_UNKNOWN_FORMAT;
      }

      if ( magic != RTEMS_RECORD_MAGIC ) {
        return RTEMS_RECORD_CLIENT_ERROR_INVALID_MAGIC;
      }

      return rtems_record_client_run( ctx, buf, n );
    } else {
      ctx->todo -= m;
      ctx->pos = pos + m;
    }
  }

  return RTEMS_RECORD_CLIENT_SUCCESS;
}

void rtems_record_client_init(
  rtems_record_client_context *ctx,
  rtems_record_client_handler  handler,
  void                        *arg
)
{
  ctx = memset( ctx, 0, sizeof( *ctx ) );
  ctx->to_bt_scaler = UINT64_C( 1 ) << 31;
  ctx->handler = handler;
  ctx->handler_arg = arg;
  ctx->todo = sizeof( ctx->header );
  ctx->pos = &ctx->header;
  ctx->consume = consume_init;
}

rtems_record_client_status rtems_record_client_run(
  rtems_record_client_context *ctx,
  const void                  *buf,
  size_t                       n
)
{
  return ( *ctx->consume )( ctx, buf, n );
}
