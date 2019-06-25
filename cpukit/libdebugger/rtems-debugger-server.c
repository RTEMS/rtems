/*
 * Copyright (c) 2016-2019 Chris Johns <chrisj@rtems.org>.
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#define RTEMS_DEBUGGER_VERBOSE_LOCK 0

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>

#include <rtems/bspIo.h>
#include <rtems/score/smp.h>

#include <rtems/rtems-debugger.h>
#include <rtems/debugger/rtems-debugger-server.h>
#include <rtems/debugger/rtems-debugger-remote.h>

#include "rtems-debugger-target.h"
#include "rtems-debugger-threads.h"

/*
 * GDB Debugger Remote Server for RTEMS.
 */

/*
 * Hack to void including bsp.h. The reset needs a better API.
 */
extern void bsp_reset(void);

/*
 * Command lookup table.
 */
typedef int (*rtems_debugger_command)(uint8_t* buffer, int size);

typedef struct rtems_debugger_packet
{
  const char* const      label;
  rtems_debugger_command command;
} rtems_debugger_packet;

/**
 * Common error strings.
 */
static const char* const r_OK = "OK";
static const char* const r_E01 = "E01";

/*
 * Global Debugger.
 *
 * The server instance is allocated on the heap so memory is only used then the
 * server is running. A global is used because:
 *
 *  1. There can only be a single instance at once.
 *  2. The backend's need access to the data and holding pointers in the TCB
 *     for each thread is mess.
 *  3. The code is smaller and faster.
 */
rtems_debugger_server* rtems_debugger;

/**
 * Print lock ot make the prints sequential. This is to debug the debugger in
 * SMP.
 */
RTEMS_INTERRUPT_LOCK_DEFINE(static, printk_lock, "printk_lock")

void
rtems_debugger_printk_lock(rtems_interrupt_lock_context* lock_context)
{
  rtems_interrupt_lock_acquire(&printk_lock, lock_context);
}

void
rtems_debugger_printk_unlock(rtems_interrupt_lock_context* lock_context)
{
  rtems_interrupt_lock_release(&printk_lock, lock_context);
}

int
rtems_debugger_clean_printf(const char* format, ...)
{
  rtems_interrupt_lock_context lock_context;
  int                          len;
  va_list                      ap;
  va_start(ap, format);
  rtems_debugger_printk_lock(&lock_context);
  len = vprintk(format, ap);
  rtems_debugger_printk_unlock(&lock_context);
  va_end(ap);
  return len;
}

int
rtems_debugger_printf(const char* format, ...)
{
  rtems_interrupt_lock_context lock_context;
  int                          len;
  va_list                      ap;
  va_start(ap, format);
  rtems_debugger_printk_lock(&lock_context);
  printk("[CPU:%d] ", (int) _SMP_Get_current_processor ());
  len = vprintk(format, ap);
  rtems_debugger_printk_unlock(&lock_context);
  va_end(ap);
  return len;
}

bool
rtems_debugger_verbose(void)
{
  return rtems_debugger_server_flag(RTEMS_DEBUGGER_FLAG_VERBOSE);
}

static inline int
hex_decode(uint8_t ch)
{
  int i;
  if (ch >= '0' && ch <= '9')
    i = (int) (ch - '0');
  else if (ch >= 'a' && ch <= 'f')
    i = (int) (ch - 'a') + 10;
  else if (ch >= 'A' && ch <= 'F')
    i = (int) (ch - 'A') + 10;
  else
    i = -1;
  return i;
}

static inline uint8_t
hex_encode(int val)
{
  return "0123456789abcdef"[val & 0xf];
}

static inline DB_UINT
hex_decode_uint(const uint8_t* data)
{
  DB_UINT ui = 0;
  size_t  i;
  if (data[0] == '-') {
    if (data[1] == '1')
      ui = (DB_UINT) -1;
  }
  else {
    for (i = 0; i < (sizeof(ui) * 2); ++i) {
      int v = hex_decode(data[i]);
      if (v < 0)
        break;
      ui = (ui << 4) | v;
    }
  }
  return ui;
}

static inline int
hex_decode_int(const uint8_t* data)
{
  return (int) hex_decode_uint(data);
}

static bool
thread_id_decode(const char* data, DB_UINT* pid, DB_UINT* tid)
{
  bool is_extended = false;
  if (*data == 'p') {
    is_extended = true;
    ++data;
  }
  *pid = *tid = hex_decode_uint((const uint8_t*) data);
  if (is_extended) {
    const char* stop = strchr(data, '.');
    if (stop != NULL) {
      *tid = hex_decode_uint((const uint8_t*) stop + 1);
    }
  }
  return is_extended;
}

static inline bool
check_pid(DB_UINT pid)
{
  return pid == 0 || rtems_debugger->pid == (pid_t) pid;
}

void
rtems_debugger_lock(void)
{
  _Mutex_recursive_Acquire(&rtems_debugger->lock);
}

void
rtems_debugger_unlock(void)
{
  _Mutex_recursive_Release(&rtems_debugger->lock);
}

static int
rtems_debugger_lock_create(void)
{
  _Mutex_recursive_Initialize_named(&rtems_debugger->lock, "DBlock");
  return 0;
}

static int
rtems_debugger_lock_destroy(void)
{
  return 0;
}

static int
rtems_debugger_task_create(const char*         name,
                           rtems_task_priority priority,
                           size_t              stack_size,
                           rtems_task_entry    entry_point,
                           rtems_task_argument argument,
                           rtems_id*           id)
{
  rtems_name        tname;
  rtems_status_code sc;

  tname = rtems_build_name(name[0], name[1], name[2], name[3]);

  sc = rtems_task_create (tname,
                          priority,
                          stack_size,
                          RTEMS_PREEMPT | RTEMS_NO_ASR,
                          RTEMS_LOCAL | RTEMS_FLOATING_POINT,
                          id);
  if (sc != RTEMS_SUCCESSFUL) {
    *id = 0;
    rtems_debugger_printf("error: rtems-db: thread create: %s: %s\n",
                          name, rtems_status_text(sc));
    errno = EIO;
    return -1;
  }

  sc = rtems_task_start(*id, entry_point, argument);
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_debugger_printf("error: rtems-db: thread start: %s: %s\n",
                          name, rtems_status_text(sc));
    rtems_task_delete(*id);
    *id = 0;
    errno = EIO;
    return -1;
  }

  return 0;
}

static int
rtems_debugger_task_destroy(const char*    name,
                            rtems_id       id,
                            volatile bool* finished,
                            int            timeout)
{
  while (timeout) {
    bool has_finished;

    rtems_debugger_lock();
    has_finished = *finished;
    rtems_debugger_unlock();

    if (has_finished)
      break;

    usleep(RTEMS_DEBUGGER_POLL_WAIT);
    if (timeout < RTEMS_DEBUGGER_POLL_WAIT)
      timeout = 0;
    else
      timeout -= RTEMS_DEBUGGER_POLL_WAIT;
  }

  if (timeout == 0) {
    rtems_debugger_printf("rtems-db: %s not stopping, killing\n", name);
    rtems_task_delete(id);
  }
  return 0;
}

