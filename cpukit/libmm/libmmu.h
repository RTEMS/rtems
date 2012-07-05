/**
 * @file
 *
 */
/*
 * Copyright (C) 2011. Gedare Bloom.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 *
 * $Id$
 */
#ifndef _RTEMS_LIBMMU_H
#define _RTEMS_LIBMMU_H

#include <inttypes.h>
#include <rtems/chain.h>
#include <rtems/rtems/status.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Type for permissions that the libcpu layer can enforce.
 */
typedef uint8_t rtems_mm_permission;

/**
 * rtems_mm_permission bit masks
 */
#define RTEMS_MEMORY_PROTECTION_NO_PERMISSION           0x00
#define RTEMS_MEMORY_PROTECTION_READ_PERMISSION         0x01
#define RTEMS_MEMORY_PROTECTION_WRITE_PERMISSION        0x02
#define RTEMS_MEMORY_PROTECTION_EXECUTE_PERMISSION      0x04
#define RTEMS_MEMORY_PROTECTION_CACHEABLE_PERMISSION    0x08
// TODO: SHAREABLE and GUARDED
#define RTEMS_MEMORY_PROTECTION_ALL_PERMISSION          0x0f

/**
 *  @brief rtems_mm_has_read_permission
 *
 *  Returns non-zero if @a _mpe has read permission bit set.
 */
#define rtems_mm_has_read_permission(_mpe) \
  (_mpe->permissions & RTEMS_MEMORY_PROTECTION_READ_PERMISSION)

/**!
 *  @brief rtems_mm_has_write_permission
 *
 *  Returns non-zero if @a _mpe has write permission bit set.
 */
#define rtems_mm_has_write_permission(_mpe) \
  (_mpe->permissions & RTEMS_MEMORY_PROTECTION_WRITE_PERMISSION)

/**
 *  @brief rtems_mm_has_execute_permission
 *
 *  Returns non-zero if @a _mpe has execute permission bit set.
 */
#define rtems_mm_has_execute_permission(_mpe) \
  (_mpe->permissions & RTEMS_MEMORY_PROTECTION_EXECUTE_PERMISSION)

/**
 *  @brief rtems_mm_has_cacheable_permission
 *
 *  Returns non-zero if @a _mpe has cacheable permission bit set.
 */
#define rtems_mm_has_cacheable_permission(_mpe) \
  (_mpe->permissions & RTEMS_MEMORY_PROTECTION_CACHEABLE_PERMISSION)

/**
 * A region of contiguous memory
 */
typedef struct
{
  char  *name;
  void  *base;
  size_t bounds;
} rtems_mm_region_descriptor;

/**
 * A memory protection entry (MPE) is a region of contiguous memory defined
 * by a rtems_mm_region_descriptor with a set of permissions.
 * If it is currently being enforced then the installed field is true.
 */
typedef struct
{
  rtems_chain_node                          node;
  rtems_mm_region_descriptor region;     /* protected region */
  bool                                      installed;  /* true if protected */
  rtems_mm_permission        permissions;
} rtems_mm_entry;

/**
 * A memory protection domain comprises a chain of active MPEs and 
 * a freelist of idle MPEs. MPE storage is allocated from the Workspace
 * and managed in the MPD.
 */
typedef struct
{
  rtems_mm_entry  *mpe_array;
  size_t                          mpe_array_size;
  rtems_chain_control             active_mpe_chain;
  rtems_chain_control             idle_mpe_chain;
} rtems_mm_domain;

/**
 * @brief rtems_mm_initialize
 *
 * Calls _CPU_Memory_protection_Initialize to initialize the hardware
 * for memory protection. Implemented in libcpu/shared/
 */
rtems_status_code rtems_mm_initialize( void );

/**
 * @brief rtems_mm_verify_permission
 *
 * Calls _CPU_Memory_protection_Verify_permission to verify the hardware
 * supports @a perms. Implemented in libcpu/shared/
 */
rtems_status_code rtems_mm_verify_permission(
  rtems_mm_permission perms
);

