/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2018, 2024 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/recordclient.h>

#include <stdlib.h>
#include <string.h>

#define TIME_MASK ( ( UINT32_C( 1 ) << RTEMS_RECORD_TIME_BITS ) - 1 )

static rtems_record_client_status visit(
  rtems_record_client_context *ctx,
  uint32_t                     time_event,
  uint64_t                     data
);

static rtems_record_client_status consume_error(
  rtems_record_client_context *ctx,
  const void                  *buf,
  size_t                       n
)
{
  (void) buf;
  (void) n;

  return ctx->status;
}

static rtems_record_client_status error(
  rtems_record_client_context *ctx,
  rtems_record_client_status   status
)
{
  ctx->status = status;
  ctx->consume = consume_error;

  return status;
}

static rtems_record_client_status process_per_cpu_count(
  rtems_record_client_context *ctx,
  uint64_t                     data
)
{
  size_t   item_capacity;
  uint32_t cpu;

  if ( ctx->per_cpu_items != 0 ) {
    return error( ctx, RTEMS_RECORD_CLIENT_ERROR_DOUBLE_PER_CPU_COUNT );
  }

  if ( ctx->cpu_count == 0 ) {
    return error( ctx, RTEMS_RECORD_CLIENT_ERROR_NO_CPU_MAX );
  }

  ctx->per_cpu_items = (uint32_t) data;

  /*
   * Use two times the ring buffer capacity so that it remains a power of two
   * and can hold the RTEMS_RECORD_PROCESSOR and RTEMS_RECORD_PER_CPU_OVERFLOW
   * items produced by rtems_record_fetch().
   */
  item_capacity = 2 * ctx->per_cpu_items;

  for ( cpu = 0; cpu < ctx->cpu_count; ++cpu ) {
    rtems_record_client_per_cpu *per_cpu;

    per_cpu = &ctx->per_cpu[ cpu ];
    per_cpu->items = realloc(
      per_cpu->items,
      item_capacity * sizeof( *per_cpu->items )
    );

    if ( per_cpu->items == NULL ) {
      return error( ctx, RTEMS_RECORD_CLIENT_ERROR_NO_MEMORY );
    }

    per_cpu->item_capacity = item_capacity;
  }

  return RTEMS_RECORD_CLIENT_SUCCESS;
}

static void set_to_bt_scaler(
  rtems_record_client_context *ctx,
  uint32_t                     frequency
)
{
  uint64_t bin_per_s;

  bin_per_s = UINT64_C( 1 ) << 32;
  ctx->to_bt_scaler = ( ( bin_per_s << 31 ) + frequency / 2 ) / frequency;
}

static bool has_time( rtems_record_event event )
{
  return event > RTEMS_RECORD_NO_TIME_LAST;
}

static uint64_t time_bt(
  const rtems_record_client_context *ctx,
  rtems_record_client_per_cpu       *per_cpu,
  uint32_t                           time,
  rtems_record_event                 event
)
{
  uint64_t time_accumulated;
  uint64_t last_bt;
  uint64_t bt;

  time_accumulated = per_cpu->uptime.time_accumulated;

  if ( has_time( event) ) {
    time_accumulated += ( time - per_cpu->uptime.time_last ) & TIME_MASK;
    per_cpu->uptime.time_last = time;
    per_cpu->uptime.time_accumulated = time_accumulated;
  }

  last_bt = per_cpu->last_bt;
  bt = per_cpu->uptime.uptime_bt;
  bt += ( time_accumulated * ctx->to_bt_scaler ) >> 31;

  if ( bt >= last_bt ) {
    per_cpu->last_bt = bt;

    return bt;
  }

  (void) ( *ctx->handler )(
    last_bt,
    ctx->cpu,
    RTEMS_RECORD_TIME_ADJUSTMENT,
    last_bt - bt,
    ctx->handler_arg
  );

  return last_bt;
}

static rtems_record_client_status call_handler(
  const rtems_record_client_context *ctx,
  rtems_record_client_per_cpu       *per_cpu,
  uint32_t                           time,
  rtems_record_event                 event,
  uint64_t                           data
)
{
  return ( *ctx->handler )(
    time_bt( ctx, per_cpu, time, event ),
    ctx->cpu,
    event,
    data,
    ctx->handler_arg
  );
}

