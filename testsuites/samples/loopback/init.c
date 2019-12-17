/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>

const char rtems_test_name[] = "LOOPBACK";

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXECUTIVE_RAM_SIZE        (512*1024)
#define CONFIGURE_MAXIMUM_SEMAPHORES        20
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES    20
#define CONFIGURE_MAXIMUM_TASKS            20

#define CONFIGURE_MICROSECONDS_PER_TICK    10000
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 50

#define CONFIGURE_INIT_TASK_STACK_SIZE    (10*1024)
#define CONFIGURE_INIT_TASK_PRIORITY    50
#define CONFIGURE_INIT_TASK_INITIAL_MODES (RTEMS_PREEMPT | \
                                           RTEMS_NO_TIMESLICE | \
                                           RTEMS_NO_ASR | \
                                           RTEMS_INTERRUPT_LEVEL(0))
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INIT
rtems_task Init(rtems_task_argument argument);

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#include <rtems/confdefs.h>

#include <rtems/rtems_bsdnet.h>
#include <rtems/error.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*
 * Network configuration
 */

struct rtems_bsdnet_config rtems_bsdnet_config = {
    NULL,                   /* Network interface */
    NULL,                   /* Use fixed network configuration */
    0,                      /* Default network task priority */
    0,                      /* Default mbuf capacity */
    0,                      /* Default mbuf cluster capacity */
    "testSystem",           /* Host name */
    "nowhere.com",          /* Domain name */
    "127.0.0.1",            /* Gateway */
    "127.0.0.1",            /* Log host */
    {"127.0.0.1" },         /* Name server(s) */
    {"127.0.0.1" },         /* NTP server(s) */
    0,
    0,
    0,
    0,
    0
};

/*
 * Spawn a task
 */
static void spawnTask(rtems_task_entry entryPoint, rtems_task_priority priority, rtems_task_argument arg)
{
    rtems_status_code sc;
    rtems_id tid;

    sc = rtems_task_create(rtems_build_name('t','a','s','k'),
            priority,
            RTEMS_MINIMUM_STACK_SIZE+(8*1024),
            RTEMS_PREEMPT|RTEMS_TIMESLICE|RTEMS_NO_ASR|RTEMS_INTERRUPT_LEVEL(0),
            RTEMS_FLOATING_POINT|RTEMS_LOCAL,
            &tid);
    if (sc != RTEMS_SUCCESSFUL)
        rtems_panic("Can't create task: %s", rtems_status_text(sc));
    sc = rtems_task_start(tid, entryPoint, arg);
    if (sc != RTEMS_SUCCESSFUL)
        rtems_panic("Can't start task: %s", rtems_status_text(sc));
}

/*
 * Server subtask
 */
static rtems_task workerTask(rtems_task_argument arg)
{
    int s = arg;
    char msg[80];
    char reply[120];
    int i;

    for (;;) {
        if ((i = read(s, msg, sizeof msg)) < 0) {
            printf("Server couldn't read message from client: %s\n", strerror(errno));
            break;
        }
        if (i == 0)
            break;
        rtems_task_wake_after(20); /* Simulate some processing delay */
        i = sprintf(reply, "Server received %d (%s)", i, msg);
        if ((i = write(s, reply, i+1)) < 0) {
            printf("Server couldn't write message to client: %s\n", strerror(errno));
            break;
        }
    }
    if (close(s) < 0)
        printf("Can't close worker task socket: %s\n", strerror(errno));
    printf("Worker task terminating.\n");
    rtems_task_exit();
}

/*
 * Server Task
 */
