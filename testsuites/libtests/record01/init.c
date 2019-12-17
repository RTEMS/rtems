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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/record.h>
#include <rtems/recordserver.h>
#include <rtems.h>

#include <sys/endian.h>
#include <sys/socket.h>

#include <string.h>
#include <unistd.h>

#include <netinet/in.h>

#ifdef RTEMS_NETWORKING
#include <rtems/rtems_bsdnet.h>
#endif

#include <tmacros.h>

const char rtems_test_name[] = "RECORD 1";

#define ITEM_COUNT 4

#define ITEM_SIZE (ITEM_COUNT * sizeof(rtems_record_item))

typedef struct {
  Record_Control control;
  rtems_record_item items[ITEM_COUNT];
} test_context;

static test_context test_instance;

const Record_Configuration _Record_Configuration = {
  .item_count = ITEM_COUNT
};

#define UE(user) RTEMS_RECORD_USER(user)

#define TE(t, e) RTEMS_RECORD_TIME_EVENT(t, e)

static const rtems_record_item expected_items_0[ITEM_COUNT] = {
  { .event = TE(2, UE(1)), .data = 3 }
};

static const rtems_record_item expected_items_1[ITEM_COUNT] = {
  { .event = TE(2, UE(1)), .data = 3 },
  { .event = TE(6, UE(5)), .data = 7 }
};

static const rtems_record_item expected_items_2[ITEM_COUNT] = {
  { .event = TE(2, UE(1)), .data = 3 },
  { .event = TE(6, UE(5)), .data = 7 },
  { .event = TE(10, UE(9)), .data = 11 }
};

static const rtems_record_item expected_items_3[ITEM_COUNT] = {
  { .event = TE(2, UE(1)), .data = 3 },
  { .event = TE(6, UE(5)), .data = 7 },
  { .event = TE(10, UE(9)), .data = 11 },
  { .event = TE(14, UE(13)), .data = 15 }
};

static const rtems_record_item expected_items_4[ITEM_COUNT] = {
  { .event = TE(18, UE(17)), .data = 19 },
  { .event = TE(6, UE(5)), .data = 7 },
  { .event = TE(10, UE(9)), .data = 11 },
  { .event = TE(14, UE(13)), .data = 15 }
};

static const rtems_record_item expected_items_5[ITEM_COUNT] = {
  { .event = TE(2, UE(1)), .data = 3 }
};

static const rtems_record_item expected_items_6[ITEM_COUNT] = {
  { .event = TE(2, UE(1)), .data = 3 },
  { .event = TE(6, UE(5)), .data = 7 }
};

static const rtems_record_item expected_items_7[ITEM_COUNT] = {
  { .event = TE(2, UE(1)), .data = 3 },
  { .event = TE(6, UE(5)), .data = 7 },
  { .event = TE(10, UE(9)), .data = 11 }
};

static const rtems_record_item expected_items_8[] = {
  { .event = TE(0, RTEMS_RECORD_PROCESSOR), .data = 0 },
  { .event = TE(0, RTEMS_RECORD_PER_CPU_TAIL), .data = 0 },
  { .event = TE(0, RTEMS_RECORD_PER_CPU_HEAD), .data = 3 },
  { .event = TE(2, UE(1)), .data = 3 },
  { .event = TE(5, UE(4)), .data = 6 },
  { .event = TE(8, UE(7)), .data = 9 }
};

static const rtems_record_item expected_items_9[] = {
  { .event = TE(0, RTEMS_RECORD_PROCESSOR), .data = 0 },
  { .event = TE(0, RTEMS_RECORD_PER_CPU_TAIL), .data = 3 },
  { .event = TE(0, RTEMS_RECORD_PER_CPU_HEAD), .data = 5 },
  { .event = TE(11, UE(10)), .data = 12 },
  { .event = TE(14, UE(13)), .data = 15 }
};

