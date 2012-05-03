/**
 * @file
 *
 * @ingroup QorIQInterCom
 *
 * @brief Inter-Processor Communication implementation.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
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

#include <assert.h>
#include <string.h>

#include <rtems.h>

#include <libcpu/powerpc-utility.h>

#include <bspopts.h>
#include <bsp/irq.h>
#include <bsp/qoriq.h>
#include <bsp/intercom.h>

#define INTERCOM_EVENT_IPI RTEMS_EVENT_13
#define INTERCOM_EVENT_WAKE_UP RTEMS_EVENT_14

#define PACKET_SIZE_COUNT 4

#define ONE_CORE(core) (1U << (core))
#define ALL_CORES ((1U << INTERCOM_CORE_COUNT) - 1U)
#define OTHER_CORES(core) (ALL_CORES & ~ONE_CORE(core))

#define IPI_INDEX 0

typedef struct consumer {
	struct consumer *next;
	rtems_id task;
} consumer;

typedef struct {
	consumer *head;
	uint32_t cache_line_alignment [7];
} consumer_list;

typedef struct {
	uint32_t lock;
	intercom_packet *head;
	size_t size;
	uint32_t cores_to_notify;
	uint32_t cache_line_alignment [4];
	consumer_list waiting_consumers [INTERCOM_CORE_COUNT];
} free_list;

typedef struct {
	uint32_t lock;
	intercom_packet *head;
	intercom_packet *tail;
	uint32_t cache_line_alignment [5];
} core_fifo;

typedef struct {
	free_list free_lists [PACKET_SIZE_COUNT];
	core_fifo core_fifos [INTERCOM_CORE_COUNT];
	intercom_service services [INTERCOM_CORE_COUNT][INTERCOM_SERVICE_COUNT];
	void *service_args [INTERCOM_CORE_COUNT][INTERCOM_SERVICE_COUNT];
	uint32_t ready_lock;
	uint32_t ready;
	uint32_t cache_line_alignment [6];
} control;

static control *const intercom = (control *) QORIQ_INTERCOM_AREA_BEGIN;

static const size_t packet_sizes [PACKET_SIZE_COUNT] = {
	64,
	512,
	2048,
	4096
};

static void send_event(rtems_id task, rtems_event_set event)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	sc = rtems_event_send(task, event);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void wait_for_event(rtems_event_set in)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	rtems_event_set out;

	sc = rtems_event_receive(
		in,
		RTEMS_EVENT_ALL | RTEMS_WAIT,
		RTEMS_NO_TIMEOUT,
		&out
	);
	assert(sc == RTEMS_SUCCESSFUL);
}

static void intercom_handler(void *arg)
{
	rtems_id task = (rtems_id) arg;
	send_event(task, INTERCOM_EVENT_IPI);
}

static void notify_core_by_index(int core)
{
	uint32_t self = ppc_processor_id();
	qoriq.pic.per_cpu [self].ipidr [IPI_INDEX].reg = ONE_CORE(core);
}

static void notify_cores(uint32_t cores)
{
	uint32_t self = ppc_processor_id();
	qoriq.pic.per_cpu [self].ipidr [IPI_INDEX].reg = cores;
}

void qoriq_intercom_free_packet(intercom_packet *packet)
{
	free_list *list = &intercom->free_lists [packet->size_index];

	uint32_t msr = qoriq_spin_lock(&list->lock);
	intercom_packet *first = list->head;
	list->head = packet;
	packet->glue.next = first;
	uint32_t cores = list->cores_to_notify;
	if (cores != 0) {
		list->cores_to_notify = 0;
		notify_cores(cores);
	}
	qoriq_spin_unlock(&list->lock, msr);
}

static void default_service(intercom_packet *packet, void *arg)
{
	qoriq_intercom_free_packet(packet);
}

static void process_free_lists(free_list *free_lists, uint32_t self)
{
	int i = 0;

	for (i = 0; i < PACKET_SIZE_COUNT; ++i) {
		free_list *list = &free_lists [i];

		uint32_t msr = qoriq_spin_lock(&list->lock);
		consumer *waiting_consumer = list->waiting_consumers [self].head;
		list->waiting_consumers [self].head = NULL;
		qoriq_spin_unlock(&list->lock, msr);

		while (waiting_consumer != NULL) {
			send_event(waiting_consumer->task, INTERCOM_EVENT_WAKE_UP);
			waiting_consumer = waiting_consumer->next;
		}
	}
}

static void process_core_fifo(core_fifo *fifo, intercom_service *services, void **service_args)
{
	uint32_t msr = qoriq_spin_lock(&fifo->lock);
	intercom_packet *packet = fifo->head;
	fifo->head = NULL;
	qoriq_spin_unlock(&fifo->lock, msr);

	while (packet != NULL) {
		intercom_packet *current = packet;
		intercom_type type_index = current->type_index;
		packet = current->glue.next;
		(*services [type_index])(current, service_args [type_index]);
	}
}

static void intercom_task(rtems_task_argument arg)
{
	uint32_t self = ppc_processor_id();
	free_list *free_lists = &intercom->free_lists [0];
	intercom_service *services = &intercom->services [self][0];
	void **service_args = &intercom->service_args [self][0];
	core_fifo *fifo = &intercom->core_fifos [self];

	while (true) {
		process_free_lists(free_lists, self);
		process_core_fifo(fifo, services, service_args);
		wait_for_event(INTERCOM_EVENT_IPI);
	}
}

static intercom_packet *free_list_and_packet_init(
	free_list *list,
	size_t count,
	intercom_packet *current,
	intercom_size size_index,
	size_t size
)
{
	intercom_packet *last = current;
	size_t inc = 1 + size / sizeof(*current);
	size_t i = 0;

	assert(count > 0);
	assert(size % sizeof(*current) == 0);

	list->size = size;
	list->head = current;
	for (i = 0; i < count; ++i) {
		intercom_packet *next = current + inc;
		current->glue.next = next;
		current->size_index = size_index;
		last = current;
		current = next;
	}
	last->glue.next = NULL;

	return current;
}

static void basic_init(void)
{
	char *begin = (char *) QORIQ_INTERCOM_AREA_BEGIN;
	size_t size = QORIQ_INTERCOM_AREA_SIZE;
	int i = 0;

	memset(begin, 0, size);

	assert(size % packet_sizes [PACKET_SIZE_COUNT - 1] == 0);

	/* Calculate data area sizes */
	size_t data_sizes [PACKET_SIZE_COUNT];
	data_sizes [PACKET_SIZE_COUNT - 1] = size / 2;
	for (i = PACKET_SIZE_COUNT - 2; i > 0; --i) {
		data_sizes [i] = data_sizes [i + 1] / 2;
	}
	data_sizes [i] = data_sizes [i + 1];

	/* Calculate packet counts */
	size_t packet_counts [PACKET_SIZE_COUNT];
	size_t count = 0;
	for (i = 1; i < PACKET_SIZE_COUNT; ++i) {
		packet_counts [i] = data_sizes [i] / packet_sizes [i];
		count += packet_counts [i];
	}
	packet_counts [0] = (data_sizes [0] - sizeof(control) - count * sizeof(intercom_packet))
		/ (sizeof(intercom_packet) + packet_sizes [0]);

	/* Initialize free lists and packets */
	intercom_packet *packet = (intercom_packet *) (begin + sizeof(control));
	for (i = 0; i < PACKET_SIZE_COUNT; ++i) {
		packet = free_list_and_packet_init(
			&intercom->free_lists [i],
			packet_counts [i],
			packet,
			i,
			packet_sizes [i]
		);
	}

	rtems_cache_flush_multiple_data_lines(begin, size);
	ppc_synchronize_data();
}

