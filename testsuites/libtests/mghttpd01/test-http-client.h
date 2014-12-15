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

#ifndef TEST_WEB_CLIENT_H
#define TEST_WEB_CLIENT_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct
{
  int socket;
  FILE *fd;
}
httpc_context;

void httpc_init_context(
  httpc_context *ctx
);

bool httpc_open_connection(
  httpc_context *ctx,
  const char *targethost,
  int targetport
);

bool httpc_close_connection(
  httpc_context *ctx
);

bool httpc_send_request(
  const httpc_context *ctx,
  const char *request,
  char *response,
  int responsesize
);

bool httpc_ws_open_connection(
  const httpc_context *ctx
);

bool httpc_ws_send_request(
  const httpc_context *ctx,
  const char *request,
  char *response,
  int responsesize
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TEST_WEB_CLIENT_H */
