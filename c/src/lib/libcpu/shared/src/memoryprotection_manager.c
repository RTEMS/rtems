
#include <rtems.h>
#include <sys/types.h>
#include <rtems/rtems/status.h>
#include <libcpu/memoryprotection.h>


/*this is a mid level API used to operate page table 
 *mainly used by memory protect manager
 */

/*****************************************************
 *  * * initialization of the ALUT (Access Look up Table )  *
 *   * *****************************************************/
rtems_status_code rtems_memory_management_initialize(void)
{
  return  _CPU_Memory_management_Initialize();
}

/**
 *  * @brief Install the memory protection entry to the enforcement mechanism.
 *   */
rtems_status_code rtems_memory_management_install_entry(
  rtems_memory_management_entry *mpe
) {
  rtems_status_code status;
  ISR_Level         level;
  _ISR_Disable( level );
    status = _CPU_Memory_management_Install_MPE(mpe);
    if ( status == RTEMS_SUCCESSFUL ) {
      mpe->installed = true;
    }
  _ISR_Enable( level );
  return status;
}


rtems_status_code rtems_memory_management_verify_size(
    size_t size
) {
  rtems_status_code status;
  ISR_Level         level;
  _ISR_Disable( level );
    status = _CPU_Memory_management_Verify_size(size);
  _ISR_Enable( level );
  return status;
}

rtems_status_code rtems_memory_management_set_write
(
  rtems_memory_management_entry* const mpe
){
  rtems_status_code status;
  ISR_Level         level;
  _ISR_Disable( level );
    status = _CPU_Memory_management_Set_write(mpe);
  _ISR_Enable( level );
  return status;

}

rtems_status_code rtems_memory_management_set_read_only
(
  rtems_memory_management_entry* const mpe
){
  rtems_status_code status;
  ISR_Level         level;
  _ISR_Disable( level );
    status = _CPU_Memory_management_Set_read_only(mpe);
  _ISR_Enable( level );
  return status;
}