static const rtems_record_item expected_items_10[] = {
  { .event = TE(0, RTEMS_RECORD_PROCESSOR), .data = 0 },
  { .event = TE(0, RTEMS_RECORD_PER_CPU_TAIL), .data = 5 },
  { .event = TE(0, RTEMS_RECORD_PER_CPU_HEAD), .data = 8 },
  { .event = TE(17, UE(16)), .data = 18 },
  { .event = TE(20, UE(19)), .data = 21 },
  { .event = TE(23, UE(22)), .data = 24 }
};

static const rtems_record_item expected_items_11[] = {
  { .event = TE(0, RTEMS_RECORD_PROCESSOR), .data = 0 },
  { .event = TE(0, RTEMS_RECORD_PER_CPU_TAIL), .data = 8 },
  { .event = TE(0, RTEMS_RECORD_PER_CPU_HEAD), .data = 9 },
  { .event = TE(26, UE(25)), .data = 27 }
};

static const rtems_record_item expected_items_12[] = {
  { .event = TE(0, RTEMS_RECORD_PROCESSOR), .data = 0 },
  { .event = TE(0, RTEMS_RECORD_PER_CPU_TAIL), .data = 9 },
  { .event = TE(0, RTEMS_RECORD_PER_CPU_HEAD), .data = 15 },
  { .event = TE(38, UE(37)), .data = 39 },
  { .event = TE(41, UE(40)), .data = 42 },
  { .event = TE(44, UE(43)), .data = 45 }
};

#ifdef RTEMS_NETWORKING
static const rtems_record_item expected_items_13[] = {
  { .event = TE(0, RTEMS_RECORD_THREAD_ID), .data = 0x9010001 },
  {
    .event = TE(0, RTEMS_RECORD_THREAD_NAME),
    .data = rtems_build_name('E', 'L', 'D', 'I')
  },
  { .event = TE(0, RTEMS_RECORD_THREAD_ID), .data = 0xa010001 },
  {
    .event = TE(0, RTEMS_RECORD_THREAD_NAME),
    .data = rtems_build_name(' ', '1', 'I', 'U')
  },
  { .event = TE(0, RTEMS_RECORD_THREAD_ID), .data = 0xa010002 },
  {
    .event = TE(0, RTEMS_RECORD_THREAD_NAME),
    .data = rtems_build_name('k', 'w', 't', 'n')
  },
  { .event = TE(0, RTEMS_RECORD_THREAD_ID), .data = 0xa010003 },
  {
    .event = TE(0, RTEMS_RECORD_THREAD_NAME),
    .data = rtems_build_name('D', 'R', 'C', 'R')
  }
};
#endif

static void init_context(test_context *ctx)
{
  memset(ctx, 0, sizeof(*ctx));
  ctx->control.mask = ITEM_COUNT - 1;
}

static void test_capacity(const Record_Control *control)
{
  unsigned int capacity;

  capacity = _Record_Capacity(control, 0, 0);
  rtems_test_assert(capacity == 3);

  capacity = _Record_Capacity(control, 0, 1);
  rtems_test_assert(capacity == 2);

  capacity = _Record_Capacity(control, 0, 2);
  rtems_test_assert(capacity == 1);

  capacity = _Record_Capacity(control, 0, 3);
  rtems_test_assert(capacity == 0);

  capacity = _Record_Capacity(control, 3, 3);
  rtems_test_assert(capacity == 3);

  capacity = _Record_Capacity(control, 3, 0);
  rtems_test_assert(capacity == 2);

  capacity = _Record_Capacity(control, 3, 1);
  rtems_test_assert(capacity == 1);

  capacity = _Record_Capacity(control, 3, 2);
  rtems_test_assert(capacity == 0);
}

static void test_index(const Record_Control *control)
{
  unsigned int index;

  index = _Record_Index(control, 0);
  rtems_test_assert(index == 0);

  index = _Record_Index(control, 1);
  rtems_test_assert(index == 1);

  index = _Record_Index(control, 2);
  rtems_test_assert(index == 2);

  index = _Record_Index(control, 3);
  rtems_test_assert(index == 3);

  index = _Record_Index(control, 4);
  rtems_test_assert(index == 0);
}