bool
rtems_debugger_server_running(void)
{
  bool running;
  rtems_debugger_lock();
  running = rtems_debugger->server_running;
  rtems_debugger_unlock();
  return running;
}

rtems_debugger_remote*
rtems_debugger_remote_handle(void)
{
  rtems_debugger_remote* remote;
  rtems_debugger_lock();
  remote = rtems_debugger->remote;
  rtems_debugger_unlock();
  return remote;
}

bool
rtems_debugger_connected(void)
{
  bool isconnected = false;
  rtems_debugger_lock();
  if (rtems_debugger->remote != NULL)
    isconnected = rtems_debugger->remote->isconnected(rtems_debugger->remote);
  rtems_debugger_unlock();
  return isconnected;
}

bool
rtems_debugger_server_events_running(void)
{
  return rtems_debugger->events_running;
}

void
rtems_debugger_server_events_signal(void)
{
  _Condition_Signal(&rtems_debugger->server_cond);
}

static void
rtems_debugger_server_events_wait(void)
{
  _Condition_Wait_recursive(&rtems_debugger->server_cond, &rtems_debugger->lock);
}

static int
rtems_debugger_remote_connect(void)
{
  rtems_debugger_remote* remote = rtems_debugger_remote_handle();
  if (remote != NULL) {
    if (!remote->isconnected(remote))
      return remote->connect(remote);
  }
  errno = EIO;
  return -1;
}

static int
rtems_debugger_remote_disconnect(void)
{
  rtems_debugger_remote* remote = rtems_debugger_remote_handle();
  if (remote != NULL) {
    if (remote->isconnected(remote))
      return remote->disconnect(remote);
  }
  errno = EIO;
  return -1;
}

static int
rtems_debugger_remote_receive(uint8_t* buffer, size_t size)
{
  rtems_debugger_remote* remote = rtems_debugger_remote_handle();
  ssize_t len = remote->read(remote, buffer, size);
  if (len < 0 && errno != EAGAIN)
    rtems_debugger_printf("rtems-db: read: (%d) %s\n",
                          errno, strerror(errno));
  return (int) len;
}

static int
rtems_debugger_remote_send(void)
{
  const uint8_t* buffer = rtems_debugger->output;
  ssize_t        size = rtems_debugger->output_level;

  if (rtems_debugger->output_level > RTEMS_DEBUGGER_BUFFER_SIZE) {
    rtems_debugger_printf("rtems-db: write too big: %d\n",
                          (int) rtems_debugger->output_level);
    return -1;
  }

  if (rtems_debugger->remote_debug) {
    size_t i = 0;
    rtems_debugger_printf("rtems-db: put:%4zu: ", rtems_debugger->output_level);
    while (i < rtems_debugger->output_level)
      rtems_debugger_clean_printf("%c", (char) rtems_debugger->output[i++]);
    rtems_debugger_clean_printf("\n");
  }

  while (size) {
    rtems_debugger_remote* remote = rtems_debugger_remote_handle();
    ssize_t                w;
    if (remote == NULL) {
      errno = EIO;
      return -1;
    }
    w = remote->write(remote, buffer, size);
    if (w < 0 && errno != EINTR) {
      rtems_debugger_printf("rtems-db: write: (%d) %s\n",
                            errno, strerror(errno));
      break;
    }
    else {
      size -= w;
      buffer += w;
    }
  }

  return (int) rtems_debugger->output_level;
}

static int
rtems_debugger_remote_send_ack(void)
{
  rtems_debugger->output[0] = '+';
  rtems_debugger->output_level = 1;
  return rtems_debugger_remote_send();
}

static int
rtems_debugger_remote_send_nack(void)
{
  rtems_debugger->output[0] = '-';
  rtems_debugger->output_level = 1;
  return rtems_debugger_remote_send();
}

static int
rtems_debugger_remote_packet_in(void)
{
  uint8_t buf[256];
  uint8_t state;
  int     in = 0;
  uint8_t csum = 0;
  uint8_t rx_csum = 0;
  bool    junk = false;
  bool    escaped = false;
  bool    remote_debug_header = true;

  /*
   * States:
   *  'H' : Looking for the start character '$', '-' or '+'.
   *  'P' : Looking for the checksum character '#' else buffer data.
   *  '1' : Looking for the first checksum character.
   *  '2' : Looking for the second checksum character.
   *  'F' : Finished.
   */

  state = 'H';

  while (state != 'F') {
    int r;
    int i;

    rtems_debugger_unlock();

    r = rtems_debugger_remote_receive(buf, sizeof(buf));

    rtems_debugger_lock();

    if (r <= 0) {
      /*
       * Timeout?
       */
      if (r < 0 && errno == EAGAIN) {
        if (rtems_debugger->ack_pending) {
          rtems_debugger_remote_send();
        }
        continue;
      }
      if (r == 0)
        rtems_debugger_printf("rtems-db: remote disconnected\n");
      return -1;
    }

    i = 0;

    while (i < r) {
      uint8_t c = buf[i++];

      if (rtems_debugger->remote_debug && remote_debug_header) {
        rtems_debugger_printf("rtems-db: get:%4d: ", r);
        remote_debug_header = false;
      }

      if (rtems_debugger->remote_debug)
        rtems_debugger_clean_printf("%c", c);

      switch (state) {
      case 'H':
        switch (c) {
        case '+':
          if (rtems_debugger->remote_debug) {
            rtems_debugger_clean_printf(" [[ACK%s]]\n",
                                  rtems_debugger->ack_pending ? "" : "?");
            remote_debug_header = true;
          }
          rtems_debugger->ack_pending = false;
          break;
        case '-':
          if (rtems_debugger->remote_debug) {
            rtems_debugger_clean_printf(" [[NACK]]\n");
            remote_debug_header = true;
          }
          /*
           * Resend.
           */
          rtems_debugger_remote_send();
          break;
        case '$':
          state = 'P';
          csum = 0;
          in = 0;
          if (junk && rtems_debugger->remote_debug) {
            rtems_debugger_clean_printf("\b [[junk dropped]]\nrtems-db: get:   : $");
            remote_debug_header = false;
          }
          break;
        case '\x3':
          if (rtems_debugger->remote_debug)
            rtems_debugger_clean_printf("^C [[BREAK]]\n");
          rtems_debugger->ack_pending = false;
          rtems_debugger->input[0] =  '^';
          rtems_debugger->input[1] =  'C';
          rtems_debugger->input[2] =  '\0';
          return 2;
        default:
          junk = true;
          break;
        }
        break;
      case 'P':
        if (c == '{' && !escaped) {
          escaped = true;
        }
        else if (c == '$' && !escaped) {
          csum = 0;
          in = 0;
          if (rtems_debugger->remote_debug) {
            rtems_debugger_clean_printf("\n");
            remote_debug_header = true;
          }
        }
        else if (c == '#' && !escaped) {
          rtems_debugger->input[in] = '\0';
          rx_csum = 0;
          state = '1';
        }
        else {
          if (in >= (RTEMS_DEBUGGER_BUFFER_SIZE - 1)) {
            rtems_debugger_printf("rtems-db: input buffer overflow\n");
            return -1;
          }
          csum += c;
          rtems_debugger->input[in++] = c;
        }
        break;
      case '1':
        rx_csum = (rx_csum << 4) | (uint8_t) hex_decode(c);
        state = '2';
        break;
      case '2':
        rx_csum = (rx_csum << 4) | (uint8_t) hex_decode(c);
        if (csum == rx_csum) {
          state = 'F';
          if (rtems_debugger->remote_debug)
            rtems_debugger_clean_printf("\n");
          rtems_debugger_remote_send_ack();
        }
        else {
          if (rtems_debugger->remote_debug) {
            rtems_debugger_clean_printf(" [[invalid checksum]]\n");
            remote_debug_header = true;
            rtems_debugger_remote_send_nack();
          }
          state = 'H';
        }
        break;
      case 'F':
          if (rtems_debugger->remote_debug)
            rtems_debugger_clean_printf(" [[extra data: 0x%02x]]", (int) c);
        break;
      default:
        rtems_debugger_printf("rtems-db: bad state\n");
        rtems_debugger_remote_send_nack();
        return -1;
      }
    }
  }

  return in;
}

