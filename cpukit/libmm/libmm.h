#ifndef _RTEMS_LIBMM_H
#define _RTEMS_LIBMM_H
/*
 * *  COPYRIGHT (c) 1989-2011.
 * *  On-Line Applications Research Corporation (OAR).
 * *
 * *  The license and distribution terms for this file may be
 * *  found in the file LICENSE in this distribution or at
 * *  http://www.rtems.com/license/LICENSE.
 * */

#include <inttypes.h>
#include <rtems/rtems/status.h>
#include <rtems/chain.h>
#ifdef __cplusplus
  extern "C" {
#endif
#define RTEMS_MEMORY_MANAGEMENT_ALUT_SIZE 64

typedef rtems_bsp_mm_mpe;

/**
 *  * A region of contiguous memory
 *   */
typedef struct
{
  char  *name;
  void  *base;
  size_t size;
} rtems_memory_management_region_descriptor;

typedef struct
{
  rtems_chain_node 				node;  /**< The mapping chain's node */
  rtems_memory_management_region_descriptor 	region;
  bool						installed;
  rtems_bsp_mm_mpe			       *cpu_mpe;
} rtems_memory_management_entry;

/*!
 *  *  *  \brief  for now , the ALUT is just an array, we can optimize  this by some
 *   *   *  kind of advanced data structure , such as hash table, or chain maybe
 *    *    */
typedef struct
{
  rtems_memory_management_entry entries[RTEMS_MEMORY_MANAGEMENT_ALUT_SIZE];
  rtems_chain_control ALUT_mappings;
  rtems_chain_control ALUT_idle;
} rtems_memory_management_alut;




rtems_status_code rtems_memory_management_initialize ( void );


rtems_status_code rtems_memory_management_create_entry(
  rtems_memory_management_region_descriptor region, 
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

rtems_status_code rtems_memory_management_verify_size(
  size_t size
);

rtems_status_code rtems_memory_management_get_size(
 rtems_memory_management_entry* const p_entry,
 size_t * size);

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