static void test_add_2_items(test_context *ctx, Record_Control *control)
{
  rtems_record_context rc;

  init_context(ctx);

  rtems_record_prepare(&rc);
  rtems_test_assert(rc.control == control);
  rtems_test_assert(rc.head == 0);
  rtems_test_assert(_Record_Head(control) == 0);
  rtems_test_assert(_Record_Tail(control) == 0);

  rc.now = RTEMS_RECORD_TIME_EVENT(2, 0);
  rtems_record_add(&rc, UE(1), 3);
  rtems_test_assert(rc.head == 1);
  rtems_test_assert(memcmp(control->Items, expected_items_0, ITEM_SIZE) == 0);
  rtems_test_assert(_Record_Head(control) == 0);
  rtems_test_assert(_Record_Tail(control) == 0);

  rc.now = RTEMS_RECORD_TIME_EVENT(6, 0);
  rtems_record_add(&rc, UE(5), 7);
  rtems_record_commit(&rc);
  rtems_test_assert(rc.head == 2);
  rtems_test_assert(memcmp(control->Items, expected_items_1, ITEM_SIZE) == 0);
  rtems_test_assert(_Record_Head(control) == 2);
  rtems_test_assert(_Record_Tail(control) == 0);
}

static void test_add_3_items(test_context *ctx, Record_Control *control)
{
  rtems_record_context rc;
  rtems_interrupt_level level;

  init_context(ctx);

  rtems_interrupt_local_disable(level);
  rtems_record_prepare_critical(&rc, _Per_CPU_Get());
  rtems_test_assert(rc.control == control);
  rtems_test_assert(rc.head == 0);
  rtems_test_assert(_Record_Head(control) == 0);
  rtems_test_assert(_Record_Tail(control) == 0);

  rc.now = RTEMS_RECORD_TIME_EVENT(2, 0);
  rtems_record_add(&rc, UE(1), 3);
  rtems_test_assert(rc.head == 1);
  rtems_test_assert(memcmp(control->Items, expected_items_5, ITEM_SIZE) == 0);
  rtems_test_assert(_Record_Head(control) == 0);
  rtems_test_assert(_Record_Tail(control) == 0);

  rc.now = RTEMS_RECORD_TIME_EVENT(6, 0);
  rtems_record_add(&rc, UE(5), 7);
  rtems_test_assert(rc.head == 2);
  rtems_test_assert(memcmp(control->Items, expected_items_6, ITEM_SIZE) == 0);
  rtems_test_assert(_Record_Head(control) == 0);
  rtems_test_assert(_Record_Tail(control) == 0);

  rc.now = RTEMS_RECORD_TIME_EVENT(10, 0);
  rtems_record_add(&rc, UE(9), 11);
  rtems_record_commit_critical(&rc);
  rtems_interrupt_local_enable(level);
  rtems_test_assert(rc.head == 3);
  rtems_test_assert(memcmp(control->Items, expected_items_7, ITEM_SIZE) == 0);
  rtems_test_assert(_Record_Head(control) == 3);
  rtems_test_assert(_Record_Tail(control) == 0);
}

static void set_time(rtems_record_item *item, uint32_t time)
{
  uint32_t event;

  event = item->event;
  event &= 0x3ff;
  event |= time << 10;
  item->event = event;
}

