#include "rtl-unwind-arm.h"

bool
rtems_rtl_elf_unwind_parse (const rtems_rtl_obj* obj,
                            const char*          name,
                            uint32_t             flags)
{
  /*
   * We location the EH sections in section flags.
   */
  return false;
}

bool
rtems_rtl_elf_unwind_register (rtems_rtl_obj* obj)
{
  return true;
}

bool
rtems_rtl_elf_unwind_deregister (rtems_rtl_obj* obj)
{
  obj->loader = NULL;
  return true;
}

/*
 * A weak reference is in libgcc, provide a real version and provide a way to
 * manage loaded modules.
 *
 * Passed in the return address and a reference to the number of records
 * found. We set the start of the exidx data and the number of records.
 */
_Unwind_Ptr __gnu_Unwind_Find_exidx (_Unwind_Ptr return_address,
                                     int*        nrec) __attribute__ ((__noinline__,
                                                                       __used__,
                                                                       __noclone__));

_Unwind_Ptr __gnu_Unwind_Find_exidx (_Unwind_Ptr return_address,
                                     int*        nrec)
{
  rtems_rtl_data*   rtl;
  rtems_chain_node* node;
  __EIT_entry*      exidx_start = &__exidx_start;
  __EIT_entry*      exidx_end = &__exidx_end;

  rtl = rtems_rtl_lock ();

  node = rtems_chain_first (&rtl->objects);
  while (!rtems_chain_is_tail (&rtl->objects, node)) {
    rtems_rtl_obj* obj = (rtems_rtl_obj*) node;
    if (rtems_rtl_obj_text_inside (obj, (void*) return_address)) {
      exidx_start = (__EIT_entry*) obj->eh_base;
      exidx_end = (__EIT_entry*) (obj->eh_base + obj->eh_size);
      break;
    }
    node = rtems_chain_next (node);
  }

  rtems_rtl_unlock ();

  *nrec = exidx_end - exidx_start;

  return (_Unwind_Ptr) exidx_start;
}
