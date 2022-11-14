/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief RTEMS local PCI data base
 */

/*
 *  Copyright (C) 2011 Daniel Hellstrom Gaisler <daniel@gaisler.com>
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

/* Only included from pci_ids.h */
#ifndef __PCI_IDS_H__
#error pci/ids_extra.h should only be included from pci/ids.h
#endif

/* Gaisler PCI IDs */
#define PCIID_VENDOR_GAISLER		0x1AC8
#define PCIID_VENDOR_GAISLER_OLD	0x16E3

/* Gaisler PCI Devices */
#define PCIID_DEVICE_GR_RASTA_IO	0x0010	/* GR-RASTA-IO */
#define PCIID_DEVICE_GR_RASTA_IO_OLD	0x0210	/* old GR-RASTA-IO ID*/
#define PCIID_DEVICE_GR_RASTA_TMTC	0x0011	/* GR-RASTA-TMTC */
#define PCIID_DEVICE_GR_RASTA_ADCDAC	0x0014	/* GR-RASTA-ADCDAC */
#define PCIID_DEVICE_GR_701		0x0701	/* GR-701 */
#define PCIID_DEVICE_GR_TMTC_1553       0x0198  /* GR-TMTC-1553 */
#define PCIID_DEVICE_GR_RASTA_SPW_RTR   0x0062  /* GR-RASTA-SPW-ROUTER */
#define PCIID_DEVICE_GR_LEON4_N2X       0x0061  /* GR-CPCI-LEON4-N2X */
#define PCIID_DEVICE_GR_NGMP_PROTO      0x0064  /* GR-NGMP_PROTO */
#define PCIID_DEVICE_GR_CPCI_GR740      0x0740  /* GR-CPCI-GR740 */