static void test_produce(test_context *ctx, Record_Control *control)
{
  init_context(ctx);

  rtems_record_produce(UE(1), 3);
  set_time(&control->Items[0], 2);
  rtems_test_assert(memcmp(control->Items, expected_items_0, ITEM_SIZE) == 0);
  rtems_test_assert(_Record_Head(control) == 1);
  rtems_test_assert(_Record_Tail(control) == 0);

  rtems_record_produce(UE(5), 7);
  set_time(&control->Items[1], 6);
  rtems_test_assert(memcmp(control->Items, expected_items_1, ITEM_SIZE) == 0);
  rtems_test_assert(_Record_Head(control) == 2);
  rtems_test_assert(_Record_Tail(control) == 0);

  rtems_record_produce(UE(9), 11);
  set_time(&control->Items[2], 10);
  rtems_test_assert(memcmp(control->Items, expected_items_2, ITEM_SIZE) == 0);
  rtems_test_assert(_Record_Head(control) == 3);
  rtems_test_assert(_Record_Tail(control) == 0);

  rtems_record_produce(UE(13), 15);
  set_time(&control->Items[3], 14);
  rtems_test_assert(memcmp(control->Items, expected_items_3, ITEM_SIZE) == 0);
  rtems_test_assert(_Record_Head(control) == 4);
  rtems_test_assert(_Record_Tail(control) == 0);

  rtems_record_produce(UE(17), 19);
  set_time(&control->Items[0], 18);
  rtems_test_assert(memcmp(control->Items, expected_items_4, ITEM_SIZE) == 0);
  rtems_test_assert(_Record_Head(control) == 5);
  rtems_test_assert(_Record_Tail(control) == 0);
}

static void test_produce_2(test_context *ctx, Record_Control *control)
{
  init_context(ctx);

  rtems_record_produce_2(UE(1), 3, UE(5), 7);
  set_time(&control->Items[0], 2);
  set_time(&control->Items[1], 6);
  rtems_test_assert(memcmp(control->Items, expected_items_1, ITEM_SIZE) == 0);
  rtems_test_assert(_Record_Head(control) == 2);
  rtems_test_assert(_Record_Tail(control) == 0);

  rtems_record_produce(UE(9), 11);
  set_time(&control->Items[2], 10);
  rtems_test_assert(memcmp(control->Items, expected_items_2, ITEM_SIZE) == 0);
  rtems_test_assert(_Record_Head(control) == 3);
  rtems_test_assert(_Record_Tail(control) == 0);

  rtems_record_produce_2(UE(13), 15, UE(17), 19);
  set_time(&control->Items[3], 14);
  set_time(&control->Items[0], 18);
  rtems_test_assert(memcmp(control->Items, expected_items_4, ITEM_SIZE) == 0);
  rtems_test_assert(_Record_Head(control) == 5);
  rtems_test_assert(_Record_Tail(control) == 0);
}

static void test_produce_n(test_context *ctx, Record_Control *control)
{
  rtems_record_item items[5];

  init_context(ctx);

  items[0].event = UE(1);
  items[0].data = 3;
  items[1].event = UE(5);
  items[1].data = 7;
  items[2].event = UE(9);
  items[2].data = 11;
  items[3].event = UE(13);
  items[3].data = 15;
  items[4].event = UE(17);
  items[4].data = 19;
  rtems_record_produce_n(items, RTEMS_ARRAY_SIZE(items));
  set_time(&control->Items[1], 6);
  set_time(&control->Items[2], 10);
  set_time(&control->Items[3], 14);
  set_time(&control->Items[0], 18);
  rtems_test_assert(memcmp(control->Items, expected_items_4, ITEM_SIZE) == 0);
  rtems_test_assert(_Record_Head(control) == 5);
  rtems_test_assert(_Record_Tail(control) == 0);
}

typedef struct {
  size_t todo;
  const rtems_record_item *items;
} visitor_context;

static void visitor(const rtems_record_item *items, size_t count, void *arg)
{
  visitor_context *vctx;

  vctx = arg;
  rtems_test_assert(vctx->todo >= count);

  while (count > 0) {
    rtems_test_assert(memcmp(items, vctx->items, sizeof(*items)) == 0);
    ++items;
    ++vctx->items;
    --count;
    --vctx->todo;
  }
}