static int
rtems_debugger_remote_packet_in_hex(uint8_t*    addr,
                                    const char* data,
                                    size_t      size)
{
  size_t i;
  for (i = 0; i < size; ++i) {
    *addr = (hex_decode(*data++) << 4);
    *addr++ |= hex_decode(*data++);
  }
  return 0;
}

#if KEEP_INCASE
static void
remote_packet_out_rewind(size_t size)
{
  size_t i = 0;
  while (rtems_debugger->output_level > 0 && i < size) {
    if (rtems_debugger->output_level > 1) {
      if (rtems_debugger->output[rtems_debugger->output_level - 1] == '}') {
        --rtems_debugger->output_level;
      }
    }
    --rtems_debugger->output_level;
    --i;
  }
}
#endif

static int
remote_packet_out_append_buffer(const char* buffer, size_t size)
{
  size_t ol = rtems_debugger->output_level;
  size_t i = 0;
  while (i < size) {
    char c = buffer[i++];
    if (c == '#' || c == '$') {
      if (rtems_debugger->output_level >= (RTEMS_DEBUGGER_BUFFER_SIZE - 1)) {
        rtems_debugger->output_level = ol;
        rtems_debugger_printf("rtems-db: output overflow\n");
        return -1;
      }
      rtems_debugger->output[rtems_debugger->output_level++] = '}';
      c ^= 0x20;
    }
    if (rtems_debugger->output_level >= (RTEMS_DEBUGGER_BUFFER_SIZE - 1)) {
      rtems_debugger->output_level = ol;
      rtems_debugger_printf("rtems-db: output overflow\n");
      return -1;
    }
    rtems_debugger->output[rtems_debugger->output_level++] = c;
  }
  return 0;
}

static int
remote_packet_out_append_hex(const uint8_t* data, size_t size)
{
  size_t ol = rtems_debugger->output_level;
  size_t i = 0;
  while (i < size) {
    uint8_t byte = data[i++];
    if (rtems_debugger->output_level >= (RTEMS_DEBUGGER_BUFFER_SIZE - 2)) {
      rtems_debugger->output_level = ol;
      rtems_debugger_printf("rtems-db: output overflow\n");
      return -1;
    }
    rtems_debugger->output[rtems_debugger->output_level++] = hex_encode(byte >> 4);
    rtems_debugger->output[rtems_debugger->output_level++] = hex_encode(byte);
  }
  return 0;
}

static int
remote_packet_out_append_str(const char* str)
{
  return remote_packet_out_append_buffer(str, strlen(str));
}

static int
remote_packet_out_append_vprintf(const char* fmt, va_list ap)
{
  int  len;
  char buffer[64];
  len = vsnprintf(buffer, sizeof(buffer), fmt, ap);
  return remote_packet_out_append_buffer(buffer, len);
}

static int
remote_packet_out_append(const char* fmt, ...)
{
  va_list ap;
  int     r;
  va_start(ap, fmt);
  r = remote_packet_out_append_vprintf(fmt, ap);
  va_end(ap);
  return r;
}

static void
remote_packet_out_reset(void)
{
  rtems_debugger->output_level = 1;
  rtems_debugger->output[0] = '$';
}

static int
remote_packet_out_buffer(const char* buffer, size_t size)
{
  remote_packet_out_reset();
  return remote_packet_out_append_buffer(buffer, size);
}

static int
remote_packet_out_str(const char* str)
{
  remote_packet_out_reset();
  return remote_packet_out_append_buffer(str, strlen(str));
}

static int
remote_packet_out(const char* fmt, ...)
{
  va_list ap;
  int     r;
  va_start(ap, fmt);
  remote_packet_out_reset();
  r = remote_packet_out_append_vprintf(fmt, ap);
  va_end(ap);
  return r;
}

static int
remote_packet_out_send(void)
{
  uint8_t csum = 0;
  size_t  i = 1;

  if (rtems_debugger->output_level >= (RTEMS_DEBUGGER_BUFFER_SIZE - 3)) {
    rtems_debugger_printf("rtems-db: output overflow\n");
    return -1;
  }

  while (i < rtems_debugger->output_level) {
    csum += rtems_debugger->output[i++];
  }

  rtems_debugger->output[rtems_debugger->output_level++] = '#';
  rtems_debugger->output[rtems_debugger->output_level++] = hex_encode((csum >> 4) & 0xf);
  rtems_debugger->output[rtems_debugger->output_level++] = hex_encode(csum & 0xf);

  rtems_debugger->ack_pending = true;;

  return rtems_debugger_remote_send();
}

static int
remote_packet_dispatch(const rtems_debugger_packet* packet,
                       size_t                       packets,
                       uint8_t*                     buffer,
                       int                          size)
{
  const rtems_debugger_packet* p;
  size_t                       i;
  int                          r = -1;
  for (i = 0, p = &packet[0]; i < packets; ++i, ++p) {
    if (strncmp(p->label,
                (const char*) &buffer[0],
                strlen(p->label)) == 0) {
      if (rtems_debugger_server_flag(RTEMS_DEBUGGER_FLAG_VERBOSE_CMDS))
        rtems_debugger_printf("rtems-db: cmd: %s [%d] '%s'\n",
                              p->label, size, (const char*) buffer);
      r = p->command(buffer, size);
      break;
    }
  }
  if (r < 0) {
    remote_packet_out_buffer("", 0);
    remote_packet_out_send();
  }
  return 0;
}

static int
remote_detach(uint8_t* buffer, int size)
{
  remote_packet_out_str(r_OK);
  remote_packet_out_send();
  rtems_debugger_remote_disconnect();
  return 0;
}

