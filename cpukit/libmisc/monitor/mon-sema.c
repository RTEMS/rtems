/*
 * RTEMS Monitor semaphore support
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include "monitor.h"
#include <rtems/rtems/semimpl.h>
#include <stdio.h>
#include <string.h>    /* memcpy() */

void
rtems_monitor_sema_canonical(
    rtems_monitor_sema_t  *canonical_sema,
    const void            *sema_void
)
{
    const Semaphore_Control *rtems_sema = (const Semaphore_Control *) sema_void;
    Thread_Control *owner;

    canonical_sema->attribute = 0;
    canonical_sema->priority_ceiling = 0;
    canonical_sema->max_count = 0;
    canonical_sema->cur_count = 0;
    canonical_sema->holder_id = 0;

#if defined(RTEMS_MULTIPROCESSING)
    if (rtems_sema->is_global) {
      canonical_sema->attribute |= RTEMS_GLOBAL;
    }
#endif

    if (rtems_sema->discipline == SEMAPHORE_DISCIPLINE_PRIORITY) {
      canonical_sema->attribute |= RTEMS_PRIORITY;
    }

    switch ( rtems_sema->variant ) {
      case SEMAPHORE_VARIANT_MUTEX_INHERIT_PRIORITY:
        canonical_sema->attribute |= RTEMS_BINARY_SEMAPHORE
          | RTEMS_INHERIT_PRIORITY;
        break;
      case SEMAPHORE_VARIANT_MUTEX_PRIORITY_CEILING:
        canonical_sema->attribute |= RTEMS_BINARY_SEMAPHORE
          | RTEMS_PRIORITY_CEILING;
        break;
      case SEMAPHORE_VARIANT_MUTEX_NO_PROTOCOL:
        canonical_sema->attribute |= RTEMS_BINARY_SEMAPHORE;
        break;
#if defined(RTEMS_SMP)
      case SEMAPHORE_VARIANT_MRSP:
        canonical_sema->attribute |= RTEMS_BINARY_SEMAPHORE
          | RTEMS_MULTIPROCESSOR_RESOURCE_SHARING;
        break;
#endif
      case SEMAPHORE_VARIANT_SIMPLE_BINARY:
        canonical_sema->attribute |= RTEMS_SIMPLE_BINARY_SEMAPHORE;
        break;
      case SEMAPHORE_VARIANT_COUNTING:
        canonical_sema->attribute |= RTEMS_COUNTING_SEMAPHORE;
        break;
    }

    switch ( rtems_sema->variant ) {
      case SEMAPHORE_VARIANT_MUTEX_PRIORITY_CEILING:
        canonical_sema->priority_ceiling = _Scheduler_Unmap_priority(
          _CORE_ceiling_mutex_Get_scheduler( &rtems_sema->Core_control.Mutex ),
          _CORE_ceiling_mutex_Get_priority( &rtems_sema->Core_control.Mutex )
        );
        /* Fall through */
      case SEMAPHORE_VARIANT_MUTEX_INHERIT_PRIORITY:
      case SEMAPHORE_VARIANT_MUTEX_NO_PROTOCOL:
        owner = _CORE_mutex_Get_owner(
          &rtems_sema->Core_control.Mutex.Recursive.Mutex
        );

        if (owner != NULL) {
          canonical_sema->holder_id = owner->Object.id;
          canonical_sema->cur_count = 0;
        } else {
          canonical_sema->cur_count = 1;
        }

        canonical_sema->max_count = 1;
        break;
#if defined(RTEMS_SMP)
      case SEMAPHORE_VARIANT_MRSP:
        canonical_sema->cur_count =
          _MRSP_Get_owner( &rtems_sema->Core_control.MRSP ) == NULL;
        canonical_sema->max_count = 1;
        break;
#endif
      case SEMAPHORE_VARIANT_SIMPLE_BINARY:
        canonical_sema->cur_count = rtems_sema->Core_control.Semaphore.count;
        canonical_sema->max_count = 1;
        break;
      case SEMAPHORE_VARIANT_COUNTING:
        canonical_sema->cur_count = rtems_sema->Core_control.Semaphore.count;
        canonical_sema->max_count = UINT32_MAX;
        break;
    }
}


void
rtems_monitor_sema_dump_header(
    bool verbose RTEMS_UNUSED
)
{
    printf("\
  ID       NAME   ATTR        PRICEIL CURR_CNT HOLDID \n");
/*23456789 123456789 123456789 123456789 123456789 123456789 123456789 1234
          1         2         3         4         5         6         7    */

    rtems_monitor_separator();
}

/*
 */

void
rtems_monitor_sema_dump(
    rtems_monitor_sema_t *monitor_sema,
    bool  verbose RTEMS_UNUSED
)
{
    int length = 0;

    length += rtems_monitor_dump_id(monitor_sema->id);
    length += rtems_monitor_pad(11, length);
    length += rtems_monitor_dump_name(monitor_sema->id);
    length += rtems_monitor_pad(18, length);
    length += rtems_monitor_dump_attributes(monitor_sema->attribute);
    length += rtems_monitor_pad(30, length);
    length += rtems_monitor_dump_priority(monitor_sema->priority_ceiling);
    length += rtems_monitor_pad(38, length);
    length += rtems_monitor_dump_decimal(monitor_sema->cur_count);
    length += rtems_monitor_pad(47, length);
    length += rtems_monitor_dump_id(monitor_sema->holder_id);
    printf("\n");
}
