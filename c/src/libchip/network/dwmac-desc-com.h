/**
 * @file
 *
 * @brief DWMAC 10/100/1000 Common Descriptor Handling.
 *
 * DWMAC 10/100/1000 on-chip Ethernet controllers.
 * Functions and data which are common to normal and enhanced DMA descriptors.
 * This header file is NOT part of the driver API.
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
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

#ifndef DWMAC_DESC_COM_H_
#define DWMAC_DESC_COM_H_

#include "dwmac-common.h"
#include <sys/queue.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DWMAC_DESC_COM_HW_CRC_BYTES 4
#define DWMAC_DESC_COM_BUF_SIZE ( ETHER_MAX_LEN + DWMAC_DESC_COM_HW_CRC_BYTES )

struct mbuf *dwmac_desc_com_new_mbuf( dwmac_common_context *self );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DWMAC_DESC_COM_H_ */
