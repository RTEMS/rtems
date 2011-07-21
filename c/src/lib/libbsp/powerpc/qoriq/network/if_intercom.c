/*
 * Copyright (c) 2011 embedded brains GmbH.	All rights reserved.
 *
 *	embedded brains GmbH
 *	Obere Lagerstr. 30
 *	82178 Puchheim
 *	Germany
 *	<info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#define __INSIDE_RTEMS_BSD_TCPIP_STACK__ 1
#define __BSD_VISIBLE 1

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/rtems_mii_ioctl.h>

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/mbuf.h>

#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>

#include <libcpu/powerpc-utility.h>

#include <bsp.h>
#include <bsp/intercom.h>

typedef struct {
	struct arpcom arpcom;
	int destination_core;
	intercom_packet *packet;
	unsigned transmitted_frames;
	unsigned received_frames;
} if_intercom_control;

static if_intercom_control if_intercom;

static struct mbuf *new_mbuf(struct ifnet *ifp, bool wait)
{
	struct mbuf *m = NULL;
	int mw = wait ? M_WAIT : M_DONTWAIT;

	MGETHDR(m, mw, MT_DATA);
	if (m != NULL) {
		MCLGET(m, mw);
		if ((m->m_flags & M_EXT) != 0) {
			m->m_pkthdr.rcvif = ifp;

			return m;
		} else {
			m_free(m);
		}
	}

	return NULL;
}

static void if_intercom_service(intercom_packet *packet, void *arg)
{
	rtems_bsdnet_semaphore_obtain();

	if_intercom_control *self = arg;
	struct ifnet *ifp = &self->arpcom.ac_if;
	struct mbuf *m = new_mbuf(ifp, true);

	memcpy(mtod(m, void *), packet->data, packet->size);

	/* Ethernet header */
	struct ether_header *eh = mtod(m, struct ether_header *);

	/* Discard Ethernet header and CRC */
	int sz = (int) packet->size - ETHER_HDR_LEN;

	/* Update mbuf */
	m->m_len = sz;
	m->m_pkthdr.len = sz;
	m->m_data = mtod(m, char *) + ETHER_HDR_LEN;

	/* Hand over */
	ether_input(ifp, eh, m);

	++self->received_frames;
	qoriq_intercom_free_packet(packet);

	rtems_bsdnet_semaphore_release();
}

static intercom_packet *allocate_packet(void)
{
	intercom_packet *packet = qoriq_intercom_allocate_packet(
		INTERCOM_TYPE_NETWORK,
		INTERCOM_SIZE_2K
	);

	packet->size = 0;

	return packet;
}

static struct mbuf *get_next_fragment(struct ifnet *ifp, struct mbuf *m)
{
	struct mbuf *n = NULL;
	int size = 0;

	while (true) {
		if (m == NULL) {
			/* Dequeue first fragment of the next frame */
			IF_DEQUEUE(&ifp->if_snd, m);

			/* Empty queue? */
			if (m == NULL) {
				return m;
			}
		}

		/* Get fragment size */
		size = m->m_len;

		if (size > 0) {
			/* Now we have a not empty fragment */
			break;
		} else {
			/* Discard empty fragments */
			m = m_free(m);
		}
	}

	/* Discard empty successive fragments */
	n = m->m_next;
	while (n != NULL && n->m_len <= 0) {
		n = m_free(n);
	}
	m->m_next = n;

	return m;
}

static void if_intercom_start(struct ifnet *ifp)
{
	if_intercom_control *self = ifp->if_softc;
	int destination_core = self->destination_core;
	intercom_packet *packet = self->packet;
	size_t size = packet->size;
	char *data = packet->data + size;
	struct mbuf *m = NULL;

	while ((m = get_next_fragment(ifp, m)) != NULL) {
		size_t fragment_size = (size_t) m->m_len;
		size_t new_size = size + fragment_size;

		assert(new_size <= 2048);

		memcpy(data, mtod(m, void *), fragment_size);
		data += fragment_size;
		size = new_size;

		m = m_free(m);

		/* Last fragment of frame ? */
		if (m == NULL) {
			packet->size = size;
			qoriq_intercom_send_packet(destination_core, packet);
			++self->transmitted_frames;
			packet = allocate_packet();
			data = packet->data;
			size = 0;
		}
	}

	packet->size = size;
	self->packet = packet;
	ifp->if_flags &= ~IFF_OACTIVE;
}

static void if_intercom_init(void *arg)
{
	if_intercom_control *self = arg;
	uint32_t self_core = ppc_processor_id();

	self->destination_core = self_core == 0 ? 1 : 0;
	self->packet = allocate_packet();

	qoriq_intercom_service_install(
		INTERCOM_TYPE_NETWORK,
		if_intercom_service,
		self
	);
}

static void show_stats(if_intercom_control *self)
{
	printf("transmitted frames: %u\n", self->transmitted_frames);
	printf("received frames: %u\n", self->received_frames);
}

static int if_intercom_ioctl(
	struct ifnet *ifp,
	ioctl_command_t command,
	caddr_t data
)
{
	if_intercom_control *self = ifp->if_softc;
	int rv = 0;

	switch (command)	{
		case SIOCGIFADDR:
		case SIOCSIFADDR:
			ether_ioctl(ifp, command, data);
			break;
		case SIOCSIFFLAGS:
			if (ifp->if_flags & IFF_RUNNING) {
				/* TODO: off */
			}
			if (ifp->if_flags & IFF_UP) {
				ifp->if_flags |= IFF_RUNNING;
				/* TODO: init */
			}
			break;
		case SIO_RTEMS_SHOW_STATS:
			show_stats(self);
			break;
		default:
			rv = EINVAL;
			break;
	}

	return rv;
}

static void if_intercom_watchdog(struct ifnet *ifp)
{
	ifp->if_timer = 0;
}

static int if_intercom_attach(struct rtems_bsdnet_ifconfig *config)
{
	if_intercom_control *self = &if_intercom;
	struct ifnet *ifp = &self->arpcom.ac_if;
	char *unit_name = NULL;
	int unit_index = rtems_bsdnet_parse_driver_name(config, &unit_name);

	assert(unit_index == 1);
	assert(strcmp(unit_name, "intercom") == 0);
	assert(config->hardware_address != NULL);

	memcpy(self->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);

	/* Set interface data */
	ifp->if_softc = self;
	ifp->if_unit = (short) unit_index;
	ifp->if_name = unit_name;
	ifp->if_mtu = (config->mtu > 0) ? (u_long) config->mtu : ETHERMTU;
	ifp->if_init = if_intercom_init;
	ifp->if_ioctl = if_intercom_ioctl;
	ifp->if_start = if_intercom_start;
	ifp->if_output = ether_output;
	ifp->if_watchdog = if_intercom_watchdog;
	ifp->if_flags = config->ignore_broadcast ? 0 : IFF_BROADCAST;
	ifp->if_snd.ifq_maxlen = ifqmaxlen;
	ifp->if_timer = 0;

	/* Attach the interface */
	if_attach(ifp);
	ether_ifattach(ifp);

	return 1;
}

int qoriq_if_intercom_attach_detach(
	struct rtems_bsdnet_ifconfig *config,
	int attaching
)
{
	if (attaching) {
		return if_intercom_attach(config);
	} else {
		assert(0);
	}
}
