/*
 *  Debugger test remote.
 *
 *  Copyright (c) 2016 Chris Johns (chrisj@rtems.org)
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "tmacros.h"

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include <rtems/rtems-debugger.h>
#include <rtems/debugger/rtems-debugger-server.h>
#include <rtems/debugger/rtems-debugger-remote.h>

#include "system.h"

/**
 * Remote data.
 */
typedef struct
{
  int    connect_count;
  bool   connected;
  size_t out;
  size_t in;
} rtems_debugger_remote_test;

static const char* out[] =
{
  "+",
  "xxxxx",
  "$x#aa",

  "$qSupported:multiprocess+;swbreak+;hwbreak+;qRelocInsn+;fork-events+;"
  "vfork-events+;exec-events+;vContSupported+;QThreadEvents+;no-resumed+#df",

  "$vMustReplyEmpty#3a",
  "+",

  "$Hgp0.0#ad",
  "+",

  "$qTStatus#49",
  "+",

  "$?#3f",
  "+",

  "$qfThreadInfo#bb",
  "+",

  "$qsThreadInfo#c8",
  "-",
  "+",

  "$qAttached:1#fa",
  "+",

  "$Hc-1#09",
  "+",

  "$qOffsets#4b",
  "+",

  "$g#67"
  "+",

  "$D;1#b0",
  "+"
};

static const char* in[] =
{
  /*  0 */
  "-",

  /*  1 */
  "+",
  "$qSupported:PacketSize=4096;QNonStop-;multiprocess+;swbreak+;hwbreak-;"
  "qRelocInsn-;fork-events-;vfork-events-;exec-events-;vContSupported+;"
  "QThreadEvents-;no-resumed+#b3",

  /*  3 */
  "+",
  "$#00",

  /*  5 */
  "+",
  "$OK#9a",

  /*  7 */
  "+",
  "$#00",

  /*  9 */
  "+",
  "$T00thread:p1.0a010001;#23",

  /* 11 */
  "+",
  "**",

  /* 13 */
  "+",
  "$l#6c",
  "$l#6c",

  /* 16 */
  "+",
  "$1#31",

  /* 18 */
  "+",
  "$OK#9a",

  /* 20 */
  "+",
  "$#00",

  /* 22 */
  "+",
  "**",

  "+",
  "$OK#9a"
};

static int
test_remote_begin(rtems_debugger_remote* remote, const char* device)
{
  rtems_debugger_remote_test* test;

  rtems_debugger_printf("error: rtems-db: test remote: begin\n");

  rtems_debugger_lock();

  /*
   * Check the device.
   */
  rtems_test_assert(strcmp(device, "something") == 0);

  test = malloc(sizeof(rtems_debugger_remote_test));
  rtems_test_assert(test != NULL);

  remote->data = test;

  test->connect_count = 0;
  test->connected = false;
  test->out = 0;
  test->in = 0;

  rtems_debugger_unlock();

  return 0;
}

static int
test_remote_end(rtems_debugger_remote* remote)
{
  rtems_debugger_remote_test* test;

  rtems_debugger_printf("error: rtems-db: test remote: end\n");

  rtems_debugger_lock();

  rtems_test_assert(remote != NULL);
  rtems_test_assert(remote->data != NULL);
  test = (rtems_debugger_remote_test*) remote->data;

  test->connected = false;

  free(test);

  rtems_debugger_unlock();

  return 0;
}

static int
test_remote_connect(rtems_debugger_remote* remote)
{
  rtems_debugger_remote_test* test;

  rtems_test_assert(remote != NULL);
  rtems_test_assert(remote->data != NULL);
  test = (rtems_debugger_remote_test*) remote->data;

  if (test->connect_count > 0) {
    rtems_event_set out = 0;
    rtems_test_assert(rtems_event_receive(RTEMS_EVENT_1,
                                          RTEMS_EVENT_ALL | RTEMS_WAIT,
                                          RTEMS_NO_TIMEOUT,
                                          &out) == RTEMS_SUCCESSFUL);
  }

  rtems_debugger_printf("error: rtems-db: test remote: connect\n");

  ++test->connect_count;
  test->connected = true;
  test->out = 0;
  test->in = 0;

  return 0;
}

static int
test_remote_disconnect(rtems_debugger_remote* remote)
{
  rtems_debugger_remote_test* test;

  rtems_test_assert(remote != NULL);
  rtems_test_assert(remote->data != NULL);
  test = (rtems_debugger_remote_test*) remote->data;

  rtems_debugger_printf("rtems-db: test remote: disconnect host\n");

  rtems_debugger_lock();

  rtems_test_assert(test->connected == true);

  test->connected = false;

  rtems_debugger_unlock();

  return 0;
}

static bool
test_remote_isconnected(rtems_debugger_remote* remote)
{
  rtems_debugger_remote_test* test;
  bool                        isconnected;
  rtems_test_assert(remote != NULL);
  rtems_test_assert(remote->data != NULL);
  test = (rtems_debugger_remote_test*) remote->data;
  isconnected = test != NULL && test->connected;
  rtems_debugger_printf("rtems-db: test remote: isconnected: %s\n",
                        isconnected ? "connected" : "not-connected");
  return isconnected;
}

static void
test_remote_print(const char* label, const char* buf, size_t size)
{
  printf(" remote: %s: ", label);
  while (size-- > 0) {
    printf("%c", *buf++);
  }
  printf("\n");
}

static ssize_t
test_remote_receive(rtems_debugger_remote* remote,
                    void*                  buf,
                    size_t                 nbytes)
{
  rtems_debugger_remote_test* test;
  size_t                      len;
  rtems_test_assert(remote != NULL);
  rtems_test_assert(remote->data != NULL);
  test = (rtems_debugger_remote_test*) remote->data;
  rtems_test_assert(test->out < RTEMS_DEBUGGER_NUMOF(out));
  len = strlen(out[test->out]);
  printf(" remote: rx: message=%zu length=%zu\n", test->out, len);
  test_remote_print("rx", out[test->out], len);
  rtems_test_assert(len < nbytes);
  memcpy(buf, out[test->out++], len);
  return len;
}

static ssize_t
test_remote_send(rtems_debugger_remote* remote,
                const void*            buf,
                size_t                 nbytes)
{
  rtems_debugger_remote_test* test;
  size_t                      len;
  bool                        no_match;
  rtems_test_assert(remote != NULL);
  rtems_test_assert(remote->data != NULL);
  test = (rtems_debugger_remote_test*) remote->data;
  rtems_test_assert(test->in < RTEMS_DEBUGGER_NUMOF(in));
  len = strlen(in[test->in]);
  no_match = len == 2 && strcmp(in[test->in], "**") == 0;
  printf(" remote: tx: message=%zu length=%zu\n", test->in, len);
  if (!no_match)
    rtems_test_assert(len == nbytes);
  test_remote_print("tx", buf, nbytes);
  if (!no_match)
    rtems_test_assert(memcmp(buf, in[test->in], nbytes) == 0);
  test->in++;
  return nbytes;
}

static rtems_debugger_remote remote_test =
{
  .name = "test",
  .begin = test_remote_begin,
  .end = test_remote_end,
  .connect = test_remote_connect,
  .disconnect = test_remote_disconnect,
  .isconnected = test_remote_isconnected,
  .read = test_remote_receive,
  .write = test_remote_send
};

int
rtems_debugger_register_test_remote(void)
{
  return rtems_debugger_remote_register(&remote_test);
}