static int
remote_ut_features(uint8_t* buffer, int size)
{
  return -1;
}

static int
remote_ut_osdata(uint8_t* buffer, int size)
{
  return -1;
}

static const rtems_debugger_packet uninterpreted_transfer[] = {
  { .label   = "qXfer:features",
    .command = remote_ut_features },
  { .label   = "qXfer:osdata",
    .command = remote_ut_osdata },
};

#define REMOTE_UNINTERPRETED_TRANSFERS \
  RTEMS_DEBUGGER_NUMOF(uninterpreted_transfer)

static int
remote_gq_uninterpreted_transfer(uint8_t* buffer, int size)
{
  return remote_packet_dispatch(uninterpreted_transfer,
                                REMOTE_UNINTERPRETED_TRANSFERS,
                                buffer, size);
}

static int
remote_gq_thread_info_subsequent(uint8_t* buffer, int size)
{
  rtems_debugger_threads* threads = rtems_debugger->threads;
  if (threads->next >= threads->current.level)
    remote_packet_out_str("l");
  else {
    rtems_debugger_thread* current;
    const char*            format = "p%d.%08lx";
    current = rtems_debugger_thread_current(threads);
    remote_packet_out_str("m");
    while (threads->next < threads->current.level) {
      int r;
      r = remote_packet_out_append(format,
                                   rtems_debugger->pid,
                                   current[threads->next].id);
      if (r < 0)
        break;
      format = ",p%d.%08lx";
      ++threads->next;
    }
  }
  remote_packet_out_send();
  return 0;
}

static int
remote_gq_thread_info_first(uint8_t* buffer, int size)
{
  rtems_debugger->threads->next = 0;
  return remote_gq_thread_info_subsequent(buffer, size);
}

static int
remote_gq_thread_extra_info(uint8_t* buffer, int size)
{
  const char* comma;
  remote_packet_out_reset();
  comma = strchr((const char*) buffer, ',');
  if (comma != NULL) {
    DB_UINT pid = 0;
    DB_UINT tid = 0;
    bool    extended;
    extended = thread_id_decode(comma + 1, &pid, &tid);
    if (extended || check_pid(pid)) {
      int r;
      r = rtems_debugger_thread_find_index(tid);
      if (r >= 0) {
        rtems_debugger_threads* threads = rtems_debugger->threads;
        rtems_debugger_thread*  current;
        rtems_debugger_thread*  thread;
        char                    buf[128];
        char                    str[32];
        size_t                  l;
        current = rtems_debugger_thread_current(threads);
        thread = &current[r];
        l = snprintf(buf, sizeof(buf),
                     "%4s (%08" PRIx32 "), ", thread->name, thread->id);
        remote_packet_out_append_hex((const uint8_t*) buf, l);
        l = snprintf(buf, sizeof(buf),
                     "priority(c:%3d r:%3d), ",
                     rtems_debugger_thread_current_priority(thread),
                     rtems_debugger_thread_real_priority(thread));
        remote_packet_out_append_hex((const uint8_t*) buf, l);
        l = snprintf(buf, sizeof(buf),
                     "stack(s:%6lu a:%p), ",
                     rtems_debugger_thread_stack_size(thread),
                     rtems_debugger_thread_stack_area(thread));
        remote_packet_out_append_hex((const uint8_t*) buf, l);
        rtems_debugger_thread_state_str(thread, str, sizeof(str));
        l = snprintf(buf, sizeof(buf), "state(%s)", str);
        remote_packet_out_append_hex((const uint8_t*) buf, l);
      }
    }
  }
  remote_packet_out_send();
  return 0;
}

static int
remote_gq_supported(uint8_t* buffer, int size)
{
  uint32_t    capabilities = rtems_debugger_target_capabilities();
  const char* p;
  bool        swbreak = false;
  bool        hwbreak = false;
  bool        vCont = false;
  bool        no_resumed = false;
  bool        multiprocess = false;
  remote_packet_out("qSupported:PacketSize=%d;QNonStop-",
                    RTEMS_DEBUGGER_BUFFER_SIZE);
  p = strchr((const char*) buffer, ':');
  if (p != NULL)
    ++p;
  while (p != NULL && *p != '\0') {
    bool  echo = false;
    char* sc;
    sc = strchr(p, ';');
    if (sc != NULL) {
      *sc++ = '\0';
    }
    if (strcmp(p, "swbreak+") == 0 &&
        !swbreak && (capabilities & RTEMS_DEBUGGER_TARGET_CAP_SWBREAK) != 0) {
      swbreak = true;
      echo = true;
    }
    if (strcmp(p, "hwbreak+") == 0 &&
        !hwbreak && (capabilities & RTEMS_DEBUGGER_TARGET_CAP_HWBREAK) != 0) {
      hwbreak = true;
      echo = true;
    }
    if (!vCont && strcmp(p, "vContSupported+") == 0) {
      rtems_debugger->flags |= RTEMS_DEBUGGER_FLAG_VCONT;
      vCont = true;
      echo = true;
    }
    if (!no_resumed && strcmp(p, "no-resumed+") == 0) {
      no_resumed = true;
      echo = true;
    }
    if (!multiprocess && strcmp(p, "multiprocess+") == 0) {
      rtems_debugger->flags |= RTEMS_DEBUGGER_FLAG_MULTIPROCESS;
      multiprocess = true;
      echo = true;
    }

    if (echo) {
      remote_packet_out_append_str(";");
      remote_packet_out_append_str(p);
    }
    else if (strncmp(p, "xmlRegisters", sizeof("xmlRegisters") - 1) == 0) {
      /* ignore */
    }
    else {
      remote_packet_out_append_str(";");
      remote_packet_out_append_buffer(p, strlen(p) - 1);
      remote_packet_out_append_str("-");
    }
    p = sc;
  }
  if (!swbreak && (capabilities & RTEMS_DEBUGGER_TARGET_CAP_SWBREAK) != 0) {
    remote_packet_out_append_str("swbreak+;");
  }
  if (!hwbreak && (capabilities & RTEMS_DEBUGGER_TARGET_CAP_HWBREAK) != 0) {
    remote_packet_out_append_str("hwbreak+;");
  }
  if (!vCont) {
    remote_packet_out_append_str("vContSupported+;");
  }
  if (!no_resumed) {
    remote_packet_out_append_str("no-resumed+;");
  }
  if (!multiprocess) {
    remote_packet_out_append_str("multiprocess+;");
  }
  remote_packet_out_send();
  return 0;
}

static int
remote_gq_attached(uint8_t* buffer, int size)
{
  const char* response = "1";
  const char* colon = strchr((const char*) buffer, ':');
  if (colon != NULL) {
    DB_UINT pid = hex_decode_uint((const uint8_t*) colon + 1);
    if ((pid_t) pid != rtems_debugger->pid)
      response = r_E01;
  }
  remote_packet_out_str(response);
  remote_packet_out_send();
  return 0;
}