static void test_drain(test_context *ctx, Record_Control *control)
{
  visitor_context vctx;

  init_context(ctx);

  vctx.todo = 0;
  vctx.items = NULL;
  rtems_record_drain(visitor, &vctx);
  rtems_test_assert(vctx.todo == 0);

  rtems_record_produce(UE(1), 3);
  set_time(&control->Items[0], 2);
  rtems_record_produce(UE(4), 6);
  set_time(&control->Items[1], 5);
  rtems_record_produce(UE(7), 9);
  set_time(&control->Items[2], 8);

  vctx.todo = RTEMS_ARRAY_SIZE(expected_items_8);
  vctx.items = expected_items_8;
  rtems_record_drain(visitor, &vctx);
  rtems_test_assert(vctx.todo == 0);

  vctx.todo = 0;
  vctx.items = NULL;
  rtems_record_drain(visitor, &vctx);
  rtems_test_assert(vctx.todo == 0);

  rtems_record_produce(UE(10), 12);
  set_time(&control->Items[3], 11);
  rtems_record_produce(UE(13), 15);
  set_time(&control->Items[0], 14);

  vctx.todo = RTEMS_ARRAY_SIZE(expected_items_9);
  vctx.items = expected_items_9;
  rtems_record_drain(visitor, &vctx);
  rtems_test_assert(vctx.todo == 0);

  vctx.todo = 0;
  vctx.items = NULL;
  rtems_record_drain(visitor, &vctx);
  rtems_test_assert(vctx.todo == 0);

  rtems_record_produce(UE(16), 18);
  set_time(&control->Items[1], 17);
  rtems_record_produce(UE(19), 21);
  set_time(&control->Items[2], 20);
  rtems_record_produce(UE(22), 24);
  set_time(&control->Items[3], 23);

  vctx.todo = RTEMS_ARRAY_SIZE(expected_items_10);
  vctx.items = expected_items_10;
  rtems_record_drain(visitor, &vctx);
  rtems_test_assert(vctx.todo == 0);

  vctx.todo = 0;
  vctx.items = NULL;
  rtems_record_drain(visitor, &vctx);
  rtems_test_assert(vctx.todo == 0);

  rtems_record_produce(UE(25), 27);
  set_time(&control->Items[0], 26);

  vctx.todo = RTEMS_ARRAY_SIZE(expected_items_11);
  vctx.items = expected_items_11;
  rtems_record_drain(visitor, &vctx);
  rtems_test_assert(vctx.todo == 0);

  vctx.todo = 0;
  vctx.items = NULL;
  rtems_record_drain(visitor, &vctx);
  rtems_test_assert(vctx.todo == 0);

  rtems_record_produce(UE(28), 30);
  set_time(&control->Items[1], 29);
  rtems_record_produce(UE(31), 33);
  set_time(&control->Items[2], 32);
  rtems_record_produce(UE(34), 36);
  set_time(&control->Items[3], 35);
  rtems_record_produce(UE(37), 39);
  set_time(&control->Items[0], 38);
  rtems_record_produce(UE(40), 42);
  set_time(&control->Items[1], 41);
  rtems_record_produce(UE(43), 45);
  set_time(&control->Items[2], 44);

  vctx.todo = RTEMS_ARRAY_SIZE(expected_items_12);
  vctx.items = expected_items_12;
  rtems_record_drain(visitor, &vctx);
  rtems_test_assert(vctx.todo == 0);

  vctx.todo = 0;
  vctx.items = NULL;
  rtems_record_drain(visitor, &vctx);
  rtems_test_assert(vctx.todo == 0);
}

#ifdef RTEMS_NETWORKING
#define PORT 1234

typedef enum {
  HEADER_ARCH,
  HEADER_MULTILIB,
  HEADER_BSP,
  HEADER_VERSION_CONTROL_KEY,
  HEADER_TOOLS,
  HEADER_LAST
} header_state;

static uint32_t get_format(void)
{
  uint32_t format;

#if BYTE_ORDER == LITTLE_ENDIAN
#if __INTPTR_WIDTH__ == 32
  format = RTEMS_RECORD_FORMAT_LE_32;
#elif __INTPTR_WIDTH__ == 64
  format = RTEMS_RECORD_FORMAT_LE_64;
#else
#error "unexpected __INTPTR_WIDTH__"
#endif
#elif BYTE_ORDER == BIG_ENDIAN
#if __INTPTR_WIDTH__ == 32
  format = RTEMS_RECORD_FORMAT_BE_32;
#elif __INTPTR_WIDTH__ == 64
  format = RTEMS_RECORD_FORMAT_BE_64;
#else
#error "unexpected __INTPTR_WIDTH__"
#endif
#else
#error "unexpected BYTE_ORDER"
#endif

  return format;
}

