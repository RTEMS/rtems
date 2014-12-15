/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <limits.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include <tmacros.h>

#include "test-http-client.h"

#define HTTPC_WS_CONN_REQ  "GET / HTTP/1.1\r\n" \
                           "Host: localhost\r\n" \
                           "Upgrade: websocket\r\n" \
                           "Connection: Upgrade\r\n" \
                           "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n" \
                           "Sec-WebSocket-Version: 13\r\n" \
                           "\r\n"
#define HTTPC_WS_CONN_RESP "HTTP/1.1 101 Switching Protocols\r\n" \
                           "Upgrade: websocket\r\n" \
                           "Connection: Upgrade\r\n" \
                           "Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=\r\n" \
                           "\r\n"

static ssize_t httpc_read_full(
  const httpc_context *ctx,
  void *response,
  size_t responsesize
);

void httpc_init_context(
  httpc_context *ctx
)
{
  ctx->socket = -1;
  ctx->fd = NULL;
}

bool httpc_open_connection(
  httpc_context *ctx,
  const char *targethost,
  int targetport
)
{
  struct sockaddr_in addr;

  struct hostent *server;

  ctx->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(ctx->socket < 0) { return false; }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(targetport);

  server = gethostbyname(targethost);
  if(server == NULL) { return false; }
  memcpy(&addr.sin_addr.s_addr, server->h_addr, (size_t) server->h_length);

  if(connect(ctx->socket, (struct sockaddr *)&addr, sizeof(addr)) != 0)
  {
    return false;
  }

  ctx->fd = fdopen(ctx->socket,"rw");
  if(ctx->fd == NULL) { return false; }

  return true;
}

bool httpc_close_connection(
  httpc_context *ctx
)
{
  if(close(ctx->socket) != 0)
  {
    return false;
  }

  return true;
}

bool httpc_send_request(
  const httpc_context *ctx,
  const char *request,
  char *response,
  int responsesize
)
{
  rtems_test_assert(ctx != NULL);
  rtems_test_assert(ctx->socket >= 0);
  rtems_test_assert(request != NULL);
  rtems_test_assert(response != NULL);
  rtems_test_assert(responsesize > 1);

  static const char * const lineend = " HTTP/1.1\r\n\r\n";

  write(ctx->socket, request, strlen(request));
  write(ctx->socket, lineend, strlen(lineend));

  ssize_t size;
  if((size = httpc_read_full(ctx, response, responsesize - 1)) == -1)
  {
    return false;
  }
  *(response + size) = '\0';

  return true;
}

bool httpc_ws_open_connection(
  const httpc_context *ctx
)
{
  rtems_test_assert(ctx != NULL);
  rtems_test_assert(ctx->socket >= 0);

  write(ctx->socket, HTTPC_WS_CONN_REQ, strlen(HTTPC_WS_CONN_REQ));

  char response[strlen(HTTPC_WS_CONN_RESP)];
  if(httpc_read_full(ctx, response, sizeof(response)) != sizeof(response))
  {
    return false;
  }
  if(strncmp(response, HTTPC_WS_CONN_RESP, sizeof(response)) != 0)
  {
    return false;
  }

  return true;
}

bool httpc_ws_send_request(
  const httpc_context *ctx,
  const char *request,
  char *response,
  int responsesize
)
{
  rtems_test_assert(ctx != NULL);
  rtems_test_assert(ctx->socket >= 0);
  rtems_test_assert(request != NULL);
  rtems_test_assert(response != NULL);
  rtems_test_assert(responsesize > 0);

  static const uint16_t ws_header_fin  = 1U << 15;
  static const uint16_t ws_header_text = 1U << 8;
  static const uint16_t ws_header_size = 0x7FU;

  /*
   * We don't support sending WebSocket messages which require multiple
   * chunks
   */
  if(strlen(request) > ws_header_size) { return false; }

  uint16_t header = htons(ws_header_fin | ws_header_text | strlen(request));

  write(ctx->socket, &header, sizeof(header));
  write(ctx->socket, request, strlen(request));

  if (httpc_read_full(ctx, &header, sizeof(header)) != sizeof(header))
  {
    return false;
  }
  header = ntohs(header);
  if (!(header & ws_header_fin)) { return false; }
  if (!(header & ws_header_text)) { return false; }
  if (responsesize < (header & ws_header_size) + 1) { return false; }

  responsesize = header & ws_header_size;
  if (httpc_read_full(ctx, response, responsesize) != responsesize)
  {
    return false;
  }
  *(response + responsesize) = '\0';

  return true;
}


static ssize_t httpc_read_full(
  const httpc_context *ctx,
  void *response,
  size_t responsesize
)
{
  rtems_test_assert(ctx != NULL);
  rtems_test_assert(ctx->socket >= 0);
  rtems_test_assert(response != NULL);
  rtems_test_assert(responsesize > 0);

  if (responsesize > SSIZE_MAX) { return -1; }

  unsigned char *pos = response;

  while(pos < (unsigned char *)response + responsesize)
  {
    ssize_t size =
      read(ctx->socket, pos, (unsigned char *)response + responsesize - pos);
    if (size == -1) { return -1; }
    if (size == 0) { break; }
    pos += size;
  }

  return (pos - (unsigned char *)response);
}
