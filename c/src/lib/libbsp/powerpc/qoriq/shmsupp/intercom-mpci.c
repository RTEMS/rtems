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

#include <libcpu/powerpc-utility.h>

#include <bsp/intercom.h>

#ifdef RTEMS_MULTIPROCESSING

typedef struct {
	intercom_packet *head;
	intercom_packet *tail;
} mpic_fifo;

static mpic_fifo fifo;

static void mpci_service(intercom_packet *packet, void *arg)
{
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	packet->glue.next = NULL;
	if (fifo.head != NULL) {
		fifo.tail->glue.next = packet;
	} else {
		fifo.head = packet;
	}
	fifo.tail = packet;
	rtems_interrupt_enable(level);

	rtems_multiprocessing_announce();
}

static void mpci_init(void)
{
	qoriq_intercom_service_install(INTERCOM_TYPE_MPCI, mpci_service, NULL);
}

static intercom_packet *packet_of_prefix(rtems_packet_prefix *prefix)
{
	return (intercom_packet *) ((char *) prefix - sizeof(intercom_packet));
}

static rtems_packet_prefix *prefix_of_packet(intercom_packet *packet)
{
	return (rtems_packet_prefix *) packet->data;
}

static void mpci_get_packet(rtems_packet_prefix **prefix_ptr)
{
	intercom_packet *packet = qoriq_intercom_allocate_packet(
		INTERCOM_TYPE_MPCI,
		INTERCOM_SIZE_512
	);
	*prefix_ptr = prefix_of_packet(packet);
}

static void mpci_return_packet(rtems_packet_prefix *prefix)
{
	intercom_packet *packet = packet_of_prefix(prefix);

	qoriq_intercom_free_packet(packet);
}

static void mpci_send_packet(uint32_t destination_node, rtems_packet_prefix *prefix)
{
	intercom_packet *packet = packet_of_prefix(prefix);
	if (destination_node != MPCI_ALL_NODES) {
		qoriq_intercom_send_packet((int) destination_node - 1, packet);
	} else {
		uint32_t self = ppc_processor_id();
		int other = self == 0 ? 1 : 0;

		qoriq_intercom_send_packet(other, packet);
	}
}

static void mpci_receive_packet(rtems_packet_prefix **prefix_ptr)
{
	rtems_interrupt_level level;

	rtems_interrupt_disable(level);
	intercom_packet *packet = fifo.head;
	if (packet != NULL) {
		fifo.head = packet->glue.next;
		*prefix_ptr = prefix_of_packet(packet);
	} else {
		*prefix_ptr = NULL;
	}
	rtems_interrupt_enable(level);
}

rtems_mpci_table qoriq_intercom_mpci = {
	.default_timeout = UINT32_MAX,
	.maximum_packet_size = 512 - sizeof(rtems_packet_prefix),
	.initialization = mpci_init,
	.get_packet = mpci_get_packet,
	.return_packet = mpci_return_packet,
	.send_packet = mpci_send_packet,
	.receive_packet = mpci_receive_packet
};

#endif /* RTEMS_MULTIPROCESSING */