static rtems_record_event hs_to_ev(header_state hs)
{
  switch (hs) {
    case HEADER_ARCH:
      return RTEMS_RECORD_ARCH;
    case HEADER_MULTILIB:
      return RTEMS_RECORD_MULTILIB;
    case HEADER_BSP:
      return RTEMS_RECORD_BSP;
    case HEADER_VERSION_CONTROL_KEY:
      return RTEMS_RECORD_VERSION_CONTROL_KEY;
    case HEADER_TOOLS:
      return RTEMS_RECORD_TOOLS;
    default:
      rtems_test_assert(0);
      return RTEMS_RECORD_EMPTY;
  }
}

static int connect_client(void)
{
  struct sockaddr_in addr;
  int fd;
  int rv;
  ssize_t n;
  uint32_t v;
  rtems_record_item item;
  rtems_record_item items[8];
  header_state hs;

  fd = socket(PF_INET, SOCK_STREAM, 0);
  rtems_test_assert(fd >= 0);

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  rv = connect(fd, (struct sockaddr *) &addr, sizeof(addr));
  rtems_test_assert(rv == 0);

  n = read(fd, &v, sizeof(v));
  rtems_test_assert(n == 4);
  rtems_test_assert(v == get_format());

  n = read(fd, &v, sizeof(v));
  rtems_test_assert(n == 4);
  rtems_test_assert(v == RTEMS_RECORD_MAGIC);

  n = read(fd, &item, sizeof(item));
  rtems_test_assert(n == (ssize_t) sizeof(item));
  rtems_test_assert(item.event == TE(0, RTEMS_RECORD_VERSION));
  rtems_test_assert(item.data == RTEMS_RECORD_THE_VERSION);

  n = read(fd, &item, sizeof(item));
  rtems_test_assert(n == (ssize_t) sizeof(item));
  rtems_test_assert(item.event == TE(0, RTEMS_RECORD_PROCESSOR_MAXIMUM));
  rtems_test_assert(item.data == 0);

  n = read(fd, &item, sizeof(item));
  rtems_test_assert(n == (ssize_t) sizeof(item));
  rtems_test_assert(item.event == TE(0, RTEMS_RECORD_PER_CPU_COUNT));
  rtems_test_assert(item.data == ITEM_COUNT);

  n = read(fd, &item, sizeof(item));
  rtems_test_assert(n == (ssize_t) sizeof(item));
  rtems_test_assert(item.event == TE(0, RTEMS_RECORD_FREQUENCY));
  rtems_test_assert(item.data == rtems_counter_frequency());

  hs = HEADER_ARCH;
  while (hs != HEADER_LAST) {
    n = read(fd, items, sizeof(items[0]));
    rtems_test_assert(n == (ssize_t) sizeof(items[0]));

    if (items[0].event != hs_to_ev(hs)) {
      ++hs;
    }

    rtems_test_assert(hs == HEADER_LAST || items[0].event == hs_to_ev(hs));
  }

  n = read(fd, &items[1], sizeof(expected_items_13) - sizeof(items[0]));
  rtems_test_assert(
    n == (ssize_t) (sizeof(expected_items_13) - sizeof(items[0]))
  );
  rtems_test_assert(
    memcmp(items, expected_items_13, sizeof(expected_items_13)) == 0
  );

  return fd;
}

