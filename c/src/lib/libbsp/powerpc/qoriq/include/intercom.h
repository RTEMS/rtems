/**
 * @file
 *
 * @ingroup QorIQInterCom
 *
 * @brief Inter-Processor Communication API.
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

#ifndef LIBBSP_POWERPC_QORIQ_INTERCOM_H
#define LIBBSP_POWERPC_QORIQ_INTERCOM_H

#include <rtems.h>
#include <rtems/chain.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup QorIQInterCom QorIQ - Inter-Processor Communication Support
 *
 * @ingroup QorIQ
 *
 * @brief Inter-processor communication support.
 *
 * @{
 */

uint32_t qoriq_spin_lock(uint32_t *lock);

void qoriq_spin_unlock(uint32_t *lock, uint32_t msr);

#define INTERCOM_CORE_COUNT 2

#define INTERCOM_SERVICE_COUNT 8

typedef enum {
	INTERCOM_TYPE_MPCI,
	INTERCOM_TYPE_UART_0,
	INTERCOM_TYPE_UART_1,
	INTERCOM_TYPE_NETWORK,
	INTERCOM_TYPE_CUSTOM_0,
	INTERCOM_TYPE_CUSTOM_1,
	INTERCOM_TYPE_CUSTOM_2,
	INTERCOM_TYPE_CUSTOM_3,
	INTERCOM_TYPE_CUSTOM_4
} intercom_type;

typedef enum {
	INTERCOM_SIZE_64 = 0,
	INTERCOM_SIZE_512,
	INTERCOM_SIZE_2K,
	INTERCOM_SIZE_4K
} intercom_size;

typedef struct intercom_packet {
	union {
		struct intercom_packet *next;
		rtems_chain_node node;
	} glue;
	intercom_type type_index;
	intercom_size size_index;
	uint32_t flags;
	size_t size;
	uint32_t cache_line_alignment [2];
	char data [];
} intercom_packet;

typedef void (*intercom_service)(intercom_packet *packet, void *arg);

void qoriq_intercom_init(void);

void qoriq_intercom_start(void);

void qoriq_intercom_service_install(intercom_type type, intercom_service service, void *arg);

void qoriq_intercom_service_remove(intercom_type type);

intercom_packet *qoriq_intercom_allocate_packet(intercom_type type, intercom_size size);

void qoriq_intercom_send_packets(int destination_core, intercom_packet *first, intercom_packet *last);

static inline void qoriq_intercom_send_packet(int destination_core, intercom_packet *packet)
{
	qoriq_intercom_send_packets(destination_core, packet, packet);
}

void qoriq_intercom_broadcast_packets(intercom_packet *first, intercom_packet *last);

static inline void qoriq_intercom_broadcast_packet(intercom_packet *packet)
{
	qoriq_intercom_broadcast_packets(packet, packet);
}

void qoriq_intercom_send(int destination_core, intercom_type type, intercom_size size, const void *buf, size_t n);

void qoriq_intercom_free_packet(intercom_packet *packet);

intercom_packet *qoriq_intercom_clone_packet(const intercom_packet *packet);

#ifdef RTEMS_MULTIPROCESSING
  extern rtems_mpci_table qoriq_intercom_mpci;
#endif

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_POWERPC_QORIQ_INTERCOM_H */
