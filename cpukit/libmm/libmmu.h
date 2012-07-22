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
#include <rtems/chain.h>
#ifdef __cplusplus
  extern "C" {
#endif
#define RTEMS_MPROT_ALUT_SIZE 64
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
 *           *  These methods used by rtems_memory_management_XXX APIs are defined in 
 *            *  /libcpu/shared/src/page_table_manager.c
 *             */
#define RTEMS_MPROT_LOCKED				0x0
#define RTEMS_MPROT_READ_ONLY                             0x1
#define RTEMS_MPROT_WRITE_ENABLE                           0x2
#define RTEMS_MPROT_EXE_ENABLE                                0x3
#define RTEMS_MPROT_SUPERVISOR_READ_ENABLE       0x4
#define RTEMS_MPROT_SUPERVISOR_WRITE_ENABLE     0x5

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


/*!
 *  *  \brief  
 *   *
 *    * typedef struct mprot_alut rtems_mprot_alut;
 *     */
/*write buffer is valid in arm*/
#if 0
#define RTEMS_MPROT_WRITEBUFFER_ENABLE                          0x800
#define RTEMS_MPROT_WRITEBUFFER_DISABLE                 0x000
#endif

#define RTEMS_MPROT_MAP_FIXED              0x10000
#define RTEMS_MPROT_MAP_PRIVATE          0x20000


/*!
 *  *  *  \brief  the default access attribute of the address blocks not included in ALUT
 *   *   */
#define RTEMS_MPROT_DEFAULT_ATTRIBUTE            0x0000071f


/**
 *  * A region of contiguous memory
 *   */
typedef struct
{
  char  *name;
  void  *base;
  size_t bounds;
} rtems_memory_management_region_descriptor;

typedef struct
{
  rtems_chain_node 				node;  /**< The mapping chain's node */
  rtems_memory_management_region_descriptor 	region;
  bool						installed;
  uint32_t 					permissions;
} rtems_memory_management_entry;

/*!
 *  *  *  \brief  for now , the ALUT is just an array, we can optimize  this by some
 *   *   *  kind of advanced data structure , such as hash table, or chain maybe
 *    *    */
typedef struct mprot_alut
{
  rtems_memory_management_entry entries[RTEMS_MPROT_ALUT_SIZE];
  rtems_chain_control ALUT_mappings;
  rtems_chain_control ALUT_idle;
  uint32_t rtems_mprot_default_permissions;
} rtems_mprot_alut;




rtems_status_code rtems_memory_management_initialize ( void );

void rtems_management_update_entry(rtems_memory_management_entry*  mpe);

rtems_status_code rtems_memory_management_create_entry(
  rtems_memory_management_region_descriptor region, 
  const uint32_t premissions,
  rtems_memory_management_entry** p_ret);

/**
 *  * @brief rtems_memory_management_install_entry
 *   *
 *    * Calls _CPU_Memory_management_Install_MPE to enforce the permissions
 *     * on @a mpe. Implemented in libcpu/shared/
 *      */
rtems_status_code rtems_memory_management_install_entry(
  rtems_memory_management_entry *mpe
);

rtems_status_code rtems_memory_management_delete_entry(
  rtems_memory_management_entry* const p_entry);

rtems_status_code rtems_memory_management_find_entry(
  void* const addr,
  rtems_memory_management_entry** p_ret);

rtems_status_code rtems_memory_management_verify_permission(
  uint32_t perms;
);

rtems_status_code rtems_memory_management_verify_size(
  size_t size
);

rtems_status_code rtems_memory_management_set_permissions(
  rtems_memory_management_entry* const p_entry,
  const uint32_t new_permissions,
  uint32_t*  old_permissions );

rtems_status_code rtems_memory_management_get_permissions(
 rtems_memory_management_entry* const p_entry,
 uint32_t*  permissions);

rtems_status_code rtems_memory_management_get_size(
 rtems_memory_management_entry* const p_entry,
 size_t * size);

uint32_t rtems_memory_management_get_default_permissions( void );

rtems_status_code 
rtems_memory_management_set_default_permissions(
  const uint32_t new_permissions,
  uint32_t* old_permissions );

rtems_status_code rtems_memory_management_verify_permissions(uint32_t);

rtems_status_code rtems_pagetable_update_permissions(uint32_t, int, int);


rtems_status_code rtems_memory_management_set_write
(
  rtems_memory_management_entry* const mpe;
);

rtems_status_code rtems_memory_management_set_read_only
(
  rtems_memory_management_entry* const mpe;
);
#ifdef __cplusplus
  }
#endif
#endif
