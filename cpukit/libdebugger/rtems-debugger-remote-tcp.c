/*
 * Copyright (c) 2016 Chris Johns <chrisj@rtems.org>.  All rights reserved.
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

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <rtems/rtems-debugger.h>
#include <rtems/debugger/rtems-debugger-server.h>
#include <rtems/debugger/rtems-debugger-remote.h>

#include <rtems/rtems-debugger-remote-tcp.h>

/**
 * Debugger default server port. 'RT' as ASCII.
 */
#define RTEMS_DB_PORT_DEFAULT (8284)

/**
 * TCP Remote data.
 */
typedef struct
{
  int fd;
  int port;
} rtems_debugger_remote_tcp;

static rtems_debugger_remote_tcp*
tcp_remote(rtems_debugger_remote* remote)
{
  rtems_debugger_remote_tcp* tcp = NULL;
  rtems_debugger_lock();
  if (remote != NULL && remote->data != NULL)
      tcp = (rtems_debugger_remote_tcp*) remote->data;
  rtems_debugger_unlock();
  return tcp;
}

static int
tcp_remote_begin(rtems_debugger_remote* remote, const char* device)
{
  rtems_debugger_remote_tcp* tcp;
  int                        port;
  char*                      end;

  rtems_debugger_lock();

  /*
   * Parse the port number.
   */
  port = strtoul(device, &end, 10);
  if (port == 0 || *end != '\0') {
    rtems_debugger_printf("error: rtems-db: tcp remote: invalid port: %s\n", device);
    return -1;
  }

  tcp = malloc(sizeof(rtems_debugger_remote_tcp));
  if (tcp == NULL) {
    errno = ENOMEM;
    return -1;
  }

  remote->data = tcp;

  tcp->fd = -1;
  tcp->port = port;

  rtems_debugger_unlock();

  return 0;
}

static int
tcp_remote_end(rtems_debugger_remote* remote)
{
  rtems_debugger_lock();

  if (remote != NULL && remote->data != NULL) {
    rtems_debugger_remote_tcp* tcp = (rtems_debugger_remote_tcp*) remote->data;
    if (tcp != NULL) {
      if (tcp->fd >= 0)
        close(tcp->fd);
      free(tcp);
      remote->data = NULL;
    }
  }

  rtems_debugger_unlock();

  return 0;
}