static void services_init(uint32_t self)
{
	int i = 0;

	for (i = 0; i < INTERCOM_SERVICE_COUNT; ++i) {
		if (intercom->services [self][i] == NULL) {
			intercom->services [self][i] = default_service;
		}
	}
}

void qoriq_intercom_init(void)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	rtems_id task = RTEMS_ID_NONE;
	uint32_t self = ppc_processor_id();

	sc = rtems_task_create(
		rtems_build_name('I', 'C', 'O', 'M'),
		10,
		0,
		RTEMS_DEFAULT_MODES,
		RTEMS_DEFAULT_ATTRIBUTES,
		&task
	);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = qoriq_pic_set_priority(
		QORIQ_IRQ_IPI_0,
		QORIQ_PIC_PRIORITY_LOWEST,
		NULL
	);
	assert(sc == RTEMS_SUCCESSFUL);

	sc = rtems_interrupt_handler_install(
		QORIQ_IRQ_IPI_0,
		"INTERCOM",
		RTEMS_INTERRUPT_UNIQUE,
		intercom_handler,
		(void *) task
	);
	assert(sc == RTEMS_SUCCESSFUL);

	if (self == 0) {
		basic_init();
	}

	services_init(self);

	sc = rtems_task_start(task, intercom_task, 0);
	assert(sc == RTEMS_SUCCESSFUL);
}

void qoriq_intercom_start(void)
{
	uint32_t self = ppc_processor_id();
	uint32_t ready = 0;

	while (ready != ALL_CORES) {
		uint32_t msr = qoriq_spin_lock(&intercom->ready_lock);
		ready = intercom->ready;
		intercom->ready = ready | ONE_CORE(self);
		qoriq_spin_unlock(&intercom->ready_lock, msr);
	}
}

