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
  return ( *ctx->handler )(
    bt,
    ctx->cpu,
    event,
    data,
    ctx->handler_arg
  );
}

static void signal_overflow(
  rtems_record_client_per_cpu *per_cpu,
  uint32_t                     data
)
{
  per_cpu->hold_back = true;
  per_cpu->item_index = 0;
  per_cpu->overflow += data;
}

static void resolve_hold_back(
  rtems_record_client_context *ctx,
  rtems_record_client_per_cpu *per_cpu
)
{
  if ( per_cpu->hold_back ) {
    uint32_t last_tail;
    uint32_t last_head;
    uint32_t last_capacity;
    uint32_t new_head;
    uint32_t new_content;
    uint32_t begin_index;
    uint32_t index;
    uint32_t first;
    uint32_t last;
    uint32_t delta;
    uint64_t uptime;

    last_head = per_cpu->head[ per_cpu->tail_head_index ];
    last_tail = per_cpu->tail[ per_cpu->tail_head_index ];
    new_head = per_cpu->head[ per_cpu->tail_head_index ^ 1 ];
    last_capacity = ( last_tail - last_head - 1 ) & ( ctx->count - 1 );
    new_content = new_head - last_head;

    if ( new_content > last_capacity ) {
      begin_index = new_content - last_capacity;
      per_cpu->overflow += begin_index;
    } else {
      begin_index = 0;
    }

    if ( begin_index >= per_cpu->item_index ) {
      per_cpu->item_index = 0;
      return;
    }

    per_cpu->hold_back = false;

    first = RTEMS_RECORD_GET_TIME( per_cpu->items[ begin_index ].event );
    last = first;
    delta = 0;
    uptime = 0;

    for ( index = begin_index; index < per_cpu->item_index; ++index ) {
      const rtems_record_item_64 *item;
      rtems_record_event          event;
      uint32_t                    time;

      item = &per_cpu->items[ index ];
      event = RTEMS_RECORD_GET_EVENT( item->event );
      time = RTEMS_RECORD_GET_TIME( item->event );
      delta += ( time - last ) & TIME_MASK;
      last = time;

      if (
        event == RTEMS_RECORD_UPTIME_LOW
          && index + 1 < per_cpu->item_index
          && RTEMS_RECORD_GET_EVENT( ( item + 1 )->event )
            == RTEMS_RECORD_UPTIME_HIGH
      ) {
        uptime = (uint32_t) item->data;
        uptime += ( item + 1 )->data << 32;
        break;
      }
    }

    per_cpu->uptime.bt = uptime - ( ( delta * ctx->to_bt_scaler ) >> 31 );
    per_cpu->uptime.time_at_bt = first;
    per_cpu->uptime.time_last = first;
    per_cpu->uptime.time_accumulated = 0;

    if ( per_cpu->overflow > 0 ) {
      call_handler(
        ctx,
        per_cpu->uptime.bt,
        RTEMS_RECORD_PER_CPU_OVERFLOW,
        per_cpu->overflow
      );
      per_cpu->overflow = 0;
    }

    for ( index = begin_index; index < per_cpu->item_index; ++index ) {
      const rtems_record_item_64 *item;

      item = &per_cpu->items[ index ];
      visit( ctx, item->event, item->data );
    }
  }
}

static void process_per_cpu_head(
  rtems_record_client_context *ctx,
  rtems_record_client_per_cpu *per_cpu,
  uint64_t                     data
)
{
  uint32_t last_tail;
  uint32_t last_head;
  uint32_t last_capacity;
  uint32_t new_tail;
  uint32_t new_head;
  uint32_t new_content;
  uint32_t content;

  new_tail = per_cpu->tail[ per_cpu->tail_head_index ];
  new_head = (uint32_t) data;
  content = new_head - new_tail;

  per_cpu->head[ per_cpu->tail_head_index ] = new_head;
  per_cpu->tail_head_index ^= 1;

  if ( content >= ctx->count ) {
    /*
     * This is a complete ring buffer overflow, the server will detect this
     * also.  It sets the tail to the head plus one and sends us all the
     * content.  This reduces the ring buffer capacity to zero.  So, during
     * transfer, new events will overwrite items in transfer.  This is handled
     * by resolve_hold_back().
     */
    per_cpu->tail[ per_cpu->tail_head_index ^ 1 ] = new_head + 1;
    signal_overflow( per_cpu, content - ctx->count + 1 );
    return;
  }

  last_tail = per_cpu->tail[ per_cpu->tail_head_index ];
  last_head = per_cpu->head[ per_cpu->tail_head_index ];

  if ( last_tail == last_head ) {
    if ( per_cpu->uptime.bt == 0 ) {
      /*
       * This is a special case during initial ramp up.  We hold back the items
       * to deduce the uptime of the first item via resolve_hold_back().
       */
      per_cpu->hold_back = true;
    } else {
      resolve_hold_back( ctx, per_cpu );
    }

    return;
  }

  last_capacity = ( last_tail - last_head - 1 ) & ( ctx->count - 1 );
  new_content = new_head - last_head;

  if ( new_content <= last_capacity || per_cpu->hold_back ) {
    resolve_hold_back( ctx, per_cpu );
    return;
  }

  signal_overflow( per_cpu, new_content - last_capacity );
}

