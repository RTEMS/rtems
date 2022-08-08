#include <unwind.h>

#include <rtems/rtl/rtl.h>
#include "rtl-unwind.h"

typedef unsigned _Unwind_Word __attribute__((__mode__(__word__)));
typedef _Unwind_Word _uw;

bool
rtems_rtl_elf_unwind_parse (const rtems_rtl_obj* obj,
                            const char*          name,
                            uint32_t             flags);

bool
rtems_rtl_elf_unwind_register (rtems_rtl_obj* obj);

bool
rtems_rtl_elf_unwind_deregister (rtems_rtl_obj* obj);

/* An exception index table entry.  */
typedef struct __EIT_entry
{
  _uw fnoffset;
  _uw content;
} __EIT_entry;

/* The exception index table location in the base module */
extern __EIT_entry __exidx_start;
extern __EIT_entry __exidx_end;

/*
 * A weak reference is in libgcc, provide a real version and provide a way to
 * manage loaded modules.
 *
 * Passed in the return address and a reference to the number of records
 * found. We set the start of the exidx data and the number of records.
 */
_Unwind_Ptr
__gnu_Unwind_Find_exidx (_Unwind_Ptr return_address,
                         int*        nrec) __attribute__ ((__noinline__,
                                                           __used__,
                                                           __noclone__));

_Unwind_Ptr
__gnu_Unwind_Find_exidx (_Unwind_Ptr return_address,
                         int*        nrec);
