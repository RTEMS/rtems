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
 * http://www.rtems.com/license/LICENSE.
 */

#include "test-http-client.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>

void httpc_init_context(
  httpc_context *ctx
)
{
  ctx->socket = -1;
  ctx->fd = NULL;
}

bool httpc_open_connection(
  httpc_context *ctx,
  char *targethost,
  int targetport
)
{
  struct sockaddr_in addr;

  struct hostent *server;
  struct servent *service;

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
  httpc_context *ctx,
  char *request,
  char *response,
  int responsesize
)
{
  int size = strlen(request);
  char lineend[] = " HTTP/1.1\r\n\r\n";

  write(ctx->socket, request, size);
  write(ctx->socket, lineend, sizeof(lineend));

  size = read(ctx->socket, response, responsesize-1);
  response[size] = '\0';

  return true;
}