static const rtems_debugger_packet general_query[] = {
  { .label   = "qfThreadInfo",
    .command = remote_gq_thread_info_first },
  { .label   = "qsThreadInfo",
    .command = remote_gq_thread_info_subsequent },
  { .label   = "qThreadExtraInfo",
    .command = remote_gq_thread_extra_info },
  { .label   = "qSupported",
    .command = remote_gq_supported },
  { .label   = "qAttached",
    .command = remote_gq_attached },
  { .label   = "qXfer",
    .command = remote_gq_uninterpreted_transfer },
};

#define REMOTE_GENERAL_QUERIES RTEMS_DEBUGGER_NUMOF(general_query)

static int
remote_general_query(uint8_t* buffer, int size)
{
  return remote_packet_dispatch(general_query, REMOTE_GENERAL_QUERIES,
                                buffer, size);
}

static int
remote_gs_non_stop(uint8_t* buffer, int size)
{
  const char* response = r_E01;
  char*       p = strchr((char*) buffer, ':');
  if (p != NULL) {
    ++p;
    response = r_OK;
    if (*p == '0') {
      rtems_debugger->flags &= ~RTEMS_DEBUGGER_FLAG_NON_STOP;
    }
    else if (*p == '1') {
      rtems_debugger->flags |= RTEMS_DEBUGGER_FLAG_NON_STOP;
    }
    else
      response = r_E01;
  }
  remote_packet_out_str(response);
  remote_packet_out_send();
  return 0;
}

static const rtems_debugger_packet general_set[] = {
  { .label   = "QNonStop",
    .command = remote_gs_non_stop },
};

#define REMOTE_GENERAL_SETS RTEMS_DEBUGGER_NUMOF(general_set)

static int
remote_general_set(uint8_t* buffer, int size)
{
  return remote_packet_dispatch(general_set, REMOTE_GENERAL_SETS,
                                buffer, size);
}

static int
remote_v_stopped(uint8_t* buffer, int size)
{
  rtems_debugger_threads* threads = rtems_debugger->threads;
  if (threads->next >= threads->stopped.level)
    remote_packet_out_str(r_OK);
  else {
    rtems_id* stopped;
    remote_packet_out("T%02x", rtems_debugger->signal);
    stopped = rtems_debugger_thread_stopped(threads);
    while (threads->next < threads->stopped.level) {
      int r;
      r = remote_packet_out_append("thread:p%d.%08lx;",
                                   rtems_debugger->pid,
                                   stopped[threads->next]);
      if (r < 0)
        break;
      ++threads->next;
    }
  }
  remote_packet_out_send();
  return 0;
}

static int
remote_stop_reason(uint8_t* buffer, int size)
{
  rtems_debugger->threads->next = 0;
  return remote_v_stopped(buffer, size);
}

static int
remote_v_continue(uint8_t* buffer, int size)
{
  buffer += 5;

  if (buffer[0] == '?') {
    /*
     * You need to supply 'c' and 'C' or GDB says vCont is not supported. As
     * Sammy-J says "Silly GDB".
     */
    remote_packet_out_str("vCont;c;C;s;r;");
  }
  else {
    const char* semi = (const char*) &buffer[0];
    bool        resume = false;
    bool        ok = true;
    while (ok && semi != NULL) {
      const char* colon = strchr(semi + 1, ':');
      const char  action = *(semi + 1);
      DB_UINT     pid = 0;
      DB_UINT     tid = 0;
      bool        extended;
      if (colon != NULL) {
        int r = -1;
        extended = thread_id_decode(colon + 1, &pid, &tid);
        if (extended || check_pid(pid)) {
          rtems_debugger_threads* threads = rtems_debugger->threads;
          rtems_debugger_thread*  thread = NULL;
          int                     index = 0;
          if (tid != (DB_UINT) -1) {
            rtems_debugger_thread* current;
            current = rtems_debugger_thread_current(threads);
            index = rtems_debugger_thread_find_index(tid);
            if (index >= 0)
              thread = &current[index];
          }
          switch (action) {
          case 'c':
          case 'C':
            if (tid == (DB_UINT) -1) {
              r = rtems_debugger_thread_continue_all();
            }
            else if (thread != NULL) {
              r = rtems_debugger_thread_continue(thread);
            }
            if (r == 0)
              resume = true;
            break;
          case 'S':
          case 's':
            if (thread != NULL) {
              r = rtems_debugger_thread_step(thread);
              if (r == 0)
                resume = true;
            }
            break;
          case 'r':
            /*
             * Range to step around inside: `r start,end`.
             */
            if (thread != NULL) {
              const char* comma;
              comma = strchr(semi + 2, ',');
              if (comma != NULL) {
                DB_UINT start;
                DB_UINT end;
                start = hex_decode_uint((const uint8_t*) semi + 2);
                end = hex_decode_uint((const uint8_t*) comma + 1);
                r = rtems_debugger_thread_stepping(thread, start, end);
                if (r == 0)
                  resume = true;
              }
              else {
                ok = false;
              }
            }
            break;
          default:
            rtems_debugger_printf("rtems-db: vCont: unkown action: %c\n", action);
            ok = false;
            break;
          }
          if (r < 0)
            ok = false;
        }
      }
      else {
        rtems_debugger_printf("rtems-db: vCont: no colon\n");
        ok = false;
      }
      semi = strchr(semi + 1, ';');
    }

    if (ok)
      remote_packet_out_str(r_OK);
    else
      remote_packet_out_str(r_E01);

    if (resume)
      rtems_debugger_thread_system_resume(false);
  }

  remote_packet_out_send();

  return 0;
}

static int
remote_v_kill(uint8_t* buffer, int size)
{
  rtems_debugger->flags |= RTEMS_DEBUGGER_FLAG_RESET;
  return remote_detach(buffer, size);
}

static const rtems_debugger_packet v_packets[] = {
  { .label   = "vCont",
    .command = remote_v_continue },
  { .label   = "vStopped",
    .command = remote_v_stopped },
  { .label   = "vKill",
    .command = remote_v_kill },
};

#define REMOTE_V_PACKETS RTEMS_DEBUGGER_NUMOF(v_packets)

static int
remote_v_packets(uint8_t* buffer, int size)
{
  return remote_packet_dispatch(v_packets, REMOTE_V_PACKETS,
                                buffer, size);
}

static int
remote_thread_select(uint8_t* buffer, int size)
{
  const char* response = r_OK;
  int*        index = NULL;

  if (buffer[1] == 'g')
    index = &rtems_debugger->threads->selector_gen;
  else if (buffer[1] == 'c')
    index = &rtems_debugger->threads->selector_cont;
  else
    response = r_E01;

  if (index != NULL) {
    DB_UINT pid = 0;
    DB_UINT tid = 0;
    bool    extended;
    extended = thread_id_decode((const char*) &buffer[2], &pid, &tid);
    if (extended && !check_pid(pid)) {
      response = r_E01;
    }
    else {
      if (tid == 0 || tid == (DB_UINT) -1)
        *index = (int) tid;
      else {
        int r;
        r = rtems_debugger_thread_find_index(tid);
        if (r < 0) {
          response = r_E01;
          *index = -1;
        }
        else
          *index = r;
      }
    }
  }

  remote_packet_out_str(response);
  remote_packet_out_send();
  return 0;
}

