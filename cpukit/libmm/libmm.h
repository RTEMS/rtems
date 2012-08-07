/*
 * @file libmm.h
 * 
 * @ingroup libmm
 *
 * @brief libmm API make use of MPU/MMU units to provide 
 * memory management
 */

/*
 * COPYRIGHT (c) 1989-2011. 
 *
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_LIBMM_H
#define _RTEMS_LIBMM_H

#include <inttypes.h>
#include <rtems/rtems/status.h>
#include <rtems/chain.h>

#ifdef __cplusplus
  extern "C" {
#endif

/**
 * This should be configurable 
 */
#define RTEMS_MEMORY_MANAGEMENT_ALUT_SIZE 64

/**
 * Define incomplete pointer to point to
 * bsp_mm_mpe defined at CPU code, the address 
 * is installed at CPU code when Installing an mpe 
 */
typedef rtems_bsp_mm_mpe;

/*
 * @brief A region of contiguous memory
 */
typedef struct
{
  char  *name;
  void  *base;
  size_t size;
} rtems_memory_management_region_descriptor;

typedef struct
{
  rtems_chain_node 				                    node;  /*  The mapping chain's node */
  rtems_memory_management_region_descriptor 	region;
  bool						                            installed;
  rtems_bsp_mm_mpe			                      *cpu_mpe;
} rtems_memory_management_entry;

/**
 * @brief  for now , the ALUT is just an array, we can optimize  this by some
 * kind of advanced data structure , such as hash table, or chain maybe
 */
typedef struct
{
  rtems_memory_management_entry entries[RTEMS_MEMORY_MANAGEMENT_ALUT_SIZE];
  rtems_chain_control ALUT_mappings;
  rtems_chain_control ALUT_idle;
} rtems_memory_management_alut;

/**
 * @brief Initializing memory management unit and necessary actions depending 
 * on the target. This function calls corresponding _CPU_Memory_management_xxx
 * at BSP code to enforce HW initialization. Implemented in libcpu/shared/
 */
rtems_status_code rtems_memory_management_initialize ( void );

/**
 *  @brief creating a high-level entry for mpe
 *  the functions returns a pointer to the new allocated
 *  mpe at @a p_ret
 */
rtems_status_code rtems_memory_management_create_entry(
  rtems_memory_management_region_descriptor region, 
  rtems_memory_management_entry** p_ret
);

/**
 * @brief rtems_memory_management_install_entry
 * Calls _CPU_Memory_management_Install_MPE to enforce the permissions
 * on @a mpe. Implemented in libcpu/shared/
 */
rtems_status_code rtems_memory_management_install_entry(
  rtems_memory_management_entry *mpe
);

/**
 * @brief deleting entry from ALUT as well as HW entries
 */
rtems_status_code rtems_memory_management_delete_entry(
  rtems_memory_management_entry* const p_entry
);

/**
 * @brief Linear search for the element emtry in the alut for
 * the address range under which it falls  
 */
rtems_status_code rtems_memory_management_find_entry(
  void* const addr,
  rtems_memory_management_entry** p_ret
);

rtems_status_code rtems_memory_management_verify_size(
  size_t size
);

/**
 * @brief check if size is supported by the CPU. 
 * this method is implemented at CPU code*/
rtems_status_code rtems_memory_management_get_size(
 rtems_memory_management_entry* const p_entry,
 size_t * size
);

/** 
 * @brief set HW write AP for that mpe 
 */
rtems_status_code rtems_memory_management_set_write
(
  rtems_memory_management_entry* const mpe;
);

/** 
 * @brief set HW Read Only AP for that mpe 
 */
rtems_status_code rtems_memory_management_set_read_only
(
  rtems_memory_management_entry* const mpe;
);
#ifdef __cplusplus
}
#endif
#endif