static rtems_record_client_status resolve_hold_back(
  rtems_record_client_context *ctx,
  rtems_record_client_per_cpu *per_cpu
)
{
  rtems_record_item_64 *items;
  uint32_t last;
  uint64_t accumulated;
  size_t index;
  rtems_record_client_uptime uptime;

  items = per_cpu->items;
  last = per_cpu->uptime.time_last;
  accumulated = 0;

  for ( index = per_cpu->item_index; index > 0; --index ) {
    uint32_t time_event;

    time_event = items[ index - 1 ].event;

    if ( has_time( RTEMS_RECORD_GET_EVENT( time_event ) ) ) {
      uint32_t time;

      time = RTEMS_RECORD_GET_TIME( time_event );
      accumulated += ( last - time ) & TIME_MASK;
      last = time;
    }
  }

  uptime = per_cpu->uptime;
  per_cpu->uptime.uptime_bt -= ( accumulated * ctx->to_bt_scaler ) >> 31;
  per_cpu->uptime.time_last = last;
  per_cpu->uptime.time_accumulated = 0;

  for ( index = 0; index < per_cpu->item_index; ++index ) {
    uint32_t time_event;
    rtems_record_client_status status;

    time_event = items[ index ].event;
    status = call_handler(
      ctx,
      per_cpu,
      RTEMS_RECORD_GET_TIME( time_event ),
      RTEMS_RECORD_GET_EVENT( time_event ),
      items[ index ].data
    );

    if ( status != RTEMS_RECORD_CLIENT_SUCCESS ) {
      return status;
    }
  }

  per_cpu->uptime = uptime;
  per_cpu->hold_back = false;
  per_cpu->item_index = 0;

  return RTEMS_RECORD_CLIENT_SUCCESS;
}

static rtems_record_client_status hold_back(
  rtems_record_client_context *ctx,
  rtems_record_client_per_cpu *per_cpu,
  uint32_t                     time_event,
  uint64_t                     data
)
{
  uint32_t item_index;

  item_index = per_cpu->item_index;

  if ( item_index >= per_cpu->item_capacity ) {
    if ( item_index >= RTEMS_RECORD_CLIENT_HOLD_BACK_REALLOCATION_LIMIT ) {
      return error( ctx, RTEMS_RECORD_CLIENT_ERROR_PER_CPU_ITEMS_OVERFLOW );
    }

    per_cpu->item_capacity = 2 * item_index;
    per_cpu->items = realloc(
      per_cpu->items,
      per_cpu->item_capacity * sizeof( *per_cpu->items )
    );

    if ( per_cpu->items == NULL ) {
      return error( ctx, RTEMS_RECORD_CLIENT_ERROR_NO_MEMORY );
    }
  }

  per_cpu->items[ item_index ].event = time_event;
  per_cpu->items[ item_index ].data = data;
  per_cpu->item_index = item_index + 1;

  return RTEMS_RECORD_CLIENT_SUCCESS;
}