static int
remote_thread_alive(uint8_t* buffer, int size)
{
  const char* response = r_E01;
  DB_UINT     pid = 0;
  DB_UINT     tid = 0;
  bool        extended;
  extended = thread_id_decode((const char*) &buffer[1], &pid, &tid);
  if (!extended || (extended && check_pid(pid))) {
    int r;
    r = rtems_debugger_thread_find_index(tid);
    if (r >= 0)
      response = r_OK;
  }
  remote_packet_out_str(response);
  remote_packet_out_send();
  return 0;
}

static int
remote_argc_argv(uint8_t* buffer, int size)
{
  return -1;
}

static int
remote_continue_at(uint8_t* buffer, int size)
{
  if (!rtems_debugger_server_flag(RTEMS_DEBUGGER_FLAG_VCONT)) {
    char* vCont_c = "vCont;c:p1.-1";
    return remote_v_continue((uint8_t*) vCont_c, strlen(vCont_c));
  }
  return -1;
}

static int
remote_read_general_regs(uint8_t* buffer, int size)
{
  rtems_debugger_threads* threads = rtems_debugger->threads;
  bool                    ok = false;
  int                     r;
  if (threads->selector_gen >= 0 &&
      threads->selector_gen < (int) threads->current.level) {
    rtems_debugger_thread* current;
    rtems_debugger_thread* thread;
    current = rtems_debugger_thread_current(threads);
    thread = &current[threads->selector_gen];
    r = rtems_debugger_target_read_regs(thread);
    if (r >= 0) {
      remote_packet_out_reset();
      r = remote_packet_out_append_hex((const uint8_t*) &thread->registers[0],
                                       rtems_debugger_target_reg_table_size());
      if (r >= 0)
        ok = true;
    }
  }
  if (!ok)
    remote_packet_out_str(r_E01);
  remote_packet_out_send();
  return 0;
}

static int
remote_write_general_regs(uint8_t* buffer, int size)
{
  rtems_debugger_threads* threads = rtems_debugger->threads;
  size_t                  reg_table_size = rtems_debugger_target_reg_table_size();
  bool                    ok = false;
  int                     r;
  if (threads->selector_gen >= 0 &&
      threads->selector_gen < (int) threads->current.level &&
      ((size - 1) / 2) == (int) reg_table_size) {
    rtems_debugger_thread* current;
    rtems_debugger_thread* thread;
    current = rtems_debugger_thread_current(threads);
    thread = &current[threads->selector_gen];
    r = rtems_debugger_target_read_regs(thread);
    if (r >= 0) {
      r = rtems_debugger_remote_packet_in_hex((uint8_t*) &thread->registers[0],
                                              (const char*) &buffer[1],
                                              reg_table_size);
      if (r >= 0) {
        thread->flags |= RTEMS_DEBUGGER_THREAD_FLAG_REG_DIRTY;
        ok = true;
      }
    }
  }
  if (!ok)
    remote_packet_out_str(r_E01);
  remote_packet_out_send();
  return 0;
}

static int
remote_read_reg(uint8_t* buffer, int size)
{
  rtems_debugger_threads* threads = rtems_debugger->threads;
  bool                    ok = false;
  int                     r;
  if (threads->selector_gen >= 0
      && threads->selector_gen < (int) threads->current.level) {
    size_t reg = hex_decode_int(&buffer[1]);
    if (reg < rtems_debugger_target_reg_num()) {
      rtems_debugger_thread* current;
      rtems_debugger_thread* thread;
      current = rtems_debugger_thread_current(threads);
      thread = &current[threads->selector_gen];
      r = rtems_debugger_target_read_regs(thread);
      if (r >= 0) {
        const size_t   reg_size = rtems_debugger_target_reg_size(reg);
        const size_t   reg_offset = rtems_debugger_target_reg_offset(reg);
        const uint8_t* addr = &thread->registers[reg_offset];
        remote_packet_out_reset();
        r = remote_packet_out_append_hex(addr, reg_size);
        if (r >= 0)
          ok = true;
      }
    }
  }
  if (!ok)
    remote_packet_out_str(r_E01);
  remote_packet_out_send();
  return 0;
}

static int
remote_write_reg(uint8_t* buffer, int size)
{
  rtems_debugger_threads* threads = rtems_debugger->threads;
  const char*             response = r_E01;
  if (threads->selector_gen >= 0
      && threads->selector_gen < (int) threads->current.level) {
    const char* equals;
    equals = strchr((const char*) buffer, '=');
    if (equals != NULL) {
      size_t reg = hex_decode_int(&buffer[1]);
      if (reg < rtems_debugger_target_reg_num()) {
        rtems_debugger_thread* current;
        rtems_debugger_thread* thread;
        int                    r;
        current = rtems_debugger_thread_current(threads);
        thread = &current[threads->selector_gen];
        r = rtems_debugger_target_read_regs(thread);
        if (r >= 0) {
          const size_t reg_size = rtems_debugger_target_reg_size(reg);
          const size_t reg_offset = rtems_debugger_target_reg_offset(reg);
          uint8_t*     addr = &thread->registers[reg_offset];
          r = rtems_debugger_remote_packet_in_hex(addr, equals + 1, reg_size);
          if (r == 0) {
            thread->flags |= RTEMS_DEBUGGER_THREAD_FLAG_REG_DIRTY;
            response = r_OK;
          }
        }
      }
    }
  }
  remote_packet_out_str(response);
  remote_packet_out_send();
  return 0;
}

static int
remote_read_memory(uint8_t* buffer, int size)
{
  const char* comma;
  comma = strchr((const char*) buffer, ',');
  if (comma == NULL)
    remote_packet_out_str(r_E01);
  else {
    DB_UINT addr;
    DB_UINT length;
    int     r;
    addr = hex_decode_uint(&buffer[1]);
    length = hex_decode_uint((const uint8_t*) comma + 1);
    remote_packet_out_reset();
    r = rtems_debugger_target_start_memory_access();
    if (r == 0) {
      /*
       * There should be specific target access for 8, 16, 32 and 64 bit reads.
       */
      r = remote_packet_out_append_hex((const uint8_t*) addr, length);
    }
    rtems_debugger_target_end_memory_access();
    if (r < 0)
      remote_packet_out_str(r_E01);
  }
  remote_packet_out_send();
  return 0;
}

static int
remote_write_memory(uint8_t* buffer, int size)
{
  const char* response = r_E01;
  const char* comma;
  const char* colon;
  comma = strchr((const char*) buffer, ',');
  colon = strchr((const char*) buffer, ':');
  if (comma != NULL && colon != NULL) {
    DB_UINT addr;
    DB_UINT length;
    int     r;
    addr = hex_decode_uint(&buffer[1]);
    length = hex_decode_uint((const uint8_t*) comma + 1);
    r = rtems_debugger_target_start_memory_access();
    if (r == 0) {
      r = rtems_debugger_remote_packet_in_hex((uint8_t*) addr,
                                              colon + 1,
                                              length);
    }
    rtems_debugger_target_end_memory_access();
    if (r == 0)
      response = r_OK;
  }
  remote_packet_out_str(response);
  remote_packet_out_send();
  return 0;
}

