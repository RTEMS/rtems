/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicObject
 *
 * @brief This header file provides the implementation interfaces of
 *   the @ref RTEMSImplClassicObject.
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

#ifndef _RTEMS_RTEMS_OBJECTIMPL_H
#define _RTEMS_RTEMS_OBJECTIMPL_H

#include <rtems/score/objectimpl.h>
#include <rtems/rtems/status.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSImplClassicObject Object Services
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This group contains the Object Services implementation.
 *
 * @{
 */

/**
 * @brief Calls _Objects_Name_to_id_u32() and converts the status.
 *
 * @param name is the name of the object to find.
 * @param node is the set of nodes to search.
 * @param[out] id is the pointer to an object identifier variable or NULL.  The
 *   object identifier will be stored in the referenced variable, if the
 *   operation was successful.
 * @param information is the pointer to an object class information block.
 *
 * @retval RTEMS_SUCCESSFUL The operations was successful.
 * @retval RTEMS_INVALID_ADDRESS The id parameter was NULL.
 * @retval RTEMS_INVALID_NAME No object exists with the specified name on the
 *   specified node set.
 */
rtems_status_code _RTEMS_Name_to_id(
  uint32_t                   name,
  uint32_t                   node,
  Objects_Id                *id,
  const Objects_Information *information
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_OBJECTIMPL_H */
