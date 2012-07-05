#ifndef _RTEMS_LIBMMU_H
#define _RTEMS_LIBMMU_H
/*
 * *  COPYRIGHT (c) 1989-2011.
 * *  On-Line Applications Research Corporation (OAR).
 * *
 * *  The license and distribution terms for this file may be
 * *  found in the file LICENSE in this distribution or at
 * *  http://www.rtems.com/license/LICENSE.
 * *
 * *
 * *  $Id$
 * */

#include <inttypes.h>
#include <rtems/rtems/status.h>

#ifdef __cplusplus
  extern "C" {
#endif
#define RTEMS_MPROT_PAGE_SIZE 0x1000

/*!
 *  * \brief  Access attribute definition below
 *   *  The attribute, especially the cache attribute macro definition is mainly 
 *    *  based on powerpc architecture for now. This may not be proper for some 
 *     *  other architectures. In that case, users can modify and define there own 
 *      *  extra attributes. 
 *       *  Please notice that the attributes difined here should not be too much related
 *        *  with the hardware. Users need to implement several other methods in libcpu
 *         *  to check whether attributes are legal or to translate the  cache and access 
 *          *  attributes to the attribute bit mode defined by hardware  
 *           *  These methods used by rtems_memory_protect_XXX APIs are defined in 
 *            *  /libcpu/shared/src/page_table_manager.c
 *             */
#define RTEMS_MPROT_READ_ENABLE                             0x1
#define RTEMS_MPROT_WRITE_ENABLE                           0x2
#define RTEMS_MPROT_SUPERVISOR_READ_ENABLE       0x4
#define RTEMS_MPROT_SUPERVISOR_WRITE_ENABLE     0x8
#define RTEMS_MPROT_EXE_ENABLE                                0x10

#define RTEMS_MPROT_WRITE_DISABLE                          0x0
#define RTEMS_MPROT_READ_DISABLE                            0x0
#define RTEMS_MPROT_SUPERVISOR_WRITE_DISABLE    0x0
#define RTEMS_MPROT_SUPERVISOR_READ_DISABLE      0x0
#define RTEMS_MPROT_EXE_DISABLE                               0x0

#define RTEMS_MPROT_CACHE_ENABLE                   0x100
#define RTEMS_MPROT_CACHE_DISABLE                  0x000
#define RTEMS_MPROT_CACHE_WRITE_THROUGH    0x200
#define RTEMS_MPROT_CACHE_WRITE_BACK           0x000
#define RTEMS_MPROT_CACHE_COHERENCY                0x400
#define RTEMS_MPROT_CACHE_NONCOHERENCY      0x000
#define RTEMS_MPROT_CACHE_GUARDED                0x800
#define RTEMS_MPROT_CACHE_UNGUARDED                0x000

#if 0
/*the attribute between defferent CPU architecture is quite different
 * *  for example , write buffer is valid in arm
 * *  the low level mmu support and page table method need to match 
 * *  these attributes, but for the libmmu level, we only need to define 
 * *  the attibutes here
 * */
#define RTEMS_MPROT_WRITEBUFFER_ENABLE                          0x800
#define RTEMS_MPROT_WRITEBUFFER_DISABLE                 0x000
#endif


#ifndef _RTEMS_LIBMMU_INTERNAL_H
typedef struct memory_protect_entry rtems_memory_protect_entry;
#endif
/*!
 *  *  \brief  
 *   *
 *    * typedef struct mprot_alut rtems_mprot_alut;
 *     */

rtems_status_code rtems_memory_protect_init ( void );

rtems_status_code rtems_memory_protect_create(
  void* const start_addr, 
  const size_t size, 
  const uint32_t attr,
  rtems_memory_protect_entry** p_ret);

rtems_status_code rtems_memory_protect_delete(
  rtems_memory_protect_entry* const p_entry);

rtems_status_code rtems_memory_protect_search(
  void* const addr,
  rtems_memory_protect_entry** p_ret);

rtems_status_code rtems_memory_protect_set_attr(
  rtems_memory_protect_entry* const p_entry,
  const uint32_t new_attribute,
  uint32_t*  old_attribute );

rtems_status_code rtems_memory_protect_get_attr(
 rtems_memory_protect_entry* const p_entry,
 uint32_t*  attr);

rtems_status_code rtems_memory_protect_get_size(
 rtems_memory_protect_entry* const p_entry,
 size_t * size);

uint32_t rtems_memory_protect_get_default_attr( void );

rtems_status_code 
rtems_memory_protect_set_default_attr(
  const uint32_t new_attribute,
  uint32_t* old_attribute );
#ifdef __cplusplus
  }
#endif
#endif