static int
remote_single_step(uint8_t* buffer, int size)
{
  if (!rtems_debugger_server_flag(RTEMS_DEBUGGER_FLAG_VCONT)) {
    rtems_debugger_threads* threads = rtems_debugger->threads;
    if (threads != NULL && rtems_debugger_thread_current(threads) != NULL) {
      rtems_debugger_thread* current;
      char                   vCont_s[32];
      current = rtems_debugger_thread_current(threads);
      snprintf(vCont_s, sizeof(vCont_s), "vCont;s:p1.%08" PRIx32 ";c:p1.-1",
               current[threads->selector_cont].id);
      return remote_v_continue((uint8_t*) vCont_s, strlen(vCont_s));
    }
    remote_packet_out_str(r_E01);
    remote_packet_out_send();
    return 0;
  }
  return -1;
}

static int
remote_breakpoints(bool insert, uint8_t* buffer, int size)
{
  const char* comma1;
  int         r = -1;
  comma1 = strchr((const char*) buffer, ',');
  if (comma1 != NULL) {
    const char* comma2;
    comma2 = strchr(comma1 + 1, ',');
    if (comma2 != NULL) {
      uint32_t capabilities;
      DB_UINT  addr;
      DB_UINT  kind;
      addr = hex_decode_uint((const uint8_t*) comma1 + 1);
      kind = hex_decode_uint((const uint8_t*)comma2 + 1);
      capabilities = rtems_debugger_target_capabilities();
      switch (buffer[1]) {
      case '0':
        if ((capabilities & RTEMS_DEBUGGER_TARGET_CAP_SWBREAK) != 0) {
          r = rtems_debugger_target_swbreak_control(insert, addr, kind);
        }
        break;
      case '1': /* execute */
      case '2': /* write */
      case '3': /* read */
      case '4': /* access */
        if ((capabilities & RTEMS_DEBUGGER_TARGET_CAP_HWWATCH) != 0) {
          rtems_debugger_target_watchpoint type;
          switch (buffer[1]) {
          case '1':
            type = rtems_debugger_target_hw_execute;
            break;
          case '2':
            type = rtems_debugger_target_hw_write;
            break;
          case '3':
            type = rtems_debugger_target_hw_read;
            break;
          case '4':
          default:
            type = rtems_debugger_target_hw_read_write;
            break;
          }
          r = rtems_debugger_target_hwbreak_control(type, insert, addr, kind);
        }
        break;
      default:
        break;
      }
    }
  }
  remote_packet_out_str(r < 0 ?  r_E01 : r_OK);
  remote_packet_out_send();
  return 0;
}

static int
remote_insert_breakpoint(uint8_t* buffer, int size)
{
  return remote_breakpoints(true, buffer, size);
}

static int
remote_remove_breakpoint(uint8_t* buffer, int size)
{
  return remote_breakpoints(false, buffer, size);
}

static int
remote_break(uint8_t* buffer, int size)
{
  int r;
  r = rtems_debugger_thread_system_suspend();
  if (r < 0) {
    rtems_debugger_printf("error: rtems-db: suspend all on break\n");
  }
  return remote_stop_reason(buffer, size);
}

static const rtems_debugger_packet packets[] = {
  { .label   = "q",
    .command = remote_general_query },
  { .label   = "Q",
    .command = remote_general_set },
  { .label   = "v",
    .command = remote_v_packets },
  { .label   = "H",
    .command = remote_thread_select },
  { .label   = "T",
    .command = remote_thread_alive },
  { .label   = "?",
    .command = remote_stop_reason },
  { .label   = "A",
    .command = remote_argc_argv },
  { .label   = "c",
    .command = remote_continue_at },
  { .label   = "g",
    .command = remote_read_general_regs },
  { .label   = "G",
    .command = remote_write_general_regs },
  { .label   = "p",
    .command = remote_read_reg },
  { .label   = "P",
    .command = remote_write_reg },
  { .label   = "m",
    .command = remote_read_memory },
  { .label   = "M",
    .command = remote_write_memory },
  { .label   = "s",
    .command = remote_single_step },
  { .label   = "Z",
    .command = remote_insert_breakpoint },
  { .label   = "z",
    .command = remote_remove_breakpoint },
  { .label   = "D",
    .command = remote_detach },
  { .label   = "k",
    .command = remote_v_kill },
  { .label   = "r",
    .command = remote_v_kill },
  { .label   = "R",
    .command = remote_v_kill },
  { .label   = "^C",
    .command = remote_break },
};

#define REMOTE_PACKETS RTEMS_DEBUGGER_NUMOF(packets)

static int
remote_packets(uint8_t* buffer, size_t size)
{
  return remote_packet_dispatch(packets, REMOTE_PACKETS,
                                buffer, size);
}

static void
rtems_debugger_events(rtems_task_argument arg)
{
  int r = 0;

  if (rtems_debugger_verbose())
    rtems_debugger_printf("rtems-db: events running\n");

  /*
   * Hold the lock until the thread blocks waiting for an event.
   */
  rtems_debugger_lock();

  rtems_debugger_target_enable();

  while (rtems_debugger_server_events_running()) {
    rtems_debugger_server_events_wait();
    if (rtems_debugger_verbose())
      rtems_debugger_printf("rtems-db: event woken\n");
    if (!rtems_debugger_server_events_running())
      break;
    r = rtems_debugger_thread_system_suspend();
    if (r < 0)
      break;
    r = remote_stop_reason(NULL, 0);
    if (r < 0)
      break;
  }

  if (r < 0)
    rtems_debugger_printf("rtems-db: error in events\n");

  rtems_debugger_target_disable();

  rtems_debugger->events_running = false;
  rtems_debugger->events_finished = true;

  rtems_debugger_unlock();

  if (rtems_debugger_verbose())
    rtems_debugger_printf("rtems-db: events finishing\n");

  rtems_task_exit();
}

