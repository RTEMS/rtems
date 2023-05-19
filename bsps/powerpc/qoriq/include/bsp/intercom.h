/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQInterCom
 *
 * @brief Inter-Processor Communication API.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
 * @ingroup RTEMSBSPsPowerPCQorIQ
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
