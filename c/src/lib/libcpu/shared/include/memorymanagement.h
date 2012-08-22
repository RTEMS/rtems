/* COPYRIGHT (c) 1989-2009.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef __LIBCPU_PAGETABLE_h
#define __LIBCPU_PAGETABLE_h

#include <sys/types.h>
#include <rtems/rtems/status.h>
#include <rtems/libmm.h>
#ifdef __cplusplus
  extern "C" {
#endif

/*
 * Initialize the hardware to prepare for memory protection directives.
 */
rtems_status_code _CPU_Memory_management_Initialize(void);

/*
 * Install (enforce) the memory protection entry @a mpe
 */
rtems_status_code _CPU_Memory_management_Install_MPE(
    rtems_memory_management_entry *mpe
);

/*
 * Check if memory protection region @a size is valid for this CPU
 */
rtems_status_code _CPU_Memory_management_Verify_size(
    size_t size
);

rtems_status_code _CPU_Memory_management_Set_read_only(
    rtems_memory_management_entry *mpe
);

rtems_status_code _CPU_Memory_management_Set_write(
    rtems_memory_management_entry *mpe
);

#ifdef __cplusplus
}
#endif
#endif