static rtems_record_client_status visit(
  rtems_record_client_context *ctx,
  uint32_t                     time_event,
  uint64_t                     data
)
{
  rtems_record_client_per_cpu *per_cpu;
  uint32_t                     time;
  rtems_record_event           event;
  rtems_record_client_status   status;
  bool                         do_hold_back;

  per_cpu = &ctx->per_cpu[ ctx->cpu ];
  time = RTEMS_RECORD_GET_TIME( time_event );
  event = RTEMS_RECORD_GET_EVENT( time_event );
  do_hold_back = per_cpu->hold_back;

  switch ( event ) {
    case RTEMS_RECORD_PROCESSOR:
      if ( data >= ctx->cpu_count ) {
        return error( ctx, RTEMS_RECORD_CLIENT_ERROR_UNSUPPORTED_CPU );
      }

      ctx->cpu = (uint32_t) data;
      per_cpu = &ctx->per_cpu[ ctx->cpu ];
      break;
    case RTEMS_RECORD_UPTIME_LOW:
      per_cpu->uptime_low = (uint32_t) data;
      per_cpu->uptime_low_valid = true;
      break;
    case RTEMS_RECORD_UPTIME_HIGH:
      if ( per_cpu->uptime_low_valid ) {
        per_cpu->uptime_low_valid = false;
        per_cpu->uptime.uptime_bt = ( data << 32 ) | per_cpu->uptime_low;
        per_cpu->uptime.time_last = time;
        per_cpu->uptime.time_accumulated = 0;

        if (do_hold_back) {
          status = resolve_hold_back( ctx, per_cpu );

          if ( status != RTEMS_RECORD_CLIENT_SUCCESS ) {
            return status;
          }
        }

        do_hold_back = false;
      }

      break;
    case RTEMS_RECORD_PROCESSOR_MAXIMUM:
      if ( data >= RTEMS_RECORD_CLIENT_MAXIMUM_CPU_COUNT ) {
        return error( ctx, RTEMS_RECORD_CLIENT_ERROR_UNSUPPORTED_CPU_MAX );
      }

      if ( ctx->cpu_count != 0 ) {
        return error( ctx, RTEMS_RECORD_CLIENT_ERROR_DOUBLE_CPU_MAX );
      }

      ctx->cpu_count = (uint32_t) data + 1;
      do_hold_back = false;
      break;
    case RTEMS_RECORD_PER_CPU_COUNT:
      status = process_per_cpu_count( ctx, data );

      if ( status != RTEMS_RECORD_CLIENT_SUCCESS ) {
        return status;
      }

      break;
    case RTEMS_RECORD_PER_CPU_OVERFLOW:
      do_hold_back = true;
      per_cpu->hold_back = true;
      break;
    case RTEMS_RECORD_FREQUENCY:
      set_to_bt_scaler( ctx, (uint32_t) data );
      break;
    case RTEMS_RECORD_VERSION:
      if ( data != RTEMS_RECORD_THE_VERSION ) {
        return error( ctx, RTEMS_RECORD_CLIENT_ERROR_UNSUPPORTED_VERSION );
      }

      do_hold_back = false;
      break;
    default:
      break;
  }

  if ( do_hold_back ) {
    return hold_back( ctx, per_cpu, time_event, data );
  }

  return call_handler( ctx, per_cpu, time, event, data );
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

      status = visit(
        ctx,
        ctx->item.format_32.event,
        ctx->item.format_32.data
      );

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

      status = visit(
        ctx,
        ctx->item.format_64.event,
        ctx->item.format_64.data
      );

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

      status = visit(
        ctx,
        __builtin_bswap32( ctx->item.format_32.event ),
        __builtin_bswap32( ctx->item.format_32.data )
      );

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

      status = visit(
        ctx,
        __builtin_bswap32( ctx->item.format_64.event ),
        __builtin_bswap64( ctx->item.format_64.data )
      );

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
          ctx->data_size = 4;
          break;
        case RTEMS_RECORD_FORMAT_LE_64:
          ctx->todo = sizeof( ctx->item.format_64 );
          ctx->pos = &ctx->item.format_64;
          ctx->consume = consume_64;
          ctx->data_size = 8;
          break;
        case RTEMS_RECORD_FORMAT_BE_32:
          ctx->todo = sizeof( ctx->item.format_32 );
          ctx->pos = &ctx->item.format_32;
          ctx->consume = consume_swap_32;
          ctx->data_size = 4;
          magic = __builtin_bswap32( magic );
          break;
        case RTEMS_RECORD_FORMAT_BE_64:
          ctx->todo = sizeof( ctx->item.format_64 );
          ctx->pos = &ctx->item.format_64;
          ctx->consume = consume_swap_64;
          ctx->data_size = 8;
          magic = __builtin_bswap32( magic );
          break;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        case RTEMS_RECORD_FORMAT_LE_32:
          ctx->todo = sizeof( ctx->item.format_32 );
          ctx->pos = &ctx->item.format_32;
          ctx->consume = consume_swap_32;
          ctx->data_size = 4;
          magic = __builtin_bswap32( magic );
          break;
        case RTEMS_RECORD_FORMAT_LE_64:
          ctx->todo = sizeof( ctx->item.format_64 );
          ctx->pos = &ctx->item.format_64;
          ctx->consume = consume_swap_64;
          ctx->data_size = 8;
          magic = __builtin_bswap32( magic );
          break;
        case RTEMS_RECORD_FORMAT_BE_32:
          ctx->todo = sizeof( ctx->item.format_32 );
          ctx->pos = &ctx->item.format_32;
          ctx->consume = consume_32;
          ctx->data_size = 4;
          break;
        case RTEMS_RECORD_FORMAT_BE_64:
          ctx->todo = sizeof( ctx->item.format_64 );
          ctx->pos = &ctx->item.format_64;
          ctx->consume = consume_64;
          ctx->data_size = 8;
          break;
#else
#error "unexpected __BYTE_ORDER__"
#endif
        default:
          return error( ctx, RTEMS_RECORD_CLIENT_ERROR_UNKNOWN_FORMAT );
      }

      if ( magic != RTEMS_RECORD_MAGIC ) {
        return error( ctx, RTEMS_RECORD_CLIENT_ERROR_INVALID_MAGIC );
      }

      return rtems_record_client_run( ctx, buf, n );
    } else {
      ctx->todo -= m;
      ctx->pos = pos + m;
    }
  }

  return RTEMS_RECORD_CLIENT_SUCCESS;
}

