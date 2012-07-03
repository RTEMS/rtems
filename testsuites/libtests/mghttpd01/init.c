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

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems.h>

#include <tmacros.h>

#include <rtems/rtems_bsdnet.h>

#include <stdio.h>
#include <mghttpd/mongoose.h>

#include <rtems/imfs.h>
#include <rtems/error.h>
#include "init_fs.h"

#include "test-http-client.h"

#define TARFILE_START init_fs_tar
#define TARFILE_SIZE  init_fs_tar_size

#define CBACKTEST_URI   "/callbacktest.txt"
#define CBACKTEST_TXT   "HTTP/1.1 200 OK\r\n" \
                        "Content-Type: text/plain\r\n" \
                        "Content-Length: 47\r\n" \
                        "\r\n" \
                        "This is a message from the callback function.\r\n"

#define INDEX_HTML      "HTTP/1.1 200 OK\r\n" \
                        "Date: xxxxxxxxxxxxxxxxxxxxxxxxxxxxx\r\n" \
                        "Last-Modified: xxxxxxxxxxxxxxxxxxxxxxxxxxxxx\r\n" \
                        "Etag: \"21dae501.a2\"\r\n" \
                        "Content-Type: text/html\r\n" \
                        "Content-Length: 162\r\n" \
                        "Connection: close\r\n" \
                        "Accept-Ranges: bytes\r\n" \
                        "\r\n" \
                        "<html>\r\n" \
                        "<head>\r\n" \
                        "<title>Second Instance</title>\r\n" \
                        "</head>\r\n" \
                        "\r\n" \
                        "<body>\r\n" \
                        "<h1>Second Instance</h1>\r\n" \
                        "A test page for the Mongoose web server on RTEMS.\r\n" \
                        "</body>\r\n" \
                        "</html>\r\n"

#define DATE_TAG        "Date: "
#define LASTMOD_TAG     "Last-Modified: "
#define TIMESTAMP_SIZE  (sizeof("Fri, 01 Jan 1988 00:00:26 GMT") - 1)

#define BUFFERSIZE      1024

static void test_tarfs_load(void)
{
  rtems_status_code sc;

  printf("Loading tarfs image ... ");
  sc = rtems_tarfs_load("/",(void *)TARFILE_START, TARFILE_SIZE);
  if (sc != RTEMS_SUCCESSFUL) {
    printf ("error: untar failed: %s\n", rtems_status_text (sc));
    rtems_test_exit(1);
  }
  printf ("successful\n");
}

typedef struct {
  char *string;
  int size;
} printctx;

static void *callback(enum mg_event event,
    struct mg_connection *conn,
    const struct mg_request_info *request_info)
{
  if (event == MG_NEW_REQUEST) {
    int cbacktest = strncmp(request_info->uri, CBACKTEST_URI, sizeof(CBACKTEST_URI));
    if (cbacktest == 0)
    {
      mg_write(conn, CBACKTEST_TXT, sizeof(CBACKTEST_TXT));

      /* Mark as processed */
      return "";
    }
  }
  return NULL;
}

static void test_mg_index_html(void)
{
  httpc_context httpc_ctx;
  char *buffer = malloc(BUFFERSIZE);
  char *workpos = buffer;
  bool brv = false;
  int rv = 0;

  rtems_test_assert(buffer != NULL);

  puts("=== Get the index.html from second Mongoose instance:");

  httpc_init_context(&httpc_ctx);
  brv = httpc_open_connection(&httpc_ctx, "127.0.0.1", 8080);
  rtems_test_assert(brv);
  brv = httpc_send_request(&httpc_ctx, "GET /index.html", buffer, BUFFERSIZE);
  rtems_test_assert(brv);
  brv = httpc_close_connection(&httpc_ctx);
  rtems_test_assert(brv);
  puts(buffer);

  /* remove timestamps from html-header */
  workpos = strstr(buffer, DATE_TAG);
  rtems_test_assert(workpos != NULL);
  workpos += sizeof(DATE_TAG) - 1;
  memset(workpos, 'x', TIMESTAMP_SIZE);

  workpos = strstr(buffer, LASTMOD_TAG);
  rtems_test_assert(workpos != NULL);
  workpos += sizeof(LASTMOD_TAG) - 1;
  memset(workpos, 'x', TIMESTAMP_SIZE);

  rv = strcmp(buffer, INDEX_HTML);
  rtems_test_assert(rv == 0);

  puts("=== OK");

  free(buffer);
}

static void test_mg_callback(void)
{
  httpc_context httpc_ctx;
  char *buffer = malloc(BUFFERSIZE);
  bool brv = false;
  int rv = 0;

  rtems_test_assert(buffer != NULL);

  puts("=== Get a page generated from a callback function from" \
      " first Mongoose instance:");

  httpc_init_context(&httpc_ctx);
  brv = httpc_open_connection(&httpc_ctx, "127.0.0.1", 80);
  rtems_test_assert(brv);
  brv = httpc_send_request(&httpc_ctx, "GET " CBACKTEST_URI, buffer, BUFFERSIZE);
  rtems_test_assert(brv);
  brv = httpc_close_connection(&httpc_ctx);
  rtems_test_assert(brv);
  puts(buffer);
  rv = strcmp(buffer, CBACKTEST_TXT);
  rtems_test_assert(rv == 0);

  puts("=== OK");

  free(buffer);
}

static void test_mongoose(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  const char *options[] = {
    "listening_ports", "80",
    "document_root", "/www/",
    "num_threads", "1",
    "max_request_size", "2048",
    "thread_stack_size", "16384",
    "thread_priority", "250",
    "thread_policy", "o",
    NULL};
  const char *options2[] = {
    "listening_ports", "8080",
    "document_root", "/www2/",
    "num_threads", "1",
    "thread_stack_size", "16384",
    "max_request_size", "2048",
    NULL};

  struct mg_context *mg1 = mg_start(&callback, NULL, options);
  struct mg_context *mg2 = mg_start(NULL, NULL, options2);

  test_mg_index_html();
  test_mg_callback();

  mg_stop(mg1);
  mg_stop(mg2);
}

static void Init(rtems_task_argument arg)
{
  int rv = 0;

  puts("\n\n*** TEST MGHTTPD 01 ***");

  rv = rtems_bsdnet_initialize_network();
  rtems_test_assert(rv == 0);

  test_tarfs_load();

  test_mongoose();

  puts("*** END OF TEST MGHTTPD 01 ***");

  rtems_test_exit(0); 
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_FILESYSTEM_IMFS

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 16

#define CONFIGURE_UNLIMITED_OBJECTS

#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_STACK_SIZE (16 * 1024)

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
