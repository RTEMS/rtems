/*
 * Stubs for memory protection
 *
 * Copyright (C) 2011. Gedare Bloom.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#include <rtems.h>
#include <libcpu/memoryprotection.h>

/*
 * The following functions should be implemented for each CPU if it has 
 * some way to enforce memory protection.  These provide the basic
 * implementation for the rtems_mm routines. 
 * 
 * This is just a series of stubs.
 */

rtems_status_code _CPU_Memory_protection_Initialize( void ) {
  return RTEMS_SUCCESSFUL;
}

rtems_status_code _CPU_Memory_protection_Verify_attribute(
    uint32_t attribute
) {
  return RTEMS_SUCCESSFUL;
}

rtems_status_code _CPU_Memory_protection_Verify_size(
    size_t size
) {
  return RTEMS_SUCCESSFUL;
}

rtems_status_code _CPU_Memory_protection_Install_MPE(
    rtems_mm_entry *mpe
) {
  return RTEMS_SUCCESSFUL;
}

rtems_status_code _CPU_Memory_protection_Uninstall_MPE(
    rtems_mm_entry *mpe
) {
  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_mm_initialize ( void) {
  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_mm_install_entry(
  rtems_mm_entry *mpe
) {
  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_mm_uninstall_entry(
  rtems_mm_entry *mpe
) {
  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_mm_verify_size(
  size_t size
) {
  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_mm_verify_permission(
  rtems_mm_permission perms
) {
  return RTEMS_SUCCESSFUL;
}