static int
tcp_remote_connect(rtems_debugger_remote* remote)
{
  int                        ld;
  struct sockaddr_in         addr;
  socklen_t                  opt;
  socklen_t                  len;
  bool                       running;
  struct timeval             timeout;
  rtems_debugger_remote_tcp* tcp = tcp_remote(remote);
  int                        r;

  if (rtems_debugger_verbose())
    rtems_debugger_printf("error: rtems-db: tcp remote: connect\n");

  ld = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (ld < 0) {
    rtems_debugger_printf("error: rtems-db: tcp remote: socket: (%d) %s\n",
                          errno, strerror(errno));
    return -1;
  }

  opt = 1;
  r = setsockopt(ld,
                 SOL_SOCKET,
                 SO_REUSEADDR,
                 (char *) &opt,
                 sizeof(opt));
  if (r < 0) {
    close(ld);
    rtems_debugger_printf("error: rtems-db: tcp remote: setsocket: reuseaddr: (%d) %s\n",
                          errno, strerror(errno));
    return -1;
  }

  addr.sin_family = PF_INET;
  addr.sin_port = htons(tcp->port);
  addr.sin_addr.s_addr = INADDR_ANY;

  r = bind(ld, (struct sockaddr *) &addr, sizeof(addr));
  if (r < 0) {
    close(ld);
    rtems_debugger_printf("error: rtems-db: tcp remote: bind: (%d) %s\n",
                          errno, strerror(errno));
    return -1;
  }

  /*
   * Backlog of 1 connection.
   */
  r = listen(ld, 1);
  if (r < 0) {
    close(ld);
    rtems_debugger_printf("error: rtems-db: tcp remote: listen: (%d) %s\n",
                          errno, strerror(errno));
    return -1;
  }

  /*
   * Use a random port if the port is 0.
   */
  if (tcp->port == 0) {
    len = sizeof(addr);
    r = getsockname(ld, (struct sockaddr *) &addr, &len);
    if (r < 0 || len < sizeof(addr)) {
      close(ld);
      rtems_debugger_printf("error: rtems-db: tcp remote: getsockname: (%d) %s\n",
                            errno, strerror(errno));
      return -1;
    }
    tcp->port = ntohs(addr.sin_port);
  }

  rtems_debugger_printf("rtems-db: tcp remote: listing on port: %d\n",
                        tcp->port);

  len = sizeof(addr);
  tcp->fd = accept(ld, (struct sockaddr *) &addr, &len);

  running = rtems_debugger_server_running();

  close(ld);

  if (tcp->fd < 0) {
    /*
     * EBADF means the socket has been closed, ignore it.
     */
    if (errno != EBADF)
      rtems_debugger_printf("error: rtems-db: accept: (%d) %s\n",
                            errno, strerror(errno));
    return -1;
  }

  if (!running) {
    close(tcp->fd);
    errno = EIO;
    return -1;
  }

  opt = 1;
  r = setsockopt(tcp->fd,
                 SOL_SOCKET, SO_KEEPALIVE,
                 (char*) &opt,
                 sizeof(opt));
  if (r < 0) {
    int errno_ = errno;
    close(tcp->fd);
    rtems_debugger_printf("error: rtems-db: tcp remote: set keepalive: (%d) %s\n",
                          errno, strerror(errno));
    errno = errno_;
    return -1;
  }

  opt = 1;
  r = setsockopt(tcp->fd,
                 IPPROTO_TCP, TCP_NODELAY,
                 (char*) &opt, sizeof(opt));
  if (r < 0) {
    int errno_ = errno;
    close(tcp->fd);
    rtems_debugger_printf("error: rtems-db: tcp remote: set no-delay: (%d) %s\n",
                          errno, strerror(errno));
    errno = errno_;
    return -1;
  }

  timeout.tv_sec = rtems_debugger->timeout;
  timeout.tv_usec = 0;

  r = setsockopt(tcp->fd,
                 SOL_SOCKET, SO_RCVTIMEO,
                 (char*) &timeout, sizeof(timeout));
  if (r < 0) {
    int errno_ = errno;
    close(tcp->fd);
    rtems_debugger_printf("error: rtems-db: tcp remote: set rcv-timeout: (%d) %s\n",
                          errno, strerror(errno));
    errno = errno_;
    return -1;
  }

  rtems_debugger_printf("rtems-db: tcp remote: connect host: %s\n",
                        inet_ntoa(addr.sin_addr));

  return 0;
}

static int
tcp_remote_disconnect(rtems_debugger_remote* remote)
{
  rtems_debugger_remote_tcp* tcp;

  rtems_debugger_lock();

  rtems_debugger_printf("rtems-db: tcp remote: disconnect host\n");

  tcp = (rtems_debugger_remote_tcp*) remote->data;
  close(tcp->fd);
  tcp->fd = -1;

  rtems_debugger_unlock();

  return 0;
}

static bool
tcp_remote_isconnected(rtems_debugger_remote* remote)
{
  rtems_debugger_remote_tcp* tcp = tcp_remote(remote);
  return tcp != NULL && tcp->fd >= 0;
}

static ssize_t
tcp_remote_receive(rtems_debugger_remote* remote,
                   void*                  buf,
                   size_t                 nbytes)
{
  rtems_debugger_remote_tcp* tcp = tcp_remote(remote);
  ssize_t                    len;
  if (tcp != NULL) {
    len = read(tcp->fd, buf, nbytes);
  }
  else {
    errno = EIO;
    len = -1;
  }
  return len;
}

static ssize_t
tcp_remote_send(rtems_debugger_remote* remote,
                const void*            buf,
                size_t                 nbytes)
{
  rtems_debugger_remote_tcp* tcp = tcp_remote(remote);
  ssize_t                    len;
  if (tcp != NULL) {
    len = write(tcp->fd, buf, nbytes);
  }
  else {
    errno = EIO;
    len = -1;
  }
  return len;
}

static rtems_debugger_remote remote_tcp =
{
  .name = "tcp",
  .begin = tcp_remote_begin,
  .end = tcp_remote_end,
  .connect = tcp_remote_connect,
  .disconnect = tcp_remote_disconnect,
  .isconnected = tcp_remote_isconnected,
  .read = tcp_remote_receive,
  .write = tcp_remote_send
};

int
rtems_debugger_register_tcp_remote(void)
{
  return rtems_debugger_remote_register(&remote_tcp);
}
