/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicAttr
 *
 * @brief This header file provides the implementation interfaces of
 *   the @ref RTEMSImplClassicAttr support.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_RTEMS_ATTR_INL
#define _RTEMS_RTEMS_ATTR_INL

#include <rtems/rtems/attr.h>
#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSImplClassicAttr Directive Attributes
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This group contains the implementation to support directive
 *   attributes.
 *
 * @{
 */

/****************** Forced Attributes in Configuration ****************/

/**
 *  This attribute constant indicates the attributes that are not
 *  supportable given the hardware configuration.
 */
#define ATTRIBUTES_NOT_SUPPORTED       0

/**
 *  This attribute constant indicates the attributes that are
 *  required given the hardware configuration.
 */
#if ( CPU_ALL_TASKS_ARE_FP == TRUE )
#define ATTRIBUTES_REQUIRED            RTEMS_FLOATING_POINT
#else
#define ATTRIBUTES_REQUIRED            0
#endif

/**
 *  @brief Sets the requested new_attributes in the attribute_set passed in.
 *
 *  This function sets the requested new_attributes in the attribute_set
 *  passed in.  The result is returned to the user.
 */
static inline rtems_attribute _Attributes_Set (
   rtems_attribute new_attributes,
   rtems_attribute attribute_set
)
{
  return attribute_set | new_attributes;
}

/**
 *  @brief Clears the requested new_attributes in the attribute_set
 *  passed in.
 *
 *  This function clears the requested new_attributes in the attribute_set
 *  passed in.  The result is returned to the user.
 */
static inline rtems_attribute _Attributes_Clear (
   rtems_attribute attribute_set,
   rtems_attribute mask
)
{
  return attribute_set & ~mask;
}

/**
 *  @brief Checks if the floating point attribute is
 *  enabled in the attribute_set.
 *
 *  This function returns TRUE if the floating point attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */
static inline bool _Attributes_Is_floating_point(
  rtems_attribute attribute_set
)
{
   return ( attribute_set & RTEMS_FLOATING_POINT ) ? true : false;
}

#if defined(RTEMS_MULTIPROCESSING)
/**
 *  @brief Checks if the global object attribute is enabled in
 *  the attribute_set.
 *
 *  This function returns TRUE if the global object attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */
static inline bool _Attributes_Is_global(
  rtems_attribute attribute_set
)
{
   return ( attribute_set & RTEMS_GLOBAL ) ? true : false;
}
#endif

/**
 *  @brief Checks if the priority attribute is enabled in the attribute_set.
 *
 *  This function returns TRUE if the priority attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */
static inline bool _Attributes_Is_priority(
  rtems_attribute attribute_set
)
{
   return ( attribute_set & RTEMS_PRIORITY ) ? true : false;
}

/**
 *  @brief Checks if the binary semaphore attribute is
 *  enabled in the attribute_set.
 *
 *  This function returns TRUE if the binary semaphore attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */
static inline bool _Attributes_Is_binary_semaphore(
  rtems_attribute attribute_set
)
{
  return ((attribute_set & RTEMS_SEMAPHORE_CLASS) == RTEMS_BINARY_SEMAPHORE);
}

/**
 *  @brief Checks if the simple binary semaphore attribute is
 *  enabled in the attribute_set
 *
 *  This function returns TRUE if the simple binary semaphore attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */
static inline bool _Attributes_Is_simple_binary_semaphore(
  rtems_attribute attribute_set
)
{
  return
    ((attribute_set & RTEMS_SEMAPHORE_CLASS) == RTEMS_SIMPLE_BINARY_SEMAPHORE);
}

/**
 *  @brief Checks if the counting semaphore attribute is
 *  enabled in the attribute_set
 *
 *  This function returns TRUE if the counting semaphore attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */
static inline bool _Attributes_Is_counting_semaphore(
  rtems_attribute attribute_set
)
{
  return ((attribute_set & RTEMS_SEMAPHORE_CLASS) == RTEMS_COUNTING_SEMAPHORE);
}

/**
 *  @brief Checks if the priority inheritance attribute
 *  is enabled in the attribute_set
 *
 *  This function returns TRUE if the priority inheritance attribute
 *  is enabled in the attribute_set and FALSE otherwise.
 */
static inline bool _Attributes_Is_inherit_priority(
  rtems_attribute attribute_set
)
{
   return ( attribute_set & RTEMS_INHERIT_PRIORITY ) ? true : false;
}

/**
 * @brief Returns true if the attribute set has at most one protocol, and false
 * otherwise.
 *
 * The protocols are RTEMS_INHERIT_PRIORITY, RTEMS_PRIORITY_CEILING and
 * RTEMS_MULTIPROCESSOR_RESOURCE_SHARING.
 */
static inline bool _Attributes_Has_at_most_one_protocol(
  rtems_attribute attribute_set
)
{
  attribute_set &= RTEMS_INHERIT_PRIORITY | RTEMS_PRIORITY_CEILING
    | RTEMS_MULTIPROCESSOR_RESOURCE_SHARING;

  return ( attribute_set & ( attribute_set - 1 ) ) == 0;
}

/**
 *  @brief Checks if the priority ceiling attribute
 *  is enabled in the attribute_set
 *
 *  This function returns TRUE if the priority ceiling attribute
 *  is enabled in the attribute_set and FALSE otherwise.
 */
static inline bool _Attributes_Is_priority_ceiling(
  rtems_attribute attribute_set
)
{
   return ( attribute_set & RTEMS_PRIORITY_CEILING ) ? true : false;
}

/**
 *  @brief Checks if the Multiprocessor Resource Sharing Protocol attribute
 *  is enabled in the attribute_set
 *
 *  This function returns TRUE if the Multiprocessor Resource Sharing Protocol
 *  attribute is enabled in the attribute_set and FALSE otherwise.
 */
static inline bool _Attributes_Is_multiprocessor_resource_sharing(
  rtems_attribute attribute_set
)
{
  return ( attribute_set & RTEMS_MULTIPROCESSOR_RESOURCE_SHARING ) != 0;
}

/**
 *  @brief Checks if the barrier automatic release
 *  attribute is enabled in the attribute_set
 *
 *  This function returns TRUE if the barrier automatic release
 *  attribute is enabled in the attribute_set and FALSE otherwise.
 */
static inline bool _Attributes_Is_barrier_automatic(
  rtems_attribute attribute_set
)
{
   return ( attribute_set & RTEMS_BARRIER_AUTOMATIC_RELEASE ) ? true : false;
}

/**
 *  @brief Checks if the system task attribute
 *  is enabled in the attribute_set.
 *
 *  This function returns TRUE if the system task attribute
 *  is enabled in the attribute_set and FALSE otherwise.
 */
static inline bool _Attributes_Is_system_task(
  rtems_attribute attribute_set
)
{
   return ( attribute_set & RTEMS_SYSTEM_TASK ) ? true : false;
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
