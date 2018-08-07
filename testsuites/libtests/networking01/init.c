/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>

#include <rtems.h>
#include <tmacros.h>

const char rtems_test_name[] = "NETWORKING 1";

/* forward declarations to avoid warnings */
static rtems_task Init(rtems_task_argument argument);

static void fill_sa(struct sockaddr *sa, sa_family_t family)
{
  memset(sa, 0, sizeof(*sa));
  sa->sa_len = sizeof(*sa);
  sa->sa_family = family;
}

static void fill_sa_in(struct sockaddr_in *sa_in,
  in_addr_t addr, in_port_t port)
{
  fill_sa((struct sockaddr *)sa_in, AF_INET);
  sa_in->sin_port = htons(port);
  sa_in->sin_addr.s_addr = htonl(addr);
}

static void test_getnameinfo(
  const struct sockaddr *sa,
  int flags,
  bool ask_node,
  bool ask_service,
  int expected_returnvalue,
  const char *expected_node,
  const char *expected_service
)
{
  char node[] = "255.255.255.255";
  char service[] = "65535";
  socklen_t salen = sa->sa_len;
  int rv;

  char *node_p = node;
  char *service_p = service;
  size_t node_l = sizeof(node);
  size_t service_l = sizeof(service);

  if(ask_node == false) {
    node_p = NULL;
    node_l = 0;
  }

  if(ask_service == false) {
    service_p = NULL;
    service_l = 0;
  }

  rv = getnameinfo(sa, salen, node_p, node_l, service_p, service_l, flags);
  rtems_test_assert(rv == expected_returnvalue);

  if(expected_node != NULL) {
    rtems_test_assert(strcmp(expected_node, node) == 0);
  }

  if(expected_service != NULL) {
    rtems_test_assert(strcmp(expected_service, service) == 0);
  }
}

static void test(void)
{
  struct sockaddr sa;
  struct sockaddr_in sa_in;
  struct sockaddr *sa_in_p = (struct sockaddr *) &sa_in;

  const in_addr_t ip1_num = 0x7F000001u;
  const char ip1_string[] = "127.0.0.1";

  const in_addr_t ip2_num = 0xC0A86464u;
  const char ip2_string[] = "192.168.100.100";

  const in_port_t port1_num = 7u;
  const char port1_string[] = "7";

  const in_port_t port2_num = 65534u;
  const char port2_string[] = "65534";


  puts("Try AF_INET6");
  fill_sa(&sa, AF_INET6);
  test_getnameinfo(&sa, 0, true, true, EAI_FAMILY, NULL, NULL);

  puts("force node name");
  fill_sa_in(&sa_in, ip1_num, port1_num);
  test_getnameinfo(sa_in_p, NI_NAMEREQD, true, true, EAI_NONAME, NULL, NULL);

  puts("force service name");
  fill_sa_in(&sa_in, ip1_num, port1_num);
  test_getnameinfo(sa_in_p, NI_NAMEREQD, true, true, EAI_NONAME, NULL, NULL);

  puts("get node only");
  fill_sa_in(&sa_in, ip1_num, port1_num);
  test_getnameinfo(sa_in_p, 0, true, false, 0, ip1_string, NULL);

  puts("get service only");
  fill_sa_in(&sa_in, ip1_num, port1_num);
  test_getnameinfo(sa_in_p, 0, false, true, 0, NULL, port1_string);

  puts("get node and service");
  fill_sa_in(&sa_in, ip1_num, port1_num);
  test_getnameinfo(sa_in_p, 0, true, true, 0, ip1_string, port1_string);

  puts("get node and service with maximum number of characters for IP");
  fill_sa_in(&sa_in, ip2_num, port2_num);
  test_getnameinfo(sa_in_p, 0, true, true, 0, ip2_string, port2_string);
}

static rtems_task Init(rtems_task_argument argument)
{
  TEST_BEGIN();
  test();
  TEST_END();

  rtems_test_exit(0);
}

#define CONFIGURE_INIT

#define CONFIGURE_MICROSECONDS_PER_TICK 10000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS (1)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#include <rtems/confdefs.h>