/**
 * @brief rtems_mm_verify_size
 *
 * Calls _CPU_Memory_protection_Verify_size to verify the hardware
 * supports @a size regions of memory. Implemented in libcpu/shared/
 */
rtems_status_code rtems_mm_verify_size(
  size_t size
);

/**
 * @brief rtems_mm_install_entry
 *
 * Calls _CPU_Memory_protection_Install_MPE to enforce the permissions
 * on @a mpe. Implemented in libcpu/shared/
 */
rtems_status_code rtems_mm_install_entry(
  rtems_mm_entry *mpe
);

/**
 * @brief rtems_mm_uninstall_entry
 *
 * Calls _CPU_Memory_protection_Uninstall_MPE to remove the @a mpe from
 * current enforcement.  Implemented in libcpu/shared/
 */
rtems_status_code rtems_mm_uninstall_entry(
  rtems_mm_entry *mpe
);

/**
 *  @brief rtems_mm_initialize_domain
 *
 *  Initialize the protection domain @a the_domain. Allocates  
 *  an array of @a max_number_of_mpes memory protection entries
 *  from the workspace.
 */
rtems_status_code rtems_mm_initialize_domain(
  rtems_mm_domain *domain,
  size_t max_number_of_mpes
);

/**
 *  @brief rtems_mm_finalize_domain
 *
 *  Uninstalls and frees the MPEs belonging to @a the_domain.
 */
rtems_status_code rtems_mm_finalize_domain(
  rtems_mm_domain *domain
);

/**
 * @brief rtems_mm_install_domain
 *
 * Install all mpes on the active list of @a the_domain.
 */
rtems_status_code rtems_mm_install_domain(
  rtems_mm_domain* the_domain
);

/**
 * @brief rtems_mm_uninstall_domain
 *
 * Uninstall all (installed) mpes within the protection domain @a the_domain.
 */
rtems_status_code rtems_mm_uninstall_domain(
  rtems_mm_domain* the_domain
);

/**
 * @brief rtems_mm_create_entry
 *
 * Create a new memory protection entry @mpe_ret for the @a region with
 * permissions in @a perms. This function adds the newly created mpe
 * to the active chain of @a the_domain but does not install it. 
 */
rtems_status_code rtems_mm_create_entry(
  rtems_mm_domain* the_domain,
  rtems_mm_region_descriptor * const region,
  const rtems_mm_permission perms,
  rtems_mm_entry** mpe_ret
);

/**
 * @brief rtems_mm_delete_entry
 *
 * Delete memory protection entry @a mpe from the protection
 * domain @a the_domain. This function uninstalls @a mpe (if installed),
 * initializes its base/bounds, and moves it from the active chain to the
 * idle chain.
 */
rtems_status_code rtems_mm_delete_entry(
  rtems_mm_domain* the_domain,
  rtems_mm_entry* const mpe
);

/**
 * @brief rtems_mm_find_entry
 *
 * Find the memory protection entry (@a mpe_ret) in @a the_domain that contains
 * @a the_address within its region.
 */
rtems_status_code rtems_mm_find_entry( 
 rtems_mm_domain* the_domain,
  void* const the_address
  
);

/**
 * @brief rtems_mm_set_permission
 *
 * Set the permissions of @a mpe to @a new_permission. This function does
 * not change whether mpe is installed or not: if the mpe is installed then
 * it will be re-installed with the new permissions (by calling 
 * uninstall then install). If the new_permission is the same as the
 * @a old_perms then nothing is done.
 */
rtems_status_code rtems_mm_set_permission
(
  rtems_mm_entry* const mpe,
  const rtems_mm_permission new_perms,
  rtems_mm_permission*  old_perms
);

rtems_status_code rtems_mm_set_write
(
  rtems_mm_entry* const mpe;
);

rtems_status_code rtems_mm_set_read
(
  rtems_mm_entry* const mpe;
);

rtems_status_code rtems_mm_set_execute
(
  rtems_mm_entry* const mpe;
);


#ifdef __cplusplus
}
#endif

#endif