rtems_record_client_status  rtems_record_client_init(
  rtems_record_client_context *ctx,
  rtems_record_client_handler  handler,
  void                        *arg
)
{
  uint32_t cpu;

  ctx = memset( ctx, 0, sizeof( *ctx ) );
  ctx->to_bt_scaler = UINT64_C( 1 ) << 31;
  ctx->handler = handler;
  ctx->handler_arg = arg;
  ctx->todo = sizeof( ctx->header );
  ctx->pos = &ctx->header;
  ctx->consume = consume_init;

  for ( cpu = 0; cpu < RTEMS_RECORD_CLIENT_MAXIMUM_CPU_COUNT; ++cpu ) {
    ctx->per_cpu[ cpu ].hold_back = true;
  }

  return RTEMS_RECORD_CLIENT_SUCCESS;
}

rtems_record_client_status rtems_record_client_run(
  rtems_record_client_context *ctx,
  const void                  *buf,
  size_t                       n
)
{
  return ( *ctx->consume )( ctx, buf, n );
}

static void calculate_best_effort_uptime(
  rtems_record_client_context *ctx,
  rtems_record_client_per_cpu *per_cpu
)
{
  rtems_record_item_64 *items;
  uint32_t last;
  uint64_t accumulated;
  size_t index;

  items = per_cpu->items;
  accumulated = 0;

  if ( per_cpu->uptime.uptime_bt != 0 ) {
    last = per_cpu->uptime.time_last;
  } else {
    last = RTEMS_RECORD_GET_TIME( items[ 0 ].event );
  }

  for ( index = 0; index < per_cpu->item_index; ++index ) {
    uint32_t time_event;

    time_event = items[ index ].event;

    if ( has_time( RTEMS_RECORD_GET_EVENT( time_event ) ) ) {
      uint32_t time;

      time = RTEMS_RECORD_GET_TIME( time_event );
      accumulated += ( time - last ) & TIME_MASK;
      last = time;
    }
  }

  per_cpu->uptime.uptime_bt += ( accumulated * ctx->to_bt_scaler ) >> 31;
  per_cpu->uptime.time_last = last;
  per_cpu->uptime.time_accumulated = 0;
}

void rtems_record_client_destroy(
  rtems_record_client_context *ctx
)
{
  uint32_t cpu;

  for ( cpu = 0; cpu < ctx->cpu_count; ++cpu ) {
    rtems_record_client_per_cpu *per_cpu;

    ctx->cpu = cpu;
    per_cpu = &ctx->per_cpu[ cpu ];

    if ( per_cpu->hold_back && per_cpu->item_index > 0 ) {
      (void) call_handler(
        ctx,
        per_cpu,
        0,
        RTEMS_RECORD_UNRELIABLE_TIME,
        0
      );
      calculate_best_effort_uptime( ctx, per_cpu );
      (void) resolve_hold_back( ctx, per_cpu );
    }

    free( per_cpu->items );
  }
}