static rtems_task serverTask(rtems_task_argument arg)
{
    int s, s1;
    socklen_t addrlen;
	struct sockaddr_in myAddr, farAddr;
    rtems_task_priority myPriority;

	printf("Create socket.\n");
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0)
		rtems_panic("Can't create socket: %s\n", strerror(errno));
    memset(&myAddr, 0, sizeof myAddr);
	myAddr.sin_family = AF_INET;
	myAddr.sin_port = htons(1234);
	myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	printf("Bind socket.\n");
	if (bind(s, (struct sockaddr *)&myAddr, sizeof myAddr) < 0)
		rtems_panic("Can't bind socket: %s\n", strerror(errno));
	if (listen(s, 5) < 0)
		printf("Can't listen on socket: %s\n", strerror(errno));
    rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &myPriority);
    for(;;) {
        addrlen = sizeof farAddr;
        s1 = accept(s, (struct sockaddr *)&farAddr, &addrlen);
        if (s1 < 0)
            if (errno == ENXIO)
                rtems_task_exit();
            else
                rtems_panic("Can't accept connection: %s", strerror(errno));
        else
            printf("ACCEPTED:%" PRIu32 "\n", ntohl(farAddr.sin_addr.s_addr));
        spawnTask(workerTask, myPriority, s1);
    }
}

/*
 * The real part of the client
 */
static rtems_task clientWorker(int arg)
{
    int s;
	struct sockaddr_in myAddr, farAddr;
    char cbuf[50];
    int i;

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		printf("Can't create client socket: %s\n", strerror(errno));
        return;
    }
    memset(&myAddr, 0, sizeof myAddr);
	myAddr.sin_family = AF_INET;
	myAddr.sin_port = htons(0);
	myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(s, (struct sockaddr *)&myAddr, sizeof myAddr) < 0) {
		printf("Can't bind socket: %s\n", strerror(errno));
        goto close;
    }
    memset(&farAddr, 0, sizeof farAddr);
	farAddr.sin_family = AF_INET;
	farAddr.sin_port = htons(1234);
	farAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	printf("Connect to server.\n");
	if (connect(s, (struct sockaddr *)&farAddr, sizeof farAddr) < 0) {
		printf("Can't connect to server: %s\n", strerror(errno));
        goto close;
    }
    rtems_task_wake_after(20); /* Simulate client delay */
    i = sprintf(cbuf, "Hi there, server (%d).", arg);
    i++;    /* Send the '\0', too */
    printf("Write %d-byte message to server.\n", i);
    if (write(s, cbuf, i) < 0) {
		printf("Can't write to server: %s\n", strerror(errno));
        goto close;
    }
    if ((i = read(s, cbuf, sizeof cbuf)) < 0) {
		printf("Can't read from server: %s\n", strerror(errno));
        goto close;
    }
    printf("Read %d from server: %.*s\n", i, i, cbuf);
    rtems_task_wake_after(20); /* Simulate client delay */
  close:
    printf("Client closing connection.\n");
    if (close(s) < 0)
        printf("Can't close client task socket: %s\n", strerror(errno));
}

/*
 * Client Task
 */
static rtems_task clientTask(rtems_task_argument arg)
{
    clientWorker(arg);
    printf("Client task terminating.\n");
    rtems_task_exit();
}

/*
 * RTEMS Startup Task
 */
rtems_task
Init (rtems_task_argument ignored)
{
    rtems_print_printer_fprintf_putc(&rtems_test_printer);

    TEST_BEGIN();

    printf("\"Network\" initializing!\n");
    rtems_bsdnet_initialize_network();
    printf("\"Network\" initialized!\n");

    printf("Try running client with no server present.\n");
    printf("Should fail with `connection refused'.\n");
    clientWorker(0);

    printf("\nStart server.\n");
    spawnTask(serverTask, 150, 0);

    printf("\nTry running client with server present.\n");
    spawnTask(clientTask, 120, 1);
    rtems_task_wake_after(500);

    printf("\nTry running two clients.\n");
    spawnTask(clientTask, 120, 2);
    spawnTask(clientTask, 120, 3);
    rtems_task_wake_after(500);

    printf("\nTry running three clients.\n");
    spawnTask(clientTask, 120, 4);
    spawnTask(clientTask, 120, 5);
    spawnTask(clientTask, 120, 6);

    rtems_task_wake_after(500);
    TEST_END();
    exit( 0 );
}
