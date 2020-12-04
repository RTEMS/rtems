/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ofw
 *
 * This file defines necessary symbols to make the RTEMS OFW API compatible
 * with FreeBSD OFW.
 */

/*
 * Copyright (C) 2020 Niteesh Babu G S <niteesh.gs@gmail.com>
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

#ifndef _OFW_COMPAT_H
#define _OFW_COMPAT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ofw/ofw.h>

#define OF_peer                   rtems_ofw_peer
#define OF_child                  rtems_ofw_child
#define OF_parent                 rtems_ofw_parent
#define OF_getproplen             rtems_ofw_get_prop_len
#define OF_getprop                rtems_ofw_get_prop
#define OF_getencprop             rtems_ofw_get_enc_prop
#define OF_hasprop                rtems_ofw_has_prop
#define OF_searchprop             rtems_ofw_search_prop
#define OF_searchencprop          rtems_ofw_search_enc_prop
#define OF_getprop_alloc          rtems_ofw_get_prop_alloc
#define OF_getprop_alloc_multi    rtems_ofw_get_prop_alloc_multi
#define OF_getencprop_alloc_multi rtems_ofw_get_enc_prop_alloc_multi
#define OF_getencprop_alloc       rtems_ofw_get_enc_prop_alloc
#define OF_prop_free              rtems_ofw_free
#define OF_nextprop               rtems_ofw_next_prop
#define OF_setprop                rtems_ofw_set_prop
#define OF_canon                  rtems_ofw_canon
#define OF_finddevice             rtems_ofw_find_device
#define OF_package_to_path        rtems_ofw_package_to_path
#define OF_node_from_xref         rtems_ofw_node_from_xref
#define OF_xref_from_node         rtems_ofw_xref_from_node
#define OF_instance_to_package    rtems_ofw_instance_to_package
#define OF_instance_to_path       rtems_ofw_instance_to_path

#ifdef __cplusplus
}
#endif

#endif /* _OFW_COMPAT_H */
