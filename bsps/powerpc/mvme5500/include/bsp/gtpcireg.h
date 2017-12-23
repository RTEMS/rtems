/*	$NetBSD: gtpcireg.h,v 1.2 2003/03/24 17:03:18 matt Exp $	*/

/*
 * Copyright (c) 2002 Allegro Networks, Inc., Wasabi Systems, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed for the NetBSD Project by
 *      Allegro Networks, Inc., and Wasabi Systems, Inc.
 * 4. The name of Allegro Networks, Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 * 5. The name of Wasabi Systems, Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ALLEGRO NETWORKS, INC. AND
 * WASABI SYSTEMS, INC. ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL EITHER ALLEGRO NETWORKS, INC. OR WASABI SYSTEMS, INC.
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#define	PCI_ARBCTL_EN	(1<<31)

#define  PCI_COMMAND_SB_DIS     0x2000  /* PCI configuration read will stop
                                         * acting as sync barrier transactin
                                         */

#define PCI_MEM_BASE_ADDR PCI_BASE_ADDRESS_4

#define PCI_IO_BASE_ADDR  PCI_BASE_ADDRESS_5

#define  PCI_STATUS_CLRERR_MASK 0xf9000000 /* <SKF> */

#define PCI_BARE_IntMemEn     0x200

#define PCI_ACCCTLBASEL_PrefetchEn     0x0001000
#define PCI_ACCCTLBASEL_RdPrefetch     0x0010000
#define PCI_ACCCTLBASEL_RdLinePrefetch 0x0020000
#define PCI_ACCCTLBASEL_RdMulPrefetch  0x0040000
#define PCI_ACCCTLBASEL_WBurst_8_QW    0x0100000
#define PCI_ACCCTLBASEL_PCISwap_NoSwap 0x1000000

#define PCI0_P2P_CONFIG                         0x1d14
#define PCI_SNOOP_BASE0_LOW                     0x1f00
#define PCI_SNOOP_BASE0_HIGH                    0x1f04
#define PCI_SNOOP_TOP0                          0x1f08

#define PCI0_SCS0_BAR_SIZE                      0x0c08
#define PCI0_SCS1_BAR_SIZE                      0x0d08
#define PCI0_SCS2_BAR_SIZE                      0x0c0c
#define PCI0_SCS3_BAR_SIZE                      0x0d0c

#define PCI0_BASE_ADDR_REG_ENABLE               0x0c3c
#define PCI0_ARBITER_CNTL                       0x1d00
#define PCI0_ACCESS_CNTL_BASE0_LOW              0x1e00
#define PCI0_ACCESS_CNTL_BASE0_HIGH             0x1e04
#define PCI0_ACCESS_CNTL_BASE0_TOP              0x1e08

#define PCI0_ACCESS_CNTL_BASE1_LOW              0x1e10
#define PCI0_ACCESS_CNTL_BASE1_HIGH             0x1e14
#define PCI0_ACCESS_CNTL_BASE1_TOP              0x1e18

#define PCI1_BASE_ADDR_REG_ENABLE               0x0cbc
#define PCI1_ARBITER_CNTL                       0x1d80
#define PCI1_ACCESS_CNTL_BASE0_LOW              0x1e80
#define PCI1_ACCESS_CNTL_BASE0_HIGH             0x1e84
#define PCI1_ACCESS_CNTL_BASE0_TOP              0x1e88

#define PCI1_ACCESS_CNTL_BASE1_LOW              0x1e90
#define PCI1_ACCESS_CNTL_BASE1_HIGH             0x1e94
#define PCI1_ACCESS_CNTL_BASE1_TOP              0x1e98

#define PCI_SNOOP_BASE1_LOW                     0x1f10
#define PCI_SNOOP_BASE1_HIGH                    0x1f14
#define PCI_SNOOP_TOP1                          0x1f18

#define PCI0_CMD_CNTL                           0xc00

#define PCI1_P2P_CONFIG                         0x1d94
#define PCI1_CMD_CNTL                           0xc80
#define PCI1_CONFIG_ADDR			0xc78
#define PCI1_CONFIG_DATA			0xc7c