static intercom_packet *allocate(intercom_type type, free_list *list)
{
	uint32_t self = ppc_processor_id();
	intercom_packet *packet = NULL;
	consumer poor = {
		.task = rtems_task_self()
	};

	while (packet == NULL) {
		uint32_t msr = qoriq_spin_lock(&list->lock);
		packet = list->head;
		if (packet != NULL) {
			list->head = packet->glue.next;
		} else {
			consumer *first = list->waiting_consumers [self].head;
			list->waiting_consumers [self].head = &poor;
			poor.next = first;
			if (first == NULL) {
				list->cores_to_notify |= ONE_CORE(self);
			}
		}
		qoriq_spin_unlock(&list->lock, msr);

		if (packet == NULL) {
			wait_for_event(INTERCOM_EVENT_WAKE_UP);
		}
	}

	packet->glue.next = NULL;
	packet->type_index = type;
	packet->flags = 0;
	packet->size = list->size;

	return packet;
}

intercom_packet *qoriq_intercom_allocate_packet(intercom_type type, intercom_size size)
{
	assert((unsigned) type < INTERCOM_SERVICE_COUNT);
	assert((unsigned) size < PACKET_SIZE_COUNT);

	return allocate(type, &intercom->free_lists [size]);
}

void qoriq_intercom_send_packets(int destination_core, intercom_packet *first, intercom_packet *last)
{
	assert(destination_core >= 0);
	assert(destination_core < INTERCOM_CORE_COUNT);

	core_fifo *fifo = &intercom->core_fifos [destination_core];

	uint32_t msr = qoriq_spin_lock(&fifo->lock);
	last->glue.next = NULL;
	if (fifo->head != NULL) {
		fifo->tail->glue.next = first;
	} else {
		fifo->head = first;
		notify_core_by_index(destination_core);
	}
	fifo->tail = last;
	qoriq_spin_unlock(&fifo->lock, msr);
}

void qoriq_intercom_broadcast_packets(intercom_packet *first, intercom_packet *last)
{
	int i = 0;

	for (i = 1; i < INTERCOM_CORE_COUNT; ++i) {
		intercom_packet *clone_first = NULL;
		intercom_packet *clone_last = NULL;

		intercom_packet *current = first;
		while (current != NULL) {
			intercom_packet *clone = qoriq_intercom_clone_packet(current);
			if (clone_first == NULL) {
				clone_first = clone;
			}
			if (clone_last != NULL) {
				clone_last->glue.next = clone;
			}
			clone_last = clone;
			current = current->glue.next;
		}

		qoriq_intercom_send_packets(i, clone_first, clone_last);
	}

	qoriq_intercom_send_packets(0, first, last);
}

void qoriq_intercom_send(int destination_core, intercom_type type, intercom_size size, const void *buf, size_t n)
{
	assert((unsigned) size < PACKET_SIZE_COUNT);

	size_t remaining = n;
	size_t packet_size = packet_sizes [size];
	const char *src = buf;
	intercom_packet *first = NULL;
	intercom_packet *last = NULL;

	do {
		intercom_packet *packet = qoriq_intercom_allocate_packet(
			type,
			size
		);
		if (first == NULL) {
			first = packet;
		}
		if (last != NULL) {
			last->glue.next = packet;
		}
		last = packet;
		size_t current_size = remaining < packet_size ? remaining : packet_size;
		remaining -= current_size;
		packet->size = current_size;
		const char *current = src;
		src += current_size;
		memcpy(packet->data, current, current_size);
	} while (remaining > 0);

	qoriq_intercom_send_packets(destination_core, first, last);
}

void qoriq_intercom_service_install(intercom_type type, intercom_service service, void *arg)
{
	assert((unsigned) type < INTERCOM_SERVICE_COUNT);

	uint32_t self = ppc_processor_id();
	intercom->service_args [self][type] = arg;
	ppc_enforce_in_order_execution_of_io();
	intercom->services [self][type] = service;
}

void qoriq_intercom_service_remove(intercom_type type)
{
	assert((unsigned) type < INTERCOM_SERVICE_COUNT);

	uint32_t self = ppc_processor_id();
	intercom->services [self][type] = default_service;
	ppc_enforce_in_order_execution_of_io();
	intercom->service_args [self][type] = NULL;
}

intercom_packet *qoriq_intercom_clone_packet(const intercom_packet *packet)
{
	intercom_packet *clone = qoriq_intercom_allocate_packet(
		packet->type_index,
		packet->size_index
	);

	clone->size = packet->size;
	memcpy(clone->data, packet->data, clone->size);

	return clone;
}