static int
rtems_debugger_session(void)
{
  int r;
  int rr;

  if (rtems_debugger_verbose())
    rtems_debugger_printf("rtems-db: remote running\n");

  /*
   * Hold the lock until the thread blocks on the remote input.
   */
  rtems_debugger_lock();

  r = rtems_debugger_target_create();
  if (r < 0) {
    rtems_debugger_unlock();
    return r;
  }

  r = rtems_debugger_thread_create();
  if (r < 0) {
    rtems_debugger_target_destroy();
    rtems_debugger_unlock();
    return r;
  }

  rtems_debugger->events_running = true;
  rtems_debugger->events_finished = false;

  r = rtems_debugger_task_create("DBSe",
                                 rtems_debugger->priority,
                                 RTEMS_DEBUGGER_STACKSIZE,
                                 rtems_debugger_events,
                                 0,
                                 &rtems_debugger->events_task);
  if (r < 0) {
    rtems_debugger_thread_destroy();
    rtems_debugger_target_destroy();
    rtems_debugger_unlock();
    return r;
  }

  while (rtems_debugger_server_running() &&
         rtems_debugger_connected()) {
    r = rtems_debugger_remote_packet_in();
    if (r < 0)
      break;
    if (r > 0) {
      remote_packets(&rtems_debugger->input[0], r);
    }
  }

  rtems_debugger->events_running = false;
  rtems_debugger_server_events_signal();

  rtems_debugger_unlock();

  rr = rtems_debugger_task_destroy("DBSe",
                                   rtems_debugger->events_task,
                                   &rtems_debugger->events_finished,
                                   RTEMS_DEBUGGER_TIMEOUT_STOP);
  if (rr < 0 && r == 0)
    r = rr;

  rtems_debugger_lock();

  rr = rtems_debugger_target_destroy();
  if (rr < 0 && r == 0)
    r = rr;

  rr = rtems_debugger_thread_destroy();
  if (rr < 0 && r == 0)
    r = rr;

  if (rtems_debugger_server_flag(RTEMS_DEBUGGER_FLAG_RESET)) {
    rtems_debugger_printf("rtems-db: shutdown\n");
    rtems_fatal_error_occurred(1122);
  }

  rtems_debugger->flags = 0;
  rtems_debugger->ack_pending = false;

  rtems_debugger_unlock();

  if (rtems_debugger_verbose())
    rtems_debugger_printf("rtems-db: remote finishing\n");

  return r;
}

static int
rtems_debugger_create(const char*          remote,
                      const char*          device,
                      rtems_task_priority  priority,
                      int                  timeout,
                      const rtems_printer* printer)
{
  int r;

  if (rtems_debugger != NULL) {
    rtems_printf(printer, "error: rtems-db: create: already active\n");
    errno = EEXIST;
    return -1;
  }

  rtems_debugger = malloc(sizeof(rtems_debugger_server));
  if (rtems_debugger == NULL) {
    rtems_printf(printer, "error: rtems-db: create: no memory\n");
    errno = ENOMEM;
    return -1;
  }

  memset(rtems_debugger, 0, sizeof(rtems_debugger_server));

  /*
   * These do not change with a session.
   */
  rtems_debugger->priority = priority;
  rtems_debugger->timeout = timeout;
  rtems_debugger->printer = *printer;
  rtems_debugger->pid = getpid();
  rtems_debugger->remote_debug = false;

  rtems_chain_initialize_empty(&rtems_debugger->exception_threads);

  rtems_debugger->remote = rtems_debugger_remote_find(remote);
  if (rtems_debugger->remote== NULL) {
    rtems_printf(printer, "error: rtems-db: remote not found: %s\n", remote);
    free(rtems_debugger);
    rtems_debugger = NULL;
    return -1;
  }

  r = rtems_debugger->remote->begin(rtems_debugger->remote, device);
  if (r < 0) {
    rtems_printf(printer, "error: rtems-db: remote begin: %s: %s\n",
                 rtems_debugger->remote->name, strerror(errno));
    free(rtems_debugger);
    rtems_debugger = NULL;
    return -1;
  }

  /*
   * Reset at the end of the session.
   */
  rtems_debugger->flags = 0;
  rtems_debugger->ack_pending = false;

  r = rtems_debugger_lock_create();
  if (r < 0) {
    free(rtems_debugger);
    rtems_debugger = NULL;
    return -1;
  }

  return 0;
}

static int
rtems_debugger_destroy(void)
{
  int r;
  int rr;

  rtems_debugger_lock();
  rtems_debugger->server_running = false;
  rtems_debugger_unlock();

  r = rtems_debugger_remote_disconnect();

  rr = rtems_debugger->remote->end(rtems_debugger->remote);
  if (rr < 0 && r == 0)
    r = rr;

  rr = rtems_debugger_task_destroy("DBSs",
                                   rtems_debugger->server_task,
                                   &rtems_debugger->server_finished,
                                   RTEMS_DEBUGGER_TIMEOUT_STOP);
  if (rr < 0 && r == 0)
    r = rr;

  rr = rtems_debugger_lock_destroy();
  if (rr < 0 && r == 0)
    r = rr;

  free(rtems_debugger);
  rtems_debugger = NULL;

  return r;
}

static void
rtems_debugger_main(rtems_task_argument arg)
{
  int r;

  rtems_debugger_lock();
  rtems_debugger->server_running = true;
  rtems_debugger->server_finished = false;
  rtems_debugger_unlock();

  rtems_debugger_printf("rtems-db: remote running\n");

  while (rtems_debugger_server_running()) {
    r = rtems_debugger_remote_connect();
    if (r < 0)
      break;
    rtems_debugger_session();
    rtems_debugger_remote_disconnect();
  }

  rtems_debugger_printf("rtems-db: remote finishing\n");

  rtems_debugger_lock();
  rtems_debugger->server_running = false;
  rtems_debugger->server_finished = true;
  rtems_debugger_unlock();

  rtems_task_exit();
}

int
rtems_debugger_start(const char*          remote,
                     const char*          device,
                     int                  timeout,
                     rtems_task_priority  priority,
                     const rtems_printer* printer)
{
  int r;

  r = rtems_debugger_create(remote, device, priority, timeout, printer);
  if (r < 0)
    return -1;

  rtems_debugger_lock();
  rtems_debugger->server_running = false;
  rtems_debugger->server_finished = true;
  _Condition_Initialize_named(&rtems_debugger->server_cond, "DBserver");
  rtems_debugger_unlock();

  r = rtems_debugger_task_create("DBSs",
                                 priority,
                                 RTEMS_DEBUGGER_STACKSIZE,
                                 rtems_debugger_main,
                                 0,
                                 &rtems_debugger->server_task);
  if (r < 0) {
    rtems_debugger_destroy();
    return -1;
  }

  return 0;
}

void
rtems_debugger_server_crash(void)
{
  rtems_debugger_lock();
  rtems_debugger->server_running = false;
  rtems_debugger_unlock();
  rtems_debugger->remote->end(rtems_debugger->remote);
}

int
rtems_debugger_stop(void)
{
  return rtems_debugger_destroy();
}

bool
rtems_debugger_running(void)
{
  return rtems_debugger != NULL;
}

void
rtems_debugger_set_verbose(bool on)
{
  if (rtems_debugger_running()) {
    if (on)
      rtems_debugger->flags |= RTEMS_DEBUGGER_FLAG_VERBOSE;
    else
      rtems_debugger->flags &= ~RTEMS_DEBUGGER_FLAG_VERBOSE;
  }
}

int
rtems_debugger_remote_debug(bool state)
{
  rtems_debugger_lock();
  rtems_debugger->remote_debug = state;
  rtems_debugger_printf("rtems-db: remote-debug is %s\n",
                        rtems_debugger->remote_debug ? "on" : "off");
  rtems_debugger_unlock();
  return 0;
}