static rtems_record_client_status process_per_cpu_count(
  rtems_record_client_context *ctx,
  uint64_t                     data
)
{
  size_t                per_cpu_items;
  rtems_record_item_64 *items;
  uint32_t              cpu;

  if ( ctx->count != 0 ) {
    return error( ctx, RTEMS_RECORD_CLIENT_ERROR_DOUBLE_PER_CPU_COUNT );
  }

  if ( ctx->cpu_count == 0 ) {
    return error( ctx, RTEMS_RECORD_CLIENT_ERROR_NO_CPU_MAX );
  }

  ctx->count = (uint32_t) data;

  /*
   * The ring buffer capacity plus two items for RTEMS_RECORD_PROCESSOR and
   * RTEMS_RECORD_PER_CPU_TAIL.
   */
  per_cpu_items = ctx->count + 1;

  items = malloc( per_cpu_items * ctx->cpu_count * sizeof( *items ) );

  if ( items == NULL ) {
    return error( ctx, RTEMS_RECORD_CLIENT_ERROR_NO_MEMORY );
  }

  for ( cpu = 0; cpu < ctx->cpu_count; ++cpu ) {
    ctx->per_cpu[ cpu ].items = items;
    items += per_cpu_items;
  }

  return RTEMS_RECORD_CLIENT_SUCCESS;
}

static rtems_record_client_status hold_back(
  rtems_record_client_context *ctx,
  rtems_record_client_per_cpu *per_cpu,
  uint32_t                     time_event,
  rtems_record_event           event,
  uint64_t                     data
)
{
  if ( event != RTEMS_RECORD_PER_CPU_HEAD ) {
    uint32_t item_index;

    item_index = per_cpu->item_index;

    if ( item_index <= ctx->count ) {
      per_cpu->items[ item_index ].event = time_event;
      per_cpu->items[ item_index ].data = data;
      per_cpu->item_index = item_index + 1;
    } else {
      return error( ctx, RTEMS_RECORD_CLIENT_ERROR_PER_CPU_ITEMS_OVERFLOW );
    }
  } else {
    return call_handler( ctx, 0, RTEMS_RECORD_GET_EVENT( time_event ), data );
  }

  return RTEMS_RECORD_CLIENT_SUCCESS;
}

static uint64_t time_bt(
  const rtems_record_client_context *ctx,
  rtems_record_client_per_cpu       *per_cpu,
  uint32_t                           time
)
{
  uint64_t bt;

  if ( time != 0 ) {
    uint32_t delta;

    delta = ( time - per_cpu->uptime.time_last ) & TIME_MASK;
    per_cpu->uptime.time_last = time;
    per_cpu->uptime.time_accumulated += delta;
    bt = ( per_cpu->uptime.time_accumulated * ctx->to_bt_scaler ) >> 31;
    bt += per_cpu->uptime.bt;
  } else {
    bt = 0;
  }

  return bt;
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

  per_cpu = &ctx->per_cpu[ ctx->cpu ];
  time = RTEMS_RECORD_GET_TIME( time_event );
  event = RTEMS_RECORD_GET_EVENT( time_event );

  switch ( event ) {
    case RTEMS_RECORD_PROCESSOR:
      if ( data >= ctx->cpu_count ) {
        return error( ctx, RTEMS_RECORD_CLIENT_ERROR_UNSUPPORTED_CPU );
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
      per_cpu->uptime.bt += data << 32;
      time = 0;
      break;
    case RTEMS_RECORD_PER_CPU_TAIL:
      per_cpu->tail[ per_cpu->tail_head_index ] = (uint32_t) data;
      break;
    case RTEMS_RECORD_PER_CPU_HEAD:
      process_per_cpu_head( ctx, per_cpu, data );
      break;
    case RTEMS_RECORD_PROCESSOR_MAXIMUM:
      if ( data >= RTEMS_RECORD_CLIENT_MAXIMUM_CPU_COUNT ) {
        return error( ctx, RTEMS_RECORD_CLIENT_ERROR_UNSUPPORTED_CPU_MAX );
      }

      if ( ctx->cpu_count != 0 ) {
        return error( ctx, RTEMS_RECORD_CLIENT_ERROR_DOUBLE_CPU_MAX );
      }

      ctx->cpu_count = (uint32_t) data + 1;
      break;
    case RTEMS_RECORD_PER_CPU_COUNT:
      status = process_per_cpu_count( ctx, data );

      if ( status != RTEMS_RECORD_CLIENT_SUCCESS ) {
        return status;
      }

      break;
    case RTEMS_RECORD_FREQUENCY:
      set_to_bt_scaler( ctx, (uint32_t) data );
      break;
    case RTEMS_RECORD_VERSION:
      if ( data != RTEMS_RECORD_THE_VERSION ) {
        return error( ctx, RTEMS_RECORD_CLIENT_ERROR_UNSUPPORTED_VERSION );
      }

      break;
    default:
      break;
  }

  if ( per_cpu->hold_back ) {
    return hold_back( ctx, per_cpu, time_event, event, data );
  }

  return call_handler( ctx, time_bt( ctx, per_cpu, time ), event, data );
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

void rtems_record_client_destroy(
  rtems_record_client_context *ctx
)
{
  uint32_t cpu;

  for ( cpu = 0; cpu < ctx->cpu_count; ++cpu ) {
    rtems_record_client_per_cpu *per_cpu;

    ctx->cpu = cpu;
    per_cpu = &ctx->per_cpu[ cpu ];
    per_cpu->head[ per_cpu->tail_head_index ^ 1 ] =
      per_cpu->head[ per_cpu->tail_head_index ];
    resolve_hold_back( ctx, per_cpu );
  }

  free( ctx->per_cpu[ 0 ].items );
}