static void produce_and_read(int fd, Record_Control *control)
{
  rtems_record_item items[6];
  ssize_t n;

  rtems_record_produce(UE(1), 3);
  set_time(&control->Items[0], 2);
  rtems_record_produce(UE(4), 6);
  set_time(&control->Items[1], 5);
  rtems_record_produce(UE(7), 9);
  set_time(&control->Items[2], 8);

  n = read(fd, items, sizeof(expected_items_8));
  rtems_test_assert(n == (ssize_t) sizeof(expected_items_8));
  rtems_test_assert(
    memcmp(items, expected_items_8, sizeof(expected_items_8)) == 0
  );

  rtems_record_produce(UE(10), 12);
  set_time(&control->Items[3], 11);
  rtems_record_produce(UE(13), 15);
  set_time(&control->Items[0], 14);

  n = read(fd, items, sizeof(expected_items_9));
  rtems_test_assert(n == (ssize_t) sizeof(expected_items_9));
  rtems_test_assert(
    memcmp(items, expected_items_9, sizeof(expected_items_9)) == 0
  );

  rtems_record_produce(UE(16), 18);
  set_time(&control->Items[1], 17);
  rtems_record_produce(UE(19), 21);
  set_time(&control->Items[2], 20);
  rtems_record_produce(UE(22), 24);
  set_time(&control->Items[3], 23);

  n = read(fd, items, sizeof(expected_items_10));
  rtems_test_assert(n == (ssize_t) sizeof(expected_items_10));
  rtems_test_assert(
    memcmp(items, expected_items_10, sizeof(expected_items_10)) == 0
  );

  rtems_record_produce(UE(25), 27);
  set_time(&control->Items[0], 26);

  n = read(fd, items, sizeof(expected_items_11));
  rtems_test_assert(n == (ssize_t) sizeof(expected_items_11));
  rtems_test_assert(
    memcmp(items, expected_items_11, sizeof(expected_items_11)) == 0
  );

  rtems_record_produce(UE(28), 30);
  set_time(&control->Items[1], 29);
  rtems_record_produce(UE(31), 33);
  set_time(&control->Items[2], 32);
  rtems_record_produce(UE(34), 36);
  set_time(&control->Items[3], 35);
  rtems_record_produce(UE(37), 39);
  set_time(&control->Items[0], 38);
  rtems_record_produce(UE(40), 42);
  set_time(&control->Items[1], 41);
  rtems_record_produce(UE(43), 45);
  set_time(&control->Items[2], 44);

  n = read(fd, items, sizeof(expected_items_12));
  rtems_test_assert(n == (ssize_t) sizeof(expected_items_12));
  rtems_test_assert(
    memcmp(items, expected_items_12, sizeof(expected_items_12)) == 0
  );
}

static void test_server(test_context *ctx, Record_Control *control)
{
  rtems_status_code sc;
  int rv;
  int fd;

  init_context(ctx);

  rv = rtems_bsdnet_initialize_network();
  rtems_test_assert(rv == 0);

  sc = rtems_record_start_server(1, PORT, 1);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  fd = connect_client();
  produce_and_read(fd, control);

  rv = close(fd);
  rtems_test_assert(rv == 0);
}
#endif

static void Init(rtems_task_argument arg)
{
  test_context *ctx;
  Per_CPU_Control *cpu_self;

  TEST_BEGIN();

  ctx = &test_instance;

  cpu_self = _Per_CPU_Get_snapshot();
  cpu_self->record = &ctx->control;

  init_context(ctx);
  test_capacity(&ctx->control);
  test_index(&ctx->control);
  test_add_2_items(ctx, &ctx->control);
  test_add_3_items(ctx, &ctx->control);
  test_produce(ctx, &ctx->control);
  test_produce_2(ctx, &ctx->control);
  test_produce_n(ctx, &ctx->control);
  test_drain(ctx, &ctx->control);
#ifdef RTEMS_NETWORKING
  test_server(ctx, &ctx->control);
#endif

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#ifdef RTEMS_NETWORKING
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 7

#define CONFIGURE_MAXIMUM_TASKS 3

#define CONFIGURE_MAXIMUM_TIMERS 1

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT
#else
#define CONFIGURE_MAXIMUM_TASKS 1
#endif

#define CONFIGURE_INIT_TASK_PRIORITY 2

#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
