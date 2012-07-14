

#ifndef __LIBCPU_PAGETABLE_h
#define __LIBCPU_PAGETABLE_h

#include <sys/types.h>
#include <rtems/rtems/status.h>

#ifdef __cplusplus
  extern "C" {
#endif

rtems_status_code rtems_pagetable_attribute_check( int attr);

int translate_access_attr( uint32_t attr, int * wimg, int * pp);

void rtems_pagetable_initialize( void );

rtems_status_code rtems_pagetable_update_attribute(
  uint32_t ea, 
  int cache_attr, 
  int mprot);

#ifdef __cplusplus
  }
#endif
#endif
