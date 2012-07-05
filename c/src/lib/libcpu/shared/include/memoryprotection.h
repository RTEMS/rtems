/*
 * libcpu Memory Protection Support
 *
 * Copyright (C) 2011. Gedare Bloom.
 *
 * The functions declared in this file can be implemented for
 * each processor in the memoryprotection.c file under libcpu/CPU/ 
 * or with the stub implementation under libcpu/shared
 *
 * These functions provide the processor specific implementations for
 * most of the RTEMS Memory Protection directives,
 * and should only ever be called by these directives.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#ifndef __LIBCPU_PAGETABLE_h
#define __LIBCPU_PAGETABLE_h

#include <rtems/libmmu.h>
#include <rtems/rtems/status.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TODO: documentation*/
/*
 * Initialize the hardware to prepare for memory protection directives.
 */
rtems_status_code _CPU_Memory_protection_Initialize( void );

/*
 * Make sure memory protection @a permission is valid for this CPU
 */
rtems_status_code _CPU_Memory_protection_Verify_permission(
    rtems_mm_permission permission
);

/*
 * Check if memory protection region @a size is valid for this CPU
 */
rtems_status_code _CPU_Memory_protection_Verify_size(
    size_t size
);

/*
 * Install (enforce) the memory protection entry @a mpe
 */
rtems_status_code _CPU_Memory_protection_Install_MPE(
    rtems_mm_entry *mpe
);

/*
 * Uninstall the memory protection entry @a mpe
 */
rtems_status_code _CPU_Memory_protection_Uninstall_MPE(
    rtems_mm_entry *mpe
);

rtems_status_code _CPU_Memory_protection_Set_write(
    rtems_mm_entry *mpe
);

rtems_status_code _CPU_Memory_protection_Set_read(
    rtems_mm_entry *mpe
);

rtems_status_code _CPU_Memory_protection_Set_execute(
    rtems_mm_entry *mpe
);
#ifdef __cplusplus
  }
#endif
#endif

